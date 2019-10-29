/*****************************************************************************
 *
 * tpoint_spgist.c
 *	  SP-GiST implementation of 10-dimensional oct-tree over temporal points
 *
 * This module provides SP-GiST implementation for boxes using oct tree
 * analogy in 10-dimensional space.  SP-GiST doesn't allow indexing of
 * overlapping objects.  We are making 5D objects never-overlapping in
 * 10D space.  This technique has some benefits compared to traditional
 * R-Tree which is implemented as GiST.  The performance tests reveal
 * that this technique especially beneficial with too much overlapping
 * objects, so called "spaghetti data".
 *
 * Unlike the original oct-tree, we are splitting the tree into 256
 * octants in 10D space.  It is easier to imagine it as splitting space
 * four times into four:
 *
 *				|	   |						|	   |		
 *				|	   |						|	   |		
 *				| -----+-----					| -----+-----
 *				|	   |						|	   |		
 *				|	   |						|	   |		
 * -------------+------------- -+- -------------+-------------
 *				|								|
 *				|								|
 *				|								|
 *				|								|
 *				|								|
 *			  FRONT							  BACK
 *
 * We are using STBOX data type as the prefix, but we are treating them
 * as points in 8-dimensional space, because 5D boxes are not enough
 * to represent the octant boundaries in 10D space.  They however are
 * sufficient to point out the additional boundaries of the next
 * octant.
 *
 * We are using traversal values provided by SP-GiST to calculate and
 * to store the bounds of the octants, while traversing into the tree.
 * Traversal value has all the boundaries in the 10D space, and is is
 * capable of transferring the required boundaries to the following
 * traversal values.  In conclusion, three things are necessary
 * to calculate the next traversal value:
 *
 *	(1) the traversal value of the parent
 *	(2) the octant of the current node
 *	(3) the prefix of the current node
 *
 * If we visualize them on our simplified drawing (see the drawing above);
 * transferred boundaries of (1) would be the outer axis, relevant part
 * of (2) would be the up range_y part of the other axis, and (3) would be
 * the inner axis.
 *
 * For example, consider the case of overlapping.  When recursion
 * descends deeper and deeper down the tree, all octants in
 * the current node will be checked for overlapping.  The boundaries
 * will be re-calculated for all octants.  Overlap check answers
 * the question: can any box from this octant overlap with the given
 * box?  If yes, then this octant will be walked.  If no, then this
 * octant will be skipped.
 *
 * This method provides restrictions for minimum and maximum values of
 * every dimension of every corner of the box on every level of the tree
 * except the root.  For the root node, we are setting the boundaries
 * that we don't yet have as infinity.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2016, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tpoint_spgist.h"

#include <access/spgist.h>
#include <utils/timestamp.h>
#include <utils/builtins.h>

#include "temporaltypes.h"
#include "oidcache.h"
#include "tpoint.h"
#include "tpoint_boxops.h"
#include "tpoint_gist.h"

/*****************************************************************************/

typedef struct
{
	STBOX	left;
	STBOX	right;
} CubeSTbox;

/*
 * Comparator for qsort
 *
 * We don't need to use the floating point macros in here, because this is
 * only going to be used in a place to affect the performance of the index,
 * not the correctness.
 */
static int
compareDoubles(const void *a, const void *b)
{
	double		x = *(double *) a;
	double		y = *(double *) b;

	if (x == y)
		return 0;
	return (x > y) ? 1 : -1;
}

/*
 * Calculate the octant
 *
 * The octant is 10 bit unsigned integer with 10 least bits in use.
 * This function accepts 2 STBOX as input.  All 10 bits are set by comparing a 
 * corner of the box. This makes 1024 octants in total.
 */
static uint16
getOctant10D(STBOX *centroid, STBOX *inBox)
{
	uint16 octant = 0;

	if (inBox->xmin > centroid->xmin)
		octant |= 0x0200;

	if (inBox->xmax > centroid->xmax)
		octant |= 0x0100;

	if (inBox->ymin > centroid->ymin)
		octant |= 0x0080;

	if (inBox->ymax > centroid->ymax)
		octant |= 0x0040;
	
	if (inBox->zmin > centroid->zmin)
		octant |= 0x0020;
	
	if (inBox->zmax > centroid->zmax)
		octant |= 0x0010;

	if (inBox->mmin > centroid->mmin)
		octant |= 0x0008;

	if (inBox->mmax > centroid->mmax)
		octant |= 0x0004;

	if (inBox->tmin > centroid->tmin)
		octant |= 0x0002;

	if (inBox->tmax > centroid->tmax)
		octant |= 0x0001;

	return octant;
}

