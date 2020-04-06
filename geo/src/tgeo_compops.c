/*****************************************************************************
 *
 * tgeo_compops.c
 *    Comparison functions and operators for temporal geometries.
 *
 * Portions Copyright (c) 2020, Maxime Schoemans, Esteban Zimanyi, 
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tgeo_compops.h"

#include <utils/builtins.h>
#include <utils/timestamp.h>

#include "lifting.h"
#include "temporal.h"
#include "temporaltypes.h"
#include "temporal_util.h"
#include "temporal_compops.h"

#include "tpoint_spatialfuncs.h"
#include "tgeo_spatialfuncs.h"

/*****************************************************************************/

PG_FUNCTION_INFO_V1(teq_geo_tgeo);

PGDLLEXPORT Datum
teq_geo_tgeo(PG_FUNCTION_ARGS)
{
    GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
    ensure_polygon_type(gs);
    Temporal *temp = PG_GETARG_TEMPORAL(1);
    ensure_same_srid_tpoint_gs(temp, gs);
    ensure_same_dimensionality_tpoint_gs(temp, gs);
    Oid datumtypid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    Temporal *result = tcomp_temporal_base(temp, PointerGetDatum(gs), datumtypid,
        &datum2_eq2, true);
    PG_FREE_IF_COPY(gs, 0);
    PG_FREE_IF_COPY(temp, 1);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(teq_tgeo_geo);

PGDLLEXPORT Datum
teq_tgeo_geo(PG_FUNCTION_ARGS)
{
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
    ensure_polygon_type(gs);
    ensure_same_srid_tpoint_gs(temp, gs);
    ensure_same_dimensionality_tpoint_gs(temp, gs);
    Oid datumtypid = get_fn_expr_argtype(fcinfo->flinfo, 1);
    Temporal *result = tcomp_temporal_base(temp, PointerGetDatum(gs), datumtypid,
        &datum2_eq2, false);
    PG_FREE_IF_COPY(temp, 0);
    PG_FREE_IF_COPY(gs, 1);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(teq_tgeo_tgeo);

PGDLLEXPORT Datum
teq_tgeo_tgeo(PG_FUNCTION_ARGS)
{
    Temporal *temp1 = PG_GETARG_TEMPORAL(0);
    Temporal *temp2 = PG_GETARG_TEMPORAL(1);
    ensure_same_srid_tpoint(temp1, temp2);
    ensure_same_dimensionality_tpoint(temp1, temp2);
    Temporal *result = sync_tfunc4_temporal_temporal_cross(temp1, temp2,
        &datum2_eq2, BOOLOID);
    PG_FREE_IF_COPY(temp1, 0);
    PG_FREE_IF_COPY(temp2, 1);
    if (result == NULL)
        PG_RETURN_NULL();
    PG_RETURN_POINTER(result);
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(tne_geo_tgeo);

PGDLLEXPORT Datum
tne_geo_tgeo(PG_FUNCTION_ARGS)
{
    GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
    ensure_point_type(gs);
    Temporal *temp = PG_GETARG_TEMPORAL(1);
    ensure_same_srid_tpoint_gs(temp, gs);
    ensure_same_dimensionality_tpoint_gs(temp, gs);
    Oid datumtypid = get_fn_expr_argtype(fcinfo->flinfo, 0);
    Temporal *result = tcomp_temporal_base(temp, PointerGetDatum(gs), datumtypid,
        &datum2_ne2, true);
    PG_FREE_IF_COPY(gs, 0);
    PG_FREE_IF_COPY(temp, 1);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tne_tgeo_geo);

PGDLLEXPORT Datum
tne_tgeo_geo(PG_FUNCTION_ARGS)
{
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
    ensure_point_type(gs);
    ensure_same_srid_tpoint_gs(temp, gs);
    ensure_same_dimensionality_tpoint_gs(temp, gs);
    Oid datumtypid = get_fn_expr_argtype(fcinfo->flinfo, 1);
    Temporal *result = tcomp_temporal_base(temp, PointerGetDatum(gs), datumtypid,
        &datum2_ne2, false);
    PG_FREE_IF_COPY(temp, 0);
    PG_FREE_IF_COPY(gs, 1);
    PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tne_tgeo_tgeo);

PGDLLEXPORT Datum
tne_tgeo_tgeo(PG_FUNCTION_ARGS)
{
    Temporal *temp1 = PG_GETARG_TEMPORAL(0);
    Temporal *temp2 = PG_GETARG_TEMPORAL(1);
    ensure_same_srid_tpoint(temp1, temp2);
    ensure_same_dimensionality_tpoint(temp1, temp2);
    Temporal *result = sync_tfunc4_temporal_temporal_cross(temp1, temp2,
        &datum2_ne2, BOOLOID);
    PG_FREE_IF_COPY(temp1, 0);
    PG_FREE_IF_COPY(temp2, 1);
    if (result == NULL)
        PG_RETURN_NULL();
    PG_RETURN_POINTER(result);
}

/*****************************************************************************/