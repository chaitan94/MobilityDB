/*****************************************************************************
 *
 * tgeo.c
 *    Basic functions for temporal geometries and geographies.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tgeo.h"

#include <utils/builtins.h>
#include <utils/timestamp.h>

#include "temporal.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "tgeo_parser.h"
#include "tpoint.h"
#include "tpoint_spatialfuncs.h"

/*****************************************************************************
 * Input/output functions
 *****************************************************************************/

/*
 * Check the consistency of the metadata we want to enforce in the typmod:
 * srid, type and dimensionality. If things are inconsistent, shut down the query.
 */
Temporal*
tgeo_valid_typmod(Temporal *temp, int32_t typmod)
{
    int32 tgeo_srid = tpoint_srid_internal(temp);
    int32 tgeo_type = temp->duration;
    int32 duration = TYPMOD_GET_DURATION(typmod);
    TYPMOD_DEL_DURATION(typmod);
    /* If there is no geometry type */
    if (typmod == 0)
        typmod = -1;
    int32 tgeo_z = MOBDB_FLAGS_GET_Z(temp->flags);
    int32 typmod_srid = TYPMOD_GET_SRID(typmod);
    int32 typmod_type = TYPMOD_GET_TYPE(typmod);
    int32 typmod_z = TYPMOD_GET_Z(typmod);

    /* No typmod (-1) */
    if (typmod < 0 && duration == 0)
        return temp;
    /* Typmod has a preference for SRID? Geometry SRID had better match.  */
    if ( typmod_srid > 0 && typmod_srid != tgeo_srid )
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Temporal geo SRID (%d) does not match column SRID (%d)",
                    tgeo_srid, typmod_srid) ));
    /* Typmod has a preference for temporal type.  */
    if (typmod_type > 0 && duration != 0 && duration != tgeo_type)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Temporal type (%s) does not match column type (%s)",
                    temporal_duration_name(tgeo_type), temporal_duration_name(duration)) ));
    /* Mismatched Z dimensionality.  */
    if (typmod > 0 && typmod_z && ! tgeo_z)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Column has Z dimension but temporal geo does not" )));
    /* Mismatched Z dimensionality (other way).  */
    if (typmod > 0 && tgeo_z && ! typmod_z)
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                errmsg("Temporal type has Z dimension but column does not" )));

    return temp;
}

PG_FUNCTION_INFO_V1(tgeo_in);

PGDLLEXPORT Datum
tgeo_in(PG_FUNCTION_ARGS) 
{
    char *input = PG_GETARG_CSTRING(0);
    Oid temptypid = PG_GETARG_OID(1);
    Oid valuetypid;
    temporal_typinfo(temptypid, &valuetypid);
    Temporal *result = tgeo_parse(&input, valuetypid);
    if (result == 0)
        PG_RETURN_NULL();
    PG_RETURN_POINTER(result);
}

static uint32 
tgeo_typmod_in(ArrayType *arr, int is_geography)
{
    int32 typmod = 0;
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
     *   column_type(Duration, Geometry) => The SRID is generic.
     *   column_type(Geometry, SRID) => The duration type is generic.
     *   column_type(Geometry) => The duration type and SRID are generic.
     *   column_type(Duration) => The geometry type and SRID are generic.
     *   column_type => The duration type, geometry type, and SRID are generic.
     *
     * For example, if the user did not set the duration type, we can use all 
     * duration types in the same column. Similarly for all generic modifiers.
     */
    deconstruct_array(arr, CSTRINGOID, -2, false, 'c', &elem_values, NULL, &n);
    int16 duration = 0;
    uint8_t geometry_type = 0;
    int z = 0, m = 0;
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
            if (geometry_type_from_string(s, &geometry_type, &z, &m) == LW_FAILURE) 
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Invalid geometry type modifier: %s", s)));
            if ((geometry_type != LINETYPE && geometry_type != POLYGONTYPE) || m)
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("Only linestring or polygon geometries without M dimension accepted")));

            TYPMOD_SET_TYPE(typmod, geometry_type);
            if (z)
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
                if (geometry_type_from_string(s, &geometry_type, &z, &m) == LW_FAILURE)
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                            errmsg("Invalid geometry type modifier: %s", s)));
                if ((geometry_type != LINETYPE && geometry_type != POLYGONTYPE) || m)
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Only linestring or polygon geometries without M dimension accepted")));

                TYPMOD_SET_TYPE(typmod, geometry_type);
                if (z)
                    TYPMOD_SET_Z(typmod);
                /* Shift to restore the 4 bits of the duration */
                TYPMOD_SET_DURATION(typmod, duration);
            }
            else if (geometry_type_from_string(s, &geometry_type, &z, &m))
            {
                /* Type modifier is (Geometry, SRID) */
                if ((geometry_type != LINETYPE && geometry_type != POLYGONTYPE) || m)
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Only linestring or polygon geometries without M dimension accepted")));

                /* Shift to remove the 4 bits of the duration */
                TYPMOD_DEL_DURATION(typmod);
                /* Set default values */
                if (is_geography)
                    TYPMOD_SET_SRID(typmod, SRID_DEFAULT);
                else
                    TYPMOD_SET_SRID(typmod, SRID_UNKNOWN);
                
                TYPMOD_SET_TYPE(typmod, geometry_type);
                if (z)
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
                    errmsg("Invalid temporal linestring or polygon type modifier: %s", s)));
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
            else if (geometry_type_from_string(s, &geometry_type, &z, &m)) 
            {
                if (geometry_type != LINETYPE && geometry_type != POLYGONTYPE)
                    ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("Only linestring or polygon geometries accepted")));

                /* Shift to remove the 4 bits of the duration */
                TYPMOD_DEL_DURATION(typmod);
                /* Set default values */
                if (is_geography)
                    TYPMOD_SET_SRID(typmod, SRID_DEFAULT);
                else
                    TYPMOD_SET_SRID(typmod, SRID_UNKNOWN);

                TYPMOD_SET_TYPE(typmod, geometry_type);
                if (z)
                    TYPMOD_SET_Z(typmod);

                /* Shift to restore the 4 bits of the duration */
                TYPMOD_SET_DURATION(typmod, duration);
            }
            else
                ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("Invalid temporal linestring or polygon type modifier: %s", s)));
            break;
        }
        default:
            ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                    errmsg("Invalid temporal linestring or polygon type modifier:")));
    }
    pfree(elem_values);
    return typmod;
}