/*
 * Initialize the traversal value
 *
 * In the beginning, we don't have any restrictions.  We have to
 * initialize the struct to cover the whole 10D space.
 */
static CubeSTbox *
initCubeSTbox(void)
{
	CubeSTbox *cube_stbox = (CubeSTbox *) palloc(sizeof(CubeSTbox));
	double infinity = get_float8_infinity();

	cube_stbox->left.xmin = cube_stbox->right.xmin = -infinity;
	cube_stbox->left.xmax = cube_stbox->right.xmax = infinity;

	cube_stbox->left.ymin = cube_stbox->right.ymin = -infinity;
	cube_stbox->left.ymax = cube_stbox->right.ymax = infinity;

	cube_stbox->left.zmin = cube_stbox->right.zmin = -infinity;
	cube_stbox->left.zmax = cube_stbox->right.zmax = infinity;

	cube_stbox->left.mmin = cube_stbox->right.mmin = -infinity;
	cube_stbox->left.mmax = cube_stbox->right.mmax = infinity;

	cube_stbox->left.tmin = cube_stbox->right.tmin = DT_NOBEGIN;
	cube_stbox->left.tmax = cube_stbox->right.tmax = DT_NOEND;

	return cube_stbox;
}

/*
 * Calculate the next traversal value
 *
 * All centroids are bounded by CubeSTbox, but SP-GiST only keeps
 * boxes.  When we are traversing the tree, we must calculate CubeSTbox,
 * using centroid and octant.
 */
static CubeSTbox *
nextCubeSTbox(CubeSTbox *cube_stbox, STBOX *centroid, uint16 octant)
{
	CubeSTbox *next_cube_stbox = (CubeSTbox *) palloc(sizeof(CubeSTbox));

	memcpy(next_cube_stbox, cube_stbox, sizeof(CubeSTbox));

	if (octant & 0x0200)
		next_cube_stbox->left.xmin = centroid->xmin;
	else
		next_cube_stbox->left.xmax = centroid->xmin;

	if (octant & 0x0100)
		next_cube_stbox->right.xmin = centroid->xmax;
	else
		next_cube_stbox->right.xmax = centroid->xmax;

	if (octant & 0x0080)
		next_cube_stbox->left.ymin = centroid->ymin;
	else
		next_cube_stbox->left.ymax = centroid->ymin;

	if (octant & 0x0040)
		next_cube_stbox->right.ymin = centroid->ymax;
	else
		next_cube_stbox->right.ymax = centroid->ymax;

	if (octant & 0x0020)
		next_cube_stbox->left.zmin = centroid->zmin;
	else
		next_cube_stbox->left.zmax = centroid->zmin;

	if (octant & 0x0010)
		next_cube_stbox->right.zmin = centroid->zmax;
	else
		next_cube_stbox->right.zmax = centroid->zmax;

	if (octant & 0x0008)
		next_cube_stbox->left.mmin = centroid->mmin;
	else
		next_cube_stbox->left.mmax = centroid->mmin;

	if (octant & 0x0004)
		next_cube_stbox->right.mmin = centroid->mmax;
	else
		next_cube_stbox->right.mmax = centroid->mmax;

	if (octant & 0x0002)
		next_cube_stbox->left.tmin = centroid->tmin;
	else
		next_cube_stbox->left.tmax = centroid->tmin;

	if (octant & 0x0001)
		next_cube_stbox->right.tmin = centroid->tmax;
	else
		next_cube_stbox->right.tmax = centroid->tmax;

	return next_cube_stbox;
}

/* Can any cube from cube_stbox overlap with query? */
static bool
overlap10D(CubeSTbox *cube_stbox, STBOX *query)
{
	bool result = true;
	/* Result value is computed only for the dimensions of the query */
	if (MOBDB_FLAGS_GET_X(query->flags))
		result &= cube_stbox->left.xmin <= query->xmax &&
			cube_stbox->right.xmax >= query->xmin &&
			cube_stbox->left.ymin <= query->ymax &&
			cube_stbox->right.ymax >= query->ymin;
	if (MOBDB_FLAGS_GET_Z(query->flags))
		result &= cube_stbox->left.zmin <= query->zmax &&
			cube_stbox->right.zmax >= query->zmin;
	if (MOBDB_FLAGS_GET_M(query->flags))
		result &= cube_stbox->left.mmin <= query->mmax &&
			cube_stbox->right.mmax >= query->mmin;
	if (MOBDB_FLAGS_GET_T(query->flags))
		result &= cube_stbox->left.tmin <= query->tmax &&
			cube_stbox->right.tmax >= query->tmin;
	return result;
}

