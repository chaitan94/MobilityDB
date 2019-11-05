/*****************************************************************************
 *
 * tgeo_parser.h
 *    Functions for parsing temporal geometries and geographies.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi, 
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TGEO_PARSER_H__
#define __TGEO_PARSER_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include "temporal.h"
#include "tgeo.h"

/*****************************************************************************/

extern Temporal *tgeo_parse(char **str, Oid basetype);

/*****************************************************************************/

#endif
