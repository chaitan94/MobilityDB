/*****************************************************************************
 *
 * tnpoint_tempspatialrels.c
 *	  Temporal spatial relationships for temporal network points.
 *
 * These relationships are applied at each instant and result in a temporal
 * boolean/text. The following relationships are supported:
 *		tcontains, tcovers, tcoveredby, tdisjoint,
 *		tequals, tintersects, ttouches, twithin, tdwithin, and
 *		trelate (with 2 and 3 arguments)
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_tempspatialrels.h"

#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H 1

#include <liblwgeom.h>

#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "lifting.h"
#include "tpoint_spatialfuncs.h"
#include "tpoint_spatialrels.h"
#include "tpoint_tempspatialrels.h"
#include "tnpoint.h"
#include "tnpoint_static.h"
#include "tnpoint_spatialfuncs.h"
#include "tnpoint_distance.h"

/*****************************************************************************
 * Temporal contains
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tcontains_geo_tnpoint);

PGDLLEXPORT Datum
tcontains_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_contains, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcontains_npoint_tnpoint);

PGDLLEXPORT Datum
tcontains_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_contains, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcontains_tnpoint_geo);

PGDLLEXPORT Datum
tcontains_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_contains, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcontains_tnpoint_npoint);

PGDLLEXPORT Datum
tcontains_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_contains, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal covers
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tcovers_geo_tnpoint);

PGDLLEXPORT Datum
tcovers_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_covers, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcovers_npoint_tnpoint);

PGDLLEXPORT Datum
tcovers_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_covers, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcovers_tnpoint_geo);

PGDLLEXPORT Datum
tcovers_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_covers, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcovers_tnpoint_npoint);

PGDLLEXPORT Datum
tcovers_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_covers, BOOLOID, false);
	pfree(geomtemp);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal coveredby
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tcoveredby_geo_tnpoint);

PGDLLEXPORT Datum
tcoveredby_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_coveredby, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcoveredby_npoint_tnpoint);

PGDLLEXPORT Datum
tcoveredby_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_coveredby, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcoveredby_tnpoint_geo);

PGDLLEXPORT Datum
tcoveredby_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_coveredby, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tcoveredby_tnpoint_npoint);

PGDLLEXPORT Datum
tcoveredby_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_coveredby, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal disjoint
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tdisjoint_geo_tnpoint);

PGDLLEXPORT Datum
tdisjoint_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_disjoint, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdisjoint_npoint_tnpoint);

PGDLLEXPORT Datum
tdisjoint_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_disjoint, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdisjoint_tnpoint_geo);

PGDLLEXPORT Datum
tdisjoint_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_disjoint, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdisjoint_tnpoint_npoint);

PGDLLEXPORT Datum
tdisjoint_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_disjoint, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdisjoint_tnpoint_tnpoint);

PGDLLEXPORT Datum
tdisjoint_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tnpoint(temp1, temp2);
	Temporal *geomtemp1 = tnpoint_as_tgeompoint_internal(temp1);
	Temporal *geomtemp2 = tnpoint_as_tgeompoint_internal(temp2);
	Temporal *result = sync_tfunc2_temporal_temporal_cross(geomtemp1, geomtemp2,
		&datum2_point_ne, BOOLOID);
	pfree(geomtemp1);
	pfree(geomtemp2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal equals
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tequals_geo_tnpoint);

PGDLLEXPORT Datum
tequals_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_equals, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tequals_npoint_tnpoint);

PGDLLEXPORT Datum
tequals_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_equals, BOOLOID, true);
	pfree(geomtemp);
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tequals_tnpoint_geo);

PGDLLEXPORT Datum
tequals_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_equals, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tequals_tnpoint_npoint);

PGDLLEXPORT Datum
tequals_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_equals, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tequals_tnpoint_tnpoint);

PGDLLEXPORT Datum
tequals_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tnpoint(temp1, temp2);
	Temporal *geomtemp1 = tnpoint_as_tgeompoint_internal(temp1);
	Temporal *geomtemp2 = tnpoint_as_tgeompoint_internal(temp2);
	Temporal *result = sync_tfunc2_temporal_temporal_cross(geomtemp1, geomtemp2,
		&datum2_point_eq, BOOLOID);
	pfree(geomtemp1);
	pfree(geomtemp2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal intersects
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tintersects_geo_tnpoint);

PGDLLEXPORT Datum
tintersects_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_intersects2d, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tintersects_npoint_tnpoint);

PGDLLEXPORT Datum
tintersects_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_intersects2d, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tintersects_tnpoint_geo);

PGDLLEXPORT Datum
tintersects_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_intersects2d, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tintersects_tnpoint_npoint);

PGDLLEXPORT Datum
tintersects_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_intersects2d, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal touches
 *****************************************************************************/

PG_FUNCTION_INFO_V1(ttouches_geo_tnpoint);

PGDLLEXPORT Datum
ttouches_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_touches, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(ttouches_npoint_tnpoint);

PGDLLEXPORT Datum
ttouches_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_touches, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(ttouches_tnpoint_geo);

PGDLLEXPORT Datum
ttouches_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_touches, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(ttouches_tnpoint_npoint);

