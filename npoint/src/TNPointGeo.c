/*****************************************************************************
 *
 * TNPointGeo.c
 *	  Geospatial functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "TNPoint.h"

/*****************************************************************************
 * Trajectory functions
 *****************************************************************************/

Datum
tnpointseq_trajectory1(TemporalInst *inst1, TemporalInst *inst2)
{
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));

	if (np1->rid != np2->rid)
		return PointerGetDatum(NULL);
	if (np1->pos == np2->pos)
		return npoint_as_geom_internal(np1);

	Datum line = route_geom(np1->rid);
	Datum traj;
	if (np1->pos < np2->pos)
	{
		if (np1->pos == 0 && np2->pos == 1)
			traj = PointerGetDatum(gserialized_copy(
				(GSERIALIZED *)PG_DETOAST_DATUM(line)));
		else
			traj = call_function3(LWGEOM_line_substring, line,
				Float8GetDatum(np1->pos), Float8GetDatum(np2->pos));
	}
	else
	{
		Datum traj2;
		if (np2->pos == 0 && np1->pos == 1)
			traj2 = PointerGetDatum(gserialized_copy(
				(GSERIALIZED *)PG_DETOAST_DATUM(line)));
		else
			traj2 = call_function3(LWGEOM_line_substring, line,
				Float8GetDatum(np2->pos), Float8GetDatum(np1->pos));
		traj = call_function1(LWGEOM_reverse, traj2);
		pfree(DatumGetPointer(traj2));
	}

	pfree(DatumGetPointer(line));
	return traj;
}

Datum
tnpointseq_trajectory(TemporalSeq *seq)
{
	Datum *trajs = palloc(sizeof(Datum) * (seq->count - 1));
	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst));
	Datum line = route_geom(np1->rid);
	int k = 0;
	for (int i = 1; i < seq->count; i++)
	{
		inst = temporalseq_inst_n(seq, i);
		npoint *np2 = DatumGetNpoint(temporalinst_value(inst));
		if (np1->pos < np2->pos)
		{
			if (np1->pos == 0 && np2->pos == 1)
				trajs[k++] = PointerGetDatum(gserialized_copy(
					(GSERIALIZED *)PG_DETOAST_DATUM(line)));
			else
				trajs[k++] = call_function3(LWGEOM_line_substring, line,
					Float8GetDatum(np1->pos), Float8GetDatum(np2->pos));
		}
		else if (np1->pos > np2->pos)
		{
			Datum traj;
			if (np2->pos == 0 && np1->pos == 1)
				traj = PointerGetDatum(gserialized_copy(
					(GSERIALIZED *)PG_DETOAST_DATUM(line)));
			else
				traj = call_function3(LWGEOM_line_substring, line,
					Float8GetDatum(np2->pos), Float8GetDatum(np1->pos));
			trajs[k++] = call_function1(LWGEOM_reverse, traj);
			pfree(DatumGetPointer(traj));
		}
		np1 = np2;
	}

	Datum result;
	if (k == 0)
		result = npoint_as_geom_internal(np1);
	else if (k == 1)
		result = trajs[0];
	else
	{
		ArrayType *array = datumarr_to_array(trajs, k, type_oid(T_GEOMETRY));
		Datum lines = call_function1(LWGEOM_collect_garray, 
			PointerGetDatum(array));
		result = call_function1(linemerge, lines);
		pfree(DatumGetPointer(lines));
		for (int i = 0; i < k; i++)
			pfree(DatumGetPointer(trajs[i]));
		pfree(array);
	}

	pfree(DatumGetPointer(line));
	pfree(trajs);
	return result;
}

Datum
tnpoints_trajectory(TemporalS *ts)
{
	if (ts->count == 1)
		return tnpointseq_geom(temporals_seq_n(ts, 0));

	Datum *points = palloc(sizeof(Datum) * ts->count);
	Datum *segments = palloc(sizeof(Datum) * ts->count);
	int j = 0, k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		Datum traj = tnpointseq_geom(seq);
		GSERIALIZED *gstraj = (GSERIALIZED *)PG_DETOAST_DATUM(traj);
		if (gserialized_get_type(gstraj) == POINTTYPE)
			points[j++] = traj;
		else
			segments[k++] = traj;
		POSTGIS_FREE_IF_COPY_P(gstraj, DatumGetPointer(traj));
	}

	Datum multipoint = (Datum) 0, multilinestring = (Datum) 0;  /* make compiler quiet */
	if (j > 0)
	{
		if (j == 1)
		{
			GSERIALIZED *gspoint = (GSERIALIZED *)PG_DETOAST_DATUM(points[0]);
			multipoint = PointerGetDatum(gserialized_copy(gspoint));
		}
		else
		{
			ArrayType *array = datumarr_to_array(points, j, type_oid(T_GEOMETRY));
			multipoint = call_function1(pgis_union_geometry_array,
				PointerGetDatum(array));
			pfree(array);
		}
	}
	if (k > 0)
	{
		if (k == 1)
		{
			GSERIALIZED *gsline = (GSERIALIZED *)PG_DETOAST_DATUM(segments[0]);
			multilinestring = PointerGetDatum(gserialized_copy(gsline));
		}
		else
		{
			ArrayType *array = datumarr_to_array(segments, k, type_oid(T_GEOMETRY));
			Datum lines = call_function1(LWGEOM_collect_garray,
				PointerGetDatum(array));
			multilinestring = call_function1(linemerge, lines);
			pfree(array);
		}
	}

	Datum result;
	if (j > 0 && k > 0)
	{
		result = call_function2(geomunion, multipoint, multilinestring);
		pfree(DatumGetPointer(multipoint)); pfree(DatumGetPointer(multilinestring));
	}
	else if (j > 0)
		result = multipoint;
	else
		result = multilinestring;

	pfree(points); pfree(segments);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_trajectory);

