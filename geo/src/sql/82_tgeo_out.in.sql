/*****************************************************************************
 *
 * tgeo_out.sql
 *    Output of temporal geometries and geographies in WKT and EWKT format
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/


/******************************************************************************
 * Output as region
 ******************************************************************************/

CREATE FUNCTION asText(tgeometry)
    RETURNS text
    AS 'MODULE_PATHNAME', 'tgeo_as_text'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION asText(tgeometry[])
    RETURNS text[]
    AS 'MODULE_PATHNAME', 'tgeoarr_as_text'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION asText(tgeography)
    RETURNS text
    AS 'MODULE_PATHNAME', 'tgeo_as_text'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION asText(tgeography[])
    RETURNS text[]
    AS 'MODULE_PATHNAME', 'tgeoarr_as_text'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION asEWKT(tgeometry)
    RETURNS text
    AS 'MODULE_PATHNAME', 'tgeo_as_ewkt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION asEWKT(tgeometry[])
    RETURNS text[]
    AS 'MODULE_PATHNAME', 'tgeoarr_as_ewkt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION asEWKT(tgeography)
    RETURNS text
    AS 'MODULE_PATHNAME', 'tgeo_as_ewkt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION asEWKT(tgeography[])
    RETURNS text[]
    AS 'MODULE_PATHNAME', 'tgeoarr_as_ewkt'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/******************************************************************************
 * Output as rtransform
 ******************************************************************************/

 CREATE FUNCTION asTransform(tgeometry)
    RETURNS text
    AS 'MODULE_PATHNAME', 'tgeo_as_transform'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION asTransform(tgeometry[])
    RETURNS text[]
    AS 'MODULE_PATHNAME', 'tgeoarr_as_transform'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION asTransform(tgeography)
    RETURNS text
    AS 'MODULE_PATHNAME', 'tgeo_as_transform'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION asTransform(tgeography[])
    RETURNS text[]
    AS 'MODULE_PATHNAME', 'tgeoarr_as_transform'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************/
