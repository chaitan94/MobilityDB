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
 * Struct definitions
 *****************************************************************************/

/* Affine transformation (only rotation and translation) */
/* Should I use (double, double, double), (double, double2) or double3 ? */

typedef struct
{
    double      theta;         /* rotation in radians (limit to -pi,pi ?) */
    double      tx;            /* translation in x */
    double      ty;            /* translation in y */
/*  int         srid;             srid (is this needed ? ) */
} rtransform;

/*****************************************************************************
 * fmgr macros
 *****************************************************************************/

/* npoint */
#define DatumGetRtransform(X)       ((rtransform *) DatumGetPointer(X))
#define RtransformGetDatum(X)       PointerGetDatum(X)
#define PG_GETARG_RTRANSFORM(i)     ((rtransform *) PG_GETARG_POINTER(i))

/*****************************************************************************
 * tgeo.c
 *****************************************************************************/

/* Input/output functions */

extern Datum tgeo_in(PG_FUNCTION_ARGS);

/* Constructor functions */

extern Datum tgeoinst_constructor(PG_FUNCTION_ARGS);

/* Accessor functions */

extern Datum tgeo_stbox(PG_FUNCTION_ARGS);

extern Datum tgeo_ever_eq(PG_FUNCTION_ARGS);
extern Datum tgeo_ever_ne(PG_FUNCTION_ARGS); // TODO

extern Datum tgeo_always_eq(PG_FUNCTION_ARGS); // TODO
extern Datum tgeo_always_ne(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif
