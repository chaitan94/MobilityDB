/*****************************************************************************
 *
 * temporalseq.c
 *	  Basic functions for temporal sequences.
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "temporalseq.h"

#include <assert.h>
#include <float.h>
#include <access/hash.h>
#include <libpq/pqformat.h>
#include <utils/builtins.h>
#include <utils/lsyscache.h>
#include <utils/timestamp.h>
#include <math.h>

#include "timestampset.h"
#include "period.h"
#include "periodset.h"
#include "timeops.h"
#include "doublen.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "temporal_boxops.h"
#include "rangetypes_ext.h"

#include "tpoint.h"
#include "tpoint_boxops.h"
#include "tgeo.h"
#include "tgeo_transform.h"
#include "tpoint_spatialfuncs.h"
#include "tpoint_distance.h"

/*****************************************************************************
 * General functions
 *****************************************************************************/

/*
 * The memory structure of a TemporalSeq with, e.g., 2 instants and
 * a precomputed trajectory is as follows
 *
 *	-------------------------------------------------------------------
 *	( TemporalSeq )_X | offset_0 | offset_1 | offset_2 | offset_3 | ...
 *	-------------------------------------------------------------------
 *	------------------------------------------------------------------------
 *	( TemporalInst_0 )_X | ( TemporalInst_1 )_X | ( bbox )_X | ( Traj )_X  |
 *	------------------------------------------------------------------------
 *
 * where the X are unused bytes added for double padding, offset_0 to offset_1
 * are offsets for the corresponding instants, offset_2 is the offset for the 
 * bounding box and offset_3 is the offset for the precomputed trajectory. 
 * Precomputed trajectories are only kept for temporal points of sequence 
 * duration.
 */

/* N-th TemporalInst of a TemporalSeq */

TemporalInst *
temporalseq_inst_n(const TemporalSeq *seq, int index)
{
	return (TemporalInst *)(
		(char *)(&seq->offsets[seq->count + 2]) + 	/* start of data */
			seq->offsets[index]);					/* offset */
}

/* Pointer to the bounding box of a TemporalSeq */

void * 
temporalseq_bbox_ptr(const TemporalSeq *seq)
{
	return (char *)(&seq->offsets[seq->count + 2]) +  	/* start of data */
		seq->offsets[seq->count];						/* offset */
}

/* Copy the bounding box of a TemporalSeq in the first argument */

void 
temporalseq_bbox(void *box, const TemporalSeq *seq)
{
	void *box1 = temporalseq_bbox_ptr(seq);
	size_t bboxsize = temporal_bbox_size(seq->valuetypid);
	memcpy(box, box1, bboxsize);
}

/* 
 * Are the three temporal instant values collinear?
 * These functions supposes that the segments are not constant.
 */

static bool
float_collinear(double x1, double x2, double x3,
	TimestampTz t1, TimestampTz t2, TimestampTz t3)
{
	double duration1 = (double) (t2 - t1);
	double duration2 = (double) (t3 - t2);
	double ratio;
	if (duration1 < duration2)
	{
		ratio = duration1 / duration2;
		x3 = x2 + (x3 - x2) * (1 - ratio);
	}
	else if (duration1 > duration2)
	{
		ratio = duration2 / duration1;
		x1 = x1 + (x2 - x1) * (1 - ratio);
	}
	double d1 = x2 - x1;
	double d2 = x3 - x2;
	return (fabs(d1 - d2) <= EPSILON);
}

static bool
double2_collinear(const double2 *x1, const double2 *x2, const double2 *x3,
	TimestampTz t1, TimestampTz t2, TimestampTz t3)
{
	double duration1 = (double) (t2 - t1);
	double duration2 = (double) (t3 - t2);
	double2 x1new, x3new;
	double ratio;
	if (duration1 < duration2)
	{
		ratio = 1 - duration1 / duration2;
		x3new.a = x2->a + (x3->a - x2->a) * ratio;
		x3new.b = x2->b + (x3->b - x2->b) * ratio;
		x3 = &x3new;
	}
	if (duration1 > duration2)
	{
		ratio = 1 - duration2 / duration1;
		x1new.a = x1->a + (x2->a - x1->a) * ratio;
		x1new.b = x1->b + (x2->b - x1->b) * ratio;
		x1 = &x1new;
	}
	double d1a = x2->a - x1->a;
	double d1b = x2->b - x1->b;
	double d2a = x3->a - x2->a;
	double d2b = x3->b - x2->b;
	bool result = (fabs(d1a - d2a) <= EPSILON && fabs(d1b - d2b) <= EPSILON);
	return result;
}

static bool
point_collinear(Datum value1, Datum value2, Datum value3,
	TimestampTz t1, TimestampTz t2, TimestampTz t3, bool hasz)
{
	double duration1 = (double) (t2 - t1);
	double duration2 = (double) (t3 - t2);
	double ratio = 1 - Min(duration1, duration2) / Max(duration1, duration2);
	POINT4D point1 = datum_get_point4d(value1);
	POINT4D point2 = datum_get_point4d(value2);
	POINT4D point3 = datum_get_point4d(value3);
	if (duration1 < duration2)
		interpolate_point4d(&point2, &point3, &point3, ratio);
	else if (duration1 > duration2)
		interpolate_point4d(&point1, &point2, &point1, ratio);
	bool result;
	if (hasz)
	{
		double dx1 = point2.x - point1.x;
		double dy1 = point2.y - point1.y;
		double dz1 = point2.z - point1.z;
		double dx2 = point3.x - point2.x;
		double dy2 = point3.y - point2.y;
		double dz2 = point3.z - point2.z;
		result = fabs(dx1 - dx2) <= EPSILON && fabs(dy1 - dy2) <= EPSILON &&
			fabs(dz1 - dz2) <= EPSILON;
	}
	else
	{
		double dx1 = point2.x - point1.x;
		double dy1 = point2.y - point1.y;
		double dx2 = point3.x - point2.x;
		double dy2 = point3.y - point2.y;
		result = fabs(dx1 - dx2) <= EPSILON && fabs(dy1 - dy2) <= EPSILON;
	}
	return result;
}

static bool
double3_collinear(const double3 *x1, const double3 *x2, const double3 *x3,
	TimestampTz t1, TimestampTz t2, TimestampTz t3)
{
	double duration1 = (double) (t2 - t1);
	double duration2 = (double) (t3 - t2);
	double3 x1new, x3new;
	double ratio;
	if (duration1 < duration2)
	{
		ratio = 1 - duration1 / duration2;
		x3new.a = x2->a + (x3->a - x2->a) * ratio;
		x3new.a = x2->b + (x3->b - x2->b) * ratio,
		x3new.a = x2->c + (x3->c - x2->c) * ratio;
		x3 = &x3new;
	}
	if (duration1 > duration2)
	{
		ratio = 1 - duration2 / duration1;
		x1new.a = x1->a + (x2->a - x1->a) * ratio;
		x1new.b = x1->b + (x2->b - x1->b) * ratio;
		x1new.c = x1->c + (x2->c - x1->c) * ratio;
		x1 = &x1new;
	}
	double d1a = x2->a - x1->a;
	double d1b = x2->b - x1->b;
	double d1c = x2->c - x1->c;
	double d2a = x3->a - x2->a;
	double d2b = x3->b - x2->b;
	double d2c = x3->c - x2->c;
	bool result = (fabs(d1a - d2a) <= EPSILON && fabs(d1b - d2b) <= EPSILON &&
		fabs(d1c - d2c) <= EPSILON);
	return result;
}

static bool
double4_collinear(const double4 *x1, const double4 *x2, const double4 *x3,
	TimestampTz t1, TimestampTz t2, TimestampTz t3)
{
	double duration1 = (double) (t2 - t1);
	double duration2 = (double) (t3 - t2);
	double4 x1new, x3new;
	double ratio;
	if (duration1 < duration2)
	{
		ratio = 1 - duration1 / duration2;
		x3new.a = x2->a + (x3->a - x2->a) * ratio;
		x3new.a = x2->b + (x3->b - x2->b) * ratio;
		x3new.a = x2->c + (x3->c - x2->c) * ratio;
		x3new.a = x2->d + (x3->d - x2->d) * ratio;
		x3 = &x3new;
	}
	if (duration1 > duration2)
	{
		ratio = 1 - duration2 / duration1;
		x1new.a = x1->a + (x2->a - x1->a) * ratio;
		x1new.b = x1->b + (x2->b - x1->b) * ratio;
		x1new.d = x1->c + (x2->c - x1->c) * ratio;
		x1new.d = x1->d + (x2->c - x1->d) * ratio;
		x1 = &x1new;
	}
	double d1a = x2->a - x1->a;
	double d1b = x2->b - x1->b;
	double d1c = x2->c - x1->c;
	double d1d = x2->d - x1->d;
	double d2a = x3->a - x2->a;
	double d2b = x3->b - x2->b;
	double d2c = x3->c - x2->c;
	double d2d = x3->d - x2->d;
	bool result = (fabs(d1a - d2a) <= EPSILON && fabs(d1b - d2b) <= EPSILON &&
		fabs(d1c - d2c) <= EPSILON && fabs(d1d - d2d) <= EPSILON);
	return result;
}

static bool
rtransform_collinear(rtransform *rt1, rtransform *rt2, rtransform *rt3, 
	TimestampTz t1, TimestampTz t2, TimestampTz t3)
{
	// TODO: If theta3 - theta1 > 180°, 
	// modify rt2 to have theta = 180° or -(180-epsilon)° 
	// depending on the rotation direction and return false.
	double duration1 = (double) (t2 - t1);
	double duration2 = (double) (t3 - t2);
	rtransform *rt1new, *rt3new;
	double ratio;
	if (duration1 < duration2)
	{
		ratio = duration1 / duration2;
		rt3new = rtransform_interpolate(rt2, rt3, ratio);
	}
	else
		rt3new = rt3;
	if (duration1 > duration2)
	{
		ratio = duration2 / duration1;
		rt1new = rtransform_interpolate(rt1, rt2, ratio);
	}
	else
		rt1new = rt1;
	double d1theta = rt2->theta - rt1new->theta;
	double d1tx = rt2->tx - rt1new->tx;
	double d1ty = rt2->ty - rt1new->ty;
	double d2theta = rt3new->theta - rt2->theta;
	double d2tx = rt3new->tx - rt2->tx;
	double d2ty = rt3new->ty - rt2->ty;
	bool result = (fabs(d1tx-d2tx) <= EPSILON 
				&& fabs(d1ty-d2ty) <= EPSILON 
				&& (fabs(d1theta-d2theta) <= EPSILON 
				 || fabs(d1theta-d2theta+2*M_PI) <= EPSILON 
				 || fabs(d1theta-d2theta-2*M_PI) <= EPSILON));
	if (duration1 < duration2)
		pfree(rt3new);
	if (duration1 > duration2)
		pfree(rt1new);
	return result;
}

static bool
datum_collinear(Oid valuetypid, Datum value1, Datum value2, Datum value3,
	TimestampTz t1, TimestampTz t2, TimestampTz t3)
{
	if (valuetypid == FLOAT8OID)
		return float_collinear(DatumGetFloat8(value1), DatumGetFloat8(value2), 
			DatumGetFloat8(value3), t1, t2, t3);
	if (valuetypid == type_oid(T_DOUBLE2))
		return double2_collinear(DatumGetDouble2P(value1), DatumGetDouble2P(value2), 
			DatumGetDouble2P(value3), t1, t2, t3);
	if (valuetypid == type_oid(T_GEOMETRY))
	{
		GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(value1);
		bool hasz = (bool) FLAGS_GET_Z(gs->flags);
		return point_collinear(value1, value2, value3, t1, t2, t3, hasz);
	}
	if (valuetypid == type_oid(T_DOUBLE3))
		return double3_collinear(DatumGetDouble3P(value1), DatumGetDouble3P(value2), 
			DatumGetDouble3P(value3), t1, t2, t3);
	if (valuetypid == type_oid(T_DOUBLE4))
		return double4_collinear(DatumGetDouble4P(value1), DatumGetDouble4P(value2), 
			DatumGetDouble4P(value3), t1, t2, t3);
	if (valuetypid == type_oid(T_RTRANSFORM))
		return rtransform_collinear(DatumGetRtransform(value1), DatumGetRtransform(value2), 
			DatumGetRtransform(value3), t1, t2, t3);
	return false;
}

/*
 * Normalize an array of instants.
 * The function assumes that there are at least 2 instants.
 * The function does not create new instants, it creates an array of pointers
 * to a subset of the instants passed in the first argument.
 */
static TemporalInst **
temporalinstarr_normalize(TemporalInst **instants, bool linear, int count, 
	int *newcount)
{
	Oid valuetypid;
	TemporalInst **result = palloc(sizeof(TemporalInst *) * count);
	/* Remove redundant instants */ 
	TemporalInst *inst1 = instants[0];
	Datum value1 = temporalinst_value(inst1);
	TemporalInst *inst2 = instants[1];
	Datum value2 = temporalinst_value(inst2);
	result[0] = inst1;
	int k = 1;
#ifdef WITH_POSTGIS
	bool regions = false;
	TemporalInst *tofree;
	if (inst2->valuetypid == type_oid(T_RTRANSFORM))
	{
		regions = true;
		rtransform *rt = rtransform_make(0, 0, 0);
		inst1 = temporalinst_make(RtransformGetDatum(rt), instants[0]->t, type_oid(T_RTRANSFORM));
		value1 = temporalinst_value(inst1);
		tofree = inst1;
	}
#endif
	for (int i = 2; i < count; i++)
	{
		valuetypid = inst1->valuetypid;
		TemporalInst *inst3 = instants[i];
		Datum value3 = temporalinst_value(inst3);
		if (
			/* step sequences and 2 consecutive instants that have the same value
				... 1@t1, 1@t2, 2@t3, ... -> ... 1@t1, 2@t3, ...
			*/
			(!linear && datum_eq(value1, value2, valuetypid))
			||
			/* 3 consecutive linear instants that have the same value
				... 1@t1, 1@t2, 1@t3, ... -> ... 1@t1, 1@t3, ...
			*/
			(linear && datum_eq(value1, value2, valuetypid) && datum_eq(value2, value3, valuetypid))
			||
			/* collinear linear instants
				... 1@t1, 2@t2, 3@t3, ... -> ... 1@t1, 3@t3, ...
			*/
			(linear && datum_collinear(valuetypid, value1, value2, value3, inst1->t, inst2->t, inst3->t))
			)
		{
			inst2 = inst3; value2 = value3;
		} 
		else 
		{
			result[k++] = inst2;
			inst1 = inst2; value1 = value2;
			inst2 = inst3; value2 = value3;
		}
	}
#ifdef WITH_POSTGIS
	if (regions)
		pfree(tofree);
#endif
	result[k++] = inst2;
	*newcount = k;
	return result;
}

/*****************************************************************************/

/* Join two temporal sequences
 * This function is called when normalizing an array of sequences. It supposes
 * that the two sequences are adjacent. The resulting sequence will remove the
 * last and/or the first instant of the first/second sequence depending on the
 * values of the last two Boolean arguments. */

TemporalSeq *
temporalseq_join(const TemporalSeq *seq1, const TemporalSeq *seq2, bool last, bool first)
{
	// TODO: Make it handle joining of rtransform sequences.
	// Can't use the standard temporalseq_from_temporalinstarr because of bounding box issues.
	// Also have to handle the trajectory if pointtype
	// Maybe enought to look at type of seq2 to differentiate

	// ex: 
	// if (seq2->valuetypid == type_oid(T_RTRANSFORM))
	//     return tgeoseq_join(TemporalSeq *seq1, TemporalSeq *seq2, bool last, bool first);

	// Can also be done more efficiently for the other cases I think
	// Maybe a general case with just copying instants, copying flags and joining bboxes.
	// Can we join trajectories?

	// Ensure that the two sequences has the same interpolation
	assert(MOBDB_FLAGS_GET_LINEAR(seq1->flags) == MOBDB_FLAGS_GET_LINEAR(seq2->flags));
	Oid valuetypid = seq1->valuetypid;

	size_t bboxsize = temporal_bbox_size(valuetypid);
	size_t memsize = double_pad(bboxsize);

	int count1 = last ? seq1->count - 1 : seq1->count;
	int start2 = first ? 1 : 0;
	for (int i = 0; i < count1; i++)
		memsize += double_pad(VARSIZE(temporalseq_inst_n(seq1, i)));
	for (int i = start2; i < seq2->count; i++)
		memsize += double_pad(VARSIZE(temporalseq_inst_n(seq2, i)));

	int count = count1 + (seq2->count - start2);

	bool trajectory = type_has_precomputed_trajectory(valuetypid);
	Datum traj = 0; /* keep compiler quiet */
	if (trajectory)
	{
		/* A trajectory is a geometry/geography, either a point or a
		 * linestring, which may be self-intersecting */
		traj = tpointseq_trajectory_join(seq1, seq2, last, first);
		memsize += double_pad(VARSIZE(DatumGetPointer(traj)));
	}

	/* Add the size of the struct and the offset array
	 * Notice that the first offset is already declared in the struct */
	size_t pdata = double_pad(sizeof(TemporalSeq)) + (count + 1) * sizeof(size_t);
	/* Create the TemporalSeq */
	TemporalSeq *result = palloc0(pdata + memsize);
	SET_VARSIZE(result, pdata + memsize);
	result->count = count;
	result->valuetypid = valuetypid;
	result->duration = TEMPORALSEQ;
	period_set(&result->period, seq1->period.lower, seq2->period.upper,
		seq1->period.lower_inc, seq2->period.upper_inc);
	MOBDB_FLAGS_SET_LINEAR(result->flags, MOBDB_FLAGS_GET_LINEAR(seq1->flags));
	MOBDB_FLAGS_SET_X(result->flags, true);
	MOBDB_FLAGS_SET_T(result->flags, true);
	if (valuetypid == type_oid(T_GEOMETRY) ||
		valuetypid == type_oid(T_GEOGRAPHY))
	{
		MOBDB_FLAGS_SET_Z(result->flags, MOBDB_FLAGS_GET_Z(seq1->flags));
		MOBDB_FLAGS_SET_GEODETIC(result->flags, MOBDB_FLAGS_GET_GEODETIC(seq1->flags));
	}

	/* Initialization of the variable-length part */
	int k = 0;
	size_t pos = 0;
	for (int i = 0; i < count1; i++)
	{
		memcpy(((char *)result) + pdata + pos, temporalseq_inst_n(seq1, i),
			VARSIZE(temporalseq_inst_n(seq1, i)));
		result->offsets[k++] = pos;
		pos += double_pad(VARSIZE(temporalseq_inst_n(seq1, i)));
	}
	for (int i = start2; i < seq2->count; i++)
	{
		memcpy(((char *)result) + pdata + pos, temporalseq_inst_n(seq2, i),
			VARSIZE(temporalseq_inst_n(seq2, i)));
		result->offsets[k++] = pos;
		pos += double_pad(VARSIZE(temporalseq_inst_n(seq2, i)));
	}
	/*
	 * Precompute the bounding box
	 */
	if (bboxsize != 0)
	{
		void *bbox = ((char *) result) + pdata + pos;
		if (valuetypid == BOOLOID || valuetypid == TEXTOID)
			memcpy(bbox, &result->period, bboxsize);
		else
		{
			memcpy(bbox, temporalseq_bbox_ptr(seq1), bboxsize);
			temporal_bbox_expand(bbox, temporalseq_bbox_ptr(seq2), valuetypid);
		}
		result->offsets[k] = pos;
		pos += double_pad(bboxsize);
	}
	if (trajectory)
	{
		result->offsets[k + 1] = pos;
		memcpy(((char *) result) + pdata + pos, DatumGetPointer(traj),
			VARSIZE(DatumGetPointer(traj)));
		pfree(DatumGetPointer(traj));
	}

	return result;
}

