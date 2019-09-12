/*****************************************************************************
 *
 * temporal_analyze.c
 *	  Functions for gathering statistics from temporal alphanumeric columns
 *
 * Various kind of statistics are collected for both the value and the time
 * dimension of temporal types. The kind of statistics depends on the duration
 * of the temporal type, which is defined in the table schema by the typmod
 * attribute. Please refer to the PostgreSQL file pg_statistic_d.h for more
 * information about the statistics collected.
 * 
 * For TemporalInst
 * - Slot 1
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_MCV.
 * 		- staop contains the OID of the "=" operator for the value dimension.
 * 		- stavalues stores the most common non-null values (MCV) for the value dimension.
 * 		- stanumbers stores the frequencies of the MCV for the value dimension.
 * 		- numnumbers contains the number of elements in the stanumbers array.
 * 		- numvalues contains the number of elements in the most common values array.
 * - Slot 2
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_HISTOGRAM.
 * 		- staop contains the OID of the "<" operator that describes the sort ordering.
 * 		- stavalues stores the histogram of scalar data for the value dimension
 * 		- numvalues contains the number of buckets in the histogram.
 * - Slot 3
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_CORRELATION.
 * 		- staop contains the OID of the "<" operator that describes the sort ordering.
 * 		- stavalues is NULL
 * 		- stanumbers contains the correlation coefficient between the sequence 
 * 		  of data values and the sequence of their actual tuple positions
 * 		- numvalues contains the number of buckets in the histogram.
 * - Slot 4
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_MCV.
 * 		- staop contains the "=" operator of the time dimension.
 * 		- stavalues stores the most common values (MCV) for the time dimension.
 * 		- stanumbers stores the frequencies of the MCV for the time dimension.
 * 		- numnumbers contains the number of elements in the stanumbers array.
 * 		- numvalues contains the number of elements in the most common values array.
 * - Slot 5
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_HISTOGRAM.
 * 		- staop contains the OID of the "<" operator that describes the sort ordering.
 * 		- stavalues stores the histogram for the time dimension.
 * For all other durations
 * - Slot 1
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_BOUNDS_HISTOGRAM.
 * 		- staop contains the "=" operator of the value dimension.
 * 		- stavalues stores the histogram of ranges for the value dimension.
 * 		- numvalues contains the number of buckets in the histogram.
 * - Slot 2
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_RANGE_LENGTH_HISTOGRAM.
 * 		- staop contains the "<" operator to the value dimension.
 * 		- stavalues stores the length of the histogram of ranges for the value dimension.
 * 		- numvalues contains the number of buckets in the histogram.
 * - Slot 3
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_PERIOD_BOUNDS_HISTOGRAM.
 * 		- staop contains the "=" operator of the time dimension.
 * 		- stavalues stores the histogram of periods for the time dimension.
 * 		- numvalues contains the number of buckets in the histogram.
 * - Slot 4
 * 		- stakind contains the type of statistics which is STATISTIC_KIND_PERIOD_LENGTH_HISTOGRAM.
 * 		- staop contains the "<" operator of the time dimension.
 * 		- stavalues stores the length of the histogram of periods for the time dimension.
 * 		- numvalues contains the number of buckets in the histogram.
 *
 * Notice that some statistics may not be collected, for example, since there
 * are no most common values. In that case, the next statistics collected is
 * stored in the next available slot.
 *
 * In the case of temporal types having a Period as bounding box, that is,
 * tbool and ttext, no statistics are collected for the value dimension and
 * the statistics for the temporal part are stored in slots 1 and 2.
 * 
 * Portions Copyright (c) 2019, Esteban Zimanyi, Mahmoud Sakr, Mohamed Bakli,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "temporal_analyze.h"

#include <assert.h>
#include <math.h>
#include <access/tuptoaster.h>
#include <catalog/pg_collation_d.h>
#include <catalog/pg_operator_d.h>
#include <commands/analyze.h>
#include <commands/vacuum.h>
#include <parser/parse_oper.h>
#include <utils/array_typanalyze.h>
#include <utils/datum.h>
#include <utils/fmgrprotos.h>
#include <utils/lsyscache.h>
#include <utils/timestamp.h>

#include "period.h"
#include "time_analyze.h"
#include "rangetypes_ext.h"
#include "temporaltypes.h"
#include "oidcache.h"
#include "temporal_util.h"
#include "temporal_analyze.h"

/*
 * To avoid consuming too much memory, IO and CPU load during analysis, and/or
 * too much space in the resulting pg_statistic rows, we ignore temporal values
 * that are wider than TEMPORAL_WIDTH_THRESHOLD (after detoasting!).  Note that 
 * this number is bigger than the similar WIDTH_THRESHOLD limit used in
 * analyze.c's standard typanalyze code, which is 1024.
 */
#define TEMPORAL_WIDTH_THRESHOLD 4096 // Should it be 0x10000 i.e. 64K as before ?

/*
 * While statistic functions are running, we keep a pointer to the extra data
 * here for use by assorted subroutines.  The functions doesn't
 * currently need to be re-entrant, so avoiding this is not worth the extra
 * notational cruft that would be needed.
 */
TemporalAnalyzeExtraData *temporal_extra_data;

/*****************************************************************************
 * Functions copied from files analyze.c and rangetypes_typanalyze.c since
 * they are not exported.
 *****************************************************************************/

/*
 * qsort_arg comparator for sorting ScalarItems
 *
 * Aside from sorting the items, we update the tupnoLink[] array
 * whenever two ScalarItems are found to contain equal datums.  The array
 * is indexed by tupno; for each ScalarItem, it contains the highest
 * tupno that that item's datum has been found to be equal to.  This allows
 * us to avoid additional comparisons in compute_scalar_stats().
 */
static int
compare_scalars(const void *a, const void *b, void *arg)
{
	Datum da = ((const ScalarItem *) a)->value;
	int ta = ((const ScalarItem *) a)->tupno;
	Datum db = ((const ScalarItem *) b)->value;
	int tb = ((const ScalarItem *) b)->tupno;
	CompareScalarsContext *cxt = (CompareScalarsContext *) arg;
	int compare;

	compare = ApplySortComparator(da, false, db, false, cxt->ssup);
	if (compare != 0)
		return compare;

	/*
	 * The two datums are equal, so update cxt->tupnoLink[].
	 */
	if (cxt->tupnoLink[ta] < tb)
		cxt->tupnoLink[ta] = tb;
	if (cxt->tupnoLink[tb] < ta)
		cxt->tupnoLink[tb] = ta;

	/*
	 * For equal datums, sort by tupno
	 */
	return ta - tb;
}

/*
 * qsort comparator for sorting ScalarMCVItems by position
 */
static int
compare_mcvs(const void *a, const void *b)
{
	int da = ((const ScalarMCVItem *) a)->first;
	int db = ((const ScalarMCVItem *) b)->first;

	return da - db;
}

/*
 * Comparison function for sorting RangeBounds.
 */
static int
range_bound_qsort_cmp(const void *a1, const void *a2)
{
	RangeBound *r1 = (RangeBound *) a1;
	RangeBound *r2 = (RangeBound *) a2;
	return period_cmp_bounds(DatumGetTimestampTz(r1->val),
							 DatumGetTimestampTz(r2->val),
							 r1->lower, r2->lower,
							 r1->inclusive, r2->inclusive);
}

/*
 * Comparison function for elements.
 *
 * We use the element type's default btree opclass, and the column collation
 * if the type is collation-sensitive.
 *
 * XXX consider using SortSupport infrastructure
 */
static int
element_compare(const void *key1, const void *key2)
{
	Datum	   d1 = *((const Datum *) key1);
	Datum	   d2 = *((const Datum *) key2);
	Datum	   c;

	c = FunctionCall2Coll(temporal_extra_data->time_cmp,
						  DEFAULT_COLLATION_OID,
						  d1, d2);
	return DatumGetInt32(c);
}

/*
 * Matching function for elements, to be used in hashtable lookups.
 */
static int
element_match(const void *key1, const void *key2, Size keysize)
{
	/* The keysize parameter is superfluous here */
	return element_compare(key1, key2);
}

static uint32
element_hash(const void *key, Size keysize, FmgrInfo *hash)
{
	Datum	   d = *((const Datum *) key);
	Datum	   h;

	h = FunctionCall1Coll(hash,
						  DEFAULT_COLLATION_OID,
						  d);
	return DatumGetUInt32(h);
}

static uint32
element_hash_value(const void *key, Size keysize)
{
	return element_hash(key, keysize, temporal_extra_data->value_hash);
}

static uint32
element_hash_time(const void *key, Size keysize)
{
	return element_hash(key, keysize, temporal_extra_data->time_hash);
}

/*
 * qsort() comparator for sorting TrackItems by frequencies (descending sort)
 */
