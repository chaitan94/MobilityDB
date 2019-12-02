#include "tgeo_transform.h"

#include <libpq/pqformat.h>
#include <executor/spi.h>
#include <liblwgeom.h>
#include <math.h>

#include "oidcache.h"
#include "tgeo.h"
#include "temporalinst.h"
#include "temporalseq.h"
#include "temporal_util.h"
#include "tgeo_parser.h"
#include "tgeo_spatialfuncs.h"

/*****************************************************************************
 * Input/Output functions for rtransform
 *****************************************************************************/

/*
 * Input function.
 * Example of input:
 *      (1.1, 1.2, 3.5)
 *      (theta, tx, ty)
 */
PG_FUNCTION_INFO_V1(rtransform_in);

PGDLLEXPORT Datum
rtransform_in(PG_FUNCTION_ARGS)
{
    char *str = PG_GETARG_CSTRING(0);
    rtransform *result = rtransform_parse(&str);
    if (result == NULL)
        PG_RETURN_NULL();
    PG_RETURN_POINTER(result);
}

/* Output function */

PG_FUNCTION_INFO_V1(rtransform_out);

PGDLLEXPORT Datum
rtransform_out(PG_FUNCTION_ARGS)
{
    rtransform *rt = PG_GETARG_RTRANSFORM(0);
    char *result = psprintf("RTransform(%g, %g, %g)", rt->theta, rt->tx, rt->ty);
    PG_RETURN_CSTRING(result);
}

/* Receive function */

PG_FUNCTION_INFO_V1(rtransform_recv);

PGDLLEXPORT Datum
rtransform_recv(PG_FUNCTION_ARGS)
{
    StringInfo buf = (StringInfo) PG_GETARG_POINTER(0);
    rtransform *result;

    result = (rtransform *) palloc(sizeof(rtransform));
    result->theta = pq_getmsgfloat8(buf);
    result->tx = pq_getmsgfloat8(buf);
    result->ty = pq_getmsgfloat8(buf);
    PG_RETURN_POINTER(result);
}

/* Send function */

PG_FUNCTION_INFO_V1(rtransform_send);

PGDLLEXPORT Datum
rtransform_send(PG_FUNCTION_ARGS)
{
    rtransform  *rt = PG_GETARG_RTRANSFORM(0);
    StringInfoData buf;

    pq_begintypsend(&buf);
    pq_sendfloat8(&buf, rt->theta);
    pq_sendfloat8(&buf, rt->tx);
    pq_sendfloat8(&buf, rt->ty);
    PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

/*****************************************************************************
 * Constructor functions
 *****************************************************************************/

rtransform *
rtransform_make(double theta, double tx, double ty)
{
    if (theta < -M_PI || theta > M_PI)
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("Rotation theta must be between -pi and pi (got: %f)", theta)));
    /* If we want a unique representation for rtransform */
    if (theta == -M_PI)
        theta = M_PI;

    rtransform *result = (rtransform *)palloc(sizeof(rtransform));
    result->theta = theta;
    result->tx = tx;
    result->ty = ty;
    return result;
}

rtransform *
rtransform_combine(rtransform *rt1, rtransform *rt2)
{
    double new_theta = rt1->theta + rt2->theta;
    if (new_theta > M_PI)
        new_theta = new_theta - 2*M_PI;
    else if (new_theta <= -M_PI)
        new_theta = new_theta + 2*M_PI;
    return rtransform_make(new_theta, rt1->tx + rt2->tx, rt1->ty + rt2->ty);
}

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

void
apply_affine_transform(LWGEOM *geom, 
    double a, double b, double c, 
    double d, double e, double f, 
    double g, double h, double i, 
    double xoff, double yoff, double zoff)
{
    AFFINE affine;
    affine.afac =  a;
    affine.bfac =  b;
    affine.cfac =  c;
    affine.dfac =  d;
    affine.efac =  e;
    affine.ffac =  f;
    affine.gfac =  g;
    affine.hfac =  h;
    affine.ifac =  i;
    affine.xoff =  xoff;
    affine.yoff =  yoff;
    affine.zoff =  zoff;
    lwgeom_affine(geom, &affine);
}

