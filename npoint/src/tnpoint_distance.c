/*****************************************************************************
 *
 * tnpoint_distance.c
 *	  Temporal distance for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_distance.h"

#include "temporaltypes.h"
#include "temporal_util.h"
#include "tpoint_spatialfuncs.h"
#include "tpoint_distance.h"
#include "tnpoint.h"
#include "tnpoint_static.h"
#include "tnpoint_spatialfuncs.h"
#include "tnpoint_tempspatialrels.h"

/*****************************************************************************
 * Generic distance functions when temporal npoints are moving
 *****************************************************************************/

static TemporalInst **
distance_tnpointseq_geo1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool linear, Datum geo, int *count)
{
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));

	/* Constant segment or stepwise interpolation */
	if (np1->pos == np2->pos || ! linear)
	{
		Datum geom1 = npoint_as_geom_internal(np1);
		TemporalInst **result = palloc(sizeof(TemporalInst *));
		result[0] = temporalinst_make(geom_distance2d(geo, geom1), 
			inst1->t, FLOAT8OID);

		pfree(DatumGetPointer(geom1));
		*count = 1;
		return result;
	}

	/* Find all vertices in the segment */
	Datum traj = tnpointseq_trajectory1(inst1, inst2);
	int countVertices = DatumGetInt32(call_function1(LWGEOM_numpoints_linestring, traj));
	TemporalInst **result = palloc(sizeof(TemporalInst *) * countVertices * 2);
	int k = 0;

	Datum vertex1 = call_function2(LWGEOM_pointn_linestring, traj, Int32GetDatum(1));
	TimestampTz time1 = inst1->t;
	for (int i = 1; i < countVertices; i++)
	{
		result[k++] = temporalinst_make(geom_distance2d(geo, vertex1), time1, FLOAT8OID);

		/* Get the i-th line segment */
		Datum vertex2 = call_function2(LWGEOM_pointn_linestring, traj, Int32GetDatum(i + 1));
		double fraction0 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj, vertex2));
		TimestampTz time2 = (TimestampTz)(inst1->t + (inst2->t - inst1->t) * fraction0);
		Datum line = geompoint_trajectory(vertex1, vertex2);

		/* If distance has a local minimum */
		double fraction = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, line, geo));
		if (fraction > 0 && fraction < 1)
		{
			TimestampTz inttime = (TimestampTz)(time1 + (time2 - time1) * fraction);
			Datum intnp = temporalseq_value_at_timestamp1(inst1, inst2, true, inttime);
			Datum intgeom = npoint_as_geom_internal(DatumGetNpoint(intnp));
			result[k++] = temporalinst_make(geom_distance2d(geo, intgeom), inttime, FLOAT8OID);

			pfree(DatumGetPointer(intgeom));
			pfree(DatumGetPointer(intnp));
		}

		pfree(DatumGetPointer(line));
		pfree(DatumGetPointer(vertex1));
		vertex1 = vertex2;
		time1 = time2;
	}

	pfree(DatumGetPointer(traj));
	pfree(DatumGetPointer(vertex1));
	*count = k;
	return result;
}

static TemporalSeq *
distance_tnpointseq_geo(const TemporalSeq *seq, Datum geo)
{
	TemporalInst ***instants = palloc(sizeof(TemporalInst *) * (seq->count - 1));
	int *countinsts = palloc0(sizeof(int) * (seq->count - 1));
	int totalinsts = 0, countinst;

	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		instants[i] = distance_tnpointseq_geo1(inst1, inst2, 
			MOBDB_FLAGS_GET_LINEAR(seq->flags), geo, &countinst);

		countinsts[i] = countinst;
		totalinsts += countinst;
		inst1 = inst2;
	}

	TemporalInst **allinstants = palloc(sizeof(TemporalInst *) * (totalinsts + 1));
	int k = 0;
	for (int i = 0; i < seq->count - 1; i++)
	{
		for (int j = 0; j < countinsts[i]; j++)
			allinstants[k++] = instants[i][j];
		pfree(instants[i]);
	}

	Datum endGeom = tnpointinst_geom(inst1);
	allinstants[k++] = temporalinst_make(geom_distance2d(geo, endGeom), 
		inst1->t, FLOAT8OID);

	TemporalSeq *result = temporalseq_make(allinstants, k,
		seq->period.lower_inc, seq->period.upper_inc, true, true);

	for (int i = 0; i < k; i++)
		pfree(allinstants[i]);
	pfree(allinstants);
	pfree(instants);
	pfree(countinsts);
	pfree(DatumGetPointer(endGeom));
	return result;
}

