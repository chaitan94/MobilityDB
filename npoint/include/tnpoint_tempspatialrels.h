/*****************************************************************************
 *
 * tnpoint_tempspatialrels.c
 *	  Temporal spatial relationships for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_TEMPSPATIALRELS_H__
#define __TNPOINT_TEMPSPATIALRELS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum tcontains_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcontains_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcontains_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tcontains_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tcovers_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcovers_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcovers_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tcovers_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tcoveredby_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tdisjoint_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tdisjoint_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tdisjoint_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tdisjoint_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tdisjoint_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tequals_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tequals_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tequals_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tequals_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tequals_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tintersects_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tintersects_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tintersects_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tintersects_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum ttouches_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum ttouches_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum ttouches_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum ttouches_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum twithin_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum twithin_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum twithin_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum twithin_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tdwithin_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tdwithin_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tdwithin_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tdwithin_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tdwithin_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum trelate_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum trelate_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum trelate_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum trelate_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum trelate_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_tnpoint_tnpoint(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif /* __TNPOINT_TEMPSPATIALRELS_H__ */
