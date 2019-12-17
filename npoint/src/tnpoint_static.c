/*****************************************************************************
 *
 * tnpoint_static.c
 *	  Network-based static point and segment types
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_static.h"

#include <assert.h>
#include <libpq/pqformat.h>
#include <executor/spi.h>
#include <liblwgeom.h>

#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "tpoint_out.h"
#include "tnpoint.h"
#include "tnpoint_parser.h"

/*****************************************************************************
 * Miscellaneous functions
 *****************************************************************************/

ArrayType *
int64arr_to_array(int64 *int64arr, int count)
{
	return construct_array((Datum *)int64arr, count, INT8OID, 8, true, 'd');
}

ArrayType *
npointarr_to_array(npoint **npointarr, int count)
{
	return construct_array((Datum *)npointarr, count, type_oid(T_NPOINT), 
		sizeof(npoint), false, 'd');
}

ArrayType *
nsegmentarr_to_array(nsegment **nsegmentarr, int count)
{
	return construct_array((Datum *)nsegmentarr, count, type_oid(T_NSEGMENT), 
		sizeof(nsegment), false, 'd');
}

/*****************************************************************************/

static int
nsegment_sort_cmp(nsegment **l, nsegment **r)
{
	return nsegment_cmp_internal(*l, *r);
}

void
nsegmentarr_sort(nsegment **segments, int count)
{
	qsort(segments, count, sizeof(nsegment *),
		  (qsort_comparator) &nsegment_sort_cmp);
}

nsegment **
nsegmentarr_normalize(nsegment **segments, int *count)
{
	assert(*count != 0);
	nsegmentarr_sort(segments, *count);
	int newcount = 0;
	nsegment **result = palloc(sizeof(nsegment *) * *count);
	nsegment *current = segments[0];
	for (int i = 1; i < *count; i++)
	{
		nsegment *seg = segments[i];
		if (current->rid == seg->rid) 
		{
			current->pos1 = Min(current->pos1, seg->pos1);
			current->pos2 = Max(current->pos2, seg->pos2);
			pfree(seg);
		} 
		else 
		{
			result[newcount++] = current;
			current = seg;
		}
	}
	result[newcount++] = current;
	*count = newcount;
	return result;
}

/*****************************************************************************
 * Input/Output functions for npoint
 *****************************************************************************/

/*
 * Input function.
 * Example of input:
 * 		(1, 0.5)
 */
PG_FUNCTION_INFO_V1(npoint_in);

PGDLLEXPORT Datum
npoint_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
	npoint *result = npoint_parse(&str);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/* Output function */

PG_FUNCTION_INFO_V1(npoint_out);

PGDLLEXPORT Datum
npoint_out(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	char *result = psprintf("NPoint(%ld,%g)", np->rid, np->pos);
	PG_RETURN_CSTRING(result);
}

/* Receive function */

PG_FUNCTION_INFO_V1(npoint_recv);

PGDLLEXPORT Datum
npoint_recv(PG_FUNCTION_ARGS)
{
	StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
	npoint *result;

	result = (npoint *) palloc(sizeof(npoint));
	result->rid = pq_getmsgint64(buf);
	result->pos = pq_getmsgfloat8(buf);
	PG_RETURN_POINTER(result);
}

/* Send function */

PG_FUNCTION_INFO_V1(npoint_send);