static TemporalS *
distance_tnpoints_geo(const TemporalS *ts, Datum geo)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		sequences[i] = distance_tnpointseq_geo(seq, geo);
	}
	TemporalS *result = temporals_make(sequences, ts->count, true);

	for (int i = 0; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalInst **
distance_tnpointseq_tnpointseq1(const TemporalInst *start1, const TemporalInst *start2,
	const TemporalInst *end1, const TemporalInst *end2, int *count)
{
	npoint *startnp1 = DatumGetNpoint(temporalinst_value(start1));
	npoint *endnp1 = DatumGetNpoint(temporalinst_value(end1));
	npoint *startnp2 = DatumGetNpoint(temporalinst_value(start2));
	npoint *endnp2 = DatumGetNpoint(temporalinst_value(end2));

	Datum startGeom1 = npoint_as_geom_internal(startnp1);
	Datum startGeom2 = npoint_as_geom_internal(startnp2);

	/* Both segments are constant */
	if (startnp1->pos == endnp1->pos && startnp2->pos == endnp2->pos)
	{
		TemporalInst **result = palloc(sizeof(TemporalInst *));
		result[0] = temporalinst_make(geom_distance2d(startGeom1, startGeom2), 
			start1->t, FLOAT8OID);

		pfree(DatumGetPointer(startGeom1)); pfree(DatumGetPointer(startGeom2));
		*count = 1;
		return result;
	}

	/* First segment is constant */
	/* Compute the distance between startGeom1 and all vertices of second segment */
	if (startnp1->pos == endnp1->pos)
	{
		Datum traj = tnpointseq_trajectory1(start2, end2);
		int countVertices = DatumGetInt32(call_function1(LWGEOM_numpoints_linestring, traj));
		TemporalInst **result = palloc(sizeof(TemporalInst *) * (countVertices - 1));
		result[0] = temporalinst_make(geom_distance2d(startGeom1, startGeom2), start1->t, FLOAT8OID);
		for (int i = 1; i < countVertices - 1; i++)
		{
			Datum vertex = call_function2(LWGEOM_pointn_linestring, traj, Int32GetDatum(i + 1));
			double fraction = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj, vertex));
			TimestampTz time = (TimestampTz)(start1->t + (end1->t - start1->t) * fraction);
			result[i] = temporalinst_make(geom_distance2d(startGeom1, vertex), time, FLOAT8OID);
			pfree(DatumGetPointer(vertex));
		}

		pfree(DatumGetPointer(traj));
		pfree(DatumGetPointer(startGeom1));
		pfree(DatumGetPointer(startGeom2));
		*count = countVertices - 1;
		return result;
	}

	/* Second segment is constant */
	/* Compute the distance between startGeom2 and all vertices of first segment */
	if (startnp2->pos == endnp2->pos)
	{
		Datum traj = tnpointseq_trajectory1(start1, end1);
		int countVertices = DatumGetInt32(call_function1(LWGEOM_numpoints_linestring, traj));
		TemporalInst **result = palloc(sizeof(TemporalInst *) * (countVertices - 1));
		result[0] = temporalinst_make(geom_distance2d(startGeom1, startGeom2), start1->t, FLOAT8OID);
		for (int i = 1; i < countVertices - 1; i++)
		{
			Datum vertex = call_function2(LWGEOM_pointn_linestring, traj, Int32GetDatum(i + 1));
			double fraction = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj, vertex));
			TimestampTz time = (TimestampTz)(start1->t + (end1->t - start1->t) * fraction);
			result[i] = temporalinst_make(geom_distance2d(vertex, startGeom2), time, FLOAT8OID);
			pfree(DatumGetPointer(vertex));
		}

		pfree(DatumGetPointer(traj));
		pfree(DatumGetPointer(startGeom1));
		pfree(DatumGetPointer(startGeom2));
		*count = countVertices - 1;
		return result;
	}

	/* None of them are constant */
	Datum traj1 = tnpointseq_trajectory1(start1, end1);
	Datum traj2 = tnpointseq_trajectory1(start2, end2);
	int countVertices1 = DatumGetInt32(call_function1(LWGEOM_numpoints_linestring, traj1));
	int countVertices2 = DatumGetInt32(call_function1(LWGEOM_numpoints_linestring, traj2));
	TemporalInst **result = palloc(sizeof(TemporalInst *) * (countVertices1 + countVertices2));
	result[0] = temporalinst_make(geom_distance2d(startGeom1, startGeom2), start1->t, FLOAT8OID);
	int k = 1;

	/* Sequentially scan all vertices */
	Datum vertex1 = call_function2(LWGEOM_pointn_linestring, traj1, Int32GetDatum(2));
	Datum vertex2 = call_function2(LWGEOM_pointn_linestring, traj2, Int32GetDatum(2));
	double fraction1 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj1, vertex1));
	double fraction2 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj2, vertex2));
	int idx1 = 3, idx2 = 3;
	while (fraction1 < 1 || fraction2 < 1)
	{
		/* When vertex1 and vertex2 happen at the same timestamp */
		if (fraction1 == fraction2)
		{
			TimestampTz inttime = (TimestampTz)(start1->t + (end1->t - start1->t) * fraction1);
			result[k++] = temporalinst_make(geom_distance2d(vertex1, vertex2), inttime, FLOAT8OID);

			pfree(DatumGetPointer(vertex1)); pfree(DatumGetPointer(vertex2));
			vertex1 = call_function2(LWGEOM_pointn_linestring, traj1, Int32GetDatum(idx1++));
			vertex2 = call_function2(LWGEOM_pointn_linestring, traj2, Int32GetDatum(idx2++));
			fraction1 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj1, vertex1));
			fraction2 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj2, vertex2));
		}
		/* When vertex1 happens before vertex2 */
		else if (fraction1 < fraction2)
		{
			TimestampTz inttime = (TimestampTz)(start1->t + (end1->t - start1->t) * fraction1);
			Datum intnp = temporalseq_value_at_timestamp1(start2, end2, true, inttime);
			Datum intgeom = npoint_as_geom_internal(DatumGetNpoint(intnp));
			result[k++] = temporalinst_make(geom_distance2d(vertex1, intgeom), inttime, FLOAT8OID);

			pfree(DatumGetPointer(vertex1));
			pfree(DatumGetPointer(intnp)); pfree(DatumGetPointer(intgeom));
			vertex1 = call_function2(LWGEOM_pointn_linestring, traj1, Int32GetDatum(idx1++));
			fraction1 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj1, vertex1));
		}
		/* When vertex2 happens before vertex1 */
		else
		{
			TimestampTz inttime = (TimestampTz)(start1->t + (end1->t - start1->t) * fraction2);
			Datum intnp = temporalseq_value_at_timestamp1(start1, end1, true, inttime);
			Datum intgeom = npoint_as_geom_internal(DatumGetNpoint(intnp));
			result[k++] = temporalinst_make(geom_distance2d(intgeom, vertex2), inttime, FLOAT8OID);

			pfree(DatumGetPointer(intnp)); pfree(DatumGetPointer(intgeom));
			pfree(DatumGetPointer(vertex2));
			vertex2 = call_function2(LWGEOM_pointn_linestring, traj2, Int32GetDatum(idx2++));
			fraction2 = DatumGetFloat8(call_function2(LWGEOM_line_locate_point, traj2, vertex2));
		}
	}

	pfree(DatumGetPointer(vertex1)); pfree(DatumGetPointer(vertex2));
	pfree(DatumGetPointer(traj1)); pfree(DatumGetPointer(traj2));
	pfree(DatumGetPointer(startGeom1)); pfree(DatumGetPointer(startGeom2));
	*count = k;
	return result;
}

