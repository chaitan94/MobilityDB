/*****************************************************************************
 *
 * tgeo_parser.c
 *    Functions for parsing temporal geometries and geographies.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tgeo_parser.h"

#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_parser.h"
#include "tgeo.h"
#include "tgeo_transform.h"

/*****************************************************************************/

rtransform *
rtransform_parse(char **str)
{
    p_whitespace(str);

    if (strncasecmp(*str,"RTRANSFORM",10) != 0)
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            errmsg("Could not parse region transform")));

    *str += 10;
    p_whitespace(str);

    int delim = 0;
    while ((*str)[delim] != ')' && (*str)[delim] != '\0')
        delim++;
    if ((*str)[delim] == '\0')
        ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
            errmsg("Could not parse region transform")));

    double theta;
    double tx;
    double ty;
    if (sscanf(*str, "( %lf , %lf , %lf )", &theta, &tx, &ty) != 3)
        ereport(ERROR,
                (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
                        errmsg("Could not parse region transform")));

    *str += delim + 1;

    return rtransform_make(theta, tx, ty);
}

static TemporalInst *
tgeoinst_parse(char **str, Oid basetype, bool end, int *tgeo_srid) 
{
    p_whitespace(str);
    /* The next instruction will throw an exception if it fails */
    Datum geo = basetype_parse(str, basetype);
    GSERIALIZED *gs = (GSERIALIZED *)PG_DETOAST_DATUM(geo);
    int geo_srid = gserialized_get_srid(gs);
    int geo_type = gserialized_get_type(gs);
    if (((geo_type != LINETYPE) && (geo_type != POLYGONTYPE)) || gserialized_is_empty(gs) ||
        FLAGS_GET_M(gs->flags) || FLAGS_GET_Z(gs->flags))
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Only non-empty linestring or polygon geometries without Z or M dimension accepted")));
    if (*tgeo_srid != SRID_UNKNOWN && geo_srid != SRID_UNKNOWN && *tgeo_srid != geo_srid)
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Geometry SRID (%d) does not match temporal type SRID (%d)", 
            geo_srid, *tgeo_srid)));
    if (basetype == type_oid(T_GEOMETRY)) // GEOMETRY
    {
        if (*tgeo_srid != SRID_UNKNOWN && geo_srid == SRID_UNKNOWN)
            gserialized_set_srid(gs, *tgeo_srid);
        if (*tgeo_srid == SRID_UNKNOWN && geo_srid != SRID_UNKNOWN)
            *tgeo_srid = geo_srid;
    }
    else                                  // GEOGRAPHY
    {
        if (*tgeo_srid != SRID_UNKNOWN && geo_srid == SRID_DEFAULT)
            gserialized_set_srid(gs, *tgeo_srid);
        if (*tgeo_srid == SRID_UNKNOWN && geo_srid != SRID_DEFAULT)
            *tgeo_srid = geo_srid;
    }
    /* The next instruction will throw an exception if it fails */
    TimestampTz t = timestamp_parse(str);
    if (end)
    {
        /* Ensure there is no more input */
        p_whitespace(str);
        if (**str != 0)
            ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
                errmsg("Could not parse temporal value")));
    }
    TemporalInst *result = temporalinst_make(PointerGetDatum(gs), t, basetype);
    pfree(gs);
    return result;
}

static TemporalI *
tgeoi_parse(char **str, Oid basetype, int *tgeo_srid) 
{
    p_whitespace(str);
    /* We are sure to find an opening brace because that was the condition 
     * to call this function in the dispatch function tpoint_parse */
    p_obrace(str);

    //FIXME: parsing twice
    char *bak = *str;
    TemporalInst *inst = tgeoinst_parse(str, basetype, false, tgeo_srid);
    int count = 1;
    while (p_comma(str)) 
    {
        count++;
        pfree(inst);
        inst = tgeoinst_parse(str, basetype, false, tgeo_srid);
    }
    pfree(inst);
    if (!p_cbrace(str))
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Could not parse temporal value")));
    /* Ensure there is no more input */
    p_whitespace(str);
    if (**str != 0)
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Could not parse temporal value")));
    /* Second parsing */
    *str = bak;
    TemporalInst **insts = palloc(sizeof(TemporalInst *) * count);
    for (int i = 0; i < count; i++) 
    {
        p_comma(str);
        insts[i] = tgeoinst_parse(str, basetype, false, tgeo_srid);
    }
    p_cbrace(str);
    TemporalI *result = temporali_from_temporalinstarr(insts, count);

    for (int i = 0; i < count; i++)
        pfree(insts[i]);
    pfree(insts);

    return result;
}

