#include "tgeo_boxops.h"

#include <assert.h>
#include <utils/builtins.h>
#include <utils/timestamp.h>
#include <math.h>
#include <float.h>

#include "timestampset.h"
#include "oidcache.h"
#include "periodset.h"
#include "temporaltypes.h"
#include "temporal_util.h"
#include "stbox.h"
#include "tgeo.h"
#include "tgeo_transform.h"
#include "tpoint_boxops.h"

/*****************************************************************************/

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

/*****************************************************************************/

/* Transform a geometry/geography to a stbox
 * Assumes that we take the rotation invariant stbox,
 * meaning we can rotate the region without going out of the stbox
 */

bool
rotating_geo_to_stbox_internal(STBOX *box, GSERIALIZED *gs)
{
    POINTARRAY *pa;
    POINT4D p;
    POINT4D centroid;
    double d = 0;
    LWPOLY *poly = (LWPOLY *) lwgeom_from_gserialized(gs);
    lwpoint_getPoint4d_p((LWPOINT *) lwgeom_centroid((LWGEOM *) poly), &centroid);
    pa = poly->rings[0];
    for (uint i = 0 ; i < pa->npoints; i++ )
    {
        getPoint4d_p(pa, i, &p);
        d = Max(d, sqrt(pow(centroid.x - p.x, 2) + pow(centroid.y - p.y, 2)));
    }
    lwpoly_free(poly);

    box->xmin = centroid.x - d;
    box->xmax = centroid.x + d;
    box->ymin = centroid.y - d;
    box->ymax = centroid.y + d;
    box->srid = gserialized_get_srid(gs);
    MOBDB_FLAGS_SET_X(box->flags, true);
    MOBDB_FLAGS_SET_Z(box->flags, false);
    MOBDB_FLAGS_SET_T(box->flags, false);
    MOBDB_FLAGS_SET_GEODETIC(box->flags, FLAGS_GET_GEODETIC(gs->flags));
    return true;
}

/*****************************************************************************/

/* Functions computing the bounding box at the creation of a temporal geometry */

void
tregioninst_make_stbox(STBOX *box, Datum value, TimestampTz t, bool rotating)
{
    GSERIALIZED *gs = (GSERIALIZED *)PointerGetDatum(value);
    if (rotating)
        assert(rotating_geo_to_stbox_internal(box, gs));
    else
        assert(geo_to_stbox_internal(box, gs));
    box->tmin = box->tmax = t;
    MOBDB_FLAGS_SET_T(box->flags, true);
    return;
}

/* TemporalInst values do not have a precomputed bounding box */
void
tregioninstarr_to_stbox(STBOX *box, TemporalInst **instants, int count, bool rotating)
{
    Datum value = temporalinst_value(instants[0]);
    tregioninst_make_stbox(box, value, instants[0]->t, rotating);
    TemporalInst *referenceInst = instants[0];
    for (int i = 1; i < count; i++)
    {
        STBOX box1;
        memset(&box1, 0, sizeof(STBOX));
        if (instants[i]->valuetypid == type_oid(T_GEOGRAPHY) || 
            instants[i]->valuetypid == type_oid(T_GEOMETRY))
        {
            // Compute bbox of next instant
            referenceInst = instants[i];
            value = temporalinst_value(instants[i]);
            tregioninst_make_stbox(&box1, value, instants[i]->t, rotating);
        } 
        else if (instants[i]->valuetypid == type_oid(T_RTRANSFORM))
        {
            // First transform next instant into geometry, then compute its bbox
            // 
            // When rotating = true, this could be optimized by copying 
            // the bbox of the initial instant and simply translating it based on
            // the current rtransform, since the bbox is symmetric around the centroid
            TemporalInst *inst = tgeoinst_rtransfrom_to_region(instants[i], referenceInst);
            value = temporalinst_value(inst);
            tregioninst_make_stbox(&box1, value, inst->t, rotating);
            pfree(inst);
        }
        stbox_expand(box, &box1);
    }
    return;
}