TemporalSeq *
distance_tnpointseq_tnpointseq(const TemporalSeq *seq1, const TemporalSeq *seq2)
{
	TemporalInst ***instants = palloc(sizeof(TemporalInst *) * (seq1->count - 1));
	int *countinsts = palloc0(sizeof(int) * (seq1->count - 1));
	int totalinsts = 0, countinst;

	TemporalInst *start1 = temporalseq_inst_n(seq1, 0);
	TemporalInst *start2 = temporalseq_inst_n(seq2, 0);
	for (int i = 0; i < seq1->count - 1; i++)
	{
		TemporalInst *end1 = temporalseq_inst_n(seq1, i + 1);
		TemporalInst *end2 = temporalseq_inst_n(seq2, i + 1);
		instants[i] = distance_tnpointseq_tnpointseq1(start1, start2, 
			end1, end2, &countinst);

		countinsts[i] = countinst;
		totalinsts += countinst;
		start1 = end1;
		start2 = end2;
	}

	TemporalInst **allinstants = palloc(sizeof(TemporalInst *) * (totalinsts + 1));
	int k = 0;
	for (int i = 0; i < seq1->count - 1; i++)
	{
		for (int j = 0; j < countinsts[i]; j++)
			allinstants[k++] = instants[i][j];
		pfree(instants[i]);
	}

	Datum endGeom1 = tnpointinst_geom(start1);
	Datum endGeom2 = tnpointinst_geom(start2);
	allinstants[k++] = temporalinst_make(geom_distance2d(endGeom1, endGeom2), start1->t, FLOAT8OID);
	pfree(DatumGetPointer(endGeom1)); pfree(DatumGetPointer(endGeom2));

	TemporalSeq *result = temporalseq_make(allinstants, k,
		seq1->period.lower_inc, seq1->period.upper_inc, true, true);

	for (int i = 0; i < k; i++)
		pfree(allinstants[i]);
	pfree(allinstants);
	pfree(instants);
	pfree(countinsts);
	return result;
}

