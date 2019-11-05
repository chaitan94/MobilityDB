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

#include "tpoint_spatialfuncs.h"

#include <assert.h>
#include <float.h>
#include <utils/builtins.h>
#include <utils/timestamp.h>

#include "periodset.h"
#include "timeops.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "lifting.h"
#include "temporal_mathfuncs.h"
#include "tpoint.h"
#include "tpoint_boxops.h"
#include "tpoint_distance.h"

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

/* Compare two geometries from serialized geometries */

bool
datum_geometry_eq(Datum geometry1, Datum geometry2)
{
    // TODO
    return false;
}

/*****************************************************************************/