static int
trackitem_compare_frequencies_desc(const void *e1, const void *e2)
{
	const TrackItem *const *t1 = (const TrackItem *const *) e1;
	const TrackItem *const *t2 = (const TrackItem *const *) e2;

	return (*t2)->frequency - (*t1)->frequency;
}

/*
 * qsort() comparator for sorting TrackItems by element values
 */
static int
trackitem_compare_element(const void *e1, const void *e2)
{
	const TrackItem *const *t1 = (const TrackItem *const *) e1;
	const TrackItem *const *t2 = (const TrackItem *const *) e2;

	return element_compare(&(*t1)->key, &(*t2)->key);
}

/*
 * qsort() comparator for sorting DECountItems by count
 */
static int
countitem_compare_count(const void *e1, const void *e2)
{
	const DECountItem *const *t1 = (const DECountItem *const *) e1;
	const DECountItem *const *t2 = (const DECountItem *const *) e2;

	if ((*t1)->count < (*t2)->count)
		return -1;
	else if ((*t1)->count == (*t2)->count)
		return 0;
	else
		return 1;
}

/*****************************************************************************
 * Compute statistics for scalar values, used both for the value and the 
 * time dimension of TemporalInst columns. Function derived from the inner 
 * part of function compute_scalar_stats of file analyze.c so that it can be 
 * called twice, for the value and for the time dimension.
 *****************************************************************************/

static int
compute_scalar_stats_mdb(VacAttrStats *stats, int values_cnt, bool is_varwidth, 
	int toowide_cnt, ScalarItem *values, int *tupnoLink, ScalarMCVItem *track, 
	Oid valuetypid, int nonnull_cnt, int null_cnt, 
	int slot_idx, int total_width, int totalrows, int samplerows, bool correlation)
{
	int			ndistinct,	/* # distinct values in sample */
				nmultiple,	/* # that appear multiple times */
				num_hist,
				dups_cnt,
				track_cnt = 0,
				num_mcv = stats->attr->attstattarget,
				num_bins = stats->attr->attstattarget,
				i;
	CompareScalarsContext cxt;
	SortSupportData ssup;
	double		corr_xysum;
	bool 		typbyval = get_typbyval_fast(valuetypid);
	int			typlen = get_typlen_fast(valuetypid);
	Oid 		ltopr, eqopr;

	memset(&ssup, 0, sizeof(ssup));
	ssup.ssup_cxt = CurrentMemoryContext;
	/* We always use the default collation for statistics */
	ssup.ssup_collation = DEFAULT_COLLATION_OID;
	ssup.ssup_nulls_first = false;

	/*
	 * For now, don't perform abbreviated key conversion, because full values
	 * are required for MCV slot generation.  Supporting that optimization
	 * would necessitate teaching compare_scalars() to call a tie-breaker.
	 */
	ssup.abbreviate = false;

	/* With respect to the original PostgreSQL function we need to look for 
	 * the specific operators for the value and type components */
	get_sort_group_operators(valuetypid,
							 false, false, false,
							 &ltopr, &eqopr, NULL,
							 NULL);
	PrepareSortSupportFromOrderingOp(ltopr, &ssup);

	/* Sort the collected values */
	cxt.ssup = &ssup;
	cxt.tupnoLink = tupnoLink;
	qsort_arg((void *) values, values_cnt, sizeof(ScalarItem),
			  compare_scalars, (void *) &cxt);

	/*
	 * Now scan the values in order, find the most common ones, and also
	 * accumulate ordering-correlation statistics.
	 *
	 * To determine which are most common, we first have to count the
	 * number of duplicates of each value.  The duplicates are adjacent in
	 * the sorted list, so a brute-force approach is to compare successive
	 * datum values until we find two that are not equal. However, that
	 * requires N-1 invocations of the datum comparison routine, which are
	 * completely redundant with work that was done during the sort.  (The
	 * sort algorithm must at some point have compared each pair of items
	 * that are adjacent in the sorted order; otherwise it could not know
	 * that it's ordered the pair correctly.) We exploit this by having
	 * compare_scalars remember the highest tupno index that each
	 * ScalarItem has been found equal to.  At the end of the sort, a
	 * ScalarItem's tupnoLink will still point to itself if and only if it
	 * is the last item of its group of duplicates (since the group will
	 * be ordered by tupno).
	 */
	corr_xysum = 0;
	ndistinct = 0;
	nmultiple = 0;
	dups_cnt = 0;
	for (i = 0; i < values_cnt; i++)
	{
		int			tupno = values[i].tupno;

		corr_xysum += ((double) i) * ((double) tupno);
		dups_cnt++;
		if (tupnoLink[tupno] == tupno)
		{
			/* Reached end of duplicates of this value */
			ndistinct++;
			if (dups_cnt > 1)
			{
				nmultiple++;
				if (track_cnt < num_mcv ||
					dups_cnt > track[track_cnt - 1].count)
				{
					/*
					 * Found a new item for the mcv list; find its
					 * position, bubbling down old items if needed. Loop
					 * invariant is that j points at an empty/ replaceable
					 * slot.
					 */
					int			j;

					if (track_cnt < num_mcv)
						track_cnt++;
					for (j = track_cnt - 1; j > 0; j--)
					{
						if (dups_cnt <= track[j - 1].count)
							break;
						track[j].count = track[j - 1].count;
						track[j].first = track[j - 1].first;
					}
					track[j].count = dups_cnt;
					track[j].first = i + 1 - dups_cnt;
				}
			}
			dups_cnt = 0;
		}
	}

	stats->stats_valid = true;
	/* Do the simple null-frac and width stats */
	stats->stanullfrac = (double) null_cnt / (double) samplerows;
	if (is_varwidth)
		stats->stawidth = total_width / (double) nonnull_cnt;
	else
		stats->stawidth = stats->attrtype->typlen;

	if (nmultiple == 0)
	{
		/*
		 * If we found no repeated non-null values, assume it's a unique
		 * column; but be sure to discount for any nulls we found.
		 */
		stats->stadistinct = -1.0 * (1.0 - stats->stanullfrac);
	}
	else if (toowide_cnt == 0 && nmultiple == ndistinct)
	{
		/*
		 * Every value in the sample appeared more than once.  Assume the
		 * column has just these values.  (This case is meant to address
		 * columns with small, fixed sets of possible values, such as
		 * boolean or enum columns.  If there are any values that appear
		 * just once in the sample, including too-wide values, we should
		 * assume that that's not what we're dealing with.)
		 */
		stats->stadistinct = ndistinct;
	}
	else
	{
		/*----------
		 * Estimate the number of distinct values using the estimator
		 * proposed by Haas and Stokes in IBM Research Report RJ 10025:
		 *		n*d / (n - f1 + f1*n/N)
		 * where f1 is the number of distinct values that occurred
		 * exactly once in our sample of n rows (from a total of N),
		 * and d is the total number of distinct values in the sample.
		 * This is their Duj1 estimator; the other estimators they
		 * recommend are considerably more complex, and are numerically
		 * very unstable when n is much smaller than N.
		 *
		 * In this calculation, we consider only non-nulls.  We used to
		 * include rows with null values in the n and N counts, but that
		 * leads to inaccurate answers in columns with many nulls, and
		 * it's intuitively bogus anyway considering the desired result is
		 * the number of distinct non-null values.
		 *
		 * Overwidth values are assumed to have been distinct.
		 *----------
		 */
		int			f1 = ndistinct - nmultiple + toowide_cnt;
		int			d = f1 + nmultiple;
		double		n = samplerows - null_cnt;
		double		N = totalrows * (1.0 - stats->stanullfrac);
		double		stadistinct;

		/* N == 0 shouldn't happen, but just in case ... */
		if (N > 0)
			stadistinct = (n * d) / ((n - f1) + f1 * n / N);
		else
			stadistinct = 0;

		/* Clamp to sane range in case of roundoff error */
		if (stadistinct < d)
			stadistinct = d;
		if (stadistinct > N)
			stadistinct = N;
		/* And round to integer */
		stats->stadistinct = floor(stadistinct + 0.5);
	}

	/*
	 * If we estimated the number of distinct values at more than 10% of
	 * the total row count (a very arbitrary limit), then assume that
	 * stadistinct should scale with the row count rather than be a fixed
	 * value.
	 */
	if (stats->stadistinct > 0.1 * totalrows)
		stats->stadistinct = -(stats->stadistinct / totalrows);

	/*
	 * Decide how many values are worth storing as most-common values. If
	 * we are able to generate a complete MCV list (all the values in the
	 * sample will fit, and we think these are all the ones in the table),
	 * then do so.  Otherwise, store only those values that are
	 * significantly more common than the values not in the list.
	 *
	 * Note: the first of these cases is meant to address columns with
	 * small, fixed sets of possible values, such as boolean or enum
	 * columns.  If we can *completely* represent the column population by
	 * an MCV list that will fit into the stats target, then we should do
	 * so and thus provide the planner with complete information.  But if
	 * the MCV list is not complete, it's generally worth being more
	 * selective, and not just filling it all the way up to the stats
	 * target.
	 */
	if (track_cnt == ndistinct && toowide_cnt == 0 &&
		stats->stadistinct > 0 &&
		track_cnt <= num_mcv)
	{
		/* Track list includes all values seen, and all will fit */
		num_mcv = track_cnt;
	}
	else
	{
		int		   *mcv_counts;

		/* Incomplete list; decide how many values are worth keeping */
		if (num_mcv > track_cnt)
			num_mcv = track_cnt;

		if (num_mcv > 0)
		{
			mcv_counts = (int *) palloc(num_mcv * sizeof(int));
			for (i = 0; i < num_mcv; i++)
				mcv_counts[i] = track[i].count;

			num_mcv = analyze_mcv_list(mcv_counts, num_mcv,
									   stats->stadistinct,
									   stats->stanullfrac,
									   samplerows, totalrows);
		}
	}

	/* Generate MCV slot entry */
	if (num_mcv > 0)
	{
		MemoryContext old_context;
		Datum	   *mcv_values;
		float4	   *mcv_freqs;

		/* Must copy the target values into anl_context */
		old_context = MemoryContextSwitchTo(stats->anl_context);
		mcv_values = (Datum *) palloc(num_mcv * sizeof(Datum));
		mcv_freqs = (float4 *) palloc(num_mcv * sizeof(float4));
		for (i = 0; i < num_mcv; i++)
		{
			mcv_values[i] = datumCopy(values[track[i].first].value, 
				typbyval,typlen);
			mcv_freqs[i] = (double) track[i].count / (double) samplerows;
		}
		MemoryContextSwitchTo(old_context);

		stats->stakind[slot_idx] = STATISTIC_KIND_MCV;
		stats->staop[slot_idx] = eqopr;
		stats->stanumbers[slot_idx] = mcv_freqs;
		stats->numnumbers[slot_idx] = num_mcv;
		stats->stavalues[slot_idx] = mcv_values;
		stats->numvalues[slot_idx] = num_mcv;
		stats->statyplen[slot_idx] = (int16) typlen;
		stats->statypid[slot_idx] = valuetypid;
		stats->statypbyval[slot_idx] = typbyval;

		/*
		 * Accept the defaults for stats->statypid and others. They have
		 * been set before we were called (see vacuum.h)
		 */
		slot_idx++;
	}

	/*
	 * Generate a histogram slot entry if there are at least two distinct
	 * values not accounted for in the MCV list.  (This ensures the
	 * histogram won't collapse to empty or a singleton.)
	 */
	num_hist = ndistinct - num_mcv;
	if (num_hist > num_bins)
		num_hist = num_bins + 1;
	if (num_hist >= 2)
	{
		MemoryContext old_context;
		Datum	   *hist_values;
		int			nvals;
		int			pos,
					posfrac,
					delta,
					deltafrac;

		/* Sort the MCV items into position order to speed next loop */
		qsort((void *) track, num_mcv,
			  sizeof(ScalarMCVItem), compare_mcvs);

		/*
		 * Collapse out the MCV items from the values[] array.
		 *
		 * Note we destroy the values[] array here... but we don't need it
		 * for anything more.  We do, however, still need values_cnt.
		 * nvals will be the number of remaining entries in values[].
		 */
		if (num_mcv > 0)
		{
			int			src,
						dest;
			int			j;

			src = dest = 0;
			j = 0;			/* index of next interesting MCV item */
			while (src < values_cnt)
			{
				int			ncopy;

				if (j < num_mcv)
				{
					int			first = track[j].first;

					if (src >= first)
					{
						/* advance past this MCV item */
						src = first + track[j].count;
						j++;
						continue;
					}
					ncopy = first - src;
				}
				else
					ncopy = values_cnt - src;
				memmove(&values[dest], &values[src],
						ncopy * sizeof(ScalarItem));
				src += ncopy;
				dest += ncopy;
			}
			nvals = dest;
		}
		else
			nvals = values_cnt;
		Assert(nvals >= num_hist);

		/* Must copy the target values into anl_context */
		old_context = MemoryContextSwitchTo(stats->anl_context);
		hist_values = (Datum *) palloc(num_hist * sizeof(Datum));

		/*
		 * The object of this loop is to copy the first and last values[]
		 * entries along with evenly-spaced values in between.  So the
		 * i'th value is values[(i * (nvals - 1)) / (num_hist - 1)].  But
		 * computing that subscript directly risks integer overflow when
		 * the stats target is more than a couple thousand.  Instead we
		 * add (nvals - 1) / (num_hist - 1) to pos at each step, tracking
		 * the integral and fractional parts of the sum separately.
		 */
		delta = (nvals - 1) / (num_hist - 1);
		deltafrac = (nvals - 1) % (num_hist - 1);
		pos = posfrac = 0;

		for (i = 0; i < num_hist; i++)
		{
			hist_values[i] = datumCopy(values[pos].value, typbyval, typlen);
			pos += delta;
			posfrac += deltafrac;
			if (posfrac >= (num_hist - 1))
			{
				/* fractional part exceeds 1, carry to integer part */
				pos++;
				posfrac -= (num_hist - 1);
			}
		}

		MemoryContextSwitchTo(old_context);

		stats->stakind[slot_idx] = STATISTIC_KIND_HISTOGRAM;
		stats->staop[slot_idx] = ltopr;
		stats->stavalues[slot_idx] = hist_values;
		stats->numvalues[slot_idx] = num_hist;
		stats->statyplen[slot_idx] = (int16) typlen;
		stats->statypid[slot_idx] = valuetypid;
		stats->statypbyval[slot_idx] = typbyval;

		/*
		 * Accept the defaults for stats->statypid and others. They have
		 * been set before we were called (see vacuum.h)
		 */
		slot_idx++;
	}

	/* Generate a correlation entry only for the value component and if there
	 * are multiple values */
	if (correlation && values_cnt > 1)
	{
		MemoryContext old_context;
		float4	   *corrs;
		double		corr_xsum,
					corr_x2sum;

		/* Must copy the target values into anl_context */
		old_context = MemoryContextSwitchTo(stats->anl_context);
		corrs = (float4 *) palloc(sizeof(float4));
		MemoryContextSwitchTo(old_context);

		/*----------
		 * Since we know the x and y value sets are both
		 *		0, 1, ..., values_cnt-1
		 * we have sum(x) = sum(y) =
		 *		(values_cnt-1)*values_cnt / 2
		 * and sum(x^2) = sum(y^2) =
		 *		(values_cnt-1)*values_cnt*(2*values_cnt-1) / 6.
		 *----------
		 */
		corr_xsum = ((double) (values_cnt - 1)) *
			((double) values_cnt) / 2.0;
		corr_x2sum = ((double) (values_cnt - 1)) *
			((double) values_cnt) * (double) (2 * values_cnt - 1) / 6.0;

		/* And the correlation coefficient reduces to */
		corrs[0] = (values_cnt * corr_xysum - corr_xsum * corr_xsum) /
			(values_cnt * corr_x2sum - corr_xsum * corr_xsum);

		stats->stakind[slot_idx] = STATISTIC_KIND_CORRELATION;
		stats->staop[slot_idx] = ltopr;
		stats->stanumbers[slot_idx] = corrs;
		stats->numnumbers[slot_idx] = 1;
		slot_idx++;
	}
	/* Returns the next available slot */
	return slot_idx;
}

