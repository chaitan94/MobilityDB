/***********************************************************************
 *
 * tpoint_spatialfuncs.c
 *	  Spatial functions for temporal points.
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tpoint_spatialfuncs.h"

#include <assert.h>
#include <float.h>
#include <math.h>
#include <utils/builtins.h>
#include <utils/timestamp.h>

#include "period.h"
#include "periodset.h"
#include "timeops.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "lifting.h"
#include "tnumber_mathfuncs.h"
#include "postgis.h"
#include "tpoint.h"
#include "tpoint_boxops.h"
#include "tpoint_distance.h"
#include "tpoint_spatialrels.h"
#include "tnpoint_spatialfuncs.h"

/***********************************************************************
 * Interpolate a point along a geographic line.
 * These functions are an extension to PostGIS
 ***********************************************************************/

/**
 * Find interpolation point p
 * between geography points p1 and p2
 * so that the len(p1,p) == len(p1,p2) * f
 * and p falls on p1,p2 segment.
 */
void
geography_interpolate_point4d(
	const POINT3D *p1, const POINT3D *p2, /* 3-space points we are interpolating between */
	const POINT4D *v1, const POINT4D *v2, /* real values and z/m values */
	double f, /* fraction */
	POINT4D *p) /* write out results here */
{
	/* Calculate interpolated point */
	POINT3D mid;
	mid.x = p1->x + ((p2->x - p1->x) * f);
	mid.y = p1->y + ((p2->y - p1->y) * f);
	mid.z = p1->z + ((p2->z - p1->z) * f);
	normalize(&mid);

	/* Calculate z/m values */
	GEOGRAPHIC_POINT g;
	cart2geog(&mid, &g);
	p->x = rad2deg(g.lon);
	p->y = rad2deg(g.lat);
	p->z = v1->z + ((v2->z - v1->z) * f);
	p->m = v1->m + ((v2->m - v1->m) * f);
}

POINTARRAY* geography_interpolate_points(const LWLINE *line, double length_fraction,
	const SPHEROID *s, bool repeat)
{
	POINT4D pt;
	uint32_t i;
	uint32_t points_to_interpolate;
	uint32_t points_found = 0;
	double line_length, curr_length;
	double length_increment = length_fraction;
	double length_consumed = 0;
	char has_z = (char) lwgeom_has_z(lwline_as_lwgeom(line));
	char has_m = (char) lwgeom_has_m(lwline_as_lwgeom(line));
	const POINTARRAY* ipa = line->points;
	POINTARRAY* opa;
	POINT4D p1, p2;
	POINT3D q1, q2;
	GEOGRAPHIC_POINT g1, g2;

	/* Empty.InterpolatePoint == Point Empty */
	if ( lwline_is_empty(line) )
	{
		return ptarray_construct_empty(has_z, has_m, 0);
	}

	/* If distance is one of the two extremes, return the point on that
	 * end rather than doing any computations
	 */
	if ( length_fraction == 0.0 || length_fraction == 1.0 )
	{
		if ( length_fraction == 0.0 )
			getPoint4d_p(ipa, 0, &pt);
		else
			getPoint4d_p(ipa, ipa->npoints-1, &pt);

		opa = ptarray_construct(has_z, has_m, 1);
		ptarray_set_point4d(opa, 0, &pt);

		return opa;
	}

	/* Interpolate points along the line */
	line_length = ptarray_length_spheroid(ipa, s);
	curr_length = line_length * length_increment;

	points_to_interpolate = repeat ? (uint32_t) floor(1 / length_fraction) : 1;
	opa = ptarray_construct(has_z, has_m, points_to_interpolate);

	getPoint4d_p(ipa, 0, &p1);
	geographic_point_init(p1.x, p1.y, &g1);
	for ( i = 0; i < ipa->npoints - 1 && points_found < points_to_interpolate; i++ )
	{
		getPoint4d_p(ipa, i+1, &p2);
		geographic_point_init(p2.x, p2.y, &g2);
		double segment_length = spheroid_distance(&g1, &g2, s);

		/* If our target distance is before the total length we've seen
		 * so far. create a new point some distance down the current
		 * segment.
		 */
		while ( curr_length < length_consumed + segment_length && points_found < points_to_interpolate )
		{
			geog2cart(&g1, &q1);
			geog2cart(&g2, &q2);
			double segment_fraction = (curr_length - length_consumed) / segment_length;
			geography_interpolate_point4d(&q1, &q2, &p1, &p2, segment_fraction, &pt);
			ptarray_set_point4d(opa, points_found++, &pt);
			length_increment += length_fraction;
			curr_length = line_length * length_increment;
		}

		length_consumed += segment_length;

		p1 = p2;
		g1 = g2;
	}

	/* Return the last point on the line. This shouldn't happen, but
	 * could if there's some floating point rounding errors. */
	if (points_found < points_to_interpolate) {
		getPoint4d_p(ipa, ipa->npoints - 1, &pt);
		ptarray_set_point4d(opa, points_found, &pt);
	}

    return opa;
}

void spheroid_init(SPHEROID *s, double a, double b)
{
	s->a = a;
	s->b = b;
	s->f = (a - b) / a;
	s->e_sq = (a*a - b*b)/(a*a);
	s->radius = (2.0 * a + b ) / 3.0;
}

PG_FUNCTION_INFO_V1(geography_line_interpolate_point);
Datum geography_line_interpolate_point(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gser = PG_GETARG_GSERIALIZED_P(0);
	double distance_fraction = PG_GETARG_FLOAT8(1);
	/* Read calculation type */
	bool use_spheroid = true;
	if ( PG_NARGS() > 2 && ! PG_ARGISNULL(2) )
		use_spheroid = PG_GETARG_BOOL(2);
	/* Read repeat mode */
	bool repeat = PG_NARGS() > 3 && PG_GETARG_BOOL(3);
	int srid = gserialized_get_srid(gser);
	LWLINE* lwline;
	LWGEOM* lwresult;
	POINTARRAY* opa;
	SPHEROID s;
	GSERIALIZED *result;

	if ( distance_fraction < 0 || distance_fraction > 1 )
	{
		elog(ERROR,"line_interpolate_point: 2nd arg isn't within [0,1]");
		PG_FREE_IF_COPY(gser, 0);
		PG_RETURN_NULL();
	}

	if ( gserialized_get_type(gser) != LINETYPE )
	{
		elog(ERROR,"line_interpolate_point: 1st arg isn't a line");
		PG_FREE_IF_COPY(gser, 0);
		PG_RETURN_NULL();
	}

	/* Initialize spheroid */
	/* We currently cannot use the following statement since PROJ4 API is not
	 * available directly to MobilityDB. */
	// spheroid_init_from_srid(fcinfo, srid, &s);
	spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

	/* Set to sphere if requested */
	if ( ! use_spheroid )
		s.a = s.b = s.radius;

	lwline = lwgeom_as_lwline(lwgeom_from_gserialized(gser));
	opa = geography_interpolate_points(lwline, distance_fraction, &s, repeat);

	lwgeom_free(lwline_as_lwgeom(lwline));
	PG_FREE_IF_COPY(gser, 0);

	if (opa->npoints <= 1)
	{
		lwresult = lwpoint_as_lwgeom(lwpoint_construct(srid, NULL, opa));
	} else {
		lwresult = lwmpoint_as_lwgeom(lwmpoint_construct(srid, opa));
	}

	result = geometry_serialize(lwresult);
	lwgeom_free(lwresult);

	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

double
ptarray_locate_point_spheroid(const POINTARRAY *pa, const POINT4D *p4d,
	const SPHEROID *s, double tolerance, double *mindistout, POINT4D *proj4d)
{
	GEOGRAPHIC_EDGE e;
	GEOGRAPHIC_POINT a, b, nearest;
	POINT4D p1, p2;
	const POINT2D *p;
	POINT2D proj;
	uint32_t i, seg = 0;
	int use_sphere = (s->a == s->b ? 1 : 0);
	int hasz;
	double za = 0.0, zb = 0.0;
	double distance,
		length, 	/* Used for computing lengths */
		seglength, /* length of the segment where the closest point is located */
		partlength, /* length from the beginning of the point array to the closest point */
		totlength;  /* length of the point array */

	/* Initialize our point */
	geographic_point_init(p4d->x, p4d->y, &a);

	/* Handle point/point case here */
	if ( pa->npoints <= 1)
	{
		if ( pa->npoints == 1 )
		{
			p = getPoint2d_cp(pa, 0);
			geographic_point_init(p->x, p->y, &b);
			/* Sphere special case, axes equal */
			*mindistout = s->radius * sphere_distance(&a, &b);
			/* If close or greater than tolerance, get the real answer to be sure */
			if ( ! use_sphere || *mindistout > 0.95 * tolerance )
				*mindistout = spheroid_distance(&a, &b, s);
		}
		return 0.0;
	}

	/* Make result really big, so that everything will be smaller than it */
	distance = FLT_MAX;

	/* Initialize start of line */
	p = getPoint2d_cp(pa, 0);
	geographic_point_init(p->x, p->y, &(e.start));

	/* Iterate through the edges in our line */
	for ( i = 1; i < pa->npoints; i++ )
	{
		double d;
		p = getPoint2d_cp(pa, i);
		geographic_point_init(p->x, p->y, &(e.end));
		/* Get the spherical distance between point and edge */
		d = s->radius * edge_distance_to_point(&e, &a, &b);
		/* New shortest distance! Record this distance / location / segment */
		if ( d < distance )
		{
			distance = d;
			nearest = b;
			seg = i - 1;
		}
		/* We've gotten closer than the tolerance... */
		if ( d < tolerance )
		{
			/* Working on a sphere? The answer is correct, return */
			if ( use_sphere )
			{
				break;
			}
			/* Far enough past the tolerance that the spheroid calculation won't change things */
			else if ( d < tolerance * 0.95 )
			{
				break;
			}
			/* On a spheroid and near the tolerance? Confirm that we are *actually* closer than tolerance */
			else
			{
				d = spheroid_distance(&a, &nearest, s);
				/* Yes, closer than tolerance, return! */
				if ( d < tolerance )
					break;
			}
		}
		e.start = e.end;
	}

	if ( mindistout ) *mindistout = distance;

	/* See if we have a third dimension */
	hasz = FLAGS_GET_Z(pa->flags);

	/* Initialize first point of array */
	getPoint4d_p(pa, 0, &p1);
	geographic_point_init(p1.x, p1.y, &a);
	if ( hasz )
		za = p1.z;

	partlength = 0.0;
	totlength = 0.0;

	/* Loop and sum the length for each segment */
	for ( i = 1; i < pa->npoints; i++ )
	{
		getPoint4d_p(pa, i, &p1);
		geographic_point_init(p1.x, p1.y, &b);
		if ( hasz )
			zb = p1.z;

		/* Special sphere case */
		if ( s->a == s->b )
			length = s->radius * sphere_distance(&a, &b);
		/* Spheroid case */
		else
			length = spheroid_distance(&a, &b, s);

		/* Add in the vertical displacement if we're in 3D */
		if ( hasz )
			length = sqrt( (zb-za)*(zb-za) + length*length );

		/* Add this segment length to the total length */
		totlength += length;

		/* Add this segment length to the partial length */
		if (i < seg)
			partlength += length;
		else if (i == seg)
			/* Save segment length */
			seglength = length;

		/* B gets incremented in the next loop, so we save the value here */
		a = b;
		za = zb;
	}

	/* Copy nearest into 2D/4D holder */
	proj4d->x = proj.x = rad2deg(nearest.lon);
	proj4d->y = proj.y = rad2deg(nearest.lat);

	/* Compute distance from beginning of the segment to closest point */

	/* Start of the segment */
	getPoint4d_p(pa, seg, &p1);
	geographic_point_init(p1.x, p1.y, &a);

	/* Closest point */
	geographic_point_init(proj4d->x, proj4d->y, &b);

	/* Special sphere case */
	if ( s->a == s->b )
		length = s->radius * sphere_distance(&a, &b);
	/* Spheroid case */
	else
		length = spheroid_distance(&a, &b, s);

	if ( hasz )
	{
		/* Compute Z and M values for closest point */
		double f = length / seglength;
		getPoint4d_p(pa, seg + 1, &p2);
		proj4d->z = p1.z + ((p2.z - p1.z) * f);
		proj4d->m = p1.m + ((p2.m - p1.m) * f);
		/* Add in the vertical displacement if we're in 3D */
		za = p1.z;
		zb = proj4d->z;
		length = sqrt( (zb-za)*(zb-za) + length*length );
	}

	/* Add this segment length to the total */
	partlength += length;

	/* Location of any point on a zero-length line is 0 */
	/* See http://trac.osgeo.org/postgis/ticket/1772#comment:2 */
	if ( totlength == 0 )
		return 0;

	/* For robustness, force 1 when closest point == endpoint */
	p = getPoint2d_cp(pa, pa->npoints - 1);
	if ( (seg >= (pa->npoints-2)) && p2d_same(&proj, p) )
		return 1.0;

	return partlength / totlength;
}

Datum geography_line_locate_point(PG_FUNCTION_ARGS);
PG_FUNCTION_INFO_V1(geography_line_locate_point);
Datum geography_line_locate_point(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gser1 = PG_GETARG_GSERIALIZED_P(0);
	GSERIALIZED *gser2 = PG_GETARG_GSERIALIZED_P(1);
	bool use_spheroid = true;
	/* Read our calculation type */
	if ( PG_NARGS() > 3 && ! PG_ARGISNULL(3) )
		use_spheroid = PG_GETARG_BOOL(3);
	double tolerance = FP_TOLERANCE;
	SPHEROID s;
	LWLINE *lwline;
	LWPOINT *lwpoint;
	POINTARRAY *pa;
	POINT4D p, p_proj;
	double ret;

	/* Initialize spheroid */
	/* We currently cannot use the following statement since PROJ4 API is not
	 * available directly to MobilityDB. */
	// spheroid_init_from_srid(fcinfo, gserialized_get_srid(gser1), &s);
	spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

	/* Set to sphere if requested */
	if ( ! use_spheroid )
		s.a = s.b = s.radius;


	if ( gserialized_get_type(gser1) != LINETYPE )
	{
		elog(ERROR,"line_locate_point: 1st arg isn't a line");
		PG_RETURN_NULL();
	}
	if ( gserialized_get_type(gser2) != POINTTYPE )
	{
		elog(ERROR,"line_locate_point: 2st arg isn't a point");
		PG_RETURN_NULL();
	}

	/* User requests spherical calculation, turn our spheroid into a sphere */
	if ( ! use_spheroid )
		s.a = s.b = s.radius;
	else
		/* Initialize spheroid */
		/* We cannot use the following statement since PROJ4 API is not
		 * available directly to MobilityDB. */
		// spheroid_init_from_srid(fcinfo, srid, &s);
		spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

	error_if_srid_mismatch(gserialized_get_srid(gser1), gserialized_get_srid(gser2));

	lwline = lwgeom_as_lwline(lwgeom_from_gserialized(gser1));
	lwpoint = lwgeom_as_lwpoint(lwgeom_from_gserialized(gser2));

	pa = lwline->points;
	lwpoint_getPoint4d_p(lwpoint, &p);

	ret = ptarray_locate_point_spheroid(pa, &p, &s, tolerance, NULL, &p_proj);

	PG_RETURN_FLOAT8(ret);
}

/*****************************************************************************
 * Functions specializing the PostGIS functions ST_LineInterpolatePoint and
 * ST_LineLocatePoint.
 *****************************************************************************/

Datum
geomseg_interpolate_point(Datum start, Datum end, double ratio)
{
	GSERIALIZED *gs1 = (GSERIALIZED *) DatumGetPointer(start);
	int srid = gserialized_get_srid(gs1);
	POINT4D p1 = datum_get_point4d(start);
	POINT4D p2 = datum_get_point4d(end);
	POINT4D p;
	interpolate_point4d(&p1, &p2, &p, ratio);
	LWPOINT *lwpoint = FLAGS_GET_Z(gs1->flags) ?
		lwpoint_make3dz(srid, p.x, p.y, p.z) :
		lwpoint_make2d(srid, p.x, p.y);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *) lwpoint));
	lwpoint_free(lwpoint);
	POSTGIS_FREE_IF_COPY_P(gs1, DatumGetPointer(start));
	return result;
}

double
geomseg_locate_point(Datum start, Datum end, Datum point, double *dist)
{
	GSERIALIZED *gs1 = (GSERIALIZED *) DatumGetPointer(start);
	POINT4D p1 = datum_get_point4d(start);
	POINT4D p2 = datum_get_point4d(end);
	POINT4D p = datum_get_point4d(point);
	POINT4D proj;
	closest_point_on_segment(&p, &p1, &p2, &proj);
	/* Return the distance between the segment and the point if requested */
	if (dist != NULL)
	{
		*dist = FLAGS_GET_Z(gs1->flags) ?
			distance3d_pt_pt((POINT3D *)&p, (POINT3D *)&proj) :
			distance2d_pt_pt((POINT2D *)&p, (POINT2D *)&proj);
	}
	/* Compute the result */
	double result;
	if (p4d_same(&p1, &proj))
		result = 0.0;
	else if (p4d_same(&p2, &proj))
		result = 1.0;
	else
	{
		result = FLAGS_GET_Z(gs1->flags) ?
			sqrt(distance3d_sqr_pt_pt((POINT3D *)&p1, (POINT3D *)&proj) /
				distance3d_sqr_pt_pt((POINT3D *)&p1, (POINT3D *)&p2)) :
			sqrt(distance2d_sqr_pt_pt((POINT2D *)&p1, (POINT2D *)&proj) /
				distance2d_sqr_pt_pt((POINT2D *)&p1, (POINT2D *)&p2));
	}
	return result;
}

/*****************************************************************************/

Datum
geogseg_interpolate_point(Datum start, Datum end, double ratio)
{
	GSERIALIZED *gs1 = (GSERIALIZED *) DatumGetPointer(start);
	int srid = gserialized_get_srid(gs1);
	POINT4D p1 = datum_get_point4d(start);
	POINT4D p2 = datum_get_point4d(end);
	POINT4D p;
	POINT3D q1, q2;
	GEOGRAPHIC_POINT g1, g2;
	geographic_point_init(p1.x, p1.y, &g1);
	geographic_point_init(p2.x, p2.y, &g2);
	geog2cart(&g1, &q1);
	geog2cart(&g2, &q2);
	geography_interpolate_point4d(&q1, &q2, &p1, &p2, ratio, &p);
	LWPOINT *lwpoint = FLAGS_GET_Z(gs1->flags) ?
		lwpoint_make3dz(srid, p.x, p.y, p.z) :
		lwpoint_make2d(srid, p.x, p.y);
	FLAGS_SET_GEODETIC(lwpoint->flags, true);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *) lwpoint));
	lwpoint_free(lwpoint);
	POSTGIS_FREE_IF_COPY_P(gs1, DatumGetPointer(start));
	return result;
}

/*
 * Write into the *proj4d argument the coordinates of the closest point on
 * the given segment AB to the reference input point p.
 */
double
closest_point_on_segment_spheroid(const POINT4D *p, const POINT4D *A, const POINT4D *B,
	const SPHEROID *s, POINT4D *proj4d)
{
	GEOGRAPHIC_EDGE e;
	GEOGRAPHIC_POINT a, closest;
	double length, /* length from A to the closest point */
		seglength; /* length of the segment AB */

	/* Initialize point */
	geographic_point_init(p->x, p->y, &a);

	/* Initialize edge */
	geographic_point_init(A->x, A->y, &(e.start));
	geographic_point_init(B->x, B->y, &(e.end));

	/* Get the spherical distance between point and edge */
	edge_distance_to_point(&e, &a, &closest);

	/* Copy nearest into returning argument */
	proj4d->x = rad2deg(closest.lon);
	proj4d->y = rad2deg(closest.lat);

	/* Compute distance from beginning of the segment to closest point */

	/* Special sphere case */
	if ( s->a == s->b )
	{
		seglength = s->radius * sphere_distance(&(e.start), &(e.end));
		length = s->radius * sphere_distance(&(e.start), &closest);
	}
	/* Spheroid case */
	else
	{
		seglength = spheroid_distance(&(e.start), &(e.end), s);
		length = spheroid_distance(&(e.start), &closest, s);
	}

	/* Compute Z and M values for closest point */
	double ratio = length / seglength;
	proj4d->z = A->z + ((B->z - A->z) * ratio);
	proj4d->m = A->m + ((B->m - A->m) * ratio);
	return ratio;
}

/* In the current version of MobilityDB all geographic computations are
 * done in using a sphere and not a spheroid */