void
apply_rtransform(LWGEOM *region, const rtransform *rt)
{
    double a = cos(rt->theta);
    double b = sin(rt->theta);

    LWPOINT *centroid = lwgeom_as_lwpoint(lwgeom_centroid(region));
    double centroid_tx = lwpoint_get_x(centroid);
    double centroid_ty = lwpoint_get_x(centroid);

    /* Translate to have centroid at (0,0) */
    apply_affine_transform(region, 1, 0, 0, 0, 1, 0, 0, 0, 1, -centroid_tx, -centroid_ty, 0);
    /* Apply tranform */
    apply_affine_transform(region, a, -b, 0, b, a, 0, 0, 0, 1, rt->tx, rt->ty, 0);
    /* Translate back */
    apply_affine_transform(region, 1, 0, 0, 0, 1, 0, 0, 0, 1, centroid_tx, centroid_ty, 0);
    if (region->bbox)
    {
        lwgeom_refresh_bbox(region);
    }
}

rtransform *
rtransform_interpolate(const rtransform *rt1, const rtransform *rt2, double ratio)
{
    if (ratio < 0 || ratio > 1)
        ereport(ERROR, (errcode(ERRCODE_INTERNAL_ERROR),
            errmsg("Interpolation ratio must be between 0 and 1 (got: %f)", ratio)));

    if (rt2->theta < rt1->theta)
        return rtransform_interpolate(rt2, rt1, 1-ratio);

    double new_theta;
    if (fabs(rt2->theta - rt1->theta) <= M_PI)
    {
        new_theta = rt1->theta + (rt2->theta - rt1->theta)*ratio;
    }
    else 
    {
        new_theta = rt2->theta + (rt1->theta - rt2->theta + 2*M_PI)*ratio;
        if (new_theta > M_PI)
        {
            new_theta = new_theta - 2*M_PI;
        }
    }
    double new_tx = rt1->tx + (rt2->tx - rt1->tx)*ratio;
    double new_ty = rt1->ty + (rt2->ty - rt1->ty)*ratio;
    return rtransform_make(new_theta, new_tx, new_ty);
}

/*****************************************************************************
 * Compute rtransform from 2 regions
 *****************************************************************************/

