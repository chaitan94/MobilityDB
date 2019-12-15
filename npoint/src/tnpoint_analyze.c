/*****************************************************************************
 *
 * tnpoint_analyze.c
 *	Functions for gathering statistics from temporal network point columns
 *
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_analyze.h"

#include <commands/vacuum.h>

#include "period.h"
#include "time_analyze.h"
#include "temporal.h"
#include "temporal_analyze.h"
#include "postgis.h"
#include "tpoint.h"
#include "tpoint_selfuncs.h"
#include "tpoint_analyze.h"
#include "tnpoint_spatialfuncs.h"

/*****************************************************************************/

static void
tnpoint_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
	int sample_rows, double total_rows)
{
	int d, i;						/* Counters */
	int notnull_cnt = 0;			/* # not null rows in the sample */
	int null_cnt = 0;				/* # null rows in the sample */
	int	slot_idx = 0;				/* slot for storing the statistics */
	double total_width = 0;			/* # of bytes used by sample */
	ND_BOX sum;						/* Sum of extents of sample boxes */
	const ND_BOX **sample_boxes;	/* ND_BOXes for each of the sample features */
	ND_BOX sample_extent;			/* Extent of the raw sample */
	int   ndims = 2;				/* Dimensionality of the sample */
	float8 *time_lengths;
	PeriodBound *time_lowers,
		   *time_uppers;

	/* Initialize sum */
	nd_box_init(&sum);

	/*
	 * This is where gserialized_analyze_nd
	 * should put its' custom parameters.
	 */
	/* void *mystats = stats->extra_data; */

	/*
	 * We might need less space, but don't think
	 * its worth saving...
	 */
	sample_boxes = palloc(sizeof(ND_BOX *) * sample_rows);

	time_lowers = (PeriodBound *) palloc(sizeof(PeriodBound) * sample_rows);
	time_uppers = (PeriodBound *) palloc(sizeof(PeriodBound) * sample_rows);
	time_lengths = (float8 *) palloc(sizeof(float8) * sample_rows);

	/*
	 * First scan:
	 *  o read boxes
	 *  o find dimensionality of the sample
	 *  o find extent of the sample
	 *  o count null-infinite/not-null values
	 *  o compute total_width
	 *  o compute total features's box area (for avgFeatureArea)
	 *  o sum features box coordinates (for standard deviation)
	 */
	for (i = 0; i < sample_rows; i++)
	{
		Datum value;
		Temporal *temp;
		GSERIALIZED *traj;
		Period period;
		PeriodBound period_lower,
				period_upper;
		GBOX gbox;
		ND_BOX *nd_box;
		bool is_null;
		bool is_copy;

		value = fetchfunc(stats, i, &is_null);

		/* Skip all NULLs. */
		if (is_null)
		{
			null_cnt++;
			continue;
		}

		/* Get temporal point */
		temp = DatumGetTemporal(value);

		/* TO VERIFY */
		is_copy = VARATT_IS_EXTENDED(temp);

		/* How many bytes does this sample use? */
		total_width += VARSIZE(temp);

		/* Get trajectory and period from temporal point */
		traj = (GSERIALIZED *) DatumGetPointer(tnpoint_geom(temp));
		temporal_period(&period, temp);

		/* Remember time bounds and length for further usage in histograms */
		period_deserialize(&period, &period_lower, &period_upper);
		time_lowers[notnull_cnt] = period_lower;
		time_uppers[notnull_cnt] = period_upper;
		time_lengths[notnull_cnt] = period_to_secs(period_upper.val, 
			period_lower.val);

		/* Read the bounds from the trajectory. */
		if (LW_FAILURE == gserialized_get_gbox_p(traj, &gbox))
		{
			/* Skip empties too. */
			continue;
		}

		/* Check bounds for validity (finite and not NaN) */
		if (! gbox_is_valid(&gbox))
		{
			continue;
		}

		/* If we're in 2D/3D mode, zero out the higher dimensions for "safety" 
		 * If we're in 3D mode set ndims to 3 */
		if (! MOBDB_FLAGS_GET_Z(temp->flags))
			gbox.zmin = gbox.zmax = gbox.mmin = gbox.mmax = 0.0;
		else
		{
			gbox.mmin = gbox.mmax = 0.0;
			ndims = 3;
		}

		/* Convert gbox to n-d box */
		nd_box = palloc0(sizeof(ND_BOX));
		nd_box_from_gbox(&gbox, nd_box);

		/* Cache n-d bounding box */
		sample_boxes[notnull_cnt] = nd_box;

		/* Initialize sample extent before merging first entry */
		if (! notnull_cnt)
			nd_box_init_bounds(&sample_extent, ndims);

		/* Add current sample to overall sample extent */
		nd_box_merge(nd_box, &sample_extent, ndims);

		/* Add bounds coordinates to sums for stddev calculation */
		for (d = 0; d < ndims; d++)
		{
			sum.min[d] += nd_box->min[d];
			sum.max[d] += nd_box->max[d];
		}

		/* Increment our "good feature" count */
		notnull_cnt++;

		/* Free up memory if our sample temporal point was copied */
		if (is_copy)
			pfree(temp);
		pfree(traj);

		/* Give backend a chance of interrupting us */
		vacuum_delay_point();
	}

	/* We can only compute real stats if we found some non-null values. */
	if (notnull_cnt > 0)
	{
		stats->stats_valid = true;
		/* Do the simple null-frac and width stats */
		stats->stanullfrac = (float4) null_cnt / (float4) sample_rows;
		stats->stawidth = (int) (total_width / notnull_cnt);

		/* Estimate that non-null values are unique */
		stats->stadistinct = (float4) (-1.0 * (1.0 - stats->stanullfrac));

		/* Compute statistics for spatial dimension */
		/* 2D Mode */
		gserialized_compute_stats(stats, sample_rows, total_rows, notnull_cnt,
			sample_boxes, &sum, &sample_extent, &slot_idx, 2);
		/* ND Mode */
		gserialized_compute_stats(stats, sample_rows, total_rows, notnull_cnt,
			sample_boxes, &sum, &sample_extent, &slot_idx, ndims);

		/* Compute statistics for time dimension */
		period_compute_stats1(stats, notnull_cnt, &slot_idx,
			time_lowers, time_uppers, time_lengths);
	}
	else if (null_cnt > 0)
	{
		/* We found only nulls; assume the column is entirely null */
		stats->stats_valid = true;
		stats->stanullfrac = 1.0;
		stats->stawidth = 0;		/* "unknown" */
		stats->stadistinct = 0.0;	/* "unknown" */
	}

	return;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(tnpoint_analyze);

PGDLLEXPORT Datum
tnpoint_analyze(PG_FUNCTION_ARGS)
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
	stats->compute_stats = tnpoint_compute_stats;
	PG_RETURN_BOOL(true);
}


/*****************************************************************************/
