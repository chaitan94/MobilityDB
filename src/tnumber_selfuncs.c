/*****************************************************************************
 *
 * tnumber_selfuncs.c
 *	  Functions for selectivity estimation of operators on temporal numeric types
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnumber_selfuncs.h"

#include <assert.h>
#include <access/htup_details.h>
#include <nodes/relation.h>
#include <utils/builtins.h>
#include <utils/array_selfuncs.h>
#include <utils/rangetypes_selfuncs.h>
#include <utils/selfuncs.h>
#include <temporal_boxops.h>

#include "period.h"
#include "rangetypes_ext.h"
#include "oidcache.h"
#include "tbox.h"
#include "time_selfuncs.h"
#include "temporaltypes.h"
#include "temporal_analyze.h"
#include "temporal_selfuncs.h"

/*****************************************************************************
 * Internal functions computing selectivity
 * The functions assume that the value and time dimensions of temporal values 
 * are independent and thus the selectivity values obtained by analyzing the 
 * histograms for each dimension can be multiplied.
 *****************************************************************************/

/* Transform the constant into a TBOX */
static bool
tnumber_const_to_tbox(const Node *other, TBOX *box)
{
    Oid consttype = ((Const *) other)->consttype;

    if (consttype == INT4OID)
        int_to_tbox_internal(box, ((Const *) other)->constvalue);
    else if (consttype == FLOAT8OID)
        float_to_tbox_internal(box, ((Const *) other)->constvalue);
    else if (consttype == type_oid(T_INTRANGE))
        intrange_to_tbox_internal(box, DatumGetRangeTypeP(((Const *) other)->constvalue));
    else if (consttype == type_oid(T_FLOATRANGE))
        floatrange_to_tbox_internal(box, DatumGetRangeTypeP(((Const *) other)->constvalue));
    else if (consttype == TIMESTAMPTZOID)
        timestamp_to_tbox_internal(box, DatumGetTimestampTz(((Const *) other)->constvalue));
    else if (consttype == type_oid(T_TIMESTAMPSET))
        timestampset_to_tbox_internal(box, ((TimestampSet *)((Const *) other)->constvalue));
    else if (consttype == type_oid(T_PERIOD))
        period_to_tbox_internal(box, (Period *) ((Const *) other)->constvalue);
    else if (consttype == type_oid(T_PERIODSET))
        periodset_to_tbox_internal(box, ((PeriodSet *)((Const *) other)->constvalue));
    else if (consttype == type_oid(T_TBOX))
        memcpy(box, DatumGetTboxP(((Const *) other)->constvalue), sizeof(TBOX));
    else if (consttype == type_oid(T_TINT) || consttype == type_oid(T_TFLOAT))
        temporal_bbox(box, DatumGetTemporal(((Const *) other)->constvalue));
    else
        return false;
    return true;
}

/* Get the enum value associated to the operator */
static bool
tnumber_cachedop(Oid operator, CachedOp *cachedOp)
{
	for (int i = LT_OP; i <= OVERAFTER_OP; i++)
	{
		if (operator == oper_oid((CachedOp) i, T_INTRANGE, T_TINT) ||
			operator == oper_oid((CachedOp) i, T_TBOX, T_TINT) ||
			operator == oper_oid((CachedOp) i, T_TINT, T_INTRANGE) ||
			operator == oper_oid((CachedOp) i, T_TINT, T_TBOX) ||
			operator == oper_oid((CachedOp) i, T_TINT, T_TINT) ||
			operator == oper_oid((CachedOp) i, T_TINT, T_TFLOAT) ||
			operator == oper_oid((CachedOp) i, T_FLOATRANGE, T_TFLOAT) ||
			operator == oper_oid((CachedOp) i, T_TBOX, T_TFLOAT) ||
			operator == oper_oid((CachedOp) i, T_TFLOAT, T_FLOATRANGE) ||
			operator == oper_oid((CachedOp) i, T_TFLOAT, T_TBOX) ||
			operator == oper_oid((CachedOp) i, T_TFLOAT, T_TINT) ||
			operator == oper_oid((CachedOp) i, T_TFLOAT, T_TFLOAT))
            {
                *cachedOp = (CachedOp) i;
                return true;
            }
	}
	return false;
}

