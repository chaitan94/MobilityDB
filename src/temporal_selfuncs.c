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
#include <utils/array_selfuncs.h>
#include <utils/builtins.h>
#include <utils/date.h>
#include <utils/datum.h>
#include <utils/memutils.h>
#include <utils/rel.h>
#include <utils/selfuncs.h>
#include <utils/syscache.h>
#include <utils/tqual.h>
#include <temporal_boxops.h>
#include <timetypes.h>

#include "timestampset.h"
#include "period.h"
#include "periodset.h"
#include "time_selfuncs.h"
#include "rangetypes_ext.h"
#include "temporaltypes.h"
#include "temporal_util.h"
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
 * Compute selectivity for columns of durations TemporalInst.
 */
Selectivity
temporali_sel(PlannerInfo *root, VariableStatData *vardata,
	Node *other, CachedOp cachedOp)
{
	TemporalI *ti = DatumGetTemporalI(((Const *) other)->constvalue);
	ArrayType *timestamps;
	Oid operator;
	double selec = 0.0;

	/* If not a temporal constant we cannot do much. Indeed, all other types
	 * of constants, including TimestampSet, are automatically casted to Period
	 * for the bounding box operators */
	if (! temporal_type_oid(((Const *) other)->consttype))
		return DEFAULT_TEMP_SELECTIVITY;

	/* Get the timespan of the TemporalI as an array of timestamps */
	timestamps = temporali_timestamps(ti);

	/*
	 * There is no ~= operator for time types and thus it is necessary to
	 * take care of this operator here.
	 */
	if (cachedOp == SAME_OP)
	{
		/* TODO */
		selec = default_temporal_selectivity(cachedOp);
	}
	else if (cachedOp == OVERLAPS_OP)
	{
		operator = OID_ARRAY_OVERLAP_OP;
		selec = calc_arraycontsel(vardata, PointerGetDatum(timestamps),
				 TIMESTAMPTZOID, operator);
	}
	else if (cachedOp == CONTAINS_OP)
	{
		operator = OID_ARRAY_CONTAINS_OP;
		selec = calc_arraycontsel(vardata, PointerGetDatum(timestamps),
				  TIMESTAMPTZOID, operator);
	}
	else if (cachedOp == CONTAINED_OP)
	{
		operator = OID_ARRAY_CONTAINED_OP;
		selec = calc_arraycontsel(vardata, PointerGetDatum(timestamps),
				  TIMESTAMPTZOID, operator);
	}
	else if (cachedOp == BEFORE_OP || cachedOp == AFTER_OP || 
		cachedOp == OVERBEFORE_OP || cachedOp == OVERAFTER_OP) 
	{
		/* TODO */
		selec = default_temporal_selectivity(cachedOp);
	}
	else if (cachedOp == LT_OP || cachedOp == LE_OP || 
		cachedOp == GT_OP || cachedOp == GE_OP) 
	{
		/* For b-tree comparisons, temporal values are first compared wrt 
		 * their bounding boxes, and if these are equal, other criteria apply.
		 * For selectivity estimation we approximate by taking into account
		 * only the bounding boxes. In the case here the bounding box is a
		 * period and thus we can use the period selectivity estimation */
		/* TODO */
		selec = default_temporal_selectivity(cachedOp);
	}
	else /* Unknown operator */
	{
		selec = default_temporal_selectivity(cachedOp);
	}
	pfree(timestamps);
	return selec;
}

