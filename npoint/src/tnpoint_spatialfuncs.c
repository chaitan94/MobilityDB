/*****************************************************************************
 *
 * tnpoint_spatialfuncs.c
 *	  Geospatial functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_spatialfuncs.h"

#include <float.h>

#include "periodset.h"
#include "timeops.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "tpoint_spatialfuncs.h"
#include "tpoint_boxops.h"
#include "tpoint_distance.h"
#include "tnpoint.h"
#include "tnpoint_static.h"
#include "tnpoint_distance.h"
#include "tnpoint_tempspatialrels.h"

/*****************************************************************************
 * Parameter tests
 *****************************************************************************/

void
ensure_same_srid_tnpoint(const Temporal *temp1, const Temporal *temp2)
{
	if (tnpoint_srid_internal(temp1) != tnpoint_srid_internal(temp2))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal network points must be in the same SRID")));
}

void
ensure_same_srid_tnpoint_stbox(const Temporal *temp, const STBOX *box)
{
	if (MOBDB_FLAGS_GET_X(box->flags) &&
		tnpoint_srid_internal(temp) != box->srid)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal netwokd point and the box must be in the same SRID")));
}

void
ensure_same_srid_tnpoint_gs(const Temporal *temp, const GSERIALIZED *gs)
{
	if (tnpoint_srid_internal(temp) != gserialized_get_srid(gs))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal network point and the geometry must be in the same SRID")));
}

void
ensure_same_srid_tnpoint_npoint(const Temporal *temp, const npoint *np)
{
	if (tnpoint_srid_internal(temp) != npoint_srid_internal(np))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal network point and the geometry must be in the same SRID")));
}

/*****************************************************************************
 * Functions for spatial reference systems
 *****************************************************************************/

/* Get the spatial reference system identifier (SRID) of a temporal network point */

int
tnpointinst_srid(const TemporalInst *inst)
{
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	Datum line = route_geom(np->rid);
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(line);
	int result = gserialized_get_srid(gs);
	pfree(DatumGetPointer(line));
	return result;
}

