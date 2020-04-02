/*****************************************************************************
 *
 * tpoint.c
 *	  Basic functions for temporal points.
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tpoint.h"

#include <utils/builtins.h>
#include <utils/timestamp.h>

#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "lifting.h"
#include "temporal_compops.h"
#include "stbox.h"
#include "tpoint_parser.h"
#include "tpoint_boxops.h"
#include "tpoint_spatialfuncs.h"

/*****************************************************************************
 * Miscellaneous functions
 *****************************************************************************/

/* Initialize the extension */

#define PGC_ERRMSG_MAXLEN 2048

static void
pg_error(const char *fmt, va_list ap)
{
	char errmsg[PGC_ERRMSG_MAXLEN + 1];

	vsnprintf (errmsg, PGC_ERRMSG_MAXLEN, fmt, ap);

	errmsg[PGC_ERRMSG_MAXLEN]='\0';
	ereport(ERROR, (errmsg_internal("%s", errmsg)));
}

static void
pg_notice(const char *fmt, va_list ap)
{
	char errmsg[PGC_ERRMSG_MAXLEN + 1];

	vsnprintf (errmsg, PGC_ERRMSG_MAXLEN, fmt, ap);

	errmsg[PGC_ERRMSG_MAXLEN]='\0';
	ereport(NOTICE, (errmsg_internal("%s", errmsg)));
}

void temporalgeom_init()
{
	lwgeom_set_handlers(palloc, repalloc, pfree, pg_error, pg_notice);
}

/*****************************************************************************
 * Input/output functions
 *****************************************************************************/

/*
 * Check the consistency of the metadata we want to enforce in the typmod:
 * srid, type and dimensionality. If things are inconsistent, shut down the query.
 */

static Temporal *
tpoint_valid_typmod(Temporal *temp, int32_t typmod)
{
	int32 tpoint_srid = tpoint_srid_internal(temp);
	int32 tpoint_type = temp->duration;
	int32 duration = TYPMOD_GET_DURATION(typmod);
	TYPMOD_DEL_DURATION(typmod);
	/* If there is no geometry type */
	if (typmod == 0)
		typmod = -1;
	int32 tpoint_z = MOBDB_FLAGS_GET_Z(temp->flags);
	int32 typmod_srid = TYPMOD_GET_SRID(typmod);
	int32 typmod_type = TYPMOD_GET_TYPE(typmod);
	int32 typmod_z = TYPMOD_GET_Z(typmod);

	/* No typmod (-1) */
	if (typmod < 0 && duration == 0)
		return temp;
	/* Typmod has a preference for SRID? Geometry SRID had better match.  */
	if (typmod_srid > 0 && typmod_srid != tpoint_srid)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Temporal point SRID (%d) does not match column SRID (%d)",
					tpoint_srid, typmod_srid) ));
	/* Typmod has a preference for temporal type.  */
	if (typmod_type > 0 && duration != 0 && duration != tpoint_type)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Temporal type (%s) does not match column type (%s)",
					temporal_duration_name(tpoint_type), temporal_duration_name(duration)) ));
	/* Mismatched Z dimensionality.  */
	if (typmod > 0 && typmod_z && ! tpoint_z)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Column has Z dimension but temporal point does not" )));
	/* Mismatched Z dimensionality (other way).  */
	if (typmod > 0 && tpoint_z && ! typmod_z)
		ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Temporal point has Z dimension but column does not" )));

	return temp;
}

/* 
 * Input function. 
 * Examples of input:
 * - tpointinst
 *	  Point(0 0) @ 2012-01-01 08:00:00
 * - tpointi
 * 		{ Point(0 0) @ 2012-01-01 08:00:00 , Point(1 1) @ 2012-01-01 08:10:00 }
 * - tpointseq
 * 		[ Point(0 0) @ 2012-01-01 08:00:00 , Point(1 1) @ 2012-01-01 08:10:00 )
 * - tpoints
 * 		{ [ Point(0 0) @ 2012-01-01 08:00:00 , Point(1 1) @ 2012-01-01 08:10:00 ) ,
 *  	  [ Point(1 1) @ 2012-01-01 08:20:00 , Point(0 0) @ 2012-01-01 08:30:00 ] }
 */