/* Can any cube from cube_stbox contain query? */
static bool
contain10D(CubeSTbox *cube_stbox, STBOX *query)
{
	bool result = true;
	/* Result value is computed only for the dimensions of the query */
	if (MOBDB_FLAGS_GET_X(query->flags))
		result &= cube_stbox->right.xmax >= query->xmax &&
			cube_stbox->left.xmin <= query->xmin &&
			cube_stbox->right.ymax >= query->ymax &&
			cube_stbox->left.ymin <= query->ymin;
	if (MOBDB_FLAGS_GET_Z(query->flags))
		result &= cube_stbox->right.zmax >= query->zmax &&
			cube_stbox->left.zmin <= query->zmin;
	if (MOBDB_FLAGS_GET_M(query->flags))
		result &= cube_stbox->right.mmax >= query->mmax &&
			cube_stbox->left.mmin <= query->mmin;
	if (MOBDB_FLAGS_GET_T(query->flags))
		result &= cube_stbox->right.tmax >= query->tmax &&
			cube_stbox->left.tmin <= query->tmin;
	return result;
}

/* Can any cube from cube_stbox be left of query? */
static bool
left10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.xmax < query->xmin);
}

/* Can any cube from cube_stbox does not extend the right of query? */
static bool
overLeft10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.xmax <= query->xmax);
}

/* Can any cube from cube_stbox be right of query? */
static bool
right10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.xmin > query->xmax);
}

/* Can any cube from cube_stbox does not extend the left of query? */
static bool
overRight10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.xmin >= query->xmin);
}

/* Can any cube from cube_stbox be below of query? */
static bool
below10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.ymax < query->ymin);
}

/* Can any cube from cube_stbox does not extend above query? */
static bool
overBelow10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.ymax <= query->ymax);
}

/* Can any cube from cube_stbox be above of query? */
static bool
above10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.ymin > query->ymax);
}

/* Can any cube from cube_stbox does not extend below of query? */
static bool
overAbove10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.ymin >= query->ymin);
}

/* Can any cube from cube_stbox be in front of query? */
static bool
front10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.zmax < query->zmin);
}

/* Can any cube from cube_stbox does not extend the back of query? */
static bool
overFront10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.zmax <= query->zmax);
}

/* Can any cube from cube_stbox be back to query? */
static bool
back10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.zmin > query->zmax);
}

/* Can any cube from cube_stbox does not extend the front of query? */
static bool
overBack10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.zmin >= query->zmin);
}

/* Can any cube from cube_stbox be before of query? */
static bool
before10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.tmax < query->tmin);
}

/* Can any cube from cube_stbox does not extend the after of query? */
static bool
overBefore10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->right.tmax <= query->tmax);
}

/* Can any cube from cube_stbox be after of query? */
static bool
after10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.tmin > query->tmax);
}

/* Can any cube from cube_stbox does not extend the before of query? */
static bool
overAfter10D(CubeSTbox *cube_stbox, STBOX *query)
{
	return (cube_stbox->left.tmin >= query->tmin);
}

/*****************************************************************************
 * SP-GiST config functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tpoint_config);

PGDLLEXPORT Datum
spgist_tpoint_config(PG_FUNCTION_ARGS)
{
	spgConfigOut *cfg = (spgConfigOut *) PG_GETARG_POINTER(1);

	Oid stbox_oid = type_oid(T_STBOX);
	cfg->prefixType = stbox_oid;	/* A type represented by its bounding box */
	cfg->labelType = VOIDOID;	/* We don't need node labels. */
	cfg->leafType = stbox_oid;
	cfg->canReturnData = false;
	cfg->longValuesOK = false;

	PG_RETURN_VOID();
}