/*
 * typmod input for tgeometry
 */
PG_FUNCTION_INFO_V1(tgeometry_typmod_in);

PGDLLEXPORT Datum 
tgeometry_typmod_in(PG_FUNCTION_ARGS)
{
    ArrayType *array = (ArrayType *) DatumGetPointer(PG_GETARG_DATUM(0));
    uint32 typmod = tgeo_typmod_in(array, false); /* Not a geography  */;
    PG_RETURN_INT32(typmod);
}

/*
 * typmod input for tgeography
 */
PG_FUNCTION_INFO_V1(tgeography_typmod_in);

PGDLLEXPORT Datum 
tgeography_typmod_in(PG_FUNCTION_ARGS)
{
    ArrayType *array = (ArrayType *) DatumGetPointer(PG_GETARG_DATUM(0));
    int32 typmod = tgeo_typmod_in(array, true);
    // int srid = TYPMOD_GET_SRID(typmod);
    // Check the SRID is legal (geographic coordinates)
    // srid_is_latlong(fcinfo, srid);
    PG_RETURN_INT32(typmod);
}

/*
 * typmod output for tgeometry and tgeography
 */
PG_FUNCTION_INFO_V1(tgeo_typmod_out);

PGDLLEXPORT Datum 
tgeo_typmod_out(PG_FUNCTION_ARGS)
{
    char *s = (char *)palloc(64);
    char *str = s;
    int32 typmod = PG_GETARG_INT32(0);
    int32 duration = TYPMOD_GET_DURATION(typmod);
    TYPMOD_DEL_DURATION(typmod);
    int32 srid = TYPMOD_GET_SRID(typmod);
    int32 geometry_type = TYPMOD_GET_TYPE(typmod);
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
    str += sprintf(str, ")");

    PG_RETURN_CSTRING(s);
}

/*
 * Ensure that an incoming geometry / geography conforms to typmod restrictions on
 * type, dims and srid.
 */
PG_FUNCTION_INFO_V1(tgeo_enforce_typmod);
PGDLLEXPORT Datum tgeo_enforce_typmod(PG_FUNCTION_ARGS)
{
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    int32 typmod = PG_GETARG_INT32(1);
    /* Check if geometry / geography typmod is consistent with the supplied one.  */
    temp = tgeo_valid_typmod(temp, typmod);
    PG_RETURN_POINTER(temp);
}

/*****************************************************************************
 * Constructor functions
 *****************************************************************************/

/* Construct a temporal instant geometry or geography from two arguments */

PG_FUNCTION_INFO_V1(tgeo_make_temporalinst);
 
PGDLLEXPORT Datum
tgeo_make_temporalinst(PG_FUNCTION_ARGS) 
{
    GSERIALIZED *gs = PG_GETARG_GSERIALIZED_P(0);
    int geo_type = gserialized_get_type(gs);
    if (((geo_type != LINETYPE) && (geo_type != POLYGONTYPE)) || gserialized_is_empty(gs) ||
        FLAGS_GET_M(gs->flags) || FLAGS_GET_Z(gs->flags))
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE), 
            errmsg("Only non-empty linestring or polygon geometries without Z or M dimension accepted")));

    TimestampTz t = PG_GETARG_TIMESTAMPTZ(1);
    Oid valuetypid = get_fn_expr_argtype(fcinfo->flinfo, 0);
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

PG_FUNCTION_INFO_V1(tgeo_stbox);

PGDLLEXPORT Datum
tgeo_stbox(PG_FUNCTION_ARGS)
{
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    STBOX *result = palloc0(sizeof(STBOX));
    temporal_bbox(result, temp);
    PG_FREE_IF_COPY(temp, 0);
    PG_RETURN_POINTER(result);
}

Datum
tgeo_values_internal(Temporal *temp)
{
    Datum result = 0;
    ensure_valid_duration(temp->duration);
    if (temp->duration == TEMPORALINST) 
        result = temporalinst_value_copy((TemporalInst *)temp);
    else if (temp->duration == TEMPORALI) {
        // TODO
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("TODO")));
    }
    else if (temp->duration == TEMPORALSEQ) {
        // TODO
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("TODO")));
    }
    else if (temp->duration == TEMPORALS) {
        // TODO
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("TODO")));
    }
    return result;
}

PG_FUNCTION_INFO_V1(tgeo_values);

PGDLLEXPORT Datum
tgeo_values(PG_FUNCTION_ARGS)
{
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    Datum result = tgeo_values_internal(temp);
    PG_FREE_IF_COPY(temp, 0);
    PG_RETURN_POINTER(result);
}

/*****************************************************************************/