PG_FUNCTION_INFO_V1(tpoint_in);

PGDLLEXPORT Datum
tpoint_in(PG_FUNCTION_ARGS) 
{
	char *input = PG_GETARG_CSTRING(0);
	Oid temptypid = PG_GETARG_OID(1);
	Oid valuetypid;
	temporal_typinfo(temptypid, &valuetypid);
	Temporal *result = tpoint_parse(&input, valuetypid);
	PG_RETURN_POINTER(result);
}

static uint32 
tpoint_typmod_in(ArrayType *arr, int is_geography)
{
	uint32 typmod = 0;
	Datum *elem_values;
	int n = 0;

	if (ARR_ELEMTYPE(arr) != CSTRINGOID)
		ereport(ERROR, (errcode(ERRCODE_ARRAY_ELEMENT_ERROR),
			errmsg("typmod array must be type cstring[]")));
	if (ARR_NDIM(arr) != 1)
		ereport(ERROR, (errcode(ERRCODE_ARRAY_SUBSCRIPT_ERROR),
			errmsg("typmod array must be one-dimensional")));
	if (ARR_HASNULL(arr))
		ereport(ERROR, (errcode(ERRCODE_NULL_VALUE_NOT_ALLOWED),
			errmsg("typmod array must not contain nulls")));

	/*
	 * There are several ways to define a column wrt type modifiers:
	 *   column_type(Duration, Geometry, SRID) => All modifiers are determined.
	 * 	 column_type(Duration, Geometry) => The SRID is generic.
	 * 	 column_type(Geometry, SRID) => The duration type is generic.
	 * 	 column_type(Geometry) => The duration type and SRID are generic.
	 *	 column_type(Duration) => The geometry type and SRID are generic.
	 *	 column_type => The duration type, geometry type, and SRID are generic.
	 *
	 * For example, if the user did not set the duration type, we can use all 
	 * duration types in the same column. Similarly for all generic modifiers.
	 */
	deconstruct_array(arr, CSTRINGOID, -2, false, 'c', &elem_values, NULL, &n);
	int16 duration = 0;
	uint8_t geometry_type = 0;
	int hasZ = 0, hasM = 0;
	char *s;
	
	switch(n)
	{
		case 3: 
		{
			/* Type_modifier is (Duration, Geometry, SRID) */
			/* Duration type */
			s = DatumGetCString(elem_values[0]);
			if (temporal_duration_from_string(s, &duration) == false) 
				ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						errmsg("Invalid duration type modifier: %s", s)));

			/* Shift to remove the 4 bits of the duration */
			TYPMOD_DEL_DURATION(typmod);
			/* Set default values */
			if (is_geography)
				TYPMOD_SET_SRID(typmod, SRID_DEFAULT);
			else
				TYPMOD_SET_SRID(typmod, SRID_UNKNOWN);
						
			/* Geometry type */
			s = DatumGetCString(elem_values[1]);
			if (geometry_type_from_string(s, &geometry_type, &hasZ, &hasM) == LW_FAILURE) 
				ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						errmsg("Invalid geometry type modifier: %s", s)));
			if (geometry_type != POINTTYPE || hasM)
				ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("Only point geometries without M dimension accepted")));

			TYPMOD_SET_TYPE(typmod, geometry_type);
			if (hasZ)
				TYPMOD_SET_Z(typmod);
		
			/* SRID */
			s = DatumGetCString(elem_values[2]);
			int srid = pg_atoi(s, sizeof(int32), '\0');
			srid = clamp_srid(srid);
			if (srid != SRID_UNKNOWN)
				TYPMOD_SET_SRID(typmod, srid);
			/* Shift to restore the 4 bits of the duration */
			TYPMOD_SET_DURATION(typmod, duration);
			break;
		}
		case 2:
		{
			/* Type modifier is either (Duration, Geometry) or (Geometry, SRID) */
			s = DatumGetCString(elem_values[0]);
			if (temporal_duration_from_string(s, &duration)) 
			{
				/* Type modifier is (Duration, Geometry) */
				/* Shift to remove the 4 bits of the duration */
				TYPMOD_DEL_DURATION(typmod);
				/* Set default values */
				if (is_geography)
					TYPMOD_SET_SRID(typmod, SRID_DEFAULT);
				else
					TYPMOD_SET_SRID(typmod, SRID_UNKNOWN);
				/* Geometry type */
				s = DatumGetCString(elem_values[1]);
				if (geometry_type_from_string(s, &geometry_type, &hasZ, &hasM) == LW_FAILURE)
					ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
							errmsg("Invalid geometry type modifier: %s", s)));
				if (geometry_type != POINTTYPE || hasM)
					ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						errmsg("Only point geometries without M dimension accepted")));

				TYPMOD_SET_TYPE(typmod, geometry_type);
				if (hasZ)
					TYPMOD_SET_Z(typmod);
				/* Shift to restore the 4 bits of the duration */
				TYPMOD_SET_DURATION(typmod, duration);
			}
			else if (geometry_type_from_string(s, &geometry_type, &hasZ, &hasM))
			{
				/* Type modifier is (Geometry, SRID) */
				if (geometry_type != POINTTYPE || hasM)
					ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						errmsg("Only point geometries without M dimension accepted")));

				/* Shift to remove the 4 bits of the duration */
				TYPMOD_DEL_DURATION(typmod);
				/* Set default values */
				if (is_geography)
					TYPMOD_SET_SRID(typmod, SRID_DEFAULT);
				else
					TYPMOD_SET_SRID(typmod, SRID_UNKNOWN);
				
				TYPMOD_SET_TYPE(typmod, geometry_type);
				if (hasZ)
					TYPMOD_SET_Z(typmod);
				/* SRID */
				s = DatumGetCString(elem_values[1]);
				int srid = pg_atoi(s, sizeof(int32), '\0');
				srid = clamp_srid(srid);
				if (srid != SRID_UNKNOWN)
					TYPMOD_SET_SRID(typmod, srid);
				/* Shift to restore the 4 bits of the duration */
				TYPMOD_SET_DURATION(typmod, duration);
			}
			else
				ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("Invalid temporal point type modifier: %s", s)));
			break;
		}
		case 1:
		{
			/* Type modifier: either (Duration) or (Geometry) */
			s = DatumGetCString(elem_values[0]);
			if (temporal_duration_from_string(s, &duration))
			{
				TYPMOD_SET_DURATION(typmod, duration);
			}
			else if (geometry_type_from_string(s, &geometry_type, &hasZ, &hasM)) 
			{
				if (geometry_type != POINTTYPE)
					ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
						errmsg("Only point geometries accepted")));

				/* Shift to remove the 4 bits of the duration */
				TYPMOD_DEL_DURATION(typmod);
				/* Set default values */
				if (is_geography)
					TYPMOD_SET_SRID(typmod, SRID_DEFAULT);
				else
					TYPMOD_SET_SRID(typmod, SRID_UNKNOWN);

				TYPMOD_SET_TYPE(typmod, geometry_type);
				if (hasZ)
					TYPMOD_SET_Z(typmod);

				/* Shift to restore the 4 bits of the duration */
				TYPMOD_SET_DURATION(typmod, duration);
			}
			else
				ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("Invalid temporal point type modifier: %s", s)));
			break;
		}
		default:
			ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("Invalid temporal point type modifier:")));
	}
	pfree(elem_values);
	return typmod;
}