/* Get the range operator associated to a cachedOp enum value */
static Oid
tnumber_cachedop_rangeop(CachedOp cachedOp)
{
	Oid op = InvalidOid;
	if (cachedOp == LT_OP)
		op = OID_RANGE_LESS_OP;
	else if (cachedOp == LE_OP)
		op = OID_RANGE_LESS_EQUAL_OP;
	else if (cachedOp == GE_OP)
		op = OID_RANGE_GREATER_EQUAL_OP;
	else if (cachedOp == GT_OP)
		op = OID_RANGE_GREATER_OP;
	else if (cachedOp == OVERLAPS_OP)
		op = OID_RANGE_OVERLAP_OP;
	else if (cachedOp == CONTAINS_OP)
		op = OID_RANGE_CONTAINS_OP;
	else if (cachedOp == CONTAINED_OP)
		op = OID_RANGE_CONTAINED_OP;
	else if (cachedOp == LEFT_OP)
		op = OID_RANGE_LEFT_OP;
	else if (cachedOp == RIGHT_OP)
		op = OID_RANGE_RIGHT_OP;
	else if (cachedOp == OVERLEFT_OP)
		op = OID_RANGE_OVERLAPS_LEFT_OP;
	else if (cachedOp == OVERRIGHT_OP)
		op = OID_RANGE_OVERLAPS_RIGHT_OP;
	return op;
}

/*
 * Returns a default selectivity estimate for given operator, when we don't
 * have statistics or cannot use them for some reason.
 */
