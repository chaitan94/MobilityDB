/*****************************************************************************
 *
 * stbox.c
 *	  Basic functions for STBOX bounding box.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
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

/* Buffer size for input and  of STBOX */
#define MAXSTBOXLEN		256

/*****************************************************************************
 * Miscellaneus functions
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

void
stbox_to_period(Period *period, const STBOX *box)
{
	assert(MOBDB_FLAGS_GET_T(box->flags));
	period_set(period, (TimestampTz) box->tmin, (TimestampTz) box->tmin, true, true);
	return;
}


/*****************************************************************************
 * Input/ functions
 *****************************************************************************/

/* 
 * Input function. 
 * Examples of input:
 * 		STBOX((1.0, 2.0), (1.0, 2.0)) -> only spatial
 * 		STBOX Z((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)) -> only spatial
 * 		STBOX T((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)) -> spatiotemporal
 * 		STBOX ZT((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0)) -> spatiotemporal
 * 		GEODSTBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0)) -> only spatial
 * 		GEODSTBOX T((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0)) -> spatiotemporal
 * where the commas are optional
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
	static int sz = MAXSTBOXLEN + 1;
	char *str = NULL, *strtmin = NULL, *strtmax = NULL;
	str = (char *)palloc(sz);
	assert(MOBDB_FLAGS_GET_X(box->flags) || MOBDB_FLAGS_GET_T(box->flags));
	if (MOBDB_FLAGS_GET_T(box->flags))
	{
		strtmin = call_output(TIMESTAMPTZOID, box->tmin);
		strtmax = call_output(TIMESTAMPTZOID, box->tmax);
	}
	if (MOBDB_FLAGS_GET_X(box->flags))
	{
		if (MOBDB_FLAGS_GET_GEODETIC(box->flags))
		{
			if (MOBDB_FLAGS_GET_T(box->flags))
				snprintf(str, sz, "GEODSTBOX T((%.8g,%.8g,%.8g,%s),(%.8g,%.8g,%.8g,%s))",
					box->xmin, box->ymin, box->zmin, strtmin, 
					box->xmax, box->ymax, box->zmax, strtmax);
			else
				snprintf(str, sz, "GEODSTBOX((%.8g,%.8g,%.8g),(%.8g,%.8g,%.8g))", 
					box->xmin, box->ymin, box->zmin, 
					box->xmax, box->ymax, box->zmax);
		}
		else if (MOBDB_FLAGS_GET_Z(box->flags) && MOBDB_FLAGS_GET_T(box->flags))
			snprintf(str, sz, "STBOX ZT((%.8g,%.8g,%.8g,%s),(%.8g,%.8g,%.8g,%s))",
				box->xmin, box->ymin, box->zmin, strtmin, 
				box->xmax, box->ymax, box->zmax, strtmax);
		else if (MOBDB_FLAGS_GET_Z(box->flags))
			snprintf(str, sz, "STBOX Z((%.8g,%.8g,%.8g),(%.8g,%.8g,%.8g))", 
				box->xmin, box->ymin, box->zmin, 
				box->xmax, box->ymax, box->zmax);
		else if (MOBDB_FLAGS_GET_T(box->flags))
			snprintf(str, sz, "STBOX T((%.8g,%.8g,%s),(%.8g,%.8g,%s))", 
				box->xmin, box->ymin, strtmin, box->xmax, box->ymax, strtmax);
		else 
			snprintf(str, sz, "STBOX((%.8g,%.8g),(%.8g,%.8g))", 
				box->xmin, box->ymin, box->xmax, box->ymax);
	}
	else
		/* Missing spatial dimension */
		snprintf(str, sz, "STBOX T((,,%s),(,,%s))", 
			strtmin, strtmax);
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
	assert(PG_NARGS() == 2 || PG_NARGS() == 4 || 
		PG_NARGS() == 6 || PG_NARGS() == 8);
	double xmin = 0, xmax = 0, ymin = 0, ymax = 0, /* keep compiler quiet */
		zmin, zmax, tmp;
	TimestampTz tmin, tmax, ttmp;
	bool hasx = false, hasz = false, hast = false;

	if (PG_NARGS() == 2)
	{
		tmin = PG_GETARG_TIMESTAMPTZ(0);
		tmax = PG_GETARG_TIMESTAMPTZ(1);
		hast = true;
	}
	if (PG_NARGS() == 4)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		xmax = PG_GETARG_FLOAT8(2);
		ymax = PG_GETARG_FLOAT8(3);
		hasx = true;
	}
	else if (PG_NARGS() == 6)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		xmax = PG_GETARG_FLOAT8(3);
		ymax = PG_GETARG_FLOAT8(4);
		zmax = PG_GETARG_FLOAT8(5);
		hasx = hasz = true;
	}
	else if (PG_NARGS() == 8)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		tmin = PG_GETARG_TIMESTAMPTZ(3);
		xmax = PG_GETARG_FLOAT8(4);
		ymax = PG_GETARG_FLOAT8(5);
		zmax = PG_GETARG_FLOAT8(6);
		tmax = PG_GETARG_TIMESTAMPTZ(7);
		hasx = hasz = hast = true;
	}

	STBOX *result = stbox_new(hasx, hasz, hast, false);
	
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

	xmin = PG_GETARG_FLOAT8(0);
	ymin = PG_GETARG_FLOAT8(1);
	tmin = PG_GETARG_TIMESTAMPTZ(2);
	xmax = PG_GETARG_FLOAT8(3);
	ymax = PG_GETARG_FLOAT8(4);
	tmax = PG_GETARG_TIMESTAMPTZ(5);

	STBOX *result = stbox_new(true, false, true, false);
	
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

	/* Process M min/max */
	if (tmin > tmax)
	{
		ttmp = tmin;
		tmin = tmax;
		tmax = ttmp;
	}
	result->tmin = tmin;
	result->tmax = tmax;
	MOBDB_FLAGS_SET_T(result->flags, true);

	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(geodstbox_constructor);

