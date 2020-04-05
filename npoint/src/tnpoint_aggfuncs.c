/*****************************************************************************
 *
 * tnpoint_aggfuncs.c
 *	  Aggregate functions for temporal network points.
 *
 * The only function currently provided is temporal centroid.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 *	  Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_aggfuncs.h"

#include <assert.h>

#include "tpoint_spatialfuncs.h"
#include "temporal_aggfuncs.h"
#include "tpoint_aggfuncs.h"
#include "tnpoint.h"

/*****************************************************************************/

PG_FUNCTION_INFO_V1(tnpoint_tcentroid_transfn);

PGDLLEXPORT Datum
tnpoint_tcentroid_transfn(PG_FUNCTION_ARGS)
{
	SkipList *state = PG_ARGISNULL(0) ? NULL : 
		(SkipList *) PG_GETARG_POINTER(0);
	if (PG_ARGISNULL(1))
	{
		if (state)
			PG_RETURN_POINTER(state);
		else
			PG_RETURN_NULL();
	}
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Temporal *temp1 = tnpoint_as_tgeompoint_internal(temp);

	geoaggstate_check_t(state, temp1);
	Datum (*func)(Datum, Datum) = MOBDB_FLAGS_GET_Z(temp1->flags) ?
		&datum_sum_double4 : &datum_sum_double3;

	int count;
	Temporal **temporals = tpoint_transform_tcentroid(temp1, &count);
	if (state)
	{
		if (skiplist_headval(state)->duration != temporals[0]->duration)
			ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
				errmsg("Cannot aggregate temporal values of different duration")));
		skiplist_splice(fcinfo, state, temporals, count, func, false);
	}
	else
	{
		state = skiplist_make(fcinfo, temporals, count);
		struct GeoAggregateState extra =
		{
			.srid = tpoint_srid_internal(temp1),
			.hasz = MOBDB_FLAGS_GET_Z(temp1->flags) != 0
		};
		aggstate_set_extra(fcinfo, state, &extra, sizeof(struct GeoAggregateState));
	}

	for (int i = 0; i< count; i++)
		pfree(temporals[i]);
	pfree(temporals);		
	pfree(temp1);		
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(state);
}

/*****************************************************************************/