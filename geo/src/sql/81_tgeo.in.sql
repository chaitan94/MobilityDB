/*****************************************************************************
 *
 * tgeo.sql
 *    Basic functions for temporal geometries and geographies.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/
 
CREATE TYPE tgeometry;
CREATE TYPE tgeography;

SELECT register_temporal('tgeometry', 'geometry');
SELECT register_temporal('tgeography', 'geography');

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE FUNCTION tgeometry_in(cstring, oid, integer)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tgeo_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_out(tgeometry)
    RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_recv(internal, oid, integer)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_recv'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_send(tgeometry)
    RETURNS bytea
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION tgeometry_typmod_in(cstring[])
    RETURNS integer
    AS 'MODULE_PATHNAME','tgeometry_typmod_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OR REPLACE FUNCTION tgeo_typmod_out(integer)
    RETURNS cstring
    AS 'MODULE_PATHNAME','tgeo_typmod_out'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeo_analyze(internal) /* TODO */
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'tpoint_analyze'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE tgeometry (
    internallength = variable,
    input = tgeometry_in,
    output = temporal_out,
    send = temporal_send,
    receive = tgeometry_recv,
    typmod_in = tgeometry_typmod_in,
    typmod_out = tgeo_typmod_out,
    storage = extended,
    alignment = double,
    analyze = tgeo_analyze
);

CREATE FUNCTION tgeography_in(cstring, oid, integer)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tgeo_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_out(tgeography)
    RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_recv(internal, oid, integer)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_recv'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_send(tgeography)
    RETURNS bytea
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION tgeography_typmod_in(cstring[])
    RETURNS integer
    AS 'MODULE_PATHNAME','tgeography_typmod_in'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE TYPE tgeography (
    internallength = variable,
    input = tgeography_in,
    output = temporal_out,
    send = temporal_send,
    receive = tgeography_recv,
    typmod_in = tgeography_typmod_in,
    typmod_out = tgeo_typmod_out,
    storage = extended,
    alignment = double,
    analyze = tgeo_analyze
);

-- Special cast for enforcing the typmod restrictions
CREATE OR REPLACE FUNCTION tgeometry(tgeometry, integer)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME','tgeo_enforce_typmod'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OR REPLACE FUNCTION tgeography(tgeography, integer)
    RETURNS tgeography
    AS 'MODULE_PATHNAME','tgeo_enforce_typmod'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- Casting CANNOT be implicit to avoid ambiguity
CREATE CAST (tgeometry AS tgeometry) WITH FUNCTION tgeometry(tgeometry, integer);
CREATE CAST (tgeography AS tgeography) WITH FUNCTION tgeography(tgeography, integer);


/******************************************************************************
 * Constructors
 ******************************************************************************/

CREATE FUNCTION tgeometryinst(geometry, timestamptz)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tgeo_make_temporalinst'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeometryi(tgeometry[])
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_make_temporali'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeometryseq(tgeometry[], lower_inc boolean DEFAULT true, 
    upper_inc boolean DEFAULT true, linear boolean DEFAULT true)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_make_temporalseq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeometrys(tgeometry[])
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_make_temporals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************/

CREATE FUNCTION tgeographyinst(geography, timestamptz)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tgeo_make_temporalinst'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeographyi(tgeography[])
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_make_temporali'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeographyseq(tgeography[], lower_inc boolean DEFAULT true, 
    upper_inc boolean DEFAULT true, linear boolean DEFAULT true)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_make_temporalseq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeographys(tgeography[])
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_make_temporals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Transformations
 ******************************************************************************/

CREATE FUNCTION tgeometryinst(tgeometry)
    RETURNS tgeometry AS 'MODULE_PATHNAME', 'temporal_to_temporalinst'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometryi(tgeometry)
    RETURNS tgeometry AS 'MODULE_PATHNAME', 'temporal_to_temporali'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometryseq(tgeometry)
    RETURNS tgeometry AS 'MODULE_PATHNAME', 'temporal_to_temporalseq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometrys(tgeometry)
    RETURNS tgeometry AS 'MODULE_PATHNAME', 'temporal_to_temporals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION tgeographyinst(tgeography)
    RETURNS tgeography AS 'MODULE_PATHNAME', 'temporal_to_temporalinst'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeographyi(tgeography)
    RETURNS tgeography AS 'MODULE_PATHNAME', 'temporal_to_temporali'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeographyseq(tgeography)
    RETURNS tgeography AS 'MODULE_PATHNAME', 'temporal_to_temporalseq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeographys(tgeography)
    RETURNS tgeography AS 'MODULE_PATHNAME', 'temporal_to_temporals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION toLinear(tgeometry)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tstepw_to_linear'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION toLinear(tgeography)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tstepw_to_linear'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Append function
 ******************************************************************************/