/*****************************************************************************
 * SP-GiST choose functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tpoint_choose);

PGDLLEXPORT Datum
spgist_tpoint_choose(PG_FUNCTION_ARGS)
{
	spgChooseIn *in = (spgChooseIn *) PG_GETARG_POINTER(0);
	spgChooseOut *out = (spgChooseOut *) PG_GETARG_POINTER(1);
	STBOX *centroid = DatumGetSTboxP(in->prefixDatum),
		*box = DatumGetSTboxP(in->leafDatum);

	out->resultType = spgMatchNode;
	out->result.matchNode.restDatum = PointerGetDatum(box);

	/* nodeN will be set by core, when allTheSame. */
	if (!in->allTheSame)
		out->result.matchNode.nodeN = getOctant10D(centroid, box);

	PG_RETURN_VOID();
}

/*****************************************************************************
 * SP-GiST pick-split function
 *
 * It splits a list of boxes into octants by choosing a central 10D
 * point as the median of the coordinates of the boxes.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tpoint_picksplit);

PGDLLEXPORT Datum
spgist_tpoint_picksplit(PG_FUNCTION_ARGS)
{
	spgPickSplitIn *in = (spgPickSplitIn *) PG_GETARG_POINTER(0);
	spgPickSplitOut *out = (spgPickSplitOut *) PG_GETARG_POINTER(1);
	STBOX *centroid;
	int	median, i;
	double *lowXs = palloc(sizeof(double) * in->nTuples);
	double *highXs = palloc(sizeof(double) * in->nTuples);
	double *lowYs = palloc(sizeof(double) * in->nTuples);
	double *highYs = palloc(sizeof(double) * in->nTuples);
	double *lowZs = palloc(sizeof(double) * in->nTuples);
	double *highZs = palloc(sizeof(double) * in->nTuples);
	double *lowMs = palloc(sizeof(double) * in->nTuples);
	double *highMs = palloc(sizeof(double) * in->nTuples);
	double *lowTs = palloc(sizeof(double) * in->nTuples);
	double *highTs = palloc(sizeof(double) * in->nTuples);
	
	/* Calculate median of all 10D coordinates */
	for (i = 0; i < in->nTuples; i++)
	{
		STBOX *box = DatumGetSTboxP(in->datums[i]);

		lowXs[i] = box->xmin;
		highXs[i] = box->xmax;
		lowYs[i] = box->ymin;
		highYs[i] = box->ymax;
		lowZs[i] = box->zmin;
		highZs[i] = box->zmax;
		lowMs[i] = box->mmin;
		highMs[i] = box->mmax;
		lowTs[i] = (double) box->tmin;
		highTs[i] = (double) box->tmax;
	}

	qsort(lowXs, in->nTuples, sizeof(double), compareDoubles);
	qsort(highXs, in->nTuples, sizeof(double), compareDoubles);
	qsort(lowYs, in->nTuples, sizeof(double), compareDoubles);
	qsort(highYs, in->nTuples, sizeof(double), compareDoubles);
	qsort(lowZs, in->nTuples, sizeof(double), compareDoubles);
	qsort(highZs, in->nTuples, sizeof(double), compareDoubles);
	qsort(lowMs, in->nTuples, sizeof(double), compareDoubles);
	qsort(highMs, in->nTuples, sizeof(double), compareDoubles);
	qsort(lowTs, in->nTuples, sizeof(double), compareDoubles);
	qsort(highTs, in->nTuples, sizeof(double), compareDoubles);

	median = in->nTuples / 2;

	centroid = palloc0(sizeof(STBOX));

	centroid->xmin = lowXs[median];
	centroid->xmax = highXs[median];
	centroid->ymin = lowYs[median];
	centroid->ymax = highYs[median];
	centroid->zmin = lowZs[median];
	centroid->zmax = highZs[median];
	centroid->mmin = lowMs[median];
	centroid->mmax = highMs[median];
	centroid->tmin = (TimestampTz) lowTs[median];
	centroid->tmax = (TimestampTz) highTs[median];

	/* Fill the output */
	out->hasPrefix = true;
	out->prefixDatum = STboxPGetDatum(centroid);

	out->nNodes = 256;
	out->nodeLabels = NULL;		/* We don't need node labels. */

	out->mapTuplesToNodes = palloc(sizeof(int) * in->nTuples);
	out->leafTupleDatums = palloc(sizeof(Datum) * in->nTuples);

	/*
	 * Assign ranges to corresponding nodes according to octants relative to
	 * the "centroid" range
	 */
	for (i = 0; i < in->nTuples; i++)
	{
		STBOX *box = DatumGetSTboxP(in->datums[i]);
		uint16 octant = getOctant10D(centroid, box);
		out->leafTupleDatums[i] = STboxPGetDatum(box);
		out->mapTuplesToNodes[i] = octant;
	}

	pfree(lowXs); pfree(highXs);
	pfree(lowYs); pfree(highYs);
	pfree(lowZs); pfree(highZs);
	pfree(lowMs); pfree(highMs);
	pfree(lowTs); pfree(highTs);
	
	PG_RETURN_VOID();
}

