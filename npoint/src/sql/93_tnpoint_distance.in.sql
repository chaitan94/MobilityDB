/*****************************************************************************
 *
 * tnpoint_distance.sql
 *	  Temporal distance for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE FUNCTION temporal_distance(geometry, tnpoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'distance_geo_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_distance(npoint, tnpoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'distance_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_distance(tnpoint, geometry)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'distance_tnpoint_geo'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_distance(tnpoint, npoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'distance_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION temporal_distance(tnpoint, tnpoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'distance_tnpoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <-> (
	PROCEDURE = temporal_distance,
	LEFTARG = geometry,
	RIGHTARG = tnpoint,
	COMMUTATOR = <->
);
CREATE OPERATOR <-> (
	PROCEDURE = temporal_distance,
	LEFTARG = npoint,
	RIGHTARG = tnpoint,
	COMMUTATOR = <->
);
CREATE OPERATOR <-> (
	PROCEDURE = temporal_distance,
	LEFTARG = tnpoint,
	RIGHTARG = geometry,
	COMMUTATOR = <->
);
CREATE OPERATOR <-> (
	PROCEDURE = temporal_distance,
	LEFTARG = tnpoint,
	RIGHTARG = npoint,
	COMMUTATOR = <->
);
CREATE OPERATOR <-> (
	PROCEDURE = temporal_distance,
	LEFTARG = tnpoint,
	RIGHTARG = tnpoint,
	COMMUTATOR = <->
);
	
/*****************************************************************************/
