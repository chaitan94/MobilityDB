/*****************************************************************************
 *
 * tpoint_spatialfuncs.c
 *    Spatial functions for temporal points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tgeo_spatialfuncs.h"

#include <assert.h>
#include <float.h>
#include <utils/builtins.h>
#include <utils/timestamp.h>

#include "temporaltypes.h"

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

/* Compare two geometries from serialized geometries */

bool
datum_geometry_eq(Datum geometry1, Datum geometry2)
{
    GSERIALIZED *gs1 = (GSERIALIZED *)PointerGetDatum(geometry1);
    GSERIALIZED *gs2 = (GSERIALIZED *)PointerGetDatum(geometry2);
    assert(gserialized_get_srid(gs1) == gserialized_get_srid(gs2) &&
        FLAGS_GET_Z(gs1->flags) == FLAGS_GET_Z(gs2->flags) &&
        FLAGS_GET_GEODETIC(gs1->flags) == FLAGS_GET_GEODETIC(gs2->flags));
    // TODO
    return false;
}

/* 
 * Test that two geometries are sufficiently similar
 * This is used to test congruence of regions
 */

// Rename

bool
lwgeom_similar(const LWGEOM *geometry1, const LWGEOM *geometry2)
{
    // SRID, Z, M, .. should already have been tested (or add them here?)
    // All geometries should already have the same type
    // All geometries should have the same amount of points and they should be in the same order
    // This function just tests that the corresponding points of each geometry are equal 
    // (up to an error of epsilon for each coordinate)
    // Is this enough? (can we have for example two different interior ring, but have the same sequence of points?)

    LWPOINTITERATOR *it1 = lwpointiterator_create(geometry1);
    LWPOINTITERATOR *it2 = lwpointiterator_create(geometry2);
    POINT4D p1;
    POINT4D p2;

    bool result = true;
    while (lwpointiterator_next(it1, &p1) 
        && lwpointiterator_next(it2, &p2) 
        && result) 
    {
        if (FLAGS_GET_Z(geometry1->flags))
        {
            result = fabs(p1.x - p2.x) < EPSILON 
                  && fabs(p1.y - p2.y) < EPSILON 
                  && fabs(p1.z - p2.z) < EPSILON;
        }
        else
        {
            result = fabs(p1.x - p2.x) < EPSILON 
                  && fabs(p1.y - p2.y) < EPSILON;
        }
    }

    lwpointiterator_destroy(it1);
    lwpointiterator_destroy(it2);
    return result;
}

/*****************************************************************************/