/*static TemporalSeq *
temporalseq_join(TemporalSeq *seq1, TemporalSeq *seq2, bool last, bool first)
{
	assert(MOBDB_FLAGS_GET_LINEAR(seq1->flags) == MOBDB_FLAGS_GET_LINEAR(seq2->flags));
	Oid valuetypid = seq1->valuetypid;
	Oid valuetypid2 = seq2->valuetypid;

	size_t bboxsize = temporal_bbox_size(valuetypid);
	size_t memsize = double_pad(bboxsize);

	int count1 = last ? seq1->count - 1 : seq1->count;
	int start2 = first ? 1 : 0;
	for (int i = 0; i < count1; i++)
		memsize += double_pad(VARSIZE(temporalseq_inst_n(seq1, i)));
	for (int i = start2; i < seq2->count; i++)
		memsize += double_pad(VARSIZE(temporalseq_inst_n(seq2, i)));

	int count = count1 + (seq2->count - start2);

#ifdef WITH_POSTGIS*/
	// Maybe use type_has_precomputed_trajectory, 
	// but we have to change valuetypeid to differentiate between point and region
/*	bool trajectory = ((valuetypid2 == type_oid(T_GEOMETRY) ||
				    	valuetypid2 == type_oid(T_GEOGRAPHY)));*/ // If region, we will have valuetypeid2 == T_RTRANSFORM
/*	Datum traj = 0;*/ /* keep compiler quiet */
/*	if (trajectory)
	{*/
		/* A trajectory is a geometry/geography, either a point or a 
		 * linestring, which may be self-intersecting */
/*		traj = tpointseq_trajectory_join(seq1, seq2, last, first);
		memsize += double_pad(VARSIZE(DatumGetPointer(traj)));
	}
#endif*/

	/* Add the size of the struct and the offset array 
	 * Notice that the first offset is already declared in the struct */
/*	size_t pdata = double_pad(sizeof(TemporalSeq)) + (count + 1) * sizeof(size_t);*/
	/* Create the TemporalSeq */
/*	TemporalSeq *result = palloc0(pdata + memsize);
	SET_VARSIZE(result, pdata + memsize);
	result->count = count;
	result->valuetypid = valuetypid;
	result->duration = TEMPORALSEQ;
	period_set(&result->period, temporalseq_inst_n(seq1, 0)->t, temporalseq_inst_n(seq2, seq2->count-1)->t,
		seq1->period.lower_inc, seq2->period.upper_inc);
	MOBDB_FLAGS_SET_LINEAR(result->flags, MOBDB_FLAGS_GET_LINEAR(seq1->flags));
#ifdef WITH_POSTGIS
	if (valuetypid == type_oid(T_GEOMETRY) ||
		valuetypid == type_oid(T_GEOGRAPHY))
	{
		MOBDB_FLAGS_SET_Z(result->flags, MOBDB_FLAGS_GET_Z(seq1->flags));
		MOBDB_FLAGS_SET_GEODETIC(result->flags, MOBDB_FLAGS_GET_GEODETIC(seq1->flags));
	}
#endif*/

	/* Initialization of the variable-length part */
/*	int k = 0;
	size_t pos = 0;
	for (int i = 0; i < count1; i++)
	{
		memcpy(((char *)result) + pdata + pos, temporalseq_inst_n(seq1, i), 
			VARSIZE(temporalseq_inst_n(seq1, i)));
		result->offsets[k++] = pos;
		pos += double_pad(VARSIZE(temporalseq_inst_n(seq1, i)));
	}
	for (int i = start2; i < seq2->count; i++)
	{
		memcpy(((char *)result) + pdata + pos, temporalseq_inst_n(seq2, i), 
			VARSIZE(temporalseq_inst_n(seq2, i)));
		result->offsets[k++] = pos;
		pos += double_pad(VARSIZE(temporalseq_inst_n(seq2, i)));
	}*/
	/*
	 * Precompute the bounding box 
	 */
/*	if (bboxsize != 0)
	{
		void *box1 = temporalseq_bbox_ptr(seq1);
		void *box2 = temporalseq_bbox_ptr(seq2);
		void *bbox = ((char *) result) + pdata + pos;
		if (valuetypid == BOOLOID || valuetypid == TEXTOID)
			period_set((Period *)bbox, 
				((Period *)box1)->lower, ((Period *)box2)->upper, 
				((Period *)box1)->lower_inc, ((Period *)box2)->upper_inc);
		else if (valuetypid == INT4OID || valuetypid == FLOAT8OID)
		{
			memcpy(bbox, box1, bboxsize);
			tbox_expand((TBOX *)bbox, (TBOX *)box2);
		}
#ifdef WITH_POSTGIS
		else if (valuetypid == type_oid(T_GEOGRAPHY) || 
				 valuetypid == type_oid(T_GEOMETRY)) 
		{
			memcpy(bbox, box1, bboxsize);
			stbox_expand((STBOX *)bbox, (STBOX *)box2);
		}
#endif
		result->offsets[k] = pos;
		pos += double_pad(bboxsize);
	}
	if (trajectory)
	{
		result->offsets[k + 1] = pos;
		memcpy(((char *) result) + pdata + pos, DatumGetPointer(traj),
			VARSIZE(DatumGetPointer(traj)));
		pfree(DatumGetPointer(traj));
	}

	return result;
}*/

/*
 * Normalize an array of temporal sequences values. 
 * It is supposed that each individual sequence is already normalized.
 * The sequences may be non-contiguous but must ordered and non-overlapping.
 * The function creates new sequences and does not free the original sequences.
 */
TemporalSeq **
temporalseqarr_normalize(TemporalSeq **sequences, int count, int *newcount)
{
	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * count);
	/* seq1 is the sequence to which we try to join subsequent seq2 */
	TemporalSeq *seq1 = sequences[0];
	Oid valuetypid = seq1->valuetypid;
	bool linear = MOBDB_FLAGS_GET_LINEAR(seq1->flags);
	bool isnew = false;
	int k = 0;
#ifdef WITH_POSTGIS
	bool regions = false;
	rtransform *rt = NULL;
	TemporalInst *inst = NULL;
	uint geo_type;
	if (seq1->valuetypid == type_oid(T_GEOMETRY) || seq1->valuetypid == type_oid(T_GEOGRAPHY))
	{
		TemporalInst *instant = temporalseq_inst_n(seq1, 0);
		Datum value = temporalinst_value(instant);
		GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(value);
		geo_type = gserialized_get_type(gs);
		if (geo_type == POLYGONTYPE || geo_type == LINETYPE)
			regions = true;
			valuetypid = type_oid(T_RTRANSFORM);
	}
#endif
	for (int i = 1; i < count; i++)
	{
		TemporalSeq *seq2 = sequences[i];
		TemporalInst *last2 = (seq1->count == 1) ? NULL : 
			temporalseq_inst_n(seq1, seq1->count - 2); 
		Datum last2value = (seq1->count == 1) ? 0 : 
			temporalinst_value(last2);
		TemporalInst *last1 = temporalseq_inst_n(seq1, seq1->count - 1);
		Datum last1value = temporalinst_value(last1);
		TemporalInst *first1 = temporalseq_inst_n(seq2, 0);
		Datum first1value = temporalinst_value(first1);
		TemporalInst *first2 = (seq2->count == 1) ? NULL : 
			temporalseq_inst_n(seq2, 1); 
		Datum first2value = (seq2->count == 1) ? 0 : 
			temporalinst_value(first2);
		if (regions && k == 0 && (seq1->count == 1 || seq1->count == 2)) 
		{
			if (rt == NULL)
				rt = rtransform_make(0, 0, 0);
			if (seq1->count == 1)
				last1value = RtransformGetDatum(rt);
			else if (seq1->count == 2)
			{
				if (inst == NULL)
					inst = temporalinst_make(last2value, last2->t, type_oid(T_RTRANSFORM));
				last2value = RtransformGetDatum(rt);
				last2 = inst;
			}
		}
		bool adjacent = seq1->period.upper == seq2->period.lower && 
			(seq1->period.upper_inc || seq2->period.lower_inc);
		/* If they are adjacent and not instantaneous */
		if (adjacent && last2 != NULL && first2 != NULL &&
			(
			/* If step and the last segment of the first sequence is constant
			   ..., 1@t1, 1@t2) [1@t2, 1@t3, ... -> ..., 1@t1, 2@t3, ... 
			   ..., 1@t1, 1@t2) [1@t2, 2@t3, ... -> ..., 1@t1, 2@t3, ... 
			   ..., 1@t1, 1@t2] (1@t2, 2@t3, ... -> ..., 1@t1, 2@t3, ... 
			 */
			(!linear && 
			datum_eq(last2value, last1value, valuetypid) && 
			datum_eq(last1value, first1value, valuetypid))
			||			
			/* If the last/first segments are constant and equal 
			   ..., 1@t1, 1@t2] (1@t2, 1@t3, ... -> ..., 1@t1, 1@t3, ... 
			 */
			(datum_eq(last2value, last1value, valuetypid) &&
			datum_eq(last1value, first1value, valuetypid) && 
			datum_eq(first1value, first2value, valuetypid))
			||			
			/* If float/point sequences and collinear last/first segments having the same duration 
			   ..., 1@t1, 2@t2) [2@t2, 3@t3, ... -> ..., 1@t1, 3@t3, ... 
			*/
			(datum_eq(last1value, first1value, valuetypid) && 
			datum_collinear(valuetypid, last2value, first1value, first2value,
				last2->t, first1->t, first2->t))
			))
		{
			/* Remove the last and first instants of the sequences */
			seq1 = temporalseq_join(seq1, seq2, true, true);
			isnew = true;
		}
		/* If step sequences and the first one has an exclusive upper bound,
		   by definition the first sequence has the last segment constant
		   ..., 1@t1, 1@t2) [2@t2, 3@t3, ... -> ..., 1@t1, 2@t2, 3@t3, ... 
		   ..., 1@t1, 1@t2) [2@t2] -> ..., 1@t1, 2@t2]
		 */
		else if (adjacent && !linear && !seq1->period.upper_inc)
		{
			/* Remove the last instant of the first sequence */
			seq1 = temporalseq_join(seq1, seq2, true, false);
			isnew = true;
		}
		/* If they are adjacent and have equal last/first value respectively 
			Stewise
			... 1@t1, 2@t2], (2@t2, 1@t3, ... -> ..., 1@t1, 2@t2, 1@t3, ...
			[1@t1], (1@t1, 2@t2, ... -> ..., 1@t1, 2@t2
			Linear	
			..., 1@t1, 2@t2), [2@t2, 1@t3, ... -> ..., 1@t1, 2@t2, 1@t3, ...
			..., 1@t1, 2@t2], (2@t2, 1@t3, ... -> ..., 1@t1, 2@t2, 1@t3, ...
			..., 1@t1, 2@t2), [2@t2] -> ..., 1@t1, 2@t2]
			[1@t1],(1@t1, 2@t2, ... -> [1@t1, 2@t2, ...
		*/
		else if (adjacent && datum_eq(last1value, first1value, valuetypid))
		{
			/* Remove the first instant of the second sequence */
			seq1 = temporalseq_join(seq1, seq2, false, true);
			isnew = true;
		} 
		else 
		{
			result[k++] = isnew ? seq1 : temporalseq_copy(seq1);
			seq1 = seq2;
			isnew = false;
		}
	}
#ifdef WITH_POSTGIS
	if (rt != NULL)
		pfree(rt);
	if (inst != NULL)
		pfree(inst);
#endif
	result[k++] = isnew ? seq1 : temporalseq_copy(seq1);
	*newcount = k;
	return result;
}

/*****************************************************************************/

/* Construct a TemporalSeq from an array of TemporalInst and the bounds.
 * Depending on the value of the normalize argument, the resulting sequence
 * will be normalized. */
TemporalSeq *
temporalseq_make(TemporalInst **instants, int count, bool lower_inc,
   bool upper_inc, bool linear, bool normalize)
{
	/* Test the validity of the instants and the bounds */
	assert(count > 0);
	if (count == 1 && (!lower_inc || !upper_inc))
		ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
			errmsg("Instant sequence must have inclusive bounds")));
	bool isgeo = (instants[0]->valuetypid == type_oid(T_GEOMETRY) ||
		instants[0]->valuetypid == type_oid(T_GEOGRAPHY));
	uint geo_type;
	uint poly_nrings;
	uint* poly_npoints;
	if (isgeo)
	{
		Datum value = temporalinst_value((TemporalInst *) instants[0]);
		GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(value);
		geo_type = gserialized_get_type(gs);
		if (geo_type == POLYGONTYPE)
		{
			LWPOLY *poly = (LWPOLY *) lwgeom_from_gserialized(gs);
			poly_nrings = poly->nrings;
			poly_npoints = malloc(sizeof(uint)*poly_nrings);
			for (int i = 0; i < (int) poly_nrings; ++i)
			{
				poly_npoints[i] = poly->rings[i]->npoints;
			}
			lwpoly_free(poly);
		}
	}
	for (int i = 1; i < count; i++)
	{
		ensure_increasing_timestamps(instants[i - 1], instants[i]);
		if (isgeo)
		{
			ensure_same_srid_tpoint((Temporal *)instants[i - 1], (Temporal *)instants[i]);
			ensure_same_dimensionality_tpoint((Temporal *)instants[i - 1], (Temporal *)instants[i]);
			if (geo_type == POLYGONTYPE)
			{
				Datum value = temporalinst_value((TemporalInst *) instants[i]);
				GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(value);
				if (geo_type != gserialized_get_type(gs))
					ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
						errmsg("All geometries must be of the same type")));
				LWPOLY *poly = (LWPOLY *) lwgeom_from_gserialized(gs);
				/* Maybe test better using number of inner rings and number of ponts of each ring */
				if (poly_nrings != poly->nrings)
					ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
						errmsg("All regions must contain the same number of rings")));
				for (int j = 0; j < (int) poly_nrings; ++j)
				{
					if (poly_npoints[j] != poly->rings[j]->npoints)
						ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
							errmsg("Corresponding rings in each region must contain the same number of points")));
				}
				lwpoly_free(poly);
			}
		}
	}
#ifdef WITH_POSTGIS
	if (isgeo && geo_type == POLYGONTYPE)
		free(poly_npoints);
#endif

	if (!linear && count > 1 && !upper_inc &&
		datum_ne(temporalinst_value(instants[count - 1]), 
			temporalinst_value(instants[count - 2]), instants[0]->valuetypid))
		ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
			errmsg("Invalid end value for temporal sequence")));

	/* Get the bounding box size */
	size_t bboxsize = temporal_bbox_size(instants[0]->valuetypid);
	size_t memsize = double_pad(bboxsize);
	void *bbox;
	bool bbox_precomputed = false;
	/*
	 * Precompute the bounding box 
	 * Compute before transforming the instants if we handle regions
	 */
#ifdef WITH_POSTGIS
	if (bboxsize != 0 && isgeo && (geo_type == LINETYPE || geo_type == POLYGONTYPE))
	{
		bbox_precomputed = true;
		bbox = palloc0(bboxsize);
		temporalseq_make_bbox(bbox, instants, count, lower_inc, upper_inc);
	}

	/*
	 * Transform the instants into rtransforms (keep the first instant as a region)
	 */
	if (count > 1 && isgeo && (geo_type == LINETYPE || geo_type == POLYGONTYPE))
		instants = geo_instarr_to_rtransform(instants, count);
#endif

	/* Normalize the array of instants */
	TemporalInst **newinstants = instants;
	int newcount = count;
	if (normalize && count > 2)
		newinstants = temporalinstarr_normalize(instants, linear, count, &newcount);

	/* Add the size of composing instants */
	for (int i = 0; i < newcount; i++)
		memsize += double_pad(VARSIZE(newinstants[i]));
	/* Precompute the trajectory */
	bool trajectory = false; /* keep compiler quiet */
	Datum traj = 0; /* keep compiler quiet */
	if (isgeo && !(geo_type == LINETYPE || geo_type == POLYGONTYPE)) // maybe change 'type_has_precomputed_trajectory'
	{
		trajectory = type_has_precomputed_trajectory(instants[0]->valuetypid);
		if (trajectory)
		{
			/* A trajectory is a geometry/geography, a point, a multipoint, 
			 * or a linestring, which may be self-intersecting */
			traj = tpointseq_make_trajectory(newinstants, newcount, linear);
			memsize += double_pad(VARSIZE(DatumGetPointer(traj)));
		}
	}
	/* Add the size of the struct and the offset array
	 * Notice that the first offset is already declared in the struct */
	size_t pdata = double_pad(sizeof(TemporalSeq)) + (newcount + 1) * sizeof(size_t);
	/* Create the TemporalSeq */
	TemporalSeq *result = palloc0(pdata + memsize);
	SET_VARSIZE(result, pdata + memsize);
	result->count = newcount;
	result->valuetypid = instants[0]->valuetypid;
	result->duration = TEMPORALSEQ;
	period_set(&result->period, newinstants[0]->t, newinstants[newcount - 1]->t,
		lower_inc, upper_inc);
	MOBDB_FLAGS_SET_LINEAR(result->flags, linear);
	MOBDB_FLAGS_SET_X(result->flags, true);
	MOBDB_FLAGS_SET_T(result->flags, true);
	if (isgeo)
	{
		MOBDB_FLAGS_SET_Z(result->flags, MOBDB_FLAGS_GET_Z(instants[0]->flags));
		MOBDB_FLAGS_SET_GEODETIC(result->flags, MOBDB_FLAGS_GET_GEODETIC(instants[0]->flags));
	}
	/* Initialization of the variable-length part */
	size_t pos = 0;
	for (int i = 0; i < newcount; i++)
	{
		memcpy(((char *)result) + pdata + pos, newinstants[i], 
			VARSIZE(newinstants[i]));
		result->offsets[i] = pos;
		pos += double_pad(VARSIZE(newinstants[i]));
	}
	/*
	 * Precompute the bounding box 
	 * Only external types have precomputed bounding box, internal types such
	 * as double2, double3, or double4 do not have precomputed bounding box.
	 * For temporal points the bounding box is computed from the trajectory 
	 * for efficiency reasons.
	 */
	if (bboxsize != 0 && !bbox_precomputed)
	{
		bbox = ((char *) result) + pdata + pos;
		if (trajectory)
		{
			geo_to_stbox_internal(bbox, (GSERIALIZED *)DatumGetPointer(traj));
			((STBOX *)bbox)->tmin = result->period.lower;
			((STBOX *)bbox)->tmax = result->period.upper;
			MOBDB_FLAGS_SET_T(((STBOX *)bbox)->flags, true);
		}
		else
			temporalseq_make_bbox(bbox, newinstants, newcount,
				lower_inc, upper_inc);
		result->offsets[newcount] = pos;
		pos += double_pad(bboxsize);
	}
	else if (bboxsize != 0 && bbox_precomputed)
	{
		memcpy(((char *)result) + pdata + pos, bbox, bboxsize);
		result->offsets[newcount] = pos;
		pos += double_pad(bboxsize);
		pfree(bbox);
	}
	if (isgeo && trajectory)
	{
		result->offsets[newcount + 1] = pos;
		memcpy(((char *) result) + pdata + pos, DatumGetPointer(traj),
			VARSIZE(DatumGetPointer(traj)));
		pfree(DatumGetPointer(traj));
	}
	if (count > 1 && isgeo && (geo_type == LINETYPE || geo_type == POLYGONTYPE))
	{
		for (int i = 0; i < count; ++i)
			pfree(instants[i]);
		pfree(instants);
	}

	if (normalize && count > 2)
		pfree(newinstants);

	return result;
}

