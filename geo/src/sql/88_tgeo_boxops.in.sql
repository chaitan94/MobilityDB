/****************************************************************************
 *
 * tgeo_boxops.sql
 *    Bounding box operators for temporal geometries.
 *
 * Portions Copyright (c) 2020, Maxime Schoemans, Esteban Zimanyi, 
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE FUNCTION stbox(tgeometry)
    RETURNS stbox
    AS 'MODULE_PATHNAME', 'tgeo_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION stbox(tgeography)
    RETURNS stbox
    AS 'MODULE_PATHNAME', 'tgeo_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE CAST (tgeometry AS stbox) WITH FUNCTION stbox(tgeometry);
CREATE CAST (tgeography AS stbox) WITH FUNCTION stbox(tgeography);

/*CREATE FUNCTION stboxes(tgeometry)
    RETURNS stbox[]
    AS 'MODULE_PATHNAME', 'tgeo_stboxes'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;*/

/*****************************************************************************/

CREATE FUNCTION expandSpatial(tgeometry, float)
    RETURNS stbox
    AS 'MODULE_PATHNAME', 'tpoint_expand_spatial'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION expandSpatial(tgeography, float)
    RETURNS stbox
    AS 'MODULE_PATHNAME', 'tpoint_expand_spatial'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION expandTemporal(tgeometry, interval)
    RETURNS stbox
    AS 'MODULE_PATHNAME', 'tpoint_expand_temporal'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION expandTemporal(tgeography, interval)
    RETURNS stbox
    AS 'MODULE_PATHNAME', 'tpoint_expand_temporal'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * Contains
 *****************************************************************************/

CREATE FUNCTION contains_bbox(geometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(stbox, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tgeometry, geometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tgeometry, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tgeometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = geometry, RIGHTARG = tgeometry,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = stbox, RIGHTARG = tgeometry,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = tgeometry, RIGHTARG = geometry,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = tgeometry, RIGHTARG = stbox,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = <@
);

/*****************************************************************************/

CREATE FUNCTION contains_bbox(geography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(stbox, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tgeography, geography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tgeography, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tgeography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = geography, RIGHTARG = tgeography,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = stbox, RIGHTARG = tgeography,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = tgeography, RIGHTARG = geography,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = tgeography, RIGHTARG = stbox,
    COMMUTATOR = <@
);
CREATE OPERATOR @> (
    PROCEDURE = contains_bbox,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = <@
);

/*****************************************************************************
 * Contained
 *****************************************************************************/

CREATE FUNCTION contained_bbox(geometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(stbox, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tgeometry, geometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tgeometry, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tgeometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = geometry, RIGHTARG = tgeometry,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = stbox, RIGHTARG = tgeometry,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = tgeometry, RIGHTARG = geometry,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = tgeometry, RIGHTARG = stbox,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = @>
);

/*****************************************************************************/

CREATE FUNCTION contained_bbox(geography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(stbox, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tgeography, geography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tgeography, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tgeography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = geography, RIGHTARG = tgeography,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = stbox, RIGHTARG = tgeography,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = tgeography, RIGHTARG = geography,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = tgeography, RIGHTARG = stbox,
    COMMUTATOR = @>
);
CREATE OPERATOR <@ (
    PROCEDURE = contained_bbox,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = @>
);

/*****************************************************************************
 * Overlaps
 *****************************************************************************/

CREATE FUNCTION overlaps_bbox(geometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(stbox, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tgeometry, geometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tgeometry, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tgeometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = geometry, RIGHTARG = tgeometry,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = stbox, RIGHTARG = tgeometry,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = tgeometry, RIGHTARG = geometry,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = tgeometry, RIGHTARG = stbox,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = &&
);

/*****************************************************************************/

CREATE FUNCTION overlaps_bbox(geography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(stbox, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tgeography, geography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tgeography, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tgeography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = geography, RIGHTARG = tgeography,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = stbox, RIGHTARG = tgeography,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = tgeography, RIGHTARG = geography,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = tgeography, RIGHTARG = stbox,
    COMMUTATOR = &&
);
CREATE OPERATOR && (
    PROCEDURE = overlaps_bbox,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = &&
);

/*****************************************************************************
 * Same
 *****************************************************************************/

CREATE FUNCTION same_bbox(geometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(stbox, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tgeometry, geometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tgeometry, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tgeometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = geometry, RIGHTARG = tgeometry,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = stbox, RIGHTARG = tgeometry,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = tgeometry, RIGHTARG = geometry,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = tgeometry, RIGHTARG = stbox,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = ~=
);

/*****************************************************************************/

CREATE FUNCTION same_bbox(geography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(stbox, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tgeography, geography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tgeography, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tgeography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'same_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = geography, RIGHTARG = tgeography,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = stbox, RIGHTARG = tgeography,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = tgeography, RIGHTARG = geography,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = tgeography, RIGHTARG = stbox,
    COMMUTATOR = ~=
);
CREATE OPERATOR ~= (
    PROCEDURE = same_bbox,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = ~=
);

/*****************************************************************************
 * Adjacent
 *****************************************************************************/

CREATE FUNCTION adjacent_bbox(geometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(stbox, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(tgeometry, geometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(tgeometry, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(tgeometry, tgeometry)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = geometry, RIGHTARG = tgeometry,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = stbox, RIGHTARG = tgeometry,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = tgeometry, RIGHTARG = geometry,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = tgeometry, RIGHTARG = stbox,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = tgeometry, RIGHTARG = tgeometry,
    COMMUTATOR = -|-
);

/*****************************************************************************/

CREATE FUNCTION adjacent_bbox(geography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_geo_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(stbox, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_stbox_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(tgeography, geography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_tpoint_geo'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(tgeography, stbox)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_tpoint_stbox'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION adjacent_bbox(tgeography, tgeography)
    RETURNS boolean
    AS 'MODULE_PATHNAME', 'adjacent_bbox_tpoint_tpoint'
    LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = geography, RIGHTARG = tgeography,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = stbox, RIGHTARG = tgeography,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = tgeography, RIGHTARG = geography,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = tgeography, RIGHTARG = stbox,
    COMMUTATOR = -|-
);
CREATE OPERATOR -|- (
    PROCEDURE = adjacent_bbox,
    LEFTARG = tgeography, RIGHTARG = tgeography,
    COMMUTATOR = -|-
);

/****************************************************************************/