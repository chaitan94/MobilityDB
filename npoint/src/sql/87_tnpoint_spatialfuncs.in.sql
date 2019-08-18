/*****************************************************************************
 *
 * tnpoint_spatialfuncs.sql
 *	  Geometric functions for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

/*****************************************************************************
 * Trajectory
 *****************************************************************************/

CREATE FUNCTION trajectory(tnpoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'tnpoint_trajectory'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * AtGeometry
 *****************************************************************************/	
	
CREATE FUNCTION atGeometry(tnpoint, geometry)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'tnpoint_at_geometry'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * MinusGeometry
 *****************************************************************************/	
	
CREATE FUNCTION minusGeometry(tnpoint, geometry)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'tnpoint_minus_geometry'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * Length
 *****************************************************************************/	
	
CREATE FUNCTION length(tnpoint)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'tnpoint_length'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/*****************************************************************************
 * Cumulative length
 *****************************************************************************/	
		
CREATE FUNCTION cumulativeLength(tnpoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'tnpoint_cumulative_length'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/*****************************************************************************
 * Speed
 *****************************************************************************/	

CREATE FUNCTION speed(tnpoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'tnpoint_speed'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;	
	
/*****************************************************************************
 * Time-weighted centroid
 *****************************************************************************/	

CREATE FUNCTION twCentroid(tnpoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'tnpoint_twcentroid'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/*****************************************************************************
 * Temporal azimuth
 *****************************************************************************/	

CREATE FUNCTION azimuth(tnpoint)
	RETURNS tfloat
	AS 'MODULE_PATHNAME', 'tnpoint_azimuth'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/*****************************************************************************
 * Nearest approach instant
 *****************************************************************************/	

CREATE FUNCTION NearestApproachInstant(geometry, tnpoint)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'NAI_geometry_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION NearestApproachInstant(tnpoint, geometry)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'NAI_tnpoint_geometry'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION NearestApproachInstant(npoint, tnpoint)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'NAI_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION NearestApproachInstant(tnpoint, npoint)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'NAI_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION NearestApproachInstant(tnpoint, tnpoint)
	RETURNS tnpoint
	AS 'MODULE_PATHNAME', 'NAI_tnpoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * Nearest approach distance
 *****************************************************************************/	

CREATE FUNCTION nearestApproachDistance(geometry, tnpoint)
	RETURNS float
	AS 'MODULE_PATHNAME', 'NAD_geometry_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION nearestApproachDistance(tnpoint, geometry)
	RETURNS float
	AS 'MODULE_PATHNAME', 'NAD_tnpoint_geometry'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION NearestApproachDistance(npoint, tnpoint)
	RETURNS float
	AS 'MODULE_PATHNAME', 'NAD_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION NearestApproachDistance(tnpoint, npoint)
	RETURNS float
	AS 'MODULE_PATHNAME', 'NAD_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION nearestApproachDistance(tnpoint, tnpoint)
	RETURNS float
	AS 'MODULE_PATHNAME', 'NAD_tnpoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR |=| (
	LEFTARG = geometry, RIGHTARG = tnpoint,
	PROCEDURE = nearestApproachDistance,
	COMMUTATOR = '|=|'
);
CREATE OPERATOR |=| (
	LEFTARG = tnpoint, RIGHTARG = geometry,
	PROCEDURE = nearestApproachDistance,
	COMMUTATOR = '|=|'
);
CREATE OPERATOR |=| (
	LEFTARG = npoint, RIGHTARG = tnpoint,
	PROCEDURE = nearestApproachDistance,
	COMMUTATOR = '|=|'
);
CREATE OPERATOR |=| (
	LEFTARG = tnpoint, RIGHTARG = npoint,
	PROCEDURE = nearestApproachDistance,
	COMMUTATOR = '|=|'
);
CREATE OPERATOR |=| (
	LEFTARG = tnpoint, RIGHTARG = tnpoint,
	PROCEDURE = nearestApproachDistance,
	COMMUTATOR = '|=|'
);

/*****************************************************************************
 * Shortest line
 *****************************************************************************/	

CREATE FUNCTION shortestLine(geometry, tnpoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'shortestline_geometry_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION shortestLine(tnpoint, geometry)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'shortestline_tnpoint_geometry'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION shortestLine(npoint, tnpoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'shortestline_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION shortestLine(tnpoint, npoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'shortestline_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION shortestLine(tnpoint, tnpoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'shortestline_tnpoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************/
	
