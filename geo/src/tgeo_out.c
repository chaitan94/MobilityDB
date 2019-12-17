#include "tgeo_out.h"

#include <assert.h>
#include <float.h>
#include <utils/builtins.h>

#include "temporal.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "tgeo.h"
#include "tgeo_out.h"
#include "tgeo_transform.h"
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

static char*
tgeoi_to_string(TemporalI *ti, char *(*value_out)(Oid, Datum))
{
    char** strings = palloc((int) (sizeof(char *)) * ti->count);
    size_t outlen = 0;

    TemporalInst *inst0 = temporali_inst_n(ti, 0);
    strings[0] = temporalinst_to_string(inst0, value_out);
    outlen += strlen(strings[0]) + 2;

    for (int i = 1; i < ti->count; i++)
    {
        TemporalInst *inst = temporali_inst_n(ti, i);
        TemporalInst *instRegion = tgeoinst_rtransfrom_to_region(inst, inst0);
        strings[i] = temporalinst_to_string(instRegion, value_out);
        pfree(instRegion);
        outlen += strlen(strings[i]) + 2;
    }
    char *result = palloc(outlen + 3);
    result[outlen] = '\0';
    result[0] = '{';
    size_t pos = 1;
    for (int i = 0; i < ti->count; i++)
    {
        strcpy(result + pos, strings[i]);
        pos += strlen(strings[i]);
        result[pos++] = ',';
        result[pos++] = ' ';
        pfree(strings[i]);
    }
    result[pos - 2] = '}';
    result[pos - 1] = '\0';
    pfree(strings);
    return result;
}

static char *
tgeoseq_to_string(TemporalSeq *seq, bool component, char *(*value_out)(Oid, Datum), TemporalInst *region)
{
    char **strings = palloc((int) (sizeof(char *)) * seq->count);
    size_t outlen = 0;
    char str[20];
    if (!component && linear_interpolation(seq->valuetypid) && 
        !MOBDB_FLAGS_GET_LINEAR(seq->flags))
        sprintf(str, "Interp=Stepwise;");
    else
        str[0] = '\0';

    if (region == NULL) 
    {
        TemporalInst *inst0 = temporalseq_inst_n(seq, 0);
        strings[0] = temporalinst_to_string(inst0, value_out);
        outlen += strlen(strings[0]) + 2;

        for (int i = 1; i < seq->count; i++)
        {
            TemporalInst *inst = temporalseq_inst_n(seq, i);
            TemporalInst *instRegion = tgeoinst_rtransfrom_to_region(inst, inst0);
            strings[i] = temporalinst_to_string(instRegion, value_out);
            pfree(instRegion);
            outlen += strlen(strings[i]) + 2;
        }
    } 
    else 
    {
        for (int i = 0; i < seq->count; i++)
        {
            TemporalInst *inst = temporalseq_inst_n(seq, i);
            TemporalInst *instRegion = tgeoinst_rtransfrom_to_region(inst, region);
            strings[i] = temporalinst_to_string(instRegion, value_out);
            pfree(instRegion);
            outlen += strlen(strings[i]) + 2;
        }
    }

    char *result = palloc(strlen(str) + outlen + 3);
    result[outlen] = '\0';
    size_t pos = 0;
    strcpy(result, str);
    pos += strlen(str);
    result[pos++] = seq->period.lower_inc ? '[' : '(';
    for (int i = 0; i < seq->count; i++)
    {
        strcpy(result + pos, strings[i]);
        pos += strlen(strings[i]);
        result[pos++] = ',';
        result[pos++] = ' ';
        pfree(strings[i]);
    }
    result[pos - 2] = seq->period.upper_inc ? ']' : ')';
    result[pos - 1] = '\0';
    pfree(strings);
    return result;
}

