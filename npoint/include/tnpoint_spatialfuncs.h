/*****************************************************************************
 *
 * tnpoint_spatialfuncs.c
 *	  Geospatial functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_SPATIALFUNCS_H__
#define __TNPOINT_SPATIALFUNCS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "Temporal.h"

/*****************************************************************************/

extern Datum tnpoint_trajectory(PG_FUNCTION_ARGS);

extern Datum tnpointseq_trajectory1(TemporalInst *inst1, TemporalInst *inst2);
extern Datum tnpointseq_trajectory(TemporalSeq *seq);
extern Datum tnpoints_trajectory(TemporalS *ts);

extern Datum tnpointinst_geom(TemporalInst *inst);
extern Datum tnpointi_geom(TemporalI *ti);
extern Datum tnpointseq_geom(TemporalSeq *seq);
extern Datum tnpoints_geom(TemporalS *ts);
extern Datum tnpoint_geom(Temporal *temp);

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

#endif 
