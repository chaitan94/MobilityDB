/*****************************************************************************
 *
 * tnpoint_spatialrels.c
 *	  Spatial relationships for temporal network points.
 *
 * These relationships project the temporal dimension and return a Boolean.
 * They are thus defined with the "at any instant" semantics, that is, the
 * traditional spatial function is applied to the union of all values taken
 * by the temporal npoint. The following relationships are supported:
 *	contains, containsproperly, covers, coveredby, crosses, disjoint,
 *	equals, intersects, overlaps, touches, within, dwithin, and
 *	relate (with 2 and 3 arguments)
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_spatialrels.h"

#include "lifting.h"
#include "tpoint_spatialrels.h"
#include "tnpoint.h"
#include "tnpoint_static.h"

/*****************************************************************************
 * Generic binary functions for tnpoint <rel> geo
 *****************************************************************************/

static bool
spatialrel_tnpointinst_geo(TemporalInst *inst, Datum geo, 
	Datum (*operator)(Datum, Datum), bool invert)
{
	Datum geom = tnpointinst_geom(inst);
	bool result = invert ? DatumGetBool(operator(geo, geom)) :
		DatumGetBool(operator(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

static bool
spatialrel_tnpointi_geo(TemporalI *ti, Datum geo, 
	Datum (*operator)(Datum, Datum), bool invert)
{
	Datum geom = tnpointi_geom(ti);
	bool result = invert ? DatumGetBool(operator(geo, geom)) :
		DatumGetBool(operator(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

static bool
spatialrel_tnpointseq_geo(TemporalSeq *seq, Datum geo, 
	Datum (*operator)(Datum, Datum), bool invert)
{
	Datum geom = tnpointseq_geom(seq);
	bool result = invert ? DatumGetBool(operator(geo, geom)) :
		DatumGetBool(operator(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

static bool
spatialrel_tnpoints_geo(TemporalS *ts, Datum geo, 
	Datum (*operator)(Datum, Datum), bool invert)
{
	Datum geom = tnpoints_geom(ts);
	bool result = invert ? DatumGetBool(operator(geo, geom)) :
		DatumGetBool(operator(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

/*****************************************************************************
 * Generic binary functions for tnpoint <rel> tnpoint
 *****************************************************************************/

static bool
spatialrel_tnpointinst_tnpointinst(TemporalInst *inst1, TemporalInst *inst2, 
	Datum (*operator)(Datum, Datum))
{
	Datum geom1 =  tnpointinst_geom(inst1);
	Datum geom2 =  tnpointinst_geom(inst2);
	bool result = DatumGetBool(operator(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static bool
spatialrel_tnpointi_tnpointi(TemporalI *ti1, TemporalI *ti2, 
	Datum (*operator)(Datum, Datum))
{
	Datum geom1 = tnpointi_geom(ti1);
	Datum geom2 = tnpointi_geom(ti2);
	bool result = DatumGetBool(operator(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static bool
spatialrel_tnpointseq_tnpointseq(TemporalSeq *seq1, TemporalSeq *seq2, 
	Datum (*operator)(Datum, Datum))
{
	Datum geom1 = tnpointseq_geom(seq1);
	Datum geom2 = tnpointseq_geom(seq2);
	bool result = DatumGetBool(operator(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static bool
spatialrel_tnpoints_tnpoints(TemporalS *ts1, TemporalS *ts2, 
	Datum (*operator)(Datum, Datum))
{
	Datum geom1 = tnpoints_geom(ts1);
	Datum geom2 = tnpoints_geom(ts2);
	bool result = DatumGetBool(operator(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

/*****************************************************************************
 * Generic ternary functions for tnpoint <rel> geo
 *****************************************************************************/

static bool
spatialrel3_tnpointinst_geo(TemporalInst *inst, Datum geo, Datum param, 
	Datum (*operator)(Datum, Datum, Datum), bool invert)
{
	Datum geom = tnpointinst_geom(inst);
	bool result = invert ? DatumGetBool(operator(geo, geom, param)) :
		DatumGetBool(operator(geom, geo, param));
	pfree(DatumGetPointer(geom));
	return result;
}

static bool
spatialrel3_tnpointi_geo(TemporalI *ti, Datum geo, Datum param, 
	Datum (*operator)(Datum, Datum, Datum), bool invert)
{
	Datum geom = tnpointi_geom(ti);
	bool result = invert ? DatumGetBool(operator(geo, geom, param)) :
		DatumGetBool(operator(geom, geo, param));
	pfree(DatumGetPointer(geom));
	return result;
}

static bool
spatialrel3_tnpointseq_geo(TemporalSeq *seq, Datum geo, Datum param, 
	Datum (*operator)(Datum, Datum, Datum), bool invert)
{
	Datum geom = tnpointseq_geom(seq);
	bool result = invert ? DatumGetBool(operator(geo, geom, param)) :
		DatumGetBool(operator(geom, geo, param));
	pfree(DatumGetPointer(geom));
	return result;
}

static bool
spatialrel3_tnpoints_geo(TemporalS *ts, Datum geo, Datum param, 
	Datum (*operator)(Datum, Datum, Datum), bool invert)
{
	Datum geom = tnpoints_geom(ts);
	bool result = invert ? DatumGetBool(operator(geo, geom, param)) :
		DatumGetBool(operator(geom, geo, param));
	pfree(DatumGetPointer(geom));
	return result;
}

/*****************************************************************************
 * Generic ternary functions for tnpoint <rel> tnpoint
 *****************************************************************************/

static bool
spatialrel3_tnpointinst_tnpointinst(TemporalInst *inst1, TemporalInst *inst2, Datum param, 
	Datum (*operator)(Datum, Datum, Datum))
{
	Datum geom1 =  tnpointinst_geom(inst1);
	Datum geom2 =  tnpointinst_geom(inst2);
	bool result = DatumGetBool(operator(geom1, geom2, param));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static bool
spatialrel3_tnpointi_tnpointi(TemporalI *ti1, TemporalI *ti2, Datum param, 
	Datum (*operator)(Datum, Datum, Datum))
{
	Datum geom1 = tnpointi_geom(ti1);
	Datum geom2 = tnpointi_geom(ti2);
	bool result = DatumGetBool(operator(geom1, geom2, param));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static bool
spatialrel3_tnpointseq_tnpointseq(TemporalSeq *seq1, TemporalSeq *seq2, Datum param, 
	Datum (*operator)(Datum, Datum, Datum))
{
	Datum geom1 = tnpointseq_geom(seq1);
	Datum geom2 = tnpointseq_geom(seq2);
	bool result = DatumGetBool(operator(geom1, geom2, param));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static bool
spatialrel3_tnpoints_tnpoints(TemporalS *ts1, TemporalS *ts2, Datum param, 
	Datum (*operator)(Datum, Datum, Datum))
{
	Datum geom1 = tnpoints_trajectory(ts1);
	Datum geom2 = tnpoints_trajectory(ts2);
	bool result = DatumGetBool(operator(geom1, geom2, param));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

/*****************************************************************************
 * Generic relate functions for tnpoint relate geo
 *****************************************************************************/

static text *
relate_tnpointinst_geo(TemporalInst *inst, Datum geo, bool invert)
{
	Datum geom = tnpointinst_geom(inst);
	text *result = invert ? DatumGetTextP(geom_relate(geo, geom)) :
		DatumGetTextP(geom_relate(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

static text *
relate_tnpointi_geo(TemporalI *ti, Datum geo, bool invert)
{
	Datum geom = tnpointi_geom(ti);
	text *result = invert ? DatumGetTextP(geom_relate(geo, geom)) :
		DatumGetTextP(geom_relate(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

static text *
relate_tnpointseq_geo(TemporalSeq *seq, Datum geo, bool invert)
{
	Datum geom = tnpointseq_geom(seq);
	text *result = invert ? DatumGetTextP(geom_relate(geo, geom)) :
		DatumGetTextP(geom_relate(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

static text *
relate_tnpoints_geo(TemporalS *ts, Datum geo, bool invert)
{
	Datum geom = tnpoints_geom(ts);
	text *result = invert ? DatumGetTextP(geom_relate(geo, geom)) :
		DatumGetTextP(geom_relate(geom, geo));
	pfree(DatumGetPointer(geom));
	return result;
}

/*****************************************************************************
 * Generic relate functions for tnpoint relate tnpoint 
 *****************************************************************************/

static text *
relate_tnpointinst_tnpointinst(TemporalInst *inst1, TemporalInst *inst2)
{
	Datum geom1 =  tnpointinst_geom(inst1);
	Datum geom2 =  tnpointinst_geom(inst2);
	text *result = DatumGetTextP(geom_relate(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static text *
relate_tnpointi_tnpointi(TemporalI *ti1, TemporalI *ti2)
{
	Datum geom1 = tnpointi_geom(ti1);
	Datum geom2 = tnpointi_geom(ti2);
	text *result = DatumGetTextP(geom_relate(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static text *
relate_tnpointseq_tnpointseq(TemporalSeq *seq1, TemporalSeq *seq2)
{
	Datum geom1 = tnpointseq_geom(seq1);
	Datum geom2 = tnpointseq_geom(seq2);
	text *result = DatumGetTextP(geom_relate(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

static text *
relate_tnpoints_tnpoints(TemporalS *ts1, TemporalS *ts2)
{
	Datum geom1 = tnpoints_trajectory(ts1);
	Datum geom2 = tnpoints_trajectory(ts2);
	text *result = DatumGetTextP(geom_relate(geom1, geom2));
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	return result;
}

/*****************************************************************************
 * Dispatch Functions
 *****************************************************************************/

static bool
spatialrel_tnpoint_geo(Temporal *temp, Datum geom,
	Datum (*operator)(Datum, Datum), bool invert)
{
	bool result = false;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = spatialrel_tnpointinst_geo((TemporalInst *)temp, geom,
			operator, invert);
	else if (temp->duration == TEMPORALI)
		result = spatialrel_tnpointi_geo((TemporalI *)temp, geom,
			operator, invert);
	else if (temp->duration == TEMPORALSEQ)
		result = spatialrel_tnpointseq_geo((TemporalSeq *)temp, geom,
			operator, invert);
	else if (temp->duration == TEMPORALS)
		result = spatialrel_tnpoints_geo((TemporalS *)temp, geom,
			operator, invert);
	return result;
}

static bool
spatialrel_tnpoint_tnpoint(Temporal *temp1, Temporal *temp2,
	Datum (*operator)(Datum, Datum))
{
	bool result = false;
	ensure_valid_duration(temp1->duration);
	if (temp1->duration == TEMPORALINST) 
		result = spatialrel_tnpointinst_tnpointinst(
			(TemporalInst *)temp1, (TemporalInst *)temp2, operator);
	else if (temp1->duration == TEMPORALI) 
		result = spatialrel_tnpointi_tnpointi(
			(TemporalI *)temp1, (TemporalI *)temp2, operator);
	else if (temp1->duration == TEMPORALSEQ)
		result = spatialrel_tnpointseq_tnpointseq(
			(TemporalSeq *)temp1, (TemporalSeq *)temp2, operator);
	else if (temp1->duration == TEMPORALS)
		result = spatialrel_tnpoints_tnpoints(
			(TemporalS *)temp1, (TemporalS *)temp2, operator);
	return result;
}

/*****************************************************************************/

static text *
relate_tnpoint_geo_internal(Temporal *temp, Datum geo, bool invert)
{
	text *result = NULL;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = relate_tnpointinst_geo((TemporalInst *)temp, geo, invert);
	else if (temp->duration == TEMPORALI)
		result = relate_tnpointi_geo((TemporalI *)temp, geo, invert);
	else if (temp->duration == TEMPORALSEQ)
		result = relate_tnpointseq_geo((TemporalSeq *)temp, geo, invert);
	else if (temp->duration == TEMPORALS)
		result = relate_tnpoints_geo((TemporalS *)temp, geo, invert);
	return result;
}

/*****************************************************************************/

static bool
spatialrel3_tnpoint_geo(Temporal *temp, Datum geom, Datum param,
	Datum (*operator)(Datum, Datum, Datum), bool invert)
{
	bool result = false;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = spatialrel3_tnpointinst_geo((TemporalInst *)temp, geom, param,
			operator, invert);
	else if (temp->duration == TEMPORALI)
		result = spatialrel3_tnpointi_geo((TemporalI *)temp, geom, param,
			operator, invert);
	else if (temp->duration == TEMPORALSEQ)
		result = spatialrel3_tnpointseq_geo((TemporalSeq *)temp, geom, param,
			operator, invert);
	else if (temp->duration == TEMPORALS)
		result = spatialrel3_tnpoints_geo((TemporalS *)temp, geom, param,
			operator, invert);
	return result;
}

static bool
spatialrel3_tnpoint_tnpoint(Temporal *temp1, Temporal *temp2, Datum param,
	Datum (*operator)(Datum, Datum, Datum))
{
	bool result = false;
	ensure_valid_duration(temp1->duration);
	if (temp1->duration == TEMPORALINST) 
		result = spatialrel3_tnpointinst_tnpointinst(
			(TemporalInst *)temp1, (TemporalInst *)temp2, param, operator);
	else if (temp1->duration == TEMPORALI) 
		result = spatialrel3_tnpointi_tnpointi(
			(TemporalI *)temp1, (TemporalI *)temp2, param, operator);
	else if (temp1->duration == TEMPORALSEQ) 
		result = spatialrel3_tnpointseq_tnpointseq(
			(TemporalSeq *)temp1, (TemporalSeq *)temp2, param, operator);
	else if (temp1->duration == TEMPORALS) 
		result = spatialrel3_tnpoints_tnpoints(
			(TemporalS *)temp1, (TemporalS *)temp2, param, operator);
	return result;
}

/*****************************************************************************
 * Temporal contains
 *****************************************************************************/

PG_FUNCTION_INFO_V1(contains_geo_tnpoint);

PGDLLEXPORT Datum
contains_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_contains, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contains_npoint_tnpoint);

PGDLLEXPORT Datum
contains_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_contains, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contains_tnpoint_geo);

PGDLLEXPORT Datum
contains_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_contains, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contains_tnpoint_npoint);

PGDLLEXPORT Datum
contains_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_contains, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(contains_tnpoint_tnpoint);

PGDLLEXPORT Datum
contains_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_contains);
	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal containsproperly
 *****************************************************************************/

PG_FUNCTION_INFO_V1(containsproperly_geo_tnpoint);

PGDLLEXPORT Datum
containsproperly_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_containsproperly, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(containsproperly_npoint_tnpoint);

PGDLLEXPORT Datum
containsproperly_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_containsproperly, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(containsproperly_tnpoint_geo);

PGDLLEXPORT Datum
containsproperly_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_containsproperly, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(containsproperly_tnpoint_npoint);

PGDLLEXPORT Datum
containsproperly_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_containsproperly, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(containsproperly_tnpoint_tnpoint);

PGDLLEXPORT Datum
containsproperly_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_containsproperly);
	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal covers
 *****************************************************************************/

PG_FUNCTION_INFO_V1(covers_geo_tnpoint);

PGDLLEXPORT Datum
covers_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_covers, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(covers_npoint_tnpoint);

PGDLLEXPORT Datum
covers_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_covers, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(covers_tnpoint_geo);

PGDLLEXPORT Datum
covers_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_covers, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(covers_tnpoint_npoint);

PGDLLEXPORT Datum
covers_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_covers, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(covers_tnpoint_tnpoint);

PGDLLEXPORT Datum
covers_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_covers);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal coveredby
 *****************************************************************************/

PG_FUNCTION_INFO_V1(coveredby_geo_tnpoint);

PGDLLEXPORT Datum
coveredby_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_coveredby, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(coveredby_npoint_tnpoint);

PGDLLEXPORT Datum
coveredby_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_coveredby, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(coveredby_tnpoint_geo);

PGDLLEXPORT Datum
coveredby_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_coveredby, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(coveredby_tnpoint_npoint);

PGDLLEXPORT Datum
coveredby_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_coveredby, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(coveredby_tnpoint_tnpoint);

PGDLLEXPORT Datum
coveredby_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_coveredby);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal crosses
 *****************************************************************************/

PG_FUNCTION_INFO_V1(crosses_geo_tnpoint);

PGDLLEXPORT Datum
crosses_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_crosses, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(crosses_npoint_tnpoint);

PGDLLEXPORT Datum
crosses_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_crosses, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(crosses_tnpoint_geo);

PGDLLEXPORT Datum
crosses_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_crosses, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(crosses_tnpoint_npoint);

PGDLLEXPORT Datum
crosses_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_crosses, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(crosses_tnpoint_tnpoint);

PGDLLEXPORT Datum
crosses_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_crosses);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal disjoint
 *****************************************************************************/

PG_FUNCTION_INFO_V1(disjoint_geo_tnpoint);

PGDLLEXPORT Datum
disjoint_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_disjoint, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(disjoint_npoint_tnpoint);

PGDLLEXPORT Datum
disjoint_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_disjoint, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(disjoint_tnpoint_geo);

PGDLLEXPORT Datum
disjoint_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_disjoint, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(disjoint_tnpoint_npoint);

PGDLLEXPORT Datum
disjoint_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_disjoint, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(disjoint_tnpoint_tnpoint);

PGDLLEXPORT Datum
disjoint_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_disjoint);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal equals
 *****************************************************************************/

PG_FUNCTION_INFO_V1(equals_geo_tnpoint);

PGDLLEXPORT Datum
equals_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_equals, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(equals_npoint_tnpoint);

PGDLLEXPORT Datum
equals_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_equals, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(equals_tnpoint_geo);

PGDLLEXPORT Datum
equals_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_equals, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(equals_tnpoint_npoint);

PGDLLEXPORT Datum
equals_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_equals, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(equals_tnpoint_tnpoint);

PGDLLEXPORT Datum
equals_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_equals);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal intersects
 *****************************************************************************/

PG_FUNCTION_INFO_V1(intersects_geo_tnpoint);

PGDLLEXPORT Datum
intersects_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_intersects2d, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intersects_npoint_tnpoint);

PGDLLEXPORT Datum
intersects_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_intersects2d, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intersects_tnpoint_geo);

PGDLLEXPORT Datum
intersects_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_intersects2d, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intersects_tnpoint_npoint);

PGDLLEXPORT Datum
intersects_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_intersects2d, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(intersects_tnpoint_tnpoint);

PGDLLEXPORT Datum
intersects_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_intersects2d);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal overlaps
 *****************************************************************************/

PG_FUNCTION_INFO_V1(overlaps_geo_tnpoint);

PGDLLEXPORT Datum
overlaps_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_overlaps, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overlaps_npoint_tnpoint);

PGDLLEXPORT Datum
overlaps_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_overlaps, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overlaps_tnpoint_geo);

PGDLLEXPORT Datum
overlaps_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_overlaps, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overlaps_tnpoint_npoint);

PGDLLEXPORT Datum
overlaps_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_overlaps, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overlaps_tnpoint_tnpoint);

PGDLLEXPORT Datum
overlaps_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_overlaps);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal touches
 *****************************************************************************/

PG_FUNCTION_INFO_V1(touches_geo_tnpoint);

PGDLLEXPORT Datum
touches_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_touches, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(touches_npoint_tnpoint);

PGDLLEXPORT Datum
touches_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_touches, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(touches_tnpoint_geo);

PGDLLEXPORT Datum
touches_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_touches, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(touches_tnpoint_npoint);

PGDLLEXPORT Datum
touches_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_touches, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(touches_tnpoint_tnpoint);

PGDLLEXPORT Datum
touches_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_touches);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal within
 *****************************************************************************/

PG_FUNCTION_INFO_V1(within_geo_tnpoint);

PGDLLEXPORT Datum
within_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_within, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(within_npoint_tnpoint);

PGDLLEXPORT Datum
within_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_within, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(within_tnpoint_geo);

PGDLLEXPORT Datum
within_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_within, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(within_tnpoint_npoint);

PGDLLEXPORT Datum
within_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel_tnpoint_geo(temp, geom, &geom_within, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(within_tnpoint_tnpoint);

PGDLLEXPORT Datum
within_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel_tnpoint_tnpoint(inter1, inter2, &geom_within);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal dwithin
 *****************************************************************************/

PG_FUNCTION_INFO_V1(dwithin_geo_tnpoint);

PGDLLEXPORT Datum
dwithin_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum dist = PG_GETARG_DATUM(2);
	bool result = spatialrel3_tnpoint_geo(temp, geom, dist, &geom_dwithin2d, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(dwithin_npoint_tnpoint);

PGDLLEXPORT Datum
dwithin_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum dist = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel3_tnpoint_geo(temp, geom, dist, &geom_dwithin2d, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(dwithin_tnpoint_geo);

PGDLLEXPORT Datum
dwithin_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	Datum dist = PG_GETARG_DATUM(2);
	bool result = spatialrel3_tnpoint_geo(temp, geom, dist, &geom_dwithin2d, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(dwithin_tnpoint_npoint);

PGDLLEXPORT Datum
dwithin_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum dist = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel3_tnpoint_geo(temp, geom, dist, &geom_dwithin2d, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(dwithin_tnpoint_tnpoint);

PGDLLEXPORT Datum
dwithin_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Datum dist = PG_GETARG_DATUM(2);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel3_tnpoint_tnpoint(inter1, inter2, dist, &geom_dwithin2d);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************
 * Temporal relate
 *****************************************************************************/

PG_FUNCTION_INFO_V1(relate_geo_tnpoint);

PGDLLEXPORT Datum
relate_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	text *result = relate_tnpoint_geo_internal(temp, geom, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(relate_npoint_tnpoint);

PGDLLEXPORT Datum
relate_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	text *result = relate_tnpoint_geo_internal(temp, geom, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(relate_tnpoint_geo);

PGDLLEXPORT Datum
relate_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	text *result = relate_tnpoint_geo_internal(temp, geom, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(relate_tnpoint_npoint);

PGDLLEXPORT Datum
relate_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	text *result = relate_tnpoint_geo_internal(temp, geom, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(relate_tnpoint_tnpoint);

PGDLLEXPORT Datum
relate_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	
	text *result = NULL;
	ensure_valid_duration(inter1->duration);
	if (inter1->duration == TEMPORALINST)
		result = relate_tnpointinst_tnpointinst(
			(TemporalInst *)inter1, (TemporalInst *)inter2);
	else if (inter1->duration == TEMPORALI)
		result = relate_tnpointi_tnpointi(
			(TemporalI *)inter1, (TemporalI *)inter2);
	else if (inter1->duration == TEMPORALSEQ)
		result = relate_tnpointseq_tnpointseq(
			(TemporalSeq *)inter1, (TemporalSeq *)inter2);
	else if (inter1->duration == TEMPORALS)
		result = relate_tnpoints_tnpoints(
			(TemporalS *)inter1, (TemporalS *)inter2);

 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_TEXT_P(result);
}

/*****************************************************************************
 * Temporal relate_pattern
 *****************************************************************************/

PG_FUNCTION_INFO_V1(relate_pattern_geo_tnpoint);

PGDLLEXPORT Datum
relate_pattern_geo_tnpoint(PG_FUNCTION_ARGS)
{
	Datum geom = PG_GETARG_DATUM(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum pattern = PG_GETARG_DATUM(2);
	bool result = spatialrel3_tnpoint_geo(temp, geom, pattern,
		&geom_relate_pattern, true);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(relate_pattern_npoint_tnpoint);

PGDLLEXPORT Datum
relate_pattern_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum pattern = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel3_tnpoint_geo(temp, geom, pattern,
		&geom_relate_pattern, true);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(relate_pattern_tnpoint_geo);

PGDLLEXPORT Datum
relate_pattern_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum geom = PG_GETARG_DATUM(1);
	Datum pattern = PG_GETARG_DATUM(2);
	bool result = spatialrel3_tnpoint_geo(temp, geom, pattern,
		&geom_relate_pattern, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(relate_pattern_tnpoint_npoint);

PGDLLEXPORT Datum
relate_pattern_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum pattern = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	bool result = spatialrel3_tnpoint_geo(temp, geom, pattern,
		&geom_relate_pattern, false);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(relate_pattern_tnpoint_tnpoint);

PGDLLEXPORT Datum
relate_pattern_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Datum pattern = PG_GETARG_DATUM(2);
	Temporal *inter1, *inter2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!intersection_temporal_temporal(temp1, temp2, &inter1, &inter2))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	bool result = spatialrel3_tnpoint_tnpoint(inter1, inter2, pattern, &geom_relate_pattern);
 	pfree(inter1); pfree(inter2); 
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************/