/* Consruct a TemporalSeq from a base value and a period */

TemporalSeq *
temporalseq_from_base_internal(Datum value, Oid valuetypid, const Period *p, bool linear)
{
	TemporalInst *instants[2];
	instants[0] = temporalinst_make(value, p->lower, valuetypid);
	instants[1] = temporalinst_make(value, p->upper, valuetypid);
	TemporalSeq *result = temporalseq_make(instants, 2, p->lower_inc,
		p->upper_inc, linear, false);
	pfree(instants[0]); pfree(instants[1]);
	return result;
}

PG_FUNCTION_INFO_V1(temporalseq_from_base);

PGDLLEXPORT Datum
temporalseq_from_base(PG_FUNCTION_ARGS)
{
	Datum value = PG_GETARG_ANYDATUM(0);
	Period *p = PG_GETARG_PERIOD(1);
	bool linear = PG_GETARG_BOOL(2);
	Oid valuetypid = get_fn_expr_argtype(fcinfo->flinfo, 0);
	TemporalSeq *result = temporalseq_from_base_internal(value, valuetypid, p, linear);
	DATUM_FREE_IF_COPY(value, valuetypid, 0);
	PG_RETURN_POINTER(result);
}


/* Append a TemporalInst to a TemporalSeq */

TemporalSeq *
temporalseq_append_instant(const TemporalSeq *seq, const TemporalInst *inst)
{
	/* Test the validity of the instant */
	assert(seq->valuetypid == inst->valuetypid);
	assert(MOBDB_FLAGS_GET_GEODETIC(seq->flags) == MOBDB_FLAGS_GET_GEODETIC(inst->flags));
	TemporalInst *inst1 = temporalseq_inst_n(seq, seq->count - 1);
	ensure_increasing_timestamps(inst1, inst);
	bool isgeo = (seq->valuetypid == type_oid(T_GEOMETRY) ||
		seq->valuetypid == type_oid(T_GEOGRAPHY));
	if (isgeo)
	{
		ensure_same_srid_tpoint((Temporal *)seq, (Temporal *)inst);
		ensure_same_dimensionality_tpoint((Temporal *)seq, (Temporal *)inst);
	}

	bool linear = MOBDB_FLAGS_GET_LINEAR(seq->flags);
	/* Normalize the result */
	int newcount = seq->count + 1;
	if (seq->count > 1)
	{
		inst1 = temporalseq_inst_n(seq, seq->count - 2);
		Datum value1 = temporalinst_value(inst1);
		TemporalInst *inst2 = temporalseq_inst_n(seq, seq->count - 1);
		Datum value2 = temporalinst_value(inst2);
		Datum value3 = temporalinst_value(inst);
		if (
			/* step sequences and 2 consecutive instants that have the same value
				... 1@t1, 1@t2, 2@t3, ... -> ... 1@t1, 2@t3, ...
			*/
			(! linear && datum_eq(value1, value2, seq->valuetypid))
			||
			/* 3 consecutive float/point instants that have the same value 
				... 1@t1, 1@t2, 1@t3, ... -> ... 1@t1, 1@t3, ...
			*/
			(datum_eq(value1, value2, seq->valuetypid) && datum_eq(value2, value3, seq->valuetypid))
			||
			/* collinear float/point instants that have the same duration
				... 1@t1, 2@t2, 3@t3, ... -> ... 1@t1, 3@t3, ...
			*/
			(linear && datum_collinear(seq->valuetypid, value1, value2, value3, inst1->t, inst2->t, inst->t))
			)
		{
			/* The new instant replaces the last instant of the sequence */
			newcount--;
		} 
	}
	/* Get the bounding box size */
	size_t bboxsize = temporal_bbox_size(seq->valuetypid);
	size_t memsize = double_pad(bboxsize);
	/* Add the size of composing instants */
	for (int i = 0; i < newcount - 1; i++)
		memsize += double_pad(VARSIZE(temporalseq_inst_n(seq, i)));
	memsize += double_pad(VARSIZE(inst));
	/* Expand the trajectory */
	bool trajectory = false; /* keep compiler quiet */
	Datum traj = 0; /* keep compiler quiet */
	if (isgeo)
	{
		trajectory = type_has_precomputed_trajectory(seq->valuetypid);
		if (trajectory)
		{
			bool replace = newcount != seq->count + 1;
			traj = tpointseq_trajectory_append(seq, inst, replace);
			memsize += double_pad(VARSIZE(DatumGetPointer(traj)));
		}
	}
	/* Add the size of the struct and the offset array
	 * Notice that the first offset is already declared in the struct */
	size_t pdata = double_pad(sizeof(TemporalSeq)) + (newcount + 1) * sizeof(size_t);
	/* Create the TemporalSeq */
	TemporalSeq *result = palloc0(pdata + memsize);
	SET_VARSIZE(result, pdata + memsize);
	result->count = newcount;
	result->valuetypid = seq->valuetypid;
	result->duration = TEMPORALSEQ;
	period_set(&result->period, seq->period.lower, inst->t, 
		seq->period.lower_inc, true);
	MOBDB_FLAGS_SET_LINEAR(result->flags, MOBDB_FLAGS_GET_LINEAR(seq->flags));
	MOBDB_FLAGS_SET_X(result->flags, true);
	MOBDB_FLAGS_SET_T(result->flags, true);
	if (isgeo)
	{
		MOBDB_FLAGS_SET_Z(result->flags, MOBDB_FLAGS_GET_Z(seq->flags));
		MOBDB_FLAGS_SET_GEODETIC(result->flags, MOBDB_FLAGS_GET_GEODETIC(seq->flags));
	}
	/* Initialization of the variable-length part */
	size_t pos = 0;
	for (int i = 0; i < newcount - 1; i++)
	{
		inst1 = temporalseq_inst_n(seq, i);
		memcpy(((char *)result) + pdata + pos, inst1, VARSIZE(inst1));
		result->offsets[i] = pos;
		pos += double_pad(VARSIZE(inst1));
	}
	/* Append the instant */
	memcpy(((char *)result) + pdata + pos, inst, VARSIZE(inst));
	result->offsets[newcount - 1] = pos;
	pos += double_pad(VARSIZE(inst));
	/* Expand the bounding box */
	if (bboxsize != 0) 
	{
		union bboxunion box;
		void *bbox = ((char *) result) + pdata + pos;
		memcpy(bbox, temporalseq_bbox_ptr(seq), bboxsize);
		temporalinst_make_bbox(&box, inst);
		temporal_bbox_expand(bbox, &box, seq->valuetypid);
		result->offsets[newcount] = pos;
	}
	if (isgeo && trajectory)
	{
		result->offsets[newcount + 1] = pos;
		memcpy(((char *) result) + pdata + pos, DatumGetPointer(traj),
			VARSIZE(DatumGetPointer(traj)));
		pfree(DatumGetPointer(traj));
	}
	return result;
}

/* Append two temporal values */

Temporal *
temporalseq_append(const TemporalSeq *seq1, const TemporalSeq *seq2)
{
	/* Test the validity of both temporal values */
	assert(seq1->valuetypid == seq2->valuetypid);
	assert(MOBDB_FLAGS_GET_LINEAR(seq1->flags) == MOBDB_FLAGS_GET_LINEAR(seq2->flags));
	assert(MOBDB_FLAGS_GET_GEODETIC(seq1->flags) == MOBDB_FLAGS_GET_GEODETIC(seq2->flags));
	bool isgeo = (seq1->valuetypid == type_oid(T_GEOMETRY) ||
		seq1->valuetypid == type_oid(T_GEOGRAPHY));
	if (isgeo)
	{
		ensure_same_srid_tpoint((Temporal *)seq1, (Temporal *)seq2);
		ensure_same_dimensionality_tpoint((Temporal *)seq1, (Temporal *)seq2);
	}
	TemporalInst *inst1 = temporalseq_inst_n(seq1, seq1->count - 1);
	TemporalInst *inst2 = temporalseq_inst_n(seq2, 0);
	if (inst1->t > inst2->t)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("The temporal values cannot overlap on time")));
	if (inst1->t == inst2->t &&
		(seq1->period.upper_inc || seq2->period.lower_inc))
	{
		if (! datum_eq(temporalinst_value(inst1), temporalinst_value(inst2), inst1->valuetypid))
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				errmsg("The temporal values have different value at their overlapping instant")));
		else
			/* Result is a TemporalSeq */
			return (Temporal *) temporalseq_join(seq1, seq2, true, false);
	}
	else
	{
		const TemporalSeq *sequences[] = {seq1, seq2};
		/* Result is a TemporalS */
		return (Temporal *) temporals_make((TemporalSeq **)sequences, 2, false);
	}
}

/* Append an array of temporal values */

Temporal *
temporalseq_append_array(TemporalSeq **seqs, int count)
{
	Oid valuetypid = seqs[0]->valuetypid;
	bool linear = MOBDB_FLAGS_GET_LINEAR(seqs[0]->flags);
	bool isgeo = (seqs[0]->valuetypid == type_oid(T_GEOMETRY) ||
		seqs[0]->valuetypid == type_oid(T_GEOGRAPHY));
	/* Keep track of the number of instants in the resulting sequences */
	int *countinst = palloc0(sizeof(int) * count);
	countinst[0] = seqs[0]->count;
	int k = 0;
	for (int i = 1; i < count; i++)
	{
		/* Test the validity of consecutive temporal values */
		assert(seqs[i]->valuetypid == valuetypid);
		assert(MOBDB_FLAGS_GET_LINEAR(seqs[i]->flags) == linear);
		if (isgeo)
		{
			ensure_same_srid_tpoint((Temporal *)seqs[i - 1], (Temporal *)seqs[i]);
			ensure_same_dimensionality_tpoint((Temporal *)seqs[i - 1], (Temporal *)seqs[i]);
		}
		TemporalInst *inst1 = temporalseq_inst_n(seqs[i - 1], seqs[i - 1]->count - 1);
		TemporalInst *inst2 = temporalseq_inst_n(seqs[i], 0);
		if (inst1->t > inst2->t)
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				errmsg("The temporal values cannot overlap on time")));
		if (inst1->t == inst2->t && seqs[i]->period.lower_inc)
		{
			if (! datum_eq(temporalinst_value(inst1), temporalinst_value(inst2), inst1->valuetypid) &&
				seqs[i - 1]->period.upper_inc && seqs[i]->period.lower_inc)
				ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
					errmsg("The temporal values have different value at their overlapping instant")));
			else
				countinst[k] += seqs[i]->count - 1;
		}
		else
			countinst[++k] = seqs[i]->count;
	}
	k++;
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * k);
	int l = 0;
	for (int i = 0; i < k; i++)
	{
		bool lowerinc = seqs[0]->period.lower_inc;
		TemporalInst **instants = palloc(sizeof(TemporalInst *) * countinst[i]);
		int m = 0;
		while (m < countinst[i] && l < count)
		{
			int start = seqs[l]->period.lower_inc && ( m == 0 || ! seqs[l -1]->period.upper_inc ) ? 0 : 1;
			int end = seqs[l]->period.upper_inc ? seqs[l]->count : seqs[l]->count - 1;
			for (int j = start; j < end; j++)
				instants[m++] = temporalseq_inst_n(seqs[l], j);
			l++;
		}
		bool upperinc = seqs[l - 1]->period.upper_inc;
		if (! upperinc)
			instants[m++] = temporalseq_inst_n(seqs[l - 1], seqs[l - 1]->count - 1);
		sequences[i] = temporalseq_make(instants, countinst[i], lowerinc,
			upperinc, linear, true);
		pfree(instants);
	}
	Temporal *result = (k == 1) ? (Temporal *)sequences[0] :
		(Temporal *)temporals_make(sequences, k, true);
	pfree(sequences);
	pfree(countinst);
	return result;
}

/* Copy a temporal sequence */

TemporalSeq *
temporalseq_copy(const TemporalSeq *seq)
{
	TemporalSeq *result = palloc0(VARSIZE(seq));
	memcpy(result, seq, VARSIZE(seq));
	return result;
}

/* Binary search of a timestamptz in a TemporalSeq */

int
temporalseq_find_timestamp(const TemporalSeq *seq, TimestampTz t)
{
	int first = 0;
	int last = seq->count - 2;
	int middle = (first + last)/2;
	while (first <= last) 
	{
		TemporalInst *inst1 = temporalseq_inst_n(seq, middle);
		TemporalInst *inst2 = temporalseq_inst_n(seq, middle + 1);
		bool lower_inc = (middle == 0) ? seq->period.lower_inc : true;
		bool upper_inc = (middle == seq->count - 2) ? seq->period.upper_inc : false;
		if ((inst1->t < t && t < inst2->t) ||
			(lower_inc && inst1->t == t) || (upper_inc && inst2->t == t))
			return middle;
		if (t <= inst1->t)
			last = middle - 1;
		else
			first = middle + 1;	
		middle = (first + last)/2;
	}
	return -1;
}

/*****************************************************************************
 * Intersection functions
 *****************************************************************************/
 
/* 
 * Intersection of a TemporalSeq and a TemporalInst values. 
 */

bool
intersection_temporalseq_temporalinst(const TemporalSeq *seq, const TemporalInst *inst,
	TemporalInst **inter1, TemporalInst **inter2)
{
	TemporalInst *inst1 = temporalseq_at_timestamp(seq, inst->t);
	if (inst1 == NULL)
		return false;
	
	*inter1 = inst1;
	*inter2 = temporalinst_copy(inst1);
	return true;
}

bool
intersection_temporalinst_temporalseq(const TemporalInst *inst, const TemporalSeq *seq,
	TemporalInst **inter1, TemporalInst **inter2)
{
	return intersection_temporalseq_temporalinst(seq, inst, inter2, inter1);
}

/* 
 * Intersection of a TemporalSeq and a TemporalI values. Each value keeps  
 * the instants in the intersection of their time spans.
 */

bool
intersection_temporalseq_temporali(const TemporalSeq *seq, const TemporalI *ti,
	TemporalI **inter1, TemporalI **inter2)
{
	/* Test whether the bounding period of the two temporal values overlap */
	Period p;
	temporali_period(&p, ti);
	if (!overlaps_period_period_internal(&seq->period, &p))
		return false;
	
	TemporalInst **instants1 = palloc(sizeof(TemporalInst *) * ti->count);
	TemporalInst **instants2 = palloc(sizeof(TemporalInst *) * ti->count);
	int k = 0;
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		if (contains_period_timestamp_internal(&seq->period, inst->t))
		{
			instants1[k] = temporalseq_at_timestamp(seq, inst->t);
			instants2[k++] = inst;
		}
		if (seq->period.upper < inst->t)
			break;
	}
	if (k == 0)
	{
		pfree(instants1); pfree(instants2); 
		return false;
	}
	
	*inter1 = temporali_make(instants1, k);
	*inter2 = temporali_make(instants2, k);
	
	for (int i = 0; i < k; i++) 
		pfree(instants1[i]);
	pfree(instants1); pfree(instants2); 

	return true;
}

bool
intersection_temporali_temporalseq(const TemporalI *ti, const TemporalSeq *seq,
	TemporalI **inter1, TemporalI **inter2)
{
	return intersection_temporalseq_temporali(seq, ti, inter2, inter1);
}

/* 
 * Intersection two TemporalSeq values. 
 */

bool
intersection_temporalseq_temporalseq(const TemporalSeq *seq1, const TemporalSeq *seq2,
	TemporalSeq **inter1, TemporalSeq **inter2)
{
	/* Test whether the bounding period of the two temporal values overlap */
	Period *inter = intersection_period_period_internal(&seq1->period, 
		&seq2->period);
	if (inter == NULL)
		return false;
	
	*inter1 = temporalseq_at_period(seq1, inter);
	*inter2 = temporalseq_at_period(seq2, inter);
	pfree(inter);
	return true;
}

/*****************************************************************************
 * Compute the intersection, if any, of a segment of a temporal sequence and
 * a value. The functions are used to add intermediate points when lifting
 * operators. They suppose that the instants are synchronized, i.e.,
 * start1->t = start2->t and end1->t = end2->t
 * The functions only return true when there is an intersection at the
 * middle, i.e., they return false if they intersect at a bound.
 * It returns true if there is an intersection in which case it
 * also returns in the output parameters inter and t the value of the segment
 * at the intersection timestampt t. The two values inter and value are equal
 * up to the floating point approximation error. The value inter is computed
 * only if the passed argument is not NULL.
 * There is no need to add functions for DoubleN, which are used for computing
 * avg and centroid aggregates, since these computations are based on sum and
 * thus they do not need to add intermediate points.
 *****************************************************************************/

static bool
tnumberseq_intersection_value(const TemporalInst *inst1, const TemporalInst *inst2,
	Datum value, Oid valuetypid, Datum *inter, TimestampTz *t)
{
	double dvalue1 = DatumGetFloat8(temporalinst_value(inst1));
	double dvalue2 = DatumGetFloat8(temporalinst_value(inst2));
	double dvalue = datum_double(value, valuetypid);
	double min = Min(dvalue1, dvalue2);
	double max = Max(dvalue1, dvalue2);
	/* if value is to the left or to the right of the range */
	if (dvalue < min || dvalue > max)
		return false;

	double range = max - min;
	double partial = dvalue - min;
	double fraction = dvalue1 < dvalue2 ? partial / range : 1 - partial / range;
	if (fabs(fraction) < EPSILON || fabs(fraction - 1.0) < EPSILON)
		return false;
	if (inter != NULL)
		*inter = Float8GetDatum(dvalue1 + ((dvalue2 - dvalue1) * fraction));
	if (t != NULL)
		*t = inst1->t + (long) ((double) (inst2->t - inst1->t) * fraction);
	return true;
}

