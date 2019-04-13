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

/*****************************************************************************
 * Accessing values
 *****************************************************************************/

CREATE FUNCTION route(npoint)
	RETURNS bigint
	AS 'MODULE_PATHNAME', 'npoint_route'
	LANGUAGE C IMMUTABLE STRICT;

-- position is a reserved word in SQL
CREATE FUNCTION getPosition(npoint)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'npoint_position'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION route(nsegment)
	RETURNS bigint
	AS 'MODULE_PATHNAME', 'nsegment_route'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION startPosition(nsegment)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'nsegment_start_position'
	LANGUAGE C IMMUTABLE STRICT;
	
CREATE FUNCTION endPosition(nsegment)
	RETURNS double precision
	AS 'MODULE_PATHNAME', 'nsegment_end_position'
	LANGUAGE C IMMUTABLE STRICT;
	
/*****************************************************************************
 * Conversions between network and space
 *****************************************************************************/
	
CREATE FUNCTION geometry(npoint)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'npoint_as_geom'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION npoint(geometry)
	RETURNS npoint
	AS 'MODULE_PATHNAME', 'geom_as_npoint'
	LANGUAGE C IMMUTABLE STRICT;

CREATE CAST (npoint AS geometry) WITH FUNCTION geometry(npoint);
CREATE CAST (geometry AS npoint) WITH FUNCTION npoint(geometry);

CREATE FUNCTION geometry(nsegment)
	RETURNS geometry
	AS 'MODULE_PATHNAME', 'nsegment_as_geom'
	LANGUAGE C IMMUTABLE STRICT;

CREATE CAST (nsegment AS geometry) WITH FUNCTION geometry(nsegment);
-- CREATE CAST (geometry AS nsegment) WITH FUNCTION nsegment(geometry);

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

CREATE OR REPLACE FUNCTION segment_in_network(geo geometry)
RETURNS nsegment[] AS $$
DECLARE
     ns nsegment;
     result nsegment[];
BEGIN
     WITH tbl_route_intersection AS (
          -- Find intersections of geo and network
          -- Each intersection is either a point or a linestring
          SELECT gid, the_geom, (ST_Dump(ST_Intersection(the_geom, geo))).geom AS intersection
          FROM ways 
     ),   
          tbl_nsegment AS (
		  -- Linear reference for point
          SELECT nsegment(gid, ST_LineLocatePoint(the_geom, intersection)) AS ns
          FROM tbl_route_intersection
          WHERE ST_GeometryType(intersection) = 'ST_Point'
          UNION ALL
		  -- Linear reference for linestring
          SELECT nsegment(gid, ST_LineLocatePoint(the_geom, ST_StartPoint(intersection)),
		ST_LineLocatePoint(the_geom, ST_EndPoint(intersection))) AS ns
          FROM tbl_route_intersection
          WHERE ST_GeometryType(intersection) = 'ST_LineString' 
     )	  
     SELECT array_agg(t.ns)
     FROM tbl_nsegment t
     INTO result;
	 
     RETURN result;
END; 
$$ LANGUAGE plpgsql;

/******************************************************************************
 * Operators
 ******************************************************************************/

