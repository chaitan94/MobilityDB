/*****************************************************************************
 *
 * tnpoint_boxops.h
 *	  Bounding box operators for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_BOXOPS_H__
#define __TNPOINT_BOXOPS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "Temporal.h"
#include "tnpoint.h"

/*****************************************************************************/

extern Datum npoint_to_stbox(PG_FUNCTION_ARGS);
extern Datum npoint_timestamp_to_stbox(PG_FUNCTION_ARGS);
extern Datum npoint_period_to_stbox(PG_FUNCTION_ARGS);
extern Datum tnpoint_to_stbox(PG_FUNCTION_ARGS);

extern bool npoint_to_stbox_internal(STBOX *box, npoint *np);
extern void tnpointinst_make_stbox(STBOX *box, Datum value, TimestampTz t);
extern void tnpointinstarr_disc_to_stbox(STBOX *box, TemporalInst **inst, int count);
extern void tnpointinstarr_cont_to_stbox(STBOX *box, TemporalInst **inst, int count);
extern void tnpointseqarr_to_stbox(STBOX *box, TemporalSeq **seq, int count);

extern Datum npoint_expand_spatial(PG_FUNCTION_ARGS);

extern Datum overlaps_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum contains_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum contains_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum contained_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum contained_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum same_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum same_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif 
