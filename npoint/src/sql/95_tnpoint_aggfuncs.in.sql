/*****************************************************************************
 *
 * tnpoint_aggfuncs.sql
 *	  Aggregate functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 *	  Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE FUNCTION tcount_transfn(internal, tnpoint)
	RETURNS internal
	AS 'MODULE_PATHNAME', 'temporal_tcount_transfn'
	LANGUAGE C IMMUTABLE PARALLEL SAFE;

CREATE AGGREGATE tcount(tnpoint) (
	SFUNC = tcount_transfn,
	STYPE = internal,
	COMBINEFUNC = tcount_combinefn,
	FINALFUNC = tint_tagg_finalfn,
	SERIALFUNC = tagg_serialize,
	DESERIALFUNC = tagg_deserialize,
	PARALLEL = SAFE
);

CREATE FUNCTION wcount_transfn(internal, tnpoint, interval)
	RETURNS internal
	AS 'MODULE_PATHNAME', 'temporal_wcount_transfn'
	LANGUAGE C IMMUTABLE PARALLEL SAFE;

CREATE AGGREGATE wcount(tnpoint, interval) (
	SFUNC = wcount_transfn,
	STYPE = internal,
	COMBINEFUNC = tint_tsum_combinefn,
	FINALFUNC = tint_tagg_finalfn,
	SERIALFUNC = tagg_serialize,
	DESERIALFUNC = tagg_deserialize,
	PARALLEL = SAFE
);

CREATE FUNCTION tcentroid_transfn(internal, tnpoint)
	RETURNS internal
	AS 'MODULE_PATHNAME', 'tnpoint_tcentroid_transfn'
	LANGUAGE C IMMUTABLE PARALLEL SAFE;

CREATE AGGREGATE tcentroid(tnpoint) (
	SFUNC = tcentroid_transfn,
	STYPE = internal,
	COMBINEFUNC = tcentroid_combinefn,
	FINALFUNC = tcentroid_finalfn,
	SERIALFUNC = tagg_serialize,
	DESERIALFUNC = tagg_deserialize,
	PARALLEL = SAFE
);

/*****************************************************************************/