double
geogseg_locate_point(Datum start, Datum end, Datum point, double *dist)
{
	GSERIALIZED *gs1 = (GSERIALIZED *) DatumGetPointer(start);
	POINT4D p1 = datum_get_point4d(start);
	POINT4D p2 = datum_get_point4d(end);
	POINT4D p = datum_get_point4d(point);
	POINT4D proj;
	GEOGRAPHIC_POINT a, b;
	SPHEROID s;
	double d;
	bool use_spheroid = true;

	/* Initialize spheroid */
	spheroid_init(&s, WGS84_MAJOR_AXIS, WGS84_MINOR_AXIS);

	/* Set to sphere if requested */
	if ( ! use_spheroid )
		s.a = s.b = s.radius;

	/* Get the closest point */
	double ratio = closest_point_on_segment_spheroid(&p, &p1, &p2, &s, &proj);

	/* Compute the distance between the segment and the point */
	geographic_point_init(p1.x, p1.y, &a);
	geographic_point_init(proj.x, proj.y, &b);
	/* Special sphere case */
	if ( s.a == s.b )
		d = s.radius * sphere_distance(&a, &b);
	/* Spheroid case */
	else
		d = spheroid_distance(&a, &b, &s);
	/* Add in the vertical displacement if we're in 3D */
	if (FLAGS_GET_Z(gs1->flags))
		d = sqrt( (proj.z - p.z) * (proj.z - p.z) + d*d );
	/* Return the distance between the segment and the point if requested */
	if (dist != NULL)
		*dist = d;

	/* Compute the result */
	double result;
	if (p4d_same(&p1, &proj))
		result = 0.0;
	else if (p4d_same(&p2, &proj))
		result = 1.0;
	else
		result = ratio;
	return result;
}

/*****************************************************************************
 * Functions derived from PostGIS to increase floating-point precision
 *****************************************************************************/

double
distance3d_sqr_pt_pt(const POINT3D *p1, const POINT3D *p2)
{
  double dx = p2->x - p1->x;
  double dy = p2->y - p1->y;
  double dz = p2->z - p1->z;
  return dx*dx + dy*dy + dz*dz;
}

/*
 *  Returns the ratio of the closest point on the segment wrt to the given point.
 * Function derived from PostGIS function closest_point_on_segment
 */
double
closest_point_on_segment_ratio(const POINT4D *p, const POINT4D *A, const POINT4D *B)
{
	double r;

	if (FP_EQUALS(A->x, B->x) && FP_EQUALS(A->y, B->y))
		return 0.0;

	/*
	 * We use comp.graphics.algorithms Frequently Asked Questions method
	 *
	 * (1)           AC dot AB
	 *           r = ----------
	 *                ||AB||^2
	 *	r has the following meaning:
	 *	r=0 P = A
	 *	r=1 P = B
	 *	r<0 P is on the backward extension of AB
	 *	r>1 P is on the forward extension of AB
	 *	0<r<1 P is interior to AB
	 *
	 */
	r = ( (p->x-A->x) * (B->x-A->x) + (p->y-A->y) * (B->y-A->y) ) /
		( (B->x-A->x) * (B->x-A->x) + (B->y-A->y) * (B->y-A->y) );

	if (r < 0)
		return 0.0;
	if (r > 1)
		return 1.0;
	return r;
}

/*****************************************************************************
 * Parameter tests
 *****************************************************************************/

void
ensure_not_geodetic_stbox(const STBOX *box)
{
	if (MOBDB_FLAGS_GET_X(box->flags) && MOBDB_FLAGS_GET_GEODETIC(box->flags))
		elog(ERROR, "The box must cannot be geodetic");
}

void
ensure_same_geodetic_stbox(const STBOX *box1, const STBOX *box2)
{
	if (MOBDB_FLAGS_GET_X(box1->flags) && MOBDB_FLAGS_GET_X(box2->flags) &&
		MOBDB_FLAGS_GET_GEODETIC(box1->flags) != MOBDB_FLAGS_GET_GEODETIC(box2->flags))
		elog(ERROR, "The boxes must be both planar or both geodetic");
}