/*****************************************************************************
 * Generic statistics functions for alphanumeric temporal types.
 * In these functions the last argument valuestats determines whether
 * statistics are computed for the value dimension, that is, it is true for
 * temporal numbers. Otherwise, statistics are computed only for the temporal
 * dimension, that is, in the the case of temporal boolean and temporal text.
 *****************************************************************************/

/* 
 * Compute statistics for TemporalInst columns.
 * Function derived from compute_scalar_stats of file analyze.c 
 */
static void
tempinst_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
					   int samplerows, double totalrows, bool valuestats)
{
	int			i;
	int			null_cnt = 0;
	int			nonnull_cnt = 0;
	int			toowide_cnt = 0;
	double		total_width = 0;
	bool		is_varlena = (!stats->attrtype->typbyval &&
							  stats->attrtype->typlen == -1);
	bool		is_varwidth = (!stats->attrtype->typbyval &&
							   stats->attrtype->typlen < 0);
	ScalarItem *scalar_values, 
			   *timestamp_values;
	int			values_cnt = 0;
	int 	   *scalar_tupnoLink,
			   *timestamp_tupnoLink;
	ScalarMCVItem *scalar_track,
			   *timestamp_track;
	int			num_mcv = stats->attr->attstattarget;
	Oid valuetypid;

	if (valuestats)
	{
		scalar_values = (ScalarItem *) palloc(samplerows * sizeof(ScalarItem));
		scalar_tupnoLink = (int *) palloc(samplerows * sizeof(int));
		scalar_track = (ScalarMCVItem *) palloc(num_mcv * sizeof(ScalarMCVItem));
		valuetypid = base_oid_from_temporal(stats->attrtypid);
	}

	timestamp_values = (ScalarItem *) palloc(samplerows * sizeof(ScalarItem));
	timestamp_tupnoLink = (int *) palloc(samplerows * sizeof(int));
	timestamp_track = (ScalarMCVItem *) palloc(num_mcv * sizeof(ScalarMCVItem));

	/* Initial scan to find sortable values */
	for (i = 0; i < samplerows; i++)
	{
		Datum		value;
		bool		isnull;
		TemporalInst *inst;

		vacuum_delay_point();

		value = fetchfunc(stats, i, &isnull);

		/* Check for null/nonnull */
		if (isnull)
		{
			null_cnt++;
			continue;
		}
		nonnull_cnt++;

		/*
		 * If it's a variable-width field, add up widths for average width
		 * calculation.  Note that if the value is toasted, we use the toasted
		 * width.  We don't bother with this calculation if it's a fixed-width
		 * type.
		 */
		if (is_varlena)
		{
			total_width += VARSIZE_ANY(DatumGetPointer(value));

			/*
			 * If the value is toasted, we want to detoast it just once to
			 * avoid repeated detoastings and resultant excess memory usage
			 * during the comparisons.  Also, check to see if the value is
			 * excessively wide, and if so don't detoast at all --- just
			 * ignore the value.
			 */
			if (toast_raw_datum_size(value) > TEMPORAL_WIDTH_THRESHOLD)
			{
				toowide_cnt++;
				continue;
			}
			value = PointerGetDatum(PG_DETOAST_DATUM(value));
		}
		else if (is_varwidth)
		{
			/* must be cstring */
			total_width += strlen(DatumGetCString(value)) + 1;
		}

		/* Get the temporal instant value and add its value and its timestamp
		 * dimensions to the lists to be sorted */
		inst = DatumGetTemporalInst(value);
		if (valuestats)
		{
			scalar_values[values_cnt].value = temporalinst_value(inst);
			scalar_values[values_cnt].tupno = values_cnt;
			scalar_tupnoLink[values_cnt] = values_cnt;
		}
		timestamp_values[values_cnt].value = TimestampTzGetDatum(inst->t);
		timestamp_values[values_cnt].tupno = values_cnt;
		timestamp_tupnoLink[values_cnt] = values_cnt;

		values_cnt++;
	}

	/* We can only compute real stats if we found some non-null values. */
	if (nonnull_cnt > 0)
	{
		int			slot_idx = 0;

		if (valuestats)
		{
			/* Compute the statistics for the value dimension */
			slot_idx = compute_scalar_stats_mdb(stats, values_cnt, is_varwidth, 
				toowide_cnt, scalar_values, scalar_tupnoLink, scalar_track, 
				valuetypid, nonnull_cnt, null_cnt, slot_idx, 
				total_width, totalrows, samplerows, true);
		}

		/* Compute the statistics for the time dimension 
		 * We don't need to get the next available slot */
		compute_scalar_stats_mdb(stats, values_cnt, is_varwidth,
			toowide_cnt, timestamp_values, timestamp_tupnoLink, timestamp_track, 
			TIMESTAMPTZOID, nonnull_cnt, null_cnt, slot_idx, 
			total_width, totalrows, samplerows, false);
	}
	else if (nonnull_cnt > 0)
	{
		/* We found some non-null values, but they were all too wide */
		Assert(nonnull_cnt == toowide_cnt);
		stats->stats_valid = true;
		/* Do the simple null-frac and width stats */
		stats->stanullfrac = (double) null_cnt / (double) samplerows;
		if (is_varwidth)
			stats->stawidth = total_width / (double) nonnull_cnt;
		else
			stats->stawidth = stats->attrtype->typlen;
		/* Assume all too-wide values are distinct, so it's a unique column */
		stats->stadistinct = -1.0 * (1.0 - stats->stanullfrac);
	}
	else if (null_cnt > 0)
	{
		/* We found only nulls; assume the column is entirely null */
		stats->stats_valid = true;
		stats->stanullfrac = 1.0;
		if (is_varwidth)
			stats->stawidth = 0;	/* "unknown" */
		else
			stats->stawidth = stats->attrtype->typlen;
		stats->stadistinct = 0.0;	/* "unknown" */
	}

	/* We don't need to bother cleaning up any of our temporary palloc's */
}

