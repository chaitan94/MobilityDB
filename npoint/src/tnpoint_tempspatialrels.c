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
 * Intersection functions
 *****************************************************************************/

/*
 * Returns whether two segments have network intersection (i.e. network
 * positions are same)
 * This function supposes that the two segments (1) are synchronized,
 * (2) are on the same rid, and (3) are not equal!
 */
bool
tnpointseq_intersection(const TemporalInst *start1, const TemporalInst *end1,
	const TemporalInst *start2, const TemporalInst *end2,
	Datum *inter1, Datum *inter2, TimestampTz *t)
{
	npoint *startnp1 = DatumGetNpoint(temporalinst_value(start1));
	npoint *endnp1 = DatumGetNpoint(temporalinst_value(end1));
	npoint *startnp2 = DatumGetNpoint(temporalinst_value(start2));
	npoint *endnp2 = DatumGetNpoint(temporalinst_value(end2));
	/* Compute the instant t at which the linear functions of the two segments
	   are equal: at + b = ct + d that is t = (d - b) / (a - c).
	   To reduce problems related to floating point arithmetic, t1 and t2
	   are shifted, respectively, to 0 and 1 before the computation */
	double x1 = startnp1->pos;
	double x2 = endnp1->pos;
	double x3 = startnp2->pos;
	double x4 = endnp2->pos;
	double denum = fabs(x2 - x1 - x4 + x3);
	if (denum == 0)
		/* Parallel segments */
		return false;

	double fraction = fabs((x3 - x1) / denum);
	if (fraction <= EPSILON || fraction >= (1.0 - EPSILON))
		/* Intersection occurs out of the period */
		return false;
	*inter1 = *inter2 =PointerGetDatum(npoint_make(startnp1->rid, fraction));
	*t = start1->t + (long) ((double) (end1->t - start1->t) * fraction);
	return true;
}

/*****************************************************************************
 * Generic binary functions for TNpoint rel Geometry
 * The last argument states whether we are computing tnpoint <trel> geo
 * or geo <trel> tnpoint 
 *****************************************************************************/

TemporalInst *
tspatialrel_tnpointinst_geo(const TemporalInst *inst, Datum geo,
	Datum (*func)(Datum, Datum), Oid valuetypid, bool invert)
{
	Datum geom = tnpointinst_geom(inst);
	Datum value = invert ? func(geo, geom) : func(geom, geo);
	TemporalInst *result = temporalinst_make(value, inst->t, valuetypid);
	pfree(DatumGetPointer(geom));
	DATUM_FREE(value, valuetypid);
	return result;
}

TemporalI *
tspatialrel_tnpointi_geo(const TemporalI *ti, Datum geo,
	Datum (*func)(Datum, Datum), Oid valuetypid, bool invert)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		Datum geom = tnpointinst_geom(inst);
		Datum value = invert ? func(geo, geom) : func(geom, geo);
		instants[i] = temporalinst_make(value, inst->t, valuetypid);

		pfree(DatumGetPointer(geom));
		DATUM_FREE(value, valuetypid);
	}
	TemporalI *result = temporali_make(instants, ti->count);
	for (int i = 0; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

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

PG_FUNCTION_INFO_V1(tintersects_tnpoint_tnpoint);

PGDLLEXPORT Datum
tintersects_tnpoint_tnpoint(PG_FUNCTION_ARGS)
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
	Temporal *sync1, *sync2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!synchronize_temporal_temporal(temp1, temp2, &sync1, &sync2, true))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}

	Temporal *geomsync1 = tnpoint_as_tgeompoint_internal(sync1);
	Temporal *geomsync2 = tnpoint_as_tgeompoint_internal(sync2);
	Temporal *result;
	ensure_valid_duration(sync1->duration);
	if (geomsync1->duration == TEMPORALINST)
		result = (Temporal *)sync_tfunc3_temporalinst_temporalinst((TemporalInst *)geomsync1,
			(TemporalInst *)geomsync2, dist, geom_dwithin2d, BOOLOID);
	else if (geomsync1->duration == TEMPORALI)
		result = (Temporal *)sync_tfunc3_temporali_temporali((TemporalI *)geomsync1,
			(TemporalI *)geomsync2, dist, geom_dwithin2d, BOOLOID);
	else if (geomsync1->duration == TEMPORALSEQ)
		result = (Temporal *)tdwithin_tpointseq_tpointseq((TemporalSeq *)geomsync1,
			(TemporalSeq *)geomsync2, dist, geom_dwithin2d);
	else /* geomsync1->duration == TEMPORALS */
		result = (Temporal *)tdwithin_tpoints_tpoints((TemporalS *)geomsync1,
			(TemporalS *)geomsync2, dist, geom_dwithin2d);

	pfree(sync1); pfree(sync2); 
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