void
ensure_same_geodetic_tpoint_stbox(const Temporal *temp, const STBOX *box)
{
	if (MOBDB_FLAGS_GET_X(box->flags) &&
		MOBDB_FLAGS_GET_GEODETIC(temp->flags) != MOBDB_FLAGS_GET_GEODETIC(box->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point and the box must be both planar or both geodetic")));
}

void
ensure_same_srid_stbox(const STBOX *box1, const STBOX *box2)
{
	if (MOBDB_FLAGS_GET_X(box1->flags) && MOBDB_FLAGS_GET_X(box2->flags) &&
		box1->srid != box2->srid)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The boxes must be in the same SRID")));
}

void
ensure_same_srid_tpoint(const Temporal *temp1, const Temporal *temp2)
{
	if (tpoint_srid_internal(temp1) != tpoint_srid_internal(temp2))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal points must be in the same SRID")));
}

void
ensure_same_srid_tpoint_stbox(const Temporal *temp, const STBOX *box)
{
	if (MOBDB_FLAGS_GET_X(box->flags) &&
		tpoint_srid_internal(temp) != box->srid)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point and the box must be in the same SRID")));
}

void
ensure_same_srid_tpoint_gs(const Temporal *temp, const GSERIALIZED *gs)
{
	if (tpoint_srid_internal(temp) != gserialized_get_srid(gs))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point and the geometry must be in the same SRID")));
}

void
ensure_same_dimensionality_stbox(const STBOX *box1, const STBOX *box2)
{
	if (MOBDB_FLAGS_GET_X(box1->flags) != MOBDB_FLAGS_GET_X(box2->flags) ||
		MOBDB_FLAGS_GET_Z(box1->flags) != MOBDB_FLAGS_GET_Z(box2->flags) ||
		MOBDB_FLAGS_GET_T(box1->flags) != MOBDB_FLAGS_GET_T(box2->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The boxes must be of the same dimensionality")));
}

void
ensure_same_dimensionality_tpoint(const Temporal *temp1, const Temporal *temp2)
{
	if (MOBDB_FLAGS_GET_Z(temp1->flags) != MOBDB_FLAGS_GET_Z(temp2->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal points must be of the same dimensionality")));
}

void
ensure_same_dimensionality_tpoint_stbox(const Temporal *temp, const STBOX *box)
{
	if (MOBDB_FLAGS_GET_X(temp->flags) != MOBDB_FLAGS_GET_X(box->flags) ||
		MOBDB_FLAGS_GET_Z(temp->flags) != MOBDB_FLAGS_GET_Z(box->flags) ||
		MOBDB_FLAGS_GET_T(temp->flags) != MOBDB_FLAGS_GET_T(box->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point and the box must be of the same dimensionality")));
}

void
ensure_same_dimensionality_tpoint_gs(const Temporal *temp, const GSERIALIZED *gs)
{
	if (MOBDB_FLAGS_GET_Z(temp->flags) != FLAGS_GET_Z(gs->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point and the geometry must be of the same dimensionality")));
}

void
ensure_common_dimension_stbox(const STBOX *box1, const STBOX *box2)
{
	if (MOBDB_FLAGS_GET_X(box1->flags) != MOBDB_FLAGS_GET_X(box2->flags) &&
		MOBDB_FLAGS_GET_T(box1->flags) != MOBDB_FLAGS_GET_T(box2->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The boxes must have at least one common dimension")));
}

void
ensure_has_X_stbox(const STBOX *box)
{
	if (! MOBDB_FLAGS_GET_X(box->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The box must have XY dimension")));
}

void
ensure_has_Z_stbox(const STBOX *box)
{
	if (! MOBDB_FLAGS_GET_Z(box->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The box must have Z dimension")));
}

void
ensure_has_not_Z_stbox(const STBOX *box)
{
	if (MOBDB_FLAGS_GET_Z(box->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The box cannot have Z dimension")));
}

void
ensure_has_T_stbox(const STBOX *box)
{
	if (! MOBDB_FLAGS_GET_T(box->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The box must have time dimension")));
}

void
ensure_has_Z_tpoint(const Temporal *temp)
{
	if (! MOBDB_FLAGS_GET_Z(temp->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point must have Z dimension")));
}

void
ensure_has_not_Z_tpoint(const Temporal *temp)
{
	if (MOBDB_FLAGS_GET_Z(temp->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("The temporal point cannot have Z dimension")));
}

void
ensure_has_Z_gs(const GSERIALIZED *gs)
{
	if (! FLAGS_GET_Z(gs->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Only geometries with Z dimension accepted")));
}

void
ensure_has_not_Z_gs(const GSERIALIZED *gs)
{
	if (FLAGS_GET_Z(gs->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Only geometries without Z dimension accepted")));
}

void
ensure_has_M_gs(const GSERIALIZED *gs)
{
	if (! FLAGS_GET_M(gs->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Only geometries with M dimension accepted")));
}

void
ensure_has_not_M_gs(const GSERIALIZED *gs)
{
	if (FLAGS_GET_M(gs->flags))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Only geometries without M dimension accepted")));
}

void
ensure_point_type(const GSERIALIZED *gs)
{
	if (gserialized_get_type(gs) != POINTTYPE)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Only point geometries accepted")));
}

void
ensure_non_empty(const GSERIALIZED *gs)
{
	if (gserialized_is_empty(gs))
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("Only non-empty geometries accepted")));
}

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

/*
 * Manipulate a geometry point directly from the GSERIALIZED.
 * These functions consitutute a SERIOUS break of encapsulation but it is the
 * only way to achieve reasonable performance when manipulating mobility data.
 * The datum_* functions suppose that the GSERIALIZED has been already
 * detoasted. This is typically the case when the datum is within a Temporal*
 * that has been already detoasted with PG_GETARG_TEMPORAL* 
 */

/* Get 2D point from a serialized geometry */

POINT2D
gs_get_point2d(GSERIALIZED *gs)
{
	POINT2D *point = (POINT2D *)((uint8_t*)gs->data + 8);
	return *point;
}

/* Get 2D point from a datum */
POINT2D
datum_get_point2d(Datum geom)
{
	GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(geom);
	POINT2D *point = (POINT2D *)((uint8_t*)gs->data + 8);
	return *point;
}

/* Get 3DZ point from a serialized geometry */

POINT3DZ
gs_get_point3dz(GSERIALIZED *gs)
{
	POINT3DZ *point = (POINT3DZ *)((uint8_t*)gs->data + 8);
	return *point;
}

/* Get 3DZ point from a datum */

POINT3DZ
datum_get_point3dz(Datum geom)
{
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(geom);
	POINT3DZ *point = (POINT3DZ *)((uint8_t*)gs->data + 8);
	return *point;
}

/* Get 4D point from a datum */

POINT4D
datum_get_point4d(Datum geom)
{
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(geom);
	POINT4D *point = (POINT4D *)((uint8_t*)gs->data + 8);
	return *point;
}

/* Compare two points from serialized geometries */

bool
datum_point_eq(Datum geopoint1, Datum geopoint2)
{
	GSERIALIZED *gs1 = (GSERIALIZED *) DatumGetPointer(geopoint1);
	GSERIALIZED *gs2 = (GSERIALIZED *) DatumGetPointer(geopoint2);
	assert(gserialized_get_srid(gs1) == gserialized_get_srid(gs2) &&
		FLAGS_GET_Z(gs1->flags) == FLAGS_GET_Z(gs2->flags) &&
		FLAGS_GET_GEODETIC(gs1->flags) == FLAGS_GET_GEODETIC(gs2->flags));
	if (FLAGS_GET_Z(gs1->flags))
	{
		POINT3DZ point1 = gs_get_point3dz(gs1);
		POINT3DZ point2 = gs_get_point3dz(gs2);
		return point1.x == point2.x && point1.y == point2.y &&
			point1.z == point2.z;
	}
	else
	{
		POINT2D point1 = gs_get_point2d(gs1);
		POINT2D point2 = gs_get_point2d(gs2);
		return point1.x == point2.x && point1.y == point2.y;
	}
}

Datum
datum2_point_eq(Datum geopoint1, Datum geopoint2)
{
	return BoolGetDatum(datum_point_eq(geopoint1, geopoint2));
}

Datum
datum2_point_ne(Datum geopoint1, Datum geopoint2)
{
	return BoolGetDatum(! datum_point_eq(geopoint1, geopoint2));
}

static Datum
datum_set_precision(Datum value, Datum size)
{
	GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(value);
	int srid = gserialized_get_srid(gs);
	LWPOINT *lwpoint;
	if (FLAGS_GET_Z(gs->flags))
	{
		POINT3DZ point = gs_get_point3dz(gs);
		double x = DatumGetFloat8(datum_round(Float8GetDatum(point.x), size));
		double y = DatumGetFloat8(datum_round(Float8GetDatum(point.y), size));
		double z = DatumGetFloat8(datum_round(Float8GetDatum(point.z), size));
		lwpoint = lwpoint_make3dz(srid, x, y, z);
	}
	else
	{
		POINT2D point = gs_get_point2d(gs);
		double x = DatumGetFloat8(datum_round(Float8GetDatum(point.x), size));
		double y = DatumGetFloat8(datum_round(Float8GetDatum(point.y), size));
		lwpoint = lwpoint_make2d(srid, x, y);
	}
	GSERIALIZED *result = geometry_serialize((LWGEOM *) lwpoint);
	result->flags = gs->flags;
	pfree(lwpoint);
	return PointerGetDatum(result);
}

/* Serialize a geometry */

GSERIALIZED *
geometry_serialize(LWGEOM *geom)
{
	size_t size;
	GSERIALIZED *result = gserialized_from_lwgeom(geom, &size);
	SET_VARSIZE(result, size);
	return result;
}

/* Call to PostGIS external functions */

static Datum
datum_transform(Datum value, Datum srid)
{
	return call_function2(transform, value, srid);
}

static Datum
geog_to_geom(Datum value)
{
	return call_function1(geometry_from_geography, value);
}

static Datum
geom_to_geog(Datum value)
{
	return call_function1(geography_from_geometry, value);
}

/*****************************************************************************
 * Trajectory functions.
 *****************************************************************************/

/*
 * Assemble the set of points of a temporal instant point as a single
 * geometry/geography. Duplicate points are removed.
 */

static Datum
tgeompointi_trajectory(const TemporalI *ti)
{
	/* Singleton instant set */
	if (ti->count == 1)
		return temporalinst_value_copy(temporali_inst_n(ti, 0));

	LWPOINT **points = palloc(sizeof(LWPOINT *) * ti->count);
	/* Remove all duplicate points */
	TemporalInst *inst = temporali_inst_n(ti, 0);
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	LWPOINT *value = lwgeom_as_lwpoint(lwgeom_from_gserialized(gs));
	points[0] = value;
	int k = 1;
	for (int i = 1; i < ti->count; i++)
	{
		inst = temporali_inst_n(ti, i);
		gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
		value = lwgeom_as_lwpoint(lwgeom_from_gserialized(gs));
		bool found = false;
		for (int j = 0; j < k; j++)
		{
			if (lwpoint_same(value, points[j]) == LW_TRUE)
			{
				found = true;
				break;
			}
		}
		if (!found)
			points[k++] = value;
	}
	LWGEOM *lwresult;
	if (k == 1)
	{
		lwresult = (LWGEOM *) points[0];
	}
	else
	{
		lwresult = (LWGEOM *) lwcollection_construct(MULTIPOINTTYPE,
			points[0]->srid, NULL, (uint32_t) k, (LWGEOM **) points);
		for (int i = 0; i < k; i++)
			lwpoint_free(points[i]);
	}
	Datum result = PointerGetDatum(geometry_serialize(lwresult));
	pfree(points);
	return result;
}

Datum
tgeogpointi_trajectory(const TemporalI *ti)
{
	TemporalI *tigeom = tfunc1_temporali(ti, &geog_to_geom,
		type_oid(T_GEOMETRY));
	Datum geomtraj = tgeompointi_trajectory(tigeom);
	Datum result = call_function1(geography_from_geometry, geomtraj);
	pfree(DatumGetPointer(geomtraj));
	return result;
}

Datum
tpointi_trajectory(const TemporalI *ti)
{
	Datum result;
	ensure_point_base_type(ti->valuetypid);
	if (ti->valuetypid == type_oid(T_GEOMETRY))
		result = tgeompointi_trajectory(ti);
	else
		result = tgeogpointi_trajectory(ti);
	return result;
}

/*****************************************************************************/

/* Compute the trajectory from the points of two consecutive instants with
 * linear interpolation. The functions are called during normalization for
 * determining whether three consecutive points are collinear, for computing
 * the temporal distance, the temporal spatial relationships, etc. */

Datum
geompoint_trajectory(Datum value1, Datum value2)
{
	GSERIALIZED *gs1 = (GSERIALIZED *)DatumGetPointer(value1);
	GSERIALIZED *gs2 = (GSERIALIZED *)DatumGetPointer(value2);
	LWGEOM *geoms[2];
	geoms[0] = lwgeom_from_gserialized(gs1);
	geoms[1] = lwgeom_from_gserialized(gs2);
	LWGEOM *traj = (LWGEOM *)lwline_from_lwgeom_array(geoms[0]->srid, 2, geoms);
	GSERIALIZED *result = geometry_serialize(traj);
	lwgeom_free(geoms[0]); lwgeom_free(geoms[1]); lwgeom_free(traj);
	return PointerGetDatum(result);
}

Datum
geogpoint_trajectory(Datum value1, Datum value2)
{
	Datum geom1 = call_function1(geometry_from_geography, value1);
	Datum geom2 = call_function1(geometry_from_geography, value2);
	Datum geom = geompoint_trajectory(geom1, geom2);
	Datum result = call_function1(geography_from_geometry, geom);
	pfree(DatumGetPointer(geom1)); pfree(DatumGetPointer(geom2));
	pfree(DatumGetPointer(geom));
	return result;
}

LWLINE *
geompoint_trajectory_lwline(Datum value1, Datum value2)
{
	GSERIALIZED *gs1 = (GSERIALIZED *)DatumGetPointer(value1);
	GSERIALIZED *gs2 = (GSERIALIZED *)DatumGetPointer(value2);
	LWGEOM *geoms[2];
	geoms[0] = lwgeom_from_gserialized(gs1);
	geoms[1] = lwgeom_from_gserialized(gs2);
	LWLINE *result = lwline_from_lwgeom_array(geoms[0]->srid, 2, geoms);
	lwgeom_free(geoms[0]); lwgeom_free(geoms[1]);
	return result;
}

/*****************************************************************************/

/* Compute a trajectory from a set of points. The result is either a line or a
 * multipoint depending on whether the interpolation is step or linear */

static Datum
lwpointarr_make_trajectory(LWGEOM **lwpoints, int count, bool linear)
{
	LWGEOM *lwgeom = linear ?
		(LWGEOM *) lwline_from_lwgeom_array(lwpoints[0]->srid,
			(uint32_t) count, lwpoints) :
		(LWGEOM *) lwcollection_construct(MULTIPOINTTYPE, lwpoints[0]->srid,
			NULL, (uint32_t) count, lwpoints);
	Datum result = PointerGetDatum(geometry_serialize(lwgeom));
	pfree(lwgeom);
	return result;
}

static Datum
pointarr_make_trajectory(const Datum *points, int count, bool linear)
{
	LWGEOM **lwpoints = palloc(sizeof(LWGEOM *) * count);
	for (int i = 0; i < count; i++)
	{
		GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(points[i]);
		lwpoints[i] = lwgeom_from_gserialized(gs);
	}
	Datum result = lwpointarr_make_trajectory(lwpoints, count, linear);
	for (int i = 0; i < count; i++)
		lwgeom_free(lwpoints[i]);
	pfree(lwpoints);
	return result;
}

/* Compute the trajectory of an array of instants.
 * This function is called by the constructor of a temporal sequence and
 * returns a single Datum which is a geometry */
Datum
tpointseq_make_trajectory(TemporalInst **instants, int count, bool linear)
{
	Oid valuetypid = instants[0]->valuetypid;
	ensure_point_base_type(valuetypid);
	bool geometry = (valuetypid == type_oid(T_GEOMETRY));
	LWPOINT **points = palloc(sizeof(LWPOINT *) * count);
	LWPOINT *lwpoint;
	Datum value;
	GSERIALIZED *gsvalue;
	int k;
	if (linear)
	{
		/* Remove two consecutive points if they are equal */
		value = geometry ? temporalinst_value(instants[0]) :
			call_function1(geometry_from_geography, temporalinst_value(instants[0]));
		gsvalue = (GSERIALIZED *) DatumGetPointer(value);
		points[0] = lwgeom_as_lwpoint(lwgeom_from_gserialized(gsvalue));
		k = 1;
		for (int i = 1; i < count; i++)
		{
			value = geometry ? temporalinst_value(instants[i]) :
				call_function1(geometry_from_geography, temporalinst_value(instants[i]));
			gsvalue = (GSERIALIZED *) DatumGetPointer(value);
			lwpoint = lwgeom_as_lwpoint(lwgeom_from_gserialized(gsvalue));
			if (! lwpoint_same(lwpoint, points[k - 1]))
				points[k++] = lwpoint;
		}
	}
	else
	{
		 /* Remove all duplicate points */
		k = 0;
		for (int i = 0; i < count; i++)
		{
			value = geometry ? temporalinst_value(instants[i]) :
				call_function1(geometry_from_geography, temporalinst_value(instants[i]));
			gsvalue = (GSERIALIZED *) DatumGetPointer(value);
			lwpoint = lwgeom_as_lwpoint(lwgeom_from_gserialized(gsvalue));
			bool found = false;
			for (int j = 0; j < k; j++)
			{
				if (lwpoint_same(lwpoint, points[j]) == LW_TRUE)
				{
					found = true;
					break;
				}
			}
			if (!found)
				points[k++] = lwpoint;
		}
	}
	Datum geomresult = (k == 1) ?
		PointerGetDatum(geometry_serialize((LWGEOM *)points[0])) :
		lwpointarr_make_trajectory((LWGEOM **)points, k, linear);
	Datum result = (geometry) ? geomresult :
		call_function1(geography_from_geometry, geomresult);
	for (int i = 0; i < k; i++)
		lwpoint_free(points[i]);
	pfree(points);
	if (! geometry)
		pfree(DatumGetPointer(geomresult));
	return result;
}

/* Get the precomputed trajectory of a tpointseq */

Datum
tpointseq_trajectory(const TemporalSeq *seq)
{
	void *traj = (char *)(&seq->offsets[seq->count + 2]) + 	/* start of data */
		seq->offsets[seq->count + 1];						/* offset */
	return PointerGetDatum(traj);
}

/* Add or replace a point to the trajectory of a sequence */

Datum
tpointseq_trajectory_append(const TemporalSeq *seq, const TemporalInst *inst,
	bool replace)
{
	Datum traj = tpointseq_trajectory(seq);
	Datum point = temporalinst_value(inst);
	GSERIALIZED *gstraj = (GSERIALIZED *) DatumGetPointer(traj);
	if (gserialized_get_type(gstraj) == POINTTYPE)
	{
		if (datum_point_eq(traj, point))
			return PointerGetDatum(gserialized_copy(gstraj));
		else
		{
			if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
				return geompoint_trajectory(traj, point);
			else
			{
				Datum points[2];
				points[0] = traj;
				points[1] = point;
				return pointarr_make_trajectory(points, 2, false);
			}
		}
	}
	else if (gserialized_get_type(gstraj) == MULTIPOINTTYPE)
	{
		int count = replace ? seq->count : seq->count + 1;
		Datum *points = palloc(sizeof(Datum) * count);
		 /* Remove all duplicate points */
		int k = 0;
		bool foundpoint = false;
		for (int i = 0; i < count - 1; i++)
		{
			Datum value = temporalinst_value(temporalseq_inst_n(seq, i));
			bool found = false;
			for (int j = 0; j < k; j++)
			{
				if (datum_point_eq(value, points[j]))
				{
					found = true;
					break;
				}
			}
			if (!found)
				points[k++] = value;
			if (!foundpoint && datum_point_eq(value, point))
				foundpoint = true;
		}
		if (!foundpoint)
			points[k++] = point;
		Datum result = pointarr_make_trajectory(points, k, false);
		pfree(points);
		return result;
	}
	/* The trajectory is a Linestring */
	else
	{
		if (replace)
			return call_function3(LWGEOM_setpoint_linestring, traj,
				Int32GetDatum(-1), point);
		else
			return call_function2(LWGEOM_addpoint, traj, point);
	}
}

/* Join two trajectories */

Datum
tpointseq_trajectory_join(const TemporalSeq *seq1, const TemporalSeq *seq2, bool last, bool first)
{
	assert(MOBDB_FLAGS_GET_LINEAR(seq1->flags) == MOBDB_FLAGS_GET_LINEAR(seq2->flags));
	int count1 = last ? seq1->count - 1 : seq1->count;
	int start2 = first ? 1 : 0;
	TemporalInst **instants = palloc(sizeof(TemporalInst *) *
		(count1 + seq2->count - start2));
	int k = 0;
	for (int i = 0; i < count1; i++)
		instants[k++] = temporalseq_inst_n(seq1, i);
	for (int i = start2; i < seq2->count; i++)
		instants[k++] = temporalseq_inst_n(seq2, i);
	Datum traj = tpointseq_make_trajectory(instants, k, MOBDB_FLAGS_GET_LINEAR(seq1->flags));
	pfree(instants);

	return traj;
}

/* Copy the precomputed trajectory of a tpointseq */

Datum
tpointseq_trajectory_copy(const TemporalSeq *seq)
{
	void *traj = (char *)(&seq->offsets[seq->count + 2]) + 	/* start of data */
			seq->offsets[seq->count + 1];					/* offset */
	return PointerGetDatum(gserialized_copy(traj));
}

/*****************************************************************************/

/* Compute the trajectory of a tpoints from the precomputed trajectories
   of its composing segments. The resulting trajectory must be freed by the
   calling function. The function removes duplicates points */

static Datum
tgeompoints_trajectory(const TemporalS *ts)
{
	/* Singleton sequence set */
	if (ts->count == 1)
		return tpointseq_trajectory_copy(temporals_seq_n(ts, 0));

	LWPOINT **points = palloc(sizeof(LWPOINT *) * ts->totalcount);
	LWGEOM **geoms = palloc(sizeof(LWGEOM *) * ts->count);
	int k = 0, l = 0;
	for (int i = 0; i < ts->count; i++)
	{
		Datum traj = tpointseq_trajectory(temporals_seq_n(ts, i));
		GSERIALIZED *gstraj = (GSERIALIZED *)DatumGetPointer(traj);
		LWPOINT *lwpoint;
		if (gserialized_get_type(gstraj) == POINTTYPE)
		{
			lwpoint = lwgeom_as_lwpoint(lwgeom_from_gserialized(gstraj));
			bool found = false;
			for (int j = 0; j < l; j++)
			{
				if (lwpoint_same(lwpoint, points[j]) == LW_TRUE)
				{
					found = true;
					break;
				}
			}
			if (!found)
				points[l++] = lwpoint;
		}
		else if (gserialized_get_type(gstraj) == MULTIPOINTTYPE)
		{
			LWMPOINT *lwmpoint = lwgeom_as_lwmpoint(lwgeom_from_gserialized(gstraj));
			int count = lwmpoint->ngeoms;
			for (int m = 0; m < count; m++)
			{
				lwpoint = lwmpoint->geoms[m];
				bool found = false;
				for (int j = 0; j < l; j++)
				{
					if (lwpoint_same(lwpoint, points[j]) == LW_TRUE)
						{
							found = true;
							break;
						}
				}
				if (!found)
					points[l++] = lwpoint;
			}
		}
		/* gserialized_get_type(gstraj) == LINETYPE */
		else
		{
			geoms[k++] = lwgeom_from_gserialized(gstraj);
		}
	}
	Datum result;
	if (k == 0)
	{
		/* Only points */
		if (l == 1)
			result = PointerGetDatum(geometry_serialize((LWGEOM *)points[0]));
		else
			result = lwpointarr_make_trajectory((LWGEOM **)points, l, false);
	}
	else if (l == 0)
	{
		/* Only lines */
		/* k > 1 since otherwise it is a singleton sequence set and this case
		 * was taken care at the begining of the function */
		// TODO add the bounding box instead of ask PostGIS to compute it again
		// GBOX *box = stbox_to_gbox(temporalseq_bbox_ptr(seq));
		LWGEOM *coll = (LWGEOM *) lwcollection_construct(MULTILINETYPE,
			geoms[0]->srid, NULL, (uint32_t) k, geoms);
		result = PointerGetDatum(geometry_serialize(coll));
		/* We cannot lwgeom_free(geoms[i] or lwgeom_free(coll) */
	}
	else
	{
		/* Both points and lines */
		if (l == 1)
			geoms[k++] = (LWGEOM *)points[0];
		else
		{
			geoms[k++] = (LWGEOM *) lwcollection_construct(MULTIPOINTTYPE,
				points[0]->srid, NULL, (uint32_t) l, (LWGEOM **) points);
			for (int i = 0; i < l; i++)
				lwpoint_free(points[i]);
		}
		// TODO add the bounding box instead of ask PostGIS to compute it again
		// GBOX *box = stbox_to_gbox(temporalseq_bbox_ptr(seq));
		LWGEOM *coll = (LWGEOM *) lwcollection_construct(COLLECTIONTYPE,
			geoms[0]->srid, NULL, (uint32_t) k, geoms);
		result = PointerGetDatum(geometry_serialize(coll));
	}
	pfree(points); pfree(geoms);
	return result;
}

static Datum
tgeogpoints_trajectory(const TemporalS *ts)
{
	TemporalS *tsgeom = tfunc1_temporals(ts, &geog_to_geom,
		type_oid(T_GEOMETRY));
	Datum geomtraj = tgeompoints_trajectory(tsgeom);
	Datum result = call_function1(geography_from_geometry, geomtraj);
	pfree(DatumGetPointer(geomtraj));
	return result;
}

Datum
tpoints_trajectory(TemporalS *ts)
{
	Datum result;
	ensure_point_base_type(ts->valuetypid);
	if (ts->valuetypid == type_oid(T_GEOMETRY))
		result = tgeompoints_trajectory(ts);
	else
		result = tgeogpoints_trajectory(ts);
	return result;
}

/*****************************************************************************/

Datum
tpoint_trajectory_internal(const Temporal *temp)
{
	Datum result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = temporalinst_value_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tpointi_trajectory((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tpointseq_trajectory_copy((TemporalSeq *)temp);
	else /* temp->duration == TEMPORALS */
		result = tpoints_trajectory((TemporalS *)temp);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_trajectory);

PGDLLEXPORT Datum
tpoint_trajectory(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result = tpoint_trajectory_internal(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Functions for spatial reference systems
 *****************************************************************************/

/* Get the spatial reference system identifier (SRID) of a temporal point */

int
tpointinst_srid(const TemporalInst *inst)
{
	GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(temporalinst_value_ptr(inst));
	return gserialized_get_srid(gs);
}

int
tpointi_srid(const TemporalI *ti)
{
	STBOX *box = temporali_bbox_ptr(ti);
	return box->srid;
}

int
tpointseq_srid(const TemporalSeq *seq)
{
	STBOX *box = temporalseq_bbox_ptr(seq);
	return box->srid;
}

int
tpoints_srid(const TemporalS *ts)
{
	STBOX *box = temporals_bbox_ptr(ts);
	return box->srid;
}

int
tpoint_srid_internal(const Temporal *temp)
{
	int result;
	ensure_valid_duration(temp->duration);
	ensure_point_base_type(temp->valuetypid);
	if (temp->duration == TEMPORALINST)
		result = tpointinst_srid((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tpointi_srid((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tpointseq_srid((TemporalSeq *)temp);
	else /* temp->duration == TEMPORALS */
		result = tpoints_srid((TemporalS *)temp);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_srid);

PGDLLEXPORT Datum
tpoint_srid(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	int result = tpoint_srid_internal(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_INT32(result);
}

/*****************************************************************************/

/* Set the spatial reference system identifier (SRID) of a temporal point */

/* TemporalInst */

static TemporalInst *
tpointinst_set_srid(TemporalInst *inst, int32 srid)
{
	TemporalInst *result = temporalinst_copy(inst);
	GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(temporalinst_value_ptr(result));
	gserialized_set_srid(gs, srid);
	return result;
}

static TemporalI *
tpointi_set_srid(TemporalI *ti, int32 srid)
{
	TemporalI *result = temporali_copy(ti);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(result, i);
		GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(temporalinst_value_ptr(inst));
		gserialized_set_srid(gs, srid);
	}
	STBOX *box = temporali_bbox_ptr(result);
	box->srid = srid;
	return result;
}

static TemporalSeq *
tpointseq_set_srid(TemporalSeq *seq, int32 srid)
{
	TemporalSeq *result = temporalseq_copy(seq);
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(result, i);
		GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(temporalinst_value_ptr(inst));
		gserialized_set_srid(gs, srid);
	}
	STBOX *box = temporalseq_bbox_ptr(result);
	box->srid = srid;
	return result;
}

static TemporalS *
tpoints_set_srid(TemporalS *ts, int32 srid)
{
	TemporalS *result = temporals_copy(ts);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(result, i);
		for (int j = 0; j < seq->count; j++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, j);
			GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(temporalinst_value_ptr(inst));
			gserialized_set_srid(gs, srid);
		}
	}
	STBOX *box = temporals_bbox_ptr(result);
	box->srid = srid;
	return result;
}

Temporal *
tpoint_set_srid_internal(Temporal *temp, int32 srid)
{
	Temporal *result;
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tpointinst_set_srid((TemporalInst *)temp, srid);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tpointi_set_srid((TemporalI *)temp, srid);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tpointseq_set_srid((TemporalSeq *)temp, srid);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)tpoints_set_srid((TemporalS *)temp, srid);

	assert(result != NULL);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_set_srid);

PGDLLEXPORT Datum
tpoint_set_srid(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	int32 srid = PG_GETARG_INT32(1);
	Temporal *result = tpoint_set_srid_internal(temp, srid);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Transform a temporal geometry point into another spatial reference system */

TemporalInst *
tpointinst_transform(const TemporalInst *inst, Datum srid)
{
	Datum geo = datum_transform(temporalinst_value(inst), srid);
	TemporalInst *result = temporalinst_make(geo, inst->t, inst->valuetypid);
	pfree(DatumGetPointer(geo));
	return result;
}

static TemporalI *
tpointi_transform(const TemporalI *ti, Datum srid)
{
	/* Singleton sequence */
	if (ti->count == 1)
	{
		TemporalInst *inst = tpointinst_transform(temporali_inst_n(ti, 0), srid);
		TemporalI *result = temporali_make(&inst, 1);
		pfree(inst);
		return result;
	}

	LWPOINT **points = palloc(sizeof(LWPOINT *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		Datum value = temporalinst_value(temporali_inst_n(ti, i));
		GSERIALIZED *gsvalue = (GSERIALIZED *)DatumGetPointer(value);
		points[i] = lwgeom_as_lwpoint(lwgeom_from_gserialized(gsvalue));
	}
	Datum multipoint = lwpointarr_make_trajectory((LWGEOM **)points, ti->count, false);
	Datum transf = call_function2(transform, multipoint, srid);
	GSERIALIZED *gs = (GSERIALIZED *) PG_DETOAST_DATUM(transf);
	LWMPOINT *lwmpoint = lwgeom_as_lwmpoint(lwgeom_from_gserialized(gs));
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		Datum point = PointerGetDatum(geometry_serialize((LWGEOM *) (lwmpoint->geoms[i])));
		TemporalInst *inst = temporali_inst_n(ti, i);
		instants[i] = temporalinst_make(point, inst->t, inst->valuetypid);
		pfree(DatumGetPointer(point));
	}
	TemporalI *result = temporali_make(instants, ti->count);
	for (int i = 0; i < ti->count; i++)
		lwpoint_free(points[i]);
	pfree(points);
	pfree(DatumGetPointer(multipoint)); pfree(DatumGetPointer(transf));
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(gs));
	lwmpoint_free(lwmpoint);
	for (int i = 0; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalSeq *
tpointseq_transform(const TemporalSeq *seq, Datum srid)
{
	/* Singleton sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = tpointinst_transform(temporalseq_inst_n(seq, 0), srid);
		TemporalSeq *result = temporalseq_make(&inst, 1, true, true,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
		pfree(inst);
		return result;
	}

	LWPOINT **points = palloc(sizeof(LWPOINT *) * seq->count);
	for (int i = 0; i < seq->count; i++)
	{
		Datum value = temporalinst_value(temporalseq_inst_n(seq, i));
		GSERIALIZED *gsvalue = (GSERIALIZED *)DatumGetPointer(value);
		points[i] = lwgeom_as_lwpoint(lwgeom_from_gserialized(gsvalue));
	}
	Datum multipoint = lwpointarr_make_trajectory((LWGEOM **)points, seq->count, false);
	Datum transf = call_function2(transform, multipoint, srid);
	GSERIALIZED *gs = (GSERIALIZED *) PG_DETOAST_DATUM(transf);
	LWMPOINT *lwmpoint = lwgeom_as_lwmpoint(lwgeom_from_gserialized(gs));
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	for (int i = 0; i < seq->count; i++)
	{
		Datum point = PointerGetDatum(geometry_serialize((LWGEOM *) (lwmpoint->geoms[i])));
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		instants[i] = temporalinst_make(point, inst->t, inst->valuetypid);
		pfree(DatumGetPointer(point));
	}
	TemporalSeq *result = temporalseq_make(instants, seq->count,
		seq->period.lower_inc, seq->period.upper_inc,
		MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
	for (int i = 0; i < seq->count; i++)
		lwpoint_free(points[i]);
	pfree(points);
	pfree(DatumGetPointer(multipoint)); pfree(DatumGetPointer(transf));
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(gs));
	lwmpoint_free(lwmpoint);
	for (int i = 0; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalS *
tpoints_transform(const TemporalS *ts, Datum srid)
{
	/* Singleton sequence set */
	if (ts->count == 1)
	{
		TemporalSeq *seq = tpointseq_transform(temporals_seq_n(ts, 0), srid);
		TemporalS *result = temporalseq_to_temporals(seq);
		pfree(seq);
		return result;
	}
	int k = 0;
	LWPOINT **points = palloc(sizeof(LWPOINT *) * ts->totalcount);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		for (int j = 0; j < seq->count; j++)
		{
			Datum value = temporalinst_value(temporalseq_inst_n(seq, j));
			GSERIALIZED *gsvalue = (GSERIALIZED *)DatumGetPointer(value);
			points[k++] = lwgeom_as_lwpoint(lwgeom_from_gserialized(gsvalue));
		}
	}
	Datum multipoint = lwpointarr_make_trajectory((LWGEOM **)points, ts->totalcount, false);
	Datum transf = call_function2(transform, multipoint, srid);
	GSERIALIZED *gs = (GSERIALIZED *) PG_DETOAST_DATUM(transf);
	LWMPOINT *lwmpoint = lwgeom_as_lwmpoint(lwgeom_from_gserialized(gs));
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
		for (int j = 0; j < seq->count; j++)
		{
			Datum point = PointerGetDatum(geometry_serialize((LWGEOM *) (lwmpoint->geoms[k])));
			TemporalInst *inst = temporalseq_inst_n(seq, j);
			instants[j] = temporalinst_make(point, inst->t, inst->valuetypid);
			pfree(DatumGetPointer(point));
		}
		sequences[i] = temporalseq_make(instants, seq->count,
			seq->period.lower_inc, seq->period.upper_inc,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
		for (int j = 0; j < seq->count; j++)
			pfree(instants[j]);
		pfree(instants);
	}
	TemporalS *result = temporals_make(sequences, ts->count, false);
	for (int i = 0; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	for (int i = 0; i < ts->totalcount; i++)
		lwpoint_free(points[i]);
	pfree(points);
	pfree(DatumGetPointer(multipoint)); pfree(DatumGetPointer(transf));
	POSTGIS_FREE_IF_COPY_P(gs, DatumGetPointer(gs));
	lwmpoint_free(lwmpoint);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_transform);

PGDLLEXPORT Datum
tpoint_transform(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum srid = PG_GETARG_DATUM(1);

	Temporal *result;
	ensure_valid_duration(temp->duration);
	ensure_point_base_type(temp->valuetypid);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *) tpointinst_transform((TemporalInst *)temp, srid);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *) tpointi_transform((TemporalI *)temp, srid);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *) tpointseq_transform((TemporalSeq *)temp, srid);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *) tpoints_transform((TemporalS *)temp, srid);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Cast functions
 * Notice that a geometry point and a geography point are of different size
 * since the geography point keeps a bounding box
 *****************************************************************************/

/* Geometry to Geography */

PG_FUNCTION_INFO_V1(tgeompoint_to_tgeogpoint);

PGDLLEXPORT Datum
tgeompoint_to_tgeogpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = tfunc1_temporal(temp, &geom_to_geog,
		type_oid(T_GEOGRAPHY));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Geography to Geometry */

TemporalInst *
tgeogpointinst_to_tgeompointinst(const TemporalInst *inst)
{
	return tfunc1_temporalinst(inst, &geog_to_geom, type_oid(T_GEOMETRY));
}

TemporalSeq *
tgeogpointseq_to_tgeompointseq(const TemporalSeq *seq)
{
	return tfunc1_temporalseq(seq, &geog_to_geom, type_oid(T_GEOMETRY));
}

TemporalS *
tgeogpoints_to_tgeompoints(const TemporalS *ts)
{
	return tfunc1_temporals(ts, &geog_to_geom, type_oid(T_GEOMETRY));
}

PG_FUNCTION_INFO_V1(tgeogpoint_to_tgeompoint);

PGDLLEXPORT Datum
tgeogpoint_to_tgeompoint(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = tfunc1_temporal(temp, &geog_to_geom,
		type_oid(T_GEOMETRY));
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Set precision of the coordinates.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(tpoint_set_precision);

PGDLLEXPORT Datum
tpoint_set_precision(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum size = PG_GETARG_DATUM(1);
	Temporal *result = tfunc2_temporal(temp, size, &datum_set_precision,
		temp->valuetypid);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Length functions
 *****************************************************************************/

/* Length traversed by the temporal point */

static double
tpointseq_length(const TemporalSeq *seq)
{
	assert(MOBDB_FLAGS_GET_LINEAR(seq->flags));
	Datum traj = tpointseq_trajectory(seq);
	GSERIALIZED *gstraj = (GSERIALIZED *)DatumGetPointer(traj);
	if (gserialized_get_type(gstraj) == POINTTYPE)
		return 0;
	
	/* We are sure that the trajectory is a line */
	double result = 0.0;
	ensure_point_base_type(seq->valuetypid);
	if (seq->valuetypid == type_oid(T_GEOMETRY))
		/* The next function call works for 2D and 3D */
		result = DatumGetFloat8(call_function1(LWGEOM_length_linestring, traj));
	else
		result = DatumGetFloat8(call_function2(geography_length, traj,
			BoolGetDatum(true)));
	return result;
}

static double
tpoints_length(const TemporalS *ts)
{
	assert(MOBDB_FLAGS_GET_LINEAR(ts->flags));
	double result = 0;
	for (int i = 0; i < ts->count; i++)
		result += tpointseq_length(temporals_seq_n(ts, i));
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_length);

PGDLLEXPORT Datum
tpoint_length(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	double result = 0.0;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI ||
		! MOBDB_FLAGS_GET_LINEAR(temp->flags))
		;
	else if (temp->duration == TEMPORALSEQ)
		result = tpointseq_length((TemporalSeq *)temp);	
	else /* temp->duration == TEMPORALS */
		result = tpoints_length((TemporalS *)temp);	
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_FLOAT8(result);
}

/*****************************************************************************/

/* Cumulative length traversed by the temporal point */

static TemporalInst *
tpointinst_cumulative_length(const TemporalInst *inst)
{
	return temporalinst_make(Float8GetDatum(0.0), inst->t, FLOAT8OID);
}

static TemporalI *
tpointi_cumulative_length(const TemporalI *ti)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	Datum length = Float8GetDatum(0.0);
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		instants[i] = temporalinst_make(length, inst->t, FLOAT8OID);
	}
	TemporalI *result = temporali_make(instants, ti->count);
	for (int i = 1; i < ti->count; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalSeq *
tpointseq_cumulative_length(const TemporalSeq *seq, double prevlength)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, 0);
		TemporalInst *inst1 = temporalinst_make(Float8GetDatum(0), inst->t,
			FLOAT8OID);
		TemporalSeq *result = temporalseq_make(&inst1, 1, true, true,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), false);
		pfree(inst1);
		return result;
	}

	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	/* Stepwise interpolation */
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		Datum length = Float8GetDatum(0.0);
		for (int i = 0; i < seq->count; i++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, i);
			instants[i] = temporalinst_make(length, inst->t, FLOAT8OID);
		}
	}
	else
	/* Linear interpolation */
	{
		Datum (*func)(Datum, Datum);
		ensure_point_base_type(seq->valuetypid);
		if (seq->valuetypid == type_oid(T_GEOMETRY))
			func = MOBDB_FLAGS_GET_Z(seq->flags) ? &pt_distance3d :
				&pt_distance2d;
		else
			func = &geog_distance;

		TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
		Datum value1 = temporalinst_value(inst1);
		double length = prevlength;
		instants[0] = temporalinst_make(Float8GetDatum(length), inst1->t,
				FLOAT8OID);
		for (int i = 1; i < seq->count; i++)
		{
			TemporalInst *inst2 = temporalseq_inst_n(seq, i);
			Datum value2 = temporalinst_value(inst2);
			if (datum_ne(value1, value2, inst1->valuetypid))
				length += DatumGetFloat8(func(value1, value2));
			instants[i] = temporalinst_make(Float8GetDatum(length), inst2->t,
				FLOAT8OID);
			inst1 = inst2;
			value1 = value2;
		}
	}
	TemporalSeq *result = temporalseq_make(instants, seq->count,
		seq->period.lower_inc, seq->period.upper_inc,
		MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
		
	for (int i = 1; i < seq->count; i++)
		pfree(instants[i]);
	pfree(instants);
	
	return result;
}

static TemporalS *
tpoints_cumulative_length(const TemporalS *ts)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	double length = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		sequences[i] = tpointseq_cumulative_length(seq, length);
		TemporalInst *end = temporalseq_inst_n(sequences[i], seq->count - 1);
		length += DatumGetFloat8(temporalinst_value(end));
	}
	TemporalS *result = temporals_make(sequences, ts->count, false);
		
	for (int i = 1; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

PG_FUNCTION_INFO_V1(tpoint_cumulative_length);

PGDLLEXPORT Datum
tpoint_cumulative_length(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tpointinst_cumulative_length((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tpointi_cumulative_length((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tpointseq_cumulative_length((TemporalSeq *)temp, 0);	
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)tpoints_cumulative_length((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Speed functions
 *****************************************************************************/

static double
tpointinst_speed(const TemporalInst *inst1, const TemporalInst *inst2,
	Datum (*func)(Datum, Datum))
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	return datum_point_eq(value1, value2) ? 0 :
		DatumGetFloat8(func(value1, value2)) / ((double)(inst2->t - inst1->t) / 1000000);
}

static TemporalSeq *
tpointseq_speed(const TemporalSeq *seq)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return NULL;
	
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	/* Stepwise interpolation */
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
	{
		Datum length = Float8GetDatum(0.0);
		for (int i = 0; i < seq->count; i++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, i);
			instants[i] = temporalinst_make(length, inst->t, FLOAT8OID);
		}
	}
	else
	/* Linear interpolation */
	{
		Datum (*func)(Datum, Datum);
		ensure_point_base_type(seq->valuetypid);
		if (seq->valuetypid == type_oid(T_GEOMETRY))
			func = MOBDB_FLAGS_GET_Z(seq->flags) ? &pt_distance3d :
				&pt_distance2d;
		else
			func = &geog_distance;

		TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
		Datum value1 = temporalinst_value(inst1);
		double speed;
		for (int i = 0; i < seq->count - 1; i++)
		{
			TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
			Datum value2 = temporalinst_value(inst2);
			if (datum_point_eq(value1, value2))
				speed = 0;
			else
				speed = DatumGetFloat8(func(value1, value2)) / ((double)(inst2->t - inst1->t) / 1000000);
			instants[i] = temporalinst_make(Float8GetDatum(speed), inst1->t,
				FLOAT8OID);
			inst1 = inst2;
			value1 = value2;
		}			
		instants[seq->count - 1] = temporalinst_make(Float8GetDatum(speed),
			seq->period.upper, FLOAT8OID);
	}
	/* The resulting sequence has step interpolation */
	TemporalSeq *result = temporalseq_make(instants, seq->count,
		seq->period.lower_inc, seq->period.upper_inc, false, true);
	for (int i = 0; i < seq->count - 1; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalS *
tpoints_speed(const TemporalS *ts)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		if (seq->count > 1)
			sequences[k++] = tpointseq_speed(seq);
	}
	if (k == 0)
	{
		pfree(sequences);
		return NULL;
	}
	/* The resulting sequence set has step interpolation */
	TemporalS *result = temporals_make(sequences, k, true);

	for (int i = 0; i < k; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

PG_FUNCTION_INFO_V1(tpoint_speed);

PGDLLEXPORT Datum
tpoint_speed(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI)
		;
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tpointseq_speed((TemporalSeq *)temp);	
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)tpoints_speed((TemporalS *)temp);	
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Time-weighed centroid for temporal geometry points
 *****************************************************************************/

Datum
tgeompointi_twcentroid(TemporalI *ti)
{
	int srid = tpointi_srid(ti);
	TemporalInst **instantsx = palloc(sizeof(TemporalInst *) * ti->count);
	TemporalInst **instantsy = palloc(sizeof(TemporalInst *) * ti->count);
	TemporalInst **instantsz = NULL; /* keep compiler quiet */
	bool hasz = MOBDB_FLAGS_GET_Z(ti->flags);
	if (hasz)
		instantsz = palloc(sizeof(TemporalInst *) * ti->count);
		
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		POINT4D point = datum_get_point4d(temporalinst_value(inst));
		instantsx[i] = temporalinst_make(Float8GetDatum(point.x), inst->t,
			FLOAT8OID);
		instantsy[i] = temporalinst_make(Float8GetDatum(point.y), inst->t,
			FLOAT8OID);
		if (hasz)
			instantsz[i] = temporalinst_make(Float8GetDatum(point.z), inst->t,
				FLOAT8OID);
	}
	TemporalI *tix = temporali_make(instantsx, ti->count);
	TemporalI *tiy = temporali_make(instantsy, ti->count);
	TemporalI *tiz = NULL; /* keep compiler quiet */
	if (hasz)
		tiz = temporali_make(instantsz, ti->count);
	double avgx = tnumberi_twavg(tix);
	double avgy = tnumberi_twavg(tiy);
	double avgz;
	if (hasz)
		avgz = tnumberi_twavg(tiz);
	LWPOINT *lwpoint;
	if (hasz)
		lwpoint = lwpoint_make3dz(srid, avgx, avgy, avgz);
	else
		lwpoint = lwpoint_make2d(srid, avgx, avgy);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *)lwpoint));

	pfree(lwpoint);
	for (int i = 0; i < ti->count; i++)
	{
		pfree(instantsx[i]);
		pfree(instantsy[i]);
		if (hasz)
			pfree(instantsz[i]);
	}
	pfree(instantsx); pfree(instantsy);
	pfree(tix); pfree(tiy);
	if (hasz)
	{
		pfree(instantsz); pfree(tiz);
	}

	return result;
}

Datum
tgeompointseq_twcentroid(TemporalSeq *seq)
{
	int srid = tpointseq_srid(seq);
	TemporalInst **instantsx = palloc(sizeof(TemporalInst *) * seq->count);
	TemporalInst **instantsy = palloc(sizeof(TemporalInst *) * seq->count);
	TemporalInst **instantsz;
	bool hasz = MOBDB_FLAGS_GET_Z(seq->flags);
	if (hasz)
		instantsz = palloc(sizeof(TemporalInst *) * seq->count);
		
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		POINT4D point = datum_get_point4d(temporalinst_value(inst));
		instantsx[i] = temporalinst_make(Float8GetDatum(point.x), inst->t,
			FLOAT8OID);
		instantsy[i] = temporalinst_make(Float8GetDatum(point.y), inst->t,
			FLOAT8OID);
		if (hasz)
			instantsz[i] = temporalinst_make(Float8GetDatum(point.z), inst->t,
				FLOAT8OID);
	}
	TemporalSeq *seqx = temporalseq_make(instantsx, seq->count,
		seq->period.lower_inc, seq->period.upper_inc,
		MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
	TemporalSeq *seqy = temporalseq_make(instantsy, seq->count,
		seq->period.lower_inc, seq->period.upper_inc,
		MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
	TemporalSeq *seqz;
	if (hasz)
		seqz = temporalseq_make(instantsz, seq->count, seq->period.lower_inc,
			seq->period.upper_inc, MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
	double twavgx = tnumberseq_twavg(seqx);
	double twavgy = tnumberseq_twavg(seqy);
	double twavgz;
	LWPOINT *lwpoint;
	if (hasz)
	{
		twavgz = tnumberseq_twavg(seqz);
		lwpoint = lwpoint_make3dz(srid, twavgx, twavgy, twavgz);
	}
	else
		lwpoint = lwpoint_make2d(srid, twavgx, twavgy);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *)lwpoint));

	pfree(lwpoint);
	for (int i = 0; i < seq->count; i++)
	{
		pfree(instantsx[i]);
		pfree(instantsy[i]);
		if (hasz)
			pfree(instantsz[i]);
	}
	pfree(instantsx); pfree(instantsy);
	pfree(seqx); pfree(seqy);
	if (hasz)
	{
		pfree(seqz); pfree(instantsz);
	}

	return result;
}

Datum
tgeompoints_twcentroid(TemporalS *ts)
{
	int srid = tpoints_srid(ts);
	TemporalSeq **sequencesx = palloc(sizeof(TemporalSeq *) * ts->count);
	TemporalSeq **sequencesy = palloc(sizeof(TemporalSeq *) * ts->count);
	TemporalSeq **sequencesz = NULL; /* keep compiler quiet */
	bool hasz = MOBDB_FLAGS_GET_Z(ts->flags);
	if (hasz)
		sequencesz = palloc(sizeof(TemporalSeq *) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalInst **instantsx = palloc(sizeof(TemporalInst *) * seq->count);
		TemporalInst **instantsy = palloc(sizeof(TemporalInst *) * seq->count);
		TemporalInst **instantsz;
		if (hasz)
			instantsz = palloc(sizeof(TemporalInst *) * seq->count);
		for (int j = 0; j < seq->count; j++)
		{
			TemporalInst *inst = temporalseq_inst_n(seq, j);
			POINT4D point = datum_get_point4d(temporalinst_value(inst));
			instantsx[j] = temporalinst_make(Float8GetDatum(point.x),
				inst->t, FLOAT8OID);		
			instantsy[j] = temporalinst_make(Float8GetDatum(point.y),
				inst->t, FLOAT8OID);
			if (hasz)
				instantsz[j] = temporalinst_make(Float8GetDatum(point.z),
					inst->t, FLOAT8OID);
		}
		sequencesx[i] = temporalseq_make(instantsx, seq->count,
			seq->period.lower_inc, seq->period.upper_inc,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
		sequencesy[i] = temporalseq_make(instantsy,
			seq->count, seq->period.lower_inc, seq->period.upper_inc,
			MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
		if (hasz)
			sequencesz[i] = temporalseq_make(instantsz, seq->count,
				seq->period.lower_inc, seq->period.upper_inc,
				MOBDB_FLAGS_GET_LINEAR(seq->flags), true);

		for (int j = 0; j < seq->count; j++)
		{
			pfree(instantsx[j]); pfree(instantsy[j]);
			if (hasz)
				pfree(instantsz[j]);
		}
		pfree(instantsx); pfree(instantsy);
		if (hasz)
			pfree(instantsz);
	}
	TemporalS *tsx = temporals_make(sequencesx, ts->count, true);
	TemporalS *tsy = temporals_make(sequencesy, ts->count, true);
	TemporalS *tsz = NULL; /* keep compiler quiet */
	if (hasz)
		tsz = temporals_make(sequencesz, ts->count, true);

	double twavgx = tnumbers_twavg(tsx);
	double twavgy = tnumbers_twavg(tsy);
	double twavgz;
	LWPOINT *lwpoint;
	if (hasz)
	{
		twavgz = tnumbers_twavg(tsz);
		lwpoint = lwpoint_make3dz(srid, twavgx, twavgy, twavgz);
	}
	else
		lwpoint = lwpoint_make2d(srid, twavgx, twavgy);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *)lwpoint));

	pfree(lwpoint);
	for (int i = 0; i < ts->count; i++)
	{
		pfree(sequencesx[i]); pfree(sequencesy[i]);
		if (hasz)
			pfree(sequencesz[i]);
	}
	pfree(sequencesx); pfree(sequencesy);
	pfree(tsx); pfree(tsy);
	if (hasz)
	{
		pfree(tsz); pfree(sequencesz);
	}
	
	return result;
}

Datum
tgeompoint_twcentroid_internal(Temporal *temp)
{
	Datum result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = temporalinst_value_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tgeompointi_twcentroid((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tgeompointseq_twcentroid((TemporalSeq *)temp);
	else /* temp->duration == TEMPORALS */
		result = tgeompoints_twcentroid((TemporalS *)temp);
	return result;
}

PG_FUNCTION_INFO_V1(tgeompoint_twcentroid);

PGDLLEXPORT Datum
tgeompoint_twcentroid(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result = tgeompoint_twcentroid_internal(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}
	
/*****************************************************************************
 * Temporal azimuth
 *****************************************************************************/

static Datum
geom_azimuth(Datum geom1, Datum geom2)
{
	POINT2D p1 = datum_get_point2d(geom1);
	POINT2D p2 = datum_get_point2d(geom2);
	double result;
	azimuth_pt_pt(&p1, &p2, &result);
	return Float8GetDatum(result);
}

static Datum
geog_azimuth(Datum geom1, Datum geom2)
{
	return call_function2(geography_azimuth, geom1, geom2);
}

static int
tpointseq_azimuth1(TemporalSeq **result, const TemporalSeq *seq)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return 0;

	/* Determine the PostGIS function to call */
	Datum (*func)(Datum, Datum);
	ensure_point_base_type(seq->valuetypid);
	if (seq->valuetypid == type_oid(T_GEOMETRY))
		func = &geom_azimuth;
	else
		func = &geog_azimuth;

	/* We are sure that there are at least 2 instants */
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * seq->count);
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	Datum value1 = temporalinst_value(inst1);
	int k = 0, l = 0;
	Datum azimuth = 0; /* Make the compiler quiet */
	bool lower_inc = seq->period.lower_inc, upper_inc;
	for (int i = 1; i < seq->count; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i);
		Datum value2 = temporalinst_value(inst2);
		upper_inc = (i == seq->count - 1) ? seq->period.upper_inc : false;
		if (datum_ne(value1, value2, seq->valuetypid))
		{
			azimuth = func(value1, value2);
			instants[k++] = temporalinst_make(azimuth, inst1->t, FLOAT8OID);
		}
		else
		{
			if (k != 0)
			{
				instants[k++] = temporalinst_make(azimuth, inst1->t, FLOAT8OID);
				upper_inc = true;
				/* Resulting sequence has step interpolation */
				result[l++] = temporalseq_make(instants, k, lower_inc,
					upper_inc, false, true);
				for (int j = 0; j < k; j++)
					pfree(instants[j]);
				k = 0;
			}
			lower_inc = true;
		}
		inst1 = inst2;
		value1 = value2;
	}
	if (k != 0)
	{
		instants[k++] = temporalinst_make(azimuth, inst1->t, FLOAT8OID);
		/* Resulting sequence has step interpolation */
		result[l++] = temporalseq_make(instants, k, lower_inc, upper_inc,
			false, true);
	}

	pfree(instants);

	return l;
}

TemporalS *
tpointseq_azimuth(TemporalSeq *seq)
{
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * seq->count);
	int count = tpointseq_azimuth1(sequences, seq);
	if (count == 0)
	{
		pfree(sequences);
		return NULL;
	}
	
	/* Resulting sequence set has step interpolation */
	TemporalS *result = temporals_make(sequences, count, true);
	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

TemporalS *
tpoints_azimuth(TemporalS *ts)
{
	if (ts->count == 1)
		return tpointseq_azimuth(temporals_seq_n(ts, 0));

	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->totalcount);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		k += tpointseq_azimuth1(&sequences[k], seq);
	}
	if (k == 0)
		return NULL;

	/* Resulting sequence set has step interpolation */
	TemporalS *result = temporals_make(sequences, k, true);

	for (int i = 0; i < k; i++)
		pfree(sequences[i]);
	pfree(sequences);
	
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_azimuth);

PGDLLEXPORT Datum
tpoint_azimuth(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Temporal *result = NULL;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI ||
		(temp->duration == TEMPORALSEQ && ! MOBDB_FLAGS_GET_LINEAR(temp->flags)) ||
		(temp->duration == TEMPORALS && ! MOBDB_FLAGS_GET_LINEAR(temp->flags)))
		;
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tpointseq_azimuth((TemporalSeq *)temp);	
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)tpoints_azimuth((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Restriction functions
 * N.B. In the current version of PostGIS (2.5) there is no true ST_Intersection
 * function for geography
 *****************************************************************************/

/* Restrict a temporal point to a geometry */

static TemporalInst *
tpointinst_at_geometry(const TemporalInst *inst, Datum geom)
{
	if (!DatumGetBool(call_function2(intersects, temporalinst_value(inst), geom)))
		return NULL;
	return temporalinst_copy(inst);
}

static TemporalI *
tpointi_at_geometry(const TemporalI *ti, Datum geom)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	int k = 0;
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		if (DatumGetBool(call_function2(intersects, temporalinst_value(inst), geom)))
			instants[k++] = inst;
	}
	TemporalI *result = NULL;
	if (k != 0)
		result = temporali_make(instants, k);
	/* We do not need to pfree the instants */
	pfree(instants);
	return result;
}

/*
 * This function assumes that inst1 and inst2 have equal SRID and that the
 * points and the geometry are in 2D
 */
static TemporalSeq **
tpointseq_at_geometry1(const TemporalInst *inst1, const TemporalInst *inst2,
	bool linear, bool lower_inc, bool upper_inc, Datum geom, int *count)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);

	/* Constant segment or step interpolation */
	bool equal = datum_point_eq(value1, value2);
	if (equal || ! linear)
	{
		if (!DatumGetBool(call_function2(intersects, value1, geom)))
		{
			*count = 0;
			return NULL;
		}

		TemporalInst *instants[2];
		instants[0] = (TemporalInst *) inst1;
		instants[1] = equal ? (TemporalInst *) inst2 :
			temporalinst_make(value1, inst2->t, inst1->valuetypid);
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_make(instants, 2, lower_inc, upper_inc,
			linear, false);
		*count = 1;
		if (! equal)
			pfree(instants[1]);
		return result;
	}

	/* Look for intersections in linear segment */
	Datum line = geompoint_trajectory(value1, value2);
	Datum inter = call_function2(intersection, line, geom);
	GSERIALIZED *gsinter = (GSERIALIZED *) PG_DETOAST_DATUM(inter);
	if (gserialized_is_empty(gsinter))
	{
		pfree(DatumGetPointer(line));
		pfree(DatumGetPointer(inter));
		POSTGIS_FREE_IF_COPY_P(gsinter, DatumGetPointer(gsinter));
		*count = 0;
		return NULL;
	}

	POINT4D start = datum_get_point4d(value1);
	POINT4D end = datum_get_point4d(value2);
	LWGEOM *lwgeom_inter = lwgeom_from_gserialized(gsinter);
	int type = lwgeom_inter->type;
	int countinter;
	LWPOINT *lwpoint_inter;
	LWLINE *lwline_inter;
	LWCOLLECTION *coll;
	if (type == POINTTYPE)
	{
		countinter = 1;
		lwpoint_inter = lwgeom_as_lwpoint(lwgeom_inter);

	}
	else if (type == LINETYPE)
	{
		countinter = 1;
		lwline_inter = lwgeom_as_lwline(lwgeom_inter);
	}
	else
	{
		coll = lwgeom_as_lwcollection(lwgeom_inter);
		countinter = coll->ngeoms;
	}
	TemporalInst *instants[2];
	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * countinter);
	double duration = (inst2->t - inst1->t);
	int k = 0;
	for (int i = 0; i < countinter; i++)
	{
		if (countinter > 1)
		{
			/* Find the i-th intersection */
			LWGEOM *subgeom = coll->geoms[i];
			if (subgeom->type == POINTTYPE)
				lwpoint_inter = lwgeom_as_lwpoint(subgeom);
			else /* type == LINETYPE */
				lwline_inter = lwgeom_as_lwline(subgeom);
			type = 	subgeom->type;
		}
		POINT4D p1, p2;
		double fraction1, fraction2;
		TimestampTz t1, t2;
		Datum point1, point2;
		/* Each intersection is either a point or a linestring with two points */
		if (type == POINTTYPE)
		{
			lwpoint_getPoint4d_p(lwpoint_inter, &p1);
			fraction1 = closest_point_on_segment_ratio(&p1, &start, &end);
			t1 = inst1->t + (long) (duration * fraction1);
			/* If the intersection is not at an exclusive bound */
			if ((lower_inc || t1 > inst1->t) && (upper_inc || t1 < inst2->t))
			{
				point1 = temporalseq_value_at_timestamp1(inst1, inst2, true, t1);
				instants[0] = temporalinst_make(point1, t1, inst1->valuetypid);
				result[k++] = temporalseq_make(instants, 1, true, true,
					linear, false);
				pfree(DatumGetPointer(point1));
				pfree(instants[0]);
			}
		}
		else
		{
			LWPOINT *lwpoint1 = lwline_get_lwpoint(lwline_inter, 0);
			LWPOINT *lwpoint2 = lwline_get_lwpoint(lwline_inter, 1);
			lwpoint_getPoint4d_p(lwpoint1, &p1);
			lwpoint_getPoint4d_p(lwpoint2, &p2);
			fraction1 = closest_point_on_segment_ratio(&p1, &start, &end);
			fraction2 = closest_point_on_segment_ratio(&p2, &start, &end);
			t1 = inst1->t + (long) (duration * fraction1);
			t2 = inst1->t + (long) (duration * fraction2);
			TimestampTz lower1 = Min(t1, t2);
			TimestampTz upper1 = Max(t1, t2);
			point1 = temporalseq_value_at_timestamp1(inst1, inst2, true, lower1);
			point2 = temporalseq_value_at_timestamp1(inst1, inst2, true, upper1);
			instants[0] = temporalinst_make(point1, lower1, inst1->valuetypid);
			instants[1] = temporalinst_make(point2, upper1, inst1->valuetypid);
			bool lower_inc1 = (lower1 == inst1->t) ? lower_inc : true;
			bool upper_inc1 = (upper1 == inst2->t) ? upper_inc : true;
			result[k++] = temporalseq_make(instants, 2, lower_inc1, upper_inc1,
				linear, false);
			pfree(DatumGetPointer(point1)); pfree(DatumGetPointer(point2));
			pfree(instants[0]); pfree(instants[1]);
		}
	}

	pfree(DatumGetPointer(line));
	pfree(DatumGetPointer(inter));
	POSTGIS_FREE_IF_COPY_P(gsinter, DatumGetPointer(gsinter));
//	POSTGIS_FREE_IF_COPY_P(gsline, DatumGetPointer(gsline));
//	lwline_free(lwline);
	lwgeom_free(lwgeom_inter);

	if (k == 0)
	{
		pfree(result);
		*count = 0;
		return NULL;
	}

	temporalseqarr_sort(result, k);
	*count = k;
	return result;
}

TemporalSeq **
tpointseq_at_geometry2(const TemporalSeq *seq, Datum geom, int *count)
{
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		/* Due to the bounding box test in the calling function we are sure
		 * that the point intersects the geometry */
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_copy(seq);
		*count = 1;
		return result;
	}

	/* Temporal sequence has at least 2 instants */
	bool linear = MOBDB_FLAGS_GET_LINEAR(seq->flags);
	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * (seq->count - 1));
	int *countseqs = palloc0(sizeof(int) * (seq->count - 1));
	int totalseqs = 0;
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	bool lower_inc = seq->period.lower_inc;
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		bool upper_inc = (i == seq->count - 2) ? seq->period.upper_inc : false;
		sequences[i] = tpointseq_at_geometry1(inst1, inst2, linear,
			lower_inc, upper_inc, geom, &countseqs[i]);
		totalseqs += countseqs[i];
		inst1 = inst2;
		lower_inc = true;
	}
	if (totalseqs == 0)
	{
		pfree(countseqs);
		pfree(sequences);
		*count = 0;
		return NULL;
	}

	TemporalSeq **result = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < seq->count - 1; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			result[k++] = sequences[i][j];
		if (sequences[i] != NULL)
			pfree(sequences[i]);
	}

	pfree(countseqs);
	pfree(sequences);
	*count = totalseqs;
	return result;
}

static TemporalS *
tpointseq_at_geometry(const TemporalSeq *seq, Datum geom)
{
	int count;
	TemporalSeq **sequences = tpointseq_at_geometry2(seq, geom, &count);
	if (sequences == NULL)
		return NULL;

	TemporalS *result = temporals_make(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalS *
tpoints_at_geometry(const TemporalS *ts, Datum geom, const STBOX *box2)
{
	/* palloc0 used due to the bounding box test in the for loop below */
	TemporalSeq ***sequences = palloc0(sizeof(TemporalSeq *) * ts->count);
	int *countseqs = palloc0(sizeof(int) * ts->count);
	int totalseqs = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		/* Bounding box test */
		STBOX *box1 = temporalseq_bbox_ptr(seq);
		if (overlaps_stbox_stbox_internal(box1, box2))
		{
			sequences[i] = tpointseq_at_geometry2(seq, geom,
				&countseqs[i]);
			totalseqs += countseqs[i];
		}
	}
	if (totalseqs == 0)
	{
		pfree(sequences);
		pfree(countseqs);
		return NULL;
	}

	TemporalSeq **allsequences = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			allsequences[k++] = sequences[i][j];
		if (sequences[i] != NULL)
			pfree(sequences[i]);
	}
	TemporalS *result = temporals_make(allsequences, totalseqs, true);

	for (int i = 0; i < totalseqs; i++)
		pfree(allsequences[i]);
	pfree(allsequences);
	pfree(sequences);
	pfree(countseqs);
	return result;
}

/* This function assumes that the arguments are of the same dimensionality,
 * have the same SRID, and that the geometry is not empty */
Temporal *
tpoint_at_geometry_internal(Temporal *temp, Datum geom)
{
	/* Bounding box test */
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	temporal_bbox(&box1, temp);
	/* Non-empty geometries have a bounding box */
	assert(geo_to_stbox_internal(&box2, (GSERIALIZED *) DatumGetPointer(geom)));
	if (!overlaps_stbox_stbox_internal(&box1, &box2))
		return NULL;

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tpointinst_at_geometry((TemporalInst *)temp, geom);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tpointi_at_geometry((TemporalI *)temp, geom);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tpointseq_at_geometry((TemporalSeq *)temp, geom);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)tpoints_at_geometry((TemporalS *)temp, geom, &box2);

	return result;
}

PG_FUNCTION_INFO_V1(tpoint_at_geometry);

PGDLLEXPORT Datum
tpoint_at_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}
	Temporal *result = tpoint_at_geometry_internal(temp, PointerGetDatum(gs));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Restrict a temporal point to an stbox */

/* This function assumes that the arguments are of the same dimensionality and
 * have the same SRID */
Temporal *
tpoint_at_stbox_internal(const Temporal *temp, const STBOX *box)
{
	/* Bounding box test */
	STBOX box1;
	memset(&box1, 0, sizeof(STBOX));
	temporal_bbox(&box1, temp);
	if (!overlaps_stbox_stbox_internal(box, &box1))
		return NULL;

	/* At least one of MOBDB_FLAGS_GET_T and MOBDB_FLAGS_GET_X is true */
	Temporal *temp1;
	if (MOBDB_FLAGS_GET_T(box->flags))
	{
		Period p;
		period_set(&p, box->tmin, box->tmax, true, true);
		temp1 = temporal_at_period_internal(temp, &p);
	}
	else
		temp1 = (Temporal *) temp;

	Temporal *result;
	if (MOBDB_FLAGS_GET_X(box->flags))
	{
		Datum gbox = PointerGetDatum(stbox_to_gbox(box));
		Datum geom = MOBDB_FLAGS_GET_Z(box->flags) ?
			call_function1(BOX3D_to_LWGEOM, gbox) :
			call_function1(BOX2D_to_LWGEOM, gbox);
		Datum geom1 = call_function2(LWGEOM_set_srid, geom,
			Int32GetDatum(box->srid));
		result = tpoint_at_geometry_internal(temp1, geom1);
		pfree(DatumGetPointer(gbox)); pfree(DatumGetPointer(geom));
		pfree(DatumGetPointer(geom1));
		if (MOBDB_FLAGS_GET_T(box->flags))
			pfree(temp1);
	}
	else
		result = temp1;
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_at_stbox);

PGDLLEXPORT Datum
tpoint_at_stbox(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	STBOX *box = PG_GETARG_STBOX_P(1);
	ensure_same_srid_tpoint_stbox(temp, box);
	Temporal *result = tpoint_at_stbox_internal(temp, box);
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Restrict a temporal point to the complement of a geometry */

static TemporalInst *
tpointinst_minus_geometry(const TemporalInst *inst, Datum geom)
{
	if (DatumGetBool(call_function2(intersects, temporalinst_value(inst), geom)))
		return NULL;
	return temporalinst_copy(inst);
}

static TemporalI *
tpointi_minus_geometry(const TemporalI *ti, Datum geom)
{
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * ti->count);
	int k = 0;
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		if (!DatumGetBool(call_function2(intersects, temporalinst_value(inst), geom)))
			instants[k++] = inst;
	}
	TemporalI *result = NULL;
	if (k != 0)
		result = temporali_make(instants, k);
	/* We do not need to pfree the instants */
	pfree(instants);
	return result;
}

/*
 * It is not possible to use a similar approach as for tpointseq_at_geometry1
 * where instead of computing the intersections we compute the difference since
 * in PostGIS the following query
 *  	select st_astext(st_difference(geometry 'Linestring(0 0,3 3)',
 *  		geometry 'MultiPoint((1 1),(2 2),(3 3))'))
 * returns "LINESTRING(0 0,3 3)". Therefore we compute tpointseq_at_geometry1
 * and then compute the complement of the value obtained.
 */
static TemporalSeq **
tpointseq_minus_geometry1(const TemporalSeq *seq, Datum geom, int *count)
{
	int countinter;
	TemporalSeq **sequences = tpointseq_at_geometry2(seq, geom, &countinter);
	if (countinter == 0)
	{
		TemporalSeq **result = palloc(sizeof(TemporalSeq *));
		result[0] = temporalseq_copy(seq);
		*count = 1;
		return result;
	}
		
	Period **periods = palloc(sizeof(Period) * countinter);
	for (int i = 0; i < countinter; i++)
		periods[i] = &sequences[i]->period;
	PeriodSet *ps1 = periodset_make_internal(periods, countinter, false);
	PeriodSet *ps2 = minus_period_periodset_internal(&seq->period, ps1);
	pfree(ps1); pfree(periods);
	if (ps2 == NULL)
	{
		*count = 0;
		return NULL;
	}
	TemporalSeq **result = temporalseq_at_periodset2(seq, ps2, count);
	pfree(ps2);
	return result;
}

static TemporalS *
tpointseq_minus_geometry(const TemporalSeq *seq, Datum geom)
{
	int count;
	TemporalSeq **sequences = tpointseq_minus_geometry1(seq, geom, &count);
	if (sequences == NULL)
		return NULL;

	TemporalS *result = temporals_make(sequences, count, true);

	for (int i = 0; i < count; i++)
		pfree(sequences[i]);
	pfree(sequences);

	return result;
}

static TemporalS *
tpoints_minus_geometry(const TemporalS *ts, Datum geom, STBOX *box2)
{
	/* Singleton sequence set */
	if (ts->count == 1)
		return tpointseq_minus_geometry(temporals_seq_n(ts, 0),
			geom);

	TemporalSeq ***sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	int *countseqs = palloc0(sizeof(int) * ts->count);
	int totalseqs = 0;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		/* Bounding box test */
		STBOX *box1 = temporalseq_bbox_ptr(seq);
		if (!overlaps_stbox_stbox_internal(box1, box2))
		{
			sequences[i] = palloc(sizeof(TemporalSeq *));
			sequences[i][0] = temporalseq_copy(seq);
			countseqs[i] = 1;
			totalseqs ++;
		}
		else
		{
			sequences[i] = tpointseq_minus_geometry1(seq, geom,
				&countseqs[i]);
			totalseqs += countseqs[i];
		}
	}
	if (totalseqs == 0)
	{
		pfree(sequences); pfree(countseqs);
		return NULL;
	}

	TemporalSeq **allsequences = palloc(sizeof(TemporalSeq *) * totalseqs);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{
		for (int j = 0; j < countseqs[i]; j++)
			allsequences[k++] = sequences[i][j];
		if (countseqs[i] != 0)
			pfree(sequences[i]);
	}
	TemporalS *result = temporals_make(allsequences, totalseqs, true);

	for (int i = 0; i < totalseqs; i++)
		pfree(allsequences[i]);
	pfree(allsequences); pfree(sequences); pfree(countseqs);

	return result;
}

/* This function assumes that the arguments are of the same dimensionality,
 * have the same SRID, and that the geometry is not empty */
Temporal *
tpoint_minus_geometry_internal(Temporal *temp, Datum geom)
{
	/* Bounding box test */
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	temporal_bbox(&box1, temp);
	/* Non-empty geometries have a bounding box */
	assert(geo_to_stbox_internal(&box2, (GSERIALIZED *) DatumGetPointer(geom)));
	if (!overlaps_stbox_stbox_internal(&box1, &box2))
		return temporal_copy(temp);

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = (Temporal *)tpointinst_minus_geometry((TemporalInst *)temp, geom);
	else if (temp->duration == TEMPORALI)
		result = (Temporal *)tpointi_minus_geometry((TemporalI *)temp, geom);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *)tpointseq_minus_geometry((TemporalSeq *)temp, geom);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)tpoints_minus_geometry((TemporalS *)temp, geom, &box2);

	return result;
}

PG_FUNCTION_INFO_V1(tpoint_minus_geometry);

PGDLLEXPORT Datum
tpoint_minus_geometry(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		Temporal *copy = temporal_copy(temp);
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(copy);
	}

	Temporal *result = tpoint_minus_geometry_internal(temp, PointerGetDatum(gs));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* This function assumes that the arguments are of the same dimensionality and
 * have the same SRID */
Temporal *
tpoint_minus_stbox_internal(const Temporal *temp, const STBOX *box)
{
	/* Bounding box test */
	STBOX box1;
	memset(&box1, 0, sizeof(STBOX));
	temporal_bbox(&box1, temp);
	if (!overlaps_stbox_stbox_internal(box, &box1))
		return temporal_copy(temp);

	PeriodSet *ps1 = temporal_get_time_internal(temp);
	Temporal *temp1 = tpoint_at_stbox_internal(temp, box);
	PeriodSet *ps2 = temporal_get_time_internal(temp1);
	PeriodSet *ps = minus_periodset_periodset_internal(ps1, ps2);
	Temporal *result = temporal_at_periodset_internal(temp, ps);
	pfree(temp1); pfree(ps1); pfree(ps2); pfree(ps);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_minus_stbox);

PGDLLEXPORT Datum
tpoint_minus_stbox(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	STBOX *box = PG_GETARG_STBOX_P(1);
	ensure_same_srid_tpoint_stbox(temp, box);
	Temporal *result = tpoint_minus_stbox_internal(temp, box);
	PG_FREE_IF_COPY(temp, 0);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Nearest approach instant
 *****************************************************************************/

static TemporalInst *
NAI_tpointi_geo(const TemporalI *ti, Datum geo, Datum (*func)(Datum, Datum))
{
	double mindist = DBL_MAX;
	int number = 0; /* keep compiler quiet */
	for (int i = 0; i < ti->count; i++)
	{
		TemporalInst *inst = temporali_inst_n(ti, i);
		Datum value = temporalinst_value(inst);
		double dist = DatumGetFloat8(func(value, geo));	
		if (dist < mindist)
		{
			mindist = dist;
			number = i;
		}
	}
	return temporalinst_copy(temporali_inst_n(ti, number));
}

/*****************************************************************************/

/* NAI between temporal sequence point with step interpolation and a
 * geometry/geography */

static TemporalInst *
NAI_tpointseq_stw_geo(const TemporalSeq *seq, Datum geo,
	Datum (*func)(Datum, Datum))
{
	double mindist = DBL_MAX;
	int number = 0; /* keep compiler quiet */
	for (int i = 0; i < seq->count; i++)
	{
		Datum value = temporalinst_value(temporalseq_inst_n(seq, i));
		double dist = DatumGetFloat8(func(value, geo));	
		if (dist < mindist)
		{
			mindist = dist;
			number = i;
		}
	}
	return temporalinst_copy(temporalseq_inst_n(seq, number));
}

/* NAI between temporal sequence point with linear interpolation and a
 * geometry/geography */

/* Functions inspired from PostGIS functions lw_dist2d_distancepoint from
 * measures.c and lw_dist3d_distancepoint from measures3d.c that also return
 * the distance between the closest/longest point */
static LWPOINT *
lw_dist2d_point_dist(const LWGEOM *lw1, const LWGEOM *lw2, int srid, int mode, double *dist)
{
	DISTPTS thedl;
	thedl.mode = mode;
	thedl.distance= FLT_MAX;
	thedl.tolerance = 0;
	lw_dist2d_comp(lw1, lw2, &thedl);
	LWPOINT *result = lwpoint_make2d(srid, thedl.p1.x, thedl.p1.y);
	*dist = thedl.distance;
	return result;
}

static LWPOINT *
lw_dist3d_point_dist(const LWGEOM *lw1, const LWGEOM *lw2, int srid, int mode, double *dist)
{
	assert(FLAGS_GET_Z(lw1->flags) && FLAGS_GET_Z(lw2->flags));
	DISTPTS3D thedl;
	thedl.mode = mode;
	thedl.distance= FLT_MAX;
	thedl.tolerance = 0;
	lw_dist3d_recursive(lw1, lw2, &thedl);
	LWPOINT *result = lwpoint_make3dz(srid, thedl.p1.x, thedl.p1.y, thedl.p1.z);
	*dist = thedl.distance;
	return result;
}

static Datum
NAI_tpointseq_geom1(const TemporalInst *inst1, const TemporalInst *inst2,
	LWGEOM *lwgeom, TimestampTz *t, bool *tofree, double *dist)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	/* Constant segment */
	if (datum_point_eq(value1, value2))
	{
		*t = inst1->t;
		*tofree = false;
		return value1;
	}

	/* The trajectory is a line */
	LWLINE *lwline = geompoint_trajectory_lwline(value1, value2);
	LWPOINT *lwpoint = MOBDB_FLAGS_GET_Z(inst1->flags) ?
		lw_dist3d_point_dist((LWGEOM *) lwline, lwgeom, lwline->srid, DIST_MIN, dist) :
		lw_dist2d_point_dist((LWGEOM *) lwline, lwgeom, lwline->srid, DIST_MIN, dist);
	POINT4D p, proj;
	lwpoint_getPoint4d_p(lwpoint, &p);
	POINT4D start = datum_get_point4d(value1);
	POINT4D end = datum_get_point4d(value2);
	double fraction = closest_point_on_segment_ratio(&p, &start, &end);
	lwline_free(lwline); lwpoint_free(lwpoint);

	if (fraction == 0)
	{
		*t = inst1->t;
		*tofree = false;
		return value1;
	}
	if (fraction == 1)
	{
		*t = inst2->t;
		*tofree = false;
		return value2;
	}

	long double duration = (long double) (inst2->t - inst1->t);
	*t = inst1->t + (long)(duration * fraction);
	*tofree = true;
	LWPOINT *lwres = lwpoint_make2d(lwpoint->srid, proj.x, proj.y);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *) lwres));
	lwpoint_free(lwres);
	return result;
}

static Datum
NAI_tpointseq_geog1(const TemporalInst *inst1, const TemporalInst *inst2,
	Datum geo, TimestampTz *t, bool *tofree)
{
	Datum value1 = temporalinst_value(inst1);
	Datum value2 = temporalinst_value(inst2);
	/* Constant segment */
	if (datum_point_eq(value1, value2))
	{
		*t = inst1->t;
		*tofree = false;
		return value1;
	}
	// xxx

	/* The trajectory is a line */
	Datum traj = geogpoint_trajectory(value1, value2);
	/* There is no function equivalent to LWGEOM_line_locate_point
	 * for geographies. We do as the ST_Intersection function, e.g.
	 * 'SELECT geography(ST_Transform(ST_Intersection(ST_Transform(geometry($1),
	 * @extschema@._ST_BestSRID($1, $2)),
	 * ST_Transform(geometry($2), @extschema@._ST_BestSRID($1, $2))), 4326))' */
	Datum bestsrid = call_function2(geography_bestsrid, traj, geo);
	Datum traj1 = call_function1(geometry_from_geography, traj);
	Datum traj2 = call_function2(transform, traj1, bestsrid);
	Datum geo1 = call_function1(geometry_from_geography, geo);
	Datum geo2 = call_function2(transform, geo1, bestsrid);
	Datum point = call_function2(LWGEOM_closestpoint, traj2, geo2);
	long double duration = (long double) (inst2->t - inst1->t);
	long double fraction = DatumGetFloat8(call_function2(
		LWGEOM_line_locate_point, traj2, point));
	pfree(DatumGetPointer(traj)); pfree(DatumGetPointer(traj1));
	pfree(DatumGetPointer(traj2)); pfree(DatumGetPointer(geo1));
	pfree(DatumGetPointer(geo2)); pfree(DatumGetPointer(point));

	if (fraction == 0)
	{
		*t = inst1->t;
		*tofree = false;
		return value1;
	}
	if (fraction == 1)
	{
		*t = inst2->t;
		*tofree = false;
		return value2;
	}

	*t = inst1->t + (long)(duration * fraction);
	*tofree = true;
	/* Linear interpolation */
	return temporalseq_value_at_timestamp1(inst1, inst2, true, *t);
}

static TemporalInst *
NAI_tpointseq_geo(const TemporalSeq *seq, Datum geo, Datum (*func)(Datum, Datum))
{
	/* Instantaneous sequence */
	if (seq->count == 1)
		return temporalinst_copy(temporalseq_inst_n(seq, 0));

	/* Stepwise interpolation */
	if (! MOBDB_FLAGS_GET_LINEAR(seq->flags))
		return NAI_tpointseq_stw_geo(seq, geo, func);

	/* Linear interpolation */
	double mindist = DBL_MAX;
	Datum minpoint = 0; /* keep compiler quiet */
	TimestampTz tmin = 0; /* keep compiler quiet */
	bool mintofree =  false; /* keep compiler quiet */
	TemporalInst *inst1 = temporalseq_inst_n(seq, 0);
	ensure_point_base_type(inst1->valuetypid);
	bool geometry = inst1->valuetypid == type_oid(T_GEOMETRY);
	GSERIALIZED *gsgeom;
	LWGEOM *lwgeom;
	if (geometry)
	{
		gsgeom = (GSERIALIZED *)PG_DETOAST_DATUM(geo);
		lwgeom = lwgeom_from_gserialized(gsgeom);
	}
	for (int i = 0; i < seq->count - 1; i++)
	{
		TemporalInst *inst2 = temporalseq_inst_n(seq, i + 1);
		TimestampTz t;
		bool tofree;
		Datum point;
		double dist;
		if (geometry)
			point = NAI_tpointseq_geom1(inst1, inst2, lwgeom, &t, &tofree, &dist);
		else
		{
			point = NAI_tpointseq_geog1(inst1, inst2, geo, &t, &tofree);
			dist = DatumGetFloat8(func(point, geo));
		}
		if (dist < mindist)
		{
			mindist = dist;
			minpoint = point;
			tmin = t;
			mintofree = tofree;
		}
		else if (tofree)
			pfree(DatumGetPointer(point)); 			
		inst1 = inst2;
	}
	TemporalInst *result = temporalinst_make(minpoint, tmin, seq->valuetypid);
	if (mintofree)
		pfree(DatumGetPointer(minpoint));
	if (geometry)
	{
		POSTGIS_FREE_IF_COPY_P(gsgeom, DatumGetPointer(geo));
		lwgeom_free(lwgeom);
	}
	return result;
}

/*****************************************************************************/

static TemporalInst *
NAI_tpoints_geo(const TemporalS *ts, Datum geo, Datum (*func)(Datum, Datum))
{
	TemporalInst *result = NULL;
	double mindist = DBL_MAX;
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalInst *inst = NAI_tpointseq_geo(seq, geo, func);
		Datum value = temporalinst_value(inst);
		double dist = DatumGetFloat8(func(value, geo));
		if (dist < mindist)
		{
			if (result != NULL)
				pfree(result);
			result = inst;
			mindist = dist;
		}
		else
			pfree(inst);
	}
	return result;
}

/*****************************************************************************/

TemporalInst *
NAI_tpoint_geo_internal(Temporal *temp, GSERIALIZED *gs)
{
	Datum (*func)(Datum, Datum);
	ensure_point_base_type(temp->valuetypid);
	if (temp->valuetypid == type_oid(T_GEOMETRY))
		func = &geom_distance2d;
	else
		func = &geog_distance;
	TemporalInst *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = temporalinst_copy((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = NAI_tpointi_geo((TemporalI *)temp,
			PointerGetDatum(gs), func);
	else if (temp->duration == TEMPORALSEQ)
		result = NAI_tpointseq_geo((TemporalSeq *)temp,
			PointerGetDatum(gs), func);
	else /* temp->duration == TEMPORALS */
		result = NAI_tpoints_geo((TemporalS *)temp,
			PointerGetDatum(gs), func);

	return result;
}

PG_FUNCTION_INFO_V1(NAI_geo_tpoint);

PGDLLEXPORT Datum
NAI_geo_tpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	TemporalInst *result = NAI_tpoint_geo_internal(temp, gs);
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(NAI_tpoint_geo);

PGDLLEXPORT Datum
NAI_tpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	// Allow 3D geometries ?
	if (MOBDB_FLAGS_GET_Z(temp->flags) || FLAGS_GET_Z(gs->flags))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
			errmsg("3D geometries are not allowed")));
	}
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	TemporalInst *result = NAI_tpoint_geo_internal(temp, gs);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

/* Find a timestamp which is sure to be an exclusive bound */

static TemporalInst *
temporalseq_find_timestamp_excl(const TemporalSeq *seq, TimestampTz t)
{
	TemporalInst *result;
	if (t == seq->period.lower)
		result = temporalseq_inst_n(seq, 0);
	else
		result = temporalseq_inst_n(seq, seq->count - 1);
	return temporalinst_copy(result);
}

static TemporalInst *
temporals_find_timestamp_excl(const TemporalS *ts, TimestampTz t)
{
	TemporalInst *result;
	int pos;
	temporals_find_timestamp(ts, t, &pos);
	TemporalSeq *seq1, *seq2;
	if (pos == 0)
	{
		seq1 = temporals_seq_n(ts, 0);
		result = temporalseq_inst_n(seq1, 0);
	}
	else if (pos == ts->count)
	{
		seq1 = temporals_seq_n(ts, ts->count - 1);
		result = temporalseq_inst_n(seq1, seq1->count - 1);
	}
	else
	{
		seq1 = temporals_seq_n(ts, pos - 1);
		seq2 = temporals_seq_n(ts, pos);
		if (temporalseq_end_timestamp(seq1) == t)
			result = temporalseq_inst_n(seq1, seq1->count - 1);
		else
			result = temporalseq_inst_n(seq2, 0);
	}
	return temporalinst_copy(result);
}

PG_FUNCTION_INFO_V1(NAI_tpoint_tpoint);

PGDLLEXPORT Datum
NAI_tpoint_tpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tpoint(temp1, temp2);
	ensure_same_dimensionality_tpoint(temp1, temp2);
	TemporalInst *result = NULL;
	Temporal *dist = distance_tpoint_tpoint_internal(temp1, temp2);
	if (dist != NULL)
	{
		TemporalInst *min = temporal_min_instant(dist);
		result = temporal_at_timestamp_internal(temp1, min->t);
		pfree(dist);
		if (result == NULL)
		{
			if (temp1->duration == TEMPORALSEQ)
				result = temporalseq_find_timestamp_excl((TemporalSeq *)temp1,
					min->t);
			else /* temp->duration == TEMPORALS */
				result = temporals_find_timestamp_excl((TemporalS *)temp1,
					min->t);
		}
	}
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Nearest approach distance
 *****************************************************************************/

PG_FUNCTION_INFO_V1(NAD_geo_tpoint);

PGDLLEXPORT Datum
NAD_geo_tpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Datum (*func)(Datum, Datum);
	ensure_point_base_type(temp->valuetypid);
	if (temp->valuetypid == type_oid(T_GEOMETRY))
		func = MOBDB_FLAGS_GET_Z(temp->flags) ? &geom_distance3d :
			&geom_distance2d;
	else
		func = &geog_distance;

	Datum traj = tpoint_trajectory_internal(temp);
	Datum result = func(traj, PointerGetDatum(gs));

	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(NAD_tpoint_geo);

PGDLLEXPORT Datum
NAD_tpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Datum (*func)(Datum, Datum);
	ensure_point_base_type(temp->valuetypid);
	if (temp->valuetypid == type_oid(T_GEOMETRY))
		func = MOBDB_FLAGS_GET_Z(temp->flags) ? &geom_distance3d :
			&geom_distance2d;
	else
		func = &geog_distance;

	Datum traj = tpoint_trajectory_internal(temp);
	Datum result = func(traj, PointerGetDatum(gs));

	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(NAD_tpoint_tpoint);

PGDLLEXPORT Datum
NAD_tpoint_tpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tpoint(temp1, temp2);
	ensure_same_dimensionality_tpoint(temp1, temp2);
	Temporal *dist = distance_tpoint_tpoint_internal(temp1, temp2);
	if (dist == NULL)
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}

	Datum result = temporal_min_value_internal(dist);
	pfree(dist);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * ShortestLine
 *****************************************************************************/

PG_FUNCTION_INFO_V1(shortestline_geo_tpoint);

PGDLLEXPORT Datum
shortestline_geo_tpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	Temporal *temp = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		PG_FREE_IF_COPY(temp, 1);
		PG_RETURN_NULL();
	}

	Datum traj = tpoint_trajectory_internal(temp);
	Datum result =  MOBDB_FLAGS_GET_Z(temp->flags) ?
		call_function2(LWGEOM_shortestline3d, traj, PointerGetDatum(gs)) :
		call_function2(LWGEOM_shortestline2d, traj, PointerGetDatum(gs));

	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(gs, 0);
	PG_FREE_IF_COPY(temp, 1);
	PG_RETURN_DATUM(result);
}