static TemporalSeq *
tgeoseq_parse(char **str, Oid basetype, bool linear, bool end, int *tgeo_srid) 
{
    p_whitespace(str);
    bool lower_inc = false, upper_inc = false;
    /* We are sure to find an opening bracket or parenthesis because that was 
     * the condition to call this function in the dispatch function tpoint_parse */
    if (p_obracket(str))
        lower_inc = true;
    else if (p_oparen(str))
        lower_inc = false;

    // FIXME: I pre-parse to have the count, then re-parse. This is the only
    // approach I see at the moment which is both correct and simple
    char *bak = *str;
    TemporalInst *inst = tgeoinst_parse(str, basetype, false, tgeo_srid);
    int count = 1;
    while (p_comma(str)) 
    {
        count++;
        pfree(inst);
        inst = tgeoinst_parse(str, basetype, false, tgeo_srid);
    }
    pfree(inst);
    if (p_cbracket(str))
        upper_inc = true;
    else if (p_cparen(str))
        upper_inc = false;
    else
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Could not parse temporal value")));
    if (end)
    {
        /* Ensure there is no more input */
        p_whitespace(str);
        if (**str != 0)
            ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
                errmsg("Could not parse temporal value")));
    }
    /* Second parsing */
    *str = bak; 
    TemporalInst **insts = palloc(sizeof(TemporalInst *) * count);
    for (int i = 0; i < count; i++) 
    {
        p_comma(str);
        insts[i] = tgeoinst_parse(str, basetype, false, tgeo_srid);
    }

    p_cbracket(str);
    p_cparen(str);

    TemporalSeq *result = temporalseq_from_temporalinstarr(insts, 
        count, lower_inc, upper_inc, linear, true);

    for (int i = 0; i < count; i++)
        pfree(insts[i]);
    pfree(insts);

    return result;
}

static TemporalS *
tgeos_parse(char **str, Oid basetype, bool linear, int *tgeo_srid) 
{
    p_whitespace(str);
    /* We are sure to find an opening brace because that was the condition 
     * to call this function in the dispatch function tpoint_parse */
    p_obrace(str);

    //FIXME: parsing twice
    char *bak = *str;
    TemporalSeq *seq = tgeoseq_parse(str, basetype, linear, false, tgeo_srid);
    int count = 1;
    while (p_comma(str)) 
    {
        count++;
        pfree(seq);
        seq = tgeoseq_parse(str, basetype, linear, false, tgeo_srid);
    }
    pfree(seq);
    if (!p_cbrace(str))
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Could not parse temporal value")));
    /* Ensure there is no more input */
    p_whitespace(str);
    if (**str != 0)
        ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
            errmsg("Could not parse temporal value")));
    /* Second parsing */
    *str = bak;
    TemporalSeq **seqs = palloc(sizeof(TemporalSeq *) * count);
    for (int i = 0; i < count; i++) 
    {
        p_comma(str);
        seqs[i] = tgeoseq_parse(str, basetype, linear, false, tgeo_srid);
    }
    p_cbrace(str);
    TemporalS *result = temporals_from_temporalseqarr(seqs, count, 
        linear, true);

    for (int i = 0; i < count; i++)
        pfree(seqs[i]);
    pfree(seqs);

    return result;
}

Temporal *
tgeo_parse(char **str, Oid basetype) 
{
    int tgeo_srid = 0;
    p_whitespace(str);
    
    /* Starts with "SRID=". The SRID specification must be gobbled for all 
     * durations excepted TemporalInst. We cannot use the atoi() function
     * because this requires a string terminated by '\0' and we cannot 
     * modify the string in case it must be passed to the tgeoinst_parse
     * function. */
    char *bak = *str;
    if (strncasecmp(*str,"SRID=",5) == 0)
    {
        /* Move str to the start of the numeric part */
        *str += 5;
        int delim = 0;
        tgeo_srid = 0;
        /* Delimiter will be either ',' or ';' depending on whether interpolation 
           is given after */
        while ((*str)[delim] != ',' && (*str)[delim] != ';' && (*str)[delim] != '\0')
        {
            tgeo_srid = tgeo_srid * 10 + (*str)[delim] - '0'; 
            delim++;
        }
        /* Set str to the start of the temporal geo */
        *str += delim + 1;
    }
    /* We cannot ensure that the SRID is geodetic for geography since
     * the srid_is_latlong function is not exported by PostGIS
    if (basetype == type_oid(T_GEOGRAPHY))
        srid_is_latlong(fcinfo, tgeo_srid);
     */ 

    bool linear = linear_interpolation(basetype);
    /* Starts with "Interp=Stepwise" */
    if (strncasecmp(*str,"Interp=Stepwise;",16) == 0)
    {
        /* Move str after the semicolon */
        *str += 16;
        linear = false;
    }
    Temporal *result = NULL; /* keep compiler quiet */
    /* Determine the type of the temporal geo */
    if (**str != '{' && **str != '[' && **str != '(')
    {
        /* Pass the SRID specification */
        *str = bak;
        result = (Temporal *)tgeoinst_parse(str, basetype, true, &tgeo_srid);
    }
    else if (**str == '[' || **str == '(')
        result = (Temporal *)tgeoseq_parse(str, basetype, linear, true, &tgeo_srid);        
    else if (**str == '{')
    {
        bak = *str;
        p_obrace(str);
        p_whitespace(str);
        if (**str == '[' || **str == '(')
        {
            *str = bak;
            result = (Temporal *)tgeos_parse(str, basetype, linear, &tgeo_srid);
        }
        else
        {
            *str = bak;
            result = (Temporal *)tgeoi_parse(str, basetype, &tgeo_srid);        
        }
    }
    return result;
}

/*****************************************************************************/
