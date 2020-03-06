-------------------------------------------------------------------------------
-- Input
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Constructors
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Accessing values
-------------------------------------------------------------------------------

SELECT MAX(route(np)) FROM tbl_npoint;
SELECT MAX(getPosition(np)) FROM tbl_npoint;

SELECT MAX(route(ns)) FROM tbl_nsegment;
SELECT MAX(startPosition(ns)) FROM tbl_nsegment;
SELECT MAX(endPosition(ns)) FROM tbl_nsegment;

-------------------------------------------------------------------------------
-- Cast functions between network and space
-------------------------------------------------------------------------------

SELECT count(*) FROM tbl_npoint where np::geometry is not null;
SELECT count(*) FROM tbl_nsegment ns::geometry is not null;

SELECT count(*) FROM tbl_npoint WHERE np = (np::geometry)::npoint;
SELECT count(*) FROM tbl_nsegment WHERE ns = (ns::geometry)::nsegment;

SELECT count(*) FROM tbl_geompoint WHERE CASE WHEN NOT ST_IsEmpty(g) THEN ST_SetSRID(g, 5676)::npoint IS NOT NULL END;
SELECT count(*) FROM tbl_geomlinestring WHERE CASE WHEN NOT ST_IsEmpty(g) THEN ST_SetSRID(g, 5676)::nsegment IS NOT NULL END;

-------------------------------------------------------------------------------
-- Comparisons
-------------------------------------------------------------------------------

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

-------------------------------------------------------------------------------/