static bool
tpointseq_intersection_value(const TemporalInst *inst1, const TemporalInst *inst2,
	Datum value, Oid valuetypid, Datum *inter, TimestampTz *t)
{
	GSERIALIZED *gs = (GSERIALIZED *) PG_DETOAST_DATUM(value);
	if (gserialized_is_empty(gs))
	{
		POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(value));
		return false;
	}

	/* We are sure that the trajectory is a line */
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	int srid = gserialized_get_srid((GSERIALIZED *) DatumGetPointer(value1));
	POINT4D proj;
	POINT4D p1 = datum_get_point4d(value1);
	POINT4D p2 = datum_get_point4d(value2);
	POINT4D p = datum_get_point4d(value);
	closest_point_on_segment(&p, &p1, &p2, &proj);
	double distance = MOBDB_FLAGS_GET_Z(inst1->flags) ?
		distance3d_pt_pt((POINT3D *)&p, (POINT3D *)&proj) :
		distance2d_pt_pt((POINT2D *)&p, (POINT2D *)&proj);
	if (distance >= EPSILON)
		return false;
	if (inter != NULL)
	{
		LWPOINT *lwpoint = MOBDB_FLAGS_GET_Z(inst1->flags) ?
			lwpoint_make3dz(srid, proj.x, proj.y, proj.z) :
			lwpoint_make2d(srid, proj.x, proj.y);
		*inter = PointerGetDatum(geometry_serialize((LWGEOM *)lwpoint));
		lwpoint_free(lwpoint);
	}
	if (t != NULL)
	{
		double fraction = MOBDB_FLAGS_GET_Z(inst1->flags) ?
			distance3d_pt_pt((POINT3D *)&p1, (POINT3D *)&proj) / distance3d_pt_pt((POINT3D *)&p1, (POINT3D *)&p2):
			distance2d_pt_pt((POINT2D *)&p1, (POINT2D *)&proj) / distance2d_pt_pt((POINT2D *)&p1, (POINT2D *)&p2);
		*t = inst1->t + (long) ((double) (inst2->t - inst1->t) * fraction);
	}
	return true;
}

bool
tlinearseq_intersection_value(const TemporalInst *inst1, const TemporalInst *inst2,
	Datum value, Oid valuetypid, Datum *inter, TimestampTz *t)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	if (datum_eq(value, value1, inst1->valuetypid) ||
		datum_eq(value, value2, inst1->valuetypid))
		return false;
	ensure_linear_interpolation(inst1->valuetypid);
	if (inst1->valuetypid == FLOAT8OID)
		return tnumberseq_intersection_value(inst1, inst2, value,
			valuetypid, inter, t);
	else if (inst1->valuetypid == type_oid(T_GEOMETRY))
		return tpointseq_intersection_value(inst1, inst2, value,
			valuetypid, inter, t);
	else if (inst1->valuetypid == type_oid(T_GEOGRAPHY))
	{
		GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(value);
		if (gserialized_is_empty(gs))
		{
			POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(value));
			return false;
		}

		/* We are sure that the trajectory is a line */
		Datum line = geogpoint_trajectory(value1, value2);
		bool hasinter = DatumGetFloat8(call_function4(geography_distance, line,
			value, Float8GetDatum(0.0), BoolGetDatum(false))) < DIST_EPSILON;
		if (!hasinter)
		{
			pfree(DatumGetPointer(line));
			return false;
		}

		/* There is no function equivalent to LWGEOM_line_locate_point
		 * for geographies. We do as the ST_Intersection function, e.g.
		 * 'SELECT geography(ST_Transform(ST_Intersection(ST_Transform(geometry($1),
		 * @extschema@._ST_BestSRID($1, $2)),
		 * ST_Transform(geometry($2), @extschema@._ST_BestSRID($1, $2))), 4326))' */

		Datum bestsrid = call_function2(geography_bestsrid, line, line);
		Datum line1 = call_function1(geometry_from_geography, line);
		Datum line2 = call_function2(transform, line1, bestsrid);
		value1 = call_function1(geometry_from_geography, value);
		value2 = call_function2(transform, value1, bestsrid);
		double fraction = DatumGetFloat8(call_function2(LWGEOM_line_locate_point,
			line2, value2));
		pfree(DatumGetPointer(line)); pfree(DatumGetPointer(line1));
		pfree(DatumGetPointer(line2)); pfree(DatumGetPointer(value1));
		pfree(DatumGetPointer(value2));
		if (fabs(fraction) < EPSILON || fabs(fraction - 1.0) < EPSILON)
			return false;
		if (t != NULL)
			*t = inst1->t + (long) ((double) (inst2->t - inst1->t) * fraction);
		if (inter != NULL)
			*inter = call_function2(LWGEOM_line_interpolate_point, line2, value2);
		return true;
	}
	return false; /* Make compiler quiet */
}

/*****************************************************************************
 * Compute the intersection, if any, of two segments of synchronized temporal
 * sequences. It returns true if there is an intersection in which chase it
 * also returns in the output parameters inter1, inter2, and t the value of
 * the corresponding segment at the intersection timestampt t. The two values
 * inter1 and inter2 are equal up to the floating point approximation error.
 * For the temporal point case we cannot use the PostGIS function
 * lw_dist3d_seg_seg since it does not take time into consideration and
 * would return, e.g., that the two following segments
 * [Point(1 1)@t1, Point(2 2)@t2] and [Point(2 2)@t1, Point(1 1)@t2]
 * intersect at Point(1 1).
 * This function is used in particular for temporal comparisons such as
 * tfloat <comp> tfloat where <comp> is <, <=, ... since the comparison
 * changes its value before/at/after the intersection point.
 *****************************************************************************/

static bool
tnumberseq_intersection(const TemporalInst *start1, const TemporalInst *end1,
	const TemporalInst *start2, const TemporalInst *end2,
	Datum *inter1, Datum *inter2, TimestampTz *t)
{
	double x1 = datum_double(temporalinst_value(start1), start1->valuetypid);
	double x2 = datum_double(temporalinst_value(end1), start1->valuetypid);
	double x3 = datum_double(temporalinst_value(start2), start2->valuetypid);
	double x4 = datum_double(temporalinst_value(end2), start2->valuetypid);
	/* Compute the instant t at which the linear functions of the two segments
	   are equal: at + b = ct + d that is t = (d - b) / (a - c).
	   To reduce problems related to floating point arithmetic, t1 and t2
	   are shifted, respectively, to 0 and 1 before the computation */
	double denum = x2 - x1 - x4 + x3;
	if (denum == 0)
		/* Parallel segments */
		return false;

	double fraction = (x3 - x1) / denum;
	if (fraction <= EPSILON || fraction >= (1.0 - EPSILON))
		/* Intersection occurs out of the period */
		return false;

	*t = start1->t + (long) ((double) (end1->t - start1->t) * fraction);
	double x5 = x1 + ((x2 - x1) * fraction);
	double x6 = x3 + ((x4 - x3) * fraction);
	*inter1 = start1->valuetypid == INT4OID ? Int32GetDatum((int32) x5) :
		Float8GetDatum(x5);
	*inter2 = start2->valuetypid == INT4OID ? Int32GetDatum((int32) x6) :
		Float8GetDatum(x6);
	return true;
}

bool
tpointseq_intersection(const TemporalInst *start1, const TemporalInst *end1,
	const TemporalInst *start2, const TemporalInst *end2,
	Datum *inter1, Datum *inter2, TimestampTz *t)
{
	int srid = gserialized_get_srid((GSERIALIZED *) DatumGetPointer(temporalinst_value(start1)));
	double fraction, xfraction = 0, yfraction = 0, xdenum, ydenum;
	LWPOINT *lwinter1, *lwinter2;
	if (MOBDB_FLAGS_GET_Z(start1->flags)) /* 3D */
	{
		POINT3DZ p1 = datum_get_point3dz(temporalinst_value(start1));
		POINT3DZ p2 = datum_get_point3dz(temporalinst_value(end1));
		POINT3DZ p3 = datum_get_point3dz(temporalinst_value(start2));
		POINT3DZ p4 = datum_get_point3dz(temporalinst_value(end2));
		xdenum = p2.x - p1.x - p4.x + p3.x;
		ydenum = p2.y - p1.y - p4.y + p3.y;
		double zdenum = p2.z - p1.z - p4.z + p3.z;
		if (xdenum == 0 && ydenum == 0 && zdenum == 0)
			/* Parallel segments */
			return false;

		double zfraction = 0;
		if (xdenum != 0)
		{
			xfraction = (p3.x - p1.x) / xdenum;
			/* If intersection occurs out of the period */
			if (xfraction <= EPSILON || xfraction >= (1.0 - EPSILON))
				return false;
		}
		if (ydenum != 0)
		{
			yfraction = (p3.y - p1.y) / ydenum;
			/* If intersection occurs out of the period */
			if (yfraction <= EPSILON || yfraction >= (1.0 - EPSILON))
				return false;
		}
		if (zdenum != 0)
		{
			/* If intersection occurs out of the period or intersect at different timestamps */
			zfraction = (p3.z - p1.z) / zdenum;
			if (zfraction <= EPSILON || zfraction >= (1.0 - EPSILON))
				return false;
		}
		/* If intersect at different timestamps on each dimension */
		if ((xdenum != 0 && ydenum != 0 && zdenum != 0 &&
			fabs(xfraction - yfraction) > EPSILON && fabs(xfraction - zfraction) > EPSILON) ||
			(xdenum == 0 && ydenum != 0 && zdenum != 0 &&
			fabs(yfraction - zfraction) > EPSILON) ||
			(xdenum != 0 && ydenum == 0 && zdenum != 0 &&
			fabs(xfraction - zfraction) > EPSILON) ||
			(xdenum != 0 && ydenum != 0 && zdenum == 0 &&
			fabs(xfraction - yfraction) > EPSILON))
			return false;
		if (xdenum != 0)
			fraction = xfraction;
		else if (ydenum != 0)
			fraction = yfraction;
		else
			fraction = zfraction;
		/* Compute the intersection value in the two segments */
		POINT3DZ p5, p6;
		p5.x = p1.x + ((p2.x - p1.x) * fraction);
		p5.y = p1.y + ((p2.y - p1.y) * fraction);
		p5.z = p1.z + ((p2.z - p1.z) * fraction);
		p6.x = p3.x + ((p4.x - p3.x) * fraction);
		p6.y = p3.y + ((p4.y - p3.y) * fraction);
		p6.z = p3.z + ((p4.z - p3.z) * fraction);
		lwinter1 = lwpoint_make3dz(srid, p5.x, p5.y, p5.z);
		lwinter2 = lwpoint_make3dz(srid, p6.x, p6.y, p6.z);
	}
	else /* 2D */
	{
		POINT2D p1 = datum_get_point2d(temporalinst_value(start1));
		POINT2D p2 = datum_get_point2d(temporalinst_value(end1));
		POINT2D p3 = datum_get_point2d(temporalinst_value(start2));
		POINT2D p4 = datum_get_point2d(temporalinst_value(end2));
		xdenum = p2.x - p1.x - p4.x + p3.x;
		ydenum = p2.y - p1.y - p4.y + p3.y;
		if (xdenum == 0 && ydenum == 0)
			/* Parallel segments */
			return false;

		if (xdenum != 0)
		{
			xfraction = (p3.x - p1.x) / xdenum;
			/* If intersection occurs out of the period */
			if (xfraction <= EPSILON || xfraction >= (1.0 - EPSILON))
				return false;
		}
		if (ydenum != 0)
		{
			yfraction = (p3.y - p1.y) / ydenum;
			/* If intersection occurs out of the period */
			if (yfraction <= EPSILON || yfraction >= (1.0 - EPSILON))
				return false;
		}
		/* If intersect at different timestamps on each dimension */
		if (xdenum != 0 && ydenum != 0 && fabs(xfraction - yfraction) > EPSILON)
			return false;
		fraction = xdenum != 0 ? xfraction : yfraction;
		/* Compute the intersection value in the two segments */
		POINT2D p5, p6;
		p5.x = p1.x + ((p2.x - p1.x) * fraction);
		p5.y = p1.y + ((p2.y - p1.y) * fraction);
		p6.x = p3.x + ((p4.x - p3.x) * fraction);
		p6.y = p3.y + ((p4.y - p3.y) * fraction);
		lwinter1 = lwpoint_make2d(srid, p5.x, p5.y);
		lwinter2 = lwpoint_make2d(srid, p6.x, p6.y);
	}
	*inter1 = PointerGetDatum(geometry_serialize((LWGEOM *) lwinter1));
	*inter2 = PointerGetDatum(geometry_serialize((LWGEOM *) lwinter2));
	lwpoint_free(lwinter1); lwpoint_free(lwinter2);
	*t = start1->t + (long) ((double) (end1->t - start1->t) * fraction);
	return true;
}

bool
temporalseq_intersection(const TemporalInst *start1, const TemporalInst *end1, bool linear1,
	const TemporalInst *start2, const TemporalInst *end2, bool linear2,
	Datum *inter1, Datum *inter2, TimestampTz *t)
{
	bool result = false; /* Make compiler quiet */
	if (! linear1)
	{
		*inter1 = temporalinst_value(start1);
		result = tlinearseq_intersection_value(start2, end2,
			*inter1, start1->valuetypid, inter2, t);
	}
	else if (! linear2)
	{
		*inter2 = temporalinst_value(start2);
		result = tlinearseq_intersection_value(start1, end1,
			*inter2, start2->valuetypid, inter1, t);
	}
	else
	{
		/* Both segments have linear interpolation */
		ensure_temporal_base_type(start1->valuetypid);
		if ((start1->valuetypid == INT4OID || start1->valuetypid == FLOAT8OID) &&
			(start2->valuetypid == INT4OID || start2->valuetypid == FLOAT8OID))
			result = tnumberseq_intersection(start1, end1, start2, end2,
				inter1, inter2, t);
		else if (start1->valuetypid == type_oid(T_GEOMETRY))
			result = tpointseq_intersection(start1, end1, start2, end2,
				inter1, inter2, t);
		else if (start1->valuetypid == type_oid(T_GEOGRAPHY))
		{
			/* For geographies we do as the ST_Intersection function, e.g.
			 * 'SELECT geography(ST_Transform(ST_Intersection(ST_Transform(geometry($1),
			 * @extschema@._ST_BestSRID($1, $2)),
			 * ST_Transform(geometry($2), @extschema@._ST_BestSRID($1, $2))), 4326))' */
			Datum line1 = geogpoint_trajectory(temporalinst_value(start1),
				temporalinst_value(end1));
			Datum line2 = geogpoint_trajectory(temporalinst_value(start2),
				temporalinst_value(end2));
			Datum bestsrid = call_function2(geography_bestsrid, line1, line2);
			TemporalInst *start1geom1 = tgeogpointinst_to_tgeompointinst(start1);
			TemporalInst *end1geom1 = tgeogpointinst_to_tgeompointinst(end1);
			TemporalInst *start2geom1 = tgeogpointinst_to_tgeompointinst(start2);
			TemporalInst *end2geom1 = tgeogpointinst_to_tgeompointinst(end2);
			TemporalInst *start1geom2 = tpointinst_transform(start1, bestsrid);
			TemporalInst *end1geom2 = tpointinst_transform(start1, bestsrid);
			TemporalInst *start2geom2 = tpointinst_transform(start2, bestsrid);
			TemporalInst *end2geom2 = tpointinst_transform(start2, bestsrid);
			result = tpointseq_intersection(start1geom2, end1geom2,
				start2geom2, end2geom2, inter1, inter2, t);
			pfree(DatumGetPointer(line1)); pfree(DatumGetPointer(line2));
			pfree(start1geom1); pfree(end1geom1); pfree(start2geom1); pfree(end2geom1);
			pfree(start1geom2); pfree(end1geom2); pfree(start2geom2); pfree(end2geom2);
		}
	}
	return result;
}

/*****************************************************************************
 * Synchronize two TemporalSeq values. The values are split into (redundant)
 * segments defined over the same set of instants covering the intersection
 * of their time spans. Depending on the value of the argument crossings,
 * potential crossings between successive pair of instants are added.
 * Crossings are only added when at least one of the sequences has linear
 * interpolation.
 *****************************************************************************/

