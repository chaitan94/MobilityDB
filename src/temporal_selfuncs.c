/*****************************************************************************
 *
 * temporal_selfuncs.c
 * 
 * Functions for selectivity estimation of operators on temporal types whose
 * bounding box is a period, that is, tbool and ttext.
 *
 * The operators currently supported are as follows
 * - B-tree comparison operators: <, <=, >, >=
 * - Bounding box operators: &&, @>, <@, ~=
 * - Relative position operators: <<#, &<#, #>>, #>>
 * - Ever/always equal operators: &=, @= // TODO
 *
 * Due to implicit casting, a condition such as tbool <<# timestamptz will be
 * transformed into tbool <<# period. This allows to reduce the number of 
 * cases for the operator definitions, indexes, selectivity, etc. 
 * 
 * Portions Copyright (c) 2019, Esteban Zimanyi, Mahmoud Sakr, Mohamed Bakli,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "temporal_selfuncs.h"

#include <assert.h>
#include <access/heapam.h>
#include <access/htup_details.h>
#include <access/itup.h>
#include <access/relscan.h>
#include <access/visibilitymap.h>
#include <access/skey.h>
#include <catalog/pg_collation_d.h>
#include <executor/tuptable.h>
#include <optimizer/paths.h>
#include <storage/bufmgr.h>
#include <utils/builtins.h>
#include <utils/date.h>
#include <utils/datum.h>
#include <utils/memutils.h>
#include <utils/rel.h>
#include <utils/syscache.h>
#include <utils/tqual.h>
#include <temporal_boxops.h>
#include <timetypes.h>

#include "timestampset.h"
#include "period.h"
#include "periodset.h"
#include "time_selfuncs.h"
#include "rangetypes_ext.h"
#include "temporal_analyze.h"
#include "tpoint.h"

/*****************************************************************************
 * Internal functions computing selectivity
 * The functions assume that the value and time dimensions of temporal values 
 * are independent and thus the selectivity values obtained by analyzing the 
 * histograms for each dimension can be multiplied.
 *****************************************************************************/

/*
 * Transform the constant into a period
 */
static bool
temporal_const_to_period(Node *other, Period *period)
{
	Oid consttype = ((Const *) other)->consttype;

	if (consttype == TIMESTAMPTZOID)
	{
		TimestampTz t = DatumGetTimestampTz(((Const *) other)->constvalue);
		period_set(period, t, t, true, true);
	}
	else if (consttype == type_oid(T_TIMESTAMPSET))
		memcpy(period, timestampset_bbox(
				DatumGetTimestampSet(((Const *) other)->constvalue)), sizeof(Period));
	else if (consttype == type_oid(T_PERIOD))
		memcpy(period, DatumGetPeriod(((Const *) other)->constvalue), sizeof(Period));
	else if (consttype== type_oid(T_PERIODSET))
		memcpy(period, periodset_bbox(
				DatumGetPeriodSet(((Const *) other)->constvalue)), sizeof(Period));
	else if (consttype == type_oid(T_TBOOL) || consttype == type_oid(T_TTEXT))
		temporal_bbox(period, DatumGetTemporal(((Const *) other)->constvalue));
	else
		return false;
	return true;
}

/* Get the enum value associated to the operator */
static bool
temporal_cachedop(Oid operator, CachedOp *cachedOp)
{
	for (int i = LT_OP; i <= OVERAFTER_OP; i++) {
		if (operator == oper_oid((CachedOp) i, T_PERIOD, T_TBOOL) ||
			operator == oper_oid((CachedOp) i, T_TBOOL, T_PERIOD) ||
			operator == oper_oid((CachedOp) i, T_TBOX, T_TBOOL) ||
			operator == oper_oid((CachedOp) i, T_TBOOL, T_TBOX) ||
			operator == oper_oid((CachedOp) i, T_TBOOL, T_TBOOL) ||
			operator == oper_oid((CachedOp) i, T_PERIOD, T_TTEXT) ||
			operator == oper_oid((CachedOp) i, T_TTEXT, T_PERIOD) ||
			operator == oper_oid((CachedOp) i, T_TBOX, T_TTEXT) ||
			operator == oper_oid((CachedOp) i, T_TTEXT, T_TBOX) ||
			operator == oper_oid((CachedOp) i, T_TTEXT, T_TTEXT))
			{
				*cachedOp = (CachedOp) i;
				return true;
			}
	}
	return false;
}

/*
 * Returns a default selectivity estimate for given operator, when we don't
 * have statistics or cannot use them for some reason.
 */
