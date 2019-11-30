#include "tgeo_out.h"

#include <assert.h>
#include <float.h>
#include <utils/builtins.h>

#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "tgeo.h"
#include "tgeo_out.h"
#include "tpoint_spatialfuncs.h"

/*****************************************************************************
 * Output in WKT and EWKT format 
 *****************************************************************************/

/* 
 * Output a moving region in Well-Known Text (WKT) and Extended Well-Known Text 
 * (EWKT) format.
 * The Oid argument is not used but is needed since the second argument of 
 * the functions temporal*_to_string is of type char *(*value_out)(Oid, Datum) 
 */
char *
region_wkt_out(Oid type, Datum value)
{
    char *result;
    if (type == type_oid(T_GEOMETRY) || type == type_oid(T_GEOGRAPHY))
    {
        GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(value);
        LWGEOM *geom = lwgeom_from_gserialized(gs);
        size_t len;
        char *wkt = lwgeom_to_wkt(geom, WKT_ISO, DBL_DIG, &len);
        result = palloc(len);
        strcpy(result, wkt);
        lwgeom_free(geom);
        pfree(wkt);
    }
    else if (type == type_oid(T_RTRANSFORM))
    {
        rtransform *rt = DatumGetRtransform(value);
        result = psprintf("RTransform(%g, %g, %g)", rt->theta, rt->tx, rt->ty);
    }
    return result;
}

char *
region_ewkt_out(Oid type, Datum value)
{
    char *result;
    if (type == type_oid(T_GEOMETRY) || type == type_oid(T_GEOGRAPHY))
    {
        GSERIALIZED *gs = (GSERIALIZED *)DatumGetPointer(value);
        LWGEOM *geom = lwgeom_from_gserialized(gs);
        size_t len;
        char *wkt = lwgeom_to_wkt(geom, WKT_EXTENDED, DBL_DIG, &len);
        result = palloc(len);
        strcpy(result, wkt);
        lwgeom_free(geom);
        pfree(wkt);
    }
    else if (type == type_oid(T_RTRANSFORM))
    {
        rtransform *rt = DatumGetRtransform(value);
        result = psprintf("RTransform(%g, %g, %g)", rt->theta, rt->tx, rt->ty);
    }
    return result;
}

/* Output a temporal geometry or geography in WKT format */

static text *
tgeo_as_text_internal(Temporal *temp)
{
    char *str = NULL;
    temporal_duration_is_valid(temp->duration);
    if (temp->duration == TEMPORALINST) 
        str = temporalinst_to_string((TemporalInst *)temp, &region_wkt_out);
    else if (temp->duration == TEMPORALI) 
        str = temporali_to_string((TemporalI *)temp, &region_wkt_out);
    else if (temp->duration == TEMPORALSEQ) 
        str = temporalseq_to_string((TemporalSeq *)temp, &region_wkt_out);
    else if (temp->duration == TEMPORALS) 
        str = temporals_to_string((TemporalS *)temp, &region_wkt_out);
    text *result = cstring_to_text(str);
    pfree(str);
    return result;
}

PG_FUNCTION_INFO_V1(tgeo_as_text);

PGDLLEXPORT Datum
tgeo_as_text(PG_FUNCTION_ARGS)
{
    /*ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
        errmsg("Alright")));*/
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    text *result = tgeo_as_text_internal(temp);
    PG_FREE_IF_COPY(temp, 0);
    PG_RETURN_TEXT_P(result);
}

/* Output a temporal geometry or geography in EWKT format */

static text *
tgeo_as_ewkt_internal(Temporal *temp)
{
    int srid = tpoint_srid_internal(temp);
    char str1[20];
    if (srid > 0)
        sprintf(str1, "SRID=%d;", srid);
    else
        str1[0] = '\0';
    char *str2 = NULL;
    temporal_duration_is_valid(temp->duration);
    if (temp->duration == TEMPORALINST) 
        str2 = temporalinst_to_string((TemporalInst *)temp, &region_ewkt_out);
    else if (temp->duration == TEMPORALI) 
        str2 = temporali_to_string((TemporalI *)temp, &region_ewkt_out);
    else if (temp->duration == TEMPORALSEQ) 
        str2 = temporalseq_to_string((TemporalSeq *)temp, &region_ewkt_out);
    else if (temp->duration == TEMPORALS) 
        str2 = temporals_to_string((TemporalS *)temp, &region_ewkt_out);
    char *str = (char *) palloc(strlen(str1) + strlen(str2) + 1);
    strcpy(str, str1);
    strcat(str, str2);
    text *result = cstring_to_text(str);
    pfree(str2); pfree(str);
    return result;
}

PG_FUNCTION_INFO_V1(tgeo_as_ewkt);

PGDLLEXPORT Datum
tgeo_as_ewkt(PG_FUNCTION_ARGS)
{
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    text *result = tgeo_as_ewkt_internal(temp);
    PG_FREE_IF_COPY(temp, 0);
    PG_RETURN_TEXT_P(result);
}

/*****************************************************************************/

/* Output a temporal geometry/geography array in WKT format */

PG_FUNCTION_INFO_V1(tgeoarr_as_text);

PGDLLEXPORT Datum
tgeoarr_as_text(PG_FUNCTION_ARGS)
{
    ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
    int count;
    Temporal **temparr = temporalarr_extract(array, &count);
    if (count == 0)
    {
        PG_FREE_IF_COPY(array, 0);
        PG_RETURN_NULL();
    }
    text **textarr = palloc(sizeof(text *) * count);
    for (int i = 0; i < count; i++)
        textarr[i] = tgeo_as_text_internal(temparr[i]);
    ArrayType *result = textarr_to_array(textarr, count);

    pfree(temparr);
    for (int i = 0; i < count; i++)
        pfree(textarr[i]);
    pfree(textarr);
    PG_FREE_IF_COPY(array, 0);

    PG_RETURN_ARRAYTYPE_P(result);
}

/* Output a temporal point array in WKT format prefixed with the SRID */

PG_FUNCTION_INFO_V1(tgeoarr_as_ewkt);

PGDLLEXPORT Datum
tgeoarr_as_ewkt(PG_FUNCTION_ARGS)
{
    ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
    int count;
    Temporal **temparr = temporalarr_extract(array, &count);
    if (count == 0)
    {
        PG_FREE_IF_COPY(array, 0);
        PG_RETURN_NULL();
    }
    text **textarr = palloc(sizeof(text *) * count);
    for (int i = 0; i < count; i++)
        textarr[i] = tgeo_as_ewkt_internal(temparr[i]);
    ArrayType *result = textarr_to_array(textarr, count);

    pfree(temparr);
    for (int i = 0; i < count; i++)
        pfree(textarr[i]);
    pfree(textarr);
    PG_FREE_IF_COPY(array, 0);

    PG_RETURN_ARRAYTYPE_P(result);
}