PGDLLEXPORT Datum
tnpoint_trajectory(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result = 0; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = tnpointinst_geom((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tnpointi_geom((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tnpointseq_trajectory((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS)
		result = tnpoints_trajectory((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Geometric positions functions
 * Return the geometric positions covered by the temporal npoint
 *****************************************************************************/

Datum
tnpointinst_geom(TemporalInst *inst)
{
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	return npoint_as_geom_internal(np);
}

Datum
tnpointi_geom(TemporalI *ti)
{
	if (ti->count == 1)
		return tnpointinst_geom(temporali_inst_n(ti, 0));

	int count;
	/* The following function removes duplicate values */
	Datum *values = temporali_values1(ti, &count);
	Datum *geoms = palloc(sizeof(Datum) * count);
	for (int i = 0; i < count; i++)
	{
		npoint *np = DatumGetNpoint(values[i]);
		geoms[i] = npoint_as_geom_internal(np);
	}
	ArrayType *array = datumarr_to_array(geoms, count, type_oid(T_GEOMETRY));
	Datum result = call_function1(LWGEOM_collect_garray, PointerGetDatum(array));
	pfree(values);
	for (int i = 0; i < count; i++)
		pfree(DatumGetPointer(geoms[i]));
	pfree(geoms);
	pfree(array);
	return result;
}

Datum
tnpointseq_geom(TemporalSeq *seq)
{
	if (seq->count == 1)
		return tnpointinst_geom(temporalseq_inst_n(seq, 0));

	nsegment *ns = tnpointseq_positions1(seq);
	return nsegment_as_geom_internal(ns);
}

Datum
tnpoints_geom(TemporalS *ts)
{
	if (ts->count == 1)
		return tnpointseq_geom(temporals_seq_n(ts, 0));

	nsegment **segments = tnpoints_positions1(ts);
	Datum result = nsegmentarr_to_geom_internal(segments, ts->count);
	for (int i = 0; i < ts->count; i++)
		pfree(segments[i]);
	pfree(segments);
	return result;
}

Datum
tnpoint_geom(Temporal *temp)
{
	Datum result = 0;
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = tnpointinst_geom((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = tnpointi_geom((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ) 
		result = tnpointseq_geom((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS) 
		result = tnpoints_geom((TemporalS *)temp);
	return result;
}


/*****************************************************************************
 * Length and speed functions
 *****************************************************************************/

/* Length traversed by the temporal npoint */

static double
tnpointseq_length(TemporalSeq *seq)
{
	if (seq->count == 1)
		return 0;

	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst));
	double length = route_length(np1->rid);
	double fraction = 0;
	for (int i = 1; i < seq->count; i++)
	{
		inst = temporalseq_inst_n(seq, i);
		npoint *np2 = DatumGetNpoint(temporalinst_value(inst));
		fraction += fabs(np2->pos - np1->pos);
		np1 = np2;
	}
	return length * fraction;
}

static double
tnpoints_length(TemporalS *ts)
{
	double result = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		result += tnpointseq_length(seq);
	}
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_length);

PGDLLEXPORT Datum
tnpoint_length(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	double result = 0.0; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI)
		;
	else if (temp->duration == TEMPORALSEQ)
		result = tnpointseq_length((TemporalSeq *)temp);	
	else if (temp->duration == TEMPORALS)
		result = tnpoints_length((TemporalS *)temp);	
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_FLOAT8(result);
}

/* Cumulative length traversed by the temporal npoint */

static TemporalInst *
tnpointinst_set_zero(TemporalInst *inst)
{
	return temporalinst_make(Float8GetDatum(0.0), inst->t, FLOAT8OID);
}

static TemporalI *
tnpointi_set_zero(TemporalI *ti)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	Datum zero = Float8GetDatum(0.0);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		instants[i] = temporalinst_make(zero, inst->t, FLOAT8OID);
	}
	TemporalI *result = temporali_from_temporalinstarr(instants, ti->count);
	for (int i = 1; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalSeq *
tnpointseq_cumulative_length(TemporalSeq *seq, double prevlength)
{
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = temporalinst_make(Float8GetDatum(prevlength), 
			inst->t, FLOAT8OID);
		TemporalSeq *result = temporalseq_from_temporalinstarr(&inst1, 1,
			true, true, false);
		pfree(inst1);
		return result;
	}

	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	double rlength = route_length(np1->rid);
	double length = prevlength;
	instants[0] = temporalinst_make(Float8GetDatum(length), inst1->t, FLOAT8OID);
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));
		length += fabs(np2->pos - np1->pos) * rlength;
		instants[i] = temporalinst_make(Float8GetDatum(length), inst2->t,
			FLOAT8OID);
		inst1 = inst2;
		np1 = np2;
	}
	TemporalSeq *result = temporalseq_from_temporalinstarr(instants, seq->count,
		seq->period.lower_inc, seq->period.upper_inc, false);

	for (int i = 1; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalS *
tnpoints_cumulative_length(TemporalS *ts)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	double length = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		sequences[i] = tnpointseq_cumulative_length(seq, length);
		TemporalInst *end = temporalseq_inst_n(sequences[i], seq->count - 1);
		length += DatumGetFloat8(temporalinst_value(end));
	}
	TemporalS *result = temporals_from_temporalseqarr(sequences,
		ts->count, false);

	for (int i = 1; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_cumulative_length);

PGDLLEXPORT Datum
tnpoint_cumulative_length(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tnpointinst_set_zero((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tnpointi_set_zero((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tnpointseq_cumulative_length((TemporalSeq *)temp, 0);	
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)tnpoints_cumulative_length((TemporalS *)temp);	
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/* Speed of the temporal npoint */

static TemporalSeq *
tnpointseq_speed(TemporalSeq *seq)
{
	if (seq->count == 1)
		return NULL;

	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	double rlength = route_length(np1->rid);
	TemporalInst *inst2 = NULL; /* make the compiler quiet */
	double speed = 0; /* make the compiler quiet */
	for (int i = 0; i < seq->count - 1; i++)
	{
		inst2 = temporalseq_inst_n(seq, i + 1);
		npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));
		double length = fabs(np2->pos - np1->pos) * rlength;
		speed = length / (((double)(inst2->t) - (double)(inst1->t)) / 1000000);
		instants[i] = temporalinst_make(Float8GetDatum(speed),
			inst1->t, FLOAT8OID);
		inst1 = inst2;
		np1 = np2;
	}
	instants[seq->count-1] = temporalinst_make(Float8GetDatum(speed),
	   inst2->t, FLOAT8OID);
	TemporalSeq *result = temporalseq_from_temporalinstarr(instants, seq->count, 
		seq->period.lower_inc, seq->period.upper_inc, true);

	for (int i = 0; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);

	return result;
}

static TemporalS *
tnpoints_speed(TemporalS *ts)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalSeq *seq1 = tnpointseq_speed(seq);
		if (seq1 != NULL)
			sequences[k++] = seq1;
	}
	if (k == 0)
	{
		pfree(sequences);
		return NULL;
	}
	TemporalS *result = temporals_from_temporalseqarr(sequences, k, true);
	for (int i = 0; i < k; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_speed);

PGDLLEXPORT Datum
tnpoint_speed(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tnpointinst_set_zero((TemporalInst *)temp);	
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tnpointi_set_zero((TemporalI *)temp);	
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tnpointseq_speed((TemporalSeq *)temp);	
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)tnpoints_speed((TemporalS *)temp);	
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Time-weighed centroid for temporal geometry points
 *****************************************************************************/

static Datum
tnpointi_twcentroid(TemporalI *ti)
{
    TemporalI *ti1 = tnpointi_as_tgeompointi(ti);
    return tgeompointi_twcentroid(ti1);
}

static Datum
tnpointseq_twcentroid(TemporalSeq *seq)
{
    TemporalSeq *seq1 = tnpointseq_as_tgeompointseq(seq);
    return tgeompointseq_twcentroid(seq1);
}

static Datum
tnpoints_twcentroid(TemporalS *ts)
{
    TemporalS *ts1 = tnpoints_as_tgeompoints(ts);
    return tgeompoints_twcentroid(ts1);
}
PG_FUNCTION_INFO_V1(tnpoint_twcentroid);

PGDLLEXPORT Datum
tnpoint_twcentroid(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result = 0; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = temporalinst_value_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = tnpointi_twcentroid((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ) 
		result = tnpointseq_twcentroid((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS) 
		result = tnpoints_twcentroid((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Temporal azimuth
 *****************************************************************************/

static TemporalSeq **
tnpointseq_azimuth1(TemporalInst *inst1, TemporalInst *inst2, bool lower_inc,
	int *count)
{
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));

	/* Constant segment */
	if (np1->pos == np2->pos)
	{
		*count = 0;
		return NULL;
	}

	/* Find all vertices in the segment */
	Datum traj = tnpointseq_trajectory1(inst1, inst2);
	int countVertices = DatumGetInt32(call_function1(
		LWGEOM_numpoints_linestring, traj));
	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * (countVertices - 1));
	TemporalInst *instants[2];

	Datum vertex1 = call_function2(LWGEOM_pointn_linestring, traj, Int32GetDatum(1));
	TimestampTz time1 = inst1->t;
	for (int i = 0; i < countVertices - 1; i++)
	{
		Datum vertex2 = call_function2(LWGEOM_pointn_linestring, traj, 
			Int32GetDatum(i + 2));
		double fraction = DatumGetFloat8(call_function2(
			LWGEOM_line_locate_point, traj, vertex2));
		TimestampTz time2 = (TimestampTz)(inst1->t + (inst2->t - inst1->t) * fraction);

		Datum azimuth = call_function2(LWGEOM_azimuth, vertex1, vertex2);
		bool lower_inc1 = (i == 0)? lower_inc: true;
		instants[0] = temporalinst_make(azimuth, time1, FLOAT8OID);
		instants[1] = temporalinst_make(azimuth, time2, FLOAT8OID);
		result[i] = temporalseq_from_temporalinstarr(instants, 2, 
			lower_inc1, false, false);

		pfree(instants[0]); pfree(instants[1]);
		pfree(DatumGetPointer(vertex1));
		vertex1 = vertex2;
		time1 = time2;
	}

	pfree(DatumGetPointer(traj));
	pfree(DatumGetPointer(vertex1));
	*count = countVertices - 1;
	return result;
}

static TemporalSeq **
tnpointseq_azimuth2(TemporalSeq *seq, int *count)
{
	if (seq->count == 1)
	{
		*count = 0;
		return NULL;
	}

	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * (seq->count - 1));
	int *countseqs = palloc0(sizeof(int) * (seq->count - 1));
	int totalseqs = 0;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		sequences[i] = tnpointseq_azimuth1(inst1, inst2, lower_inc, &countseqs[i]);
		totalseqs += countseqs[i];
		inst1 = inst2;
		lower_inc = true;
	}

	TemporalSeq **allsequences = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < seq->count - 1; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			allsequences[k++] = sequences[i][j];
		if (sequences[i] != NULL)
			pfree(sequences[i]);
	}

	pfree(sequences);
	pfree(countseqs);
	*count = totalseqs;
	return allsequences;
}

static TemporalS *
tnpointseq_azimuth(TemporalSeq *seq)
{
	int countseqs;
	TemporalSeq **allsequences = tnpointseq_azimuth2(seq, &countseqs);
	if (countseqs == 0)
		return NULL;

	TemporalS *result = temporals_from_temporalseqarr(allsequences, countseqs, true);
	for (int i = 0; i < countseqs; i++)
		pfree(allsequences[i]);
	pfree(allsequences);
	return result;
}

static TemporalS *
tnpoints_azimuth(TemporalS *ts)
{
	if (ts->count == 1)
		return tnpointseq_azimuth(temporals_seq_n(ts, 0));
		
	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int *countseqs = palloc0(sizeof(int) * ts->count);
	int totalseqs = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		sequences[i] = tnpointseq_azimuth2(seq, &countseqs[i]);
		totalseqs += countseqs[i];
	}
	if (totalseqs == 0)
	{
		pfree(sequences);
		pfree(countseqs);
		return NULL;
	}

	TemporalSeq **allsequences = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			allsequences[k++] = sequences[i][j];
		if (sequences[i] != NULL)
			pfree(sequences[i]);
	}
	TemporalS *result = temporals_from_temporalseqarr(allsequences, totalseqs, true);

	for (int i = 0; i < totalseqs; i++)
		pfree(allsequences[i]);
	pfree(allsequences);
	pfree(sequences);
	pfree(countseqs);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_azimuth);

PGDLLEXPORT Datum
tnpoint_azimuth(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI)
		;
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tnpointseq_azimuth((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)tnpoints_azimuth((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Restriction functions
 *****************************************************************************/

/* Restrict a temporal npoint to a geometry */

static TemporalInst *
tnpointinst_at_geometry(TemporalInst *inst, Datum geom)
{
	Datum point = tnpointinst_geom(inst);
	bool inter = DatumGetBool(call_function2(intersects, point, geom));
	pfree(DatumGetPointer(point));
	if (!inter)
		return NULL;
	return temporalinst_make(temporalinst_value(inst), inst->t, 
		inst->valuetypid);
}

static TemporalI *
tnpointi_at_geometry(TemporalI *ti, Datum geom)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	int k = 0;
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		Datum point = tnpointinst_geom(inst);
		if (DatumGetBool(call_function2(intersects, point, geom)))
			instants[k++] = temporalinst_make(temporalinst_value(inst), 
				inst->t, ti->valuetypid);
		pfree(DatumGetPointer(point));
	}
	TemporalI *result = NULL;
	if (k != 0)
	{
		result = temporali_from_temporalinstarr(instants, k);
		for (int i = 0; i < k; i++)
			pfree(instants[i]);
	}
	pfree(instants);
	return result;
}

/* This function assumes that inst1 and inst2 have same rid */
static TemporalSeq **
tnpointseq_at_geometry1(TemporalInst *inst1, TemporalInst *inst2,
	bool lower_inc, bool upper_inc, Datum geom, int *count)
{
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));

	/* Constant sequence */
	if (np1->pos == np2->pos)
	{
		Datum point = npoint_as_geom_internal(np1);
		bool inter = DatumGetBool(call_function2(intersects, point, geom));
		pfree(DatumGetPointer(point));
		if (!inter)
		{
			*count = 0;
			return NULL;
		}

		TemporalInst *instants[2];
		instants[0] = inst1;
		instants[1] = inst2;
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_from_temporalinstarr(instants, 2, 
			lower_inc, upper_inc, false);
		*count = 1;
		return result;
	}

	/* Look for intersections */
	Datum line = tnpointseq_trajectory1(inst1, inst2);
	Datum intersections = call_function2(intersection, line, geom);
	if (DatumGetBool(call_function1(LWGEOM_isempty, intersections)))
	{
		pfree(DatumGetPointer(line));
		pfree(DatumGetPointer(intersections));
		*count = 0;
		return NULL;
	}

	int countinter = DatumGetInt32(call_function1(
		LWGEOM_numgeometries_collection, intersections));
	TemporalInst *instants[2];
	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * countinter);
	int k = 0;
	TimestampTz lower = inst1->t;
	TimestampTz upper = inst2->t;

	for (int i = 1; i <= countinter; i++)
	{
		/* Find the i-th intersection */
		Datum inter = call_function2(LWGEOM_geometryn_collection, 
			intersections, Int32GetDatum(i));
		GSERIALIZED *gsinter = (GSERIALIZED *) PG_DETOAST_DATUM(inter);

		/* Each intersection is either a point or a linestring with two points */
		if (gserialized_get_type(gsinter) == POINTTYPE)
		{
			double fraction = DatumGetFloat8(call_function2(
				LWGEOM_line_locate_point, line, inter));
			TimestampTz time = (TimestampTz)(lower + (upper - lower) * fraction);

			/* If the intersection is not at the exclusive bound */
			if ((lower_inc || time > lower) && (upper_inc || time < upper))
			{
				double pos = np1->pos + (np2->pos * fraction - np1->pos * fraction);
				npoint *intnp = npoint_make(np1->rid, pos);
				instants[0] = temporalinst_make(PointerGetDatum(intnp), time,
					type_oid(T_NPOINT));
				result[k++] = temporalseq_from_temporalinstarr(instants,
					1, true, true, false);
				pfree(instants[0]);
				pfree(intnp);
			}
		}
		else
		{
			Datum inter1 = call_function2(LWGEOM_pointn_linestring, inter, 1);
			double fraction1 = DatumGetFloat8(call_function2(
				LWGEOM_line_locate_point, line, inter1));
			TimestampTz time1 = (TimestampTz)(lower + (upper - lower) * fraction1);
			double pos1 = np1->pos + (np2->pos * fraction1 - np1->pos * fraction1);
			npoint *intnp1 = npoint_make(np1->rid, pos1);

			Datum inter2 = call_function2(LWGEOM_pointn_linestring, inter, 2);
			double fraction2 = DatumGetFloat8(call_function2(
				LWGEOM_line_locate_point, line, inter2));
			TimestampTz time2 = (TimestampTz)(lower + (upper - lower) * fraction2);
			double pos2 = np1->pos + (np2->pos * fraction2 - np1->pos * fraction2);
			npoint *intnp2 = npoint_make(np1->rid, pos2);

			TimestampTz lower1 = Min(time1, time2);
			TimestampTz upper1 = Max(time1, time2);
			bool lower_inc1 = lower1 == lower? lower_inc : true;
			bool upper_inc1 = upper1 == upper? upper_inc : true;
			instants[0] = temporalinst_make(PointerGetDatum(intnp1), lower1,
				type_oid(T_NPOINT));
			instants[1] = temporalinst_make(PointerGetDatum(intnp2), upper1,
				type_oid(T_NPOINT));
			result[k++] = temporalseq_from_temporalinstarr(instants, 2,
				lower_inc1, upper_inc1, false);

			pfree(instants[0]); pfree(instants[1]);
			pfree(DatumGetPointer(inter1)); pfree(DatumGetPointer(inter2));
			pfree(intnp1); pfree(intnp2);
		}
		POSTGIS_FREE_IF_COPY_P(gsinter, DatumGetPointer(inter));
	}

	pfree(DatumGetPointer(line));
	pfree(DatumGetPointer(intersections));

	if (k == 0)
	{
		pfree(result);
		*count = 0;
		return NULL;
	}

	temporalseqarr_sort(result, k);
	*count = k;
	return result;
}

static TemporalSeq **
tnpointseq_at_geometry2(TemporalSeq *seq, Datum geom, int *count)
{
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		Datum point = tnpointinst_geom(inst);
		bool inter = DatumGetBool(call_function2(intersects, point, geom));
		pfree(DatumGetPointer(point));
		if (!inter)
		{
			*count = 0;
			return NULL;
		}
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_from_temporalinstarr(&inst, 1, 
			true, true, false);
		*count = 1;
		return result;
	}

	/* Temporal sequence has at least 2 instants */
	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * (seq->count - 1));
	int *countseqs = palloc0(sizeof(int) * (seq->count - 1));
	int totalseqs = 0;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		bool upper_inc = (i == seq->count - 2)? seq->period.upper_inc: false;
		sequences[i] = tnpointseq_at_geometry1(inst1, inst2, 
			lower_inc, upper_inc, geom, &countseqs[i]);
		totalseqs += countseqs[i];
		inst1 = inst2;
		lower_inc = true;
	}

	if (totalseqs == 0)
	{
		pfree(countseqs);
		pfree(sequences);
		*count = 0;
		return NULL;
	}

	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < seq->count - 1; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			result[k++] = sequences[i][j];
		if (sequences[i] != NULL)
			pfree(sequences[i]);
	}

	pfree(countseqs);
	pfree(sequences);
	*count = totalseqs;
	return result;
}