/*
 * typmod input for tgeompoint
 */
PG_FUNCTION_INFO_V1(tgeompoint_typmod_in);

PGDLLEXPORT Datum 
tgeompoint_typmod_in(PG_FUNCTION_ARGS)
{
	ArrayType *array = (ArrayType *) DatumGetPointer(PG_GETARG_DATUM(0));
	uint32 typmod = tpoint_typmod_in(array, false); /* Not a geography  */;
	PG_RETURN_INT32(typmod);
}

/*
 * typmod input for tgeogpoint
 */
PG_FUNCTION_INFO_V1(tgeogpoint_typmod_in);

PGDLLEXPORT Datum 
tgeogpoint_typmod_in(PG_FUNCTION_ARGS)
{
	ArrayType *array = (ArrayType *) DatumGetPointer(PG_GETARG_DATUM(0));
	int32 typmod = tpoint_typmod_in(array, true);
	// int srid = TYPMOD_GET_SRID(typmod);
	// /* Check the SRID is legal (geographic coordinates) */
	// srid_is_latlong(fcinfo, srid);
	PG_RETURN_INT32(typmod);
}

/*
 * typmod input for tgeompoint and tgeogpoint
 */
PG_FUNCTION_INFO_V1(tpoint_typmod_out);

PGDLLEXPORT Datum 
tpoint_typmod_out(PG_FUNCTION_ARGS)
{
	char *s = (char *) palloc(64);
	char *str = s;
	int32 typmod = PG_GETARG_INT32(0);
	int16 duration = TYPMOD_GET_DURATION(typmod);
	TYPMOD_DEL_DURATION(typmod);
	int32 srid = TYPMOD_GET_SRID(typmod);
	uint8_t geometry_type = (uint8_t) TYPMOD_GET_TYPE(typmod);
	int32 hasz = TYPMOD_GET_Z(typmod);

	/* No duration type or geometry type? Then no typmod at all. 
	  Return empty string. */
	if (typmod < 0 || (!duration && !geometry_type))
	{
		*str = '\0';
		PG_RETURN_CSTRING(str);
	}
	/* Opening bracket.  */
	str += sprintf(str, "(");
	/* Has duration type?  */
	if (duration)
		str += sprintf(str, "%s", temporal_duration_name(duration));
	if (geometry_type)
	{
		if (duration) str += sprintf(str, ",");
		str += sprintf(str, "%s", lwtype_name(geometry_type));
		/* Has Z?  */
		if (hasz) str += sprintf(str, "Z");
		/* Has SRID?  */
		if (srid) str += sprintf(str, ",%d", srid);
	}
	/* Closing bracket.  */
	sprintf(str, ")");

	PG_RETURN_CSTRING(s);
}