PG_FUNCTION_INFO_V1(shortestline_tpoint_geo);

PGDLLEXPORT Datum
shortestline_tpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	if (gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Datum traj = tpoint_trajectory_internal(temp);
	Datum result =  MOBDB_FLAGS_GET_Z(temp->flags) ?
		call_function2(LWGEOM_shortestline3d, traj, PointerGetDatum(gs)) :
		call_function2(LWGEOM_shortestline2d, traj, PointerGetDatum(gs));

	pfree(DatumGetPointer(traj));
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************/
/* These functions suppose that the temporal values overlap in time */

static Datum
shortestline_tpointinst_tpointinst(const TemporalInst *inst1,
	const TemporalInst *inst2)
{
	LWGEOM *lwgeoms[2];
	GSERIALIZED *gs1 = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst1));
	GSERIALIZED *gs2 = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst2));
	lwgeoms[0] = lwgeom_from_gserialized(gs1);
	lwgeoms[1] = lwgeom_from_gserialized(gs2);
	LWLINE *line = lwline_from_lwgeom_array(lwgeoms[0]->srid, 2, lwgeoms);
	Datum result = PointerGetDatum(geometry_serialize((LWGEOM *)line));
	lwgeom_free(lwgeoms[0]);
	lwgeom_free(lwgeoms[1]);
	return result;
}