static TemporalS *
tnpointseq_at_geometry(TemporalSeq *seq, Datum geom)
{
	int count;
	TemporalSeq **sequences = tnpointseq_at_geometry2(seq, geom, &count);
	if (sequences == NULL)
		return NULL;

	TemporalS *result = temporals_from_temporalseqarr(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalS *
tnpoints_at_geometry(TemporalS *ts, Datum geom)
{
	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int *countseqs = palloc0(sizeof(int) * ts->count);
	int totalseqs = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		sequences[i] = tnpointseq_at_geometry2(seq, geom, &countseqs[i]);
		totalseqs += countseqs[i];
	}

	if (totalseqs == 0)
	{
		pfree(sequences);
		pfree(countseqs);
		return NULL;
	}

	TemporalSeq **allsequences = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			allsequences[k++] = sequences[i][j];
		if (sequences[i] != NULL)
			pfree(sequences[i]);
	}
	TemporalS *result = temporals_from_temporalseqarr(allsequences, totalseqs, true);

	for (int i = 0; i < totalseqs; i++)
		pfree(allsequences[i]);
	pfree(allsequences);
	pfree(sequences);
	pfree(countseqs);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_at_geometry);

PGDLLEXPORT Datum
tnpoint_at_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *result = NULL; 
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tnpointinst_at_geometry((TemporalInst *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tnpointi_at_geometry((TemporalI *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tnpointseq_at_geometry((TemporalSeq *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS)
		result = (Temporal *)tnpoints_at_geometry((TemporalS *)temp,
			PointerGetDatum(gs));
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/* Restrict a temporal point to the complement of a geometry */

static TemporalInst *
tnpointinst_minus_geometry(TemporalInst *inst, Datum geom)
{
    Datum value = npoint_as_geom_internal(DatumGetNpoint(temporalinst_value(inst)));
	if (DatumGetBool(call_function2(intersects, value, geom)))
		return NULL;
	return temporalinst_copy(inst);
}

static TemporalI *
tnpointi_minus_geometry(TemporalI *ti, Datum geom)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	int k = 0;
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		Datum value = npoint_as_geom_internal(DatumGetNpoint(temporalinst_value(inst)));
		if (!DatumGetBool(call_function2(intersects, value, geom)))
			instants[k++] = inst;
	}
	TemporalI *result = NULL;
	if (k != 0)
		result = temporali_from_temporalinstarr(instants, k);
	pfree(instants);
	return result;
}

/* 
 * It is not possible to use a similar approach as for tnpointseq_at_geometry1
 * where instead of computing the intersections we compute the difference since
 * in PostGIS the following query
 *  	select st_astext(st_difference(geometry 'Linestring(0 0,3 3)',
 *  		geometry 'MultiPoint((1 1),(2 2),(3 3))'))
 * returns "LINESTRING(0 0,3 3)". Therefore we compute tnpointseq_at_geometry1
 * and then compute the complement of the value obtained.
 */
static TemporalSeq **
tnpointseq_minus_geometry1(TemporalSeq *seq, Datum geom, int *count)
{
	int countinter;
	TemporalSeq **sequences = tnpointseq_at_geometry2(seq, geom, &countinter);
	if (countinter == 0)
	{
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_copy(seq);
		*count = 1;
		return result;
	}
		
	Period **periods = palloc(sizeof(Period) * countinter);
	for (int i = 0; i < countinter; i++)
		periods[i] = &sequences[i]->period;
	PeriodSet *ps1 = periodset_from_periodarr_internal(periods, countinter, false);
	PeriodSet *ps2 = minus_period_periodset_internal(&seq->period, ps1);
	pfree(ps1); pfree(periods);
	if (ps2 == NULL)
	{
		*count = 0;
		return NULL;
	}
	TemporalSeq **result = temporalseq_at_periodset2(seq, ps2, count);
	pfree(ps2);
	return result;
}

static TemporalS *
tnpointseq_minus_geometry(TemporalSeq *seq, Datum geom)
{
	int count;
	TemporalSeq **sequences = tnpointseq_minus_geometry1(seq, geom, &count);
	if (sequences == NULL)
		return NULL;

	TemporalS *result = temporals_from_temporalseqarr(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalS *
tnpoints_minus_geometry(TemporalS *ts, GSERIALIZED *gs, GBOX *box2)
{
	/* Singleton sequence set */
	if (ts->count == 1)
		return tnpointseq_minus_geometry(temporals_seq_n(ts, 0), 
			PointerGetDatum(gs));

	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int *countseqs = palloc0(sizeof(int) * ts->count);
	int totalseqs = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		/* Bounding box test */
		GBOX *box1 = temporalseq_bbox_ptr(seq);
		if (!overlaps_gbox_gbox_internal(box1, box2))
		{
			sequences[i] = palloc(sizeof(TemporalSeq *));
			sequences[i][0] = temporalseq_copy(seq);
			countseqs[i] = 1;
			totalseqs ++;
		}
		else
		{
			sequences[i] = tnpointseq_minus_geometry1(seq, PointerGetDatum(gs), 
				&countseqs[i]);
			totalseqs += countseqs[i];
		}
	}
	if (totalseqs == 0)
	{
		pfree(sequences); pfree(countseqs);
		return NULL;
	}

	TemporalSeq **allsequences = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			allsequences[k++] = sequences[i][j];
		if (countseqs[i] != 0)
			pfree(sequences[i]);
	}
	TemporalS *result = temporals_from_temporalseqarr(allsequences, totalseqs, true);

	for (int i = 0; i < totalseqs; i++)
		pfree(allsequences[i]);
	pfree(allsequences); pfree(sequences); pfree(countseqs);

	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_minus_geometry);

PGDLLEXPORT Datum
tnpoint_minus_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);

	/* Bounding box test */
	GBOX box1, box2;
	if (!geo_to_gbox_internal(&box2, gs))
	{
		Temporal* copy = temporal_copy(temp) ;
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(copy);
	}
	temporal_bbox(&box1, temp);
	if (!overlaps_gbox_gbox_internal(&box1, &box2))
	{
		Temporal* copy = temporal_copy(temp) ;
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(copy);
	}

	Temporal *result = NULL;
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)tnpointinst_minus_geometry((TemporalInst *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)tnpointi_minus_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)tnpointseq_minus_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)tnpoints_minus_geometry((TemporalS *)temp, gs, &box2);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Nearest approach instant
 *****************************************************************************/

