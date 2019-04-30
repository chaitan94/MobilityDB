/*****************************************************************************
 *
 * BoundBoxOps.c
 *	  Bounding box operators for temporal network-constrained points.
 *
 * These operators test the bounding boxes of temporal npoints, which are
 * 3D boxes, where the x and y coordinates are for the space (value)
 * dimension and the z coordinate is for the time dimension.
 * The following operators are defined:
 *	  overlaps, contains, contained, same
 * The operators consider as many dimensions as they are shared in both
 * arguments: only the space dimension, only the time dimension, or both
 * the space and the time dimensions.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "TNPoint.h"

/*****************************************************************************
 * Transform a temporal npoint to a GBOX
 *****************************************************************************/

// TODO: set return value appropriately
bool
npoint_to_gbox_internal(GBOX *box, npoint *np)
{
	double infinity = get_float8_infinity();
	Datum geom = npoint_as_geom_internal(DatumGetNpoint(np));
	GBOX gbox;
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	if (gserialized_get_gbox_p(gs, &gbox) == LW_FAILURE)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("Error while computing the bounding box of the network point")));

	memcpy(box, &gbox, sizeof(GBOX));
	box->zmin = box->mmin = -infinity;
	box->zmax = box->mmax = infinity;
	FLAGS_SET_Z(box->flags, false);
	FLAGS_SET_M(box->flags, false);
	FLAGS_SET_GEODETIC(box->flags, false);
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	return true;
}

void
tnpointinst_make_gbox(GBOX *box, Datum value, TimestampTz t)
{
	npoint_to_gbox_internal(box, DatumGetNpoint(value));
	box->mmin = box->mmax = t;
	FLAGS_SET_M(box->flags, true);
	return;
}

void
tnpointinstarr_disc_to_gbox(GBOX *box, TemporalInst **instants, int count)
{
	Datum value = temporalinst_value(instants[0]);
	tnpointinst_make_gbox(box, value, instants[0]->t);
	for (int i = 1; i < count; i++)
	{
		GBOX box1;
		value = temporalinst_value(instants[i]);
		tnpointinst_make_gbox(&box1, value, instants[i]->t);
		gbox_merge(&box1, box);
	}
	return;
}

void
tnpointinstarr_cont_to_gbox(GBOX *box, TemporalInst **instants, int count)
{
	double infinity = get_float8_infinity();
	npoint *np = DatumGetNpoint(temporalinst_value(instants[0]));
	int64 rid = np->rid;
	double posmin = np->pos, posmax = np->pos, mmin = instants[0]->t, mmax = instants[0]->t;
	for (int i = 1; i < count; i++)
	{
		np = DatumGetNpoint(temporalinst_value(instants[i]));
		posmin = Min(posmin, np->pos);
		posmax = Max(posmax, np->pos);
		mmin = Min(mmin, instants[i]->t);
		mmax = Max(mmax, instants[i]->t);
	}

	Datum line = route_geom(rid);
	Datum geom;
	if (posmin == 0 && posmax == 1)
		geom = PointerGetDatum(gserialized_copy((GSERIALIZED *)PG_DETOAST_DATUM(line)));
	else
		geom = call_function3(LWGEOM_line_substring, line,
			Float8GetDatum(posmin), Float8GetDatum(posmax));
								 
	GBOX gbox;
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	if (gserialized_get_gbox_p(gs, &gbox) == LW_FAILURE)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("Error while computing the bounding box of the temporal point")));
								 
	memcpy(box, &gbox, sizeof(GBOX));
	box->mmin = mmin;
	box->mmax = mmax;
	box->zmin = -infinity;
	box->zmax = infinity;
	FLAGS_SET_M(box->flags, true);
	pfree(DatumGetPointer(line));
	pfree(DatumGetPointer(geom));
	return;
}

void
tnpointseqarr_to_gbox(GBOX *box, TemporalSeq **sequences, int count)
{
	temporalseq_bbox(box, sequences[0]);
	for (int i = 1; i < count; i++)
	{
		GBOX box1;
		temporalseq_bbox(&box1, sequences[i]);
		gbox_merge(&box1, box);
	}
	return;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(npoint_to_gbox);

PGDLLEXPORT Datum
npoint_to_gbox(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	GBOX *result = palloc0(sizeof(GBOX));
	npoint_to_gbox_internal(result, np);
	PG_RETURN_POINTER(result);
}

static bool
npoint_timestamp_to_gbox_internal(GBOX *box, npoint *np, TimestampTz t)
{
	npoint_to_gbox_internal(box, np);
	box->mmin = box->mmax = t;
	FLAGS_SET_M(box->flags, true);
	return true;
}

PG_FUNCTION_INFO_V1(npoint_timestamp_to_gbox);

PGDLLEXPORT Datum
npoint_timestamp_to_gbox(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	TimestampTz t = PG_GETARG_TIMESTAMPTZ(1);
	GBOX *result = palloc0(sizeof(GBOX));
	npoint_timestamp_to_gbox_internal(result, np, t);
	PG_RETURN_POINTER(result);
}

static bool
npoint_period_to_gbox_internal(GBOX *box, npoint *np, Period *p)
{
	npoint_to_gbox_internal(box, np);
	box->mmin = p->lower;
	box->mmax = p->upper;
	FLAGS_SET_M(box->flags, true);
	return true;
}

PG_FUNCTION_INFO_V1(npoint_period_to_gbox);

PGDLLEXPORT Datum
npoint_period_to_gbox(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Period *p = PG_GETARG_PERIOD(1);
	GBOX *result = palloc0(sizeof(GBOX));
	npoint_period_to_gbox_internal(result, np, p);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tnpoint_to_gbox);

PGDLLEXPORT Datum
tnpoint_to_gbox(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GBOX *result = palloc0(sizeof(GBOX));
	temporal_bbox(result, temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
/*
 * Expand the network point on the spatial dimension
 */

PG_FUNCTION_INFO_V1(npoint_expand_spatial);

PGDLLEXPORT Datum
npoint_expand_spatial(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	double d = PG_GETARG_FLOAT8(1);
	GBOX box;
	npoint_to_gbox_internal(&box, np);
	GBOX *result = gbox_expand_spatial_internal(&box, d);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * overlaps
 *****************************************************************************/

PG_FUNCTION_INFO_V1(overlaps_bbox_npoint_tnpoint);

PGDLLEXPORT Datum
overlaps_bbox_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box1, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = overlaps_gbox_gbox_internal(&box1, &box2);
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overlaps_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
overlaps_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	temporal_bbox(&box1, temp);
	if (!geo_to_gbox_internal(&box2, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	bool result = overlaps_gbox_gbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * contains
 *****************************************************************************/

PG_FUNCTION_INFO_V1(contains_bbox_npoint_tnpoint);

PGDLLEXPORT Datum
contains_bbox_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box1, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = contains_gbox_gbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contains_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
contains_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box2, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = contains_gbox_gbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * contained
 *****************************************************************************/

PG_FUNCTION_INFO_V1(contained_bbox_npoint_tnpoint);

PGDLLEXPORT Datum
contained_bbox_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box1, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = contained_gbox_gbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contained_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
contained_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box2, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = contained_gbox_gbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * same
 *****************************************************************************/

PG_FUNCTION_INFO_V1(same_bbox_npoint_tnpoint);

PGDLLEXPORT Datum
same_bbox_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box1, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = same_gbox_gbox_internal(&box1, &box2);
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(same_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
same_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box2, gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
		pfree(DatumGetPointer(geom));
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = same_gbox_gbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************/
