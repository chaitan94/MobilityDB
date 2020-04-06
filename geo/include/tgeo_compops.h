/*****************************************************************************
 *
 * tgeo_compops.h
 *    Comparison functions and operators for temporal geometries.
 *
 * Portions Copyright (c) 2020, Maxime Schoemans, Esteban Zimanyi, 
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TGEO_COMPOPS_H__
#define __TGEO_COMPOPS_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include <liblwgeom.h>

#include "temporal.h"

/*****************************************************************************/

extern Datum teq_geo_tgeo(PG_FUNCTION_ARGS);
extern Datum teq_tgeo_geo(PG_FUNCTION_ARGS);
extern Datum teq_tgeo_tgeo(PG_FUNCTION_ARGS);

extern Datum tne_geo_tgeo(PG_FUNCTION_ARGS);
extern Datum tne_tgeo_geo(PG_FUNCTION_ARGS);
extern Datum tne_tgeo_tgeo(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif 