/*****************************************************************************
 * SP-GiST inner consistent functions for temporal points
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tpoint_inner_consistent);

PGDLLEXPORT Datum
spgist_tpoint_inner_consistent(PG_FUNCTION_ARGS)
{
	spgInnerConsistentIn *in = (spgInnerConsistentIn *) PG_GETARG_POINTER(0);
	spgInnerConsistentOut *out = (spgInnerConsistentOut *) PG_GETARG_POINTER(1);
	int	i;
	MemoryContext old_ctx;
	CubeSTbox *cube_stbox;
	uint16 octant;
	STBOX *centroid = DatumGetSTboxP(in->prefixDatum), *queries;

	if (in->allTheSame)
	{
		/* Report that all nodes should be visited */
		out->nNodes = in->nNodes;
		out->nodeNumbers = (int *) palloc(sizeof(int) * in->nNodes);
		for (i = 0; i < in->nNodes; i++)
			out->nodeNumbers[i] = i;

		PG_RETURN_VOID();
	}
	
	/*
	 * We are saving the traversal value or initialize it an unbounded one, if
	 * we have just begun to walk the tree.
	 */
	if (in->traversalValue)
		cube_stbox = in->traversalValue;
	else
		cube_stbox = initCubeSTbox();

	/*
	 * Transform the queries into bounding boxes initializing the dimensions
	 * that must not be taken into account for the operators to infinity.
	 * This transformation is done here to avoid doing it for all octants
	 * in the loop below.
	 */
	queries = (STBOX *) palloc0(sizeof(STBOX) * in->nkeys);
	for (i = 0; i < in->nkeys; i++)
	{
		StrategyNumber strategy = in->scankeys[i].sk_strategy;
		Oid subtype = in->scankeys[i].sk_subtype;
		
		if (subtype == type_oid(T_GEOMETRY) || subtype == type_oid(T_GEOGRAPHY))
			/* We do not test the return value of the next function since
			   if the result is false all dimensions of the box have been 
			   initialized to +-infinity */
			geo_to_stbox_internal(&queries[i], 
				(GSERIALIZED*)PG_DETOAST_DATUM(in->scankeys[i].sk_argument));
		else if (subtype == type_oid(T_STBOX))
			memcpy(&queries[i], DatumGetSTboxP(in->scankeys[i].sk_argument), sizeof(STBOX));
		else if (temporal_type_oid(subtype))
			temporal_bbox(&queries[i],
				DatumGetTemporal(in->scankeys[i].sk_argument));
		else
			elog(ERROR, "Unrecognized strategy number: %d", strategy);
	}

	/* Allocate enough memory for nodes */
	out->nNodes = 0;
	out->nodeNumbers = (int *) palloc(sizeof(int) * in->nNodes);
	out->traversalValues = (void **) palloc(sizeof(void *) * in->nNodes);

	/*
	 * We switch memory context, because we want to allocate memory for new
	 * traversal values (next_cube_stbox) and pass these pieces of memory to
	 * further call of this function.
	 */
	old_ctx = MemoryContextSwitchTo(in->traversalMemoryContext);

	for (octant = 0; octant < in->nNodes; octant++)
	{
		CubeSTbox *next_cube_stbox = nextCubeSTbox(cube_stbox, centroid, octant);
		bool flag = true;
		for (i = 0; i < in->nkeys; i++)
		{
			StrategyNumber strategy = in->scankeys[i].sk_strategy;
			switch (strategy)
			{
				case RTOverlapStrategyNumber:
				case RTContainedByStrategyNumber:
					flag = overlap10D(next_cube_stbox, &queries[i]);
					break;
				case RTContainsStrategyNumber:
				case RTSameStrategyNumber:
					flag = contain10D(next_cube_stbox, &queries[i]);
					break;
				case RTLeftStrategyNumber:
					flag = !overRight10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverLeftStrategyNumber:
					flag = !right10D(next_cube_stbox, &queries[i]);
					break;
				case RTRightStrategyNumber:
					flag = !overLeft10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverRightStrategyNumber:
					flag = !left10D(next_cube_stbox, &queries[i]);
					break;
				case RTFrontStrategyNumber:
					flag = !overBack10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverFrontStrategyNumber:
					flag = !back10D(next_cube_stbox, &queries[i]);
					break;
				case RTBackStrategyNumber:
					flag = !overFront10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverBackStrategyNumber:
					flag = !front10D(next_cube_stbox, &queries[i]);
					break;
				case RTAboveStrategyNumber:
					flag = !overBelow10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverAboveStrategyNumber:
					flag = !below10D(next_cube_stbox, &queries[i]);
					break;
				case RTBelowStrategyNumber:
					flag = !overAbove10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverBelowStrategyNumber:
					flag = !above10D(next_cube_stbox, &queries[i]);
					break;
				case RTAfterStrategyNumber:
					flag = !overBefore10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverAfterStrategyNumber:
					flag = !before10D(next_cube_stbox, &queries[i]);
					break;
				case RTBeforeStrategyNumber:
					flag = !overAfter10D(next_cube_stbox, &queries[i]);
					break;
				case RTOverBeforeStrategyNumber:
					flag = !after10D(next_cube_stbox, &queries[i]);
					break;
				default:
					elog(ERROR, "unrecognized strategy: %d", strategy);
			}

			/* If any check is failed, we have found our answer. */
			if (!flag)
				break;
		}

		if (flag)
		{
			out->traversalValues[out->nNodes] = next_cube_stbox;
			out->nodeNumbers[out->nNodes] = octant;
			out->nNodes++;
		}
		else
		{
			/*
			 * If this node is not selected, we don't need to keep the next
			 * traversal value in the memory context.
			 */
			pfree(next_cube_stbox);
		}
	}

	/* Switch after */
	MemoryContextSwitchTo(old_ctx);

	pfree(queries);
	
	PG_RETURN_VOID();
}

