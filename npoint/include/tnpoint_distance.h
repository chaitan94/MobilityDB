/*****************************************************************************
 *
 * tnpoint_distance.c
 *	  Temporal distance for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_DISTANCE_H__
#define __TNPOINT_DISTANCE_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include "temporal.h"

/*****************************************************************************/

extern Datum distance_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum distance_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum distance_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum distance_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum distance_tnpoint_tnpoint(PG_FUNCTION_ARGS);

extern TemporalSeq *distance_tnpointseq_tnpointseq(const TemporalSeq *seq1, const TemporalSeq *seq2);
extern TemporalS *distance_tnpoints_tnpoints(const TemporalS *ts1, const TemporalS *ts2);
extern Temporal *distance_tnpoint_tnpoint_internal(const Temporal *temp1, const Temporal *temp2);

/*****************************************************************************/

#endif /* __TNPOINT_DISTANCE_H__ */