static double
default_temporal_selectivity(CachedOp operator)
{
	switch (operator)
	{
		case OVERLAPS_OP:
			return 0.005;

		case CONTAINS_OP:
		case CONTAINED_OP:
			return 0.002;

		case SAME_OP:
			return 0.001;

		case LEFT_OP:
		case RIGHT_OP:
		case OVERLEFT_OP:
		case OVERRIGHT_OP:
		case AFTER_OP:
		case BEFORE_OP:
		case OVERAFTER_OP:
		case OVERBEFORE_OP:

			/* these are similar to regular scalar inequalities */
			return DEFAULT_INEQ_SEL;

		default:
			/* all operators should be handled above, but just in case */
			return 0.001;
	}
}

/* 
 * Compute selectivity for columns of TemporalInst duration 
 */
Selectivity
temporalinst_sel(PlannerInfo *root, VariableStatData *vardata,
	Period *period, CachedOp cachedOp)
{
	double selec = 0.0;
	Oid operator;
	bool iseq;

	if (cachedOp == SAME_OP || cachedOp == CONTAINS_OP)
	{
		/* If the period is not equivalent to a TimestampTz return 0.0 */
		if (period->lower != period->upper)
			return selec;
		
		operator = oper_oid(EQ_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = var_eq_const(vardata, operator, TimestampTzGetDatum(period->lower), 
			false, false, false);
	}
	else if (cachedOp == CONTAINED_OP || cachedOp == OVERLAPS_OP)
	{
		/* 
		 * For TemporalInst, the two conditions TimestampTz t <@ Period p and
		 * TimestampTz t && Period p are equivalent. Furtheremore, if the
		 * lower and upper bounds of the period are inclusive, then
		 * 
		 * TimestampTz t <@ Period p <=> lower(p) <= t AND t <= upper(p)
		 * 		<=> NOT (lower(p) > t OR t > upper(p))
		 * 		<=> NOT (t < lower(p) OR t > upper(p))
		 *
		 * Since t < lower(p) and t > upper(p) are mutually exclusive 
		 * events we can sum their probabilities to find probability of 
		 * t < lower(p) OR t > upper(p). In the code that follows we
		 * take care of whether the lower bounds are inclusive or not
		 */			
		operator = period->lower_inc ? 
			oper_oid(LT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ) :
			oper_oid(LE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, false, period->lower_inc, 
			vardata, TimestampTzGetDatum(period->lower), TIMESTAMPTZOID);
		operator = period->upper_inc ? 
			oper_oid(GT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ) :
			oper_oid(GE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec += scalarineqsel(root, operator, true, period->upper_inc, 
			vardata, TimestampTzGetDatum(period->upper), TIMESTAMPTZOID);
		selec = 1 - selec;
	}
	/* For b-tree comparisons, temporal values are first compared wrt 
	 * their bounding boxes, and if these are equal, other criteria apply.
	 * For selectivity estimation we approximate by taking into account
	 * only the bounding boxes. In the case here we use the scalar 
	 * inequality selectivity */
	else if (cachedOp == BEFORE_OP || cachedOp == LT_OP || cachedOp == LE_OP)
	{
		/* TimestampTz t <<# Period p <=> t < (<=) lower(p) depending on
		 * whether lower_inc(p) is true or false */
		operator = period->lower_inc ? 
			oper_oid(LT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ) :
			oper_oid(LE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		iseq = (cachedOp == LE_OP) ? true : ! period->lower_inc;
		selec = scalarineqsel(root, operator, false, iseq, vardata, 
			period->lower, TIMESTAMPTZOID);
	}
	else if (cachedOp == AFTER_OP || cachedOp == GT_OP || cachedOp == GE_OP)
	{
		/* TimestampTz t #>> Period p <=> t > (>=) upper(p) depending on 
		 * whether lower_inc(p) is true or false */
		operator = period->upper_inc ? 
			oper_oid(GT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ) :
			oper_oid(GE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		iseq = (cachedOp == LE_OP) ? true : ! period->upper_inc;
		selec = scalarineqsel(root, operator, true, iseq, vardata, 
			period->lower, TIMESTAMPTZOID);
	}
	else if (cachedOp == OVERBEFORE_OP)
	{
		/* TimestampTz t &<# Period p <=> t <= (<) upper(p) depending on 
		 * whether lower_inc(p) is true or false */
		operator = period->upper_inc ? 
			oper_oid(LE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ) :
			oper_oid(LT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, false, period->upper_inc, vardata, 
			period->upper, TIMESTAMPTZOID);
	}
	else if (cachedOp == OVERAFTER_OP)
	{
		/* TimestampTz t #&> Period p <=> t >= (>) lower(p) depending on
		 * whether lower_inc(p) is true or false */
		operator = period->lower_inc ? 
			oper_oid(GE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ) :
			oper_oid(GT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, true, period->lower_inc, vardata, 
			period->lower, TIMESTAMPTZOID);
	}
	else /* Unknown operator */
	{
		selec = default_temporal_selectivity(cachedOp);
	}
	return selec;
}

/*
 * Compute selectivity for columns of durations distinct from TemporalInst,
 * including columns containing temporal values of mixed durations.
 */
Selectivity
temporals_sel(PlannerInfo *root, VariableStatData *vardata,
	Period *period, CachedOp cachedOp)
{
	double selec = 0.0;
	Oid operator = oper_oid(cachedOp, T_PERIOD, T_PERIOD);

	/*
	 * There is no ~= operator for time types and thus it is necessary to
	 * take care of this operator here.
	 */
	if (cachedOp == SAME_OP)
	{
		operator = oper_oid(EQ_OP, T_PERIOD, T_PERIOD);
		selec = var_eq_const(vardata, operator, PeriodGetDatum(period), 
			false, false, false);
	}
	else if (cachedOp == OVERLAPS_OP || cachedOp == CONTAINS_OP ||
		cachedOp == CONTAINED_OP || cachedOp == BEFORE_OP ||
		cachedOp == AFTER_OP || cachedOp == OVERBEFORE_OP || 
		cachedOp == OVERAFTER_OP) 
	{
		selec = calc_period_hist_selectivity(vardata, period, cachedOp);
	}
	else if (cachedOp == LT_OP || cachedOp == LE_OP || 
		cachedOp == GT_OP || cachedOp == GE_OP) 
	{
		/* For b-tree comparisons, temporal values are first compared wrt 
		 * their bounding boxes, and if these are equal, other criteria apply.
		 * For selectivity estimation we approximate by taking into account
		 * only the bounding boxes. In the case here the bounding box is a
		 * period and thus we can use the period selectivity estimation */
		selec = calc_period_hist_selectivity(vardata, period, cachedOp);
	}
	else /* Unknown operator */
	{
		selec = default_temporal_selectivity(cachedOp);
	}
	return selec;
}

/*****************************************************************************/

/*
 * Estimate the selectivity value of the operators for temporal types whose
 * bounding box is a Period, that is, tbool and ttext.
 */
PG_FUNCTION_INFO_V1(temporal_sel);

PGDLLEXPORT Datum
temporal_sel(PG_FUNCTION_ARGS)
{
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
	Oid operator = PG_GETARG_OID(1);
	List *args = (List *) PG_GETARG_POINTER(2);
	int varRelid = PG_GETARG_INT32(3);
	VariableStatData vardata;
	Node *other;
	bool varonleft;
	Selectivity selec = DEFAULT_TEMP_SELECTIVITY;
	CachedOp cachedOp;
	Period constperiod;

	/*
	 * Get enumeration value associated to the operator
	 */
	bool found = temporal_cachedop(operator, &cachedOp);
	/* In the case of unknown operator */
	if (!found)
		PG_RETURN_FLOAT8(DEFAULT_TEMP_SELECTIVITY);

	/*
	 * If expression is not (variable op something) or (something op
	 * variable), then punt and return a default estimate.
	 */
	if (!get_restriction_variable(root, args, varRelid,
								  &vardata, &other, &varonleft))
		PG_RETURN_FLOAT8(default_temporal_selectivity(cachedOp));

	/*
	 * Can't do anything useful if the something is not a constant, either.
	 */
	if (!IsA(other, Const))
	{
		ReleaseVariableStats(vardata);
		PG_RETURN_FLOAT8(default_temporal_selectivity(cachedOp));
	}

	/*
	 * All the period operators are strict, so we can cope with a NULL constant
	 * right away.
	 */
	if (((Const *) other)->constisnull)
	{
		ReleaseVariableStats(vardata);
		PG_RETURN_FLOAT8(0.0);
	}

	/*
	 * If var is on the right, commute the operator, so that we can assume the
	 * var is on the left in what follows.
	 */
	if (!varonleft)
	{
		/* we have other Op var, commute to make var Op other */
		operator = get_commutator(operator);
		if (!operator)
		{
			/* Use default selectivity (should we raise an error instead?) */
			ReleaseVariableStats(vardata);
			PG_RETURN_FLOAT8(default_temporal_selectivity(cachedOp));
		}
	}

	/*
	 * Transform the constant into a Period
	 */
	found = temporal_const_to_period(other, &constperiod);
	/* In the case of unknown constant */
	if (!found)
		PG_RETURN_FLOAT8(default_temporal_selectivity(cachedOp));

	/* Get the duration of the temporal column */
	int duration = TYPMOD_GET_DURATION(vardata.atttypmod);
	temporal_duration_all_is_valid(duration);

	/* Dispatch based on duration */
	if (duration == TEMPORALINST)
		selec = temporalinst_sel(root, &vardata, &constperiod, cachedOp);
	else
		selec = temporals_sel(root, &vardata, &constperiod, cachedOp);

	ReleaseVariableStats(vardata);
	CLAMP_PROBABILITY(selec);
	PG_RETURN_FLOAT8(selec);
}

PG_FUNCTION_INFO_V1(temporal_joinsel);

PGDLLEXPORT Datum
temporal_joinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(DEFAULT_TEMP_SELECTIVITY);
}

/*****************************************************************************/
