/*****************************************************************************/

SELECT max(st_npoints(st_astext(trajectory(temp)))) FROM tbl_tnpoint;

SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE atGeometry(t1.temp, t2.g) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE minusGeometry(t1.temp, t2.g) IS NOT NULL;

SELECT MAX(length(temp)) FROM tbl_tnpoint;

SELECT MAX(maxValue(cumulativeLength(temp))) FROM tbl_tnpoint;

SELECT MAX(maxValue(speed(temp))) FROM tbl_tnpoint;

SELECT round(azimuth(temp), 13) FROM tbl_tnpoint WHERE azimuth(temp) IS NOT NULL LIMIT 10;

/*****************************************************************************/

SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE nearestApproachInstant(t1.temp, t2.g) IS NOT NULL;
SELECT count(*) FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE nearestApproachInstant(t1.g, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE nearestApproachInstant(t1.temp, t2.np) IS NOT NULL;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE nearestApproachInstant(t1.np, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE nearestApproachInstant(t1.temp, t2.temp) IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE nearestApproachDistance(t1.temp, t2.g) IS NOT NULL;
SELECT count(*) FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE nearestApproachDistance(t1.g, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE nearestApproachDistance(t1.temp, t2.np) IS NOT NULL;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE nearestApproachDistance(t1.np, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE nearestApproachDistance(t1.temp, t2.temp) IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE shortestLine(t1.temp, t2.g) IS NOT NULL;
SELECT count(*) FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE shortestLine(t1.g, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE shortestLine(t1.temp, t2.np) IS NOT NULL;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE shortestLine(t1.np, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE shortestLine(t1.np, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE shortestLine(t1.temp, t2.temp) IS NOT NULL;


/*****************************************************************************/

