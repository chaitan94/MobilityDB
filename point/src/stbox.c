/*****************************************************************************
 *
 * stbox.c
 *	  Basic functions for STBOX bounding box.
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "stbox.h"

#include <assert.h>
#include <utils/timestamp.h>

#include "period.h"
#include "temporal_util.h"
#include "tpoint.h"
#include "tpoint_parser.h"
#include "tpoint_spatialfuncs.h"

/* Buffer size for input and  of STBOX */
#define MAXSTBOXLEN		256

/*****************************************************************************
 * Miscellaneous functions
 *****************************************************************************/

STBOX *
stbox_new(bool hasx, bool hasz, bool hast, bool geodetic)
{
	STBOX *result = palloc0(sizeof(STBOX));
	MOBDB_FLAGS_SET_X(result->flags, hasx);
	MOBDB_FLAGS_SET_Z(result->flags, hasz);
	MOBDB_FLAGS_SET_T(result->flags, hast);
	MOBDB_FLAGS_SET_GEODETIC(result->flags, geodetic);
	return result;
}

STBOX *
stbox_copy(const STBOX *box)
{
	STBOX *result = palloc0(sizeof(STBOX));
	memcpy(result, box, sizeof(STBOX));
	return result;
}

/*****************************************************************************
 * Input/Ouput functions
 *****************************************************************************/

/* 
 * Input function. 
 * Examples of input:
 * 		STBOX((1.0, 2.0), (3.0, 4.0)) -> only spatial
 * 		STBOX Z((1.0, 2.0, 3.0), (4.0, 5.0, 6.0)) -> only spatial
 * 		STBOX T((1.0, 2.0, 2001-01-01), (3.0, 4.0, 2001-01-02)) -> spatiotemporal
 * 		STBOX ZT((1.0, 2.0, 3.0, 2001-01-01), (4.0, 5.0, 6.0, 2001-01-02)) -> spatiotemporal
 * 		STBOX T(( , , 2001-01-01), ( , , 2001-01-02)) -> only temporal
 * 		SRID=xxxx;STBOX... (any of the above)
 * 		GEODSTBOX((1.0, 2.0, 3.0), (4.0, 5.0, 6.0)) -> only spatial
 * 		GEODSTBOX T((1.0, 2.0, 3.0, 2001-01-01), (4.0, 5.0, 6.0, 2001-01-02)) -> spatiotemporal
 * 		GEODSTBOX T(( , , 2001-01-01), ( , , 2001-01-02)) -> only temporal
 * 		SRID=xxxx;GEODSTBOX... (any of the above)
 * where the commas are optional and the SRID is optional. If the SRID is not
 * stated it is by default 0 for non geodetic boxes and 4326 for geodetic boxes
 */
PG_FUNCTION_INFO_V1(stbox_in);

PGDLLEXPORT Datum
stbox_in(PG_FUNCTION_ARGS)
{
	char *input = PG_GETARG_CSTRING(0);
	STBOX *result = stbox_parse(&input);
	PG_RETURN_POINTER(result);
}

