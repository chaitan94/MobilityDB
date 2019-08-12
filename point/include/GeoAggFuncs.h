/*****************************************************************************
 *
 * GeoAggFuncs.h
 *	Aggregate functions for temporal points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __GEOAGGFUNCS_H__
#define __GEOAGGFUNCS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "AggregateFuncs.h"

/*****************************************************************************/

extern Datum tpoint_tcentroid_transfn(PG_FUNCTION_ARGS);
extern Datum tpoint_tcentroid_combinefn(PG_FUNCTION_ARGS);
extern Datum tpoint_tcentroid_finalfn(PG_FUNCTION_ARGS);

extern void geoaggstate_check_t(AggregateState *state, Temporal *t);
extern AggregateState *aggstate_make_tcentroid(FunctionCallInfo fcinfo, Temporal *temp);

/*****************************************************************************/

#endif
