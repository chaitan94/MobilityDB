/*****************************************************************************
 *
 * tnpoint_analyze.c
 *	Functions for gathering statistics from temporal network point columns
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_ANALYZE_H__
#define __TNPOINT_ANALYZE_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum tnpoint_analyze(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif /* __TNPOINT_ANALYZE_H__ */