static char *
stbox_to_string(const STBOX *box)
{
	static size_t size = MAXSTBOXLEN + 1;
	char *str = NULL, *strtmin = NULL, *strtmax = NULL;
	str = (char *) palloc(size);
	char srid[20];
	if (box->srid > 0)
		sprintf(srid, "SRID=%d;", box->srid);
	else
		srid[0] = '\0';
	char *boxtype = MOBDB_FLAGS_GET_GEODETIC(box->flags) ? "GEODSTBOX" : "STBOX";

	assert(MOBDB_FLAGS_GET_X(box->flags) || MOBDB_FLAGS_GET_T(box->flags));
	if (MOBDB_FLAGS_GET_T(box->flags))
	{
		strtmin = call_output(TIMESTAMPTZOID, TimestampTzGetDatum(box->tmin));
		strtmax = call_output(TIMESTAMPTZOID, TimestampTzGetDatum(box->tmax));
	}
	if (MOBDB_FLAGS_GET_X(box->flags))
	{
		if (MOBDB_FLAGS_GET_GEODETIC(box->flags))
		{
			if (MOBDB_FLAGS_GET_T(box->flags))
				snprintf(str, size, "%s%s T((%.8g,%.8g,%.8g,%s),(%.8g,%.8g,%.8g,%s))", srid, boxtype,
					box->xmin, box->ymin, box->zmin, strtmin, 
					box->xmax, box->ymax, box->zmax, strtmax);
			else
				snprintf(str, size, "%s%s((%.8g,%.8g,%.8g),(%.8g,%.8g,%.8g))", srid, boxtype,
					box->xmin, box->ymin, box->zmin, 
					box->xmax, box->ymax, box->zmax);
		}
		else if (MOBDB_FLAGS_GET_Z(box->flags) && MOBDB_FLAGS_GET_T(box->flags))
			snprintf(str, size, "%s%s ZT((%.8g,%.8g,%.8g,%s),(%.8g,%.8g,%.8g,%s))", srid, boxtype,
				box->xmin, box->ymin, box->zmin, strtmin, 
				box->xmax, box->ymax, box->zmax, strtmax);
		else if (MOBDB_FLAGS_GET_Z(box->flags))
			snprintf(str, size, "%s%s Z((%.8g,%.8g,%.8g),(%.8g,%.8g,%.8g))", srid, boxtype,
				box->xmin, box->ymin, box->zmin, 
				box->xmax, box->ymax, box->zmax);
		else if (MOBDB_FLAGS_GET_T(box->flags))
			snprintf(str, size, "%s%s T((%.8g,%.8g,%s),(%.8g,%.8g,%s))", srid, boxtype,
				box->xmin, box->ymin, strtmin, box->xmax, box->ymax, strtmax);
		else 
			snprintf(str, size, "%s%s((%.8g,%.8g),(%.8g,%.8g))", srid, boxtype,
				box->xmin, box->ymin, box->xmax, box->ymax);
	}
	else
		/* Missing spatial dimension */
		snprintf(str, size, "%s%s T((,,%s),(,,%s))", srid, boxtype, strtmin, strtmax);
	if (MOBDB_FLAGS_GET_T(box->flags))
	{
		pfree(strtmin);
		pfree(strtmax);
	}
	return str;
}

/* 
 * Output function. 
 */
PG_FUNCTION_INFO_V1(stbox_out);

PGDLLEXPORT Datum
stbox_out(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	char *result = stbox_to_string(box);
	PG_RETURN_CSTRING(result);
}

