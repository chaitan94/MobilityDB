/*****************************************************************************
 *
 * tnpoint_spatialfuncs.c
 *	  Geospatial functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_SPATIALFUNCS_H__
#define __TNPOINT_SPATIALFUNCS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "temporal.h"
#include "tnpoint_static.h"

/*****************************************************************************/

/* Parameter tests */

extern void ensure_same_srid_tnpoint(const Temporal *temp1, const Temporal *temp2);
extern void ensure_same_srid_tnpoint_stbox(const Temporal *temp, const STBOX *box);
extern void ensure_same_srid_tnpoint_gs(const Temporal *temp, const GSERIALIZED *gs);
extern void ensure_same_srid_tnpoint_npoint(const Temporal *temp, const npoint *np);
extern void ensure_same_rid_tnpointinst(const TemporalInst *inst1, const TemporalInst *inst2);

/* Functions for spatial reference systems */

extern int tnpoint_srid_internal(const Temporal *temp);

extern Datum tnpoint_trajectory(PG_FUNCTION_ARGS);

extern Datum tnpointseq_trajectory1(const TemporalInst *inst1, const TemporalInst *inst2);
extern Datum tnpointseq_trajectory(const TemporalSeq *seq);
extern Datum tnpoints_trajectory(const TemporalS *ts);

extern Datum tnpointinst_geom(const TemporalInst *inst);
extern Datum tnpointi_geom(const TemporalI *ti);
extern Datum tnpointseq_geom(const TemporalSeq *seq);
extern Datum tnpoints_geom(const TemporalS *ts);
extern Datum tnpoint_geom(const Temporal *temp);

extern Datum tnpoint_length(PG_FUNCTION_ARGS);
extern Datum tnpoint_cumulative_length(PG_FUNCTION_ARGS);
extern Datum tnpoint_speed(PG_FUNCTION_ARGS);
extern Datum tnpoint_twcentroid(PG_FUNCTION_ARGS);
extern Datum tnpoint_azimuth(PG_FUNCTION_ARGS);
extern Datum tnpoint_at_geometry(PG_FUNCTION_ARGS);
extern Datum tnpoint_minus_geometry(PG_FUNCTION_ARGS);

extern Datum NAI_geometry_tnpoint(PG_FUNCTION_ARGS);
extern Datum NAI_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum NAI_tnpoint_geometry(PG_FUNCTION_ARGS);
extern Datum NAI_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum NAI_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum NAD_geometry_tnpoint(PG_FUNCTION_ARGS);
extern Datum NAD_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum NAD_tnpoint_geometry(PG_FUNCTION_ARGS);
extern Datum NAD_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum NAD_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum shortestline_geometry_tnpoint(PG_FUNCTION_ARGS);
extern Datum shortestline_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum shortestline_tnpoint_geometry(PG_FUNCTION_ARGS);
extern Datum shortestline_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum shortestline_tnpoint_tnpoint(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif /* __TNPOINT_SPATIALFUNCS_H__ */
