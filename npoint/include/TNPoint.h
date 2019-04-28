#ifndef __TEMPORALNPOINT_H__
#define __TEMPORALNPOINT_H__

#include "TemporalPoint.h"
#include <executor/spi.h>

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
 * Parser.c
 *****************************************************************************/

extern npoint *npoint_parse(char **str);
extern nsegment *nsegment_parse(char **str);
extern Temporal *tnpoint_parse(char **str, Oid basetype);

/*****************************************************************************
 * StaticObjects.c
 *****************************************************************************/

extern Datum npoint_in(PG_FUNCTION_ARGS);
extern Datum npoint_out(PG_FUNCTION_ARGS);
extern Datum npoint_recv(PG_FUNCTION_ARGS);
extern Datum npoint_send(PG_FUNCTION_ARGS);

extern Datum nsegment_in(PG_FUNCTION_ARGS);
extern Datum nsegment_out(PG_FUNCTION_ARGS);
extern Datum nsegment_recv(PG_FUNCTION_ARGS);
extern Datum nsegment_send(PG_FUNCTION_ARGS);

extern Datum npoint_constructor(PG_FUNCTION_ARGS);

extern Datum npoint_route(PG_FUNCTION_ARGS);
extern Datum npoint_position(PG_FUNCTION_ARGS);
extern Datum nsegment_route(PG_FUNCTION_ARGS);
extern Datum nsegment_start_position(PG_FUNCTION_ARGS);
extern Datum nsegment_end_position(PG_FUNCTION_ARGS);

extern Datum npoint_as_geom(PG_FUNCTION_ARGS);
extern Datum nsegment_as_geom(PG_FUNCTION_ARGS);

extern Datum nsegmentarr_to_geom_internal(nsegment **segments, int count);

extern npoint *npoint_make(int64 rid, double pos);
extern nsegment *nsegment_make(int64 rid, double pos1, double pos2);

extern bool route_exists(int64 rid);
extern double route_length(int64 rid);
extern Datum route_geom(int64 rid);

extern Datum npoint_as_geom_internal(npoint *np);
extern Datum nsegment_as_geom_internal(nsegment *ns);

extern ArrayType *int64arr_to_array(int64 *int64arr, int count);
extern ArrayType *npointarr_to_array(npoint **npointarr, int count);
extern ArrayType *nsegmentarr_to_array(nsegment **nsegmentarr, int count);

extern Datum npoint_eq(PG_FUNCTION_ARGS);
extern Datum npoint_ne(PG_FUNCTION_ARGS);
extern Datum npoint_lt(PG_FUNCTION_ARGS);
extern Datum npoint_le(PG_FUNCTION_ARGS);
extern Datum npoint_gt(PG_FUNCTION_ARGS);
extern Datum npoint_ge(PG_FUNCTION_ARGS);

extern bool npoint_eq_internal(npoint *np1, npoint *np2);
extern bool npoint_ne_internal(npoint *np1, npoint *np2);
extern bool npoint_lt_internal(npoint *np1, npoint *np2);
extern bool npoint_le_internal(npoint *np1, npoint *np2);
extern bool npoint_gt_internal(npoint *np1, npoint *np2);
extern bool npoint_ge_internal(npoint *np1, npoint *np2);

extern Datum nsegment_eq(PG_FUNCTION_ARGS);
extern Datum nsegment_ne(PG_FUNCTION_ARGS);
extern Datum nsegment_lt(PG_FUNCTION_ARGS);
extern Datum nsegment_le(PG_FUNCTION_ARGS);
extern Datum nsegment_gt(PG_FUNCTION_ARGS);
extern Datum nsegment_ge(PG_FUNCTION_ARGS);

extern bool nsegment_eq_internal(nsegment *np1, nsegment *np2);
extern bool nsegment_ne_internal(nsegment *np1, nsegment *np2);
extern bool nsegment_lt_internal(nsegment *np1, nsegment *np2);
extern bool nsegment_le_internal(nsegment *np1, nsegment *np2);
extern bool nsegment_gt_internal(nsegment *np1, nsegment *np2);
extern bool nsegment_ge_internal(nsegment *np1, nsegment *np2);

extern Datum point_in_network(PG_FUNCTION_ARGS);

extern int64 rid_from_geom(Datum geom);
extern npoint * geom_as_npoint_internal(Datum value);