CREATE FUNCTION appendInstant(tgeometry, tgeometry)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_append_instant'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION appendInstant(tgeography, tgeography)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_append_instant'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Casting
 ******************************************************************************/

CREATE FUNCTION period(tgeometry)
    RETURNS period
    AS 'MODULE_PATHNAME', 'temporal_to_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION period(tgeography)
    RETURNS period
    AS 'MODULE_PATHNAME', 'temporal_to_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- Casting CANNOT be implicit to avoid ambiguity
CREATE CAST (tgeometry AS period) WITH FUNCTION period(tgeometry);
CREATE CAST (tgeography AS period) WITH FUNCTION period(tgeography);

/******************************************************************************
 * Functions
 ******************************************************************************/

CREATE FUNCTION duration(tgeometry)
    RETURNS text
    AS 'MODULE_PATHNAME', 'temporal_duration'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION duration(tgeography)
    RETURNS text
    AS 'MODULE_PATHNAME', 'temporal_duration'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION memSize(tgeometry)
    RETURNS int
    AS 'MODULE_PATHNAME', 'temporal_mem_size'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION memSize(tgeography)
    RETURNS int
    AS 'MODULE_PATHNAME', 'temporal_mem_size'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- value is a reserved word in SQL
CREATE FUNCTION getValue(tgeometry)
    RETURNS geometry
    AS 'MODULE_PATHNAME', 'temporalinst_get_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION getValue(tgeography)
    RETURNS geography
    AS 'MODULE_PATHNAME', 'temporalinst_get_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/* TODO */

/*CREATE FUNCTION getValues(tgeometry)
    RETURNS geometry
    AS 'MODULE_PATHNAME', 'tgeo_values'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION getValues(tgeography)
    RETURNS geography
    AS 'MODULE_PATHNAME', 'tgeo_values'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;*/

-- time is a reserved word in SQL
CREATE FUNCTION getTime(tgeometry)
    RETURNS periodset
    AS 'MODULE_PATHNAME', 'temporal_get_time'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION getTime(tgeography)
    RETURNS periodset
    AS 'MODULE_PATHNAME', 'temporal_get_time'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION getTimestamp(tgeometry)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporalinst_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION getTimestamp(tgeography)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporalinst_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/* TODO */

/*CREATE FUNCTION ever_equals(tgeometry, geometry(Point))
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'tgeo_ever_equals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION ever_equals(tgeography, geography(Point))
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'tgeo_ever_equals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR &= (
    LEFTARG = tgeometry, RIGHTARG = geometry(Point),
    PROCEDURE = ever_equals,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR &= (
    LEFTARG = tgeography, RIGHTARG = geography(Point),
    PROCEDURE = ever_equals,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);*/

/* TODO */

/*CREATE FUNCTION always_equals(tgeometry, geometry(Point))
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'tgeo_always_equals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION always_equals(tgeography, geography(Point))
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'tgeo_always_equals'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @= (
    LEFTARG = tgeometry, RIGHTARG = geometry(Point),
    PROCEDURE = always_equals,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR @= (
    LEFTARG = tgeography, RIGHTARG = geography(Point),
    PROCEDURE = always_equals,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);*/

