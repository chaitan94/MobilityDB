/*****************************************************************************
 *
 * TNPointBoundBoxOps.sql
 *	  Bounding box operators for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li,
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

/*****************************************************************************/

CREATE FUNCTION tnpoint_overlaps_sel(internal, oid, internal, integer)
	RETURNS float
	AS 'MODULE_PATHNAME', 'tnpoint_overlaps_sel'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION tnpoint_overlaps_joinsel(internal, oid, internal, smallint, internal)
	RETURNS float
	AS 'MODULE_PATHNAME', 'tnpoint_overlaps_joinsel'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION tnpoint_contains_sel(internal, oid, internal, integer)
	RETURNS float
	AS 'MODULE_PATHNAME', 'tnpoint_contains_sel'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION tnpoint_contains_joinsel(internal, oid, internal, smallint, internal)
	RETURNS float
	AS 'MODULE_PATHNAME', 'tnpoint_contains_joinsel'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION tnpoint_same_sel(internal, oid, internal, integer)
	RETURNS float
	AS 'MODULE_PATHNAME', 'tnpoint_same_sel'
	LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION tnpoint_same_joinsel(internal, oid, internal, smallint, internal)
	RETURNS float
	AS 'MODULE_PATHNAME', 'tnpoint_same_joinsel'
	LANGUAGE C IMMUTABLE STRICT;

/*****************************************************************************
 * Temporal npoint to gbox
 *****************************************************************************/

CREATE FUNCTION gbox(npoint)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'npoint_to_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
CREATE FUNCTION gbox(npoint, timestamptz)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'npoint_timestamp_to_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
CREATE FUNCTION gbox(npoint, period)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'npoint_period_to_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
CREATE FUNCTION gbox(tnpoint)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'tnpoint_to_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
	
/*****************************************************************************
 * Expand
 *****************************************************************************/

CREATE FUNCTION expandSpatial(npoint, float)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'npoint_expand_spatial'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION expandSpatial(tnpoint, float)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'tpoint_expand_spatial'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE FUNCTION expandTemporal(tnpoint, interval)
	RETURNS gbox
	AS 'MODULE_PATHNAME', 'tpoint_expand_temporal'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

/*****************************************************************************
 * Contains
 *****************************************************************************/

CREATE FUNCTION contains_bbox(geometry, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_geo_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(gbox, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_gbox_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(npoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = geometry, RIGHTARG = tnpoint,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = gbox, RIGHTARG = tnpoint,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = npoint, RIGHTARG = tnpoint,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);

CREATE FUNCTION contains_bbox(tnpoint, geometry)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_geo'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tnpoint, gbox)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tnpoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contains_bbox(tnpoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contains_bbox_tpoint_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = tnpoint, RIGHTARG = geometry,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = tnpoint, RIGHTARG = gbox,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = tnpoint, RIGHTARG = npoint,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR @> (
	PROCEDURE = contains_bbox,
	LEFTARG = tnpoint, RIGHTARG = tnpoint,
	COMMUTATOR = <@,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);

/*****************************************************************************
 * Contained
 *****************************************************************************/

CREATE FUNCTION contained_bbox(geometry, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_geo_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(gbox, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_gbox_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(npoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = geometry, RIGHTARG = tnpoint,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = gbox, RIGHTARG = tnpoint,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = npoint, RIGHTARG = tnpoint,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);

CREATE FUNCTION contained_bbox(tnpoint, geometry)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_geo'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tnpoint, gbox)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tnpoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION contained_bbox(tnpoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'contained_bbox_tpoint_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = tnpoint, RIGHTARG = geometry,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = tnpoint, RIGHTARG = gbox,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = tnpoint, RIGHTARG = npoint,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);
CREATE OPERATOR <@ (
	PROCEDURE = contained_bbox,
	LEFTARG = tnpoint, RIGHTARG = tnpoint,
	COMMUTATOR = @>,
	RESTRICT = tnpoint_contains_sel, JOIN = tnpoint_contains_joinsel
);

/*****************************************************************************
 * Overlaps
 *****************************************************************************/

CREATE FUNCTION overlaps_bbox(geometry, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_geo_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(gbox, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_gbox_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(npoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = geometry, RIGHTARG = tnpoint,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);
CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = gbox, RIGHTARG = tnpoint,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);
CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = npoint, RIGHTARG = tnpoint,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);

CREATE FUNCTION overlaps_bbox(tnpoint, geometry)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_geo'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tnpoint, gbox)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tnpoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION overlaps_bbox(tnpoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'overlaps_bbox_tpoint_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = tnpoint, RIGHTARG = geometry,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);
CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = tnpoint, RIGHTARG = gbox,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);
CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = tnpoint, RIGHTARG = npoint,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);
CREATE OPERATOR && (
	PROCEDURE = overlaps_bbox,
	LEFTARG = tnpoint, RIGHTARG = tnpoint,
	COMMUTATOR = &&,
	RESTRICT = tnpoint_overlaps_sel, JOIN = tnpoint_overlaps_joinsel
);

/*****************************************************************************
 * Same
 *****************************************************************************/

CREATE FUNCTION same_bbox(geometry, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_geo_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(gbox, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_gbox_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(npoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_npoint_tnpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = geometry, RIGHTARG = tnpoint,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);
CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = gbox, RIGHTARG = tnpoint,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);
CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = npoint, RIGHTARG = tnpoint,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);

CREATE FUNCTION same_bbox(tnpoint, geometry)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_tpoint_geo'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tnpoint, gbox)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_tpoint_gbox'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tnpoint, npoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_tnpoint_npoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION same_bbox(tnpoint, tnpoint)
	RETURNS boolean
	AS 'MODULE_PATHNAME', 'same_bbox_tpoint_tpoint'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = tnpoint, RIGHTARG = geometry,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);
CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = tnpoint, RIGHTARG = gbox,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);
CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = tnpoint, RIGHTARG = npoint,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);
CREATE OPERATOR ~= (
	PROCEDURE = same_bbox,
	LEFTARG = tnpoint, RIGHTARG = tnpoint,
	COMMUTATOR = ~=,
	RESTRICT = tnpoint_same_sel, JOIN = tnpoint_same_joinsel
);

/*****************************************************************************/
