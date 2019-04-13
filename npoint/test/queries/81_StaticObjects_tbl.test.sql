/******************************************************************************
 * Input
 ******************************************************************************/

/******************************************************************************
 * Constructors
 ******************************************************************************/

/******************************************************************************
 * Accessing values
 ******************************************************************************/

SELECT MAX(route(np)) FROM tbl_npoint;
SELECT MAX(getPosition(np)) FROM tbl_npoint;

SELECT MAX(route(ns)) FROM tbl_nsegment;
SELECT MAX(startPosition(ns)) FROM tbl_nsegment;
SELECT MAX(endPosition(ns)) FROM tbl_nsegment;

/******************************************************************************
 * Cast functions
 ******************************************************************************/

SELECT st_astext(np::geometry) FROM tbl_npoint;
SELECT st_astext(ns::geometry) FROM tbl_nsegment;

SELECT count(*) FROM tbl_npoint WHERE np = (np::geometry)::npoint;
SELECT count(*) FROM tbl_nsegment WHERE ns = (ns::geometry)::nsegment;

/******************************************************************************
 * Conversions between network and space
 ******************************************************************************/

SELECT count(*) FROM tbl_geompoint t WHERE point_in_network(t.g) IS NOT NULL;

SELECT count(*) FROM tbl_geompoint t WHERE segment_in_network(t.g) IS NOT NULL;

/******************************************************************************
 * Comparisons
 ******************************************************************************/

SELECT count(*) FROM tbl_npoint t1, tbl_npoint t2 WHERE t1.np = t2.np;
SELECT count(*) FROM tbl_npoint t1, tbl_npoint t2 WHERE t1.np != t2.np;
SELECT count(*) FROM tbl_npoint t1, tbl_npoint t2 WHERE t1.np < t2.np;
SELECT count(*) FROM tbl_npoint t1, tbl_npoint t2 WHERE t1.np <= t2.np;
SELECT count(*) FROM tbl_npoint t1, tbl_npoint t2 WHERE t1.np > t2.np;
SELECT count(*) FROM tbl_npoint t1, tbl_npoint t2 WHERE t1.np >= t2.np;

SELECT count(*) FROM tbl_nsegment t1, tbl_nsegment t2 WHERE t1.ns = t2.ns;
SELECT count(*) FROM tbl_nsegment t1, tbl_nsegment t2 WHERE t1.ns != t2.ns;
SELECT count(*) FROM tbl_nsegment t1, tbl_nsegment t2 WHERE t1.ns < t2.ns;
SELECT count(*) FROM tbl_nsegment t1, tbl_nsegment t2 WHERE t1.ns <= t2.ns;
SELECT count(*) FROM tbl_nsegment t1, tbl_nsegment t2 WHERE t1.ns > t2.ns;
SELECT count(*) FROM tbl_nsegment t1, tbl_nsegment t2 WHERE t1.ns >= t2.ns;

/******************************************************************************/