static TemporalInst *
NAI_tnpointi_geometry(TemporalI *ti, Datum geom)
{
	TemporalInst *inst = temporali_inst_n(ti, 0);
	double mindist = DBL_MAX;
	int number = 0; /* keep compiler quiet */ 
	for (int i = 0; i < ti->count; i++)
	{
		inst = temporali_inst_n(ti, i);
		npoint *np = DatumGetNpoint(temporalinst_value(inst));
		Datum value = npoint_as_geom_internal(np);
		double dist = DatumGetFloat8(call_function2(distance, value, geom));	
		if (dist < mindist)
		{
			mindist = dist;
			number = i;
		}
	}
	return temporalinst_copy(temporali_inst_n(ti, number));
}

static TemporalInst *
NAI_tnpointseq_geometry(TemporalSeq *seq, Datum geom)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return temporalinst_copy(temporalseq_inst_n(seq, 0));

	double mindist = DBL_MAX;
	TimestampTz t = 0; /* keep compiler quiet */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	for (int i = 0; i < seq->count-1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i+1);
		Datum traj = tnpointseq_trajectory1(inst1, inst2);
		Datum point = call_function2(LWGEOM_closestpoint, traj, geom);
		double dist = DatumGetFloat8(call_function2(distance, point, geom));
		if (dist < mindist)
		{
			mindist = dist;
			GSERIALIZED *gstraj = (GSERIALIZED *)DatumGetPointer(traj);
			if (gserialized_get_type(gstraj) == POINTTYPE)
				t = inst1->t;
			else
			{
				double fraction = DatumGetFloat8(call_function2(
					LWGEOM_line_locate_point, traj, point));
				t = inst1->t + (inst2->t - inst1->t) * fraction;
			}
		}
		inst1 = inst2;
		pfree(DatumGetPointer(traj)); 
		pfree(DatumGetPointer(point)); 			
	}
	TemporalInst *result = temporalseq_at_timestamp(seq, t);
	/* If t is at an exclusive bound */
	 if (result == NULL)
	 {
		if (timestamp_cmp_internal(seq->period.lower, t) == 0)
			result = temporalinst_copy(temporalseq_inst_n(seq, 0));
		else
			result = temporalinst_copy(temporalseq_inst_n(seq, seq->count - 1));
	 }
	return result;
}