static Datum
shortestline_tpointi_tpointi(const TemporalI *ti1, const TemporalI *ti2,
	Datum (*func)(Datum, Datum))
{
	/* Compute the distance */
	TemporalI *dist = sync_tfunc2_temporali_temporali(ti1, ti2, func,
		FLOAT8OID);
	Datum mind = temporali_min_value(dist);
	TemporalI *mindist = temporali_at_value(dist, mind);
	TimestampTz t = temporali_start_timestamp(mindist);
	TemporalInst *inst1 = temporali_at_timestamp(ti1, t);
	TemporalInst *inst2 = temporali_at_timestamp(ti2, t);
	Datum result = shortestline_tpointinst_tpointinst(inst1, inst2);
	pfree(dist); pfree(mindist); pfree(inst1); pfree(inst2);
	return result;
}

static Datum
shortestline_tpointseq_tpointseq(const TemporalSeq *seq1, const TemporalSeq *seq2,
	Datum (*func)(Datum, Datum))
{
	/* Compute the distance */
	bool linear = MOBDB_FLAGS_GET_LINEAR(seq1->flags) ||
		MOBDB_FLAGS_GET_LINEAR(seq2->flags);
	TemporalSeq *dist = sync_tfunc2_temporalseq_temporalseq(seq1, seq2,
		func, FLOAT8OID, linear, NULL);
	TemporalInst *min = temporalseq_min_instant(dist);
	/* Timestamp t may be at an exclusive bound */
	TemporalInst *inst1, *inst2;
	if (min->t == seq1->period.lower)
	{
		inst1 = temporalseq_inst_n(seq1, 0);
		inst2 = temporalseq_inst_n(seq2, 0);
	}
	else if (min->t == seq1->period.upper)
	{
		inst1 = temporalseq_inst_n(seq1, seq1->count - 1);
		inst2 = temporalseq_inst_n(seq2, seq1->count - 1);
	}
	else
	{
		inst1 = temporalseq_at_timestamp(seq1, min->t);
		inst2 = temporalseq_at_timestamp(seq2, min->t);
	}
	Datum result = shortestline_tpointinst_tpointinst(inst1, inst2);
	pfree(dist);
	return result;
}

