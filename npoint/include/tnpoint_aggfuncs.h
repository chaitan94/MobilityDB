/*****************************************************************************
 *
 * tnpoint_aggfuncs.h
 *	  Aggregate functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_AGGFUNCS_H__
#define __TNPOINT_AGGFUNCS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum tnpoint_tcentroid_transfn(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif 