static TemporalInst *
NAI_tnpoints_geometry(TemporalS *ts, Datum geom)
{
	TemporalInst *result = NULL;
	double mindist = DBL_MAX;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalInst *inst = NAI_tnpointseq_geometry(seq, geom);
		npoint *np = DatumGetNpoint(temporalinst_value(inst));
		Datum value = npoint_as_geom_internal(np);
		double dist = DatumGetFloat8(call_function2(distance, value, geom));
		if (dist < mindist)
		{
			if (result != NULL)
				pfree(result);
			result = inst;
		}
		else
			pfree(inst);
	}
	return result;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(NAI_geometry_tnpoint);

PGDLLEXPORT Datum
NAI_geometry_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *result = NULL;
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)NAI_tnpoints_geometry((TemporalS *)temp,
			PointerGetDatum(gs));
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAI_npoint_tnpoint);

PGDLLEXPORT Datum
NAI_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);

	Temporal *result = NULL;
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)NAI_tnpoints_geometry((TemporalS *)temp,
			PointerGetDatum(gs));
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAI_tnpoint_geometry);

PGDLLEXPORT Datum
NAI_tnpoint_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *result = NULL;
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)NAI_tnpoints_geometry((TemporalS *)temp,
			PointerGetDatum(gs));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAI_tnpoint_npoint);