PGDLLEXPORT Datum
geodstbox_constructor(PG_FUNCTION_ARGS)
{
	double xmin = 0, xmax = 0, ymin = 0, ymax = 0, 
		zmin = 0, zmax = 0, tmp; /* keep compiler quiet */
	TimestampTz tmin, tmax, ttmp;
	bool hast = false;

	assert(PG_NARGS() == 6 || PG_NARGS() == 8);
	if (PG_NARGS() == 6)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		xmax = PG_GETARG_FLOAT8(3);
		ymax = PG_GETARG_FLOAT8(4);
		zmax = PG_GETARG_FLOAT8(5);
	}
	else if (PG_NARGS() == 8)
	{
		xmin = PG_GETARG_FLOAT8(0);
		ymin = PG_GETARG_FLOAT8(1);
		zmin = PG_GETARG_FLOAT8(2);
		tmin = PG_GETARG_TIMESTAMPTZ(3);
		xmax = PG_GETARG_FLOAT8(4);
		ymax = PG_GETARG_FLOAT8(5);
		zmax = PG_GETARG_FLOAT8(6);
		tmax = PG_GETARG_TIMESTAMPTZ(7);
		hast = true;
	}

	STBOX *result = stbox_new(true, true, hast, true);

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

	/* Process Z min/max */
	if (zmin > zmax)
	{
		tmp = zmin;
		zmin = zmax;
		zmax = tmp;
	}
	result->zmin = zmin;
	result->zmax = zmax;
	MOBDB_FLAGS_SET_Z(result->flags, true);

	if (hast)
	{
		/* Process M min/max */
		if ( tmin > tmax )
		{
			ttmp = tmin;
			tmin = tmax;
			tmax = ttmp;
		}
		result->tmin = tmin;
		result->tmax = tmax;
		MOBDB_FLAGS_SET_T(result->flags, true);
	}


	PG_RETURN_POINTER(result);
}

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
		box1->zmax != box2->zmax || box1->tmax != box2->tmax)
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