static double
default_tnumber_selectivity(CachedOp operator)
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
		case ABOVE_OP:
		case BELOW_OP:
		case OVERABOVE_OP:
		case OVERBELOW_OP:
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
tnumberinst_sel(PlannerInfo *root, VariableStatData *vardata, TBOX *box, 
	CachedOp cachedOp, Oid valuetypid)
{
	double selec; 
	Oid operator;

	if (cachedOp == SAME_OP || cachedOp == CONTAINS_OP)
	{
		/* If the box is not equivalent to a temporal instant return 0.0 */
		if (box->xmin != box->xmax || box->tmin != box->tmax)
			return 0.0;

		/* Enable the multiplication of the selectivity of the value and time 
		 * dimensions since either may be missing */
		selec = 1.0; 

		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
		{
			operator = oper_oid(EQ_OP, valuetypid, valuetypid);
			selec *= var_eq_const(vardata, operator, 
				Float8GetDatum(box->xmin), false, false, false);
		}
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
		{
			operator *= oper_oid(EQ_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
			selec *= var_eq_const(vardata, operator, 
				TimestampTzGetDatum(box->tmin), false, false, false);
		}
	}
	else if (cachedOp == CONTAINED_OP || cachedOp == OVERLAPS_OP)
	{
		/* 
		 * For TemporalInst, the two conditions v@t <@ TBOX b and
		 * v@t && TBOX b are equivalent. Then
		 * 
		 * v@t <@ TBOX b <=> 
		 * 		box->xmin <= v AND v <= box->xmax AND box->tmin <= t AND t <= box->tmax <=> 
		 * 		NOT (box->xmin > v OR v > box->xmax OR box->tmin > t AND t > box->tmax) <=> 
		 * 		NOT (v < box->xmin OR v > box->xmax OR t < box->tmin AND t > box->tmax)
		 *
		 * Since the components of v < box->xmin OR ... are mutually exclusive 
		 * events we can sum their probabilities to find probability of 
		 * v < box->xmin OR ... 
		 */

		/* Enable the multiplication of the selectivity of the value and time 
		 * dimensions since either may be missing */
		selec = 0.0; 

		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
		{
			operator = oper_oid(LT_OP, valuetypid, valuetypid);
			selec += scalarineqsel(root, operator, false, false, vardata, 
				Float8GetDatum(box->xmin), valuetypid);
			operator = oper_oid(GT_OP, valuetypid, valuetypid);
			selec += scalarineqsel(root, operator, true, false, vardata, 
				Float8GetDatum(box->xmax), valuetypid);
		}
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
		{
			operator = oper_oid(LT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
			selec += scalarineqsel(root, operator, false, false, vardata, 
				TimestampTzGetDatum(box->tmin), TIMESTAMPTZOID);
			operator = oper_oid(GT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
			selec += scalarineqsel(root, operator, true, false, vardata, 
				TimestampTzGetDatum(box->tmax), TIMESTAMPTZOID);
		}
		selec = 1 - selec;
	}
	else if (cachedOp == LEFT_OP)
	{
		/* TemporalInst v@t << TBOX b <=> v < box->xmin */
		operator = oper_oid(LT_OP, valuetypid, valuetypid);
		selec = scalarineqsel(root, operator, false, false, vardata, 
			box->xmin, valuetypid);
	}
	else if (cachedOp == RIGHT_OP)
	{
		/* TemporalInst v@t >> TBOX b <=> v > box->xmax */
		operator = oper_oid(GT_OP, valuetypid, valuetypid);
		selec = scalarineqsel(root, operator, true, false, vardata, 
			box->xmax, valuetypid);
	}
	else if (cachedOp == OVERLEFT_OP)
	{
		/* TemporalInst v@t &< TBOX b <=> v <= box->xmax */
		operator = oper_oid(LE_OP, valuetypid, valuetypid);
		selec = scalarineqsel(root, operator, false, true, vardata, 
			box->xmax, valuetypid);
	}
	else if (cachedOp == OVERRIGHT_OP)
	{
		/* TemporalInst v@t &> TBOX b <=> v >= box->xmin */
		operator = oper_oid(GE_OP, valuetypid, valuetypid);
		selec = scalarineqsel(root, operator, true, true, vardata, 
			box->xmin, valuetypid);
	}
	else if (cachedOp == BEFORE_OP)
	{
		/* TemporalInst v@t <<# TBOX b <=> t < box->tmin */
		operator = oper_oid(LT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, false, false, vardata, 
			box->tmin, TIMESTAMPTZOID);
	}
	else if (cachedOp == AFTER_OP)
	{
		/* TemporalInst v@t #>> TBOX b <=> t > box->tmax */
		operator = oper_oid(GT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, true, false, vardata, 
			box->tmax, TIMESTAMPTZOID);
	}
	else if (cachedOp == OVERBEFORE_OP)
	{
		/* TemporalInst v@t &<# TBOX b <=> t <= box->tmax */
		operator = oper_oid(LE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, false, true, vardata, 
			box->tmax, TIMESTAMPTZOID);
	}
	else if (cachedOp == OVERAFTER_OP)
	{
		/* TemporalInst v@t #&> TBOX b <=> t >= box->tmin */
		operator = oper_oid(GE_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
		selec = scalarineqsel(root, operator, true, true, vardata, 
			box->tmin, TIMESTAMPTZOID);
	}
	/* For b-tree comparisons, temporal values are first compared wrt 
	 * their bounding boxes, and if these are equal, other criteria apply.
	 * For selectivity estimation we approximate by taking into account
	 * only the bounding boxes. In the case here we use the scalar 
	 * inequality selectivity */
	else if (cachedOp == LT_OP || cachedOp == LE_OP)
	{
		/* TemporalInst v@t < TBOX b <=> v < box->xmin AND t < box->tmin */

		/* Enable the multiplication of the selectivity of the value and time 
		 * dimensions since either may be missing */
		selec = 1.0; 

		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
		{
			operator = oper_oid(LT_OP, valuetypid, valuetypid);
			selec *= scalarineqsel(root, operator, false, cachedOp == LT_OP, 
				vardata, box->xmin, valuetypid);
		}
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
		{
			operator = oper_oid(LT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
			selec *= scalarineqsel(root, operator, false, cachedOp == LT_OP, 
				vardata, box->tmin, TIMESTAMPTZOID);
		}
	}
	else if (cachedOp == GT_OP || cachedOp == GE_OP)
	{
		/* TemporalInst v@t > TBOX b <=> v > box->xmax AND t > box->tmax */

		/* Enable the multiplication of the selectivity of the value and time 
		 * dimensions since either may be missing*/
		selec = 1.0; 

		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
		{
			operator = oper_oid(GT_OP, valuetypid, valuetypid);
			selec *= scalarineqsel(root, operator, true, cachedOp == GT_OP, 
				vardata, box->xmax, valuetypid);
		}
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
		{
			operator = oper_oid(GT_OP, T_TIMESTAMPTZ, T_TIMESTAMPTZ);
			selec *= scalarineqsel(root, operator, true, cachedOp == GT_OP, 
				vardata, box->tmax, TIMESTAMPTZOID);
		}
	}
	else /* Unknown operator */
	{
		selec = default_tnumber_selectivity(cachedOp);
	}
	return selec;
}

/*
 * Compute selectivity for columns of durations TemporalInst.
 */
Selectivity
tnumberi_sel(PlannerInfo *root, VariableStatData *vardata,
	Node *other, CachedOp cachedOp)
{
	TemporalI *ti = DatumGetTemporalI(((Const *) other)->constvalue);
	ArrayType *values, *timestamps;
	Oid operator;
	double selec = 0.0;

	/* If not a temporal constant we cannot do much. Indeed, all other types
	 * of constants, including TimestampSet, are automatically casted to Period
	 * for the bounding box operators */
	if (! temporal_type_oid(((Const *) other)->consttype))
		return DEFAULT_TEMP_SELECTIVITY;

	/* Get the arrays of values and timestamps of the TemporalI */
	values = temporali_values(ti);
	timestamps = temporali_timestamps(ti);

	/*
	 * There is no ~= operator for time types and thus it is necessary to
	 * take care of this operator here.
	 */
	if (cachedOp == SAME_OP)
	{
		/* TODO */
		selec = default_tnumber_selectivity(cachedOp);
	}
	else if (cachedOp == OVERLAPS_OP)
	{
		operator = OID_ARRAY_OVERLAP_OP;
		selec = calc_arraycontsel(vardata, PointerGetDatum(values),
				 ti->valuetypid, operator);
		selec *= calc_arraycontsel(vardata, PointerGetDatum(timestamps),
				 TIMESTAMPTZOID, operator);
	}
	else if (cachedOp == CONTAINS_OP)
	{
		operator = OID_ARRAY_CONTAINS_OP;
		selec = calc_arraycontsel(vardata, PointerGetDatum(values),
				 ti->valuetypid, operator);
		selec *= calc_arraycontsel(vardata, PointerGetDatum(timestamps),
				  TIMESTAMPTZOID, operator);
	}
	else if (cachedOp == CONTAINED_OP)
	{
		operator = OID_ARRAY_CONTAINED_OP;
		selec = calc_arraycontsel(vardata, PointerGetDatum(values),
				 ti->valuetypid, operator);
		selec *= calc_arraycontsel(vardata, PointerGetDatum(timestamps),
				  TIMESTAMPTZOID, operator);
	}
	else if (cachedOp == BEFORE_OP || cachedOp == AFTER_OP || 
		cachedOp == OVERBEFORE_OP || cachedOp == OVERAFTER_OP) 
	{
		/* TODO */
		selec = default_tnumber_selectivity(cachedOp);
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
		selec = default_tnumber_selectivity(cachedOp);
	}
	else /* Unknown operator */
	{
		selec = default_tnumber_selectivity(cachedOp);
	}
	pfree(values);
	pfree(timestamps);
	return selec;
}

/*
 * Compute selectivity for columns of durations TemporalSeq, TemporalS,
 * and Temporal (columns containing temporal values of mixed duration).
 */
Selectivity
tnumbers_sel(PlannerInfo *root, VariableStatData *vardata, TBOX *box, 
	CachedOp cachedOp, Oid valuetypid)
{
	Period period;
	RangeType *range;
	TypeCacheEntry *typcache;
	double selec;
	Oid rangetypid, value_oprid, period_oprid;

	/* Enable the multiplication of the selectivity of the value and time 
	 * dimensions since either may be missing */
	selec = 1.0; 

	if (MOBDB_FLAGS_GET_X(box->flags))
	{
		/* Fetch the range operator corresponding to the cachedOp */
		value_oprid = tnumber_cachedop_rangeop(cachedOp);
		/* If the corresponding range operator is not found */
		if (value_oprid == InvalidOid)
			return default_tnumber_selectivity(cachedOp);
		range = range_make(Float8GetDatum(box->xmin), 
			Float8GetDatum(box->xmax), true, true, valuetypid);
		rangetypid = range_oid_from_base(valuetypid);		
		typcache = lookup_type_cache(rangetypid, TYPECACHE_RANGE_INFO);
	}
	if (MOBDB_FLAGS_GET_T(box->flags))
		period_set(&period, box->tmin, box->tmax, true, true);

	/*
	 * There is no ~= operator for range/time types and thus it is necessary to
	 * take care of this operator here.
	 */
	if (cachedOp == SAME_OP)
	{
		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
		{
			value_oprid = oper_oid(EQ_OP, valuetypid, valuetypid);
			selec *= var_eq_const(vardata, value_oprid, RangeTypePGetDatum(range),
				false, false, false);
		}
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
		{
			period_oprid = oper_oid(EQ_OP, T_PERIOD, T_PERIOD);
			selec *= var_eq_const(vardata, period_oprid, PeriodGetDatum(&period), 
				false, false, false);
		}
	}
	else if (cachedOp == OVERLAPS_OP || cachedOp == CONTAINS_OP ||
		cachedOp == CONTAINED_OP || cachedOp == BEFORE_OP ||
		cachedOp == AFTER_OP || cachedOp == OVERBEFORE_OP || 
		cachedOp == OVERAFTER_OP) 
	{
		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
			selec *= calc_hist_selectivity(typcache, vardata, range, 
				value_oprid);
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
			selec *= calc_period_hist_selectivity(vardata, &period, cachedOp);
	}
	else if (cachedOp == LT_OP || cachedOp == LE_OP || 
		cachedOp == GT_OP || cachedOp == GE_OP) 
	{
		/* For b-tree comparisons, temporal values are first compared wrt 
		 * their bounding boxes, and if these are equal, other criteria apply.
		 * For selectivity estimation we approximate by taking into account
		 * only the bounding boxes. In the case here the bounding box is a
		 * period and thus we can use the period selectivity estimation */
		/* Selectivity for the value dimension */
		if (MOBDB_FLAGS_GET_X(box->flags))
			selec *= calc_hist_selectivity(typcache, vardata, range, 
				value_oprid);
		/* Selectivity for the time dimension */
		if (MOBDB_FLAGS_GET_T(box->flags))
			selec *= calc_period_hist_selectivity(vardata, &period, cachedOp);
	}
	else /* Unknown operator */
	{
		selec = default_tnumber_selectivity(cachedOp);
	}
	if (MOBDB_FLAGS_GET_X(box->flags))
		pfree(range);
	return selec;
}

/*****************************************************************************/

/*
 * Estimate the selectivity value of the operators for temporal types whose
 * bounding box is a TBOX, that is, tint and tfloat.
 */
PG_FUNCTION_INFO_V1(tnumber_sel);

PGDLLEXPORT Datum
tnumber_sel(PG_FUNCTION_ARGS)
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
	TBOX constBox;
	Oid valuetypid;

	/*
	 * Get enumeration value associated to the operator
	 */
	bool found = tnumber_cachedop(operator, &cachedOp);
	/* In the case of unknown operator */
	if (!found)
		PG_RETURN_FLOAT8(DEFAULT_TEMP_SELECTIVITY);

	/*
	 * If expression is not (variable op something) or (something op
	 * variable), then punt and return a default estimate.
	 */
	if (!get_restriction_variable(root, args, varRelid,
								  &vardata, &other, &varonleft))
		PG_RETURN_FLOAT8(default_tnumber_selectivity(cachedOp));

	/*
	 * Can't do anything useful if the something is not a constant, either.
	 */
	if (!IsA(other, Const))
	{
		ReleaseVariableStats(vardata);
		PG_RETURN_FLOAT8(default_tnumber_selectivity(cachedOp));
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
			PG_RETURN_FLOAT8(default_tnumber_selectivity(cachedOp));
		}
	}

    /* 
	 * Transform the constant into a TBOX 
	 */
    memset(&constBox, 0, sizeof(TBOX));
    found = tnumber_const_to_tbox(other, &constBox);
    /* In the case of unknown constant */
    if (!found)
		PG_RETURN_FLOAT8(default_tnumber_selectivity(cachedOp));

	assert(MOBDB_FLAGS_GET_X(constBox.flags) || MOBDB_FLAGS_GET_T(constBox.flags));
	
	/* Get the base type and duration of the temporal column */
	valuetypid = base_oid_from_temporal(vardata.atttype);
	numeric_base_type_oid(valuetypid);
	int duration = TYPMOD_GET_DURATION(vardata.atttypmod);
	temporal_duration_all_is_valid(duration);


	/* Dispatch based on duration */
	if (duration == TEMPORALINST)
		selec = tnumberinst_sel(root, &vardata, &constBox, cachedOp, valuetypid);
	else if (duration == TEMPORALI)
		selec = tnumberi_sel(root, &vardata, other, cachedOp);
	else
		selec = tnumbers_sel(root, &vardata, &constBox, cachedOp, valuetypid);

	ReleaseVariableStats(vardata);
	CLAMP_PROBABILITY(selec);
	PG_RETURN_FLOAT8(selec);
}

PG_FUNCTION_INFO_V1(tnumber_joinsel);

PGDLLEXPORT Datum
tnumber_joinsel(PG_FUNCTION_ARGS)
{
	PG_RETURN_FLOAT8(DEFAULT_TEMP_SELECTIVITY);
}

/*****************************************************************************/