PGDLLEXPORT Datum
NAI_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);

	Temporal *result = NULL;
	temporal_duration_is_valid(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALS) 
		result = (Temporal *)NAI_tnpoints_geometry((TemporalS *)temp,
			PointerGetDatum(gs));
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
    PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAI_tnpoint_tnpoint);

PGDLLEXPORT Datum
NAI_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	TemporalInst *result = NULL;
	Temporal *dist = distance_tnpoint_tnpoint_internal(temp1, temp2);
	if (dist != NULL)
	{
		Temporal *mindist = temporal_at_min_internal(dist);
		TimestampTz t = temporal_start_timestamp_internal(mindist);
		result = temporal_at_timestamp_internal(temp1, t);
		pfree(dist); pfree(mindist);
	}
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Nearest approach distance
 *****************************************************************************/

PG_FUNCTION_INFO_V1(NAD_geometry_tnpoint);

PGDLLEXPORT Datum
NAD_geometry_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(distance, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(NAD_npoint_tnpoint);

PGDLLEXPORT Datum
NAD_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(distance, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));	
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAD_tnpoint_geometry);

PGDLLEXPORT Datum
NAD_tnpoint_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(distance, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(NAD_tnpoint_npoint);

PGDLLEXPORT Datum
NAD_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(distance, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));	
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
    PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAD_tnpoint_tnpoint);

