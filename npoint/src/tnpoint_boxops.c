/*****************************************************************************
 *
 * tnpoint_boxops.c
 *	  Bounding box operators for temporal network points.
 *
 * These operators test the bounding boxes of temporal npoints, which are
 * STBOX boxes. The following operators are defined:
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

#include "tnpoint_boxops.h"

#include <utils/timestamp.h>

#include "temporaltypes.h"
#include "temporal_util.h"
#include "stbox.h"
#include "tpoint_boxops.h"
#include "tnpoint.h"
#include "tnpoint_static.h"

/*****************************************************************************
 * Transform a temporal npoint to a STBOX
 *****************************************************************************/

bool
npoint_to_stbox_internal(STBOX *box, npoint *np)
{
	Datum geom = npoint_as_geom_internal(DatumGetNpoint(np));
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	bool result = geo_to_stbox_internal(box, gs);
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	return result;
}

void
tnpointinst_make_stbox(STBOX *box, TemporalInst *inst)
{
	npoint_to_stbox_internal(box, DatumGetNpoint(temporalinst_value(inst)));
	box->tmin = box->tmax = inst->t;
	MOBDB_FLAGS_SET_T(box->flags, true);
	return;
}

void
tnpointinstarr_stepw_to_stbox(STBOX *box, TemporalInst **instants, int count)
{
	tnpointinst_make_stbox(box, instants[0]);
	for (int i = 1; i < count; i++)
	{
		STBOX box1;
		memset(&box1, 0, sizeof(STBOX));
		tnpointinst_make_stbox(&box1, instants[i]);
		stbox_expand(box, &box1);
	}
	return;
}

void
tnpointinstarr_linear_to_stbox(STBOX *box, TemporalInst **instants, int count)
{
	npoint *np = DatumGetNpoint(temporalinst_value(instants[0]));
	int64 rid = np->rid;
	double posmin = np->pos, posmax = np->pos;
	TimestampTz tmin = instants[0]->t, tmax = instants[0]->t;
	for (int i = 1; i < count; i++)
	{
		np = DatumGetNpoint(temporalinst_value(instants[i]));
		posmin = Min(posmin, np->pos);
		posmax = Max(posmax, np->pos);
		tmin = Min(tmin, instants[i]->t);
		tmax = Max(tmax, instants[i]->t);
	}

	Datum line = route_geom(rid);
	Datum geom = (posmin == 0 && posmax == 1) ? line :
		call_function3(LWGEOM_line_substring, line, Float8GetDatum(posmin),
			Float8GetDatum(posmax));
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	geo_to_stbox_internal(box, gs);
	box->tmin = tmin;
	box->tmax = tmax;
	MOBDB_FLAGS_SET_T(box->flags, true);
	pfree(DatumGetPointer(line));
	if (posmin != 0 || posmax != 1)
		pfree(DatumGetPointer(geom));
	return;
}

void
tnpointseqarr_to_stbox(STBOX *box, TemporalSeq **sequences, int count)
{
	temporalseq_bbox(box, sequences[0]);
	for (int i = 1; i < count; i++)
	{
		STBOX *box1 = temporalseq_bbox_ptr(sequences[i]);
		stbox_expand(box, box1);
	}
	return;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(npoint_to_stbox);

PGDLLEXPORT Datum
npoint_to_stbox(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	STBOX *result = palloc0(sizeof(STBOX));
	npoint_to_stbox_internal(result, np);
	PG_RETURN_POINTER(result);
}

bool
nsegment_to_stbox_internal(STBOX *box, nsegment *ns)
{
	Datum geom = nsegment_as_geom_internal(DatumGetNsegment(ns));
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	bool result = geo_to_stbox_internal(box, gs);
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	return result;
}

PG_FUNCTION_INFO_V1(nsegment_to_stbox);

PGDLLEXPORT Datum
nsegment_to_stbox(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	STBOX *result = palloc0(sizeof(STBOX));
	nsegment_to_stbox_internal(result, ns);
	PG_RETURN_POINTER(result);
}

static bool
npoint_timestamp_to_stbox_internal(STBOX *box, npoint *np, TimestampTz t)
{
	npoint_to_stbox_internal(box, np);
	box->tmin = box->tmax = t;
	MOBDB_FLAGS_SET_T(box->flags, true);
	return true;
}

PG_FUNCTION_INFO_V1(npoint_timestamp_to_stbox);

PGDLLEXPORT Datum
npoint_timestamp_to_stbox(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	TimestampTz t = PG_GETARG_TIMESTAMPTZ(1);
	STBOX *result = palloc0(sizeof(STBOX));
	npoint_timestamp_to_stbox_internal(result, np, t);
	PG_RETURN_POINTER(result);
}

static bool
npoint_period_to_stbox_internal(STBOX *box, npoint *np, Period *p)
{
	npoint_to_stbox_internal(box, np);
	box->tmin = p->lower;
	box->tmax = p->upper;
	MOBDB_FLAGS_SET_T(box->flags, true);
	return true;
}

PG_FUNCTION_INFO_V1(npoint_period_to_stbox);

PGDLLEXPORT Datum
npoint_period_to_stbox(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Period *p = PG_GETARG_PERIOD(1);
	STBOX *result = palloc0(sizeof(STBOX));
	npoint_period_to_stbox_internal(result, np, p);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tnpoint_to_stbox);

PGDLLEXPORT Datum
tnpoint_to_stbox(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	STBOX *result = palloc0(sizeof(STBOX));
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
	STBOX box;
	memset(&box, 0, sizeof(STBOX));
	npoint_to_stbox_internal(&box, np);
	STBOX *result = stbox_expand_spatial_internal(&box, d);
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
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box1, np))
	{
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = overlaps_stbox_stbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overlaps_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
overlaps_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	temporal_bbox(&box1, temp);
	if (!npoint_to_stbox_internal(&box2, np))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	bool result = overlaps_stbox_stbox_internal(&box1, &box2);
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
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box1, np))
	{
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = contains_stbox_stbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contains_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
contains_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box2, np))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = contains_stbox_stbox_internal(&box1, &box2);
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
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box1, np))
	{
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = contained_stbox_stbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contained_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
contained_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box2, np))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = contained_stbox_stbox_internal(&box1, &box2);
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
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box1, np))
	{
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = same_stbox_stbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(same_bbox_tnpoint_npoint);

PGDLLEXPORT Datum
same_bbox_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!npoint_to_stbox_internal(&box2, np))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = same_stbox_stbox_internal(&box1, &box2);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************/
