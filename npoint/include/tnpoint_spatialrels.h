/*****************************************************************************
 *
 * tnpoint_spatialrels.h
 *	  Spatial relationships for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_SPATIALRELS_H__
#define __TNPOINT_SPATIALRELS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum contains_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum contains_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum contains_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum contains_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum contains_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum containsproperly_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum covers_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum covers_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum covers_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum covers_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum covers_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum coveredby_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum coveredby_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum coveredby_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum coveredby_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum coveredby_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum crosses_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum crosses_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum crosses_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum crosses_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum crosses_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum disjoint_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum disjoint_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum disjoint_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum disjoint_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum disjoint_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum equals_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum equals_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum equals_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum equals_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum equals_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum intersects_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum intersects_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum intersects_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum intersects_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum intersects_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum overlaps_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overlaps_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum touches_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum touches_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum touches_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum touches_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum touches_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum within_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum within_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum within_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum within_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum within_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum dwithin_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum dwithin_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum dwithin_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum dwithin_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum dwithin_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum relate_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum relate_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum relate_pattern_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_tnpoint_tnpoint(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif 
