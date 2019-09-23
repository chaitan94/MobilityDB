/*****************************************************************************
 *
 * tarray.sql
 *	  Basic functions for generic temporal array types.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE TYPE tarray;

SELECT register_temporal('tarray', 'anyarray') ;

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE FUNCTION tarray_in(cstring, oid, integer)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_in'
	LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION temporal_out(tarray)
	RETURNS cstring
	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION tarray_recv(internal, oid, integer)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_recv'
	LANGUAGE C IMMUTABLE STRICT;
CREATE FUNCTION temporal_send(tarray)
	RETURNS bytea
	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE tarray (
	internallength = variable,
	input = tarray_in,
	output = temporal_out,
	receive = tarray_recv,
	send = temporal_send,
	alignment = double
);

-- Special cast for enforcing the typmod restrictions
CREATE FUNCTION tarray(tarray, integer)
	RETURNS tarray
	AS 'MODULE_PATHNAME','temporal_enforce_typmod'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (tarray AS tarray) WITH FUNCTION tarray(tarray, integer) AS IMPLICIT;

/******************************************************************************
 * Constructors
 ******************************************************************************/

/* Temporal instant */

CREATE FUNCTION tarrayinst(val anyarray, t timestamptz)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_make_temporalinst'
	LANGUAGE C IMMUTABLE STRICT;

/* Temporal instant set */

CREATE FUNCTION tarrayi(tarray[])
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_make_temporali'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/* Temporal sequence */

CREATE FUNCTION tarrayseq(tarray[], lower_inc boolean DEFAULT true, 
	upper_inc boolean DEFAULT true)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_make_temporalseq'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/* Temporal sequence set */
	
CREATE FUNCTION tarrays(tarray[])
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_make_temporals'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Append function
 ******************************************************************************/