CREATE FUNCTION shift(tgeometry, interval)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_shift'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION shift(tgeography, interval)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_shift'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startValue(tgeometry)
    RETURNS geometry
    AS 'MODULE_PATHNAME', 'temporal_start_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION startValue(tgeography)
    RETURNS geography
    AS 'MODULE_PATHNAME', 'temporal_start_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endValue(tgeometry)
    RETURNS geometry
    AS 'MODULE_PATHNAME', 'temporal_end_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION endValue(tgeography)
    RETURNS geography
    AS 'MODULE_PATHNAME', 'temporal_end_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION timespan(tgeometry)
    RETURNS interval
    AS 'MODULE_PATHNAME', 'temporal_timespan'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION timespan(tgeography)
    RETURNS interval
    AS 'MODULE_PATHNAME', 'temporal_timespan'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION numInstants(tgeometry)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_num_instants'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION numInstants(tgeography)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_num_instants'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startInstant(tgeometry)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_start_instant'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION startInstant(tgeography)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_start_instant'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endInstant(tgeometry)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_end_instant'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION endInstant(tgeography)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_end_instant'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION instantN(tgeometry, integer)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_instant_n'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION instantN(tgeography, integer)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_instant_n'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION instants(tgeometry)
    RETURNS tgeometry[]
    AS 'MODULE_PATHNAME', 'temporal_instants'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION instants(tgeography)
    RETURNS tgeography[]
    AS 'MODULE_PATHNAME', 'temporal_instants'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION numTimestamps(tgeometry)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_num_timestamps'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION numTimestamps(tgeography)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_num_timestamps'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startTimestamp(tgeometry)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporal_start_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION startTimestamp(tgeography)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporal_start_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endTimestamp(tgeometry)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporal_end_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION endTimestamp(tgeography)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporal_end_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION timestampN(tgeometry, integer)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporal_timestamp_n'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION timestampN(tgeography, integer)
    RETURNS timestamptz
    AS 'MODULE_PATHNAME', 'temporal_timestamp_n'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION timestamps(tgeometry)
    RETURNS timestamptz[]
    AS 'MODULE_PATHNAME', 'temporal_timestamps'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION timestamps(tgeography)
    RETURNS timestamptz[]
    AS 'MODULE_PATHNAME', 'temporal_timestamps'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION numSequences(tgeometry)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_num_sequences'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION numSequences(tgeography)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_num_sequences'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION startSequence(tgeometry)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_start_sequence'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION startSequence(tgeography)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_start_sequence'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION endSequence(tgeometry)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_end_sequence'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION endSequence(tgeography)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_end_sequence'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION sequenceN(tgeometry, integer)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_sequence_n'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION sequenceN(tgeography, integer)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_sequence_n'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION sequences(tgeometry)
    RETURNS tgeometry[]
    AS 'MODULE_PATHNAME', 'temporal_sequences'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION sequences(tgeography)
    RETURNS tgeography[]
    AS 'MODULE_PATHNAME', 'temporal_sequences'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/* TODO */

/*CREATE FUNCTION atValue(tgeometry, geometry(Point))
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tgeo_at_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION atValue(tgeography, geography(Point))
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tgeo_at_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusValue(tgeometry, geometry(Point))
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tgeo_minus_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION minusValue(tgeography, geography(Point))
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tgeo_minus_value'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atValues(tgeometry, geometry(Point)[])
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tgeo_at_values'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION atValues(tgeography, geography(Point)[])
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tgeo_at_values'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusValues(tgeometry, geometry(Point)[])
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'tgeo_minus_values'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION minusValues(tgeography, geography(Point)[])
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'tgeo_minus_values'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;*/

/*CREATE FUNCTION atTimestamp(tgeometry, timestamptz)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_at_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION atTimestamp(tgeography, timestamptz)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_at_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusTimestamp(tgeometry, timestamptz)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_minus_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION minusTimestamp(tgeography, timestamptz)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_minus_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION valueAtTimestamp(tgeometry, timestamptz)
    RETURNS geometry
    AS 'MODULE_PATHNAME', 'temporal_value_at_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION valueAtTimestamp(tgeography, timestamptz)
    RETURNS geography
    AS 'MODULE_PATHNAME', 'temporal_value_at_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atTimestampSet(tgeometry, timestampset)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_at_timestampset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION atTimestampSet(tgeography, timestampset)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_at_timestampset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusTimestampSet(tgeometry, timestampset)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_minus_timestampset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION minusTimestampSet(tgeography, timestampset)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_minus_timestampset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atPeriod(tgeometry, period)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_at_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION atPeriod(tgeography, period)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_at_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusPeriod(tgeometry, period)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_minus_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION minusPeriod(tgeography, period)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_minus_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION atPeriodSet(tgeometry, periodset)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_at_periodset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION atPeriodSet(tgeography, periodset)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_at_periodset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION minusPeriodSet(tgeometry, periodset)
    RETURNS tgeometry
    AS 'MODULE_PATHNAME', 'temporal_minus_periodset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION minusPeriodSet(tgeography, periodset)
    RETURNS tgeography
    AS 'MODULE_PATHNAME', 'temporal_minus_periodset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;*/

CREATE FUNCTION intersectsTimestamp(tgeometry, timestamptz)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION intersectsTimestamp(tgeography, timestamptz)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_timestamp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    
CREATE FUNCTION intersectsTimestampSet(tgeometry, timestampset)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_timestampset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION intersectsTimestampSet(tgeography, timestampset)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_timestampset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION intersectsPeriod(tgeometry, period)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION intersectsPeriod(tgeography, period)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_period'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION intersectsPeriodSet(tgeometry, periodset)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_periodset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION intersectsPeriodSet(tgeography, periodset)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'temporal_intersects_periodset'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Comparison functions and B-tree indexing
 ******************************************************************************/