PGDLLEXPORT Datum
npoint_send(PG_FUNCTION_ARGS)
{
	npoint	*np = PG_GETARG_NPOINT(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, np->rid);
	pq_sendfloat8(&buf, np->pos);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * Input/Output functions for nsegment
 *****************************************************************************/

/*
 * Input function.
 * Example of input:
 * 		(1, 0.5, 0.6)
 */
PG_FUNCTION_INFO_V1(nsegment_in);

PGDLLEXPORT Datum
nsegment_in(PG_FUNCTION_ARGS)
{
	char *str = PG_GETARG_CSTRING(0);
	nsegment *result = nsegment_parse(&str);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/* Output function */

PG_FUNCTION_INFO_V1(nsegment_out);

PGDLLEXPORT Datum
nsegment_out(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	char *result = psprintf("NSegment(%ld,%g,%g)", ns->rid, ns->pos1, ns->pos2);
	PG_RETURN_CSTRING(result);
}

/* Receive function */

PG_FUNCTION_INFO_V1(nsegment_recv);

PGDLLEXPORT Datum
nsegment_recv(PG_FUNCTION_ARGS)
{
	StringInfo	buf = (StringInfo) PG_GETARG_POINTER(0);
	nsegment *result;

	result = (nsegment *) palloc(sizeof(nsegment));
	result->rid = pq_getmsgint64(buf);
	result->pos1 = pq_getmsgfloat8(buf);
	result->pos2 = pq_getmsgfloat8(buf);
	PG_RETURN_POINTER(result);
}

/* Send function */

PG_FUNCTION_INFO_V1(nsegment_send);

PGDLLEXPORT Datum
nsegment_send(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	StringInfoData buf;

	pq_begintypsend(&buf);
	pq_sendint64(&buf, ns->rid);
	pq_sendfloat8(&buf, ns->pos1);
	pq_sendfloat8(&buf, ns->pos2);
	PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * Constructors
 *****************************************************************************/

/*
 * Set an npoint value from arguments
 */
void
npoint_set(npoint *np, int64 rid, double pos)
{
	if (!route_exists(rid))
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("there is no route with gid value %lu in table ways", rid)));
	if (pos < 0 || pos > 1)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("the relative position must be a real number between 0 and 1")));

	np->rid = rid;
	np->pos = pos;
	return;
}

/*
 * Construct an npoint value from arguments
 */
npoint *
npoint_make(int64 rid, double pos)
{
	if (!route_exists(rid))
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("there is no route with gid value %lu in table ways", rid)));
	if (pos < 0 || pos > 1)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("the relative position must be a real number between 0 and 1")));

	npoint *result = (npoint *)palloc(sizeof(npoint));
	result->rid = rid;
	result->pos = pos;
	return result;
}

PG_FUNCTION_INFO_V1(npoint_constructor);

PGDLLEXPORT Datum
npoint_constructor(PG_FUNCTION_ARGS)
{
	int64 rid = PG_GETARG_INT64(0);
	double pos = PG_GETARG_FLOAT8(1);
	npoint *result = npoint_make(rid, pos);
	PG_RETURN_POINTER(result);
}


/*
 * Set an nsegment value from arguments
 */
void
nsegment_set(nsegment *ns, int64 rid, double pos1, double pos2)
{
	if (!route_exists(rid))
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("there is no route with gid value %lu in table ways", rid)));
	if (pos1 < 0 || pos1 > 1 || pos2 < 0 || pos2 > 1)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("The relative position of a network segment must be a real number between 0 and 1")));

	ns->rid = rid;
	ns->pos1 = Min(pos1, pos2);
	ns->pos2 = Max(pos1, pos2);
	return;
}

/*
 * Construct an nsegment value from arguments
 */
nsegment *
nsegment_make(int64 rid, double pos1, double pos2)
{
	if (!route_exists(rid))
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("there is no route with gid value %lu in table ways", rid)));
	if (pos1 < 0 || pos1 > 1 || pos2 < 0 || pos2 > 1)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("The relative position of a network segment must be a real number between 0 and 1")));

	nsegment *result = (nsegment *)palloc(sizeof(nsegment));
	result->rid = rid;
	result->pos1 = Min(pos1, pos2);
	result->pos2 = Max(pos1, pos2);
	return result;
}

PG_FUNCTION_INFO_V1(nsegment_constructor);