/* 
 * Compute statistics for TemporalI columns.
 * The function is called twice for the values and for the time elements.
 * Function derived from compute_array_stats of file array_typanalyze.c 
 */
static int
tempi_elems_compute_stats(VacAttrStats *stats, HTAB *elements_tab, 
	HTAB *count_tab, int64 element_no, int analyzed_rows, int num_mcelem, 
	int bucket_width, int slot_idx, bool valuestats)
{
	int			nonnull_cnt = analyzed_rows;
	int			count_items_count;
	TrackItem  *item;
	TrackItem **sort_table;
	int			track_len;
	int64		cutoff_freq;
	int64		minfreq,
				maxfreq;
	int		 	i;
	HASH_SEQ_STATUS scan_status;

	/* 
	* We assume the standard stats code already took care of setting
	* stats_valid, stanullfrac, stawidth, stadistinct.  We'd have to
	* re-compute those values if we wanted to not store the standard
	* stats.
	*/

	/*
	* Construct an array of the interesting hashtable items, that is,
	* those meeting the cutoff frequency (s - epsilon)*N.  Also identify
	* the minimum and maximum frequencies among these items.
	*
	* Since epsilon = s/10 and bucket_width = 1/epsilon, the cutoff
	* frequency is 9*N / bucket_width.
	*/
	cutoff_freq = 9 * element_no / bucket_width;

	i = (int) hash_get_num_entries(elements_tab); /* surely enough space */
	sort_table = (TrackItem **) palloc(sizeof(TrackItem *) * i);

	hash_seq_init(&scan_status, elements_tab);
	track_len = 0;
	minfreq = element_no;
	maxfreq = 0;
	while ((item = (TrackItem *) hash_seq_search(&scan_status)) != NULL)
	{
		if (item->frequency > cutoff_freq)
		{
			sort_table[track_len++] = item;
			minfreq = Min(minfreq, item->frequency);
			maxfreq = Max(maxfreq, item->frequency);
		}
	}

	/*
	* If we obtained more elements than we really want, get rid of those
	* with least frequencies.  The easiest way is to qsort the array into
	* descending frequency order and truncate the array.
	*/
	if (num_mcelem < track_len)
	{
		qsort(sort_table, track_len, sizeof(TrackItem *),
			trackitem_compare_frequencies_desc);
		/* reset minfreq to the smallest frequency we're keeping */
		minfreq = sort_table[num_mcelem - 1]->frequency;
	}
	else
		num_mcelem = track_len;

	/* Generate MCELEM slot entry */
	if (num_mcelem > 0)
	{
		MemoryContext old_context;
		Datum	   *mcelem_values;
		float4	   *mcelem_freqs;

		/*
		* We want to store statistics sorted on the element value using
		* the element type's default comparison function.  This permits
		* fast binary searches in selectivity estimation functions.
		*/
		qsort(sort_table, num_mcelem, sizeof(TrackItem *),
			trackitem_compare_element);

		/* Must copy the target values into anl_context */
		old_context = MemoryContextSwitchTo(stats->anl_context);

		/*
		* We sorted statistics on the element value, but we want to be
		* able to find the minimal and maximal frequencies without going
		* through all the values.  We don't want the frequency of null
		* elements since there are non null elements. Store these two
		* values at the end of mcelem_freqs.
		*/
		mcelem_values = (Datum *) palloc(num_mcelem * sizeof(Datum));
		mcelem_freqs = (float4 *) palloc((num_mcelem + 2) * sizeof(float4));

		/*
		* See comments above about use of nonnull_cnt as the divisor for
		* the final frequency estimates.
		*/
		for (i = 0; i < num_mcelem; i++)
		{
			TrackItem  *item = sort_table[i];

			mcelem_values[i] = item->key;
			mcelem_freqs[i] = (float4) item->frequency /
							(float4) nonnull_cnt;
		}
		mcelem_freqs[i++] = (float4) minfreq / (float4) nonnull_cnt;
		mcelem_freqs[i++] = (float4) maxfreq / (float4) nonnull_cnt;

		MemoryContextSwitchTo(old_context);

		stats->stakind[slot_idx] = STATISTIC_KIND_MCELEM;
		stats->staop[slot_idx] = (valuestats) ? temporal_extra_data->value_eq_opr :
                                 temporal_extra_data->time_eq_opr;
		stats->stanumbers[slot_idx] = mcelem_freqs;
		/* See above comment about extra stanumber entries */
		stats->numnumbers[slot_idx] = num_mcelem + 2;
		stats->stavalues[slot_idx] = mcelem_values;
		stats->numvalues[slot_idx] = num_mcelem;
		/* We are storing values of element type */
		stats->statypid[slot_idx] = (valuestats) ? temporal_extra_data->value_type_id :
                                    temporal_extra_data->time_type_id;
		stats->statyplen[slot_idx] = (valuestats) ? temporal_extra_data->value_typlen :
                                     temporal_extra_data->time_typlen;
		stats->statypbyval[slot_idx] = (valuestats) ? temporal_extra_data->value_typbyval :
                                       temporal_extra_data->time_typbyval;
		stats->statypalign[slot_idx] = (valuestats) ? temporal_extra_data->value_typalign :
                                       temporal_extra_data->time_typalign;
		slot_idx++;
	}

	/* Generate DECHIST slot entry */
	count_items_count = (int) hash_get_num_entries(count_tab);
	if (count_items_count > 0)
	{
		int			num_hist = stats->attr->attstattarget;
		DECountItem **sorted_count_items;
		DECountItem *count_item;
		int			j;
		int			delta;
		int64		frac;
		float4	   *hist;

		/* num_hist must be at least 2 for the loop below to work */
		num_hist = Max(num_hist, 2);

		/*
		* Create an array of DECountItem pointers, and sort them into
		* increasing count order.
		*/
		sorted_count_items = (DECountItem **)
				palloc(sizeof(DECountItem *) * count_items_count);
		hash_seq_init(&scan_status, count_tab);
		j = 0;
		while ((count_item = (DECountItem *) hash_seq_search(&scan_status)) != NULL)
		{
			sorted_count_items[j++] = count_item;
		}
		qsort(sorted_count_items, count_items_count,
			sizeof(DECountItem *), countitem_compare_count);

		/*
		* Prepare to fill stanumbers with the histogram, followed by the
		* average count.  This array must be stored in anl_context.
		*/
		hist = (float4 *) MemoryContextAlloc(stats->anl_context,
								sizeof(float4) * (num_hist + 1));
		hist[num_hist] = (float4) element_no / (float4) nonnull_cnt;

		/*----------
		* Construct the histogram of distinct-element counts (DECs).
		*
		* The object of this loop is to copy the min and max DECs to
		* hist[0] and hist[num_hist - 1], along with evenly-spaced DECs
		* in between (where "evenly-spaced" is with reference to the
		* whole input population of arrays).  If we had a complete sorted
		* array of DECs, one per analyzed row, the i'th hist value would
		* come from DECs[i * (analyzed_rows - 1) / (num_hist - 1)]
		* (compare the histogram-making loop in compute_scalar_stats()).
		* But instead of that we have the sorted_count_items[] array,
		* which holds unique DEC values with their frequencies (that is,
		* a run-length-compressed version of the full array).  So we
		* control advancing through sorted_count_items[] with the
		* variable "frac", which is defined as (x - y) * (num_hist - 1),
		* where x is the index in the notional DECs array corresponding
		* to the start of the next sorted_count_items[] element's run,
		* and y is the index in DECs from which we should take the next
		* histogram value.  We have to advance whenever x <= y, that is
		* frac <= 0.  The x component is the sum of the frequencies seen
		* so far (up through the current sorted_count_items[] element),
		* and of course y * (num_hist - 1) = i * (analyzed_rows - 1),
		* per the subscript calculation above.  (The subscript calculation
		* implies dropping any fractional part of y; in this formulation
		* that's handled by not advancing until frac reaches 1.)
		*
		* Even though frac has a bounded range, it could overflow int32
		* when working with very large statistics targets, so we do that
		* math in int64.
		*----------
		*/
		delta = analyzed_rows - 1;
		j = 0;				/* current index in sorted_count_items */
		/* Initialize frac for sorted_count_items[0]; y is initially 0 */
		frac = (int64) sorted_count_items[0]->frequency * (num_hist - 1);
		for (i = 0; i < num_hist; i++)
		{
			while (frac <= 0)
			{
				/* Advance, and update x component of frac */
				j++;
				frac += (int64) sorted_count_items[j]->frequency * (num_hist - 1);
			}
			hist[i] = sorted_count_items[j]->count;
			frac -= delta;	/* update y for upcoming i increment */
		}
		Assert(j == count_items_count - 1);

		stats->stakind[slot_idx] = STATISTIC_KIND_DECHIST;
		stats->staop[slot_idx] = (valuestats) ? temporal_extra_data->value_eq_opr :
                                 temporal_extra_data->time_eq_opr;
		stats->stanumbers[slot_idx] = hist;
		stats->numnumbers[slot_idx] = num_hist + 1;
		slot_idx++;
	}
	return slot_idx;
}

