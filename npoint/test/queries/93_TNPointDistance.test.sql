/*****************************************************************************/

SELECT MAX(maxValue(t1.g <-> t2.temp)) FROM tbl_geompoint t1, tbl_tnpoint t2;
SELECT MAX(maxValue(t1.temp <-> t2.g)) FROM tbl_tnpoint t1, tbl_geompoint t2;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <-> t2.temp IS NOT NULL;

/*****************************************************************************/