static Datum
shortestline_tpoints_tpoints(const TemporalS *ts1, const TemporalS *ts2,
	Datum (*func)(Datum, Datum))
{
	/* Compute the distance */
	bool linear = MOBDB_FLAGS_GET_LINEAR(ts1->flags) ||
		MOBDB_FLAGS_GET_LINEAR(ts2->flags);
	TemporalS *dist = sync_tfunc2_temporals_temporals(ts1, ts2, func,
		FLOAT8OID, linear, NULL);
	TemporalInst *min = temporals_min_instant(dist);
	TemporalInst *inst1 = temporals_at_timestamp(ts1, min->t);
	TemporalInst *inst2 = temporals_at_timestamp(ts2, min->t);
	
	/* If t is at an exclusive bound */
	bool freeinst1 = (inst1 != NULL);
	TemporalSeq *seq1, *seq2;
	if (inst1 == NULL)
	{
		int pos;
		temporals_find_timestamp(ts1, min->t, &pos);
		if (pos == 0)
		{
			seq1 = temporals_seq_n(ts1, 0);
			inst1 = temporalseq_inst_n(seq1, 0);
		}
		else if (pos == ts1->count)
		{
			seq1 = temporals_seq_n(ts1, ts1->count - 1);
			inst1 = temporalseq_inst_n(seq1, seq1->count - 1);
		}
		else
		{
			seq1 = temporals_seq_n(ts1, pos - 1);
			seq2 = temporals_seq_n(ts1, pos);
			if (temporalseq_end_timestamp(seq1) == min->t)
				inst1 = temporalseq_inst_n(seq1, seq1->count - 1);
			else
				inst1 = temporalseq_inst_n(seq2, 0);
		}
	}
	
	/* If t is at an exclusive bound */
	bool freeinst2 = (inst2 != NULL);
	if (inst2 == NULL)
	{
		int pos;
		temporals_find_timestamp(ts2, min->t, &pos);
		if (pos == 0)
		{
			seq2 = temporals_seq_n(ts2, 0);
			inst2 = temporalseq_inst_n(seq2, 0);
		}
		else if (pos == ts2->count)
		{
			seq2 = temporals_seq_n(ts2, ts2->count - 1);
			inst2 = temporalseq_inst_n(seq2, seq2->count - 1);
		}
		else
		{
			seq1 = temporals_seq_n(ts2, pos - 1);
			seq2 = temporals_seq_n(ts2, pos);
			if (temporalseq_end_timestamp(seq1) == min->t)
				inst2 = temporalseq_inst_n(seq1, seq1->count - 1);
			else
				inst2 = temporalseq_inst_n(seq2, 0);
		}
	}
	
	Datum result = shortestline_tpointinst_tpointinst(inst1, inst2);
	pfree(dist);
	if (freeinst1)
		pfree(inst1);
	if (freeinst2)
		pfree(inst2);
	return result;
}

/*****************************************************************************/

/* The internal function supposes that the temporal points are synchronized */

Datum
shortestline_tpoint_tpoint_internal(const Temporal *temp1, const Temporal *temp2)
{
	Datum (*func)(Datum, Datum);
	ensure_point_base_type(temp1->valuetypid);
	if (temp1->valuetypid == type_oid(T_GEOMETRY))
		func = MOBDB_FLAGS_GET_Z(temp1->flags) ?
			&geom_distance3d : &geom_distance2d;
	else
		func = &geog_distance;

	Datum result;
	ensure_valid_duration(temp1->duration);
	if (temp1->duration == TEMPORALINST)
		result = shortestline_tpointinst_tpointinst((TemporalInst *)temp1,
			(TemporalInst *)temp2);
	else if (temp1->duration == TEMPORALI)
		result = shortestline_tpointi_tpointi((TemporalI *)temp1,
			(TemporalI *)temp2, func);
	else if (temp1->duration == TEMPORALSEQ)
		result = shortestline_tpointseq_tpointseq((TemporalSeq *)temp1,
			(TemporalSeq *)temp2, func);
	else /* temp1->duration == TEMPORALS */
		result = shortestline_tpoints_tpoints((TemporalS *)temp1,
			(TemporalS *)temp2, func);

	return result;
}

PG_FUNCTION_INFO_V1(shortestline_tpoint_tpoint);