bool
synchronize_temporalseq_temporalseq(const TemporalSeq *seq1, const TemporalSeq *seq2,
	TemporalSeq **sync1, TemporalSeq **sync2, bool crossings)
{
	/* Test whether the bounding period of the two temporal values overlap */
	Period *inter = intersection_period_period_internal(&seq1->period, 
		&seq2->period);
	if (inter == NULL)
		return false;

	bool linear1 = MOBDB_FLAGS_GET_LINEAR(seq1->flags);
	bool linear2 = MOBDB_FLAGS_GET_LINEAR(seq2->flags);
	/* If the two sequences intersect at an instant */
	if (inter->lower == inter->upper)
	{
		TemporalInst *inst1 = temporalseq_at_timestamp(seq1, inter->lower);
		TemporalInst *inst2 = temporalseq_at_timestamp(seq2, inter->lower);
		*sync1 = temporalseq_make(&inst1, 1, true, true, linear1, false);
		*sync2 = temporalseq_make(&inst2, 1, true, true, linear2, false);
		pfree(inst1); pfree(inst2); pfree(inter);
		return true;
	}
	
	/* 
	 * General case 
	 * seq1 =  ... *     *   *   *      *>
	 * seq2 =       <*            *     * ...
	 * sync1 =      <X C * C * C X C X C *>
	 * sync1 =      <* C X C X C * C * C X>
	 * where X are values added for synchronization and C are values added
	 * for the crossings
	 */
	TemporalInst *inst1 = temporalseq_inst_n(seq1, 0);
	TemporalInst *inst2 = temporalseq_inst_n(seq2, 0);
	TemporalInst *tofreeinst = NULL;
	int i = 0, j = 0, k = 0, l = 0;
	if (inst1->t < inter->lower)
	{
		inst1 = tofreeinst = temporalseq_at_timestamp(seq1, inter->lower);
		i = temporalseq_find_timestamp(seq1, inter->lower);
	}
	else if (inst2->t < inter->lower)
	{
		inst2 = tofreeinst = temporalseq_at_timestamp(seq2, inter->lower);
		j = temporalseq_find_timestamp(seq2, inter->lower);
	}
	int count = (seq1->count - i + seq2->count - j) * 2;
	TemporalInst **instants1 = palloc(sizeof(TemporalInst *) * count);
	TemporalInst **instants2 = palloc(sizeof(TemporalInst *) * count);
	TemporalInst **tofree = palloc(sizeof(TemporalInst *) * count * 2);
	if (tofreeinst != NULL)
		tofree[l++] = tofreeinst;
	while (i < seq1->count && j < seq2->count &&
		(inst1->t <= inter->upper || inst2->t <= inter->upper))
	{
		int cmp = timestamp_cmp_internal(inst1->t, inst2->t);
		if (cmp == 0)
		{
			i++; j++;
		}
		else if (cmp < 0)
		{
			i++;
			inst2 = temporalseq_at_timestamp(seq2, inst1->t);
			tofree[l++] = inst2;
		}
		else 
		{
			j++;
			inst1 = temporalseq_at_timestamp(seq1, inst2->t);
			tofree[l++] = inst1;
		}
		/* If not the first instant add potential crossing before adding
		   the new instants */
		if (crossings && (linear1 || linear2) && k > 0)
		{
			TimestampTz crosstime;
			Datum inter1, inter2;
			if (temporalseq_intersection(instants1[k - 1], inst1, linear1,
				instants2[k - 1], inst2, linear2, &inter1, &inter2, &crosstime))
			{
				instants1[k] = tofree[l++] = temporalinst_make(inter1,
					crosstime, seq1->valuetypid);
				instants2[k++] = tofree[l++] = temporalinst_make(inter2,
					crosstime, seq2->valuetypid);
			}
		}
		instants1[k] = inst1; instants2[k++] = inst2;
		if (i == seq1->count || j == seq2->count)
			break;
		inst1 = temporalseq_inst_n(seq1, i);
		inst2 = temporalseq_inst_n(seq2, j);
	}
	/* We are sure that k != 0 due to the period intersection test above */
	/* The last two values of sequences with step interpolation and
	   exclusive upper bound must be equal */
	if (! inter->upper_inc && k > 1 && ! linear1)
	{
		if (datum_ne(temporalinst_value(instants1[k - 2]), 
			temporalinst_value(instants1[k - 1]), seq1->valuetypid))
		{
			instants1[k - 1] = temporalinst_make(temporalinst_value(instants1[k - 2]),
				instants1[k - 1]->t, instants1[k - 1]->valuetypid); 
			tofree[l++] = instants1[k - 1];
		}
	}
	if (! inter->upper_inc && k > 1 && ! linear2)
	{
		if (datum_ne(temporalinst_value(instants2[k - 2]), 
			temporalinst_value(instants2[k - 1]), seq2->valuetypid))
		{
			instants2[k - 1] = temporalinst_make(temporalinst_value(instants2[k - 2]),
				instants2[k - 1]->t, instants2[k - 1]->valuetypid); 
			tofree[l++] = instants2[k - 1];
		}
	}
	*sync1 = temporalseq_make(instants1, k, inter->lower_inc,
		inter->upper_inc, linear1, false);
	*sync2 = temporalseq_make(instants2, k, inter->lower_inc,
		inter->upper_inc, linear2, false);
	
	for (i = 0; i < l; i++)
		pfree(tofree[i]);
	pfree(instants1); pfree(instants2); pfree(tofree); pfree(inter);

	return true;
}

/*****************************************************************************
 * Input/output functions
 *****************************************************************************/

/* Convert to string */
 
char *
temporalseq_to_string(const TemporalSeq *seq, bool component, char *(*value_out)(Oid, Datum))
{
	char **strings = palloc(sizeof(char *) * seq->count);
	size_t outlen = 0;
	char str[20];
	if (! component && linear_interpolation(seq->valuetypid) && 
		!MOBDB_FLAGS_GET_LINEAR(seq->flags))
		sprintf(str, "Interp=Stepwise;");
	else
		str[0] = '\0';
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		strings[i] = temporalinst_to_string(inst, value_out);
		outlen += strlen(strings[i]) + 2;
	}
	char *result = palloc(strlen(str) + outlen + 3);
	result[outlen] = '\0';
	size_t pos = 0;
	strcpy(result, str);
	pos += strlen(str);
	result[pos++] = seq->period.lower_inc ? (char) '[' : (char) '(';
	for (int i = 0; i < seq->count; i++)
	{
		strcpy(result + pos, strings[i]);
		pos += strlen(strings[i]);
		result[pos++] = ',';
		result[pos++] = ' ';
		pfree(strings[i]);
	}
	result[pos - 2] = seq->period.upper_inc ? (char) ']' : (char) ')';
	result[pos - 1] = '\0';
	pfree(strings);
	return result;
}

/* Send function */

void
temporalseq_write(const TemporalSeq *seq, StringInfo buf)
{
	pq_sendint(buf, (uint32) seq->count, 4);
	pq_sendbyte(buf, seq->period.lower_inc ? (uint8) 1 : (uint8) 0);
	pq_sendbyte(buf, seq->period.upper_inc ? (uint8) 1 : (uint8) 0);
	pq_sendbyte(buf, MOBDB_FLAGS_GET_LINEAR(seq->flags) ? (uint8) 1 : (uint8) 0);
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		temporalinst_write(inst, buf);
	}
}

/* Receive function */

TemporalSeq *
temporalseq_read(StringInfo buf, Oid valuetypid)
{
	int count = (int) pq_getmsgint(buf, 4);
	bool lower_inc = (char) pq_getmsgbyte(buf);
	bool upper_inc = (char) pq_getmsgbyte(buf);
	bool linear = (char) pq_getmsgbyte(buf);
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * count);
	for (int i = 0; i < count; i++)
		instants[i] = temporalinst_read(buf, valuetypid);
	TemporalSeq *result = temporalseq_make(instants, count, lower_inc,
		upper_inc, linear, true);

	for (int i = 0; i < count; i++)
		pfree(instants[i]);
	pfree(instants);

	return result;
}

/*****************************************************************************
 * Cast functions
 *****************************************************************************/

/* Cast a temporal integer as a temporal float */

TemporalSeq *
tintseq_to_tfloatseq(const TemporalSeq *seq)
{
	/* It is not necessary to set the linear flag to false since it is already
	 * set by the fact that the input argument is a temporal integer */
	TemporalSeq *result = temporalseq_copy(seq);
	result->valuetypid = FLOAT8OID;
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(result, i);
		inst->valuetypid = FLOAT8OID;
		Datum *value_ptr = temporalinst_value_ptr(inst);
		*value_ptr = Float8GetDatum((double)DatumGetInt32(temporalinst_value(inst)));
	}
	return result;
}

/* Cast a temporal float with step interpolation as a temporal integer */

TemporalSeq *
tfloatseq_to_tintseq(const TemporalSeq *seq)
{
	if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Cannot cast temporal float with linear interpolation to temporal integer")));
	/* It is not necessary to set the linear flag to false since it is already
	 * set by the fact that the input argument has step interpolation */
	TemporalSeq *result = temporalseq_copy(seq);
	result->valuetypid = INT4OID;
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(result, i);
		inst->valuetypid = INT4OID;
		Datum *value_ptr = temporalinst_value_ptr(inst);
		*value_ptr = Int32GetDatum((double)DatumGetFloat8(temporalinst_value(inst)));
	}
	return result;
}

/*****************************************************************************
 * Transformation functions
 *****************************************************************************/

TemporalSeq *
temporalinst_to_temporalseq(const TemporalInst *inst, bool linear)
{
	return temporalseq_make((TemporalInst **)&inst, 1, true, true, linear, false);
}

TemporalSeq *
temporali_to_temporalseq(const TemporalI *ti, bool linear)
{
	if (ti->count != 1)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Cannot transform input to a temporal sequence")));
	TemporalInst *inst = temporali_inst_n(ti, 0);
	return temporalseq_make(&inst, 1, true, true, linear, false);
}

TemporalSeq *
temporals_to_temporalseq(const TemporalS *ts)
{
	if (ts->count != 1)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Cannot transform input to a temporal sequence")));
	return temporalseq_copy(temporals_seq_n(ts, 0));
}

/* Transform a temporal value with continuous base type from step to linear interpolation */

int
tstepseq_to_linear1(TemporalSeq **result, const TemporalSeq *seq)
{
	if (seq->count == 1)
	{
		result[0] = temporalseq_copy(seq);
		MOBDB_FLAGS_SET_LINEAR(result[0]->flags, true);
		return 1;
	}

	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	Datum value1 = temporalinst_value(inst1);
	Datum value2;
	TemporalInst *inst2;
	bool lower_inc = seq->period.lower_inc;
	int k = 0;
	for (int i = 1; i < seq->count; i++)
	{
		inst2 = temporalseq_inst_n(seq, i);
		value2 = temporalinst_value(inst2);
		TemporalInst *instants[2];
		instants[0] = inst1;
		instants[1] = temporalinst_make(value1, inst2->t, seq->valuetypid);
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc &&
			datum_eq(value1, value2, seq->valuetypid) : false;
		result[k++] = temporalseq_make(instants, 2, lower_inc, upper_inc,
			true, false);
		inst1 = inst2;
		value1 = value2;
		lower_inc = true;
		pfree(instants[1]);
	}
	if (seq->period.upper_inc)
	{
		value1 = temporalinst_value(temporalseq_inst_n(seq, seq->count - 2));
		value2 = temporalinst_value(inst2);
		if (datum_ne(value1, value2, seq->valuetypid))
			result[k++] = temporalseq_make(&inst2, 1, true, true,
				true, false);
	}
	return k;
}

TemporalS *
tstepseq_to_linear(const TemporalSeq *seq)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count);
	int count = tstepseq_to_linear1(sequences, seq);
	TemporalS *result = temporals_make(sequences, count, false);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/*****************************************************************************
 * Accessor functions
 *****************************************************************************/

/* Values of a TemporalSeq with step interpolation */

Datum *
temporalseq_values1(const TemporalSeq *seq, int *count)
{
	Datum *result = palloc(sizeof(Datum *) * seq->count);
	for (int i = 0; i < seq->count; i++)
		result[i] = temporalinst_value(temporalseq_inst_n(seq, i));
	datum_sort(result, seq->count, seq->valuetypid);
	*count = datum_remove_duplicates(result, seq->count, seq->valuetypid);
	return result;
}

ArrayType *
temporalseq_values(const TemporalSeq *seq)
{
	int count;
	Datum *values = temporalseq_values1(seq, &count);
	ArrayType *result = datumarr_to_array(values, count, seq->valuetypid);
	pfree(values);
	return result;
}

/* Range of a TemporalSeq float with linear interpolation */

RangeType *
tfloatseq_range(const TemporalSeq *seq)
{
	assert(MOBDB_FLAGS_GET_LINEAR(seq->flags));
	TBOX *box = temporalseq_bbox_ptr(seq);
	Datum min = Float8GetDatum(box->xmin);
	Datum max = Float8GetDatum(box->xmax);
	if (box->xmin == box->xmax)
		return range_make(min, max, true, true, FLOAT8OID);

	Datum start = temporalinst_value(temporalseq_inst_n(seq, 0));
	Datum end = temporalinst_value(temporalseq_inst_n(seq, seq->count - 1));
	Datum lower, upper;
	bool lower_inc, upper_inc;
	if (DatumGetFloat8(start) < DatumGetFloat8(end))
	{
		lower = start; lower_inc = seq->period.lower_inc;
		upper = end; upper_inc = seq->period.upper_inc;
	}
	else
	{
		lower = end; lower_inc = seq->period.upper_inc;
		upper = start; upper_inc = seq->period.lower_inc;
	}
	bool min_inc = DatumGetFloat8(min) < DatumGetFloat8(lower) ||
		(DatumGetFloat8(min) == DatumGetFloat8(lower) && lower_inc);
	bool max_inc = DatumGetFloat8(max) > DatumGetFloat8(upper) ||
		(DatumGetFloat8(max) == DatumGetFloat8(upper) && upper_inc);
	if (!min_inc || !max_inc)
	{
		for (int i = 1; i < seq->count - 1; i++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, i);
			if (min_inc || DatumGetFloat8(min) == DatumGetFloat8(temporalinst_value(inst)))
				min_inc = true;
			if (max_inc || DatumGetFloat8(max) == DatumGetFloat8(temporalinst_value(inst)))
				max_inc = true;
			if (min_inc && max_inc)
				break;
		}
	}
	return range_make(min, max, min_inc, max_inc, FLOAT8OID);
}

int
tfloatseq_ranges1(RangeType **result, const TemporalSeq *seq)
{
	/* Temporal float with linear interpolation */
	if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		result[0] = tfloatseq_range(seq);
		return 1;
	}

	/* Temporal float with step interpolation */
	int count;
	Datum *values = temporalseq_values1(seq, &count);
	for (int i = 0; i < count; i++)
		result[i] = range_make(values[i], values[i], true, true, FLOAT8OID);
	pfree(values);
	return count;
}

ArrayType *
tfloatseq_ranges(const TemporalSeq *seq)
{
	int count = MOBDB_FLAGS_GET_LINEAR(seq->flags) ? 1 : seq->count;
	RangeType **ranges = palloc(sizeof(RangeType *) * count);
	int count1 = tfloatseq_ranges1(ranges, seq);
	ArrayType *result = rangearr_to_array(ranges, count1, type_oid(T_FLOATRANGE));
	for (int i = 0; i < count1; i++)
		pfree(ranges[i]);
	pfree(ranges);
	return result;
}

/* Get time */

PeriodSet *
temporalseq_get_time(const TemporalSeq *seq)
{
	return period_to_periodset_internal(&seq->period);
}

/* Minimum value */

Datum
temporalseq_min_value(const TemporalSeq *seq)
{
	if (seq->valuetypid == INT4OID)
	{
		TBOX *box = temporalseq_bbox_ptr(seq);
		return Int32GetDatum((int)(box->xmin));
	}
	if (seq->valuetypid == FLOAT8OID)
	{
		TBOX *box = temporalseq_bbox_ptr(seq);
		return Float8GetDatum(box->xmin);
	}
	Datum result = temporalinst_value(temporalseq_inst_n(seq, 0));
	for (int i = 1; i < seq->count; i++)
	{
		Datum value = temporalinst_value(temporalseq_inst_n(seq, i));
		if (datum_lt(value, result, seq->valuetypid))
			result = value;
	}
	return result;
}

/* Maximum value */

Datum
temporalseq_max_value(const TemporalSeq *seq)
{
	if (seq->valuetypid == INT4OID)
	{
		TBOX *box = temporalseq_bbox_ptr(seq);
		return Int32GetDatum((int)(box->xmax));
	}
	if (seq->valuetypid == FLOAT8OID)
	{
		TBOX *box = temporalseq_bbox_ptr(seq);
		return Float8GetDatum(box->xmax);
	}
	Datum result = temporalinst_value(temporalseq_inst_n(seq, 0));
	for (int i = 1; i < seq->count; i++)
	{
		Datum value = temporalinst_value(temporalseq_inst_n(seq, i));
		if (datum_gt(value, result, seq->valuetypid))
			result = value;
	}
	return result;
}

/* Timespan */

Datum
temporalseq_timespan(const TemporalSeq *seq)
{
	Interval *result = period_timespan_internal(&seq->period);
	return PointerGetDatum(result);
}

/* Bounding period on which the temporal value is defined */

void
temporalseq_period(Period *p, const TemporalSeq *seq)
{
	period_set(p, seq->period.lower, seq->period.upper,
		seq->period.lower_inc, seq->period.upper_inc);
}

/* Instants */

TemporalInst **
temporalseq_instants(const TemporalSeq *seq)
{
	TemporalInst **result = palloc(sizeof(TemporalInst *) * seq->count);
	for (int i = 0; i < seq->count; i++)
		result[i] = temporalseq_inst_n(seq, i);
	return result;
}

ArrayType *
temporalseq_instants_array(const TemporalSeq *seq)
{
	TemporalInst **instants = temporalseq_instants(seq);
	ArrayType *result = temporalarr_to_array((Temporal **)instants, seq->count);
	pfree(instants);
	return result;
}

/* Start timestamptz */

TimestampTz
temporalseq_start_timestamp(const TemporalSeq *seq)
{
	return (temporalseq_inst_n(seq, 0))->t;
}

/* End timestamptz */

TimestampTz
temporalseq_end_timestamp(const TemporalSeq *seq)
{
	return (temporalseq_inst_n(seq, seq->count - 1))->t;
}

/* Timestamps */

TimestampTz *
temporalseq_timestamps1(const TemporalSeq *seq)
{
	TimestampTz *result = palloc(sizeof(TimestampTz) * seq->count);
	for (int i = 0; i < seq->count; i++)
		result[i] = temporalseq_inst_n(seq, i)->t;
	return result;
}

ArrayType *
temporalseq_timestamps(const TemporalSeq *seq)
{
	TimestampTz *times = temporalseq_timestamps1(seq);
	ArrayType *result = timestamparr_to_array(times, seq->count);
	pfree(times);
	return result;
}

/* Shift the time span of a temporal value by an interval */

TemporalSeq *
temporalseq_shift(const TemporalSeq *seq, const Interval *interval)
{
	TemporalSeq *result = temporalseq_copy(seq);
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = instants[i] = temporalseq_inst_n(result, i);
		inst->t = DatumGetTimestampTz(
			DirectFunctionCall2(timestamptz_pl_interval,
			TimestampTzGetDatum(inst->t), PointerGetDatum(interval)));
	}
	/* Shift period */
	result->period.lower = DatumGetTimestampTz(
			DirectFunctionCall2(timestamptz_pl_interval,
			TimestampTzGetDatum(seq->period.lower), PointerGetDatum(interval)));
	result->period.upper = DatumGetTimestampTz(
			DirectFunctionCall2(timestamptz_pl_interval,
			TimestampTzGetDatum(seq->period.upper), PointerGetDatum(interval)));
	/* Shift bounding box */
	void *bbox = temporalseq_bbox_ptr(result);
	temporal_bbox_shift(bbox, interval, seq->valuetypid);
	pfree(instants);
	return result;
}

/*****************************************************************************
 * Ever/always comparison operators
 * The functions assume that the temporal value and the datum value are of
 * the same valuetypid. Ever/always equal are valid for all temporal types
 * including temporal points. All the other comparisons are only valid for
 * temporal alphanumeric types.
 *****************************************************************************/

/* Is the temporal value ever equal to the value? */

static bool
tlinearseq_ever_eq1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool lower_inc, bool upper_inc, Datum value)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	Oid valuetypid = inst1->valuetypid;

	/* Constant segment */
	if (datum_eq(value1, value2, valuetypid) &&
		datum_eq(value1, value, valuetypid))
		return true;

	/* Test of bounds */
	if (datum_eq(value1, value, valuetypid))
		return lower_inc;
	if (datum_eq(value2, value, valuetypid))
		return upper_inc;

	/* Interpolation for continuous base type */
	return tlinearseq_intersection_value(inst1, inst2, value, valuetypid,
		NULL, NULL);
}