int
tnpoint_srid_internal(const Temporal *temp)
{
	int result = 0;
	ensure_valid_duration(temp->duration);
	ensure_point_base_type(temp->valuetypid) ;
	if (temp->duration == TEMPORALINST)
		result = tnpointinst_srid((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tpointi_srid((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tpointseq_srid((TemporalSeq *)temp);
	else if (temp->duration == TEMPORALS)
		result = tpoints_srid((TemporalS *)temp);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_srid);

PGDLLEXPORT Datum
tnpoint_srid(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	int result = tnpoint_srid_internal(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_INT32(result);
}

/*****************************************************************************
 * Trajectory functions
 *****************************************************************************/

Datum
tnpointseq_trajectory1(const TemporalInst *inst1, const TemporalInst *inst2)
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

/*****************************************************************************
 * Geometric positions functions
 * Return the geometric positions covered by the temporal npoint
 *****************************************************************************/

Datum
tnpointinst_geom(const TemporalInst *inst)
{
	npoint *np = DatumGetNpoint(temporalinst_value(inst));
	return npoint_as_geom_internal(np);
}

Datum
tnpointi_geom(const TemporalI *ti)
{
	/* Instantaneous sequence */
	if (ti->count == 1)
		return tnpointinst_geom(temporali_inst_n(ti, 0));

	int count;
	nsegment **segments = tnpointi_positions(ti, &count);
	Datum result = nsegmentarr_to_geom_internal(segments, count);
	for (int i = 0; i < count; i++)
		pfree(segments[i]);
	pfree(segments);
	return result;
}

Datum
tnpointseq_geom(const TemporalSeq *seq)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return tnpointinst_geom(temporalseq_inst_n(seq, 0));

	int count;
	nsegment **segments = tnpointseq_positions(seq, &count);
	Datum result = nsegmentarr_to_geom_internal(segments, count);
	for (int i = 0; i < count; i++)
		pfree(segments[i]);
	pfree(segments);
	return result;
}

Datum
tnpoints_geom(const TemporalS *ts)
{
	/* Singleton sequence set */
	if (ts->count == 1)
		return tnpointseq_geom(temporals_seq_n(ts, 0));

	int count;
	nsegment **segments = tnpoints_positions(ts, &count);
	Datum result = nsegmentarr_to_geom_internal(segments, count);
	for (int i = 0; i < count; i++)
		pfree(segments[i]);
	pfree(segments);
	return result;
}

Datum
tnpoint_geom(const Temporal *temp)
{
	int count;
	nsegment **segments = tnpoint_positions_internal(temp, &count);
	Datum result = nsegmentarr_to_geom_internal(segments, count);
	for (int i = 0; i < count; i++)
		pfree(segments[i]);
	pfree(segments);
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_trajectory);

PGDLLEXPORT Datum
tnpoint_trajectory(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result = tnpoint_geom(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Geographical equality for network points
 * Two network points may be have different rid but represent the same
 * geographical point at the intersection of the two rids
 *****************************************************************************/

bool
npoint_same_internal(const npoint *np1, const npoint *np2)
{
	/* Same route identifier */
	if (np1->rid == np2->rid)
		return fabs(np1->pos - np2->pos) < EPSILON;
	Datum point1 = npoint_as_geom_internal(np1);
	Datum point2 = npoint_as_geom_internal(np2);
	bool result = datum_eq(point1, point2, type_oid(T_GEOMETRY));
	pfree(DatumGetPointer(point1)); pfree(DatumGetPointer(point2));
	return result;
}

PG_FUNCTION_INFO_V1(npoint_same);

PGDLLEXPORT Datum
npoint_same(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	PG_RETURN_BOOL(npoint_same_internal(np1, np2));
}

/*****************************************************************************
 * Length functions
 *****************************************************************************/

/* Length traversed by the temporal npoint */

static double
tnpointseq_length(const TemporalSeq *seq)
{
	/* Instantaneous sequence */
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
tnpoints_length(const TemporalS *ts)
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
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI ||
		(temp->duration == TEMPORALSEQ && ! MOBDB_FLAGS_GET_LINEAR(temp->flags)) ||
		(temp->duration == TEMPORALS && ! MOBDB_FLAGS_GET_LINEAR(temp->flags)))
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
tnpointinst_set_zero(const TemporalInst *inst)
{
	return temporalinst_make(Float8GetDatum(0.0), inst->t, FLOAT8OID);
}

static TemporalI *
tnpointi_set_zero(const TemporalI *ti)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	Datum zero = Float8GetDatum(0.0);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		instants[i] = temporalinst_make(zero, inst->t, FLOAT8OID);
	}
	TemporalI *result = temporali_make(instants, ti->count);
	for (int i = 1; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalSeq *
tnpointseq_cumulative_length(const TemporalSeq *seq, double prevlength)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = temporalinst_make(Float8GetDatum(prevlength), 
			inst->t, FLOAT8OID);
		TemporalSeq *result = temporalseq_make(&inst1, 1,
			true, true, true, false);
		pfree(inst1);
		return result;
	}

	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	/* Stepwise interpolation */
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		Datum length = Float8GetDatum(0.0);
		for (int i = 0; i < seq->count; i++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, i);
			instants[i] = temporalinst_make(length, inst->t, FLOAT8OID);
		}
	}
	else
	/* Linear interpolation */
	{
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
	}
	TemporalSeq *result = temporalseq_make(instants,
		seq->count, seq->period.lower_inc, seq->period.upper_inc,
		MOBDB_FLAGS_GET_LINEAR(seq->flags), false);

	for (int i = 1; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalS *
tnpoints_cumulative_length(const TemporalS *ts)
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
	TemporalS *result = temporals_make(sequences, ts->count, false);

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
	ensure_valid_duration(temp->duration);
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

/*****************************************************************************
 * Speed functions
 *****************************************************************************/

static TemporalSeq *
tnpointseq_speed(const TemporalSeq *seq)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return NULL;

	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	/* Stepwise interpolation */
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		Datum length = Float8GetDatum(0.0);
		for (int i = 0; i < seq->count; i++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, i);
			instants[i] = temporalinst_make(length, inst->t, FLOAT8OID);
		}
	}
	else
	/* Linear interpolation */
	{
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
	}
	/* The resulting sequence has stepwise interpolation */
	TemporalSeq *result = temporalseq_make(instants, seq->count,
		seq->period.lower_inc, seq->period.upper_inc, false, true);

	for (int i = 0; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);

	return result;
}

static TemporalS *
tnpoints_speed(const TemporalS *ts)
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
	/* The resulting sequence set has stepwise interpolation */
	TemporalS *result = temporals_make(sequences, k, true);
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
	ensure_valid_duration(temp->duration);
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

PG_FUNCTION_INFO_V1(tnpoint_twcentroid);

PGDLLEXPORT Datum
tnpoint_twcentroid(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *tgeom = tnpoint_as_tgeompoint_internal(temp);
	Datum result = tgeompoint_twcentroid_internal(tgeom);
	pfree(tgeom);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Temporal azimuth
 *****************************************************************************/

static TemporalInst **
tnpointseq_azimuth1(const TemporalInst *inst1, const TemporalInst *inst2,
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
	TemporalInst **result = palloc(sizeof(TemporalInst *) * countVertices);
	Datum vertex1 = call_function2(LWGEOM_pointn_linestring, traj, 
		Int32GetDatum(1)); /* 1-based */
	Datum azimuth;
	TimestampTz time = inst1->t;
	for (int i = 0; i < countVertices - 1; i++)
	{
		Datum vertex2 = call_function2(LWGEOM_pointn_linestring, traj, 
			Int32GetDatum(i + 2)); /* 1-based */
		double fraction = DatumGetFloat8(call_function2(
			LWGEOM_line_locate_point, traj, vertex2));
		azimuth = call_function2(LWGEOM_azimuth, vertex1, vertex2);
		result[i] = temporalinst_make(azimuth, time, FLOAT8OID);
		pfree(DatumGetPointer(vertex1));
		vertex1 = vertex2;
		time = 	inst1->t + (inst2->t - inst1->t) * fraction;
	}
	pfree(DatumGetPointer(traj));
	pfree(DatumGetPointer(vertex1));
	*count = countVertices - 1;
	return result;
}

static int
tnpointseq_azimuth2(TemporalSeq **result, const TemporalSeq *seq)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return 0;

	TemporalInst ***instants = palloc(sizeof(TemporalInst *) * (seq->count - 1));
	int *countinsts = palloc0(sizeof(int) * (seq->count - 1));
	int totalinsts = 0; /* number of created instants so far */
	int l = 0; /* number of created sequences */
	int m = 0; /* index of the segment from which to assemble instants */
	Datum last_value;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		instants[i] = tnpointseq_azimuth1(inst1, inst2, &countinsts[i]);
		/* If constant segment */
		if (countinsts[i] == 0)
		{
			/* Assemble all instants created so far */
			if (totalinsts != 0)
			{
				TemporalInst **allinstants = palloc(sizeof(TemporalInst *) * (totalinsts + 1));
				int n = 0;
				for (int j = m; j < i; j++)
				{
					for (int k = 0; k < countinsts[j]; k++)
						allinstants[n++] = instants[j][k];
					if (instants[j] != NULL)
						pfree(instants[j]);
				}
				/* Add closing instant */
				last_value = temporalinst_value(allinstants[n - 1]);
				allinstants[n++] = temporalinst_make(last_value, inst1->t, FLOAT8OID);
				/* Resulting sequence has stepwise interpolation */
				result[l++] = temporalseq_make(allinstants,
					n, lower_inc, true, false, true);
				for (int j = 0; j < n; j++)
					pfree(allinstants[j]);
				pfree(allinstants);
				/* Indicate that we have consommed all instants created so far */
				m = i;
				totalinsts = 0;
			}
		}
		else
		{
			totalinsts += countinsts[i];
		}
		inst1 = inst2;
		lower_inc = true;
	}
	if (totalinsts != 0)
	{
		/* Assemble all instants created so far */
		TemporalInst **allinstants = palloc(sizeof(TemporalInst *) * (totalinsts + 1));
		int n = 0;
		for (int j = m; j < seq->count - 1; j++)
		{
			for (int k = 0; k < countinsts[j]; k++)
				allinstants[n++] = instants[j][k];
			if (instants[j] != NULL)
				pfree(instants[j]);
		}
		/* Add closing instant */
		last_value = temporalinst_value(allinstants[n - 1]);
		allinstants[n++] = temporalinst_make(last_value, inst1->t, FLOAT8OID);
		/* Resulting sequence has stepwise interpolation */
		result[l++] = temporalseq_make(allinstants,
			n, lower_inc, true, false, true);
		for (int j = 0; j < n; j++)
			pfree(allinstants[j]);
		pfree(allinstants);
	}
	pfree(instants);
	pfree(countinsts);
	return l;
}