/*****************************************************************************
 * Constructor functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(stbox_constructor);

PGDLLEXPORT Datum
stbox_constructor(PG_FUNCTION_ARGS)
{
	assert(PG_NARGS() == 3 || PG_NARGS() == 5 || PG_NARGS() == 7 || PG_NARGS() == 9);
	double xmin = 0, xmax = 0, ymin = 0, ymax = 0, /* keep compiler quiet */
		zmin, zmax, tmp;
	TimestampTz tmin, tmax, ttmp;
	bool hasx = false, hasz = false, hast = false;
	int srid;

	if (PG_NARGS() == 3)
	{
		tmin = PG_GETARG_TIMESTAMPTZ(0);
		tmax = PG_GETARG_TIMESTAMPTZ(1);
		srid = PG_GETARG_INT32(2);
		hast = true;
	}
	else if (PG_NARGS() == 5)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		xmax = PG_GETARG_FLOAT8(2);
		ymax = PG_GETARG_FLOAT8(3);
		srid = PG_GETARG_INT32(4);
		hasx = true;
	}
	else if (PG_NARGS() == 7)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		xmax = PG_GETARG_FLOAT8(3);
		ymax = PG_GETARG_FLOAT8(4);
		zmax = PG_GETARG_FLOAT8(5);
		srid = PG_GETARG_INT32(6);
		hasx = hasz = true;
	}
	else /* PG_NARGS() == 9 */
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		tmin = PG_GETARG_TIMESTAMPTZ(3);
		xmax = PG_GETARG_FLOAT8(4);
		ymax = PG_GETARG_FLOAT8(5);
		zmax = PG_GETARG_FLOAT8(6);
		tmax = PG_GETARG_TIMESTAMPTZ(7);
		srid = PG_GETARG_INT32(8);
		hasx = hasz = hast = true;
	}

	STBOX *result = stbox_new(hasx, hasz, hast, false);
	result->srid = srid;
	/* Process X min/max */
	if (hasx)
	{
		if (xmin > xmax)
		{
			tmp = xmin;
			xmin = xmax;
			xmax = tmp;
		}
		result->xmin = xmin;
		result->xmax = xmax;

		/* Process Y min/max */
		if (ymin > ymax)
		{
			tmp = ymin;
			ymin = ymax;
			ymax = tmp;
		}
		result->ymin = ymin;
		result->ymax = ymax;

		/* Process Z min/max */
		if (hasz)
		{
			if (zmin > zmax)
			{
				tmp = zmin;
				zmin = zmax;
				zmax = tmp;
			}
			result->zmin = zmin;
			result->zmax = zmax;
		}
	}

	/* Process T min/max */
	if (hast)
	{
		if (tmin > tmax)
		{
			ttmp = tmin;
			tmin = tmax;
			tmax = ttmp;
		}
		result->tmin = tmin;
		result->tmax = tmax;
	}

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(stboxt_constructor);

PGDLLEXPORT Datum
stboxt_constructor(PG_FUNCTION_ARGS)
{
	double xmin, xmax, ymin, ymax, tmp;
	TimestampTz tmin, tmax, ttmp;
	int srid;

	xmin = PG_GETARG_FLOAT8(0);
	ymin = PG_GETARG_FLOAT8(1);
	tmin = PG_GETARG_TIMESTAMPTZ(2);
	xmax = PG_GETARG_FLOAT8(3);
	ymax = PG_GETARG_FLOAT8(4);
	tmax = PG_GETARG_TIMESTAMPTZ(5);
	srid = PG_GETARG_INT32(6);

	STBOX *result = stbox_new(true, false, true, false);
	result->srid = srid;
	/* Process X min/max */
	if (xmin > xmax)
	{
		tmp = xmin;
		xmin = xmax;
		xmax = tmp;
	}
	result->xmin = xmin;
	result->xmax = xmax;

	/* Process Y min/max */
	if (ymin > ymax)
	{
		tmp = ymin;
		ymin = ymax;
		ymax = tmp;
	}
	result->ymin = ymin;
	result->ymax = ymax;

	/* Process T min/max */
	if (tmin > tmax)
	{
		ttmp = tmin;
		tmin = tmax;
		tmax = ttmp;
	}
	result->tmin = tmin;
	result->tmax = tmax;

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(geodstbox_constructor);

PGDLLEXPORT Datum
geodstbox_constructor(PG_FUNCTION_ARGS)
{
	double xmin, xmax, ymin, ymax, zmin, zmax, tmp;
	TimestampTz tmin, tmax, ttmp;
	bool hasx = false, hasz = false, hast = false;
	int srid;

	assert(PG_NARGS() == 3 || PG_NARGS() == 7 || PG_NARGS() == 9);
	if (PG_NARGS() == 3)
	{
		tmin = PG_GETARG_TIMESTAMPTZ(0);
		tmax = PG_GETARG_TIMESTAMPTZ(1);
		srid = PG_GETARG_INT32(2);
		hast = true;
	}
	else if (PG_NARGS() == 7)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		xmax = PG_GETARG_FLOAT8(3);
		ymax = PG_GETARG_FLOAT8(4);
		zmax = PG_GETARG_FLOAT8(5);
		srid = PG_GETARG_INT32(6);
		hasx = hasz = true;
	}
	else /* PG_NARGS() == 9) */
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		tmin = PG_GETARG_TIMESTAMPTZ(3);
		xmax = PG_GETARG_FLOAT8(4);
		ymax = PG_GETARG_FLOAT8(5);
		zmax = PG_GETARG_FLOAT8(6);
		tmax = PG_GETARG_TIMESTAMPTZ(7);
		srid = PG_GETARG_INT32(8);
		hasx = hasz = hast = true;
	}

	STBOX *result = stbox_new(hasx, hasz, hast, true);
	result->srid = srid;
	/* Process X min/max */
	if (hasx)
	{
		if (xmin > xmax)
		{
			tmp = xmin;
			xmin = xmax;
			xmax = tmp;
		}
		result->xmin = xmin;
		result->xmax = xmax;

		/* Process Y min/max */
		if (ymin > ymax)
		{
			tmp = ymin;
			ymin = ymax;
			ymax = tmp;
		}
		result->ymin = ymin;
		result->ymax = ymax;

		/* Process Z min/max */
		if (zmin > zmax)
		{
			tmp = zmin;
			zmin = zmax;
			zmax = tmp;
		}
		result->zmin = zmin;
		result->zmax = zmax;
	}

	if (hast)
	{
		/* Process T min/max */
		if (tmin > tmax)
		{
			ttmp = tmin;
			tmin = tmax;
			tmax = ttmp;
		}
		result->tmin = tmin;
		result->tmax = tmax;
	}

	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Accessor functions
 *****************************************************************************/

