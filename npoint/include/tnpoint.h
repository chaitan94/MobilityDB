/*****************************************************************************
 *
 * tnpoint.h
 *	  Functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_H__
#define __TNPOINT_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include "temporal.h"

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

extern TemporalInst *tnpointinst_as_tgeompointinst(const TemporalInst *inst);
extern TemporalI *tnpointi_as_tgeompointi(const TemporalI *ti);
extern TemporalSeq *tnpointseq_as_tgeompointseq(const TemporalSeq *seq);
extern TemporalS *tnpoints_as_tgeompoints(const TemporalS *ts);
extern Temporal *tnpoint_as_tgeompoint_internal(const Temporal *temp);

extern TemporalInst *tgeompointinst_as_tnpointinst(const TemporalInst *inst);
extern TemporalI *tgeompointi_as_tnpointi(const TemporalI *ti);
extern TemporalSeq *tgeompointseq_as_tnpointseq(const TemporalSeq *seq);
extern TemporalS *tgeompoints_as_tnpoints(const TemporalS *ts);

extern Datum tnpoint_positions(PG_FUNCTION_ARGS);
extern Datum tnpoint_route(PG_FUNCTION_ARGS);
extern Datum tnpoint_routes(PG_FUNCTION_ARGS);

extern nsegment **tnpointinst_positions(const TemporalInst *inst);
extern nsegment **tnpointi_positions(const TemporalI *ti, int *count);
extern nsegment *tnpointseq_linear_positions(const TemporalSeq *seq);
extern nsegment **tnpointseq_positions(const TemporalSeq *seq, int *count);
extern nsegment **tnpoints_positions(const TemporalS *ts, int *count);
extern nsegment **tnpoint_positions_internal(const Temporal *temp, int *count);

extern ArrayType *tnpointinst_routes(const TemporalInst *inst);
extern ArrayType *tnpointi_routes(const TemporalI *ti);
extern ArrayType *tnpointseq_routes(const TemporalSeq *seq);
extern ArrayType *tnpoints_routes(const TemporalS *ts);

/*****************************************************************************/

#endif /* __TNPOINT_H__ */