PGDLLEXPORT Datum
shortestline_tpoint_tpoint(PG_FUNCTION_ARGS)
{
	Temporal *temp1 = PG_GETARG_TEMPORAL(0);
	Temporal *temp2 = PG_GETARG_TEMPORAL(1);
	ensure_same_srid_tpoint(temp1, temp2);
	ensure_same_dimensionality_tpoint(temp1, temp2);
	Temporal *sync1, *sync2;
	/* Return NULL if the temporal points do not intersect in time */
	if (!synchronize_temporal_temporal(temp1, temp2, &sync1, &sync2, true))
	{
		PG_FREE_IF_COPY(temp1, 0);
		PG_FREE_IF_COPY(temp2, 1);
		PG_RETURN_NULL();
	}

	Datum result = shortestline_tpoint_tpoint_internal(sync1, sync2);
	pfree(sync1); pfree(sync2);
	PG_FREE_IF_COPY(temp1, 0);
	PG_FREE_IF_COPY(temp2, 1);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Convert a temporal point into a trajectory geometry/geography where the M 
 * coordinates encode the timestamps in number of seconds since '1970-01-01'
 * The internal representation of timestamps in PostgreSQL is in microseconds
 * since '2000-01-01'. Therefore we need to compute
 * select date_part('epoch', timestamp '2000-01-01' - timestamp '1970-01-01')
 * which results in 946684800
 *****************************************************************************/

static LWPOINT *
point_to_trajpoint(GSERIALIZED *gs, TimestampTz t)
{
	int32 srid = gserialized_get_srid(gs);
	double epoch = ((double)t / 1e6) + 946684800;
	LWPOINT *result;
	if (FLAGS_GET_Z(gs->flags))
	{
		POINT3DZ point = gs_get_point3dz(gs);
		result = lwpoint_make4d(srid, point.x, point.y, point.z, epoch);
	}
	else
	{
		POINT2D point = gs_get_point2d(gs);
		result = lwpoint_make3dm(srid, point.x, point.y, epoch);
	}
	FLAGS_SET_GEODETIC(result->flags, FLAGS_GET_GEODETIC(gs->flags));
	return result;
}

static Datum
tpointinst_to_geo(const TemporalInst *inst)
{
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	LWPOINT *point = point_to_trajpoint(gs, inst->t);
	GSERIALIZED *result = geometry_serialize((LWGEOM *)point);
	pfree(point);
	return PointerGetDatum(result);
}

static Datum
tpointi_to_geo(const TemporalI *ti)
{
	TemporalInst *inst = temporali_inst_n(ti, 0);
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	int32 srid = gserialized_get_srid(gs);
	LWGEOM **points = palloc(sizeof(LWGEOM *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		inst = temporali_inst_n(ti, i);
		gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
		points[i] = (LWGEOM *)point_to_trajpoint(gs, inst->t);
	}
	GSERIALIZED *result;
	if (ti->count == 1)
		result = geometry_serialize(points[0]);
	else
	{
		LWGEOM *mpoint = (LWGEOM *)lwcollection_construct(MULTIPOINTTYPE, srid,
			NULL, (uint32_t) ti->count, points);
		result = geometry_serialize(mpoint);
		pfree(mpoint);
	}

	for (int i = 0; i < ti->count; i++)
		pfree(points[i]);
	pfree(points);
	return PointerGetDatum(result);
}

static LWGEOM *
tpointseq_to_geo1(const TemporalSeq *seq)
{
	LWPOINT **points = palloc(sizeof(LWGEOM *) * seq->count);
	for (int i = 0; i < seq->count; i++)
	{
		TemporalInst *inst = temporalseq_inst_n(seq, i);
		GSERIALIZED *gs = (GSERIALIZED *) PointerGetDatum(temporalinst_value(inst));
		points[i] = point_to_trajpoint(gs, inst->t);
	}
	LWGEOM *result;
	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		result = (LWGEOM *) points[0];
		pfree(points);
	}
	else
	{
		if (MOBDB_FLAGS_GET_LINEAR(seq->flags))
			result = (LWGEOM *) lwline_from_lwgeom_array(points[0]->srid,
				(uint32_t) seq->count, (LWGEOM **) points);
		else
			result = (LWGEOM *) lwcollection_construct(MULTIPOINTTYPE,
				points[0]->srid, NULL, (uint32_t) seq->count, (LWGEOM **) points);
		for (int i = 0; i < seq->count; i++)
			lwpoint_free(points[i]);
		pfree(points);
	}
	return result;
}


static Datum
tpointseq_to_geo(const TemporalSeq *seq)
{
	LWGEOM *lwgeom = tpointseq_to_geo1(seq);
	GSERIALIZED *result = geometry_serialize(lwgeom);
	return PointerGetDatum(result);
}

static Datum
tpoints_to_geo(const TemporalS *ts)
{
	/* Instantaneous sequence */
	if (ts->count == 1)
	{
		TemporalSeq *seq = temporals_seq_n(ts, 0);
		return tpointseq_to_geo(seq);
	}
	uint32_t colltype = 0;
	LWGEOM **geoms = palloc(sizeof(LWGEOM *) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		geoms[i] = tpointseq_to_geo1(seq);
		/* Output type not initialized */
		if (! colltype)
			colltype = lwtype_get_collectiontype(geoms[i]->type);
			/* Input type not compatible with output */
			/* make output type a collection */
		else if (colltype != COLLECTIONTYPE &&
			lwtype_get_collectiontype(geoms[i]->type) != colltype)
			colltype = COLLECTIONTYPE;
	}
	// TODO add the bounding box instead of ask PostGIS to compute it again
	// GBOX *box = stbox_to_gbox(temporalseq_bbox_ptr(seq));
	LWGEOM *coll = (LWGEOM *) lwcollection_construct((uint8_t) colltype,
		geoms[0]->srid, NULL, (uint32_t) ts->count, geoms);
	Datum result = PointerGetDatum(geometry_serialize(coll));
	/* We cannot lwgeom_free(geoms[i] or lwgeom_free(coll) */
	pfree(geoms);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_to_geo);

PGDLLEXPORT Datum
tpoint_to_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST)
		result = tpointinst_to_geo((TemporalInst *)temp);
	else if (temp->duration == TEMPORALI)
		result = tpointi_to_geo((TemporalI *)temp);
	else if (temp->duration == TEMPORALSEQ)
		result = tpointseq_to_geo((TemporalSeq *)temp);
	else /* temp->duration == TEMPORALS */
		result = tpoints_to_geo((TemporalS *)temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_DATUM(result);
}

/*****************************************************************************
 * Convert trajectory geometry/geography where the M coordinates encode the
 * timestamps in number of seconds since '1970-01-01' into a temporal point.
 *****************************************************************************/

static TemporalInst *
trajpoint_to_tpointinst(LWPOINT *lwpoint)
{
	bool hasz = (bool) FLAGS_GET_Z(lwpoint->flags);
	bool geodetic = (bool) FLAGS_GET_GEODETIC(lwpoint->flags);
	LWPOINT *lwpoint1;
	TimestampTz t;
	if (hasz)
	{
		POINT4D point = getPoint4d(lwpoint->point, 0);
		t = (long) ((point.m - 946684800) * 1e6);
		lwpoint1 = lwpoint_make3dz(lwpoint->srid, point.x, point.y, point.z);
	}
	else
	{
		POINT3DM point = getPoint3dm(lwpoint->point, 0);
		t = (long) ((point.m - 946684800) * 1e6);
		lwpoint1 = lwpoint_make2d(lwpoint->srid, point.x, point.y);
	}
	FLAGS_SET_GEODETIC(lwpoint1->flags, geodetic);
	GSERIALIZED *gs = geometry_serialize((LWGEOM *)lwpoint1);
	Oid valuetypid = geodetic ? type_oid(T_GEOGRAPHY) : type_oid(T_GEOMETRY);
	TemporalInst *result = temporalinst_make(PointerGetDatum(gs), t,
		valuetypid);
	pfree(gs);
	return result;	
}

static TemporalInst *
geo_to_tpointinst(GSERIALIZED *gs)
{
	/* Geometry is a POINT */
	LWGEOM *lwgeom = lwgeom_from_gserialized(gs);
	TemporalInst *result = trajpoint_to_tpointinst((LWPOINT *)lwgeom);
	lwgeom_free(lwgeom);
	return result;
}

static TemporalI *
geo_to_tpointi(GSERIALIZED *gs)
{
	TemporalI *result;
	/* Geometry is a MULTIPOINT */
	LWGEOM *lwgeom = lwgeom_from_gserialized(gs);
	bool hasz = (bool) FLAGS_GET_Z(gs->flags);
	/* Verify that is a valid set of trajectory points */
	LWCOLLECTION *lwcoll = lwgeom_as_lwcollection(lwgeom);
	double m1 = -1 * DBL_MAX, m2;
	int npoints = lwcoll->ngeoms;
	for (int i = 0; i < npoints; i++)
	{
		LWPOINT *lwpoint = (LWPOINT *)lwcoll->geoms[i];
		if (hasz)
		{
			POINT4D point = getPoint4d(lwpoint->point, 0);
			m2 = point.m;
		}
		else
		{
			POINT3DM point = getPoint3dm(lwpoint->point, 0);
			m2 = point.m;
		}
		if (m1 >= m2)
		{
			lwgeom_free(lwgeom);
			ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Trajectory must be valid")));
		}
		m1 = m2;
	}
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * npoints);
	for (int i = 0; i < npoints; i++)
		instants[i] = trajpoint_to_tpointinst((LWPOINT *)lwcoll->geoms[i]);
	result = temporali_make(instants, npoints);
	
	lwgeom_free(lwgeom);
	for (int i = 0; i < npoints; i++)
		pfree(instants[i]);
	pfree(instants);
	return result;
}

static TemporalSeq *
geo_to_tpointseq(GSERIALIZED *gs)
{
	/* Geometry is a LINESTRING */
	bool hasz =(bool)  FLAGS_GET_Z(gs->flags);
	LWGEOM *lwgeom = lwgeom_from_gserialized(gs);
	LWLINE *lwline = lwgeom_as_lwline(lwgeom);
	int npoints = lwline->points->npoints;
	/*
	 * Verify that the trajectory is valid.
	 * Since calling lwgeom_is_trajectory causes discrepancies with regression
	 * tests because of the error message depends on PostGIS version,
	 * the verification is made here.
	 */
	double m1 = -1 * DBL_MAX, m2;
	for (int i = 0; i < npoints; i++)
	{
		if (hasz)
		{
			POINT4D point = getPoint4d(lwline->points, (uint32_t) i);
			m2 = point.m;
		}
		else
		{
			POINT3DM point = getPoint3dm(lwline->points, (uint32_t) i);
			m2 = point.m;
		}
		if (m1 >= m2)
		{
			lwgeom_free(lwgeom);
			ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Trajectory must be valid")));
		}
		m1 = m2;
	}
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * npoints);
	for (int i = 0; i < npoints; i++)
	{
		/* Returns freshly allocated LWPOINT */
		LWPOINT *lwpoint = lwline_get_lwpoint(lwline, (uint32_t) i);
		instants[i] = trajpoint_to_tpointinst(lwpoint);
		lwpoint_free(lwpoint);
	}
	/* The resulting sequence assumes linear interpolation */
	TemporalSeq *result = temporalseq_make(instants, npoints, true, true,
		true, true);
	for (int i = 0; i < npoints; i++)
		pfree(instants[i]);
	pfree(instants);
	lwgeom_free(lwgeom);
	return result;
}

static TemporalS *
geo_to_tpoints(GSERIALIZED *gs)
{
	TemporalS *result;
	/* Geometry is a MULTILINESTRING or a COLLECTION */
	LWGEOM *lwgeom = lwgeom_from_gserialized(gs);
	LWCOLLECTION *lwcoll = lwgeom_as_lwcollection(lwgeom);
	int ngeoms = lwcoll->ngeoms;
	for (int i = 0; i < ngeoms; i++)
	{
		LWGEOM *lwgeom1 = lwcoll->geoms[i];
		if (lwgeom1->type != POINTTYPE && lwgeom1->type != LINETYPE)
		{
			lwgeom_free(lwgeom);
			ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Component geometry/geography must be of type Point(Z)M or Linestring(Z)M")));
		}
	}
	
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ngeoms);
	for (int i = 0; i < ngeoms; i++)
	{
		LWGEOM *lwgeom1 = lwcoll->geoms[i];
		GSERIALIZED *gs1 = geometry_serialize(lwgeom1);
		if (lwgeom1->type == POINTTYPE)
		{
			TemporalInst *inst = geo_to_tpointinst(gs1);
			/* The resulting sequence assumes linear interpolation */
			sequences[i] = temporalseq_make(&inst, 1, true, true,
				true, false);
			pfree(inst);
		}
		else /* lwgeom1->type == LINETYPE */
			sequences[i] = geo_to_tpointseq(gs1);
		pfree(gs1);
	}
	/* The resulting sequence set assumes linear interpolation */
	result = temporals_make(sequences, ngeoms, false);
	for (int i = 0; i < ngeoms; i++)
		pfree(sequences[i]);
	pfree(sequences);
	lwgeom_free(lwgeom);
	return result;
}

PG_FUNCTION_INFO_V1(geo_to_tpoint);