PGDLLEXPORT Datum
nsegment_constructor(PG_FUNCTION_ARGS)
{
	int64 rid = PG_GETARG_INT64(0);
	double pos1 = PG_GETARG_FLOAT8(1);
	double pos2 = PG_GETARG_FLOAT8(2);
	nsegment *result = nsegment_make(rid, pos1, pos2);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(nsegment_from_npoint);

PGDLLEXPORT Datum
nsegment_from_npoint(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	nsegment *result = nsegment_make(np->rid, np->pos, np->pos);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Accessing values
 *****************************************************************************/

PG_FUNCTION_INFO_V1(npoint_route);

PGDLLEXPORT Datum
npoint_route(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	int64 result = np->rid;
	PG_RETURN_INT64(result);
}

PG_FUNCTION_INFO_V1(npoint_position);

PGDLLEXPORT Datum
npoint_position(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	PG_RETURN_FLOAT8(np->pos);
}

PG_FUNCTION_INFO_V1(nsegment_route);

PGDLLEXPORT Datum
nsegment_route(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	PG_RETURN_INT64(ns->rid);
}

PG_FUNCTION_INFO_V1(nsegment_start_position);

PGDLLEXPORT Datum
nsegment_start_position(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	PG_RETURN_FLOAT8(ns->pos1);
}

PG_FUNCTION_INFO_V1(nsegment_end_position);

PGDLLEXPORT Datum
nsegment_end_position(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	PG_RETURN_FLOAT8(ns->pos2);
}

/*****************************************************************************
 * Functions for defining B-tree index
 *****************************************************************************/

/* equality  */
bool
npoint_eq_internal(npoint *np1, npoint *np2)
{
	return np1->rid == np2->rid && fabs(np1->pos - np2->pos) < EPSILON;
}

PG_FUNCTION_INFO_V1(npoint_eq);

PGDLLEXPORT Datum
npoint_eq(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	PG_RETURN_BOOL(npoint_eq_internal(np1, np2));
}

/* inequality */
bool
npoint_ne_internal(npoint *np1, npoint *np2)
{
	return (!npoint_eq_internal(np1, np2));
}

PG_FUNCTION_INFO_V1(npoint_ne);

PGDLLEXPORT Datum
npoint_ne(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	PG_RETURN_BOOL(npoint_ne_internal(np1, np2));
}

/* btree comparator */
int
npoint_cmp_internal(npoint *np1, npoint *np2)
{
	if (np1->rid < np2->rid)
		return -1;
	else if (np1->rid > np2->rid)
		return 1;
	/* Both rid are equal */
	else if(np1->pos < np2->pos)
		return -1;
	else if (np1->pos > np2->pos)
		return 1;
	return 0;
}

PG_FUNCTION_INFO_V1(npoint_cmp);

PGDLLEXPORT Datum
npoint_cmp(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	PG_RETURN_INT32(npoint_cmp_internal(np1, np2));	
}

/* inequality operators using the npoint_cmp function */
bool
npoint_lt_internal(npoint *np1, npoint *np2)
{
	int	cmp = npoint_cmp_internal(np1, np2);
	return (cmp < 0);
}

PG_FUNCTION_INFO_V1(npoint_lt);

PGDLLEXPORT Datum
npoint_lt(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	int	cmp = npoint_cmp_internal(np1, np2);
	PG_RETURN_BOOL(cmp < 0);
}

bool
npoint_le_internal(npoint *np1, npoint *np2)
{
	int	cmp = npoint_cmp_internal(np1, np2);
	return (cmp <= 0);
}

PG_FUNCTION_INFO_V1(npoint_le);

PGDLLEXPORT Datum
npoint_le(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	int	cmp = npoint_cmp_internal(np1, np2);
	PG_RETURN_BOOL(cmp <= 0);
}

bool
npoint_ge_internal(npoint *np1, npoint *np2)
{
	int	cmp = npoint_cmp_internal(np1, np2);
	return (cmp >= 0);
}

PG_FUNCTION_INFO_V1(npoint_ge);

PGDLLEXPORT Datum
npoint_ge(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	int	cmp = npoint_cmp_internal(np1, np2);
	PG_RETURN_BOOL(cmp >= 0);
}

bool
npoint_gt_internal(npoint *np1, npoint *np2)
{
	int	cmp = npoint_cmp_internal(np1, np2);
	return (cmp > 0);
}

PG_FUNCTION_INFO_V1(npoint_gt);

PGDLLEXPORT Datum
npoint_gt(PG_FUNCTION_ARGS)
{
	npoint *np1 = PG_GETARG_NPOINT(0);
	npoint *np2 = PG_GETARG_NPOINT(1);
	int	cmp = npoint_cmp_internal(np1, np2);
	PG_RETURN_BOOL(cmp > 0);
}

/*****************************************************************************/

/* equality  */
bool
nsegment_eq_internal(nsegment *ns1, nsegment *ns2)
{
	return ns1->rid == ns2->rid && fabs(ns1->pos1 - ns2->pos1) < EPSILON &&
		fabs(ns1->pos2 - ns2->pos2) < EPSILON;
}

PG_FUNCTION_INFO_V1(nsegment_eq);

PGDLLEXPORT Datum
nsegment_eq(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	PG_RETURN_BOOL(nsegment_eq_internal(ns1, ns2));
}

/* inequality */
bool
nsegment_ne_internal(nsegment *ns1, nsegment *ns2)
{
	return (!nsegment_eq_internal(ns1, ns2));
}

PG_FUNCTION_INFO_V1(nsegment_ne);

PGDLLEXPORT Datum
nsegment_ne(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	PG_RETURN_BOOL(nsegment_ne_internal(ns1, ns2));
}

/* btree comparator */
int
nsegment_cmp_internal(nsegment *ns1, nsegment *ns2)
{
	if (ns1->rid < ns2->rid)
		return -1;
	else if (ns1->rid > ns2->rid)
		return 1;
	/* Both rid are equal */
	else if(ns1->pos1 < ns2->pos1)
		return -1;
	else if (ns1->pos1 > ns2->pos1)
		return 1;
	/* Both pos1 are equal */
	else if(ns1->pos2 < ns2->pos2)
		return -1;
	else if (ns1->pos2 > ns2->pos2)
		return 1;
	return 0;
}

PG_FUNCTION_INFO_V1(nsegment_cmp);

PGDLLEXPORT Datum
nsegment_cmp(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	PG_RETURN_INT32(nsegment_cmp_internal(ns1, ns2));	
}

/* inequality operators using the nsegment_cmp function */
bool
nsegment_lt_internal(nsegment *ns1, nsegment *ns2)
{
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	return (cmp < 0);
}

PG_FUNCTION_INFO_V1(nsegment_lt);

PGDLLEXPORT Datum
nsegment_lt(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	PG_RETURN_BOOL(cmp < 0);
}

bool
nsegment_le_internal(nsegment *ns1, nsegment *ns2)
{
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	return (cmp <= 0);
}

PG_FUNCTION_INFO_V1(nsegment_le);

PGDLLEXPORT Datum
nsegment_le(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	PG_RETURN_BOOL(cmp <= 0);
}

bool
nsegment_ge_internal(nsegment *ns1, nsegment *ns2)
{
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	return (cmp >= 0);
}

PG_FUNCTION_INFO_V1(nsegment_ge);

PGDLLEXPORT Datum
nsegment_ge(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	PG_RETURN_BOOL(cmp >= 0);
}

bool
nsegment_gt_internal(nsegment *ns1, nsegment *ns2)
{
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	return (cmp > 0);
}

PG_FUNCTION_INFO_V1(nsegment_gt);

PGDLLEXPORT Datum
nsegment_gt(PG_FUNCTION_ARGS)
{
	nsegment *ns1 = PG_GETARG_NSEGMENT(0);
	nsegment *ns2 = PG_GETARG_NSEGMENT(1);
	int	cmp = nsegment_cmp_internal(ns1, ns2);
	PG_RETURN_BOOL(cmp > 0);
}

/*****************************************************************************
 * Conversions between network and Euclidean space
 *****************************************************************************/

/* Access edge table to get the route length from corresponding rid */

bool
route_exists(int64 rid)
{
	char sql[64];
	sprintf(sql, "SELECT true FROM public.ways WHERE gid = %ld", rid);
	bool isNull = true;
	bool result = false;
	SPI_connect();
	int ret = SPI_execute(sql, true, 1);
	uint64 proc = SPI_processed;
	if (ret > 0 && proc > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		result = DatumGetBool(SPI_getbinval(tuptable->vals[0], 
			tuptable->tupdesc, 1, &isNull));
	}
	SPI_finish();
	return result;
}

double
route_length(int64 rid)
{
	char sql[64];
	sprintf(sql, "SELECT length FROM public.ways WHERE gid = %ld", rid);
	bool isNull = true;
	double result = 0;
	SPI_connect();
	int ret = SPI_execute(sql, true, 1);
	uint64 proc = SPI_processed;
	if (ret > 0 && proc > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		result = DatumGetFloat8(SPI_getbinval(tuptable->vals[0], 
			tuptable->tupdesc, 1, &isNull));
	}
	SPI_finish();

	if (isNull)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("cannot get the length for route %ld", rid)));

	return result;
}