static TemporalS *
tnpointseq_azimuth(const TemporalSeq *seq)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * (seq->count - 1));
	int count = tnpointseq_azimuth2(sequences, seq);
	if (count == 0)
	{
		pfree(sequences);
		return NULL;
	}

	/* Resulting sequence set has stepwise interpolation */
	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

static TemporalS *
tnpoints_azimuth(const TemporalS *ts)
{
	if (ts->count == 1)
		return tnpointseq_azimuth(temporals_seq_n(ts, 0));
		
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->totalcount);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		int countstep = tnpointseq_azimuth2(&sequences[k], seq);
		k += countstep;
	}
	if (k == 0)
		return NULL;

	/* Resulting sequence set has stepwise interpolation */
	TemporalS *result = temporals_make(sequences, k, true);

	for (int i = 0; i < k; i++)
		pfree(sequences[i]);
	pfree(sequences);
	
	return result;
}

PG_FUNCTION_INFO_V1(tnpoint_azimuth);

PGDLLEXPORT Datum
tnpoint_azimuth(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL; 
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI ||
		(temp->duration == TEMPORALSEQ && ! MOBDB_FLAGS_GET_LINEAR(temp->flags)) ||
		(temp->duration == TEMPORALS && ! MOBDB_FLAGS_GET_LINEAR(temp->flags)))
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
tnpointinst_at_geometry(const TemporalInst *inst, Datum geom)
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
tnpointi_at_geometry(const TemporalI *ti, Datum geom)
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
		result = temporali_make(instants, k);
		for (int i = 0; i < k; i++)
			pfree(instants[i]);
	}
	pfree(instants);
	return result;
}

