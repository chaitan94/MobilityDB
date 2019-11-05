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

/* 
 * This function is used to remove the time part from the sample rows after
 * getting the statistics from the time dimension, to be able to collect
 * spatial statistics in the same stats variable.
 */
static HeapTuple
tgeo_remove_timedim(HeapTuple tuple, TupleDesc tupDesc, int tupattnum, 
    int natts, Datum value)
{
    Datum *replValues = (Datum *) palloc(natts * sizeof(Datum));
    bool *replIsnull = (bool *) palloc(natts * sizeof(bool));
    bool *doReplace = (bool *) palloc(natts * sizeof(bool));

    for (int i = 0; i < natts; i++)
    {
        /* tupattnum is 1-based */
        if (i == tupattnum - 1)
        {
            replValues[i] = 0; // TODO: tgeo_values_internal(DatumGetTemporal(value));
            replIsnull[i] = false;
            doReplace[i] = true;
        }
        else
        {
            replValues[i] = 0;
            replIsnull[i] = false;
            doReplace[i] = false;
        }
    }
    HeapTuple result = heap_modify_tuple(tuple, tupDesc, replValues, replIsnull, doReplace);
    pfree(replValues); pfree(replIsnull); pfree(doReplace);
    return result;
}

static void
tgeo_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
                     int samplerows, double totalrows)
{
    MemoryContext old_context;
    int duration = TYPMOD_GET_DURATION(stats->attrtypmod);
    int stawidth;

    /* Compute statistics for the time component */
    if (duration == TEMPORALINST)
        temporalinst_compute_stats(stats, fetchfunc, samplerows, totalrows);
    else
        temporals_compute_stats(stats, fetchfunc, samplerows, totalrows);

    stawidth = stats->stawidth;

    /* Must copy the target values into anl_context */
    old_context = MemoryContextSwitchTo(stats->anl_context);

    /* Remove time component for the tuples */
    for (int i = 0; i < samplerows; i++)
    {
        bool isnull;
        Datum value = fetchfunc(stats, i, &isnull);
        if (isnull)
            continue;
        stats->rows[i] = tgeo_remove_timedim(stats->rows[i],  
            stats->tupDesc, stats->tupattnum, stats->tupDesc->natts, value);
    }

    /* Compute statistics for the geometry component */
    call_function1(gserialized_analyze_nd, PointerGetDatum(stats));
    stats->compute_stats(stats, fetchfunc, samplerows, totalrows);

    /* Put the total width of the column, variable size */
    stats->stawidth = stawidth;
    
    /* Switch back to the previous context */
    MemoryContextSwitchTo(old_context);

    return;
}

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
    temporal_duration_all_is_valid(duration);
    if (duration != TEMPORALINST)
        temporal_extra_info(stats);

    /* Set the callback function to compute statistics. */
    stats->compute_stats = tgeo_compute_stats;
    PG_RETURN_BOOL(true);
}

/*****************************************************************************/