/* 
 * Compute statistics for TemporalI columns.
 * Function derived from compute_array_stats of file array_typanalyze.c 
 */
static void
tempi_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
					int samplerows, double totalrows, bool valuestats)
{
	int		num_mcelem;
	int		null_cnt = 0;
	int		analyzed_rows = 0;
	double	total_width = 0;

	/* This is D from the LC algorithm. */
	HTAB	   *elements_tab_value, *elements_tab_time;
	HASHCTL		elem_hash_ctl_value, elem_hash_ctl_time;

	/* This is the current bucket number from the LC algorithm */
	int			b_current_value, b_current_time;

	/* This is 'w' from the LC algorithm */
	int			bucket_width;
	int64		element_no_value, element_no_time;
	TrackItem  *item_value, *item_time;
	int			slot_idx;
	HTAB	   *count_tab_value, *count_tab_time;
	HASHCTL		count_hash_ctl_value, count_hash_ctl_time;
	DECountItem *count_item_value, *count_item_time;

	temporal_extra_data = (TemporalAnalyzeExtraData *) stats->extra_data;

	/*
	 * We want statistics_target * 10 elements in the MCELEM array. This
	 * multiplier is pretty arbitrary, but is meant to reflect the fact that
	 * the number of individual elements tracked in pg_statistic ought to be
	 * more than the number of values for a simple scalar column.
	 */
	num_mcelem = stats->attr->attstattarget * 10;

	/*
	 * We set bucket width equal to num_mcelem / 0.007 as per the comment
	 * above.
	 */
	bucket_width = num_mcelem * 1000 / 7;

	/*
	 * Create the hashtable. It will be in local memory, so we don't need to
	 * worry about overflowing the initial size. Also we don't need to pay any
	 * attention to locking and memory management.
	 */

	if (valuestats)
	{
		MemSet(&elem_hash_ctl_value, 0, sizeof(elem_hash_ctl_value));
		elem_hash_ctl_value.keysize = sizeof(Datum);
		elem_hash_ctl_value.entrysize = sizeof(TrackItem);
		elem_hash_ctl_value.hash = element_hash_value;
		elem_hash_ctl_value.match = element_match;
		elem_hash_ctl_value.hcxt = CurrentMemoryContext;
		elements_tab_value = hash_create("Analyzed value elements table",
										num_mcelem,
										&elem_hash_ctl_value,
										HASH_ELEM | HASH_FUNCTION | HASH_COMPARE | HASH_CONTEXT);

		/* hashtable for array distinct elements counts */
		MemSet(&count_hash_ctl_value, 0, sizeof(count_hash_ctl_value));
		count_hash_ctl_value.keysize = sizeof(int);
		count_hash_ctl_value.entrysize = sizeof(DECountItem);
		count_hash_ctl_value.hcxt = CurrentMemoryContext;
		count_tab_value = hash_create("Distinct value element count table",
									64,
									&count_hash_ctl_value,
									HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);
	}

	MemSet(&elem_hash_ctl_time, 0, sizeof(elem_hash_ctl_time));
	elem_hash_ctl_time.keysize = sizeof(Datum);
	elem_hash_ctl_time.entrysize = sizeof(TrackItem);
	elem_hash_ctl_time.hash = element_hash_time;
	elem_hash_ctl_time.match = element_match;
	elem_hash_ctl_time.hcxt = CurrentMemoryContext;
	elements_tab_time = hash_create("Analyzed time elements table",
										num_mcelem,
										&elem_hash_ctl_time,
										HASH_ELEM | HASH_FUNCTION | HASH_COMPARE | HASH_CONTEXT);

	/* hashtable for array distinct elements counts */
	MemSet(&count_hash_ctl_time, 0, sizeof(count_hash_ctl_time));
	count_hash_ctl_time.keysize = sizeof(int);
	count_hash_ctl_time.entrysize = sizeof(DECountItem);
	count_hash_ctl_time.hcxt = CurrentMemoryContext;
	count_tab_time = hash_create("Distinct time element count table",
									 64,
									 &count_hash_ctl_time,
									 HASH_ELEM | HASH_BLOBS | HASH_CONTEXT);

	/* Initialize counters. */
	b_current_value = 1;
	b_current_time = 1;
	element_no_value = 0;
	element_no_time = 0;

	/* Loop over the TemporalI values. */
	for (int i = 0; i < samplerows; i++)
	{
		Datum		value;
		bool		isnull;
		TemporalI  *ti;
		int			j;
		int64		prev_element_no_value = element_no_value,
					prev_element_no_time = element_no_time;
		int			distinct_count_value, distinct_count_time;
		bool		count_item_found_value, count_item_found_time;

		vacuum_delay_point();

		value = fetchfunc(stats, i, &isnull);
		if (isnull)
		{
			/* TemporalI value is null, just count that */
			null_cnt++;
			continue;
		}

		/* Skip too-large values. */
		if (toast_raw_datum_size(value) > TEMPORAL_WIDTH_THRESHOLD)
			continue;
		else
			analyzed_rows++;

		total_width += VARSIZE_ANY(DatumGetPointer(value));

		/*
		 * Now detoast the TemporalI value if needed, and deconstruct into datum.
		 */
		ti = DatumGetTemporalI(value);
		/*
		 * We loop through the elements in the TemporalI and add them to our
		 * tracking hashtables.
		 */

		for (j = 0; j < ti->count; j++)
		{
			TemporalInst *inst = temporali_inst_n(ti, j);
			Datum elem_value, elem_time;
			bool found_value, found_time;

			if (valuestats)
			{
				/* Lookup current element value in hashtable, adding it if new */
				elem_value = temporalinst_value(inst);
				item_value = (TrackItem *) hash_search(elements_tab_value,
													(const void *) &elem_value,
													HASH_ENTER, &found_value);

				if (found_value)
				{
					/* The element value is already on the tracking list */

					/*
					* The operators we assist ignore duplicate array elements, so
					* count a given distinct element only once per array.
					*/
					if (item_value->last_container == i)
						continue;

					item_value->frequency++;
					item_value->last_container = i;
				}
				else
				{
					/* Initialize new tracking list element */

					/*
					* If element type is pass-by-reference, we must copy it into
					* palloc'd space, so that we can release the array below. (We
					* do this so that the space needed for element values is
					* limited by the size of the hashtable; if we kept all the
					* array values around, it could be much more.)
					*/
					item_value->key = elem_value;
					item_value->frequency = 1;
					item_value->delta = b_current_value - 1;
					item_value->last_container = i;
				}

				/* element_no is the number of elements processed (ie N) */
				element_no_value++;
			}

			/* Lookup current time element in hashtable, adding it if new */
			elem_time = TimestampTzGetDatum(inst->t);
			item_time = (TrackItem *) hash_search(elements_tab_time,
												  (const void *) &elem_time,
												  HASH_ENTER, &found_time);

			if (found_time)
			{
				/* The element value is already on the tracking list */

				/*
				 * The operators we assist ignore duplicate array elements, so
				 * count a given distinct element only once per array.
				 */
				if (item_time->last_container == i)
					continue;

				item_time->frequency++;
				item_time->last_container = i;
			}
			else
			{
				/* Initialize new tracking list element */

				/*
				 * If element type is pass-by-reference, we must copy it into
				 * palloc'd space, so that we can release the array below. (We
				 * do this so that the space needed for element values is
				 * limited by the size of the hashtable; if we kept all the
				 * array values around, it could be much more.)
				 */
				item_time->key = elem_time;
				item_time->frequency = 1;
				item_time->delta = b_current_time - 1;
				item_time->last_container = i;
			}

			/* element_no is the number of elements processed (ie N) */
			element_no_time++;
		}

		/* Update frequency of the particular array distinct element count. */
		if (valuestats)
		{
			distinct_count_value = (int) (element_no_value - prev_element_no_value);
			count_item_value = (DECountItem *) hash_search(count_tab_value, &distinct_count_value,
														HASH_ENTER,
														&count_item_found_value);
			if (count_item_found_value)
				count_item_value->frequency++;
			else
				count_item_value->frequency = 1;
		}

		distinct_count_time = (int) (element_no_time - prev_element_no_time);
		count_item_time = (DECountItem *) hash_search(count_tab_time, &distinct_count_time,
													  HASH_ENTER,
													  &count_item_found_time);
		if (count_item_found_time)
			count_item_time->frequency++;
		else
			count_item_time->frequency = 1;
	}

	/* We can only compute real stats if we found some non-null values. */
	if (analyzed_rows > 0)
	{
		stats->stats_valid = true;

		/* Do the simple null-frac and width stats */
		stats->stanullfrac = (double) null_cnt / (double) samplerows;
		stats->stawidth = total_width / (double) analyzed_rows;

		/* Estimate that non-null values are unique */
		stats->stadistinct = -1.0 * (1.0 - stats->stanullfrac);

		slot_idx = 0;
		if (valuestats)
		{
			/* Statistics for the value dimension 
			 * The following function uses at most 2 slots */
			slot_idx = tempi_elems_compute_stats(stats, elements_tab_value, 
				count_tab_value, element_no_value, analyzed_rows, 
				num_mcelem, bucket_width, slot_idx, true);
		}
		/*  Statistics for the temporal dimension
		 *  The statistics are stored in the next slot */
		tempi_elems_compute_stats(stats, elements_tab_time, 
			count_tab_time, element_no_time, analyzed_rows, 
			num_mcelem, bucket_width, slot_idx, false);
	}

	/*
	 * We don't need to bother cleaning up any of our temporary palloc's. The
	 * hashtable should also go away, as it used a child memory context.
	 */
}