/*
 * Compute selectivity for columns of durations TemporalSeq, TemporalS,
 * and Temporal (columns containing temporal values of mixed duration).
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

double
temporal_joinsel_inner(Oid operator, CachedOp cachedOp, VariableStatData *vardata1, 
	VariableStatData *vardata2, int duration1, int duration2)
{
	if (duration1 == TEMPORALINST && duration2 == TEMPORALINST &&
		cachedOp  == EQ_OP)
	{
		Oid operator1 = oper_oid(EQ_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		return eqjoinsel_inner(operator1, vardata1, vardata2);
	}

	return DEFAULT_TEMP_SELECTIVITY;
}

double
temporal_joinsel_semi(Oid operator,CachedOp cachedOp,  VariableStatData *vardata1, 
	VariableStatData *vardata2, RelOptInfo *inner_rel, 
	int duration1, int duration2)
{
	if (duration1 == TEMPORALINST && duration2 == TEMPORALINST &&
		cachedOp  == EQ_OP)
	{
		Oid operator1 = oper_oid(EQ_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		return eqjoinsel_semi(operator1, vardata1, vardata2, inner_rel);
	}

	return DEFAULT_TEMP_SELECTIVITY;
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
	Selectivity selec;
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
	else if (duration == TEMPORALI)
		selec = temporali_sel(root, &vardata, other, cachedOp);
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
	PlannerInfo *root = (PlannerInfo *) PG_GETARG_POINTER(0);
	Oid			operator = PG_GETARG_OID(1);
	List	   *args = (List *) PG_GETARG_POINTER(2);
#ifdef NOT_USED
	JoinType	jointype = (JoinType) PG_GETARG_INT16(3);
#endif
	SpecialJoinInfo *sjinfo = (SpecialJoinInfo *) PG_GETARG_POINTER(4);
	double		selec;
	VariableStatData vardata1;
	VariableStatData vardata2;
	bool		join_is_reversed;
	RelOptInfo *inner_rel;
	int 		duration1, duration2 = -1;
	CachedOp	cachedOp;

	/*
	 * Get enumeration value associated to the operator
	 */
	bool found = temporal_cachedop(operator, &cachedOp);
	/* In the case of unknown operator */
	if (!found)
		PG_RETURN_FLOAT8(DEFAULT_TEMP_SELECTIVITY);

	get_join_variables(root, args, sjinfo,
					   &vardata1, &vardata2, &join_is_reversed);

	/* Get the duration of the left temporal column */
	duration1 = TYPMOD_GET_DURATION(vardata1.atttypmod);
	temporal_duration_all_is_valid(duration1);

	/* The right column may be a temporal type or a period */
	if (temporal_type_oid(vardata2.atttype))
	{
		/* If the right column is a temporal type get the duration */
		duration2 = TYPMOD_GET_DURATION(vardata2.atttypmod);
		temporal_duration_all_is_valid(duration2);
	}
	else if (vardata2.atttype != type_oid(T_PERIOD))
		/* other values not expected here */
		elog(ERROR, "unrecognized type for join column: %d", vardata2.atttype);

	switch (sjinfo->jointype)
	{
		case JOIN_INNER:
		case JOIN_LEFT:
		case JOIN_FULL:
			selec = temporal_joinsel_inner(operator, cachedOp, &vardata1, 
				&vardata2, duration1, duration2);
			break;
		case JOIN_SEMI:
		case JOIN_ANTI:

			/*
			 * Look up the join's inner relation.  min_righthand is sufficient
			 * information because neither SEMI nor ANTI joins permit any
			 * reassociation into or out of their RHS, so the righthand will
			 * always be exactly that set of rels.
			 */
			inner_rel = find_join_input_rel(root, sjinfo->min_righthand);

			if (!join_is_reversed)
				selec = temporal_joinsel_semi(operator, cachedOp, &vardata1, &vardata2,
					inner_rel, duration1, duration2);
			else
			{
				operator = get_commutator(operator);
				found = temporal_cachedop(operator, &cachedOp);
				/* In the case of unknown operator */
				if (!found)
					PG_RETURN_FLOAT8(DEFAULT_TEMP_SELECTIVITY);

				selec = temporal_joinsel_semi(operator, cachedOp,
					&vardata2, &vardata1, inner_rel, duration1, duration2);
			}
			break;
		default:
			/* other values not expected here */
			elog(ERROR, "unrecognized join type: %d",
				 (int) sjinfo->jointype);
			selec = 0;			/* keep compiler quiet */
			break;
	}

	ReleaseVariableStats(vardata1);
	ReleaseVariableStats(vardata2);

	CLAMP_PROBABILITY(selec);

	PG_RETURN_FLOAT8((float8) selec);
}

/*****************************************************************************/