CREATE FUNCTION tgeometry_lt(tgeometry, tgeometry)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_lt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_le(tgeometry, tgeometry)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_le'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_eq(tgeometry, tgeometry)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_eq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_ne(tgeometry, tgeometry)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_ne'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_ge(tgeometry, tgeometry)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_ge'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_gt(tgeometry, tgeometry)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_gt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeometry_cmp(tgeometry, tgeometry)
    RETURNS int4
    AS 'MODULE_PATHNAME', 'temporal_cmp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR < (
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    PROCEDURE = tgeometry_lt,
    COMMUTATOR = >,
    NEGATOR = >=,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR <= (
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    PROCEDURE = tgeometry_le,
    COMMUTATOR = >=,
    NEGATOR = >,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR = (
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    PROCEDURE = tgeometry_eq,
    COMMUTATOR = =,
    NEGATOR = <>,
    RESTRICT = eqsel, JOIN = eqjoinsel
);
CREATE OPERATOR <> (
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    PROCEDURE = tgeometry_ne,
    COMMUTATOR = <>,
    NEGATOR = =,
    RESTRICT = neqsel, JOIN = neqjoinsel
);
CREATE OPERATOR >= (
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    PROCEDURE = tgeometry_ge,
    COMMUTATOR = <=,
    NEGATOR = <,
    RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);
CREATE OPERATOR > (
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    PROCEDURE = tgeometry_gt,
    COMMUTATOR = <,
    NEGATOR = <=,
    RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS tgeometry_ops
    DEFAULT FOR TYPE tgeometry USING btree AS
        OPERATOR    1   <,
        OPERATOR    2   <=,
        OPERATOR    3   =,
        OPERATOR    4   >=,
        OPERATOR    5   >,
        FUNCTION    1   tgeometry_cmp(tgeometry, tgeometry);

/******************************************************************************/

CREATE FUNCTION tgeography_lt(tgeography, tgeography)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_lt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_le(tgeography, tgeography)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_le'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_eq(tgeography, tgeography)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_eq'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_ne(tgeography, tgeography)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_ne'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_ge(tgeography, tgeography)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_ge'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_gt(tgeography, tgeography)
    RETURNS bool
    AS 'MODULE_PATHNAME', 'temporal_gt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_cmp(tgeography, tgeography)
    RETURNS int4
    AS 'MODULE_PATHNAME', 'temporal_cmp'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR < (
    LEFTARG = tgeography, RIGHTARG = tgeography,
    PROCEDURE = tgeography_lt,
    COMMUTATOR = >, NEGATOR = >=,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR <= (
    LEFTARG = tgeography, RIGHTARG = tgeography,
    PROCEDURE = tgeography_le,
    COMMUTATOR = >=, NEGATOR = >,
    RESTRICT = scalarltsel, JOIN = scalarltjoinsel
);
CREATE OPERATOR = (
    LEFTARG = tgeography, RIGHTARG = tgeography,
    PROCEDURE = tgeography_eq,
    COMMUTATOR = =, NEGATOR = <>,
    RESTRICT = eqsel, JOIN = eqjoinsel
);
CREATE OPERATOR <> (
    LEFTARG = tgeography, RIGHTARG = tgeography,
    PROCEDURE = tgeography_ne,
    COMMUTATOR = <>, NEGATOR = =,
    RESTRICT = neqsel, JOIN = neqjoinsel
);
CREATE OPERATOR >= (
    LEFTARG = tgeography, RIGHTARG = tgeography,
    PROCEDURE = tgeography_ge,
    COMMUTATOR = <=, NEGATOR = <,
    RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);
CREATE OPERATOR > (
    LEFTARG = tgeography, RIGHTARG = tgeography,
    PROCEDURE = tgeography_gt,
    COMMUTATOR = <, NEGATOR = <=,
    RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS tgeography_ops
    DEFAULT FOR TYPE tgeography USING btree AS
        OPERATOR    1   <,
        OPERATOR    2   <=,
        OPERATOR    3   =,
        OPERATOR    4   >=,
        OPERATOR    5   >,
        FUNCTION    1   tgeography_cmp(tgeography, tgeography);

/******************************************************************************/

CREATE FUNCTION tgeometry_hash(tgeometry)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_hash'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeography_hash(tgeography)
    RETURNS integer
    AS 'MODULE_PATHNAME', 'temporal_hash'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR CLASS hash_tgeometry_ops
    DEFAULT FOR TYPE tgeometry USING hash AS
    OPERATOR    1   = ,
    FUNCTION    1   tgeometry_hash(tgeometry);
CREATE OPERATOR CLASS hash_tgeography_ops
    DEFAULT FOR TYPE tgeography USING hash AS
    OPERATOR    1   = ,
    FUNCTION    1   tgeography_hash(tgeography);

/******************************************************************************/

