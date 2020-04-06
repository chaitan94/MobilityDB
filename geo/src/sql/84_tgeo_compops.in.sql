/*****************************************************************************
 *
 * tgeo_compops.sql
 *    Comparison functions and operators for temporal geometries.
 *
 * Portions Copyright (c) 2020, Maxime Schoemans, Esteban Zimanyi,
 *    Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

/*****************************************************************************
 * Temporal equal
 *****************************************************************************/

CREATE FUNCTION tgeo_eq(geometry(Polygon), tgeometry)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'teq_geo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_eq(tgeometry, geometry(Polygon))
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'teq_tgeo_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_eq(tgeometry, tgeometry)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'teq_tgeo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR #= (
    PROCEDURE = tgeo_eq,
    LEFTARG = geometry(Polygon), RIGHTARG = tgeometry,
    COMMUTATOR = #=
);
CREATE OPERATOR #= (
    PROCEDURE = tgeo_eq,
    LEFTARG = tgeometry, RIGHTARG = geometry(Polygon),
    COMMUTATOR = #=
);
CREATE OPERATOR #= (
    PROCEDURE = tgeo_eq,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = #=
);

/*****************************************************************************/

CREATE FUNCTION tgeo_eq(geography(Polygon), tgeography)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'teq_geo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_eq(tgeography, geography(Polygon))
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'teq_tgeo_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_eq(tgeography, tgeography)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'teq_tgeo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR #= (
    PROCEDURE = tgeo_eq,
    LEFTARG = geography(Polygon), RIGHTARG = tgeography,
    COMMUTATOR = #=
);
CREATE OPERATOR #= (
    PROCEDURE = tgeo_eq,
    LEFTARG = tgeography, RIGHTARG = geography(Polygon),
    COMMUTATOR = #=
);
CREATE OPERATOR #= (
    PROCEDURE = tgeo_eq,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = #=
);

/*****************************************************************************
 * Temporal not equal
 *****************************************************************************/

CREATE FUNCTION tgeo_ne(geometry(Polygon), tgeometry)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'tne_geo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_ne(tgeometry, geometry(Polygon))
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'tne_tgeo_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_ne(tgeometry, tgeometry)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'tne_tgeo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR #<> (
    PROCEDURE = tgeo_ne,
    LEFTARG = geometry(Polygon), RIGHTARG = tgeometry,
    COMMUTATOR = #<>
);
CREATE OPERATOR #<> (
    PROCEDURE = tgeo_ne,
    LEFTARG = tgeometry, RIGHTARG = geometry(Polygon),
    COMMUTATOR = #<>
);
CREATE OPERATOR #<> (
    PROCEDURE = tgeo_ne,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = #<>
);

/*****************************************************************************/

CREATE FUNCTION tgeo_ne(geography(Polygon), tgeography)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'tne_geo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_ne(tgeography, geography(Polygon))
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'tne_tgeo_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION tgeo_ne(tgeography, tgeography)
    RETURNS tbool
    AS 'MODULE_PATHNAME', 'tne_tgeo_tgeo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR #<> (
    PROCEDURE = tgeo_ne,
    LEFTARG = geography(Polygon), RIGHTARG = tgeography,
    COMMUTATOR = #<>
);
CREATE OPERATOR #<> (
    PROCEDURE = tgeo_ne,
    LEFTARG = tgeography, RIGHTARG = geography(Polygon),
    COMMUTATOR = #<>
);
CREATE OPERATOR #<> (
    PROCEDURE = tgeo_ne,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = #<>
);

/******************************************************************************/
