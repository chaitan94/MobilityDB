/*****************************************************************************
 *
 * tnpoint_distance.c
 *	  Temporal distance for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_distance.h"

#include "temporaltypes.h"
#include "temporal_util.h"
#include "lifting.h"
#include "tpoint_spatialfuncs.h"
#include "tpoint_distance.h"
#include "tnpoint.h"
#include "tnpoint_static.h"
#include "tnpoint_spatialfuncs.h"
#include "tnpoint_tempspatialrels.h"

/*****************************************************************************
 * Temporal distance
 *****************************************************************************/

PG_FUNCTION_INFO_V1(distance_geo_tnpoint);

PGDLLEXPORT Datum
distance_geo_tnpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	ensure_point_type(gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = distance_tpoint_geo_internal(geomtemp,
		PointerGetDatum(gs));
	pfree(geomtemp);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(distance_npoint_tnpoint);

PGDLLEXPORT Datum
distance_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = distance_tpoint_geo_internal(geomtemp, geom);
	pfree(DatumGetPointer(geom));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(distance_tnpoint_geo);

PGDLLEXPORT Datum
distance_tnpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = distance_tpoint_geo_internal(geomtemp,
		PointerGetDatum(gs));
	pfree(geomtemp);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(distance_tnpoint_npoint);

PGDLLEXPORT Datum
distance_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
	Datum geom = npoint_as_geom_internal(np);
	Temporal *geomtemp = tnpoint_as_tgeompoint_internal(temp);
	Temporal *result = distance_tpoint_geo_internal(geomtemp, geom);
	pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

Temporal *
distance_tnpoint_tnpoint_internal(const Temporal *temp1, const Temporal *temp2)
{
	Temporal *sync1, *sync2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!synchronize_temporal_temporal(temp1, temp2, &sync1, &sync2, true))
		return NULL;
	
	Temporal *geomsync1 = tnpoint_as_tgeompoint_internal(sync1);
	Temporal *geomsync2 = tnpoint_as_tgeompoint_internal(sync2);
	Temporal *result = distance_tpoint_tpoint_internal(geomsync1, geomsync2);
	pfree(sync1); pfree(sync2);
	pfree(geomsync1); pfree(geomsync2);
	return result;
}

PG_FUNCTION_INFO_V1(distance_tnpoint_tnpoint);

PGDLLEXPORT Datum
distance_tnpoint_tnpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	Temporal *result = distance_tnpoint_tnpoint_internal(temp1, temp2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
