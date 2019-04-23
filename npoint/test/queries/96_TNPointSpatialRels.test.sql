﻿/*****************************************************************************
 * Geometry rel tnpoint
 *****************************************************************************/

SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE contains(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE containsproperly(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE covers(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE coveredby(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE crosses(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE disjoint(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE equals(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE intersects(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE overlaps(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE touches(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE within(t1.g, t2.temp);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE dwithin(t1.g, t2.temp, 0.01);
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE relate(t1.g, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE relate(t1.g, t2.temp, 'T*****FF*');

/*****************************************************************************
 * tnpoint rel <type>
 *****************************************************************************/

SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE contains(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE containsproperly(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE covers(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE coveredby(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE crosses(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE disjoint(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE equals(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE intersects(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE overlaps(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE touches(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE within(t1.temp, t2.g);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE dwithin(t1.temp, t2.g, 0.01);
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE relate(t1.temp, t2.g) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE relate(t1.temp, t2.g, 'T*****FF*');

SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE contains(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE containsproperly(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE covers(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE coveredby(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE crosses(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE disjoint(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE equals(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE intersects(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE overlaps(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE touches(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE within(t1.temp, t2.temp);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE dwithin(t1.temp, t2.temp, 0.01);
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE relate(t1.temp, t2.temp) IS NOT NULL;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE relate(t1.temp, t2.temp, 'T*****FF*');

/*****************************************************************************/