/* This function assumes that inst1 and inst2 have same rid */
static TemporalSeq **
tnpointseq_at_geometry1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool linear, bool lower_inc, bool upper_inc, Datum geom, int *count)
{
	npoint *np1 = DatumGetNpoint(temporalinst_value(inst1));
	npoint *np2 = DatumGetNpoint(temporalinst_value(inst2));

	/* Constant sequence */
	if (np1->pos == np2->pos || linear)
	{
		Datum point = npoint_as_geom_internal(np1);
		bool inter = DatumGetBool(call_function2(intersects, point, geom));
		pfree(DatumGetPointer(point));
		if (!inter)
		{
			*count = 0;
			return NULL;
		}

		const TemporalInst *instants[]= {inst1, inst2};
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_make((TemporalInst **) instants, 2,
			lower_inc, upper_inc, linear, false);
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
				result[k++] = temporalseq_make(instants, 1,
					true, true, linear, false);
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
			result[k++] = temporalseq_make(instants, 2,
				lower_inc1, upper_inc1, linear, false);

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
tnpointseq_at_geometry2(const TemporalSeq *seq, Datum geom, int *count)
{
	/* Instantaneous sequence */
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
		result[0] = temporalseq_make(&inst, 1,
			true, true, true, false);
		*count = 1;
		return result;
	}

	/* Temporal sequence has at least 2 instants */
	bool linear = MOBDB_FLAGS_GET_LINEAR(seq->flags);
	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * (seq->count - 1));
	int *countseqs = palloc0(sizeof(int) * (seq->count - 1));
	int totalseqs = 0;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		bool upper_inc = (i == seq->count - 2)? seq->period.upper_inc: false;
		sequences[i] = tnpointseq_at_geometry1(inst1, inst2, linear,
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
tnpointseq_at_geometry(const TemporalSeq *seq, Datum geom)
{
	int count;
	TemporalSeq **sequences = tnpointseq_at_geometry2(seq, geom, &count);
	if (sequences == NULL)
		return NULL;

	TemporalS *result = temporals_make(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalS *
tnpoints_at_geometry(const TemporalS *ts, Datum geom)
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
	TemporalS *result = temporals_make(allsequences, totalseqs, true);

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
	ensure_valid_duration(temp->duration);
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
tnpointinst_minus_geometry(const TemporalInst *inst, Datum geom)
{
    Datum value = npoint_as_geom_internal(DatumGetNpoint(temporalinst_value(inst)));
	if (DatumGetBool(call_function2(intersects, value, geom)))
		return NULL;
	return temporalinst_copy(inst);
}

static TemporalI *
tnpointi_minus_geometry(const TemporalI *ti, Datum geom)
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
		result = temporali_make(instants, k);
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
tnpointseq_minus_geometry1(const TemporalSeq *seq, Datum geom, int *count)
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
	PeriodSet *ps1 = periodset_make_internal(periods, countinter, false);
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
tnpointseq_minus_geometry(const TemporalSeq *seq, Datum geom)
{
	int count;
	TemporalSeq **sequences = tnpointseq_minus_geometry1(seq, geom, &count);
	if (sequences == NULL)
		return NULL;

	TemporalS *result = temporals_make(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalS *
tnpoints_minus_geometry(const TemporalS *ts,  Datum geom, const STBOX *box2)
{
	/* Singleton sequence set */
	if (ts->count == 1)
		return tnpointseq_minus_geometry(temporals_seq_n(ts, 0), geom);

	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int *countseqs = palloc0(sizeof(int) * ts->count);
	int totalseqs = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		/* Bounding box test */
		STBOX *box1 = temporalseq_bbox_ptr(seq);
		if (!overlaps_stbox_stbox_internal(box1, box2))
		{
			sequences[i] = palloc(sizeof(TemporalSeq *));
			sequences[i][0] = temporalseq_copy(seq);
			countseqs[i] = 1;
			totalseqs ++;
		}
		else
		{
			sequences[i] = tnpointseq_minus_geometry1(seq, geom,
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
	TemporalS *result = temporals_make(allsequences, totalseqs, true);

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
	STBOX box1, box2;
	if (!geo_to_stbox_internal(&box2, gs))
	{
		Temporal* copy = temporal_copy(temp) ;
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(copy);
	}
	temporal_bbox(&box1, temp);
	if (!overlaps_stbox_stbox_internal(&box1, &box2))
	{
		Temporal* copy = temporal_copy(temp) ;
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(copy);
	}

	Temporal *result = NULL;
	ensure_valid_duration(temp->duration);
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
		result = (Temporal *)tnpoints_minus_geometry((TemporalS *)temp,
			PointerGetDatum(gs), &box2);
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
NAI_tnpointi_geometry(const TemporalI *ti, Datum geom)
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
NAI_tnpointseq_geometry(const TemporalSeq *seq, Datum geom)
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
NAI_tnpoints_geometry(const TemporalS *ts, Datum geom)
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
	ensure_valid_duration(temp->duration);
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
	ensure_valid_duration(temp->duration);
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
	ensure_valid_duration(temp->duration);
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
	ensure_valid_duration(temp->duration);
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
shortestline_tnpointinst_tnpointinst(const TemporalInst *inst1,
	const TemporalInst *inst2)
{
	Datum value1 = tnpointinst_geom(inst1);
	Datum value2 = tnpointinst_geom(inst2);
	Datum result = geompoint_trajectory(value1, value2);
	pfree(DatumGetPointer(value1)); pfree(DatumGetPointer(value2));
	return result;
}

static Datum
shortestline_tnpointi_tnpointi(const TemporalI *ti1, const TemporalI *ti2)
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
shortestline_tnpointseq_tnpointseq(const TemporalSeq *seq1,
	const TemporalSeq *seq2)
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
shortestline_tnpoints_tnpoints(const TemporalS *ts1, const TemporalS *ts2)
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
	ensure_valid_duration(sync1->duration);
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