PGDLLEXPORT Datum
NAD_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *dist = distance_tnpoint_tnpoint_internal(temp1, temp2);
	if (dist == NULL)
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}

	Datum result = temporal_min_value_internal(dist);
	pfree(dist);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * ShortestLine
 *****************************************************************************/

PG_FUNCTION_INFO_V1(shortestline_geometry_tnpoint);

PGDLLEXPORT Datum
shortestline_geometry_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(LWGEOM_shortestline2d, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(shortestline_npoint_tnpoint);

PGDLLEXPORT Datum
shortestline_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(LWGEOM_shortestline2d, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));	
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(shortestline_tnpoint_geometry);

PGDLLEXPORT Datum
shortestline_tnpoint_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(LWGEOM_shortestline2d, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(shortestline_tnpoint_npoint);

PGDLLEXPORT Datum
shortestline_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
	Datum traj = tnpoint_geom(temp);
	Datum result = call_function2(LWGEOM_shortestline2d, traj, PointerGetDatum(gs));
	pfree(DatumGetPointer(traj));	
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
	pfree(DatumGetPointer(geom));
    PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
/* These functions suppose that the temporal values overlap in time */

static Datum
shortestline_tnpointinst_tnpointinst(TemporalInst *inst1, TemporalInst *inst2)
{
	Datum value1 = tnpointinst_geom(inst1);
	Datum value2 = tnpointinst_geom(inst2);
	Datum result = geompoint_trajectory(value1, value2);
	pfree(DatumGetPointer(value1)); pfree(DatumGetPointer(value2));
	return result;
}

static Datum
shortestline_tnpointi_tnpointi(TemporalI *ti1, TemporalI *ti2)
{
	/* Compute the distance */
	TemporalI *dist = tspatialrel_tnpointi_tnpointi(ti1, ti2, 
		&geom_distance2d, FLOAT8OID);
	TemporalI *mindist = temporali_at_min(dist);
	TimestampTz t = temporali_start_timestamp(mindist);
	TemporalInst *inst1 = temporali_at_timestamp(ti1, t);
	TemporalInst *inst2 = temporali_at_timestamp(ti2, t);
	Datum value1 = tnpointinst_geom(inst1);
	Datum value2 = tnpointinst_geom(inst2);
	Datum result = geompoint_trajectory(value1, value2);
	pfree(dist); pfree(mindist); pfree(inst1); pfree(inst2);
	pfree(DatumGetPointer(value1)); pfree(DatumGetPointer(value2));
	return result;
}

static Datum
shortestline_tnpointseq_tnpointseq(TemporalSeq *seq1, TemporalSeq *seq2)
{
	/* Compute the distance */
	TemporalSeq *dist = distance_tnpointseq_tnpointseq(seq1, seq2);
	TemporalS *mindist = temporalseq_at_min(dist);
	TimestampTz t = temporals_start_timestamp(mindist);
	/* Make a copy of the sequences with inclusive bounds */
	TemporalSeq *newseq1 = temporalseq_copy(seq1);
	newseq1->period.lower_inc = newseq1->period.upper_inc = true;
	TemporalSeq *newseq2 = temporalseq_copy(seq2);
	newseq2->period.lower_inc = newseq2->period.upper_inc = true;
	TemporalInst *inst1 = temporalseq_at_timestamp(newseq1, t);
	TemporalInst *inst2 = temporalseq_at_timestamp(newseq2, t);
	Datum value1 = tnpointinst_geom(inst1);
	Datum value2 = tnpointinst_geom(inst2);
	Datum result = geompoint_trajectory(value1, value2);
	pfree(dist); pfree(mindist); pfree(newseq1); pfree(newseq2);
	pfree(inst1); pfree(inst2);
	pfree(DatumGetPointer(value1)); pfree(DatumGetPointer(value2));
	return result;
}

static Datum
shortestline_tnpoints_tnpoints(TemporalS *ts1, TemporalS *ts2)
{
	/* Compute the distance */
	TemporalS *dist = distance_tnpoints_tnpoints(ts1, ts2);
	TemporalS *mindist = temporals_at_min(dist);
	TimestampTz t = temporals_start_timestamp(mindist);
	TemporalInst *inst1 = temporals_at_timestamp(ts1, t);
	TemporalInst *inst2 = temporals_at_timestamp(ts2, t);
	
	/* If t is at an exclusive bound */
	bool freeinst1 = (inst1 != NULL);
	if (inst1 == NULL)
	{
		int pos;
		temporals_find_timestamp(ts1, t, &pos);
		if (pos == 0)
		{
			TemporalSeq *seq = temporals_seq_n(ts1, 0);
			inst1 = temporalseq_inst_n(seq, 0);
		}
		else if (pos == ts1->count)
		{
			TemporalSeq *seq = temporals_seq_n(ts1, ts1->count-1);
			inst1 = temporalseq_inst_n(seq, seq->count-1);
		}
		else
		{
			TemporalSeq *seq1 = temporals_seq_n(ts1, pos-1);
			TemporalSeq *seq2 = temporals_seq_n(ts1, pos);
			if (timestamp_cmp_internal(temporalseq_end_timestamp(seq1), t) == 0)
				inst1 = temporalseq_inst_n(seq1, seq1->count-1);
			else
				inst1 = temporalseq_inst_n(seq2, 0);
			}		
	}
	
	/* If t is at an exclusive bound */
	bool freeinst2 = (inst2 != NULL);
	if (inst2 == NULL)
	{
		int pos;
		temporals_find_timestamp(ts2, t, &pos);
		if (pos == 0)
		{
			TemporalSeq *seq = temporals_seq_n(ts2, 0);
			inst2 = temporalseq_inst_n(seq, 0);
		}
		else if (pos == ts2->count)
		{
			TemporalSeq *seq = temporals_seq_n(ts2, ts2->count-1);
			inst2 = temporalseq_inst_n(seq, seq->count-1);
		}
		else
		{
			TemporalSeq *seq1 = temporals_seq_n(ts2, pos-1);
			TemporalSeq *seq2 = temporals_seq_n(ts2, pos);
			if (timestamp_cmp_internal(temporalseq_end_timestamp(seq1), t) == 0)
				inst2 = temporalseq_inst_n(seq1, seq1->count-1);
			else
				inst2 = temporalseq_inst_n(seq2, 0);
			}		
	}
	Datum value1 = tnpointinst_geom(inst1);
	Datum value2 = tnpointinst_geom(inst2);
	Datum result = geompoint_trajectory(value1, value2);
	pfree(dist); pfree(mindist); 
	if (freeinst1)
		pfree(inst1); 
	if (freeinst2)
		pfree(inst2);
	pfree(DatumGetPointer(value1)); pfree(DatumGetPointer(value2));
	return result;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(shortestline_tnpoint_tnpoint);

PGDLLEXPORT Datum
shortestline_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *sync1, *sync2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!synchronize_temporal_temporal(temp1, temp2, &sync1, &sync2, true))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}
	
	Datum result = 0;
	temporal_duration_is_valid(sync1->duration);
	if (sync1->duration == TEMPORALINST)
		result = shortestline_tnpointinst_tnpointinst((TemporalInst *)sync1,
			(TemporalInst *)sync2);
	else if (sync1->duration == TEMPORALI)
		result = shortestline_tnpointi_tnpointi((TemporalI *)sync1,
			(TemporalI *)sync2);
	else if (sync1->duration == TEMPORALSEQ)
		result = shortestline_tnpointseq_tnpointseq((TemporalSeq *)sync1,
			(TemporalSeq *)sync2);
	else if (sync1->duration == TEMPORALS)
		result = shortestline_tnpoints_tnpoints((TemporalS *)sync1,
			(TemporalS *)sync2);
	pfree(sync1); pfree(sync2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************/
