/*****************************************************************************
 *
 * TNPointAggFuncs.c
 *	  Aggregate functions for temporal network points.
 *
 * The only function currently provided is temporal centroid.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 *	  Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "TNPoint.h"

PG_FUNCTION_INFO_V1(tnpoint_tcentroid_transfn);

PGDLLEXPORT Datum
tnpoint_tcentroid_transfn(PG_FUNCTION_ARGS)
{
	AggregateState *state = PG_ARGISNULL(0) ?
		aggstate_make(fcinfo, 0, NULL) : (AggregateState *) PG_GETARG_POINTER(0);
	if (PG_ARGISNULL(1))
		PG_RETURN_POINTER(state);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Temporal *temp1 = tnpoint_as_tgeompoint_internal(temp);
	AggregateState *result;
	if (temp->type == TEMPORALINST)
		result = tpointinst_tcentroid_transfn(fcinfo, state, (TemporalInst *)temp1,
			&datum_sum_double3);
	else if (temp->type == TEMPORALI)
		result = tpointi_tcentroid_transfn(fcinfo, state, (TemporalI *)temp1,
			&datum_sum_double3);
	else if (temp->type == TEMPORALSEQ)
		result = tpointseq_tcentroid_transfn(fcinfo, state, (TemporalSeq *)temp1,
			&datum_sum_double3);
	else if (temp->type == TEMPORALS)
		result = tpoints_tcentroid_transfn(fcinfo, state, (TemporalS *)temp1,
			&datum_sum_double3);
	else
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("Operation not supported")));
			
	pfree(temp1);
    PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/