PGDLLEXPORT Datum
ttouches_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_touches, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal within
 *****************************************************************************/

PG_FUNCTION_INFO_V1(twithin_geo_tnpoint);

PGDLLEXPORT Datum
twithin_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_within, BOOLOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(twithin_npoint_tnpoint);

PGDLLEXPORT Datum
twithin_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_within, BOOLOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(twithin_tnpoint_geo);

PGDLLEXPORT Datum
twithin_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_within, BOOLOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(twithin_tnpoint_npoint);

PGDLLEXPORT Datum
twithin_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_within, BOOLOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal dwithin
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tdwithin_geo_tnpoint);

PGDLLEXPORT Datum
tdwithin_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum dist = PG_GETARG_DATUM(2);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tdwithin_tpoint_geo_internal(geomtemp, gs, dist);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdwithin_npoint_tnpoint);

PGDLLEXPORT Datum
tdwithin_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum dist = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *) PG_DETOAST_DATUM(geom);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tdwithin_tpoint_geo_internal(geomtemp, gs, dist);
    pfree(gs);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdwithin_tnpoint_geo);

PGDLLEXPORT Datum
tdwithin_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	Datum dist = PG_GETARG_DATUM(2);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tdwithin_tpoint_geo_internal(geomtemp, gs, dist);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdwithin_tnpoint_npoint);

PGDLLEXPORT Datum
tdwithin_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum dist = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *) PG_DETOAST_DATUM(geom);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tdwithin_tpoint_geo_internal(geomtemp, gs, dist);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
    pfree(gs);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(tdwithin_tnpoint_tnpoint);

PGDLLEXPORT Datum
tdwithin_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Datum dist = PG_GETARG_DATUM(2);
	Temporal *geomsync1 = tnpoint_as_tgeompoint_internal(temp1);
	Temporal *geomsync2 = tnpoint_as_tgeompoint_internal(temp2);
	Temporal *result = tdwithin_tpoint_tpoint_internal(temp1, temp2, dist);
	pfree(geomsync1); pfree(geomsync2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal relate
 *****************************************************************************/

PG_FUNCTION_INFO_V1(trelate_geo_tnpoint);

PGDLLEXPORT Datum
trelate_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_relate, TEXTOID, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_npoint_tnpoint);

PGDLLEXPORT Datum
trelate_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_relate, TEXTOID, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_tnpoint_geo);

PGDLLEXPORT Datum
trelate_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, PointerGetDatum(gs),
		&geom_relate, TEXTOID, false);
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_tnpoint_npoint);

PGDLLEXPORT Datum
trelate_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel_tpoint_geo(geomtemp, geom,
		&geom_relate, TEXTOID, false);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_tnpoint_tnpoint);

PGDLLEXPORT Datum
trelate_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tnpoint(temp1, temp2);
	Temporal *geomtemp1 = tnpoint_as_tgeompoint_internal(temp1);
	Temporal *geomtemp2 = tnpoint_as_tgeompoint_internal(temp2);
	Temporal *result = sync_tfunc2_temporal_temporal_cross(geomtemp1, geomtemp2,
		&geom_relate, TEXTOID);
	pfree(geomtemp1);
	pfree(geomtemp2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Temporal relate_pattern
 *****************************************************************************/

PG_FUNCTION_INFO_V1(trelate_pattern_geo_tnpoint);

PGDLLEXPORT Datum
trelate_pattern_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum pattern = PG_GETARG_DATUM(2);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel3_tpoint_geo(geomtemp, PointerGetDatum(gs),
		pattern, &geom_relate_pattern, true);
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_pattern_npoint_tnpoint);

PGDLLEXPORT Datum
trelate_pattern_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np  = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum pattern = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel3_tpoint_geo(geomtemp, geom, pattern,
		&geom_relate_pattern, true);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_pattern_tnpoint_geo);

PGDLLEXPORT Datum
trelate_pattern_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	Datum pattern = PG_GETARG_DATUM(2);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel3_tpoint_geo(geomtemp, PointerGetDatum(gs),
		pattern, &geom_relate_pattern, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_pattern_tnpoint_npoint);

PGDLLEXPORT Datum
trelate_pattern_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np  = PG_GETARG_NPOINT(1);
	Datum pattern = PG_GETARG_DATUM(2);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = tspatialrel3_tpoint_geo(geomtemp, geom, pattern,
		&geom_relate_pattern, false);
	PG_FREE_IF_COPY(temp, 0);
    pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(trelate_pattern_tnpoint_tnpoint);

PGDLLEXPORT Datum
trelate_pattern_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Datum pattern = PG_GETARG_DATUM(2);
	ensure_same_srid_tnpoint(temp1, temp2);
	Temporal *geomtemp1 = tnpoint_as_tgeompoint_internal(temp1);
	Temporal *geomtemp2 = tnpoint_as_tgeompoint_internal(temp2);
	Temporal *result = sync_tfunc3_temporal_temporal_cross(geomtemp1, geomtemp2,
		pattern, &geom_relate_pattern, BOOLOID);
	pfree(geomtemp1);
	pfree(geomtemp2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