bool
temporalseq_ever_eq(const TemporalSeq *seq, Datum value)
{
	/* Bounding box test */
	if (seq->valuetypid == INT4OID || seq->valuetypid == FLOAT8OID)
	{
		TBOX box;
		memset(&box, 0, sizeof(TBOX));
		temporalseq_bbox(&box, seq);
		double d = datum_double(value, seq->valuetypid);
		if (d < box.xmin || box.xmax < d)
			return false;
	}

	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags) || seq->count == 1)
	{
		for (int i = 0; i < seq->count; i++)
		{
			Datum valueinst = temporalinst_value(temporalseq_inst_n(seq, i));
			if (datum_eq(valueinst, value, seq->valuetypid))
				return true;
		}
		return false;
	}

	/* Continuous base type */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		if (tlinearseq_ever_eq1(inst1, inst2, lower_inc, upper_inc, value))
			return true;
		inst1 = inst2;
		lower_inc = true;
	}
	return false;
}

/* Is the temporal value always equal to the value? */

bool
temporalseq_always_eq(const TemporalSeq *seq, Datum value)
{
	/* Bounding box test */
	if (seq->valuetypid == INT4OID || seq->valuetypid == FLOAT8OID)
	{
		TBOX box;
		memset(&box, 0, sizeof(TBOX));
		temporalseq_bbox(&box, seq);
		if (seq->valuetypid == INT4OID)
			return box.xmin == box.xmax &&
				(int)(box.xmax) == DatumGetInt32(value);
		else
			return box.xmin == box.xmax &&
				(int)(box.xmax) == DatumGetFloat8(value);
	}

	/* The following test assumes that the sequence is in normal form */
	if (seq->count > 2)
		return false;
	for (int i = 0; i < seq->count; i++)
	{
		Datum valueinst = temporalinst_value(temporalseq_inst_n(seq, i));
		if (datum_ne(valueinst, value, seq->valuetypid))
			return false;
	}
	return true;
}

/*****************************************************************************/

static bool
tempcontseq_ever_le1(Datum value1, Datum value2, Oid valuetypid,
	bool lower_inc, bool upper_inc, Datum value)
{
	/* Constant segment */
	if (datum_eq(value1, value2, valuetypid))
		return datum_le(value1, value, valuetypid);
	/* Increasing segment */
	if (datum_lt(value1, value2, valuetypid))
		return datum_lt(value1, value, valuetypid) ||
			(lower_inc && datum_eq(value1, value, valuetypid));
	/* Decreasing segment */
	return datum_lt(value2, value, valuetypid) ||
		(upper_inc && datum_eq(value2, value, valuetypid));
}

static bool
tempcontseq_always_lt1(Datum value1, Datum value2, Oid valuetypid,
	bool lower_inc, bool upper_inc, Datum value)
{
	/* Constant segment */
	if (datum_eq(value1, value2, valuetypid))
		return datum_lt(value1, value1, valuetypid);
	/* Increasing segment */
	if (datum_lt(value1, value2, valuetypid))
		return datum_lt(value2, value, valuetypid) ||
			(! upper_inc && datum_eq(value, value2, valuetypid));
	/* Decreasing segment */
	return datum_lt(value1, value, valuetypid) ||
		(! lower_inc && datum_eq(value1, value, valuetypid));
}

/*****************************************************************************/

/*
 * Is the temporal value ever less than the value?
 */

bool
temporalseq_ever_lt(const TemporalSeq *seq, Datum value)
{
	/* Bounding box test */
	if (seq->valuetypid == INT4OID || seq->valuetypid == FLOAT8OID)
	{
		TBOX box;
		memset(&box, 0, sizeof(TBOX));
		temporalseq_bbox(&box, seq);
		double d = datum_double(value, seq->valuetypid);
		if (box.xmin < d)
			return true;
		/* It is not necessary to take the bounds into account */
		return false;
	}

	/* We are sure that the type has stewpwise interpolation since
	 * there are currenty no other continuous base type besides tfloat
	 * to which the ever < comparison applies */
	assert(! MOBDB_FLAGS_GET_LINEAR(seq->flags));
	for (int i = 0; i < seq->count; i++)
	{
		Datum valueinst = temporalinst_value(temporalseq_inst_n(seq, i));
		if (datum_lt(valueinst, value, seq->valuetypid))
			return true;
	}
	return false;
}

/*
 * Is the temporal value ever less than or equal to the value?
 */

bool
temporalseq_ever_le(const TemporalSeq *seq, Datum value)
{
	/* Bounding box test */
	if (seq->valuetypid == INT4OID || seq->valuetypid == FLOAT8OID)
	{
		TBOX box;
		memset(&box, 0, sizeof(TBOX));
		temporalseq_bbox(&box, seq);
		double d = datum_double(value, seq->valuetypid);
		if (d < box.xmin)
			return false;
	}

	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags) || seq->count == 1)
	{
		for (int i = 0; i < seq->count; i++)
		{
			Datum valueinst = temporalinst_value(temporalseq_inst_n(seq, i));
			if (datum_le(valueinst, value, seq->valuetypid))
				return true;
		}
		return false;
	}

	/* Continuous base type */
	Datum value1 = temporalinst_value(temporalseq_inst_n(seq, 0));
	bool lower_inc = seq->period.lower_inc;
	for (int i = 1; i < seq->count; i++)
	{
		Datum value2 = temporalinst_value(temporalseq_inst_n(seq, i));
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		if (tempcontseq_ever_le1(value1, value2, seq->valuetypid,
			lower_inc, upper_inc, value))
			return true;
		value1 = value2;
		lower_inc = true;
	}
	return false;
}

/* Is the temporal value always less than the value? */

bool
temporalseq_always_lt(const TemporalSeq *seq, Datum value)
{
	/* Bounding box test */
	if (seq->valuetypid == INT4OID || seq->valuetypid == FLOAT8OID)
	{
		TBOX box;
		memset(&box, 0, sizeof(TBOX));
		temporalseq_bbox(&box, seq);
		double d = datum_double(value, seq->valuetypid);
		/* Minimum value may be non inclusive */
		if (d < box.xmax)
			return false;
	}

	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags) || seq->count == 1)
	{
		for (int i = 0; i < seq->count; i++)
		{
			Datum valueinst = temporalinst_value(temporalseq_inst_n(seq, i));
			if (! datum_lt(valueinst, value, seq->valuetypid))
				return false;
		}
		return true;
	}

	/* Continuous base type */
	Datum value1 = temporalinst_value(temporalseq_inst_n(seq, 0));
	bool lower_inc = seq->period.lower_inc;
	for (int i = 1; i < seq->count; i++)
	{
		Datum value2 = temporalinst_value(temporalseq_inst_n(seq, i));
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		if (! tempcontseq_always_lt1(value1, value2, seq->valuetypid,
			lower_inc, upper_inc, value))
			return false;
		value1 = value2;
		lower_inc = true;
	}
	return true;
}

/* Is the temporal value always less than or equal to the value? */

bool
temporalseq_always_le(const TemporalSeq *seq, Datum value)
{
	/* Bounding box test */
	if (seq->valuetypid == INT4OID || seq->valuetypid == FLOAT8OID)
	{
		TBOX box;
		memset(&box, 0, sizeof(TBOX));
		temporalseq_bbox(&box, seq);
		double d = datum_double(value, seq->valuetypid);
		if (d < box.xmax)
			return false;
		/* It is not necessary to take the bounds into account */
		return true;
	}

	/* We are sure that the type has stewpwise interpolation since
	 * there are currenty no other continuous base type besides tfloat
	 * to which the always <= comparison applies */
	assert(! MOBDB_FLAGS_GET_LINEAR(seq->flags));
	for (int i = 0; i < seq->count; i++)
	{
		Datum valueinst = temporalinst_value(temporalseq_inst_n(seq, i));
		if (! datum_le(valueinst, value, seq->valuetypid))
			return false;
	}
	return true;
}

/*****************************************************************************
 * Restriction Functions
 *****************************************************************************/

/* Restriction to a value for a segment */

static TemporalSeq *
temporalseq_at_value1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool linear, bool lower_inc, bool upper_inc, Datum value)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	Oid valuetypid = inst1->valuetypid;
	TemporalInst *instants[2];

	/* Constant segment (step or linear interpolation) */
	if (datum_eq(value1, value2, valuetypid))
	{
		/* If not equal to value */
		if (datum_ne(value1, value, valuetypid))
			return NULL;
		instants[0] = (TemporalInst *) inst1;
		instants[1] = (TemporalInst *) inst2;
		TemporalSeq *result = temporalseq_make(instants, 2, lower_inc,
			upper_inc, linear, false);
		return result;
	}

	/* Stepwise interpolation */
	if (! linear)
	{
		TemporalSeq *result = NULL;
		if (datum_eq(value1, value, valuetypid))
		{
			/* <value@t1 x@t2> */
			instants[0] = (TemporalInst *) inst1;
			instants[1] = temporalinst_make(value1, inst2->t, valuetypid);
			result = temporalseq_make(instants, 2, lower_inc, false,
				linear, false);
			pfree(instants[1]);
		}
		else if (upper_inc && datum_eq(value, value2, valuetypid))
		{
			/* <x@t1 value@t2] */
			result = temporalseq_make((TemporalInst **)&inst2, 1, true, true, linear, false);
		}
		return result;
	}

	/* Linear interpolation: Test of bounds */
	if (datum_eq(value1, value, valuetypid))
	{
		if (!lower_inc)
			return NULL;
		return temporalseq_make((TemporalInst **)&inst1, 1, true, true, linear, false);
	}
	if (datum_eq(value2, value, valuetypid))
	{
		if (!upper_inc)
			return NULL;
		return temporalseq_make((TemporalInst **)&inst2, 1, true, true, linear, false);
	}

	/* Continuous base type: Interpolation */
	Datum projvalue;
	TimestampTz t;
	if (! tlinearseq_intersection_value(inst1, inst2, value, valuetypid, &projvalue, &t))
		return NULL;

	TemporalInst *inst = temporalinst_make(projvalue, t, valuetypid);
	TemporalSeq *result = temporalseq_make(&inst, 1, true, true, linear, false);
	pfree(inst); DATUM_FREE(projvalue, valuetypid);
	return result;
}

/*
   Restriction to a value.
   This function is called for each sequence of a TemporalS.
*/

int
temporalseq_at_value2(TemporalSeq **result, const TemporalSeq *seq, Datum value)
{
	Oid valuetypid = seq->valuetypid;
	/* Bounding box test */
	if (valuetypid == INT4OID || valuetypid == FLOAT8OID)
	{
		TBOX box1, box2;
		memset(&box1, 0, sizeof(TBOX));
		memset(&box2, 0, sizeof(TBOX));
		temporalseq_bbox(&box1, seq);
		number_to_box(&box2, value, valuetypid);
		if (!contains_tbox_tbox_internal(&box1, &box2))
			return 0;
	}

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		if (datum_ne(temporalinst_value(inst), value, valuetypid))
			return 0;
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* General case */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	int k = 0;
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		TemporalSeq *seq1 = temporalseq_at_value1(inst1, inst2,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), lower_inc, upper_inc, value);
		if (seq1 != NULL)
			result[k++] = seq1;
		inst1 = inst2;
		lower_inc = true;
	}
	return k;
}

TemporalS *
temporalseq_at_value(const TemporalSeq *seq, Datum value)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count);
	int count = temporalseq_at_value2(sequences, seq, value);
	if (count == 0)
	{
		pfree(sequences);
		return NULL;
	}

	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/* Restriction to the complement of a value for a segment with linear interpolation. */

static int
tlinearseq_minus_value1(TemporalSeq **result,
	const TemporalInst *inst1, const TemporalInst *inst2,
	bool lower_inc, bool upper_inc, Datum value)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	Oid valuetypid = inst1->valuetypid;
	TemporalInst *instants[2];

	/* Constant segment */
	if (datum_eq(value1, value2, valuetypid))
	{
		/* Equal to value */
		if (datum_eq(value1, value, valuetypid))
			return 0;

		instants[0] = (TemporalInst *) inst1;
		instants[1] = (TemporalInst *) inst2;
		result[0] = temporalseq_make(instants, 2,
			lower_inc, upper_inc, true, false);
		return 1;
	}

	/* Test of bounds */
	if (datum_eq(value1, value, valuetypid))
	{
		instants[0] = (TemporalInst *) inst1;
		instants[1] = (TemporalInst *) inst2;
		result[0] = temporalseq_make(instants, 2, false, upper_inc,
			true, false);
		return 1;
	}
	if (datum_eq(value2, value, valuetypid))
	{
		instants[0] = (TemporalInst *) inst1;
		instants[1] = (TemporalInst *) inst2;
		result[0] = temporalseq_make(instants, 2, lower_inc, false, true, false);
		return 1;
	}

	/* Linear interpolation */
	Datum projvalue;
	TimestampTz t;
	if (!tlinearseq_intersection_value(inst1, inst2, value, valuetypid,
		&projvalue, &t))
	{
		instants[0] = (TemporalInst *) inst1;
		instants[1] = (TemporalInst *) inst2;
		result[0] = temporalseq_make(instants, 2, lower_inc, upper_inc, true, false);
		return 1;
	}
	instants[0] = (TemporalInst *) inst1;
	instants[1] = temporalinst_make(projvalue, t, valuetypid);
	result[0] = temporalseq_make(instants, 2, lower_inc, false, true, false);
	instants[0] = instants[1];
	instants[1] = (TemporalInst *) inst2;
	result[1] = temporalseq_make(instants, 2, false, upper_inc, true, false);
	pfree(instants[0]); DATUM_FREE(projvalue, valuetypid);
	return 2;
}

/*
   Restriction to the complement of a value.
   This function is called for each sequence of a TemporalS.
*/

int
temporalseq_minus_value2(TemporalSeq **result, const TemporalSeq *seq, Datum value)
{
	Oid valuetypid = seq->valuetypid;
	/* Bounding box test */
	if (valuetypid == INT4OID || valuetypid == FLOAT8OID)
	{
		TBOX box1, box2;
		memset(&box1, 0, sizeof(TBOX));
		memset(&box2, 0, sizeof(TBOX));
		temporalseq_bbox(&box1, seq);
		number_to_box(&box2, value, valuetypid);
		if (!contains_tbox_tbox_internal(&box1, &box2))
		{
			result[0] = temporalseq_copy(seq);
			return 1;
		}
	}

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		if (datum_eq(temporalinst_value(inst), value, valuetypid))
			return 0;
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* General case */
	int k = 0;
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		/* Stepwise interpolation */
		TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
		bool lower_inc = seq->period.lower_inc;
		int j = 0;
		for (int i = 0; i < seq->count; i++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, i);
			Datum value1 = temporalinst_value(inst);
			if (datum_eq(value1, value, valuetypid))
			{
				if (j > 0)
				{
					instants[j] = temporalinst_make(temporalinst_value(instants[j - 1]),
						inst->t, valuetypid);
					bool upper_inc = (i == seq->count - 2) ? seq->period.upper_inc : false;
					result[k++] = temporalseq_make(instants, j + 1, lower_inc,
						upper_inc, MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
					pfree(instants[j]);
					j = 0;
				}
				lower_inc = true;
			}
			else
				instants[j++] = inst;
		}
		if (j > 0)
			result[k++] = temporalseq_make(instants, j, lower_inc,
				seq->period.upper_inc, MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
		pfree(instants);
	}
	else
	{
		/* Linear interpolation */
		bool lower_inc = seq->period.lower_inc;
		TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
		for (int i = 1; i < seq->count; i++)
		{
			TemporalInst *inst2 = temporalseq_inst_n(seq, i);
			bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
			/* The next step adds between one and two sequences */
			k += tlinearseq_minus_value1(&result[k], inst1, inst2,
				lower_inc, upper_inc, value);
			inst1 = inst2;
			lower_inc = true;
		}
	}	
	return k;
}

/* Restriction to the complement of a value */

TemporalS *
temporalseq_minus_value(const TemporalSeq *seq, Datum value)
{
	int maxcount;
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
		maxcount = seq->count;
	else 
		maxcount = seq->count * 2;
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * maxcount);
	int count = temporalseq_minus_value2(sequences, seq, value);
	if (count == 0)
		return NULL;
	
	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/* 
 * Restriction to an array of values.
 * The function assumes that there are no duplicates values.
 * This function is called for each sequence of a TemporalS. 
 */
int
temporalseq_at_values1(TemporalSeq **result, const TemporalSeq *seq, const Datum *values, int count)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = temporalinst_at_values(inst, values, count);
		if (inst1 == NULL)
			return 0;
		
		pfree(inst1); 
		result[0] = temporalseq_copy(seq);
		return 1;
	}
	
	/* General case */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	int k = 0;	
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		for (int j = 0; j < count; j++)
		{
			TemporalSeq *seq1 = temporalseq_at_value1(inst1, inst2, 
				MOBDB_FLAGS_GET_LINEAR(seq->flags), lower_inc, upper_inc, values[j]);
			if (seq1 != NULL) 
				result[k++] = seq1;
		}
		inst1 = inst2;
		lower_inc = true;
	}
	temporalseqarr_sort(result, k);
	return k;
}
	
TemporalS *
temporalseq_at_values(const TemporalSeq *seq, const Datum *values, int count)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count * count);
	int newcount = temporalseq_at_values1(sequences, seq, values, count);
	if (newcount == 0)
	{
		pfree(sequences);
		return NULL;
	}
	TemporalS *result = temporals_make(sequences, newcount, true);
	for (int i = 0; i < newcount; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/*
 * Restriction to the complement of an array of values.
 * The function assumes that there are no duplicates values.
 * This function is called for each sequence of a TemporalS. 
 */
int
temporalseq_minus_values1(TemporalSeq **result, const TemporalSeq *seq, const Datum *values, int count)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = temporalinst_minus_values(inst, values, count);
		if (inst1 == NULL)
			return 0;
		pfree(inst1); 
		result[0] = temporalseq_copy(seq);
		return 1;
	}
	
	/* 
	 * General case
	 * Compute first the temporalseq_at_values, then compute its complement.
	 */
	TemporalS *ts = temporalseq_at_values(seq, values, count);
	if (ts == NULL)
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}
	PeriodSet *ps1 = temporals_get_time(ts);
	PeriodSet *ps2 = minus_period_periodset_internal(&seq->period, ps1);
	int newcount = 0;
	if (ps2 != NULL)
	{
		newcount = temporalseq_at_periodset1(result, seq, ps2);
		pfree(ps2);
	}
	pfree(ts); pfree(ps1); 
	return newcount;
}

