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

#ifndef __TEMPORALNPOINT_H__
#define __TEMPORALNPOINT_H__

#include "TemporalPoint.h"
#include <executor/spi.h>

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
 * TNPointParser.c
 *****************************************************************************/

extern npoint *npoint_parse(char **str);
extern nsegment *nsegment_parse(char **str);
extern Temporal *tnpoint_parse(char **str, Oid basetype);

/*****************************************************************************
 * StaticObjects.c
 *****************************************************************************/

extern ArrayType *int64arr_to_array(int64 *int64arr, int count);
extern ArrayType *npointarr_to_array(npoint **npointarr, int count);
extern ArrayType *nsegmentarr_to_array(nsegment **nsegmentarr, int count);

extern Datum npoint_in(PG_FUNCTION_ARGS);
extern Datum npoint_out(PG_FUNCTION_ARGS);
extern Datum npoint_recv(PG_FUNCTION_ARGS);
extern Datum npoint_send(PG_FUNCTION_ARGS);

extern Datum nsegment_in(PG_FUNCTION_ARGS);
extern Datum nsegment_out(PG_FUNCTION_ARGS);
extern Datum nsegment_recv(PG_FUNCTION_ARGS);
extern Datum nsegment_send(PG_FUNCTION_ARGS);

extern Datum npoint_constructor(PG_FUNCTION_ARGS);
extern Datum nsegment_constructor(PG_FUNCTION_ARGS);
extern Datum nsegment_from_npoint(PG_FUNCTION_ARGS);

extern npoint *npoint_make(int64 rid, double pos);
extern nsegment *nsegment_make(int64 rid, double pos1, double pos2);

extern Datum npoint_route(PG_FUNCTION_ARGS);
extern Datum npoint_position(PG_FUNCTION_ARGS);
extern Datum nsegment_route(PG_FUNCTION_ARGS);
extern Datum nsegment_start_position(PG_FUNCTION_ARGS);
extern Datum nsegment_end_position(PG_FUNCTION_ARGS);

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

extern bool route_exists(int64 rid);
extern double route_length(int64 rid);
extern Datum route_geom(int64 rid);
extern int64 rid_from_geom(Datum geom);

extern Datum npoint_as_geom(PG_FUNCTION_ARGS);
extern Datum geom_as_npoint(PG_FUNCTION_ARGS);
extern Datum nsegment_as_geom(PG_FUNCTION_ARGS);
extern Datum geom_as_nsegment(PG_FUNCTION_ARGS);

extern Datum npoint_as_geom_internal(npoint *np);
extern Datum nsegment_as_geom_internal(nsegment *ns);
extern npoint *geom_as_npoint_internal(Datum geom);
extern nsegment *geom_as_nsegment_internal(Datum line);

extern Datum nsegmentarr_to_geom_internal(nsegment **segments, int count);

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

/*****************************************************************************
 * TNPointGeo.c
 *****************************************************************************/

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

/*****************************************************************************
 * TNPointDistance.c
 *****************************************************************************/

extern Datum distance_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum distance_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum distance_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum distance_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum distance_tnpoint_tnpoint(PG_FUNCTION_ARGS);

extern TemporalSeq *distance_tnpointseq_tnpointseq(TemporalSeq *seq1, TemporalSeq *seq2);
extern TemporalS *distance_tnpoints_tnpoints(TemporalS *ts1, TemporalS *ts2);
extern Temporal *distance_tnpoint_tnpoint_internal(Temporal *temp1, Temporal *temp2);

/*****************************************************************************
 * TNPointBoundBoxOps.c
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

extern Datum npoint_expand_spatial(PG_FUNCTION_ARGS);

extern Datum overlaps_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum contains_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum contains_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum contained_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum contained_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum same_bbox_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum same_bbox_tnpoint_npoint(PG_FUNCTION_ARGS);

/*****************************************************************************
 * TNPointSpatialRels.c
 *****************************************************************************/

extern Datum contains_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum contains_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum contains_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum contains_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum contains_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum containsproperly_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum containsproperly_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum covers_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum covers_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum covers_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum covers_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum covers_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum coveredby_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum coveredby_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum coveredby_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum coveredby_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum coveredby_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum crosses_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum crosses_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum crosses_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum crosses_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum crosses_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum disjoint_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum disjoint_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum disjoint_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum disjoint_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum disjoint_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum equals_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum equals_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum equals_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum equals_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum equals_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum intersects_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum intersects_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum intersects_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum intersects_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum intersects_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overlaps_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum overlaps_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overlaps_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum touches_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum touches_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum touches_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum touches_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum touches_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum within_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum within_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum within_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum within_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum within_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum dwithin_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum dwithin_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum dwithin_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum dwithin_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum dwithin_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum relate_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum relate_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum relate_pattern_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum relate_pattern_tnpoint_tnpoint(PG_FUNCTION_ARGS);

/*****************************************************************************
 * TNPointTempSpatialRels.c
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

extern Datum tcontains_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcontains_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcontains_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tcontains_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tcontains_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcovers_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcovers_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcovers_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tcovers_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tcovers_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum tcoveredby_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum tcoveredby_tnpoint_tnpoint(PG_FUNCTION_ARGS);
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
extern Datum ttouches_tnpoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum twithin_geo_tnpoint(PG_FUNCTION_ARGS);
extern Datum twithin_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum twithin_tnpoint_geo(PG_FUNCTION_ARGS);
extern Datum twithin_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum twithin_tnpoint_tnpoint(PG_FUNCTION_ARGS);
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

/*****************************************************************************
 * TNPointRelPosOps.c
 *****************************************************************************/

extern Datum left_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overleft_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum right_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overright_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum below_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overbelow_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum above_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overabove_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum left_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overleft_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum right_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overright_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum below_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overbelow_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum above_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overabove_tnpoint_npoint(PG_FUNCTION_ARGS);

/*****************************************************************************
 * TNPointAggFuncs.c
 *****************************************************************************/

extern Datum tnpoint_tcentroid_transfn(PG_FUNCTION_ARGS);

/*****************************************************************************
 * TNPointIndex.c
 *****************************************************************************/

extern Datum gist_tnpoint_compress(PG_FUNCTION_ARGS);
extern Datum spgist_tnpoint_compress(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif /* __TEMPORALPOINT_H__ */
