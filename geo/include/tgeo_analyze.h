/*****************************************************************************
 *
 * tgeo_analyze.h
 *    Functions for gathering statistics from temporal geometry columns
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi, Arthur Lesuisse, 
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TGEO_ANALYZE_H__
#define __TGEO_ANALYZE_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum tgeo_analyze(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif
