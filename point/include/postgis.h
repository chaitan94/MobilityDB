/*****************************************************************************
 *
 * postgis.h
 *	  PostGIS definitions that are needed in the extension but are not 
 *	  exported in PostGIS headers
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __POSTGIS_H__
#define __POSTGIS_H__

/*
 * This macro is based on PG_FREE_IF_COPY, except that it accepts two pointers.
 * See PG_FREE_IF_COPY comment in src/include/fmgr.h in postgres source code
 * for more details.
 */
#define POSTGIS_FREE_IF_COPY_P(ptrsrc, ptrori) \
	do { \
		if ((Pointer) (ptrsrc) != (Pointer) (ptrori)) \
			pfree(ptrsrc); \
	} while (0)

extern void srid_is_latlong(FunctionCallInfo fcinfo, int srid);
extern int clamp_srid(int srid);
extern int getSRIDbySRS(const char* srs);
extern char *getSRSbySRID(int32_t srid, bool short_crs);
extern int lwprint_double(double d, int maxdd, char* buf, size_t bufsize);
extern char getMachineEndian(void);

extern Datum transform(PG_FUNCTION_ARGS);
extern Datum buffer(PG_FUNCTION_ARGS);
extern Datum centroid(PG_FUNCTION_ARGS);

extern Datum geography_from_geometry(PG_FUNCTION_ARGS);
extern Datum geometry_from_geography(PG_FUNCTION_ARGS);

extern Datum contains(PG_FUNCTION_ARGS);
extern Datum containsproperly(PG_FUNCTION_ARGS);
extern Datum covers(PG_FUNCTION_ARGS);
extern Datum coveredby(PG_FUNCTION_ARGS);
extern Datum crosses(PG_FUNCTION_ARGS);
extern Datum disjoint(PG_FUNCTION_ARGS);
extern Datum ST_Equals(PG_FUNCTION_ARGS);
extern Datum intersects(PG_FUNCTION_ARGS); /* For 2D */
extern Datum intersects3d(PG_FUNCTION_ARGS); /* For 3D */
extern Datum overlaps(PG_FUNCTION_ARGS);
extern Datum touches(PG_FUNCTION_ARGS);
extern Datum within(PG_FUNCTION_ARGS);
extern Datum relate_full(PG_FUNCTION_ARGS);
extern Datum relate_pattern(PG_FUNCTION_ARGS);

extern Datum intersection(PG_FUNCTION_ARGS);
extern Datum distance(PG_FUNCTION_ARGS); /* For 2D */
extern Datum distance3d(PG_FUNCTION_ARGS); /* For 3D */

extern Datum LWGEOM_addpoint(PG_FUNCTION_ARGS);
extern Datum LWGEOM_azimuth(PG_FUNCTION_ARGS);
extern Datum LWGEOM_closestpoint(PG_FUNCTION_ARGS); /* For 2D */
extern Datum LWGEOM_collect_garray(PG_FUNCTION_ARGS);
extern Datum LWGEOM_dwithin(PG_FUNCTION_ARGS); /* For 2D */
extern Datum LWGEOM_dwithin3d(PG_FUNCTION_ARGS); /* For 3D */
extern Datum LWGEOM_geometryn_collection(PG_FUNCTION_ARGS);
extern Datum LWGEOM_get_srid(PG_FUNCTION_ARGS);	/* also for geography */
extern Datum LWGEOM_isempty(PG_FUNCTION_ARGS);
extern Datum LWGEOM_length_linestring(PG_FUNCTION_ARGS);
extern Datum LWGEOM_line_locate_point(PG_FUNCTION_ARGS);
extern Datum LWGEOM_line_interpolate_point(PG_FUNCTION_ARGS);
extern Datum LWGEOM_makepoint(PG_FUNCTION_ARGS);
extern Datum LWGEOM_numgeometries_collection(PG_FUNCTION_ARGS);
extern Datum LWGEOM_numpoints_linestring(PG_FUNCTION_ARGS);
extern Datum LWGEOM_pointn_linestring(PG_FUNCTION_ARGS);
extern Datum LWGEOM_setpoint_linestring(PG_FUNCTION_ARGS);
extern Datum LWGEOM_shortestline2d(PG_FUNCTION_ARGS); /* For 2D */
extern Datum LWGEOM_shortestline3d(PG_FUNCTION_ARGS); /* For 3D */

extern Datum lwgeom_eq(PG_FUNCTION_ARGS);
extern Datum lwgeom_lt(PG_FUNCTION_ARGS);
extern Datum lwgeom_hash(PG_FUNCTION_ARGS);

extern Datum geography_covers(PG_FUNCTION_ARGS);
extern Datum geography_length(PG_FUNCTION_ARGS);
extern Datum geography_dwithin(PG_FUNCTION_ARGS);
extern Datum geography_distance(PG_FUNCTION_ARGS);
extern Datum geography_azimuth(PG_FUNCTION_ARGS);
extern Datum geography_bestsrid(PG_FUNCTION_ARGS);

extern Datum geography_eq(PG_FUNCTION_ARGS);
extern Datum geography_lt(PG_FUNCTION_ARGS);

#define PG_GETARG_GSERIALIZED_P(varno) ((GSERIALIZED *)PG_DETOAST_DATUM(PG_GETARG_DATUM(varno)))

#endif /* __TEMPORAL_POSTGIS_H__ */