/* Access edge table to get the route geometry from corresponding rid */

Datum
route_geom(int64 rid)
{
	char sql[64];
	sprintf(sql, "SELECT the_geom FROM public.ways WHERE gid = %ld", rid);
	bool isNull = true;
	GSERIALIZED *result = NULL;
	SPI_connect();
	int ret = SPI_execute(sql, true, 1);
	uint64 proc = SPI_processed;
	if (ret > 0 && proc > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		Datum line = SPI_getbinval(tuptable->vals[0], tuptable->tupdesc, 1, &isNull);
		if (!isNull)
		{
			/* Must allocate this in upper executor context to keep it alive after SPI_finish() */
			GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(line);
			result = (GSERIALIZED *)SPI_palloc(gs->size);
			memcpy(result, gs, gs->size);
		}
	}
	SPI_finish();

	if (isNull)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("cannot get the geometry for route %ld", rid)));

	return PointerGetDatum(result);
}

/* Access edge table to get the rid from corresponding geometry */

int64
rid_from_geom(Datum geom)
{
	char *geomstr = ewkt_out(ANYOID, geom);
	char sql[128];
	sprintf(sql, "SELECT gid FROM ways WHERE ST_DWithin(the_geom, '%s', 1e-5) \
		ORDER BY ST_Distance(the_geom, '%s') LIMIT 1", geomstr, geomstr);
	pfree(geomstr);
	bool isNull = true;
	int64 result = 0; /* make compiler quiet */
	SPI_connect();
	int ret = SPI_execute(sql, true, 1);
	uint64 proc = SPI_processed;
	if (ret > 0 && proc > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		result = DatumGetInt64(SPI_getbinval(tuptable->vals[0], tuptable->tupdesc, 1, &isNull));
	}
	SPI_finish();
	if (isNull)
		ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("cannot get route identifier from geometry point")));

	return result;
}

