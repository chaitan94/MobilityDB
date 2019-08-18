/*****************************************************************************
 *
 * tnpoint_posops.c
 *	  Relative position operators for temporal network points.
 *
 * The following operators are defined for the spatial dimension:
 * - left, overleft, right, overright, below, overbelow, above, overabove,
 *   front, overfront, back, overback
 * There are no equivalent operators for the temporal geography points since
 * PostGIS does not currently provide such functionality for geography.
 * The following operators for the temporal dimension:
 * - before, overbefore, after, overafter
 * for both temporal geometry and geography points are "inherited" from the
 * basic temporal types. In this file they are defined when one of the
 * arguments is a stbox.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_posops.h"

#include "GeoBoundBoxOps.h"
#include "GeoRelativePosOps.h"
#include "tnpoint.h"
#include "tnpoint_static.h"

/*****************************************************************************/
/* stbox op Temporal */

PG_FUNCTION_INFO_V1(left_npoint_tnpoint);

PGDLLEXPORT Datum
left_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = left_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overleft_npoint_tnpoint);

PGDLLEXPORT Datum
overleft_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = overleft_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(right_npoint_tnpoint);

PGDLLEXPORT Datum
right_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = right_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overright_npoint_tnpoint);

PGDLLEXPORT Datum
overright_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = overright_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(below_npoint_tnpoint);

PGDLLEXPORT Datum
below_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = below_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overbelow_npoint_tnpoint);

PGDLLEXPORT Datum
overbelow_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = overbelow_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(above_npoint_tnpoint);

PGDLLEXPORT Datum
above_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = above_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overabove_npoint_tnpoint);

PGDLLEXPORT Datum
overabove_npoint_tnpoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box1, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box2, temp);
	bool result = overabove_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************/
/* Temporal op stbox */

PG_FUNCTION_INFO_V1(left_tnpoint_npoint);

PGDLLEXPORT Datum
left_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = left_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overleft_tnpoint_npoint);

PGDLLEXPORT Datum
overleft_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = overleft_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(right_tnpoint_npoint);

PGDLLEXPORT Datum
right_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = right_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overright_tnpoint_npoint);

PGDLLEXPORT Datum
overright_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = overright_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(below_tnpoint_npoint);

PGDLLEXPORT Datum
below_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = below_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overbelow_tnpoint_npoint);

PGDLLEXPORT Datum
overbelow_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = overbelow_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(above_tnpoint_npoint);

PGDLLEXPORT Datum
above_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = above_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(overabove_tnpoint_npoint);

PGDLLEXPORT Datum
overabove_tnpoint_npoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	npoint *np = PG_GETARG_NPOINT(1);
    Datum geom = npoint_as_geom_internal(np);
	GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geom);
    STBOX box1 = {0}, box2 = {0};
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();		
	}
	temporal_bbox(&box1, temp);
	bool result = overabove_stbox_stbox_internal(&box1, &box2);
    POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(geom));
    pfree(DatumGetPointer(geom));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_BOOL(result);
}

/*****************************************************************************/