static char *
tgeos_to_string(TemporalS *ts, char *(*value_out)(Oid, Datum))
{
    char **strings = palloc((int) (sizeof(char *) * ts->count));
    size_t outlen = 0;
    char str[20];
    if (linear_interpolation(ts->valuetypid) && 
        !MOBDB_FLAGS_GET_LINEAR(ts->flags))
        sprintf(str, "Interp=Stepwise;");
    else
        str[0] = '\0';

    TemporalSeq *seq0 = temporals_seq_n(ts, 0);
    TemporalInst *inst0 = temporalseq_inst_n(seq0, 0);
    strings[0] = tgeoseq_to_string(seq0, true, value_out, NULL);
    outlen += strlen(strings[0]) + 2;

    for (int i = 1; i < ts->count; i++)
    {
        TemporalSeq *seq = temporals_seq_n(ts, i);
        strings[i] = tgeoseq_to_string(seq, true, value_out, inst0);
        outlen += strlen(strings[i]) + 2;
    }
    char *result = palloc(strlen(str) + outlen + 3);
    result[outlen] = '\0';
    size_t pos = 0;
    strcpy(result, str);
    pos += strlen(str);
    result[pos++] = '{';
    for (int i = 0; i < ts->count; i++)
    {
        strcpy(result + pos, strings[i]);
        pos += strlen(strings[i]);
        result[pos++] = ',';
        result[pos++] = ' ';
        pfree(strings[i]);
    }
    result[pos - 2] = '}';
    result[pos - 1] = '\0';
    pfree(strings);
    return result;
}

/* Output a temporal geometry or geography in WKT format */

static text *
tgeo_as_text_internal(Temporal *temp, bool asRegion)
{
    char *str = NULL;
    ensure_valid_duration(temp->duration);
    if (temp->duration == TEMPORALINST) 
        str = temporalinst_to_string((TemporalInst *)temp, &region_wkt_out);
    else if (temp->duration == TEMPORALI) 
    {
        if (asRegion)
            str = tgeoi_to_string((TemporalI *)temp, &region_wkt_out);
        else
            str = temporali_to_string((TemporalI *)temp, &region_wkt_out);
    }
    else if (temp->duration == TEMPORALSEQ) 
    {
        if (asRegion)
            str = tgeoseq_to_string((TemporalSeq *)temp, false, &region_wkt_out, NULL);
        else
            str = temporalseq_to_string((TemporalSeq *)temp, false, &region_wkt_out);
    }
    else if (temp->duration == TEMPORALS) 
    {
        if (asRegion)
            str = tgeos_to_string((TemporalS *)temp, &region_wkt_out);
        else
            str = temporals_to_string((TemporalS *)temp, &region_wkt_out);
    }
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
    text *result = tgeo_as_text_internal(temp, true);
    PG_FREE_IF_COPY(temp, 0);
    PG_RETURN_TEXT_P(result);
}

PG_FUNCTION_INFO_V1(tgeo_as_transform);

PGDLLEXPORT Datum
tgeo_as_transform(PG_FUNCTION_ARGS)
{
    /*ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
        errmsg("Alright")));*/
    Temporal *temp = PG_GETARG_TEMPORAL(0);
    text *result = tgeo_as_text_internal(temp, false);
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
    ensure_valid_duration(temp->duration);
    if (temp->duration == TEMPORALINST) 
        str2 = temporalinst_to_string((TemporalInst *)temp, &region_ewkt_out);
    else if (temp->duration == TEMPORALI) 
        str2 = temporali_to_string((TemporalI *)temp, &region_ewkt_out);
    else if (temp->duration == TEMPORALSEQ) 
        str2 = temporalseq_to_string((TemporalSeq *)temp, false, &region_ewkt_out);
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
        textarr[i] = tgeo_as_text_internal(temparr[i], true);
    ArrayType *result = textarr_to_array(textarr, count);

    pfree(temparr);
    for (int i = 0; i < count; i++)
        pfree(textarr[i]);
    pfree(textarr);
    PG_FREE_IF_COPY(array, 0);

    PG_RETURN_ARRAYTYPE_P(result);
}

PG_FUNCTION_INFO_V1(tgeoarr_as_transform);

PGDLLEXPORT Datum
tgeoarr_as_transform(PG_FUNCTION_ARGS)
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
        textarr[i] = tgeo_as_text_internal(temparr[i], false);
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