TemporalS *
distance_tnpoints_tnpoints(const TemporalS *ts1, const TemporalS *ts2)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts1->count);
	for (int i = 0; i < ts1->count; i++)
	{
		TemporalSeq *seq1 = temporals_seq_n(ts1, i);
		TemporalSeq *seq2 = temporals_seq_n(ts2, i);
		sequences[i] = distance_tnpointseq_tnpointseq(seq1, seq2);
	}
	TemporalS *result = temporals_make(sequences, ts1->count, true);

	for (int i = 0; i < ts1->count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

/*****************************************************************************
 * Temporal distance
 *****************************************************************************/

PG_FUNCTION_INFO_V1(distance_geo_tnpoint);

PGDLLEXPORT Datum
distance_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	ensure_point_type(gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_RETURN_NULL();
	}

	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Temporal *result = NULL; 
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tspatialrel_tnpointinst_geo((TemporalInst *)temp,
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, true);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tspatialrel_tnpointi_geo((TemporalI *)temp,
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, true);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)distance_tnpointseq_geo((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)distance_tnpoints_geo((TemporalS *)temp, 
			PointerGetDatum(gs));
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(distance_npoint_tnpoint);

PGDLLEXPORT Datum
distance_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	Temporal *result = NULL; 
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tspatialrel_tnpointinst_geo((TemporalInst *)temp,
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, true);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tspatialrel_tnpointi_geo((TemporalI *)temp,
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, true);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)distance_tnpointseq_geo((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)distance_tnpoints_geo((TemporalS *)temp, 
			PointerGetDatum(gs));
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(distance_tnpoint_geo);

PGDLLEXPORT Datum
distance_tnpoint_geo(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL; 
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tspatialrel_tnpointinst_geo((TemporalInst *)temp, 
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, false);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tspatialrel_tnpointi_geo((TemporalI *)temp, 
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, false);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)distance_tnpointseq_geo((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)distance_tnpoints_geo((TemporalS *)temp, 
			PointerGetDatum(gs));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(distance_tnpoint_npoint);

PGDLLEXPORT Datum
distance_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);

	Temporal *result = NULL; 
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tspatialrel_tnpointinst_geo((TemporalInst *)temp,
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, true);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tspatialrel_tnpointi_geo((TemporalI *)temp,
			PointerGetDatum(gs), &geom_distance2d, 
			FLOAT8OID, true);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)distance_tnpointseq_geo((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)distance_tnpoints_geo((TemporalS *)temp, 
			PointerGetDatum(gs));
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

Temporal *
distance_tnpoint_tnpoint_internal(const Temporal *temp1, const Temporal *temp2)
{
	Temporal *sync1, *sync2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!synchronize_temporal_temporal(temp1, temp2, &sync1, &sync2, true))
		return NULL;
	
	Temporal *result = NULL;
	ensure_valid_duration(sync1->duration);
	if (sync1->duration == TEMPORALINST)
		result = (Temporal *)tspatialrel_tnpointinst_tnpointinst(
			(TemporalInst *)sync1, (TemporalInst *)sync2, 
			&geom_distance2d, FLOAT8OID);
	else if (sync1->duration == TEMPORALI)
		result = (Temporal *)tspatialrel_tnpointi_tnpointi(
			(TemporalI *)sync1, (TemporalI *)sync2, 
			&geom_distance2d, FLOAT8OID);
	else if (sync1->duration == TEMPORALSEQ)
		result = (Temporal *)distance_tnpointseq_tnpointseq(
			(TemporalSeq *)sync1, (TemporalSeq *)sync2);
	else if (sync1->duration == TEMPORALS)
		result = (Temporal *)distance_tnpoints_tnpoints(
			(TemporalS *)sync1, (TemporalS *)sync2);

	pfree(sync1); pfree(sync2);
	return result;
}

PG_FUNCTION_INFO_V1(distance_tnpoint_tnpoint);

PGDLLEXPORT Datum
distance_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *result = distance_tnpoint_tnpoint_internal(temp1, temp2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
