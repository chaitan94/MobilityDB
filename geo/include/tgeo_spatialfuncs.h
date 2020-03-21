/*****************************************************************************
 *
 * tgeo_spatialfuncs.c
 *    Geospatial functions for temporal geometries and geographies.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi, 
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TGEO_SPATIALFUNCS_H__
#define __TGEO_SPATIALFUNCS_H__

#include <postgres.h>
#include <liblwgeom.h>
#include <catalog/pg_type.h>
#include "temporal.h"

/*****************************************************************************/

extern void ensure_same_srid_tgeo_gs(const Temporal *temp, const GSERIALIZED *gs);
extern void ensure_same_dimensionality_tgeo_gs(const Temporal *temp, const GSERIALIZED *gs);
extern void ensure_polygon_type(const GSERIALIZED *gs);

extern void ensure_same_rings_tgeometryinst(const TemporalInst *ti1, const TemporalInst *ti2);
extern void ensure_same_rings_lwpoly(const LWPOLY *poly1, const LWPOLY *poly2);

extern bool datum_geometry_eq(Datum geometry1, Datum geometry2);
extern bool lwgeom_similar(const LWGEOM *geometry1, const LWGEOM *geometry2);

/*****************************************************************************/

#endif 