rtransform *
rtransform_compute(LWGEOM *region_1, LWGEOM *region_2)
{
    LWPOINT *centroid_1 = lwgeom_as_lwpoint(lwgeom_centroid(region_1));
    double centroid_1_tx = lwpoint_get_x(centroid_1);
    double centroid_1_ty = lwpoint_get_x(centroid_1);

    /* Translate both to have centroid_1 at (0,0) */
    apply_affine_transform(region_1, 1, 0, 0, 0, 1, 0, 0, 0, 1, -centroid_1_tx, -centroid_1_ty, 0);
    apply_affine_transform(region_2, 1, 0, 0, 0, 1, 0, 0, 0, 1, -centroid_1_tx, -centroid_1_ty, 0);

    POINTARRAY *point_arr_1;
    POINTARRAY *point_arr_2;

    if (region_1->type == POLYGONTYPE)
    {
        point_arr_1 = ((LWPOLY *) region_1)->rings[0];
    }
    else if (region_1->type == LINETYPE)
    {
        point_arr_1 = ((LWLINE *) region_1)->points;
    }
    if (region_2->type == POLYGONTYPE)
    {
        point_arr_2 = ((LWPOLY *) region_2)->rings[0];
    }
    else if (region_2->type == LINETYPE)
    {
        point_arr_2 = ((LWLINE *) region_2)->points;
    }

    POINT2D p11 = getPoint2d(point_arr_1, 0);
    POINT2D p12 = getPoint2d(point_arr_1, 1);
    POINT2D p21 = getPoint2d(point_arr_2, 0);
    POINT2D p22 = getPoint2d(point_arr_2, 1);

    double x1 = p11.x, y1 = p11.y;
    double x2 = p12.x, y2 = p12.y;
    double x1_ = p21.x, y1_ = p21.y;
    double x2_ = p22.x, y2_ = p22.y;
    double a, b, c, d;

    // TODO division by 0

    /* Compute affine tranformation from region_1 to region_2 */
    a = ((x1_ - x2_)*(x1 - x2) + (y1_ - y2_)*(y1 - y2))/((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
    b = ((y1_ - y2_)*(x1 - x2) - (x1_ - x2_)*(y1 - y2))/((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
    c = x1_ - a*x1 + b*y1;
    d = y1_ - a*y1 - b*x1;

    double theta = atan2(b, a);

    /* Translate back (we ideally don't want to modify the regions, but is probably less costly than working on copies) */
    apply_affine_transform(region_1, 1, 0, 0, 0, 1, 0, 0, 0, 1, centroid_1_tx, centroid_1_ty, 0);
    apply_affine_transform(region_2, 1, 0, 0, 0, 1, 0, 0, 0, 1, centroid_1_tx, centroid_1_ty, 0);
    
    return rtransform_make(theta, c, d);
}

/*****************************************************************************
 * Cast functions
 *****************************************************************************/

/* 
Computes the transformations for all instants with respect to the first instant
Raises an error if the regions are not colinear enough
Creates a new array of instants, does not free old array
 */

TemporalInst **
geo_instarr_to_rtransform(TemporalInst **instants, int count)
{
    TemporalInst *firstInst = instants[0];
    Datum firstValue = temporalinst_value(firstInst);
    GSERIALIZED *firstGs = (GSERIALIZED *) DatumGetPointer(firstValue);
    LWGEOM *firstLwgeom = lwgeom_from_gserialized(firstGs);
    TemporalInst **newInstants = palloc(sizeof(TemporalInst *) * count);;
    newInstants[0] = (TemporalInst *) temporal_copy((Temporal *) firstInst);
    for (int i = 1; i < count; ++i)
    {
        /* 
        Compute transformation
        Compute tranformed region
        Compare regions
        Create new instant if ok, else throw error
         */
        LWGEOM *firstLwgeomCopy = lwgeom_clone_deep(firstLwgeom);
        Datum value = temporalinst_value((TemporalInst *) instants[i]);
        GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(value);
        LWGEOM *lwgeom = lwgeom_from_gserialized(gs);
        rtransform *rt = rtransform_compute(firstLwgeomCopy, lwgeom);
        apply_rtransform(firstLwgeomCopy, rt);
        if (!lwgeom_similar(firstLwgeomCopy, lwgeom))
            ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
                        errmsg("All regions must be congruent")));
        newInstants[i] = temporalinst_make(RtransformGetDatum(rt), instants[i]->t, type_oid(T_RTRANSFORM));
        pfree(rt);
        lwgeom_free(firstLwgeomCopy);
        lwgeom_free(lwgeom);
    }
    lwgeom_free(firstLwgeom);
    return newInstants;
}

/* 
Computes the transformations for all first instants of each sequence 
with respect to the first instant of the first sequence
Raises an error if the regions are not colinear enough
Creates a new array of sequences, does not free old array
 */

TemporalSeq **
geo_seqarr_to_rtransform(TemporalSeq **sequences, int count)
{
    TemporalSeq *firstSeq = sequences[0];
    TemporalInst *firstInst = temporalseq_inst_n(firstSeq, 0);
    Datum firstValue = temporalinst_value(firstInst);
    GSERIALIZED *firstGs = (GSERIALIZED *) DatumGetPointer(firstValue);
    LWGEOM *firstLwgeom = lwgeom_from_gserialized(firstGs);
    TemporalSeq **newSequences = palloc(sizeof(TemporalSeq *) * count);;
    newSequences[0] = temporalseq_copy(firstSeq);
    for (int i = 1; i < count; ++i)
    {
        /* 
        Compute transformation
        Compute tranformed region
        Compare regions
        Create new instant if ok, else throw error
         */
        LWGEOM *firstLwgeomCopy = lwgeom_clone_deep(firstLwgeom);
        TemporalInst *instant = temporalseq_inst_n(sequences[i], 0);
        Datum value = temporalinst_value(instant);
        GSERIALIZED *gs = (GSERIALIZED *) DatumGetPointer(value);
        LWGEOM *lwgeom = lwgeom_from_gserialized(gs);
        rtransform *rt = rtransform_compute(firstLwgeomCopy, lwgeom);
        apply_rtransform(firstLwgeomCopy, rt);
        if (!lwgeom_similar(firstLwgeomCopy, lwgeom))
            ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION), 
                        errmsg("All regions must be congruent")));
        rtransform **newRts = palloc(sizeof(rtransform *) * sequences[i]->count);
        TemporalInst **newInstants = palloc(sizeof(TemporalInst *) * sequences[i]->count);
        newRts[0] = rt;
        TemporalInst *newInstant = temporalinst_make(RtransformGetDatum(rt), instant->t, type_oid(T_RTRANSFORM));
        newInstants[0] = newInstant;
        for (int j = 1; j < sequences[i]->count; j++)
        {
            instant = temporalseq_inst_n(sequences[i], j);
            rtransform *old_rt = DatumGetRtransform(temporalinst_value(instant));
            newRts[j] = rtransform_combine(old_rt, rt);
            newInstants[j] = temporalinst_make(RtransformGetDatum(newRts[j]), instant->t, instant->valuetypid);
        }

        // Create new sequence, copy flags, instants, period and bbox
        size_t bboxsize = sizeof(STBOX);
        size_t memsize = double_pad(bboxsize);
        for (int j = 0; j < sequences[i]->count; j++)
            memsize += double_pad(VARSIZE(newInstants[j]));
        size_t pdata = double_pad(sizeof(TemporalSeq)) + (sequences[i]->count + 1) * sizeof(size_t);
        newSequences[i] = palloc0(pdata + memsize);
        SET_VARSIZE(newSequences[i], pdata + memsize);
        newSequences[i]->count = sequences[i]->count;
        newSequences[i]->valuetypid = type_oid(T_RTRANSFORM);
        newSequences[i]->duration = TEMPORALSEQ;
        memcpy((char *) &newSequences[i]->period, (char *) &sequences[i]->period, sizeof(Period));
        MOBDB_FLAGS_SET_LINEAR(newSequences[i]->flags, MOBDB_FLAGS_GET_LINEAR(sequences[i]->flags));
        // Set Z or Geodetic?
        size_t pos = 0;
        for (int j = 0; j < sequences[i]->count; j++)
        {
            memcpy(((char *)newSequences[i]) + pdata + pos, newInstants[j], 
                VARSIZE(newInstants[j]));
            newSequences[i]->offsets[j] = pos;
            pos += double_pad(VARSIZE(newInstants[j]));
        }
        void *bbox = ((char *) newSequences[i]) + pdata + pos;
        temporalseq_bbox(bbox, sequences[i]);
        newSequences[i]->offsets[sequences[i]->count] = pos;

        for (int j = 0; j < sequences[i]->count; ++j)
        {
            pfree(newRts[j]);
            pfree(newInstants[j]);
        }
        pfree(newRts);
        pfree(newInstants);
        lwgeom_free(firstLwgeomCopy);
        lwgeom_free(lwgeom);
    }
    lwgeom_free(firstLwgeom);
    return newSequences;
}