/* 
 * Compute statistics for all durations distinct from TemporalInst.
 * Function derived from compute_range_stats of file rangetypes_typanalyze.c 
 */
static void
temps_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
					int samplerows, double totalrows, bool valuestats)
{
	int null_cnt = 0,
		analyzed_rows = 0,
		slot_idx = 0,
		num_bins = stats->attr->attstattarget,
		num_hist;
	float8 *value_lengths, 
		   *time_lengths;
	RangeBound *value_lowers,
			*value_uppers;
	PeriodBound *time_lowers,
			*time_uppers;
	double total_width = 0;
	Oid rangetypid;

	temporal_extra_data = (TemporalAnalyzeExtraData *)stats->extra_data;

	if (valuestats)
	{
		/* Ensure function is called for temporal numbers */
		numeric_base_type_oid(temporal_extra_data->value_type_id);
		if (temporal_extra_data->value_type_id == INT4OID)
			rangetypid = type_oid(T_INTRANGE);
		else if (temporal_extra_data->value_type_id == FLOAT8OID)
			rangetypid = type_oid(T_FLOATRANGE);
		value_lowers = (RangeBound *) palloc(sizeof(RangeBound) * samplerows);
		value_uppers = (RangeBound *) palloc(sizeof(RangeBound) * samplerows);
		value_lengths = (float8 *) palloc(sizeof(float8) * samplerows);
	}
	time_lowers = (PeriodBound *) palloc(sizeof(PeriodBound) * samplerows);
	time_uppers = (PeriodBound *) palloc(sizeof(PeriodBound) * samplerows);
	time_lengths = (float8 *) palloc(sizeof(float8) * samplerows);

	/* Loop over the temporal values. */
	for (int i = 0; i < samplerows; i++)
	{
		Datum value;
		bool isnull, isempty;
		RangeType *range;
		TypeCacheEntry *typcache;
		RangeBound range_lower,
				range_upper;
		Period period;
		PeriodBound period_lower,
				period_upper;
		Temporal *temp;
	
		vacuum_delay_point();

		value = fetchfunc(stats, i, &isnull);
		if (isnull)
		{
			/* Temporal is null, just count that */
			null_cnt++;
			continue;
		}

		/* Skip too-large values. */
		if (toast_raw_datum_size(value) > TEMPORAL_WIDTH_THRESHOLD)
			continue;

		total_width += VARSIZE_ANY(DatumGetPointer(value));

		/* Get Temporal value */
		temp = DatumGetTemporal(value);

		/* Remember bounds and length for further usage in histograms */
		if (valuestats)
		{
			range = tnumber_value_range_internal(temp);
			typcache = lookup_type_cache(rangetypid, TYPECACHE_RANGE_INFO);
			range_deserialize(typcache, range, &range_lower, &range_upper, &isempty);
			value_lowers[analyzed_rows] = range_lower;
			value_uppers[analyzed_rows] = range_upper;

			if (temporal_extra_data->value_type_id == INT4OID)
				value_lengths[analyzed_rows] = DatumGetInt32(range_upper.val) -
					DatumGetInt32(range_lower.val);
			else if (temporal_extra_data->value_type_id == FLOAT8OID)
				value_lengths[analyzed_rows] = (float8) (DatumGetFloat8(range_upper.val) -
					DatumGetFloat8(range_lower.val));
		}
		temporal_timespan_internal(&period, temp);
		period_deserialize(&period, &period_lower, &period_upper);
		time_lowers[analyzed_rows] = period_lower;
		time_uppers[analyzed_rows] = period_upper;
		time_lengths[analyzed_rows] = period_duration_secs(period_upper.val, 
			period_lower.val);

		analyzed_rows++;
	}

	/* We can only compute real stats if we found some non-null values. */
	if (analyzed_rows > 0)
	{
		int pos,
			posfrac,
			delta,
			deltafrac,
			i;
		MemoryContext old_cxt;
		float4	   *emptyfrac;

		stats->stats_valid = true;

		/* Do the simple null-frac and width stats */
		stats->stanullfrac = (float4) null_cnt / (float4) samplerows;
		stats->stawidth = (int) (total_width / analyzed_rows);

		/* Estimate that non-null values are unique */
		stats->stadistinct = (float4) (-1.0 * (1.0 - stats->stanullfrac));

		/* Must copy the target values into anl_context */
		old_cxt = MemoryContextSwitchTo(stats->anl_context);

		if (valuestats)
		{
			Datum *value_bound_hist_values;
			Datum *value_length_hist_values;

			/*
			* Generate value histograms if there are at least two values.
			*/
			if (analyzed_rows >= 2)
			{
				/* Generate a bounds histogram slot entry */

				/* Sort bound values */
				qsort(value_lowers, analyzed_rows, sizeof(RangeBound), range_bound_qsort_cmp);
				qsort(value_uppers, analyzed_rows, sizeof(RangeBound), range_bound_qsort_cmp);

				num_hist = analyzed_rows;
				if (num_hist > num_bins)
					num_hist = num_bins + 1;

				value_bound_hist_values = (Datum *) palloc(num_hist * sizeof(Datum));

				/*
				* The object of this loop is to construct ranges from first and
				* last entries in lowers[] and uppers[] along with evenly-spaced
				* values in between. So the i'th value is a range of lowers[(i *
				* (nvals - 1)) / (num_hist - 1)] and uppers[(i * (nvals - 1)) /
				* (num_hist - 1)]. But computing that subscript directly risks
				* integer overflow when the stats target is more than a couple
				* thousand.  Instead we add (nvals - 1) / (num_hist - 1) to pos
				* at each step, tracking the integral and fractional parts of the
				* sum separately.
				*/
				delta = (analyzed_rows - 1) / (num_hist - 1);
				deltafrac = (analyzed_rows - 1) % (num_hist - 1);
				pos = posfrac = 0;

				for (i = 0; i < num_hist; i++)
				{
					value_bound_hist_values[i] = PointerGetDatum(
							range_make(value_lowers[pos].val, value_uppers[pos].val,
									true, true, temporal_extra_data->value_type_id));

					pos += delta;
					posfrac += deltafrac;
					if (posfrac >= (num_hist - 1))
					{
						/* fractional part exceeds 1, carry to integer part */
						pos++;
						posfrac -= (num_hist - 1);
					}
				}

				TypeCacheEntry *range_typeentry = lookup_type_cache(rangetypid,
																	TYPECACHE_EQ_OPR |
																	TYPECACHE_CMP_PROC_FINFO |
																	TYPECACHE_HASH_PROC_FINFO);

				stats->stakind[slot_idx] = STATISTIC_KIND_BOUNDS_HISTOGRAM;
				stats->staop[slot_idx] = temporal_extra_data->value_eq_opr;
				stats->stavalues[slot_idx] = value_bound_hist_values;
				stats->numvalues[slot_idx] = num_hist;
				stats->statypid[slot_idx] = range_typeentry->type_id;
				stats->statyplen[slot_idx] = range_typeentry->typlen;
				stats->statypbyval[slot_idx] =range_typeentry->typbyval;
				stats->statypalign[slot_idx] = range_typeentry->typalign;

				slot_idx++;

				/* Generate a length histogram slot entry */

				/*
				* Ascending sort of range lengths for further filling of
				* histogram
				*/
				qsort(value_lengths, analyzed_rows, sizeof(float8), float8_qsort_cmp);

				num_hist = analyzed_rows;
				if (num_hist > num_bins)
					num_hist = num_bins + 1;

				value_length_hist_values = (Datum *) palloc(num_hist * sizeof(Datum));

				/*
				* The object of this loop is to copy the first and last lengths[]
				* entries along with evenly-spaced values in between. So the i'th
				* value is lengths[(i * (nvals - 1)) / (num_hist - 1)]. But
				* computing that subscript directly risks integer overflow when
				* the stats target is more than a couple thousand.  Instead we
				* add (nvals - 1) / (num_hist - 1) to pos at each step, tracking
				* the integral and fractional parts of the sum separately.
				*/
				delta = (analyzed_rows - 1) / (num_hist - 1);
				deltafrac = (analyzed_rows - 1) % (num_hist - 1);
				pos = posfrac = 0;

				for (i = 0; i < num_hist; i++)
				{
					value_length_hist_values[i] = Float8GetDatum(value_lengths[pos]);
					pos += delta;
					posfrac += deltafrac;
					if (posfrac >= (num_hist - 1))
					{
						/* fractional part exceeds 1, carry to integer part */
						pos++;
						posfrac -= (num_hist - 1);
					}
				}
			}
			else
			{
				/*
				* Even when we don't create the histogram, store an empty array
				* to mean "no histogram". We can't just leave stavalues NULL,
				* because get_attstatsslot() errors if you ask for stavalues, and
				* it's NULL. We'll still store the empty fraction in stanumbers.
				*/
				value_length_hist_values = palloc(0);
				num_hist = 0;
			}
			stats->stakind[slot_idx] = STATISTIC_KIND_RANGE_LENGTH_HISTOGRAM;
			stats->staop[slot_idx] = Float8LessOperator;
			stats->stavalues[slot_idx] = value_length_hist_values;
			stats->numvalues[slot_idx] = num_hist;
			stats->statypid[slot_idx] = FLOAT8OID;
			stats->statyplen[slot_idx] = sizeof(float8);
			stats->statypbyval[slot_idx] = true;
			stats->statypalign[slot_idx] = 'd';

			/* Store 0 as value for the fraction of empty ranges */
			emptyfrac = (float4 *) palloc(sizeof(float4));
			*emptyfrac = 0.0;
			stats->stanumbers[slot_idx] = emptyfrac;
			stats->numnumbers[slot_idx] = 1;
		}

		Datum *bound_hist_time;
		Datum *length_hist_time;

		/*
		 * Generate temporal histograms if there are at least two values.
		 */
		if (analyzed_rows >= 2)
		{
			/* Generate a bounds histogram slot entry */

			/* Sort bound values */
			qsort(time_lowers, analyzed_rows, sizeof(PeriodBound), period_bound_qsort_cmp);
			qsort(time_uppers, analyzed_rows, sizeof(PeriodBound), period_bound_qsort_cmp);

			num_hist = analyzed_rows;
			if (num_hist > num_bins)
				num_hist = num_bins + 1;

			bound_hist_time = (Datum *) palloc(num_hist * sizeof(Datum));

			/*
			 * The object of this loop is to construct periods from first and
			 * last entries in lowers[] and uppers[] along with evenly-spaced
			 * values in between. So the i'th value is a period of lowers[(i *
			 * (nvals - 1)) / (num_hist - 1)] and uppers[(i * (nvals - 1)) /
			 * (num_hist - 1)]. But computing that subscript directly risks
			 * integer overflow when the stats target is more than a couple
			 * thousand.  Instead we add (nvals - 1) / (num_hist - 1) to pos
			 * at each step, tracking the integral and fractional parts of the
			 * sum separately.
			 */
			delta = (analyzed_rows - 1) / (num_hist - 1);
			deltafrac = (analyzed_rows - 1) % (num_hist - 1);
			pos = posfrac = 0;

			for (i = 0; i < num_hist; i++)
			{
				bound_hist_time[i] =
						PointerGetDatum(period_make(time_lowers[pos].val, time_uppers[pos].val,
													time_lowers[pos].inclusive, time_uppers[pos].inclusive));

				pos += delta;
				posfrac += deltafrac;
				if (posfrac >= (num_hist - 1))
				{
					/* fractional part exceeds 1, carry to integer part */
					pos++;
					posfrac -= (num_hist - 1);
				}
			}

			stats->stakind[slot_idx] = STATISTIC_KIND_PERIOD_BOUNDS_HISTOGRAM;
			stats->staop[slot_idx] = temporal_extra_data->time_eq_opr; 
			stats->stavalues[slot_idx] = bound_hist_time;
			stats->numvalues[slot_idx] = num_hist;
			stats->statypid[slot_idx] = temporal_extra_data->time_type_id;
			stats->statyplen[slot_idx] = temporal_extra_data->time_typlen;
			stats->statypbyval[slot_idx] = temporal_extra_data->time_typbyval;
			stats->statypalign[slot_idx] = temporal_extra_data->time_typalign;
			slot_idx++;

			/* Generate a length histogram slot entry. */

			/*
			 * Ascending sort of period lengths for further filling of
			 * histogram
			 */
			qsort(time_lengths, analyzed_rows, sizeof(float8), float8_qsort_cmp);

			num_hist = analyzed_rows;
			if (num_hist > num_bins)
				num_hist = num_bins + 1;

			length_hist_time = (Datum *) palloc(num_hist * sizeof(Datum));

			/*
			 * The object of this loop is to copy the first and last lengths[]
			 * entries along with evenly-spaced values in between. So the i'th
			 * value is lengths[(i * (nvals - 1)) / (num_hist - 1)]. But
			 * computing that subscript directly risks integer overflow when
			 * the stats target is more than a couple thousand.  Instead wes
			 * add (nvals - 1) / (num_hist - 1) to pos at each step, tracking
			 * the integral and fractional parts of the sum separately.
			 */
			delta = (analyzed_rows - 1) / (num_hist - 1);
			deltafrac = (analyzed_rows - 1) % (num_hist - 1);
			pos = posfrac = 0;

			for (i = 0; i < num_hist; i++)
			{
				length_hist_time[i] = Float8GetDatum(time_lengths[pos]);
				pos += delta;
				posfrac += deltafrac;
				if (posfrac >= (num_hist - 1))
				{
					/* fractional part exceeds 1, carry to integer part */
					pos++;
					posfrac -= (num_hist - 1);
				}
			}
		}
		else
		{
			/*
			 * Even when we don't create the histogram, store an empty array
			 * to mean "no histogram". We can't just leave stavalues NULL,
			 * because get_attstatsslot() errors if you ask for stavalues, and
			 * it's NULL.
			 */
			length_hist_time = palloc(0);
			num_hist = 0;
		}
		stats->stakind[slot_idx] = STATISTIC_KIND_PERIOD_LENGTH_HISTOGRAM;
		stats->staop[slot_idx] = Float8LessOperator;
		stats->stavalues[slot_idx] = length_hist_time;
		stats->numvalues[slot_idx] = num_hist;
		stats->statypid[slot_idx] = FLOAT8OID;
		stats->statyplen[slot_idx] = sizeof(float8);
		stats->statypbyval[slot_idx] = true;
		stats->statypalign[slot_idx] = 'd';

		MemoryContextSwitchTo(old_cxt);
	}
	else if (null_cnt > 0)
	{
		/* We found only nulls; assume the column is entirely null */
		stats->stats_valid = true;
		stats->stanullfrac = 1.0;
		stats->stawidth = 0;		/* "unknown" */
		stats->stadistinct = 0.0;	/* "unknown" */
	}

	/*
	 * We don't need to bother cleaning up any of our temporary palloc's. The
	 * hashtable should also go away, as it used a child memory context.
	 */
}

