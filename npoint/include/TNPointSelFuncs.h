/*****************************************************************************
 *
 * TNPointSelFuncs.c
 *      Functions for selectivity estimation of operators on temporal network 
 *      points
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINTSELFUNCS_H__
#define __TNPOINTSELFUNCS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum tnpoint_overlaps_sel(PG_FUNCTION_ARGS);
extern Datum tnpoint_overlaps_joinsel(PG_FUNCTION_ARGS);
extern Datum tnpoint_contains_sel(PG_FUNCTION_ARGS);
extern Datum tnpoint_contains_joinsel(PG_FUNCTION_ARGS);
extern Datum tnpoint_same_sel(PG_FUNCTION_ARGS);
extern Datum tnpoint_same_joinsel(PG_FUNCTION_ARGS);
extern Datum tnpoint_position_sel(PG_FUNCTION_ARGS);
extern Datum tnpoint_position_joinsel(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif 
