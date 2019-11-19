/*****************************************************************************
 *
 * rtransform.sql
 *    Region transformation type.
 *
 * Portions Copyright (c) 2019, Maxime Schoemans, Esteban Zimanyi,
 *      Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE TYPE rtransform;

/******************************************************************************
 * Input/Output
 ******************************************************************************/

CREATE FUNCTION rtransform_in(cstring)
    RETURNS rtransform
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION rtransform_out(rtransform)
    RETURNS cstring
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION rtransform_recv(internal)
    RETURNS rtransform
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE FUNCTION rtransform_send(rtransform)
    RETURNS bytea
    AS 'MODULE_PATHNAME'
    LANGUAGE C IMMUTABLE STRICT;

CREATE TYPE rtransform (
    internallength = 24,
    input = rtransform_in,
    output = rtransform_out,
    receive = rtransform_recv,
    send = rtransform_send,
    alignment = double
);