/*****************************************************************************
 * Statistics functions for temporal types
 *****************************************************************************/

void
temporalinst_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
						   int samplerows, double totalrows)
{
	return tempinst_compute_stats(stats, fetchfunc, samplerows, totalrows, false);
}

void
temporali_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
						int samplerows, double totalrows)
{
	return tempi_compute_stats(stats, fetchfunc, samplerows, totalrows, false);
}

void
temporals_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
						int samplerows, double totalrows)
{
	return temps_compute_stats(stats, fetchfunc, samplerows, totalrows, false);
}

/*****************************************************************************
 * Statistics functions for temporal types
 *****************************************************************************/

void
tnumberinst_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
						   int samplerows, double totalrows)
{
	return tempinst_compute_stats(stats, fetchfunc, samplerows, totalrows, true);
}

void
tnumberi_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
						int samplerows, double totalrows)
{
	return tempi_compute_stats(stats, fetchfunc, samplerows, totalrows, true);
}

void
tnumbers_compute_stats(VacAttrStats *stats, AnalyzeAttrFetchFunc fetchfunc,
						int samplerows, double totalrows)
{
	return temps_compute_stats(stats, fetchfunc, samplerows, totalrows, true);
}

/*****************************************************************************
 * Statistics information for temporal types
 *****************************************************************************/

