/*****************************************************************************
 *
 * StaticObjects.sql
 *	  Network-based static point/region
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE TYPE npoint;
CREATE TYPE nsegment;
CREATE TYPE nregion;

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE FUNCTION npoint_in(cstring)
	RETURNS npoint
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION npoint_out(npoint)
	RETURNS cstring
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION npoint_recv(internal)
	RETURNS npoint
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION npoint_send(npoint)
	RETURNS bytea
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE npoint (
	internallength = 16,
	input = npoint_in,
	output = npoint_out,
	receive = npoint_recv,
	send = npoint_send,
	alignment = double
);

CREATE FUNCTION nsegment_in(cstring)
	RETURNS nsegment
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nsegment_out(nsegment)
	RETURNS cstring
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nsegment_recv(internal)
	RETURNS nsegment
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nsegment_send(nsegment)
	RETURNS bytea
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE nsegment (
	internallength = 24,
	input = nsegment_in,
	output = nsegment_out,
	receive = nsegment_recv,
	send = nsegment_send,
	alignment = double
);

CREATE FUNCTION nregion_in(cstring)
	RETURNS nregion
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nregion_out(nregion)
	RETURNS cstring
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nregion_recv(internal)
	RETURNS nregion
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION nregion_send(nregion)
	RETURNS bytea
 	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE nregion (
	internallength = variable,
	input = nregion_in,
	output = nregion_out,
	receive = nregion_recv,
	send = nregion_send,
	storage = extended,
	alignment = double
);

/******************************************************************************
 * Constructors
 ******************************************************************************/

CREATE FUNCTION npoint(bigint, double precision)
	RETURNS npoint
	AS 'MODULE_PATHNAME', 'npoint_constructor'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION nsegment(bigint, double precision DEFAULT 0, double precision DEFAULT 1)
	RETURNS nsegment
	AS 'MODULE_PATHNAME', 'nsegment_constructor'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION nsegment(npoint)
	RETURNS nsegment
	AS 'MODULE_PATHNAME', 'nsegment_from_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION nregion(nsegment)
	RETURNS nregion
	AS 'MODULE_PATHNAME', 'nregion_from_nsegment'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
CREATE FUNCTION nregion(nsegment[])
	RETURNS nregion
	AS 'MODULE_PATHNAME', 'nregion_from_nsegmentarr'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * Accessing values
 *****************************************************************************/

CREATE FUNCTION route(npoint)
	RETURNS bigint
	AS 'MODULE_PATHNAME', 'npoint_route'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION pos(npoint)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'npoint_pos'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION route(nsegment)
	RETURNS bigint
	AS 'MODULE_PATHNAME', 'nsegment_route'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION startPosition(nsegment)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'nsegment_start_pos'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION endPosition(nsegment)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'nsegment_end_pos'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION segments(nregion)
	RETURNS nsegment[]
	AS 'MODULE_PATHNAME', 'nregion_segments'
	LANGUAGE C IMMUTABLE STRICT;

/*****************************************************************************
 * Conversions between network and space
 *****************************************************************************/
	
CREATE FUNCTION in_space(npoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'npoint_geom'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION in_space(nregion)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'nregion_geom'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE OR REPLACE FUNCTION point_in_network(p geometry(point))
RETURNS npoint AS $$
DECLARE
     np npoint;
BEGIN
     SELECT npoint(gid, ST_LineLocatePoint(the_geom, p))
     FROM ways
     WHERE ST_DWithin(the_geom, p, 1e-5)
     ORDER BY ST_Distance(the_geom, p)
     LIMIT 1
     INTO np;
     
     RETURN np;
END; 
$$ LANGUAGE plpgsql;


CREATE OR REPLACE FUNCTION geometry_in_network(geo geometry)
RETURNS nregion AS $$
DECLARE
     nreg nregion;
BEGIN
     WITH route_intersection_tbl AS (
          -- Find intersections of geo and network
          -- Each intersection is either a point or a linestring
          SELECT gid, the_geom, (ST_Dump(ST_Intersection(the_geom, geo))).geom AS intersection
          FROM ways 
     ),   
          nregion_tbl AS (
		  -- Linear reference for point
          SELECT nregion(gid, ST_LineLocatePoint(the_geom, intersection)) AS region
          FROM route_intersection_tbl
          WHERE ST_GeometryType(intersection) = 'ST_Point'
          UNION ALL
		  -- Linear reference for linestring
          SELECT nregion(gid, 
		                 ST_LineLocatePoint(the_geom, ST_StartPoint(intersection)),
				         ST_LineLocatePoint(the_geom, ST_EndPoint(intersection)))
				 AS region
          FROM route_intersection_tbl
          WHERE ST_GeometryType(intersection) = 'ST_LineString' 
     )	  
     SELECT nregion_agg(region)
     FROM nregion_tbl
     INTO nreg;
	 
     RETURN nreg;
END; 
$$ LANGUAGE plpgsql;
	
/******************************************************************************/
	