TemporalS *
temporalseq_minus_values(const TemporalSeq *seq, const Datum *values, int count)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count * count * 2);
	int newcount = temporalseq_minus_values1(sequences, seq, values, count);
	if (newcount == 0)
	{
		pfree(sequences);
		return NULL;
	}
	TemporalS *result = temporals_make(sequences, newcount, true);
	for (int i = 0; i < newcount; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/* Restriction to the range for a segment */

static TemporalSeq *
tnumberseq_at_range1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool lower_incl, bool upper_incl, bool linear, RangeType *range)
{
	TypeCacheEntry *typcache = lookup_type_cache(range->rangetypid, TYPECACHE_RANGE_INFO);
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	Oid valuetypid = inst1->valuetypid;
	TemporalInst *instants[2];
	/* Discrete base type or constant segment */
	if (!linear || datum_eq(value1, value2, valuetypid))
	{
		if (!range_contains_elem_internal(typcache, range, value1)) 
			return NULL;

		instants[0] = (TemporalInst *) inst1;
		instants[1] = linear ? (TemporalInst *) inst2 :
			temporalinst_make(value1, inst2->t, valuetypid);
		TemporalSeq *result = temporalseq_make(instants, 2, lower_incl,
			upper_incl, linear, false);
		return result;
	}

	/* Ensure data type with linear interpolation */
	assert(valuetypid == FLOAT8OID);
	RangeType *valuerange = (DatumGetFloat8(value1) < DatumGetFloat8(value2)) ?
		range_make(value1, value2, lower_incl, upper_incl, FLOAT8OID) :
		range_make(value2, value1, upper_incl, lower_incl, FLOAT8OID);	
	RangeType *intersect = DatumGetRangeTypeP(call_function2(range_intersect, 
		RangeTypePGetDatum(valuerange), RangeTypePGetDatum(range)));
	if (RangeIsEmpty(intersect))
	{
		pfree(valuerange);
		pfree(intersect);
		return NULL;
	}

	TemporalSeq *result;
	Datum lowervalue = lower_datum(intersect);
	Datum uppervalue = upper_datum(intersect);
	/* Intersection range is a single value */
	if (datum_eq(lowervalue, uppervalue, valuetypid))
	{
		/* Test with inclusive bounds */
		TemporalSeq *newseq = temporalseq_at_value1(inst1, inst2, 
			linear, true, true, lowervalue);
		/* We are sure that newseq is an instant sequence */
		TemporalInst *inst = temporalseq_inst_n(newseq, 0);
		result = temporalseq_make(&inst, 1, true, true, linear, false);
		pfree(newseq); 
	}
	else
	{
		/* Test with inclusive bounds */
		TemporalSeq *newseq1 = temporalseq_at_value1(inst1, inst2, 
			linear, true, true, lowervalue);
		TemporalSeq *newseq2 = temporalseq_at_value1(inst1, inst2, 
			linear, true, true, uppervalue);
		TimestampTz time1 = newseq1->period.lower;
		TimestampTz time2 = newseq2->period.upper;
		/* We are sure that both newseq1 and newseq2 are instant sequences */
		bool lower_incl1, upper_incl1;
		if (time1 < time2)
		{
			/* Segment increasing in value */
			instants[0] = temporalseq_inst_n(newseq1, 0);
			instants[1] = temporalseq_inst_n(newseq2, 0);
			lower_incl1 = (time1 == inst1->t) ?
				lower_incl && lower_inc(intersect) : lower_inc(intersect);
			upper_incl1 = (time2 == inst2->t) ?
				upper_incl && upper_inc(intersect) : upper_inc(intersect);
		}
		else
		{
			/* Segment decreasing in value */
			instants[0] = temporalseq_inst_n(newseq2, 0);
			instants[1] = temporalseq_inst_n(newseq1, 0);
			lower_incl1 = (time2 == inst1->t) ?
				lower_incl && upper_inc(intersect) : upper_inc(intersect);
			upper_incl1 = (time1 == inst1->t) ?
				upper_incl && lower_inc(intersect) : lower_inc(intersect);
		}
		result = temporalseq_make(instants, 2, lower_incl1, upper_incl1,
			linear, false);
		pfree(newseq1); pfree(newseq2); 
	}
	pfree(valuerange); pfree(intersect); 

	return result;
}

/*
 * Restriction to the range.
 * This function is called for each sequence of a TemporalS.
 */
int 
tnumberseq_at_range2(TemporalSeq **result, const TemporalSeq *seq, RangeType *range)
{
	/* Bounding box test */
	TBOX box1, box2;
	memset(&box1, 0, sizeof(TBOX));
	memset(&box2, 0, sizeof(TBOX));
	temporalseq_bbox(&box1, seq);
	range_to_tbox_internal(&box2, range);
	if (!overlaps_tbox_tbox_internal(&box1, &box2))
		return 0;

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* General case */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	int k = 0;
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		TemporalSeq *seq1 = tnumberseq_at_range1(inst1, inst2, 
			lower_inc, upper_inc, MOBDB_FLAGS_GET_LINEAR(seq->flags), range);
		if (seq1 != NULL) 
			result[k++] = seq1;
		inst1 = inst2;
		lower_inc = true;
	}
	return k;
}

TemporalS *
tnumberseq_at_range(const TemporalSeq *seq, RangeType *range)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count);
	int count = tnumberseq_at_range2(sequences, seq, range);
	if (count == 0)
		return NULL;

	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}
	
/*
 * Restriction to the complement of a range.
 * This function is called for each sequence of a TemporalS.
 */
int
tnumberseq_minus_range1(TemporalSeq **result, const TemporalSeq *seq, RangeType *range)
{
	/* Bounding box test */
	TBOX box1, box2;
	memset(&box1, 0, sizeof(TBOX));
	memset(&box2, 0, sizeof(TBOX));
	temporalseq_bbox(&box1, seq);
	range_to_tbox_internal(&box2, range);
	if (!overlaps_tbox_tbox_internal(&box1, &box2))
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* Instantaneous sequence */
	if (seq->count == 1)
		return 0;

	/*
	 * General case
	 * Compute first tnumberseq_at_range, then compute its complement.
	 */
	TemporalS *ts = tnumberseq_at_range(seq, range);
	if (ts == NULL)
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}
	PeriodSet *ps1 = temporals_get_time(ts);
	PeriodSet *ps2 = minus_period_periodset_internal(&seq->period, ps1);
	int count = 0;
	if (ps2 != NULL)
	{
		count = temporalseq_at_periodset1(result, seq, ps2);
		pfree(ps2);
	}
	
	pfree(ts); pfree(ps1); 

	return count;
}

TemporalS *
tnumberseq_minus_range(const TemporalSeq *seq, RangeType *range)
{
	int maxcount;
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
		maxcount = seq->count;
	else 
		maxcount = seq->count * 2;
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * maxcount);
	int count = tnumberseq_minus_range1(sequences, seq, range);
	if (count == 0)
	{
		pfree(sequences);
		return NULL;
	}

	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/* 
 * Restriction to an array of ranges 
 * This function is called for each sequence of a TemporalS.
 */
int
tnumberseq_at_ranges1(TemporalSeq **result, const TemporalSeq *seq,
	RangeType **normranges, int count)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = tnumberinst_at_ranges(inst, normranges, count);
		if (inst1 == NULL)
			return 0;
		pfree(inst1); 
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* General case */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	int k = 0;	
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		bool upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		for (int j = 0; j < count; j++)
		{
			TemporalSeq *seq1 = tnumberseq_at_range1(inst1, inst2, 
				lower_inc, upper_inc, MOBDB_FLAGS_GET_LINEAR(seq->flags), normranges[j]);
			if (seq1 != NULL) 
				result[k++] = seq1;
		}
		inst1 = inst2;
		lower_inc = true;
	}
	if (k == 0) 
		return 0;
	
	temporalseqarr_sort(result, k);
	return k;
}

TemporalS *
tnumberseq_at_ranges(const TemporalSeq *seq, RangeType **normranges, int count)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count * count);
	int newcount = tnumberseq_at_ranges1(sequences, seq, normranges, count);
	if (newcount == 0)
	{
		pfree(sequences);
		return NULL;
	}
	TemporalS *result = temporals_make(sequences, newcount, true);
	for (int i = 0; i < newcount; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/*
 * Restriction to the complement of an array of ranges.
 * This function is called for each sequence of a TemporalS.
 */
int 
tnumberseq_minus_ranges1(TemporalSeq **result, const TemporalSeq *seq, RangeType **normranges, int count)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = tnumberinst_minus_ranges(inst, normranges, count);
		if (inst1 == NULL)
			return 0;

		pfree(inst1); 
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/*  
	 * General case
	 * Compute first the tnumberseq_at_ranges, then compute its complement.
	 */
	TemporalS *ts = tnumberseq_at_ranges(seq, normranges, count);
	if (ts == NULL)
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}
	PeriodSet *ps1 = temporals_get_time(ts);
	PeriodSet *ps2 = minus_period_periodset_internal(&seq->period, ps1);
	int newcount = 0;
	if (ps2 != NULL)
	{
		newcount = temporalseq_at_periodset1(result, seq, ps2);
		pfree(ps2);
	}
	pfree(ts); pfree(ps1); 
	return newcount;
}	