/*
 * Ensure that an incoming geometry conforms to typmod restrictions on
 * type, dims and srid.
 */
PG_FUNCTION_INFO_V1(tpoint_enforce_typmod);

PGDLLEXPORT Datum
tpoint_enforce_typmod(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	int32 typmod = PG_GETARG_INT32(1);
	/* Check if geometry typmod is consistent with the supplied one.  */
	temp = tpoint_valid_typmod(temp, typmod);
	PG_RETURN_POINTER(temp);
}

/*****************************************************************************
 * Constructor functions
 *****************************************************************************/

/* Construct a temporal instant point from two arguments */

PG_FUNCTION_INFO_V1(tpointinst_constructor);
 
PGDLLEXPORT Datum
tpointinst_constructor(PG_FUNCTION_ARGS) 
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	ensure_point_type(gs);
	ensure_non_empty(gs);
	ensure_has_not_M_gs(gs);
	TimestampTz t = PG_GETARG_TIMESTAMPTZ(1);
	Oid	valuetypid = get_fn_expr_argtype(fcinfo->flinfo, 0);
	Temporal *result = (Temporal *)temporalinst_make(PointerGetDatum(gs),
		t, valuetypid);
	PG_FREE_IF_COPY(gs, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Accessor functions
 *****************************************************************************/

/* Get the precomputed bounding box of a Temporal (if any) 
   Notice that TemporalInst do not have a precomputed bounding box */

PG_FUNCTION_INFO_V1(tpoint_stbox);

PGDLLEXPORT Datum
tpoint_stbox(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	STBOX *result = palloc0(sizeof(STBOX));
	temporal_bbox(result, temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Ever/always comparison operators
 *****************************************************************************/

/* Is the temporal value ever equal to the value? */

PG_FUNCTION_INFO_V1(tpoint_ever_eq);

PGDLLEXPORT Datum
tpoint_ever_eq(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	/* Bounding box test */
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_BOOL(false);
	}
	temporal_bbox(&box1, temp);
	if (!contains_stbox_stbox_internal(&box1, &box2))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_BOOL(false);
	}

	bool result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = temporalinst_ever_eq((TemporalInst *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALI) 
		result = temporali_ever_eq((TemporalI *)temp, 
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = temporalseq_ever_eq((TemporalSeq *)temp, 
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
		result = temporals_ever_eq((TemporalS *)temp, 
			PointerGetDatum(gs));

	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_BOOL(result);
}

PG_FUNCTION_INFO_V1(tpoint_always_eq);

PGDLLEXPORT Datum
tpoint_always_eq(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	/* The bounding box test is enough to test the predicate */
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_BOOL(false);
	}
	temporal_bbox(&box1, temp);
	if (!same_stbox_stbox_internal(&box1, &box2))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_BOOL(false);
	}

	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_BOOL(true);
}

/* Is the temporal value ever not equal to the value? */

PG_FUNCTION_INFO_V1(tpoint_ever_ne);

PGDLLEXPORT Datum
tpoint_ever_ne(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(! tpoint_always_eq(fcinfo));
}

/* Is the temporal value always not equal to the value? */

PG_FUNCTION_INFO_V1(tpoint_always_ne);

PGDLLEXPORT Datum
tpoint_always_ne(PG_FUNCTION_ARGS)
{
	PG_RETURN_BOOL(! tpoint_ever_eq(fcinfo));
}

/*****************************************************************************
 * Temporal comparisons
 *****************************************************************************/

PG_FUNCTION_INFO_V1(teq_geo_tpoint);

PGDLLEXPORT Datum
teq_geo_tpoint(PG_FUNCTION_ARGS)
{
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
	ensure_point_type(gs);
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

PG_FUNCTION_INFO_V1(teq_tpoint_geo);

PGDLLEXPORT Datum
teq_tpoint_geo(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	Oid datumtypid = get_fn_expr_argtype(fcinfo->flinfo, 1);
	Temporal *result = tcomp_temporal_base(temp, PointerGetDatum(gs), datumtypid,
		&datum2_eq2, false);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(teq_tpoint_tpoint);

PGDLLEXPORT Datum
teq_tpoint_tpoint(PG_FUNCTION_ARGS)
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

PG_FUNCTION_INFO_V1(tne_geo_tpoint);

PGDLLEXPORT Datum
tne_geo_tpoint(PG_FUNCTION_ARGS)
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

PG_FUNCTION_INFO_V1(tne_tpoint_geo);

PGDLLEXPORT Datum
tne_tpoint_geo(PG_FUNCTION_ARGS)
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

PG_FUNCTION_INFO_V1(tne_tpoint_tpoint);

PGDLLEXPORT Datum
tne_tpoint_tpoint(PG_FUNCTION_ARGS)
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

/*****************************************************************************
 * Assemble the Trajectory/values of a temporal point as a single
 * geometry/geography.
 *****************************************************************************/

Datum
tpoint_values_internal(Temporal *temp)
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

PG_FUNCTION_INFO_V1(tpoint_values);

PGDLLEXPORT Datum
tpoint_values(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	Datum result = tpoint_values_internal(temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_POINTER(result);
}

/*****************************************************************************
 * Restriction functions
 *****************************************************************************/

 /* Restriction to the value */

PG_FUNCTION_INFO_V1(tpoint_at_value);

PGDLLEXPORT Datum
tpoint_at_value(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	/* Bounding box test */
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!geo_to_stbox_internal(&box2, gs))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}
	temporal_bbox(&box1, temp);
	if (!contains_stbox_stbox_internal(&box1, &box2))
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_NULL();
	}

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_at_value((TemporalInst *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)temporali_at_value((TemporalI *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)temporalseq_at_value((TemporalSeq *)temp,
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)temporals_at_value((TemporalS *)temp,
			PointerGetDatum(gs));

	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Restriction to the complement of a value */

PG_FUNCTION_INFO_V1(tpoint_minus_value);

PGDLLEXPORT Datum
tpoint_minus_value(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(1);
	ensure_point_type(gs);
	ensure_same_srid_tpoint_gs(temp, gs);
	ensure_same_dimensionality_tpoint_gs(temp, gs);
	/* Bounding box test */
	STBOX box1, box2;
	memset(&box1, 0, sizeof(STBOX));
	memset(&box2, 0, sizeof(STBOX));
	if (!geo_to_stbox_internal(&box2, gs))
	{
		Temporal *result;
		if (temp->duration == TEMPORALSEQ)
			result = (Temporal *)temporals_make((TemporalSeq **)&temp, 1, false);
		else
			result = temporal_copy(temp);
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(result);
	}
	temporal_bbox(&box1, temp);
	if (!contains_stbox_stbox_internal(&box1, &box2))
	{
		Temporal *result;
		if (temp->duration == TEMPORALSEQ)
			result = (Temporal *)temporals_make((TemporalSeq **)&temp, 1, false);
		else
			result = temporal_copy(temp);
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(gs, 1);
		PG_RETURN_POINTER(result);
	}

	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_minus_value((TemporalInst *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)temporali_minus_value((TemporalI *)temp,
			PointerGetDatum(gs));
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)temporalseq_minus_value((TemporalSeq *)temp,
			PointerGetDatum(gs));
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)temporals_minus_value((TemporalS *)temp,
			PointerGetDatum(gs));

	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(gs, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/

/* Restriction to the values */

PG_FUNCTION_INFO_V1(tpoint_at_values);

PGDLLEXPORT Datum
tpoint_at_values(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	ArrayType *array = PG_GETARG_ARRAYTYPE_P(1);
	int count;
	Datum *values = datumarr_extract(array, &count);
	if (count == 0)
	{
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(array, 1);
		PG_RETURN_NULL();
	}
	for (int i = 0; i < count; i++)
	{
		GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(values[i]);
		ensure_point_type(gs);
		ensure_same_srid_tpoint_gs(temp, gs);
		ensure_same_dimensionality_tpoint_gs(temp, gs);
	}
	
	Oid valuetypid = temp->valuetypid;
	datum_sort(values, count, valuetypid);
	int count1 = datum_remove_duplicates(values, count, valuetypid);
	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_at_values((TemporalInst *)temp, 
			values, count1);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)temporali_at_values((TemporalI *)temp,
			values, count1);
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)temporalseq_at_values((TemporalSeq *)temp,
			values, count1);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)temporals_at_values((TemporalS *)temp,
			values, count1);

	pfree(values);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(array, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/************************************************************************/

/* Restriction to the complement of values */

PG_FUNCTION_INFO_V1(tpoint_minus_values);

PGDLLEXPORT Datum
tpoint_minus_values(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	ArrayType *array = PG_GETARG_ARRAYTYPE_P(1);
	int count;
	Datum *values = datumarr_extract(array, &count);
	if (count == 0)
	{
		Temporal *result = temporal_copy(temp);
		PG_FREE_IF_COPY(temp, 0);
		PG_FREE_IF_COPY(array, 1);
		PG_RETURN_POINTER(result);
	}
	for (int i = 0; i < count; i++)
	{
		GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(values[i]);
		ensure_point_type(gs);
		ensure_same_srid_tpoint_gs(temp, gs);
		ensure_same_dimensionality_tpoint_gs(temp, gs);
	}
	
	Oid valuetypid = temp->valuetypid;
	datum_sort(values, count, valuetypid);
	int count1 = datum_remove_duplicates(values, count, valuetypid);
	Temporal *result;
	ensure_valid_duration(temp->duration);
	if (temp->duration == TEMPORALINST) 
		result = (Temporal *)temporalinst_minus_values((TemporalInst *)temp,
			values, count1);
	else if (temp->duration == TEMPORALI) 
		result = (Temporal *)temporali_minus_values((TemporalI *)temp,
			values, count1);
	else if (temp->duration == TEMPORALSEQ) 
		result = (Temporal *)temporalseq_minus_values((TemporalSeq *)temp,
			values, count1);
	else /* temp->duration == TEMPORALS */
		result = (Temporal *)temporals_minus_values((TemporalS *)temp,
			values, count1);

	pfree(values);
	PG_FREE_IF_COPY(temp, 0);
	PG_FREE_IF_COPY(array, 1);
	if (result == NULL)
		PG_RETURN_NULL();
	PG_RETURN_POINTER(result);
}

/*****************************************************************************/