/*****************************************************************************/

/* npoint as geometry */

Datum
npoint_as_geom_internal(npoint *np)
{
	Datum line = route_geom(np->rid);
	Datum result = call_function2(LWGEOM_line_interpolate_point, line, Float8GetDatum(np->pos));
	pfree(DatumGetPointer(line));
	return result;
}

PG_FUNCTION_INFO_V1(npoint_as_geom);

PGDLLEXPORT Datum
npoint_as_geom(PG_FUNCTION_ARGS)
{
	npoint *np = PG_GETARG_NPOINT(0);
	Datum result = npoint_as_geom_internal(np);
	PG_RETURN_DATUM(result);
}

/* geometry as npoint */

npoint *
geom_as_npoint_internal(Datum geom)
{
	char *geomstr = ewkt_out(ANYOID, geom);
	char sql[512];
	sprintf(sql, "SELECT npoint(gid, ST_LineLocatePoint(the_geom, '%s'))\
		FROM public.ways WHERE ST_DWithin(the_geom, '%s', 1e-5) \
		ORDER BY ST_Distance(the_geom, '%s') LIMIT 1", geomstr, geomstr, geomstr);
	pfree(geomstr);
	npoint *result = (npoint *)palloc(sizeof(npoint));
	bool isNull = true;
	SPI_connect();
	int ret = SPI_execute(sql, true, 1);
	uint64 proc = SPI_processed;
	if (ret > 0 && proc > 0 && SPI_tuptable != NULL)
	{
		SPITupleTable *tuptable = SPI_tuptable;
		Datum value = SPI_getbinval(tuptable->vals[0], tuptable->tupdesc, 1, &isNull);
		if (!isNull)
		{
			/* Must allocate this in upper executor context to keep it alive after SPI_finish() */
			npoint *np = DatumGetNpoint(value);
			memcpy(result, np, sizeof(npoint));
		}
	}
	SPI_finish();
	if (isNull)
	{
		pfree(result);
		return NULL;
	}
	return result;
}

PG_FUNCTION_INFO_V1(geom_as_npoint);

PGDLLEXPORT Datum
geom_as_npoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	if (gserialized_get_type(gs) != POINTTYPE ||
		gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), 
			errmsg("Only non-empty point geometries accepted")));
	}
	npoint *result = geom_as_npoint_internal(PointerGetDatum(gs));
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* nsegment to geometry */