TemporalS *
tnumberseq_minus_ranges(const TemporalSeq *seq, RangeType **normranges, int count)
{
	int maxcount;
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
		maxcount = seq->count * count;
	else 
		maxcount = seq->count * count * 2;
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * maxcount);
	int newcount = tnumberseq_minus_ranges1(sequences, seq, normranges, 
		count);
	if (newcount == 0) 
		return NULL;
	
	TemporalS *result = temporals_make(sequences, newcount, true);
	for (int i = 0; i < newcount; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/* Restriction to the minimum value */

int
temporalseq_at_minmax(TemporalSeq **result, const TemporalSeq *seq, Datum value)
{
	int count = temporalseq_at_value2(result, seq, value);
	/* If minimum/maximum is at an exclusive bound */
	if (count == 0)
	{
		TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
		TemporalInst *inst2 = temporalseq_inst_n(seq, seq->count - 1);
		Datum value1 = temporalinst_value(inst1);
		Datum value2 = temporalinst_value(inst2);
		if (datum_eq(value, value1, seq->valuetypid) &&
			datum_eq(value, value2, seq->valuetypid))
		{
			result[0] = temporalseq_make(&inst1, 1, true, true, 
				MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
			result[1] = temporalseq_make(&inst2, 1, true, true, 
				MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
			count = 2;
		}
		else if (datum_eq(value, value1, seq->valuetypid))
		{
			result[0] = temporalseq_make(&inst1, 1, true, true, 
				MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
			count = 1;
		}
		else if (datum_eq(value, value2, seq->valuetypid))
		{
			result[0] = temporalseq_make(&inst2, 1, true, true, 
				MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
			count = 1;
		}
	}
	return count;
}

TemporalS *
temporalseq_at_min(const TemporalSeq *seq)
{
	Datum min = temporalseq_min_value(seq);
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count);
	int count = temporalseq_at_minmax(sequences, seq, min);
	TemporalS *result = temporals_make(sequences, count, false);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/* Restriction to the complement of the minimum value */

TemporalS *
temporalseq_minus_min(const TemporalSeq *seq)
{
	Datum min = temporalseq_min_value(seq);
	return temporalseq_minus_value(seq, min);
}

/* Restriction to the maximum value */

TemporalS *
temporalseq_at_max(const TemporalSeq *seq)
{
	Datum max = temporalseq_max_value(seq);
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count);
	int count = temporalseq_at_minmax(sequences, seq, max);
	TemporalS *result = temporals_make(sequences, count, false);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}
 
/* Restriction to the complement of the maximum value */

TemporalS *
temporalseq_minus_max(const TemporalSeq *seq)
{
	Datum max = temporalseq_max_value(seq);
	return temporalseq_minus_value(seq, max);
}

/*
 * Value that the temporal sequence takes at the timestamp.
 * The function supposes that the timestamp t is between inst1->t and inst2->t
 * (both inclusive). The function creates a new value that must be freed.
 */
Datum
temporalseq_value_at_timestamp1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool linear, TimestampTz t)
{
	Oid valuetypid = inst1->valuetypid;
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	/* Constant segment or t is equal to lower bound or step interpolation */
	if (datum_eq(value1, value2, valuetypid) ||
		inst1->t == t || (! linear && t < inst2->t))
		return temporalinst_value_copy(inst1);

	/* t is equal to upper bound */
	if (inst2->t == t)
		return temporalinst_value_copy(inst2);
	
	/* Interpolation for types with linear interpolation */
	double ratio = (double) (t - inst1->t) / (double) (inst2->t - inst1->t);
	Datum result = 0;
	ensure_linear_interpolation_all(valuetypid);
	if (valuetypid == FLOAT8OID)
	{ 
		double start = DatumGetFloat8(value1);
		double end = DatumGetFloat8(value2);
		double dresult = start + (end - start) * ratio;
		result = Float8GetDatum(dresult);
	}
	else if (valuetypid == type_oid(T_DOUBLE2))
	{
		double2 *start = DatumGetDouble2P(value1);
		double2 *end = DatumGetDouble2P(value2);
		double2 *dresult = palloc(sizeof(double2));
		dresult->a = start->a + (end->a - start->a) * ratio;
		dresult->b = start->b + (end->b - start->b) * ratio;
		result = Double2PGetDatum(dresult);
	}
	else if (valuetypid == type_oid(T_GEOMETRY))
	{
		result = seg_interpolate_point(value1, value2, ratio);
	}
	else if (valuetypid == type_oid(T_GEOGRAPHY))
	{
		/* We are sure that the trajectory is a line */
		Datum line = geogpoint_trajectory(value1, value2);
		/* There is no function equivalent to LWGEOM_line_interpolate_point
		 * for geographies. We do as the ST_Intersection function, e.g.
		 * 'SELECT geography(ST_Transform(ST_Intersection(ST_Transform(geometry($1),
		 * @extschema@._ST_BestSRID($1, $2)),
		 * ST_Transform(geometry($2), @extschema@._ST_BestSRID($1, $2))), 4326))' */
		Datum bestsrid = call_function2(geography_bestsrid, line, line);
		Datum line1 = call_function1(geometry_from_geography, line);
		Datum line2 = call_function2(transform, line1, bestsrid);
		Datum point = call_function2(LWGEOM_line_interpolate_point,
			line2, Float8GetDatum(ratio));
		Datum srid = call_function1(LWGEOM_get_srid, value1);
		Datum point1 = call_function2(transform, point, srid);
		result = call_function1(geography_from_geometry, point1);
		pfree(DatumGetPointer(line)); pfree(DatumGetPointer(line1));
		pfree(DatumGetPointer(line2)); pfree(DatumGetPointer(point));
		/* Cannot pfree(DatumGetPointer(point1)); */
	}
	else if (valuetypid == type_oid(T_DOUBLE3))
	{
		double3 *start = DatumGetDouble3P(value1);
		double3 *end = DatumGetDouble3P(value2);
		double3 *dresult = palloc(sizeof(double3));
		dresult->a = start->a + (end->a - start->a) * ratio;
		dresult->b = start->b + (end->b - start->b) * ratio;
		dresult->c = start->c + (end->c - start->c) * ratio;
		result = Double3PGetDatum(dresult);
	}
	else if (valuetypid == type_oid(T_DOUBLE4))
	{
		double4 *start = DatumGetDouble4P(value1);
		double4 *end = DatumGetDouble4P(value2);
		double4 *dresult = palloc(sizeof(double4));
		dresult->a = start->a + (end->a - start->a) * ratio;
		dresult->b = start->b + (end->b - start->b) * ratio;
		dresult->c = start->c + (end->c - start->c) * ratio;
		dresult->d = start->d + (end->d - start->d) * ratio;
		result = Double4PGetDatum(dresult);
	}
	return result;
}

/*
 * Value at a timestamp.
 */
bool
temporalseq_value_at_timestamp(const TemporalSeq *seq, TimestampTz t, Datum *result)
{
	/* Bounding box test */
	if (!contains_period_timestamp_internal(&seq->period, t))
		return false;

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		*result = temporalinst_value_copy(temporalseq_inst_n(seq, 0));
		return true;
	}

	/* General case */
	int n = temporalseq_find_timestamp(seq, t);
	TemporalInst *inst1 = temporalseq_inst_n(seq, n);
	TemporalInst *inst2 = temporalseq_inst_n(seq, n + 1);
	*result = temporalseq_value_at_timestamp1(inst1, inst2, MOBDB_FLAGS_GET_LINEAR(seq->flags), t);
	return true;
}

/* 
 * Restriction to a timestamp.
 * The function supposes that the timestamp t is between inst1->t and inst2->t
 * (both inclusive).
 */
TemporalInst *
temporalseq_at_timestamp1(const TemporalInst *inst1, const TemporalInst *inst2,
	TimestampTz t, bool linear)
{
	Datum value = temporalseq_value_at_timestamp1(inst1, inst2, linear, t);
	TemporalInst *result = temporalinst_make(value, t, inst1->valuetypid);
	DATUM_FREE(value, inst1->valuetypid);
	return result;
}

/*
 * Restriction to a timestamp.
 */
TemporalInst *
temporalseq_at_timestamp(const TemporalSeq *seq, TimestampTz t)
{
	/* Bounding box test */
	if (!contains_period_timestamp_internal(&seq->period, t))
		return NULL;

	/* Instantaneous sequence */
	if (seq->count == 1)
		return temporalinst_copy(temporalseq_inst_n(seq, 0));
	
	/* General case */
	int n = temporalseq_find_timestamp(seq, t);
	TemporalInst *inst1 = temporalseq_inst_n(seq, n);
	TemporalInst *inst2 = temporalseq_inst_n(seq, n + 1);
	return temporalseq_at_timestamp1(inst1, inst2, t, MOBDB_FLAGS_GET_LINEAR(seq->flags));
}

/*
 * Restriction to the complement of a timestamp.
 * This function is called for each sequence of a TemporalS.
 */
int
temporalseq_minus_timestamp1(TemporalSeq **result, const TemporalSeq *seq,
	TimestampTz t)
{
	/* Bounding box test */
	if (!contains_period_timestamp_internal(&seq->period, t))
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* Instantaneous sequence */
	if (seq->count == 1)
		return 0;
	
	/* General case */
	bool linear = MOBDB_FLAGS_GET_LINEAR(seq->flags);
	TemporalInst **instants = palloc0(sizeof(TemporalInst *) * seq->count);
	int k = 0;
	int n = temporalseq_find_timestamp(seq, t);
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0), *inst2;
	/* Compute the first sequence until t */
	if (n != 0 || inst1->t < t)
	{
		for (int i = 0; i < n; i++)
			instants[i] = temporalseq_inst_n(seq, i);
		inst1 = temporalseq_inst_n(seq, n);
		inst2 = temporalseq_inst_n(seq, n + 1);
		if (inst1->t == t)
		{
			if (linear)
			{
				instants[n] = inst1;
				result[k++] = temporalseq_make(instants, n + 1, 
					seq->period.lower_inc, false, linear, false);
			}
			else
			{
				instants[n] = temporalinst_make(temporalinst_value(instants[n - 1]), t,
					inst1->valuetypid);
				result[k++] = temporalseq_make(instants, n + 1, 
					seq->period.lower_inc, false, linear, false);
				pfree(instants[n]);
			}
		}
		else
		{
			/* inst1->t < t */
			instants[n] = inst1;
			instants[n + 1] = linear ?
				temporalseq_at_timestamp1(inst1, inst2, t, true) :
				temporalinst_make(temporalinst_value(inst1), t,
					inst1->valuetypid);
			result[k++] = temporalseq_make(instants, n + 2, 
				seq->period.lower_inc, false, linear, false);
			pfree(instants[n + 1]);
		}
	}
	/* Compute the second sequence after t */
	inst1 = temporalseq_inst_n(seq, n);
	inst2 = temporalseq_inst_n(seq, n + 1);
	if (t < inst2->t)
	{
		instants[0] = temporalseq_at_timestamp1(inst1, inst2, t, linear);
		for (int i = 1; i < seq->count - n; i++)
			instants[i] = temporalseq_inst_n(seq, i + n);
		result[k++] = temporalseq_make(instants, seq->count - n, 
			false, seq->period.upper_inc, linear, false);
		pfree(instants[0]);
	}
	return k;
}

/*
 * Restriction to the complement of a timestamp.
 */

TemporalS *
temporalseq_minus_timestamp(const TemporalSeq *seq, TimestampTz t)
{
	TemporalSeq *sequences[2];
	int count = temporalseq_minus_timestamp1((TemporalSeq **)sequences, seq, t);
	if (count == 0)
		return NULL;
	TemporalS *result = temporals_make(sequences, count, false);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	return result;
}

/* 
 * Restriction to a timestampset.
 */
TemporalI *
temporalseq_at_timestampset(const TemporalSeq *seq, const TimestampSet *ts)
{
	/* Bounding box test */
	Period *p = timestampset_bbox(ts);
	if (!overlaps_period_period_internal(&seq->period, p))
		return NULL;
	
	/* Instantaneous sequence */
	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	if (seq->count == 1)
	{
		if (!contains_timestampset_timestamp_internal(ts, inst->t))
			return NULL;
		return temporali_make(&inst, 1);
	}

	/* General case */
	TimestampTz t = Max(seq->period.lower, p->lower);
	int n;
	timestampset_find_timestamp(ts, t, &n);
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * (ts->count - n));
	int k = 0;
	for (int i = n; i < ts->count; i++) 
	{
		t = timestampset_time_n(ts, i);
		inst = temporalseq_at_timestamp(seq, t);
		if (inst != NULL)
			instants[k++] = inst;
	}
	if (k == 0)
	{
		pfree(instants);
		return NULL;
	}

	TemporalI *result = temporali_make(instants, k);
	for (int i = 0; i < k; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

int
temporalseq_minus_timestampset1(TemporalSeq **result, const TemporalSeq *seq,
	const TimestampSet *ts)
{
	/* Bounding box test */
	Period *p = timestampset_bbox(ts);
	if (!overlaps_period_period_internal(&seq->period, p))
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		if (contains_timestampset_timestamp_internal(ts,inst->t))
			return 0;
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* Instantaneous timestamp set */
	if (ts->count == 1)
	{
		return temporalseq_minus_timestamp1(result, seq,
			timestampset_time_n(ts, 0));
	}

	/* General case */
	bool linear = MOBDB_FLAGS_GET_LINEAR(seq->flags);
	TemporalInst **instants = palloc0(sizeof(TemporalInst *) * seq->count);
	TemporalInst *inst;
	int k = 0,	/* current number of new sequences */
		l = 0,	/* current instant of the argument sequence */
		m = 0,	/* current timestamp of the argument timestamp set */
		n = 0;	/* number of instants in the currently constructed sequence */
	bool lower_inc = seq->period.lower_inc;
	while (l < seq->count && m < ts->count)
	{
		bool upper_inc = (l == seq->count - 1) ? seq->period.upper_inc : false;
		inst = temporalseq_inst_n(seq, l);
		TimestampTz t = timestampset_time_n(ts, m);
		if (inst->t < t)
		{
			instants[n++] = inst;
			l++; /* advance instants */
		}
		else if (inst->t == t)
		{
			if (n > 0)
			{
				if (linear)
				{
					instants[n] = inst;
					result[k++] = temporalseq_make(instants, n + 1,
						lower_inc, false, linear, false);
				}
				else
				{
					instants[n] = temporalinst_make(temporalinst_value(instants[n - 1]), t,
						inst->valuetypid);
					result[k++] = temporalseq_make(instants, n + 1,
						lower_inc, false, linear, false);
					pfree(instants[n]);
				}
				n = 0;
			}
			lower_inc = false;
			m++; /* advance timestamps */
		}
		else
		{
			/* inst->t > t */
			if (n > 0)
			{
				instants[n] = linear ?
					temporalseq_at_timestamp1(instants[n - 1], inst, t, true) :
					temporalinst_make(temporalinst_value(instants[n - 1]), t,
						inst->valuetypid);
				result[k++] = temporalseq_make(instants, n + 1,
					lower_inc, upper_inc, linear, false);
				pfree(instants[n]);
				n = 0;
				lower_inc = false;

			}
			m++; /* advance timestamps */
		}
	}
	/* Compute the sequence after the timestamp set */
	if (l < seq->count - 1)
	{
		for (int i = l; i < seq->count; i++)
			instants[n++] = temporalseq_inst_n(seq, i);
		result[k++] = temporalseq_make(instants, n,
			false, seq->period.upper_inc, linear, false);
	}
	return k;
}

TemporalS *
temporalseq_minus_timestampset(const TemporalSeq *seq, const TimestampSet *ts)
{
	TemporalSeq **sequences = palloc0(sizeof(TemporalSeq *) * (ts->count + 1));
	int count = temporalseq_minus_timestampset1(sequences, seq, ts);
	if (count == 0)
		return NULL;

	TemporalS *result = temporals_make(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

/*
 * Restriction to a period.
 */
TemporalSeq *
temporalseq_at_period(const TemporalSeq *seq, const Period *p)
{
	/* Bounding box test */
	if (!overlaps_period_period_internal(&seq->period, p))
		return NULL;

	/* Instantaneous sequence */
	if (seq->count == 1)
		return temporalseq_copy(seq);

	/* General case */
	Period *inter = intersection_period_period_internal(&seq->period, p);
	/* Intersecting period is instantaneous */
	if (inter->lower == inter->upper)
	{
		TemporalInst *inst = temporalseq_at_timestamp(seq, inter->lower);
		TemporalSeq *result = temporalseq_make(&inst, 1, true, true,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
		pfree(inst); pfree(inter);
		return result;		
	}
	
	int n = temporalseq_find_timestamp(seq, inter->lower);
	/* If the lower bound of the intersecting period is exclusive */
	if (n == -1)
		n = 0;
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * (seq->count - n));
	/* Compute the value at the beginning of the intersecting period */
	TemporalInst *inst1 = temporalseq_inst_n(seq, n);
	TemporalInst *inst2 = temporalseq_inst_n(seq, n + 1);
	instants[0] = temporalseq_at_timestamp1(inst1, inst2, inter->lower,
		MOBDB_FLAGS_GET_LINEAR(seq->flags));
	int k = 1;
	for (int i = n + 2; i < seq->count; i++)
	{
		/* If the end of the intersecting period is between inst1 and inst2 */
		if (inst1->t <= inter->upper && inter->upper <= inst2->t)
			break;

		inst1 = inst2;
		inst2 = temporalseq_inst_n(seq, i);
		/* If the intersecting period contains inst1 */
		if (inter->lower <= inst1->t && inst1->t <= inter->upper)
			instants[k++] = inst1;
	}
	/* The last two values of sequences with step interpolation and
	   exclusive upper bound must be equal */
	if (MOBDB_FLAGS_GET_LINEAR(seq->flags) || inter->upper_inc)
		instants[k++] = temporalseq_at_timestamp1(inst1, inst2, inter->upper,
			MOBDB_FLAGS_GET_LINEAR(seq->flags));
	else
	{	
		Datum value = temporalinst_value(instants[k - 1]);
		instants[k++] = temporalinst_make(value, inter->upper, seq->valuetypid);
	}
	/* Since by definition the sequence is normalized it is not necessary to
	   normalize the projection of the sequence to the period */
	TemporalSeq *result = temporalseq_make(instants, k, inter->lower_inc,
		inter->upper_inc, MOBDB_FLAGS_GET_LINEAR(seq->flags), false);

	pfree(instants[0]); pfree(instants[k - 1]); pfree(instants); pfree(inter);
	
	return result;
}

/*
 * Restriction to the complement of a period.
 */
int
temporalseq_minus_period1(TemporalSeq **result, const TemporalSeq *seq, const Period *p)
{
	/* Bounding box test */
	if (!overlaps_period_period_internal(&seq->period, p))
	{
		result[0] = temporalseq_copy(seq);
		return 1;
	}
	
	/* Instantaneous sequence */
	if (seq->count == 1)
		return 0;

	/* General case */
	PeriodSet *ps = minus_period_period_internal(&seq->period, p);
	if (ps == NULL)
		return 0;
	for (int i = 0; i < ps->count; i++)
	{
		Period *p1 = periodset_per_n(ps, i);
		result[i] = temporalseq_at_period(seq, p1);
	}
	pfree(ps);
	return ps->count;
}

TemporalS *
temporalseq_minus_period(const TemporalSeq *seq, const Period *p)
{
	TemporalSeq *sequences[2];
	int count = temporalseq_minus_period1(sequences, seq, p);
	if (count == 0)
		return NULL;
	TemporalS *result = temporals_make(sequences, count, false);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	return result;
}

/*
 * Restriction to a periodset.
 * This function is called for each sequence of a TemporalS.
 */

int
temporalseq_at_periodset1(TemporalSeq **result, const TemporalSeq *seq, const PeriodSet *ps)
{
	/* Bounding box test */
	Period *p = periodset_bbox(ps);
	if (!overlaps_period_period_internal(&seq->period, p))
		return 0;

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		if (!contains_periodset_timestamp_internal(ps, inst->t))
			return 0;
		result[0] = temporalseq_copy(seq);
		return 1;
	}

	/* General case */
	int n;
	periodset_find_timestamp(ps, seq->period.lower, &n);
	int k = 0;
	for (int i = n; i < ps->count; i++)
	{
		p = periodset_per_n(ps, i);
		TemporalSeq *seq1 = temporalseq_at_period(seq, p);
		if (seq1 != NULL)
			result[k++] = seq1;
		if (seq->period.upper < p->upper)
			break;
	}
	return k;
}

TemporalSeq **
temporalseq_at_periodset2(const TemporalSeq *seq, const PeriodSet *ps, int *count)
{
	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * ps->count);
	*count = temporalseq_at_periodset1(result, seq, ps);
	if (*count == 0)
	{
		pfree(result);
		return NULL;
	}
	return result;
}

TemporalS *
temporalseq_at_periodset(const TemporalSeq *seq, const PeriodSet *ps)
{
	int count;
	TemporalSeq **sequences = temporalseq_at_periodset2(seq, ps, &count);
	if (count == 0)
		return NULL;
	
	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/*
 * Restriction to the complement of a periodset.
 */

int
temporalseq_minus_periodset1(TemporalSeq **result, const TemporalSeq *seq, const PeriodSet *ps,
	int from, int count)
{
	/* The sequence can be split at most into (count + 1) sequences
		|----------------------|
			|---| |---| |---|
	*/
	TemporalSeq *curr = temporalseq_copy(seq);
	int k = 0;
	for (int i = from; i < count; i++)
	{
		Period *p1 = periodset_per_n(ps, i);
		/* If the remaining periods are to the left of the current period */
		// if (period_cmp_bounds(curr->period.upper, p1->lower, false, true,
		//		curr->period.upper_inc, p1->lower_inc) < 0)
		int cmp = timestamp_cmp_internal(curr->period.upper, p1->lower);
		if (cmp < 0 || (cmp == 0 && curr->period.upper_inc && ! p1->lower_inc))
		{
			result[k++] = curr;
			break;
		}
		TemporalSeq *minus[2];
		int countminus = temporalseq_minus_period1(minus, curr, p1);
		pfree(curr);
		/* minus can have from 0 to 2 periods */
		if (countminus == 0)
			break;
		else if (countminus == 1)
			curr = minus[0];
		else /* countminus == 2 */
		{
			result[k++] = minus[0];
			curr = minus[1];
		}
		/* There are no more periods left */
		if (i == count - 1)
			result[k++] = curr;
	}
	return k;
}

TemporalS *
temporalseq_minus_periodset(const TemporalSeq *seq, const PeriodSet *ps)
{
	/* Bounding box test */
	Period *p = periodset_bbox(ps);
	if (!overlaps_period_period_internal(&seq->period, p))
		return temporalseq_to_temporals(seq);

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		if (contains_periodset_timestamp_internal(ps, inst->t))
			return NULL;
		return temporalseq_to_temporals(seq);
	}

	/* General case */
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * (ps->count + 1));
	int count = temporalseq_minus_periodset1(sequences, seq, ps,
		0, ps->count);
	if (count == 0)
	{
		pfree(sequences);
		return NULL;
	}

	TemporalS *result =temporals_make(sequences, count, false);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/*****************************************************************************
 * Intersects functions 
 *****************************************************************************/

/* Does the temporal value intersects the timestamp? */

bool
temporalseq_intersects_timestamp(const TemporalSeq *seq, TimestampTz t)
{
	return contains_period_timestamp_internal(&seq->period, t);
}

/* Does the temporal value intersects the timestamp set? */
bool
temporalseq_intersects_timestampset(const TemporalSeq *seq, const TimestampSet *ts)
{
	for (int i = 0; i < ts->count; i++)
		if (temporalseq_intersects_timestamp(seq, timestampset_time_n(ts, i))) 
			return true;
	return false;
}

/* Does the temporal value intersects the period? */

bool
temporalseq_intersects_period(const TemporalSeq *seq, const Period *p)
{
	return overlaps_period_period_internal(&seq->period, p);
}

/* Does the temporal value intersects the period set? */

bool
temporalseq_intersects_periodset(const TemporalSeq *seq, const PeriodSet *ps)
{
	for (int i = 0; i < ps->count; i++)
		if (temporalseq_intersects_period(seq, periodset_per_n(ps, i))) 
			return true;
	return false;
}

/*****************************************************************************
 * Local aggregate functions 
 *****************************************************************************/

/* Integral of the temporal numbers with step interpolation */

static double
tstepseq_integral(const TemporalSeq *seq)
{
	double result = 0;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		result += datum_double(temporalinst_value(inst1), inst1->valuetypid) *
			(double) (inst2->t - inst1->t);
		inst1 = inst2;
	}
	return result;
}

/* Integral of the temporal float with linear interpolation */

static double
tlinearseq_integral(const TemporalSeq *seq)
{
	double result = 0;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		double min = Min(DatumGetFloat8(temporalinst_value(inst1)), 
			DatumGetFloat8(temporalinst_value(inst2)));
		double max = Max(DatumGetFloat8(temporalinst_value(inst1)), 
			DatumGetFloat8(temporalinst_value(inst2)));
		result += (max + min) * (double) (inst2->t - inst1->t) / 2.0;
		inst1 = inst2;
	}
	return result;
}

double
tnumberseq_integral(const TemporalSeq *seq)
{
	if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
		return tlinearseq_integral(seq);
	else
		return tstepseq_integral(seq);
}

/* Time-weighted average of temporal numbers */

double
tnumberseq_twavg(const TemporalSeq *seq)
{
	double duration = (double) (seq->period.upper - seq->period.lower);
	double result;
	if (duration == 0)
		/* Instantaneous sequence */
		result = datum_double(temporalinst_value(temporalseq_inst_n(seq, 0)),
			seq->valuetypid);
	else
		result = tnumberseq_integral(seq) / duration;
	return result;
}

/*****************************************************************************
 * Functions for defining B-tree index
 * The functions assume that the arguments are of the same temptypid
 *****************************************************************************/

/* 
 * Equality operator
 * The internal B-tree comparator is not used to increase efficiency
 */
bool
temporalseq_eq(const TemporalSeq *seq1, const TemporalSeq *seq2)
{
	/* If number of sequences, flags, or periods are not equal */
	if (seq1->count != seq2->count || seq1->flags != seq2->flags ||
			! period_eq_internal(&seq1->period, &seq2->period)) 
		return false;

	/* If bounding boxes are not equal */
	void *box1 = temporalseq_bbox_ptr(seq1);
	void *box2 = temporalseq_bbox_ptr(seq2);
	if (! temporal_bbox_eq(box1, box2, seq1->valuetypid))
		return false;
	
	/* Compare the composing instants */
	for (int i = 0; i < seq1->count; i++)
	{
		TemporalInst *inst1 = temporalseq_inst_n(seq1, i);
		TemporalInst *inst2 = temporalseq_inst_n(seq2, i);
		if (! temporalinst_eq(inst1, inst2))
			return false;
	}
	return true;
}

/*
 * B-tree comparator
 * This function supposes for optimization purposes that
 * - a bounding box comparison has been done before in the calling function
 *   and thus that the bounding boxes are equal
 * - the flags of two TemporalSeq values of the same base type only differ
 *   on the linear interpolation flag.
 * These hypothesis may change in the future and the function must be
 * adapted accordingly.
 */
int
temporalseq_cmp(const TemporalSeq *seq1, const TemporalSeq *seq2)
{
	/* Compare inclusive/exclusive bounds
	 * These tests are redundant for temporal types whose bounding box is a
	 * period, that is, tbool and ttext */
	if ((seq1->period.lower_inc && ! seq2->period.lower_inc) ||
		(! seq1->period.upper_inc && seq2->period.upper_inc))
		return -1;
	else if ((seq2->period.lower_inc && ! seq1->period.lower_inc) ||
		(! seq2->period.upper_inc && seq1->period.upper_inc))
		return 1;
	int result;
	/* Compare composing instants */
	int count = Min(seq1->count, seq2->count);
	for (int i = 0; i < count; i++)
	{
		TemporalInst *inst1 = temporalseq_inst_n(seq1, i);
		TemporalInst *inst2 = temporalseq_inst_n(seq2, i);
		result = temporalinst_cmp(inst1, inst2);
		if (result) 
			return result;
	}
	/* Compare flags  */
	if (seq1->flags < seq2->flags)
		return -1;
	if (seq1->flags > seq2->flags)
		return 1;
	/* The two values are equal */
	return 0;
}

/*****************************************************************************
 * Function for defining hash index
 * The function reuses the approach for array types for combining the hash of  
 * the elements and the approach for range types for combining the period 
 * bounds.
 *****************************************************************************/

uint32
temporalseq_hash(const TemporalSeq *seq)
{
	uint32		result;
	char		flags = '\0';

	/* Create flags from the lower_inc and upper_inc values */
	if (seq->period.lower_inc)
		flags |= 0x01;
	if (seq->period.upper_inc)
		flags |= 0x02;
	result = DatumGetUInt32(hash_uint32((uint32) flags));
	
	/* Merge with hash of instants */
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		uint32 inst_hash = temporalinst_hash(inst);
		result = (result << 5) - result + inst_hash;
	}
	return result;
}

/*****************************************************************************/
