/*****************************************************************************
 *
 * TNPoint.h
 *	  Functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_H__
#define __TNPOINT_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include "Temporal.h"

/*****************************************************************************
 * Struct definitions
 *****************************************************************************/

/* Network-based point */

typedef struct
{
	int64		rid;			 /* route identifier */
	double		pos;			 /* position */
} npoint;

/* Network-based segment */

typedef struct
{
	int64		rid;			/* route identifier */
	double		pos1;			/* position1 */
	double		pos2;			/* position2 */
} nsegment;

/*****************************************************************************
 * fmgr macros
 *****************************************************************************/

/* npoint */
#define DatumGetNpoint(X)		((npoint *) DatumGetPointer(X))
#define NpointGetDatum(X)		PointerGetDatum(X)
#define PG_GETARG_NPOINT(i)		((npoint *) PG_GETARG_POINTER(i))

/* nsegment */
#define DatumGetNsegment(X)		((nsegment *) DatumGetPointer(X))
#define NsegmentGetDatum(X)		PointerGetDatum(X)
#define PG_GETARG_NSEGMENT(i)	((nsegment *) PG_GETARG_POINTER(i))

/*****************************************************************************
 * TNPoint.c
 *****************************************************************************/

extern Datum tnpoint_in(PG_FUNCTION_ARGS);

extern Datum tnpoint_make_tnpointseq(PG_FUNCTION_ARGS);

extern Datum tnpoint_as_tgeompoint(PG_FUNCTION_ARGS);
extern Datum tgeompoint_as_tnpoint(PG_FUNCTION_ARGS);

extern TemporalInst *tnpointinst_as_tgeompointinst(TemporalInst *inst);
extern TemporalI *tnpointi_as_tgeompointi(TemporalI *ti);
extern TemporalSeq *tnpointseq_as_tgeompointseq(TemporalSeq *seq);
extern TemporalS *tnpoints_as_tgeompoints(TemporalS *ts);
extern Temporal *tnpoint_as_tgeompoint_internal(Temporal *temp);

extern TemporalInst *tgeompointinst_as_tnpointinst(TemporalInst *inst);
extern TemporalI *tgeompointi_as_tnpointi(TemporalI *ti);
extern TemporalSeq *tgeompointseq_as_tnpointseq(TemporalSeq *seq);
extern TemporalS *tgeompoints_as_tnpoints(TemporalS *ts);

extern Datum tnpoint_positions(PG_FUNCTION_ARGS);
extern Datum tnpointinst_route(PG_FUNCTION_ARGS);
extern Datum tnpoint_routes(PG_FUNCTION_ARGS);

extern ArrayType *tnpointinst_positions(TemporalInst *inst);
extern ArrayType *tnpointi_positions(TemporalI *ti);
extern nsegment *tnpointseq_positions1(TemporalSeq *seq);
extern ArrayType *tnpointseq_positions(TemporalSeq *seq);
extern nsegment **tnpoints_positions1(TemporalS *ts);
extern ArrayType *tnpoints_positions(TemporalS *ts);

extern ArrayType *tnpointinst_routes(TemporalInst *inst);
extern ArrayType *tnpointi_routes(TemporalI *ti);
extern ArrayType *tnpointseq_routes(TemporalSeq *seq);
extern ArrayType *tnpoints_routes(TemporalS *ts);

/*****************************************************************************/

#endif 
