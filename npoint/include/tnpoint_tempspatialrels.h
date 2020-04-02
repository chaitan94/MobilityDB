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
#include "temporal.h"

/*****************************************************************************/

extern bool tnpointseq_intersection(const TemporalInst *start1,
	const TemporalInst *end1, const TemporalInst *start2,
	const TemporalInst *end2, Datum *inter1, Datum *inter2, TimestampTz *t);
extern TemporalInst *tspatialrel_tnpointinst_geo(const TemporalInst *inst, Datum geo,
	Datum (*operator)(Datum, Datum), Oid valuetypid, bool invert);
extern TemporalI *tspatialrel_tnpointi_geo(const TemporalI *ti, Datum geo,
	Datum (*operator)(Datum, Datum), Oid valuetypid, bool invert);
extern TemporalInst *tspatialrel_tnpointinst_tnpointinst(
	const TemporalInst *inst1, const TemporalInst *inst2,
	Datum (*operator)(Datum, Datum), Oid valuetypid);
extern TemporalI *tspatialrel_tnpointi_tnpointi(const TemporalI *ti1, const TemporalI *ti2,
	Datum (*operator)(Datum, Datum), Oid valuetypid);

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
extern Datum tintersects_tnpoint_tnpoint(PG_FUNCTION_ARGS);
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
