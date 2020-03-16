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

/* Transform a geometry/geography to a stbox
 * Assumes that we take the rotation invariant stbox,
 * meaning we can rotate the region without going out of the stbox
 */

bool
rotating_geo_to_stbox_internal(STBOX *box, GSERIALIZED *gs)
{
    int geo_type = gserialized_get_type(gs);
    bool has_z = FLAGS_GET_Z(gs->flags);
    POINTARRAY *pa;
    POINT4D p;
    POINT4D centroid;
    double d = 0;
    double zmin = -1*FLT_MAX;
    double zmax = FLT_MAX;
    LWLINE *line;
    LWPOLY *poly;
    if (geo_type == LINETYPE)
    {
        line = (LWLINE *) lwgeom_from_gserialized(gs);
        lwpoint_getPoint4d_p((LWPOINT *) lwgeom_centroid((LWGEOM *) line), &centroid);
        pa = line->points;
    }
    else if (geo_type == POLYGONTYPE)
    {
        poly = (LWPOLY *) lwgeom_from_gserialized(gs);
        lwpoint_getPoint4d_p((LWPOINT *) lwgeom_centroid((LWGEOM *) poly), &centroid);
        pa = poly->rings[0];
    }
    for (uint i = 0 ; i < pa->npoints; i++ )
    {
        getPoint4d_p(pa, i, &p);
        d = Max(d, sqrt(pow(centroid.x - p.x, 2) + pow(centroid.y - p.y, 2)));
        if ( has_z )
        {
            zmin = Min(zmin, p.z);
            zmax = Max(zmax, p.z);
        }
    }
    if (geo_type == LINETYPE)
        lwline_free(line);
    else if (geo_type == POLYGONTYPE)
        lwpoly_free(poly);

    box->xmin = centroid.x - d;
    box->xmax = centroid.x + d;
    box->ymin = centroid.y - d;
    box->ymax = centroid.y + d;
    if (has_z || FLAGS_GET_GEODETIC(gs->flags))
    {
        box->zmin = zmin;
        box->zmax = zmax;
    }
    MOBDB_FLAGS_SET_X(box->flags, true);
    MOBDB_FLAGS_SET_Z(box->flags, has_z);
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
            referenceInst = instants[i];
            value = temporalinst_value(instants[i]);
            tregioninst_make_stbox(&box1, value, instants[i]->t, rotating);
        } 
        else if (instants[i]->valuetypid == type_oid(T_RTRANSFORM))
        {
            TemporalInst *inst = tgeoinst_rtransfrom_to_region(instants[i], referenceInst);
            value = temporalinst_value(inst);
            tregioninst_make_stbox(&box1, value, inst->t, rotating);
            pfree(inst);
        }
        stbox_expand(box, &box1);
    }
    return;
}