Datum
nsegment_as_geom_internal(nsegment *ns)
{
	Datum line = route_geom(ns->rid);
	Datum result;
	if (fabs(ns->pos1 - ns->pos2) < EPSILON)
		result = call_function2(LWGEOM_line_interpolate_point, line, 
			Float8GetDatum(ns->pos1));
	else
		result = call_function3(LWGEOM_line_substring, line, 
			Float8GetDatum(ns->pos1), Float8GetDatum(ns->pos2));
	pfree(DatumGetPointer(line));
	return result;
}

PG_FUNCTION_INFO_V1(nsegment_as_geom);

PGDLLEXPORT Datum
nsegment_as_geom(PG_FUNCTION_ARGS)
{
	nsegment *ns = PG_GETARG_NSEGMENT(0);
	Datum result = nsegment_as_geom_internal(ns);
	PG_RETURN_DATUM(result);
}

nsegment *
geom_as_nsegment_internal(Datum line)
{
	int numpoints = DatumGetInt32(call_function1(LWGEOM_numpoints_linestring, line));
	npoint **points = palloc0(sizeof(npoint *) * numpoints);
	int k = 0;
	for (int i = 0; i < numpoints; i++)
	{
		/* The composing points are from 1 to numcount */
		Datum point = call_function2(LWGEOM_pointn_linestring, line, Int32GetDatum(i + 1));
		npoint *np = geom_as_npoint_internal(point);
		if (np != NULL)
			points[k++] = np;
		/* Cannot pfree(DatumGetPointer(point)); */
	}
	if (k == 0)
	{
		pfree(points);
		return NULL;
	}

	int64 rid = points[0]->rid;
	double minPos = points[0]->pos, maxPos = points[0]->pos;
	for (int i = 1; i < k; i++)
	{
		if (points[i]->rid != rid)
		{
			for (int j = 0; j < k; j++)
				pfree(points[j]);
			pfree(points);
			return NULL;
		}
        minPos = Min(minPos, points[i]->pos);
		maxPos = Max(maxPos, points[i]->pos);
	}
	nsegment *result = nsegment_make(rid, minPos, maxPos);
	for (int i = 0; i < k; i++)
		pfree(points[i]);
	pfree(points);
	return result;
}

PG_FUNCTION_INFO_V1(geom_as_nsegment);

PGDLLEXPORT Datum
geom_as_nsegment(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	if (gserialized_get_type(gs) != LINETYPE ||
		gserialized_is_empty(gs))
	{
		PG_FREE_IF_COPY(gs, 0);
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), 
			errmsg("Only non-empty line geometries accepted")));
	}
	nsegment *result = geom_as_nsegment_internal(PointerGetDatum(gs));
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/* nsegment array to geometry */

Datum
nsegmentarr_to_geom_internal(nsegment **segments, int count)
{
	Datum *geoms = palloc(sizeof(Datum) * count);
	for (int i = 0; i < count; i++)
	{
		Datum line = route_geom(segments[i]->rid);
		if (segments[i]->pos1 == 0 && segments[i]->pos2 == 1)
			geoms[i] = PointerGetDatum(gserialized_copy((GSERIALIZED *)PG_DETOAST_DATUM(line)));
		else if (segments[i]->pos1 == segments[i]->pos2)
			geoms[i] = call_function2(LWGEOM_line_interpolate_point, line, Float8GetDatum(segments[i]->pos1));
		else
			geoms[i] = call_function3(LWGEOM_line_substring, line,
				Float8GetDatum(segments[i]->pos1), Float8GetDatum(segments[i]->pos2));
		pfree(DatumGetPointer(line));
	}
	Datum result;
	if (count == 1)
		result = geoms[0];
	else
	{
		ArrayType *array = datumarr_to_array(geoms, count, type_oid(T_GEOMETRY));
		result = call_function1(pgis_union_geometry_array, PointerGetDatum(array));
		pfree(array);
		for (int i = 0; i < count; i++)
			pfree(DatumGetPointer(geoms[i])); 
		pfree(geoms); 
	}
	PG_RETURN_DATUM(result);
}

/*****************************************************************************/