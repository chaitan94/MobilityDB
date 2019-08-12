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

#include "TNPointAggFuncs.h"

#include <assert.h>

#include "AggregateFuncs.h"
#include "GeoAggFuncs.h"
#include "TNPoint.h"

/*****************************************************************************/

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
	geoaggstate_check_t(state, temp);

    AggregateState *state2 = aggstate_make_tcentroid(fcinfo, temp);
	AggregateState *result;
	temporal_duration_is_valid(temp->duration);
    if(temp->duration == TEMPORALINST || temp->duration == TEMPORALI)
        result = temporalinst_tagg_combinefn(fcinfo, state, state2, &datum_sum_double3);
    if(temp->duration == TEMPORALSEQ || temp->duration == TEMPORALS)
        result = temporalseq_tagg_combinefn(fcinfo, state, state2, &datum_sum_double3, false);

    assert(result != NULL) ;

    if (result != state)
        pfree(state);
    if (result != state2)
        pfree(state2);

    aggstate_move_extra(result, state2) ;

	pfree(temp1);
    PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/