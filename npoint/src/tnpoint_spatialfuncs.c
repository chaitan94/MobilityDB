/*****************************************************************************
 *
 * tnpoint_spatialfuncs.c
 *	  Geospatial functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_spatialfuncs.h"

#include <assert.h>
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
			errmsg("The temporal network point and the box must be in the same SRID")));
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
			errmsg("The temporal network point and the network point must be in the same SRID")));
}

/*****************************************************************************
 * Functions for spatial reference systems
 *****************************************************************************/

/* Spatial reference system identifier (SRID) of a temporal network point.
 * For temporal points of duration distinct from TEMPORALINST the SRID is
 * obtained from the bounding box. */

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
	int result;
	ensure_valid_duration(temp->duration);
	ensure_point_base_type(temp->valuetypid) ;
	if (temp->duration == TEMPORALINST)
		result = tnpointinst_srid((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tpointi_srid((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tpointseq_srid((TemporalSeq *)temp);
	else /* temp->duration == TEMPORALS */
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
	assert(np1->rid == np2->rid);

	if (np1->pos == np2->pos)
		return npoint_as_geom_internal(np1);

	Datum line = route_geom(np1->rid);
	if ((np1->pos == 0 && np2->pos == 1) ||
		(np2->pos == 0 && np1->pos == 1))
		return line;

	Datum traj;
	if (np1->pos < np2->pos)
		traj = call_function3(LWGEOM_line_substring, line,
			Float8GetDatum(np1->pos), Float8GetDatum(np2->pos));
	else /* np1->pos < np2->pos */
	{
		Datum traj2 = call_function3(LWGEOM_line_substring, line,
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

/*
 * NPoints functions
 * Return the network points covered by the moving object
 * Only the particular cases returning points are covered
 */

npoint **
tnpointi_npoints(const TemporalI *ti, int *count)
{
	npoint **result = palloc(sizeof(npoint *) * ti->count);
	result[0] =  DatumGetNpoint(temporalinst_value(temporali_inst_n(ti, 0)));
	int k = 1;
	for (int i = 1; i < ti->count; i++)
	{
		npoint *np = DatumGetNpoint(temporalinst_value(temporali_inst_n(ti, i)));
		bool found = false;
		for (int j = 0; j < k; j++)
		{
			if (npoint_eq_internal(np, result[j]))
			{
				found = true;
				break;
			}
		}
		if (!found)
			result[k++] = np;
	}
	*count = k;
	return result;
}

npoint **
tnpointseq_step_npoints(const TemporalSeq *seq, int *count)
{
	npoint **result = palloc(sizeof(npoint *) * seq->count);
	result[0] =  DatumGetNpoint(temporalinst_value(temporalseq_inst_n(seq, 0)));
	int k = 1;
	for (int i = 1; i < seq->count; i++)
	{
		npoint *np = DatumGetNpoint(temporalinst_value(temporalseq_inst_n(seq, i)));
		bool found = false;
		for (int j = 0; j < k; j++)
		{
			if (npoint_eq_internal(np, result[j]))
			{
				found = true;
				break;
			}
		}
		if (!found)
			result[k++] = np;
	}
	*count = k;
	return result;
}

npoint **
tnpoints_step_npoints(const TemporalS *ts, int *count)
{
	npoint **result = palloc(sizeof(npoint *) * ts->totalcount);
	TemporalSeq *seq = temporals_seq_n(ts, 0);
	result[0] =  DatumGetNpoint(temporalinst_value(temporalseq_inst_n(seq, 0)));
	int l = 1;
	for (int i = 1; i < ts->count; i++)
	{
		seq = temporals_seq_n(ts, i);
		for (int j = 1; j < seq->count; j++)
		{
			npoint *np = DatumGetNpoint(temporalinst_value(temporalseq_inst_n(seq, j)));
			bool found = false;
			for (int k = 0; k < l; k++)
			{
				if (npoint_eq_internal(np, result[k]))
				{
					found = true;
					break;
				}
			}
			if (!found)
				result[l++] = np;
		}
	}
	*count = l;
	return result;
}

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
	/* The following function removes duplicate values */
	npoint **points = tnpointi_npoints(ti, &count);
	Datum result = npointarr_to_geom_internal(points, count);
	pfree(points);
	return result;
}

Datum
tnpointseq_geom(const TemporalSeq *seq)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return tnpointinst_geom(temporalseq_inst_n(seq, 0));

	Datum result;
	if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		nsegment *segment = tnpointseq_linear_positions(seq);
		result = nsegment_as_geom_internal(segment);
		pfree(segment);
	}
	else
	{
		int count;
		/* The following function removes duplicate values */
		npoint **points = tnpointseq_step_npoints(seq, &count);
		result = npointarr_to_geom_internal(points, count);
		pfree(points);
	}
	return result;
}

Datum
tnpoints_geom(const TemporalS *ts)
{
	/* Singleton sequence set */
	if (ts->count == 1)
		return tnpointseq_geom(temporals_seq_n(ts, 0));

	int count;
	Datum result;
	if (MOBDB_FLAGS_GET_LINEAR(ts->flags))
	{
		nsegment **segments = tnpoints_positions(ts, &count);
		result = nsegmentarr_to_geom_internal(segments, count);
		for (int i = 0; i < count; i++)
			pfree(segments[i]);
		pfree(segments);
	}
	else
	{
		npoint **points = tnpoints_step_npoints(ts, &count);
		result = npointarr_to_geom_internal(points, count);
		pfree(points);
	}
	return result;
}

Datum
tnpoint_geom(const Temporal *temp)
{
	Datum result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = tnpointinst_geom((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tnpointi_geom((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tnpointseq_geom((TemporalSeq *)temp);
	else /* temp->duration == TEMPORALS */
		result = tnpoints_geom((TemporalS *)temp);
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
	else /* temp->duration == TEMPORALS */
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
	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tnpointinst_set_zero((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tnpointi_set_zero((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tnpointseq_cumulative_length((TemporalSeq *)temp, 0);	
	else /* temp->duration == TEMPORALS */
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
	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tnpointinst_set_zero((TemporalInst *)temp);	
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tnpointi_set_zero((TemporalI *)temp);	
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tnpointseq_speed((TemporalSeq *)temp);	
	else /* temp->duration == TEMPORALS */
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
		time = 	inst1->t + (long) ((double) (inst2->t - inst1->t) * fraction);
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
	else /* temp->duration == TEMPORALS */
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

PG_FUNCTION_INFO_V1(tnpoint_at_geometry);

PGDLLEXPORT Datum
tnpoint_at_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tnpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}
	ensure_has_not_Z_gs(gs);

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *geomresult = tpoint_at_geometry_internal(geomtemp, gs);
	Temporal *result = NULL;
	if (geomresult != NULL)
	{
		result = tgeompoint_as_tnpoint_internal(geomresult);
		pfree(geomresult);
	}
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/* Restrict a temporal point to the complement of a geometry */

PG_FUNCTION_INFO_V1(tnpoint_minus_geometry);

PGDLLEXPORT Datum
tnpoint_minus_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tnpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		Temporal* copy = temporal_copy(temp);
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(copy);
	}
	ensure_has_not_Z_gs(gs);

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *geomresult = tpoint_minus_geometry_internal(geomtemp, gs);
	Temporal *result = NULL;
	if (geomresult != NULL)
	{
		result = tgeompoint_as_tnpoint_internal(geomresult);
		pfree(geomresult);
	}
	pfree(geomtemp);
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
	TemporalInst *inst;
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
				t = inst1->t + (long) ((double) (inst2->t - inst1->t) * fraction);
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
			mindist = dist;
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

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
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

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
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

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
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

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)NAI_tnpointi_geometry((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)NAI_tnpointseq_geometry((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
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

	Temporal *geomsync1 = tnpoint_as_tgeompoint_internal(sync1);
	Temporal *geomsync2 = tnpoint_as_tgeompoint_internal(sync2);
	Datum result = shortestline_tpoint_tpoint_internal(geomsync1, geomsync2);
	pfree(geomsync1); pfree(geomsync2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************/