PGDLLEXPORT Datum
geo_to_tpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	ensure_non_empty(gs);
	ensure_has_M_gs(gs);
	
	Temporal *result = NULL; /* Make compiler quiet */
	if (gserialized_get_type(gs) == POINTTYPE)
		result = (Temporal *)geo_to_tpointinst(gs);
	else if (gserialized_get_type(gs) == MULTIPOINTTYPE)
		result = (Temporal *)geo_to_tpointi(gs);
	else if (gserialized_get_type(gs) == LINETYPE)
		result = (Temporal *)geo_to_tpointseq(gs);
	else if (gserialized_get_type(gs) == MULTILINETYPE ||
		gserialized_get_type(gs) == COLLECTIONTYPE)
		result = (Temporal *)geo_to_tpoints(gs);
	else
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("Invalid geometry type for trajectory")));
	
	PG_FREE_IF_COPY(gs, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Convert a temporal point into a LinestringM geometry/geography where the M
 * coordinates values are given by a temporal float.
 *****************************************************************************/

static LWPOINT *
point_measure_to_geo_measure(GSERIALIZED *gs, int32 srid, double measure)
{
	LWPOINT *result;
	if (FLAGS_GET_Z(gs->flags))
	{
		POINT3DZ point = gs_get_point3dz(gs);
		result = lwpoint_make4d(srid, point.x, point.y, point.z, measure);
	}
	else
	{
		POINT2D point = gs_get_point2d(gs);
		result = lwpoint_make3dm(srid, point.x, point.y, measure);
	}
	FLAGS_SET_GEODETIC(result->flags, FLAGS_GET_GEODETIC(gs->flags));
	return result;
}

static Datum
tpointinst_to_geo_measure(const TemporalInst *inst, const TemporalInst *measure)
{
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	int32 srid = gserialized_get_srid(gs);
	LWPOINT *point = point_measure_to_geo_measure(gs, srid,
		DatumGetFloat8(temporalinst_value(measure)));
	GSERIALIZED *result = geometry_serialize((LWGEOM *)point);
	pfree(point);
	return PointerGetDatum(result);
}

static Datum
tpointi_to_geo_measure(const TemporalI *ti, const TemporalI *measure)
{
	TemporalInst *inst = temporali_inst_n(ti, 0);
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	int32 srid = gserialized_get_srid(gs);
	LWGEOM **points = palloc(sizeof(LWGEOM *) * ti->count);
	for (int i = 0; i < ti->count; i++)
	{
		inst = temporali_inst_n(ti, i);
		TemporalInst *m = temporali_inst_n(measure, i);
		gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
		points[i] = (LWGEOM *) point_measure_to_geo_measure(gs, srid,
			DatumGetFloat8(temporalinst_value(m)));
	}
	GSERIALIZED *result;
	if (ti->count == 1)
		result = geometry_serialize(points[0]);
	else
	{
		LWGEOM *mpoint = (LWGEOM *) lwcollection_construct(MULTIPOINTTYPE, srid,
			NULL, (uint32_t) ti->count, points);
		result = geometry_serialize(mpoint);
		pfree(mpoint);
	}

	for (int i = 0; i < ti->count; i++)
		pfree(points[i]);
	pfree(points);
	return PointerGetDatum(result);
}

static LWGEOM *
tpointseq_to_geo_measure1(const TemporalSeq *seq, const TemporalSeq *measure)
{
	LWPOINT **points = palloc(sizeof(LWPOINT *) * seq->count);
	/* Remove two consecutive points if they are equal */
	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	TemporalInst *m = temporalseq_inst_n(measure, 0);
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	int32 srid = gserialized_get_srid(gs);
	LWPOINT *value1 = point_measure_to_geo_measure(gs, srid,
		DatumGetFloat8(temporalinst_value(m)));
	points[0] = value1;
	int k = 1;
	for (int i = 1; i < seq->count; i++)
	{
		inst = temporalseq_inst_n(seq, i);
		m = temporalseq_inst_n(measure, i);
		gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
		LWPOINT *value2 = point_measure_to_geo_measure(gs, srid,
			DatumGetFloat8(temporalinst_value(m)));
		if (lwpoint_same(value1, value2) != LW_TRUE)
			points[k++] = value2;
		value1 = value2;
	}
	LWGEOM *result;
	if (k == 1)
	{
		result = (LWGEOM *) points[0];
		pfree(points);
	}
	else
	{
		result = MOBDB_FLAGS_GET_LINEAR(seq->flags) ?
			(LWGEOM *) lwline_from_lwgeom_array(points[0]->srid, (uint32_t) k,
				(LWGEOM **) points) :
			(LWGEOM *) lwcollection_construct(MULTIPOINTTYPE,
				points[0]->srid, NULL, (uint32_t) k, (LWGEOM **) points);
		for (int i = 0; i < k; i++)
			lwpoint_free(points[i]);
		pfree(points);
	}
	return result;
}

static Datum
tpointseq_to_geo_measure(const TemporalSeq *seq, const TemporalSeq *measure)
{
	LWGEOM *lwgeom = tpointseq_to_geo_measure1(seq, measure);
	GSERIALIZED *result = geometry_serialize(lwgeom);
	return PointerGetDatum(result);
}

static Datum
tpoints_to_geo_measure(const TemporalS *ts, const TemporalS *measure)
{
	/* Instantaneous sequence */
	if (ts->count == 1)
	{
		TemporalSeq *seq1 = temporals_seq_n(ts, 0);
		TemporalSeq *seq2 = temporals_seq_n(measure, 0);
		return tpointseq_to_geo_measure(seq1, seq2);
	}

	uint8_t colltype = 0;
	LWGEOM **geoms = palloc(sizeof(LWGEOM *) * ts->count);
	for (int i = 0; i < ts->count; i++)
	{
		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalSeq *m = temporals_seq_n(measure, i);
		geoms[i] = tpointseq_to_geo_measure1(seq, m);
		/* Output type not initialized */
		if (! colltype)
			colltype = (uint8_t) lwtype_get_collectiontype(geoms[i]->type);
		/* Input type not compatible with output */
		/* make output type a collection */
		else if (colltype != COLLECTIONTYPE &&
			lwtype_get_collectiontype(geoms[i]->type) != colltype)
			colltype = COLLECTIONTYPE;
	}
	// TODO add the bounding box instead of ask PostGIS to compute it again
	// GBOX *box = stbox_to_gbox(temporalseq_bbox_ptr(seq));
	LWGEOM *coll = (LWGEOM *) lwcollection_construct(colltype,
		geoms[0]->srid, NULL, (uint32_t) ts->count, geoms);
	Datum result = PointerGetDatum(geometry_serialize(coll));
	/* We cannot lwgeom_free(geoms[i] or lwgeom_free(coll) */
	pfree(geoms);
	return result;
}

/*****************************************************************************/

static int
tpointseq_to_geo_measure_segmentize1(LWGEOM **result, const TemporalSeq *seq,
	const TemporalSeq *measure)
{
	TemporalInst *inst = temporalseq_inst_n(seq, 0);
	double m = DatumGetFloat8(temporalinst_value(temporalseq_inst_n(measure, 0)));
	LWPOINT *points[2];
	GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
	int32 srid = gserialized_get_srid(gs);

	/* Instantaneous sequence */
	if (seq->count == 1)
	{
		result[0] = (LWGEOM *) point_measure_to_geo_measure(gs, srid, m);
		return 1;
	}

	/* General case */
	for (int i = 0; i < seq->count - 1; i++)
	{
		points[0] = point_measure_to_geo_measure(gs, srid, m);
		inst = temporalseq_inst_n(seq, i + 1);
		gs = (GSERIALIZED *) DatumGetPointer(temporalinst_value_ptr(inst));
		points[1] = point_measure_to_geo_measure(gs, srid, m);
		result[i] = (LWGEOM *) lwline_from_lwgeom_array(srid, 2, (LWGEOM **) points);
		lwpoint_free(points[0]); lwpoint_free(points[1]);
		m = DatumGetFloat8(temporalinst_value(temporalseq_inst_n(measure, i + 1)));
	}
	return seq->count - 1;
}

static Datum
tpointseq_to_geo_measure_segmentize(const TemporalSeq *seq, const TemporalSeq *measure)
{
	int count = (seq->count == 1) ? 1 : seq->count - 1;
	LWGEOM **geoms = palloc(sizeof(LWGEOM *) * count);
	tpointseq_to_geo_measure_segmentize1(geoms, seq, measure);
	Datum result;
	/* Instantaneous sequence */
	if (seq->count == 1)
		result = PointerGetDatum(geometry_serialize(geoms[0]));
	else
	{
		// TODO add the bounding box instead of ask PostGIS to compute it again
		// GBOX *box = stbox_to_gbox(temporalseq_bbox_ptr(seq));
		LWGEOM *segcoll = (LWGEOM *) lwcollection_construct(MULTILINETYPE,
			geoms[0]->srid, NULL, (uint32_t)(seq->count - 1), geoms);
		result = PointerGetDatum(geometry_serialize(segcoll));
	}
	for (int i = 0; i < count; i++)
		lwgeom_free(geoms[i]);
	pfree(geoms);
	return result;
}

static Datum
tpoints_to_geo_measure_segmentize(const TemporalS *ts, const TemporalS *measure)
{
	/* Instantaneous sequence */
	if (ts->count == 1)
	{
		TemporalSeq *seq1 = temporals_seq_n(ts, 0);
		TemporalSeq *seq2 = temporals_seq_n(measure, 0);
		return tpointseq_to_geo_measure_segmentize(seq1, seq2);
	}

	uint8_t colltype = 0;
	LWGEOM **geoms = palloc(sizeof(LWGEOM *) * ts->totalcount);
	int k = 0;
	for (int i = 0; i < ts->count; i++)
	{

		TemporalSeq *seq = temporals_seq_n(ts, i);
		TemporalSeq *m = temporals_seq_n(measure, i);
		k += tpointseq_to_geo_measure_segmentize1(&geoms[k], seq, m);
		/* Output type not initialized */
		if (! colltype)
			colltype = (uint8_t) lwtype_get_collectiontype(geoms[k - 1]->type);
			/* Input type not compatible with output */
			/* make output type a collection */
		else if (colltype != COLLECTIONTYPE &&
				 lwtype_get_collectiontype(geoms[k - 1]->type) != colltype)
			colltype = COLLECTIONTYPE;
	}
	Datum result;
	// TODO add the bounding box instead of ask PostGIS to compute it again
	// GBOX *box = stbox_to_gbox(temporals_bbox_ptr(seq));
	LWGEOM *coll = (LWGEOM *) lwcollection_construct(colltype,
		geoms[0]->srid, NULL, (uint32_t) k, geoms);
	result = PointerGetDatum(geometry_serialize(coll));
	for (int i = 0; i < k; i++)
		lwgeom_free(geoms[i]);
	pfree(geoms);
	return result;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(tpoint_to_geo_measure);

PGDLLEXPORT Datum
tpoint_to_geo_measure(PG_FUNCTION_ARGS)
{
	Temporal *tpoint = PG_GETARG_TEMPORAL(0);
	Temporal *measure = PG_GETARG_TEMPORAL(1);
	bool segmentize = PG_GETARG_BOOL(2);
	ensure_point_base_type(tpoint->valuetypid);
	ensure_numeric_base_type(measure->valuetypid);

	Temporal *sync1, *sync2;
	/* Return false if the temporal values do not intersect in time
	   The last parameter crossing must be set to false  */
	if (!synchronize_temporal_temporal(tpoint, measure, &sync1, &sync2, false))
	{
		PG_FREE_IF_COPY(tpoint, 0);
		PG_FREE_IF_COPY(measure, 1);
		PG_RETURN_NULL();
	}

	Temporal *result;
	ensure_valid_duration(sync1->duration);
	if (sync1->duration == TEMPORALINST)
		result = (Temporal *) tpointinst_to_geo_measure(
				(TemporalInst *) sync1, (TemporalInst *) sync2);
	else if (sync1->duration == TEMPORALI)
		result = (Temporal *) tpointi_to_geo_measure(
				(TemporalI *) sync1, (TemporalI *) sync2);
	else if (sync1->duration == TEMPORALSEQ)
		result = segmentize ?
			(Temporal *) tpointseq_to_geo_measure_segmentize(
					(TemporalSeq *) sync1, (TemporalSeq *) sync2) :
			(Temporal *) tpointseq_to_geo_measure(
				(TemporalSeq *) sync1, (TemporalSeq *) sync2);
	else /* sync1->duration == TEMPORALS */
		result = segmentize ?
			(Temporal *) tpoints_to_geo_measure_segmentize(
					(TemporalS *) sync1, (TemporalS *) sync2) :
			(Temporal *) tpoints_to_geo_measure(
				(TemporalS *) sync1, (TemporalS *) sync2);

	pfree(sync1); pfree(sync2);
	PG_FREE_IF_COPY(tpoint, 0);
	PG_FREE_IF_COPY(measure, 1);
	PG_RETURN_POINTER(result);
}

/***********************************************************************
 * Simple spatio-temporal Douglas-Peucker line simplification.
 * No checks are done to avoid introduction of self-intersections.
 * No topology relations are considered.
 ***********************************************************************/

/*-------------------------------------------------------------------------
 * Determine the 3D hypotenuse.
 *
 * If required, x, y, and z are swapped to make x the larger number. The
 * traditional formula of x^2+y^2+z^2 is rearranged to factor x outside the
 * sqrt. This allows computation of the hypotenuse for significantly
 * larger values, and with a higher precision than when using the naive
 * formula.  In particular, this cannot overflow unless the final result
 * would be out-of-range.
 *
 * sqrt( x^2 + y^2 + z^2 ) 	= sqrt( x^2( 1 + y^2/x^2 + z^2/x^2) )
 * 							= x * sqrt( 1 + y^2/x^2 + z^2/x^2)
 * 					 		= x * sqrt( 1 + y/x * y/x + z/x * z/x)
 *
 * A similar solution is used for the 4D hypotenuse
 *-----------------------------------------------------------------------
 */
double
hypot3d(double x, double y, double z)
{
	double		yx;
	double		zx;
	double		temp;

	/* Handle INF and NaN properly */
	if (isinf(x) || isinf(y) || isinf(z))
		return get_float8_infinity();

	if (isnan(x) || isnan(y) || isnan(z))
		return get_float8_nan();

	/* Else, drop any minus signs */
	x = fabs(x);
	y = fabs(y);
	z = fabs(z);

	/* Swap x, y and z if needed to make x the larger one */
	if (x < y)
	{
		temp = x;
		x = y;
		y = temp;
	}
	if (x < z)
	{
		temp = x;
		x = z;
		z = temp;
	}
	/*
	 * If x is zero, the hypotenuse is computed with the 2D case.
	 * This test saves a few cycles in such cases, but more importantly
	 * it also protects against divide-by-zero errors, since now x >= y.
	 */
	if (x == 0)
		return hypot(y, z);

	/* Determine the hypotenuse */
	yx = y / x;
	zx = z / x;
	return x * sqrt(1.0 + (yx * yx) + (zx * zx));
}

double
hypot4d(double x, double y, double z, double m)
{
	double		yx;
	double		zx;
	double		mx;
	double		temp;

	/* Handle INF and NaN properly */
	if (isinf(x) || isinf(y) || isinf(z) || isinf(m))
		return get_float8_infinity();

	if (isnan(x) || isnan(y) || isnan(z) || isnan(m))
		return get_float8_nan();

	/* Else, drop any minus signs */
	x = fabs(x);
	y = fabs(y);
	z = fabs(z);
	m = fabs(m);

	/* Swap x, y, z, and m if needed to make x the larger one */
	if (x < y)
	{
		temp = x;
		x = y;
		y = temp;
	}
	if (x < z)
	{
		temp = x;
		x = z;
		z = temp;
	}
	if (x < m)
	{
		temp = x;
		x = m;
		m = temp;
	}
	/*
	 * If x is zero, the hypotenuse is computed with the 3D case.
	 * This test saves a few cycles in such cases, but more importantly
	 * it also protects against divide-by-zero errors, since now x >= y.
	 */
	if (x == 0)
		return hypot3d(y, z, m);

	/* Determine the hypotenuse */
	yx = y / x;
	zx = z / x;
	mx = m / x;
	return x * sqrt(1.0 + (yx * yx) + (zx * zx) + (mx * mx));
}

double
dist2d_pt_pt(POINT2D *p1, POINT2D *p2)
{
	double dx = p2->x - p1->x;
	double dy = p2->y - p1->y;
	return hypot(dx, dy);
}

double
dist3d_pt_pt(POINT3DZ *p1, POINT3DZ *p2)
{
	double dx = p2->x - p1->x;
	double dy = p2->y - p1->y;
	double dz = p2->z - p1->z;
	return hypot3d(dx, dy, dz);
}

double
dist4d_pt_pt(POINT4D *p1, POINT4D *p2)
{
	double dx = p2->x - p1->x;
	double dy = p2->y - p1->y;
	double dz = p2->z - p1->z;
	double dm = p2->m - p1->m;
	return hypot4d(dx, dy, dz, dm);
}

/* Distance between a 2D/3D/4D point and a 2/3D/4D line segment
 * Derived from PostGIS functions lw_dist2d_pt_seg in file measures.c
 * and lw_dist3d_pt_seg in file measures3d.c
 * See also http://geomalgorithms.com/a02-_lines.html */

double
dist2d_pt_seg(POINT2D *p, POINT2D *A, POINT2D *B)
{
	POINT2D c;
	double	r;
	/* If start==end, then use pt distance */
	if (A->x == B->x && A->y == B->y)
		return dist2d_pt_pt(p, A);

	r = ( (p->x-A->x) * (B->x-A->x) + (p->y-A->y) * (B->y-A->y) ) /
		( (B->x-A->x) * (B->x-A->x) + (B->y-A->y) * (B->y-A->y) );

	if (r < 0)	/* If the first vertex A is closest to the point p */
		return dist2d_pt_pt(p, A);
	if (r > 1)	/* If the second vertex B is closest to the point p */
		return dist2d_pt_pt(p, B);

	/* else if the point p is closer to some point between a and b
	then we find that point and send it to dist2d_pt_pt */
	c.x = A->x + r * (B->x - A->x);
	c.y = A->y + r * (B->y - A->y);

	return dist2d_pt_pt(p, &c);
}

double
dist3d_pt_seg(POINT3DZ *p, POINT3DZ *A, POINT3DZ *B)
{
	POINT3DZ c;
	double	r;
	/* If start==end, then use pt distance */
	if (A->x == B->x && A->y == B->y && A->z == B->z)
		return dist3d_pt_pt(p, A);

	r = ( (p->x-A->x) * (B->x-A->x) + (p->y-A->y) * (B->y-A->y) + (p->z-A->z) * (B->z-A->z) ) /
		( (B->x-A->x) * (B->x-A->x) + (B->y-A->y) * (B->y-A->y) + (B->z-A->z) * (B->z-A->z) );

	if (r < 0)	/* If the first vertex A is closest to the point p */
		return dist3d_pt_pt(p, A);
	if (r > 1)	/* If the second vertex B is closest to the point p */
		return dist3d_pt_pt(p, B);

	/* else if the point p is closer to some point between a and b
	then we find that point and send it to dist3d_pt_pt */
	c.x = A->x + r * (B->x - A->x);
	c.y = A->y + r * (B->y - A->y);
	c.z = A->z + r * (B->z - A->z);

	return dist3d_pt_pt(p, &c);
}

double
dist4d_pt_seg(POINT4D *p, POINT4D *A, POINT4D *B)
{
	POINT4D c;
	double	r;
	/* If start==end, then use pt distance */
	if (A->x == B->x && A->y == B->y && A->z == B->z && A->m == B->m)
		return dist4d_pt_pt(p, A);

	r = ( (p->x-A->x) * (B->x-A->x) + (p->y-A->y) * (B->y-A->y) + (p->z-A->z) * (B->z-A->z) + (p->m-A->m) * (B->m-A->m) ) /
		( (B->x-A->x) * (B->x-A->x) + (B->y-A->y) * (B->y-A->y) + (B->z-A->z) * (B->z-A->z) + (B->m-A->m) * (B->m-A->m) );

	if (r < 0)	/* If the first vertex A is closest to the point p */
		return dist4d_pt_pt(p, A);
	if (r > 1)	/* If the second vertex B is closest to the point p */
		return dist4d_pt_pt(p, B);

	/* else if the point p is closer to some point between a and b
	then we find that point and send it to dist3d_pt_pt */
	c.x = A->x + r * (B->x - A->x);
	c.y = A->y + r * (B->y - A->y);
	c.z = A->z + r * (B->z - A->z);
	c.m = A->m + r * (B->m - A->m);

	return dist4d_pt_pt(p, &c);
}

static void
tpointseq_dp_findsplit(const TemporalSeq *seq, int p1, int p2, bool withspeed,
	int *split, double *dist, double *delta_speed)
{
	int k;
	POINT2D p2k, p2k_tmp, p2a, p2b;
	POINT3DZ p3k, p3k_tmp, p3a, p3b;
	POINT4D p4k, p4a, p4b;
	double d_tmp, d, speed_seg, speed_pt;
	bool hasz = MOBDB_FLAGS_GET_Z(seq->flags);
	*split = p1;
	d = -1;
	if (p1 + 1 < p2)
	{
		Datum (*func)(Datum, Datum);
		if (withspeed)
			func = hasz ? &pt_distance3d : &pt_distance2d;
		TemporalInst *inst1 = temporalseq_inst_n(seq, p1);
		TemporalInst *inst2 = temporalseq_inst_n(seq, p2);
		if (withspeed)
			speed_seg = tpointinst_speed(inst1, inst2, func);
		if (hasz)
		{
			p3a = datum_get_point3dz(temporalinst_value(inst1));
			p3b = datum_get_point3dz(temporalinst_value(inst2));
			if (withspeed)
			{
				p4a.x = p3a.x; p4a.y = p3a.y; p4a.z = p3a.z; p4a.m = speed_seg;
				p4b.x = p3b.x; p4b.y = p3b.y; p4b.z = p3b.z; p4b.m = speed_seg;
			}
		}
		else
		{
			p2a = datum_get_point2d(temporalinst_value(inst1));
			p2b = datum_get_point2d(temporalinst_value(inst2));
			if (withspeed)
			{
				p3a.x = p2a.x; p3a.y = p2a.y; p3a.z = speed_seg;
				p3b.x = p2b.x; p3b.y = p2b.y; p3b.z = speed_seg;
			}
		}
		for (k = p1 + 1; k < p2; k++)
		{
			inst2 = temporalseq_inst_n(seq, k);
			if (withspeed)
				speed_pt = tpointinst_speed(inst1, inst2, func);
			if (hasz)
			{
				p3k_tmp = datum_get_point3dz(temporalinst_value(inst2));
				if (withspeed)
				{
					p4k.x = p3k_tmp.x; p4k.y = p3k_tmp.y; p4k.z = p3k_tmp.z; p4k.m = speed_pt;
					d_tmp = dist4d_pt_seg(&p4k, &p4a, &p4b);
				}
				else
					d_tmp = dist3d_pt_seg(&p3k_tmp, &p3a, &p3b);
			}
			else
			{
				p2k_tmp = datum_get_point2d(temporalinst_value(inst2));
				if (withspeed)
				{
					p3k.x = p2k_tmp.x; p3k.y = p2k_tmp.y; p3k.z = speed_pt;
					d_tmp = dist3d_pt_seg(&p3k, &p3a, &p3b);
				}
				else
					d_tmp = dist2d_pt_seg(&p2k_tmp, &p2a, &p2b);
			}
			if (d_tmp > d)
			{
				/* record the maximum */
				d = d_tmp;
				if (hasz)
					p3k = p3k_tmp;
				else
					p2k = p2k_tmp;
				if (withspeed)
					*delta_speed = fabs(speed_seg - speed_pt);
				*split = k;
			}
			inst1 = inst2;
		}
		*dist = hasz ? dist3d_pt_seg(&p3k, &p3a, &p3b) :
				distance2d_pt_seg(&p2k, &p2a, &p2b);
	}
	else
		*dist = -1;
}

/***********************************************************************/

static int
int_cmp(const void *a, const void *b)
{
	/* casting pointer types */
	const int *ia = (const int *)a;
	const int *ib = (const int *)b;
	/* returns negative if b > a and positive if a > b */
	return *ia - *ib;
}

TemporalSeq *
tpointseq_simplify(const TemporalSeq *seq, double eps_dist, double eps_speed, uint32_t minpts)
{
	static size_t stack_size = 256;
	int *stack, *outlist; /* recursion stack */
	int stack_static[stack_size];
	int outlist_static[stack_size];
	int sp = -1; /* recursion stack pointer */
	int p1, split;
	uint32_t outn = 0;
	uint32_t i;
	double dist, delta_speed;
	bool withspeed = eps_speed > 0;

	/* Do not try to simplify really short things */
	if (seq->count < 3)
		return temporalseq_copy(seq);

	/* Only heap allocate book-keeping arrays if necessary */
	if ((unsigned int) seq->count > stack_size)
	{
		stack = palloc(sizeof(int) * seq->count);
		outlist = palloc(sizeof(int) * seq->count);
	}
	else
	{
		stack = stack_static;
		outlist = outlist_static;
	}

	p1 = 0;
	stack[++sp] = seq->count - 1;
	/* Add first point to output list */
	outlist[outn++] = 0;
	do
	{
		tpointseq_dp_findsplit(seq, p1, stack[sp], withspeed, &split, &dist, &delta_speed);
		bool dosplit;
		if (withspeed)
			dosplit = (dist >= 0 &&
				(dist > eps_dist || delta_speed > eps_speed || outn + sp + 1 < minpts));
		else
			dosplit = (dist >= 0 &&
				(dist > eps_dist || outn + sp + 1 < minpts));
		if (dosplit)
			stack[++sp] = split;
		else
		{
			outlist[outn++] = stack[sp];
			p1 = stack[sp--];
		}
	}
	while (sp >= 0);

	/* Put list of retained points into order */
	qsort(outlist, outn, sizeof(int), int_cmp);
	/* Create new TemporalSeq */
	TemporalInst **instants = palloc(sizeof(TemporalInst *) * outn);
	for (i = 0; i < outn; i++)
		instants[i] = temporalseq_inst_n(seq, outlist[i]);
	TemporalSeq *result = temporalseq_make(instants, outn,
		seq->period.lower_inc, seq->period.upper_inc,
		MOBDB_FLAGS_GET_LINEAR(seq->flags), true);
	pfree(instants);

	/* Only free if arrays are on heap */
	if (stack != stack_static)
		pfree(stack);
	if (outlist != outlist_static)
		pfree(outlist);

	return result;
}

TemporalS *
tpoints_simplify(const TemporalS *ts, double eps_dist, double eps_speed, uint32_t minpts)
{
	TemporalSeq *seq;
	TemporalS *result;
	/* Singleton sequence set */
	if (ts->count == 1)
	{
		seq = tpointseq_simplify(temporals_seq_n(ts, 0), eps_dist, eps_speed, minpts);
		result = temporalseq_to_temporals(seq);
		pfree(seq);
		return result;
	}

	/* General case */
	TemporalSeq **sequences = palloc(sizeof(TemporalSeq *) * ts->count);
	for (int i = 0; i < ts->count; i++)
		sequences[i] = tpointseq_simplify(temporals_seq_n(ts, i), eps_dist, eps_speed, minpts);
	result = temporals_make(sequences, ts->count, true);
	for (int i = 0; i < ts->count; i++)
		pfree(sequences[i]);
	pfree(sequences);
	return result;
}

PG_FUNCTION_INFO_V1(tpoint_simplify);

Datum
tpoint_simplify(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	double eps_dist = PG_GETARG_FLOAT8(1);
	double eps_speed = PG_GETARG_FLOAT8(2);

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST || temp->duration == TEMPORALI ||
		! MOBDB_FLAGS_GET_LINEAR(temp->flags))
		result = temporal_copy(temp);
	else if (temp->duration == TEMPORALSEQ)
		result = (Temporal *) tpointseq_simplify((TemporalSeq *)temp,
			eps_dist, eps_speed, 2);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *) tpoints_simplify((TemporalS *)temp,
			eps_dist, eps_speed, 2);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************//*****************************************************************************/