/*****************************************************************************
 * TemporalNPoint.c
 *****************************************************************************/

extern Datum tnpointseq_in(PG_FUNCTION_ARGS);
extern Datum tnpoints_in(PG_FUNCTION_ARGS);

extern TemporalInst *tnpointinst_as_tgeompointinst(TemporalInst *inst);
extern TemporalI *tnpointi_as_tgeompointi(TemporalI *ti);
extern TemporalSeq *tnpointseq_as_tgeompointseq(TemporalSeq *seq);
extern TemporalS *tnpoints_as_tgeompoints(TemporalS *ts);

extern Datum tnpoint_positions(PG_FUNCTION_ARGS);
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

/*****************************************************************************
 * TemporalGeo.c
 *****************************************************************************/

extern Datum tnpointseq_trajectory(TemporalSeq *seq);
extern Datum tnpointseq_trajectory1(TemporalInst *inst1, TemporalInst *inst2);
extern Datum tnpoints_trajectory(TemporalS *ts);

extern Datum tnpointinst_geom(TemporalInst *inst);
extern Datum tnpointi_geom(TemporalI *ti);
extern Datum tnpointseq_geom(TemporalSeq *seq);
extern Datum tnpoints_geom(TemporalS *ts);

/*****************************************************************************
 * TempDistance.c
 *****************************************************************************/

extern TemporalSeq *distance_geo_tnpointseq(Datum geo, TemporalSeq *seq);
extern TemporalS *distance_geo_tnpoints(Datum geo, TemporalS *ts);
extern TemporalSeq *distance_tnpointseq_geo(TemporalSeq *seq, Datum geo);
extern TemporalSeq *distance_tnpointseq_tnpointseq(TemporalSeq *seq1, TemporalSeq *seq2);
extern TemporalS *distance_tnpointseq_tnpoints(TemporalSeq *seq, TemporalS *ts);
extern TemporalS *distance_tnpoints_geo(TemporalS *ts, Datum geo);
extern TemporalS *distance_tnpoints_tnpointseq(TemporalS *ts, TemporalSeq *seq);
extern TemporalS *distance_tnpoints_tnpoints(TemporalS *ts1, TemporalS *ts2);
extern Temporal *distance_tnpoint_tnpoint_internal(Temporal *temp1, Temporal *temp2);

/*****************************************************************************
 * BoundBoxOps.c
 *****************************************************************************/

extern Datum npoint_to_gbox(PG_FUNCTION_ARGS);
extern Datum npoint_timestamp_to_gbox(PG_FUNCTION_ARGS);
extern Datum npoint_period_to_gbox(PG_FUNCTION_ARGS);
extern Datum tnpoint_to_gbox(PG_FUNCTION_ARGS);

extern bool npoint_to_gbox_internal(GBOX *box, npoint *np);

extern void tnpointinst_make_gbox(GBOX *box, Datum value, TimestampTz t);
extern void tnpointinstarr_disc_to_gbox(GBOX *box, TemporalInst **inst, int count);
extern void tnpointinstarr_cont_to_gbox(GBOX *box, TemporalInst **inst, int count);
extern void tnpointseqarr_to_gbox(GBOX *box, TemporalSeq **seq, int count);

/*****************************************************************************
 * TempSpatialRels.c
 *****************************************************************************/

extern bool tnpointseq_intersect_at_timestamp(TemporalInst *start1, 
	TemporalInst *end1,	TemporalInst *start2, TemporalInst *end2, 
	bool lower_inc, bool upper_inc, TimestampTz *inter);

extern TemporalInst *tspatialrel_tnpointinst_geo(TemporalInst *inst, Datum geo,
	Datum (*operator)(Datum, Datum), Oid valuetypid, bool invert);
extern TemporalI *tspatialrel_tnpointi_geo(TemporalI *ti, Datum geo,
	Datum (*operator)(Datum, Datum), Oid valuetypid, bool invert);

extern TemporalInst *tspatialrel_tnpointinst_tnpointinst(
	TemporalInst *inst1, TemporalInst *inst2,
	Datum (*operator)(Datum, Datum), Oid valuetypid);
extern TemporalI *tspatialrel_tnpointi_tnpointi(TemporalI *ti1, TemporalI *ti2,
	Datum (*operator)(Datum, Datum), Oid valuetypid);


/*****************************************************************************/

#endif /* __TEMPORALPOINT_H__ */
