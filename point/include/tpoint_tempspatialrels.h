/*****************************************************************************
 *
 * tpoint_tempspatialrels.h
 *	  Temporal spatial relationships for temporal points.
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TPOINT_TEMPSPATIALRELS_H__
#define __TPOINT_TEMPSPATIALRELS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "temporal.h"

/*****************************************************************************/

extern Datum tcontains_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tcontains_tpoint_geo(PG_FUNCTION_ARGS);

extern Datum tcovers_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tcovers_tpoint_geo(PG_FUNCTION_ARGS);

extern Datum tcoveredby_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_tpoint_geo(PG_FUNCTION_ARGS);

extern Datum tdisjoint_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tdisjoint_tpoint_geo(PG_FUNCTION_ARGS);
extern Datum tdisjoint_tpoint_tpoint(PG_FUNCTION_ARGS);

extern Datum tequals_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tequals_tpoint_geo(PG_FUNCTION_ARGS);
extern Datum tequals_tpoint_tpoint(PG_FUNCTION_ARGS);

extern Datum tintersects_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tintersects_tpoint_geo(PG_FUNCTION_ARGS);
extern Datum tintersects_tpoint_tpoint(PG_FUNCTION_ARGS);

extern Datum ttouches_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum ttouches_tpoint_geo(PG_FUNCTION_ARGS);

extern Datum twithin_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum twithin_tpoint_geo(PG_FUNCTION_ARGS);

extern Datum tdwithin_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum tdwithin_tpoint_geo(PG_FUNCTION_ARGS);
extern Datum tdwithin_tpoint_tpoint(PG_FUNCTION_ARGS);

extern Datum trelate_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum trelate_tpoint_geo(PG_FUNCTION_ARGS);
extern Datum trelate_tpoint_tpoint(PG_FUNCTION_ARGS);

extern Datum trelate_pattern_geo_tpoint(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_tpoint_geo(PG_FUNCTION_ARGS);
extern Datum trelate_pattern_tpoint_tpoint(PG_FUNCTION_ARGS);

extern Temporal *tspatialrel_tpoint_geo(Temporal *temp, Datum geo,
	Datum (*func)(Datum, Datum), Oid valuetypid, bool invert);
extern Temporal *tspatialrel3_tpoint_geo(Temporal *temp, Datum geo, Datum param,
	Datum (*func)(Datum, Datum, Datum), bool invert);

extern TemporalS *tdwithin_tpointseq_geo(TemporalSeq *seq, Datum geo, Datum dist);
extern TemporalS *tdwithin_tpoints_geo(TemporalS *ts, Datum geo, Datum dist);
extern Temporal *tdwithin_tpoint_geo_internal(Temporal *temp, GSERIALIZED *gs, Datum dist);

extern TemporalS *tdwithin_tpointseq_tpointseq(TemporalSeq *seq1, TemporalSeq *seq2,
	Datum param, Datum (*func)(Datum, Datum, Datum));
extern TemporalS *tdwithin_tpoints_tpoints(TemporalS *ts1, TemporalS *ts2, Datum d,
	Datum (*func)(Datum, Datum, Datum));
/*****************************************************************************/

#endif