/* Get the minimum X of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_xmin);

PGDLLEXPORT Datum
stbox_xmin(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_X(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_FLOAT8(box->xmin);
}

/* Get the maximum X of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_xmax);

PGDLLEXPORT Datum
stbox_xmax(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_X(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_FLOAT8(box->xmax);
}

/* Get the minimum Y of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_ymin);

PGDLLEXPORT Datum
stbox_ymin(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_X(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_FLOAT8(box->ymin);
}

/* Get the maximum Y of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_ymax);

PGDLLEXPORT Datum
stbox_ymax(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_X(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_FLOAT8(box->ymax);
}

/* Get the minimum Z of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_zmin);

PGDLLEXPORT Datum
stbox_zmin(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_Z(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_FLOAT8(box->zmin);
}

/* Get the maximum Z of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_zmax);

PGDLLEXPORT Datum
stbox_zmax(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_Z(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_FLOAT8(box->zmax);
}

/* Get the minimum timestamp of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_tmin);

PGDLLEXPORT Datum
stbox_tmin(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_T(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_TIMESTAMPTZ(box->tmin);
}

/* Get the maximum timestamp of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_tmax);

PGDLLEXPORT Datum
stbox_tmax(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_T(box->flags))
		PG_RETURN_NULL();
	PG_RETURN_TIMESTAMPTZ(box->tmax);
}

/* Get the SRID of an STBOX value */

PG_FUNCTION_INFO_V1(stbox_srid);

PGDLLEXPORT Datum
stbox_srid(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	PG_RETURN_INT32(box->srid);
}

/*****************************************************************************
 * Casting
 *****************************************************************************/

/* Cast an STBOX as a period */

PG_FUNCTION_INFO_V1(stbox_to_period);

PGDLLEXPORT Datum
stbox_to_period(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_T(box->flags))
		PG_RETURN_NULL();
	Period *result = period_make(box->tmin, box->tmax, true, true);
	PG_RETURN_POINTER(result);
}

/* Cast an STBOX as a box2d */

PG_FUNCTION_INFO_V1(stbox_to_box2d);

PGDLLEXPORT Datum
stbox_to_box2d(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_X(box->flags))
		PG_RETURN_NULL();

	/* Initialize existing dimensions */
	GBOX *result = palloc0(sizeof(GBOX));
	result->xmin = box->xmin;
	result->xmax = box->xmax;
	result->ymin = box->ymin;
	result->ymax = box->ymax;
	/* Strip out higher dimensions */
	FLAGS_SET_Z(result->flags, 0);
	FLAGS_SET_M(result->flags, 0);
	PG_RETURN_POINTER(result);
}


