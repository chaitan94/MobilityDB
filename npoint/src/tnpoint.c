/*****************************************************************************
 *
 * tnpoint.c
 *	  Basic functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint.h"

#include "temporaltypes.h"
#include "temporal_parser.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "tnpoint_static.h"
#include "tnpoint_parser.h"

/*****************************************************************************
 * Input/output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tnpoint_in);

PGDLLEXPORT Datum
tnpoint_in(PG_FUNCTION_ARGS) 
{
	char *input = PG_GETARG_CSTRING(0);
	Oid temptypid = PG_GETARG_OID(1);
	Oid valuetypid;
	temporal_typinfo(temptypid, &valuetypid);
	Temporal *result = temporal_parse(&input, valuetypid);
	if (result == 0)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Cast functions
 *****************************************************************************/

/* Cast tnpoint as tgeompoint */

TemporalInst *
tnpointinst_as_tgeompointinst(TemporalInst *inst)
{
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	Datum geom = npoint_as_geom_internal(np);
	TemporalInst *result = temporalinst_make(geom, inst->t, type_oid(T_GEOMETRY));
	pfree(DatumGetPointer(geom));
	return result;
}

TemporalI *
tnpointi_as_tgeompointi(TemporalI *ti)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		instants[i] = tnpointinst_as_tgeompointinst(inst);
	}
	TemporalI *result = temporali_from_temporalinstarr(instants, ti->count);
	for (int i = 0; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

TemporalSeq *
tnpointseq_as_tgeompointseq(TemporalSeq *seq)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		instants[i] = tnpointinst_as_tgeompointinst(inst);
	}
	TemporalSeq *result = temporalseq_from_temporalinstarr(instants, 
		seq->count, seq->period.lower_inc, seq->period.upper_inc, 
		MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
	for (int i = 0; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

TemporalS *
tnpoints_as_tgeompoints(TemporalS *ts)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		sequences[i] = tnpointseq_as_tgeompointseq(seq);
	}
	TemporalS *result = temporals_from_temporalseqarr(sequences, ts->count, 
		MOBDB_FLAGS_GET_LINEAR(ts->flags), false);
	for (int i = 0; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

Temporal *
tnpoint_as_tgeompoint_internal(Temporal *temp)
{
	Temporal *result = NULL;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)tnpointinst_as_tgeompointinst((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)tnpointi_as_tgeompointi((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)tnpointseq_as_tgeompointseq((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)tnpoints_as_tgeompoints((TemporalS *)temp);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_as_tgeompoint);

PGDLLEXPORT Datum
tnpoint_as_tgeompoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = tnpoint_as_tgeompoint_internal(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Cast tgeompoint as tnpoint */

TemporalInst *
tgeompointinst_as_tnpointinst(TemporalInst *inst)
{
	Datum geom = temporalinst_value(inst);
	npoint *np = geom_as_npoint_internal(geom);
	if (np == NULL)
		return NULL;
	TemporalInst *result = temporalinst_make(PointerGetDatum(np), inst->t,
		type_oid(T_NPOINT));
	pfree(np);
	return result;
}

TemporalI *
tgeompointi_as_tnpointi(TemporalI *ti)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		TemporalInst *inst1 = tgeompointinst_as_tnpointinst(inst);
		if (inst1 == NULL)
		{
			for (int j = 0; j < i; j++)
				pfree(instants[j]);
			pfree(instants);
			return NULL;
		}
		instants[i] = inst1;
	}
	TemporalI *result = temporali_from_temporalinstarr(instants, ti->count);
	for (int i = 0; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

TemporalSeq *
tgeompointseq_as_tnpointseq(TemporalSeq *seq)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		TemporalInst *inst1 = tgeompointinst_as_tnpointinst(inst);
		if (inst1 == NULL)
		{
			for (int j = 0; j < i; j++)
				pfree(instants[j]);
			pfree(instants);
			return NULL;
		}
		instants[i] = inst1;
	}
	TemporalSeq *result = temporalseq_from_temporalinstarr(instants, 
		seq->count, seq->period.lower_inc, seq->period.upper_inc, 
		MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
	for (int i = 0; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

TemporalS *
tgeompoints_as_tnpoints(TemporalS *ts)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalSeq *seq1 = tgeompointseq_as_tnpointseq(seq);
		if (seq1 == NULL)
		{
			for (int j = 0; j < i; j++)
				pfree(sequences[j]);
			pfree(sequences);
			return NULL;
		}
		sequences[i] = seq1;
	}
	TemporalS *result = temporals_from_temporalseqarr(sequences, ts->count, 
		MOBDB_FLAGS_GET_LINEAR(ts->flags), false);
	for (int i = 0; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

PG_FUNCTION_INFO_V1(tgeompoint_as_tnpoint);

PGDLLEXPORT Datum
tgeompoint_as_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)tgeompointinst_as_tnpointinst((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)tgeompointi_as_tnpointi((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)tgeompointseq_as_tnpointseq((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)tgeompoints_as_tnpoints((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Accessor functions
 *****************************************************************************/

/*
 * Positions functions
 * Return the network segments covered by the moving object
 */

ArrayType *
tnpointinst_positions(TemporalInst *inst)
{
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	nsegment *ns = nsegment_make(np->rid, np->pos, np->pos);
	ArrayType *result = nsegmentarr_to_array(&ns, 1);
	pfree(ns);
	return result;
}

/* Set of segments taken by the temporal value */

ArrayType *
tnpointi_positions(TemporalI *ti)
{
	int count;
	/* The following function removes duplicate values */
	Datum *values = temporali_values1(ti, &count);
	nsegment **segments = palloc(sizeof(nsegment *) * count);
	for (int i = 0; i < count; i++)
	{
		npoint *np = DatumGetNpoint(values[i]);
		segments[i] = nsegment_make(np->rid, np->pos, np->pos);
	}
	ArrayType *result = nsegmentarr_to_array(segments, count);
	for (int i = 0; i < count; i++)
		pfree(segments[i]);
	pfree(segments); pfree(values);
	return result;
}

nsegment **
tnpointseq_stepw_positions1(TemporalSeq *seq, int *count)
{
	int count1;
	/* The following function removes duplicate values */
	Datum *values = temporalseq_values1(seq, &count1);
	nsegment **result = palloc(sizeof(nsegment *) * count1);
	for (int i = 0; i < count1; i++)
	{
		npoint *np = DatumGetNpoint(values[i]);
		result[i] = nsegment_make(np->rid, np->pos, np->pos);
	}
	*count = count1;
	return result;
}

nsegment *
tnpointseq_linear_positions1(TemporalSeq *seq)
{
	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	int64 rid = np->rid;
	double minPos = np->pos, maxPos = np->pos;
	for (int i = 1; i < seq->count; i++)
	{
		inst = temporalseq_inst_n(seq, i);
		np = DatumGetNpoint(temporalinst_value(inst));
		minPos = Min(minPos, np->pos);
		maxPos = Max(maxPos, np->pos);
	}
	return nsegment_make(rid, minPos, maxPos);
}

nsegment **
tnpointseq_positions1(TemporalSeq *seq, int *count)
{
	if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		nsegment **result = palloc(sizeof(nsegment *));
		result[0] = tnpointseq_linear_positions1(seq);
		*count = 1;
		return result;
	}
	else
		return tnpointseq_stepw_positions1(seq, count);
}

ArrayType *
tnpointseq_positions(TemporalSeq *seq)
{
	int count;
	nsegment **segments = tnpointseq_positions1(seq, &count);
	ArrayType *result = nsegmentarr_to_array(segments, count);
	pfree(segments);
	return result;
}

nsegment **
tnpoints_stepw_positions1(TemporalS *ts, int *count)
{
	int count1;
	/* The following function removes duplicate values */
	Datum *values = temporals_values1(ts, &count1);
	nsegment **result = palloc(sizeof(nsegment *) * count1);
	for (int i = 0; i < count1; i++)
	{
		npoint *np = DatumGetNpoint(values[i]);
		result[i] = nsegment_make(np->rid, np->pos, np->pos);
	}
	*count = count1;
	return result;
}

nsegment **
tnpoints_linear_positions1(TemporalS *ts, int *count)
{
	nsegment **result = palloc(sizeof(nsegment *) * ts->count);
	for (int i = 0; i < ts->count; i++) 
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		result[i] = tnpointseq_linear_positions1(seq);
	}
	*count = ts->count;
	return result;
}

nsegment **
tnpoints_positions1(TemporalS *ts, int *count)
{
	nsegment **result;
	if (MOBDB_FLAGS_GET_LINEAR(ts->flags))
		result = tnpoints_linear_positions1(ts, count);
	else
		result = tnpoints_stepw_positions1(ts, count);
	return result;
}

ArrayType *
tnpoints_positions(TemporalS *ts)
{
	int count;
	nsegment **segments = tnpoints_positions1(ts, &count);
	ArrayType *result = nsegmentarr_to_array(segments, count);
	for (int i = 0; i < count; i++)
		pfree(segments[i]);
	pfree(segments);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_positions);

PGDLLEXPORT Datum
tnpoint_positions(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	ArrayType *result = NULL; /* make the compiler quiet */
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = tnpointinst_positions((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = tnpointi_positions((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ) 
		result = tnpointseq_positions((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS) 
		result = tnpoints_positions((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Route of a temporal instant */

PG_FUNCTION_INFO_V1(tnpointinst_route);

PGDLLEXPORT Datum
tnpointinst_route(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	if (temp->duration != TEMPORALINST)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Input must be a temporal instant")));

	npoint *np = DatumGetNpoint(temporalinst_value((TemporalInst *)temp));
	int64 result = np->rid;
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_INT64(result);
}

/*
 * Routes functions
 * Return the routes covered by the moving object
 */

ArrayType *
tnpointinst_routes(TemporalInst *inst)
{
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	ArrayType *result = int64arr_to_array(&np->rid, 1);
	return result;
}

ArrayType *
tnpointi_routes(TemporalI *ti)
{
	int64 *routes = palloc(sizeof(int64) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		npoint *np = DatumGetNpoint(temporalinst_value(inst));
		routes[i] = np->rid;
	}
	ArrayType *result = int64arr_to_array(routes, ti->count);
	pfree(routes);
	return result;
}

ArrayType *
tnpointseq_routes(TemporalSeq *seq)
{
	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	ArrayType *result = int64arr_to_array(&np->rid, 1);
	return result;
}

ArrayType *
tnpoints_routes(TemporalS *ts)
{
	int64 *routes = palloc(sizeof(int64) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		npoint *np = DatumGetNpoint(temporalinst_value(inst));
		routes[i] = np->rid;
	}
	ArrayType *result = int64arr_to_array(routes, ts->count);
	pfree(routes);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_routes);

PGDLLEXPORT Datum
tnpoint_routes(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	ArrayType *result = NULL; /* make the compiler quiet */
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = tnpointinst_routes((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = tnpointi_routes((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ) 
		result = tnpointseq_routes((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS) 
		result = tnpoints_routes((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
