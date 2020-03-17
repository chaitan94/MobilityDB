/*****************************************************************************
 *
 * tgeo_analyze.c
 *    Functions for gathering statistics from temporal geometry columns
 *
 * Various kind of statistics are collected for both the value and the time
 * dimensions of temporal types. The kind of statistics depends on the duration
 * of the temporal type, which is defined in the table schema by the typmod
 * attribute. Please refer to the PostgreSQL file pg_statistic_d.h and the
 * PostGIS file gserialized_estimate.c for more information about the 
 * statistics collected.
 * 
 * For the spatial dimension, the statistics collected are the same for all 
 * durations. These statistics are obtained by calling the PostGIS function
 * gserialized_analyze_nd.
 * - Slot 1
 *      - stakind contains the type of statistics which is STATISTIC_SLOT_2D.
 *      - stanumbers stores the 2D histrogram of occurrence of features.
 * - Slot 2
 *      - stakind contains the type of statistics which is STATISTIC_SLOT_ND.
 *      - stanumbers stores the ND histrogram of occurrence of features.
 * For the time dimension, the statistics collected in Slots 3 and 4 depend on 
 * the duration. Please refer to file temporal_analyze.c for more information.
 * 
 * Portions Copyright (c) 2019, Esteban Zimanyi, Mahmoud Sakr, Mohamed Bakli,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tgeo_analyze.h"

#include <assert.h>
#include <access/htup_details.h>

#include "temporal.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "temporal_analyze.h"
#include "postgis.h"
#include "tgeo.h"

/*****************************************************************************/

PG_FUNCTION_INFO_V1(tgeo_analyze);

PGDLLEXPORT Datum
tgeo_analyze(PG_FUNCTION_ARGS)
{
    VacAttrStats *stats = (VacAttrStats *) PG_GETARG_POINTER(0);
    int duration;

    /*
     * Call the standard typanalyze function.  It may fail to find needed
     * operators, in which case we also can't do anything, so just fail.
     */
    if (!std_typanalyze(stats))
        PG_RETURN_BOOL(false);

    /* 
     * Ensure duration is valid and collect extra information about the 
     * temporal type and its base and time types.
     */
    duration = TYPMOD_GET_DURATION(stats->attrtypmod);
    ensure_valid_duration_all(duration);
    if (duration != TEMPORALINST)
        temporal_extra_info(stats);

    /* Set the callback function to compute statistics. */
    // TODO
    /*stats->compute_stats = tgeo_compute_stats;*/
    PG_RETURN_BOOL(true);
}

/*****************************************************************************/