/* Cast an STBOX as a box3d */

PG_FUNCTION_INFO_V1(stbox_to_box3d);

PGDLLEXPORT Datum
stbox_to_box3d(PG_FUNCTION_ARGS)
{
	STBOX *box = PG_GETARG_STBOX_P(0);
	if (!MOBDB_FLAGS_GET_X(box->flags))
		PG_RETURN_NULL();

	/* Initialize existing dimensions */
	BOX3D *result = palloc0(sizeof(BOX3D));
	result->xmin = box->xmin;
	result->xmax = box->xmax;
	result->ymin = box->ymin;
	result->ymax = box->ymax;
	if (MOBDB_FLAGS_GET_Z(box->flags))
	{
		result->zmin = box->zmin;
		result->zmax = box->zmax;
	}
	else
	{
		result->zmin = result->zmax = 0;
	}
	result->srid = box->srid;
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Operators
 *****************************************************************************/

/* Intersection of two boxes */

STBOX *
stbox_intersection_internal(const STBOX *box1, const STBOX *box2)
{
	bool hasx = MOBDB_FLAGS_GET_X(box1->flags) && MOBDB_FLAGS_GET_X(box2->flags);
	assert(! hasx || MOBDB_FLAGS_GET_GEODETIC(box1->flags) == MOBDB_FLAGS_GET_GEODETIC(box2->flags));
	bool hasz = MOBDB_FLAGS_GET_Z(box1->flags) && MOBDB_FLAGS_GET_Z(box2->flags);
	bool hast = MOBDB_FLAGS_GET_T(box1->flags) && MOBDB_FLAGS_GET_T(box2->flags);
	bool geodetic = MOBDB_FLAGS_GET_GEODETIC(box1->flags);
	/* If there is no common dimension */
	if ((! hasx && ! hast) ||
		/* If they do no intersect in one common dimension */
		(hasx && (box1->xmin > box2->xmax || box2->xmin > box1->xmax ||
			box1->ymin > box2->ymax || box2->ymin > box1->ymax)) ||
		(hasz && (box1->zmin > box2->zmax || box2->zmin > box1->zmax)) ||
		(hast && (box1->tmin > box2->tmax || box2->tmin > box1->tmax)))
		return(NULL);

	STBOX *result = stbox_new(hasx, hasz, hast, geodetic);
	if (hasx)
	{
		result->xmin = Max(box1->xmin, box2->xmin);
		result->xmax = Min(box1->xmax, box2->xmax);
		result->ymin = Max(box1->ymin, box2->ymin);
		result->ymax = Min(box1->ymax, box2->ymax);
		if (hasz)
			{
			result->zmin = Max(box1->zmin, box2->zmin);
			result->zmax = Min(box1->zmax, box2->zmax);
			}
	}
	if (hast)
	{
		result->tmin = Max(box1->tmin, box2->tmin);
		result->tmax = Min(box1->tmax, box2->tmax);
	}
	return(result);
}

PG_FUNCTION_INFO_V1(stbox_intersection);

PGDLLEXPORT Datum
stbox_intersection(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	ensure_same_geodetic_stbox(box1, box2);
	ensure_same_srid_stbox(box1, box2);
	STBOX *result = stbox_intersection_internal(box1, box2);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Comparison functions
 *****************************************************************************/

/*
 * Compare two boxes
 */
int 
stbox_cmp_internal(const STBOX *box1, const STBOX *box2)
{
	/* Compare the box minima */
	if (MOBDB_FLAGS_GET_X(box1->flags) && MOBDB_FLAGS_GET_X(box2->flags))
	{
		if (box1->xmin < box2->xmin)
			return -1;
		if (box1->xmin > box2->xmin)
			return 1;
		if (box1->ymin < box2->ymin)
			return -1;
		if (box1->ymin > box2->ymin)
			return 1;
	}
	if (MOBDB_FLAGS_GET_Z(box1->flags) && MOBDB_FLAGS_GET_Z(box2->flags))
	{
		if (box1->zmin < box2->zmin)
			return -1;
		if (box1->zmin > box2->zmin)
			return 1;
	}
	if (MOBDB_FLAGS_GET_T(box1->flags) && MOBDB_FLAGS_GET_T(box2->flags))
	{
		if (box1->tmin < box2->tmin)
			return -1;
		if (box1->tmin > box2->tmin)
			return 1;
	}
	/* Compare the box maxima */
	if (MOBDB_FLAGS_GET_X(box1->flags) && MOBDB_FLAGS_GET_X(box2->flags))
	{
		if (box1->xmax < box2->xmax)
			return -1;
		if (box1->xmax > box2->xmax)
			return 1;
		if (box1->ymax < box2->ymax)
			return -1;
		if (box1->ymax > box2->ymax)
			return 1;
	}
	if (MOBDB_FLAGS_GET_Z(box1->flags) && MOBDB_FLAGS_GET_Z(box2->flags))
	{
		if (box1->zmax < box2->zmax)
			return -1;
		if (box1->zmax > box2->zmax)
			return 1;
	}
	if (MOBDB_FLAGS_GET_T(box1->flags) && MOBDB_FLAGS_GET_T(box2->flags))
	{
		if (box1->tmax < box2->tmax)
			return -1;
		if (box1->tmax > box2->tmax)
			return 1;
	}
	/* Finally compare the flags */
	if (box1->flags < box2->flags)
		return -1;
	if (box1->flags > box2->flags)
		return 1;
	/* The two boxes are equal */
	return 0;
}

PG_FUNCTION_INFO_V1(stbox_cmp);

PGDLLEXPORT Datum
stbox_cmp(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	int	cmp = stbox_cmp_internal(box1, box2);
	PG_RETURN_INT32(cmp);
}

PG_FUNCTION_INFO_V1(stbox_lt);

PGDLLEXPORT Datum
stbox_lt(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	int	cmp = stbox_cmp_internal(box1, box2);
	PG_RETURN_BOOL(cmp < 0);
}

PG_FUNCTION_INFO_V1(stbox_le);

PGDLLEXPORT Datum
stbox_le(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	int	cmp = stbox_cmp_internal(box1, box2);
	PG_RETURN_BOOL(cmp <= 0);
}

PG_FUNCTION_INFO_V1(stbox_ge);

PGDLLEXPORT Datum
stbox_ge(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	int	cmp = stbox_cmp_internal(box1, box2);
	PG_RETURN_BOOL(cmp >= 0);
}

PG_FUNCTION_INFO_V1(stbox_gt);

PGDLLEXPORT Datum
stbox_gt(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	int	cmp = stbox_cmp_internal(box1, box2);
	PG_RETURN_BOOL(cmp > 0);
}

/*
 * Equality and inequality of two boxes
 */
bool
stbox_eq_internal(const STBOX *box1, const STBOX *box2)
{
	if (box1->xmin != box2->xmin || box1->ymin != box2->ymin ||
		box1->zmin != box2->zmin || box1->tmin != box2->tmin ||
		box1->xmax != box2->xmax || box1->ymax != box2->ymax ||
		box1->zmax != box2->zmax || box1->tmax != box2->tmax ||
		box1->flags != box2->flags )
		return false;
	/* The two boxes are equal */
	return true;
}

PG_FUNCTION_INFO_V1(stbox_eq);

PGDLLEXPORT Datum
stbox_eq(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	PG_RETURN_BOOL(stbox_eq_internal(box1, box2));
}

PG_FUNCTION_INFO_V1(stbox_ne);

PGDLLEXPORT Datum
stbox_ne(PG_FUNCTION_ARGS)
{
	STBOX *box1 = PG_GETARG_STBOX_P(0);
	STBOX *box2 = PG_GETARG_STBOX_P(1);
	PG_RETURN_BOOL(! stbox_eq_internal(box1, box2));
}

/*****************************************************************************/

