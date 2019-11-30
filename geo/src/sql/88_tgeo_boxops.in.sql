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