CREATE FUNCTION npoint_eq(npoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'npoint_eq'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION npoint_ne(npoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'npoint_ne'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION npoint_lt(npoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'npoint_lt'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION npoint_le(npoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'npoint_le'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION npoint_ge(npoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'npoint_ge'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION npoint_gt(npoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'npoint_gt'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION npoint_cmp(npoint, npoint)
	RETURNS int4
	AS 'MODULE_PATHNAME', 'npoint_cmp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 

CREATE OPERATOR = (
	PROCEDURE = npoint_eq,
	LEFTARG = npoint, RIGHTARG = npoint,
	COMMUTATOR = =, NEGATOR = <>,
	RESTRICT = eqsel, JOIN = eqjoinsel
);
CREATE OPERATOR <> (
	PROCEDURE = npoint_ne,
	LEFTARG = npoint, RIGHTARG = npoint,
	COMMUTATOR = <>, NEGATOR = =,
	RESTRICT = neqsel, JOIN = neqjoinsel
);
CREATE OPERATOR < (
	PROCEDURE = npoint_lt,
	LEFTARG = npoint, RIGHTARG = npoint,
	COMMUTATOR = >, NEGATOR = >=,
	RESTRICT = scalarltsel, JOIN = scalarltjoinsel 
);
CREATE OPERATOR <= (
	PROCEDURE = npoint_le,
	LEFTARG = npoint, RIGHTARG = npoint,
	COMMUTATOR = >=, NEGATOR = >,
	RESTRICT = scalarlesel, JOIN = scalarlejoinsel 
);
CREATE OPERATOR >= (
	PROCEDURE = npoint_ge,
	LEFTARG = npoint, RIGHTARG = npoint,
	COMMUTATOR = <=, NEGATOR = <,
	RESTRICT = scalargesel, JOIN = scalargejoinsel
);
CREATE OPERATOR > (
	PROCEDURE = npoint_gt,
	LEFTARG = npoint, RIGHTARG = npoint,
	COMMUTATOR = <, NEGATOR = <=,
	RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS npoint_ops
	DEFAULT FOR TYPE npoint USING btree	AS
	OPERATOR	1	< ,
	OPERATOR	2	<= ,
	OPERATOR	3	= ,
	OPERATOR	4	>= ,
	OPERATOR	5	> ,
	FUNCTION	1	npoint_cmp(npoint, npoint);

/******************************************************************************/

CREATE FUNCTION nsegment_eq(nsegment, nsegment)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'nsegment_eq'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION nsegment_ne(nsegment, nsegment)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'nsegment_ne'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION nsegment_lt(nsegment, nsegment)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'nsegment_lt'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION nsegment_le(nsegment, nsegment)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'nsegment_le'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION nsegment_ge(nsegment, nsegment)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'nsegment_ge'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION nsegment_gt(nsegment, nsegment)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'nsegment_gt'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 
CREATE FUNCTION nsegment_cmp(nsegment, nsegment)
	RETURNS int4
	AS 'MODULE_PATHNAME', 'nsegment_cmp'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE; 

CREATE OPERATOR = (
	PROCEDURE = nsegment_eq,
	LEFTARG = nsegment, RIGHTARG = nsegment,
	COMMUTATOR = =, NEGATOR = <>,
	RESTRICT = eqsel, JOIN = eqjoinsel
);
CREATE OPERATOR <> (
	PROCEDURE = nsegment_ne,
	LEFTARG = nsegment, RIGHTARG = nsegment,
	COMMUTATOR = <>, NEGATOR = =,
	RESTRICT = neqsel, JOIN = neqjoinsel
);
CREATE OPERATOR < (
	PROCEDURE = nsegment_lt,
	LEFTARG = nsegment, RIGHTARG = nsegment,
	COMMUTATOR = >, NEGATOR = >=,
	RESTRICT = scalarltsel, JOIN = scalarltjoinsel 
);
CREATE OPERATOR <= (
	PROCEDURE = nsegment_le,
	LEFTARG = nsegment, RIGHTARG = nsegment,
	COMMUTATOR = >=, NEGATOR = >,
	RESTRICT = scalarlesel, JOIN = scalarlejoinsel 
);
CREATE OPERATOR >= (
	PROCEDURE = nsegment_ge,
	LEFTARG = nsegment, RIGHTARG = nsegment,
	COMMUTATOR = <=, NEGATOR = <,
	RESTRICT = scalargesel, JOIN = scalargejoinsel
);
CREATE OPERATOR > (
	PROCEDURE = nsegment_gt,
	LEFTARG = nsegment, RIGHTARG = nsegment,
	COMMUTATOR = <, NEGATOR = <=,
	RESTRICT = scalargtsel, JOIN = scalargtjoinsel
);

CREATE OPERATOR CLASS nsegment_ops
	DEFAULT FOR TYPE nsegment USING btree	AS
	OPERATOR	1	< ,
	OPERATOR	2	<= ,
	OPERATOR	3	= ,
	OPERATOR	4	>= ,
	OPERATOR	5	> ,
	FUNCTION	1	nsegment_cmp(nsegment, nsegment);

/******************************************************************************/