CREATE FUNCTION appendInstant(tarray, tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_append_instant'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Cast functions
 ******************************************************************************/


/******************************************************************************
 * Transformation functions
 ******************************************************************************/

CREATE FUNCTION tarrayinst(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_as_temporalinst'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarrayi(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_as_temporali'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarrayseq(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_as_temporalseq'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarrays(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_as_temporals'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/******************************************************************************
 * Accessor functions
 ******************************************************************************/

CREATE FUNCTION temporalType(tarray)
	RETURNS text
	AS 'MODULE_PATHNAME', 'temporal_type'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
 
CREATE FUNCTION memSize(tarray)
	RETURNS int
	AS 'MODULE_PATHNAME', 'temporal_mem_size'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/*
CREATE FUNCTION period(tbool)
	RETURNS period
	AS 'MODULE_PATHNAME', 'temporal_timespan'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION box(tint)
	RETURNS tbox
	AS 'MODULE_PATHNAME', 'temporal_tbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION box(tfloat)
	RETURNS tbox
	AS 'MODULE_PATHNAME', 'temporal_tbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION period(ttext)
	RETURNS period
	AS 'MODULE_PATHNAME', 'temporal_timespan'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
*/

-- value is a reserved word in SQL
CREATE FUNCTION getValue(tarray)
	RETURNS anyarray
	AS 'MODULE_PATHNAME', 'temporalinst_get_value'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- values is a reserved word in SQL
CREATE FUNCTION getValues(tarray)
	RETURNS anyarray
	AS 'MODULE_PATHNAME', 'tempdisc_get_values'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startValue(tarray)
	RETURNS anyarray
	AS 'MODULE_PATHNAME', 'temporal_start_value'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endValue(tarray)
	RETURNS anyarray
	AS 'MODULE_PATHNAME', 'temporal_end_value'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- timestamp is a reserved word in SQL
CREATE FUNCTION getTimestamp(tarray)
	RETURNS timestamptz
	AS 'MODULE_PATHNAME', 'temporalinst_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- time is a reserved word in SQL
CREATE FUNCTION getTime(tarray)
	RETURNS periodset
	AS 'MODULE_PATHNAME', 'temporal_get_time'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION timespan(tarray)
	RETURNS period
	AS 'MODULE_PATHNAME', 'temporal_timespan'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION duration(tarray)
	RETURNS interval
	AS 'MODULE_PATHNAME', 'temporal_duration'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION numSequences(tarray)
	RETURNS integer
	AS 'MODULE_PATHNAME', 'temporal_num_sequences'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startSequence(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_start_sequence'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endSequence(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_end_sequence'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION sequenceN(tarray, integer)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_sequence_n'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION sequences(tarray)
	RETURNS tarray[]
	AS 'MODULE_PATHNAME', 'temporal_sequences'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION numInstants(tarray)
	RETURNS integer
	AS 'MODULE_PATHNAME', 'temporal_num_instants'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startInstant(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_start_instant'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endInstant(tarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_end_instant'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION instantN(tarray, integer)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_instant_n'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION instants(tarray)
	RETURNS tarray[]
	AS 'MODULE_PATHNAME', 'temporal_instants'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION numTimestamps(tarray)
	RETURNS integer
	AS 'MODULE_PATHNAME', 'temporal_num_timestamps'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startTimestamp(tarray)
	RETURNS timestamptz
	AS 'MODULE_PATHNAME', 'temporal_start_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endTimestamp(tarray)
	RETURNS timestamptz
	AS 'MODULE_PATHNAME', 'temporal_end_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION timestampN(tarray, integer)
	RETURNS timestamptz
	AS 'MODULE_PATHNAME', 'temporal_timestamp_n'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION timestamps(tarray)
	RETURNS timestamptz[]
	AS 'MODULE_PATHNAME', 'temporal_timestamps'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION ever_equals(tarray, anyarray)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'temporal_ever_equals'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR &= (
	LEFTARG = tarray, RIGHTARG = anyarray,
	PROCEDURE = ever_equals,
	RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);

CREATE FUNCTION always_equals(tarray, anyarray)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'temporal_always_equals'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @= (
	LEFTARG = tarray, RIGHTARG = anyarray,
	PROCEDURE = always_equals,
	RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);

CREATE FUNCTION shift(tarray, interval)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_shift'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION isContinuousInTime(tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporals_continuous_time'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-------------------------------------------------------------------------------
-- Restriction functions
-------------------------------------------------------------------------------

CREATE FUNCTION atValue(tarray, anyarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_at_value'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusValue(tarray, anyarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_minus_value'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atValues(tarray, anyarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_at_values'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusValues(tarray, anyarray)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_minus_values'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atTimestamp(tarray, timestamptz)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_at_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusTimestamp(tarray, timestamptz)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_minus_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION valueAtTimestamp(tarray, timestamptz)
	RETURNS anyarray
	AS 'MODULE_PATHNAME', 'temporal_value_at_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atTimestampSet(tarray, timestampset)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_at_timestampset'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusTimestampSet(tarray, timestampset)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_minus_timestampset'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atPeriod(tarray, period)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_at_period'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusPeriod(tarray, period)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_minus_period'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atPeriodSet(tarray, periodset)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_at_periodset'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusPeriodSet(tarray, periodset)
	RETURNS tarray
	AS 'MODULE_PATHNAME', 'temporal_minus_periodset'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION intersectsTimestamp(tarray, timestamptz)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'temporal_intersects_timestamp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION intersectsTimestampSet(tarray, timestampset)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'temporal_intersects_timestampset'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION intersectsPeriod(tarray, period)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'temporal_intersects_period'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION intersectsPeriodSet(tarray, periodset)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'temporal_intersects_periodset'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Comparison functions and B-tree indexing
 ******************************************************************************/

CREATE FUNCTION tarray_lt(tarray, tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporal_lt'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarray_le(tarray, tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporal_le'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarray_eq(tarray, tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporal_eq'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarray_ne(tarray, tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporal_ne'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarray_ge(tarray, tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporal_ge'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarray_gt(tarray, tarray)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'temporal_gt'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tarray_cmp(tarray, tarray)
	RETURNS integer
	AS 'MODULE_PATHNAME', 'temporal_cmp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR < (
	LEFTARG = tarray, RIGHTARG = tarray,
	PROCEDURE = tarray_lt,
	COMMUTATOR = >,
	NEGATOR = >=,
	RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR <= (
	LEFTARG = tarray, RIGHTARG = tarray,
	PROCEDURE = tarray_le,
	COMMUTATOR = >=,
	NEGATOR = >,
	RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR = (
	LEFTARG = tarray, RIGHTARG = tarray,
	PROCEDURE = tarray_eq,
	COMMUTATOR = =,
	NEGATOR = <>,
	RESTRICT = eqsel, JOIN = eqjoinsel
);
CREATE OPERATOR <> (
	LEFTARG = tarray, RIGHTARG = tarray,
	PROCEDURE = tarray_ne,
	COMMUTATOR = <>,
	NEGATOR = =,
	RESTRICT = neqsel, JOIN = neqjoinsel
);
CREATE OPERATOR >= (
	LEFTARG = tarray, RIGHTARG = tarray,
	PROCEDURE = tarray_ge,
	COMMUTATOR = <=,
	NEGATOR = <,
	RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);
CREATE OPERATOR > (
	LEFTARG = tarray, RIGHTARG = tarray,
	PROCEDURE = tarray_gt,
	COMMUTATOR = <,
	NEGATOR = <=,
	RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS tarray_ops
	DEFAULT FOR TYPE tarray USING btree AS
		OPERATOR	1	<,
		OPERATOR	2	<=,
		OPERATOR	3	=,
		OPERATOR	4	>=,
		OPERATOR	5	>,
		FUNCTION	1	tarray_cmp(tarray, tarray);

/******************************************************************************/
/* hash */

/******************************************************************************/
