/*****************************************************************************
 *
 * tnpoint_static.h
 *	  Network-based static point/segments
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_STATIC_H__
#define __TNPOINT_STATIC_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "tnpoint.h"

/*****************************************************************************
 * StaticObjects.c
 *****************************************************************************/

extern ArrayType *int64arr_to_array(const int64 *int64arr, int count);
extern ArrayType *npointarr_to_array(npoint **npointarr, int count);
extern ArrayType *nsegmentarr_to_array(nsegment **nsegmentarr, int count);
extern nsegment **nsegmentarr_normalize(nsegment **segments, int *count);

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

extern void npoint_set(npoint *np, int64 rid, double pos);
extern npoint *npoint_make(int64 rid, double pos);
extern void nsegment_set( nsegment *ns, int64 rid, double pos1, double pos2);
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

extern bool npoint_same_internal(const npoint *np1, const npoint *np2);
extern bool npoint_eq_internal(const npoint *np1, const npoint *np2);
extern bool npoint_ne_internal(const npoint *np1, const npoint *np2);
extern bool npoint_lt_internal(const npoint *np1, const npoint *np2);
extern bool npoint_le_internal(const npoint *np1, const npoint *np2);
extern bool npoint_gt_internal(const npoint *np1, const npoint *np2);
extern bool npoint_ge_internal(const npoint *np1, const npoint *np2);

extern Datum nsegment_eq(PG_FUNCTION_ARGS);
extern Datum nsegment_ne(PG_FUNCTION_ARGS);
extern Datum nsegment_lt(PG_FUNCTION_ARGS);
extern Datum nsegment_le(PG_FUNCTION_ARGS);
extern Datum nsegment_gt(PG_FUNCTION_ARGS);
extern Datum nsegment_ge(PG_FUNCTION_ARGS);

extern int nsegment_cmp_internal(const nsegment *ns1, const nsegment *ns2);
extern bool nsegment_eq_internal(const nsegment *ns1, const nsegment *ns2);
extern bool nsegment_ne_internal(const nsegment *ns1, const nsegment *ns2);
extern bool nsegment_lt_internal(const nsegment *ns1, const nsegment *ns2);
extern bool nsegment_le_internal(const nsegment *ns1, const nsegment *ns2);
extern bool nsegment_gt_internal(const nsegment *ns1, const nsegment *ns2);
extern bool nsegment_ge_internal(const nsegment *ns1, const nsegment *ns2);

extern bool route_exists(int64 rid);
extern double route_length(int64 rid);
extern Datum route_geom(int64 rid);
extern int64 rid_from_geom(Datum geom);

extern int npoint_srid_internal(const npoint *np);
extern Datum npoint_as_geom(PG_FUNCTION_ARGS);
extern Datum geom_as_npoint(PG_FUNCTION_ARGS);
extern Datum nsegment_as_geom(PG_FUNCTION_ARGS);
extern Datum geom_as_nsegment(PG_FUNCTION_ARGS);

extern Datum npoint_as_geom_internal(const npoint *np);
extern Datum nsegment_as_geom_internal(const nsegment *ns);
extern npoint *geom_as_npoint_internal(Datum geom);
extern nsegment *geom_as_nsegment_internal(Datum line);

extern Datum nsegmentarr_to_geom_internal(nsegment **segments, int count);

/*****************************************************************************/

#endif /* __TNPOINT_STATIC_H__ */
