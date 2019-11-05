/*****************************************************************************
 *
 * tgeo.h
 *	  Functions for temporal geometries and geographies.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TGEO_H__
#define __TGEO_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include <liblwgeom.h>

#include "temporal.h"

/*****************************************************************************
 * tgeo.c
 *****************************************************************************/

extern Datum tgeo_in(PG_FUNCTION_ARGS);

extern Datum tgeo_make_temporalinst(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif
