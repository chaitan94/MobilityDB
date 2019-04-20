/*****************************************************************************/

SELECT max(st_npoints(st_astext(trajectory(temp)))) FROM tbl_tnpoint;

SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE atGeometry(t1.temp, t2.g) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE minusGeometry(t1.temp, t2.g) IS NOT NULL;

SELECT length(seq) FROM tbl_tnpointseq;
SELECT length(ts) FROM tbl_tnpoints;

SELECT cumulativeLength(seq) FROM tbl_tnpointseq;
SELECT cumulativeLength(ts) FROM tbl_tnpoints;

SELECT speed(seq) FROM tbl_tnpointseq;
SELECT speed(ts) FROM tbl_tnpoints;

SELECT azimuth(seq) FROM tbl_tnpointseq;
SELECT azimuth(ts) FROM tbl_tnpoints;

/*****************************************************************************/