void
temporal_extra_info(VacAttrStats *stats)
{
	TypeCacheEntry *typentry;
	TemporalAnalyzeExtraData *extra_data;
	Form_pg_attribute attr = stats->attr;

	/*
	 * Check attribute data type is a temporal type.
	 */
	if (! temporal_type_oid(stats->attrtypid))
		elog(ERROR, "temporal_analyze was invoked with invalid type %u",
			 stats->attrtypid);

	/* Store our findings for use by stats functions */
	extra_data = (TemporalAnalyzeExtraData *) palloc(sizeof(TemporalAnalyzeExtraData));

	/*
	 * Gather information about the temporal type and its value and time types.
	 */

	/* Information about the temporal type */
	typentry = lookup_type_cache(stats->attrtypid,
								 TYPECACHE_EQ_OPR | TYPECACHE_LT_OPR |
								 TYPECACHE_CMP_PROC_FINFO |
								 TYPECACHE_HASH_PROC_FINFO);
	extra_data->type_id = typentry->type_id;
	extra_data->eq_opr = typentry->eq_opr;
	extra_data->lt_opr = typentry->lt_opr;
	extra_data->typbyval = typentry->typbyval;
	extra_data->typlen = typentry->typlen;
	extra_data->typalign = typentry->typalign;
	extra_data->cmp = &typentry->cmp_proc_finfo;
	extra_data->hash = &typentry->hash_proc_finfo;

	/* Information about the value type */
	typentry = lookup_type_cache(base_oid_from_temporal(stats->attrtypid),
										TYPECACHE_EQ_OPR | TYPECACHE_LT_OPR |
										TYPECACHE_CMP_PROC_FINFO |
										TYPECACHE_HASH_PROC_FINFO);
	extra_data->value_type_id = typentry->type_id;
	extra_data->value_eq_opr = typentry->eq_opr;
	extra_data->value_lt_opr = typentry->lt_opr;
	extra_data->value_typbyval = typentry->typbyval;
	extra_data->value_typlen = typentry->typlen;
	extra_data->value_typalign = typentry->typalign;
	extra_data->value_cmp = &typentry->cmp_proc_finfo;
	extra_data->value_hash = &typentry->hash_proc_finfo;

	/* Information about the time type, that is TimestampTz */
	if (stats->attrtypmod == TEMPORALINST || stats->attrtypmod == TEMPORALI)
	{
		typentry = lookup_type_cache(TIMESTAMPTZOID,
										  TYPECACHE_EQ_OPR | TYPECACHE_LT_OPR |
										  TYPECACHE_CMP_PROC_FINFO |
										  TYPECACHE_HASH_PROC_FINFO);
		extra_data->time_type_id = TIMESTAMPTZOID;
		extra_data->time_eq_opr = typentry->eq_opr;
		extra_data->time_lt_opr = typentry->lt_opr;
		extra_data->time_typbyval = true;
		extra_data->time_typlen = sizeof(TimestampTz);
		extra_data->time_typalign = 'd';
		extra_data->time_cmp = &typentry->cmp_proc_finfo;
		extra_data->time_hash = &typentry->hash_proc_finfo;
	}
	else
	{
		Oid pertypoid = type_oid(T_PERIOD);
		typentry = lookup_type_cache(pertypoid,
										  TYPECACHE_EQ_OPR | TYPECACHE_LT_OPR |
										  TYPECACHE_CMP_PROC_FINFO |
										  TYPECACHE_HASH_PROC_FINFO);
		extra_data->time_type_id = pertypoid;
		extra_data->time_eq_opr = typentry->eq_opr;
		extra_data->time_lt_opr = typentry->lt_opr;
		extra_data->time_typbyval = false;
		extra_data->time_typlen = sizeof(Period);
		extra_data->time_typalign = 'd';
		extra_data->time_cmp = &typentry->cmp_proc_finfo;
		extra_data->time_hash = &typentry->hash_proc_finfo;
	}

	extra_data->std_extra_data = stats->extra_data;
	stats->extra_data = extra_data;

	stats->minrows = 300 * attr->attstattarget;
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(temporal_analyze);

PGDLLEXPORT Datum
temporal_analyze(PG_FUNCTION_ARGS)
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
	if (duration == TEMPORALINST)
		stats->compute_stats = temporalinst_compute_stats;
	else if (duration == TEMPORALI)
		stats->compute_stats = temporali_compute_stats;
	else 
    	stats->compute_stats = temporals_compute_stats;

	PG_RETURN_BOOL(true);
}

/*****************************************************************************/

PG_FUNCTION_INFO_V1(tnumber_analyze);

PGDLLEXPORT Datum
tnumber_analyze(PG_FUNCTION_ARGS)
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
	if (duration == TEMPORALINST)
		stats->compute_stats = tnumberinst_compute_stats;
	else if (duration == TEMPORALI)
		stats->compute_stats = tnumberi_compute_stats;
	else
		stats->compute_stats = tnumbers_compute_stats;

	PG_RETURN_BOOL(true);
}

/*****************************************************************************/