/*****************************************************************************
 * SP-GiST leaf-level consistency function
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tpoint_leaf_consistent);

PGDLLEXPORT Datum
spgist_tpoint_leaf_consistent(PG_FUNCTION_ARGS)
{
	spgLeafConsistentIn *in = (spgLeafConsistentIn *) PG_GETARG_POINTER(0);
	spgLeafConsistentOut *out = (spgLeafConsistentOut *) PG_GETARG_POINTER(1);
	STBOX *key = DatumGetSTboxP(in->leafDatum);
	bool res = true;
	int i;

	/* Initialize the value to do not recheck, will be updated below */
	out->recheck = false;

	/* leafDatum is what it is... */
	out->leafValue = in->leafDatum;

	/* Perform the required comparison(s) */
	for (i = 0; i < in->nkeys; i++)
	{
		StrategyNumber strategy = in->scankeys[i].sk_strategy;
		Oid subtype = in->scankeys[i].sk_subtype;
		STBOX query;

		/* Update the recheck flag according to the strategy */
		out->recheck |= index_tpoint_recheck(strategy);	

		if (subtype == type_oid(T_GEOMETRY) || subtype == type_oid(T_GEOGRAPHY))
		{
			GSERIALIZED *gs = (GSERIALIZED*)PG_DETOAST_DATUM(in->scankeys[i].sk_argument);
			if (!geo_to_stbox_internal(&query, gs))
				res = false;
			else
				res = index_leaf_consistent_stbox(key, &query, strategy);
		}
		else if (subtype == type_oid(T_STBOX))
		{
			STBOX *box = DatumGetSTboxP(in->scankeys[i].sk_argument);
			memcpy(&query, box, sizeof(STBOX));
			res = index_leaf_consistent_stbox(key, &query, strategy);
		}
		else if (temporal_type_oid(subtype))
		{
			temporal_bbox(&query,
				DatumGetTemporal(in->scankeys[i].sk_argument));
			res = index_leaf_consistent_stbox(key, &query, strategy);
		}
		else
			elog(ERROR, "unrecognized strategy: %d", strategy);

		/* If any check is failed, we have found our answer. */
		if (!res)
			break;
	}

	PG_RETURN_BOOL(res);
}

/*****************************************************************************
 * SP-GiST compress functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tpoint_compress);

PGDLLEXPORT Datum
spgist_tpoint_compress(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	STBOX *result = palloc0(sizeof(STBOX));
	temporal_bbox(result, temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_STBOX_P(result);
}

/*****************************************************************************/
