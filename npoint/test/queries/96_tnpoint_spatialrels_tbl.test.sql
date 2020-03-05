﻿-------------------------------------------------------------------------------
set parallel_tuple_cost=0;
set parallel_setup_cost=0;
set force_parallel_mode=regress;
-------------------------------------------------------------------------------
-- Geometry rel tnpoint
 -------------------------------------------------------------------------------

SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE contains(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE containsproperly(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE covers(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE coveredby(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE crosses(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE disjoint(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE equals(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE intersects(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE overlaps(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE touches(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE within(t1.g, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE dwithin(t1.g, t2.temp, 0.01) AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE relate(t1.g, t2.temp) IS NOT NULL AND t1.k < 10;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE relate(t1.g, t2.temp, 'T*****FF*') AND t1.k < 10;

-------------------------------------------------------------------------------
-- npoint rel tnpoint
 -------------------------------------------------------------------------------

SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE contains(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE containsproperly(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE covers(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE coveredby(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE crosses(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE disjoint(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE equals(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE intersects(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE overlaps(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE touches(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE within(t1.np, t2.temp) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE dwithin(t1.np, t2.temp, 0.01) AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE relate(t1.np, t2.temp) IS NOT NULL AND t1.k < 10;
SELECT count(*) FROM tbl_npoint t1, tbl_tnpoint t2 WHERE relate(t1.np, t2.temp, 'T*****FF*') AND t1.k < 10;

-------------------------------------------------------------------------------
-- tnpoint rel <type>
 -------------------------------------------------------------------------------

SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE contains(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE containsproperly(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE covers(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE coveredby(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE crosses(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE disjoint(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE equals(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE intersects(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE overlaps(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE touches(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE within(t1.temp, t2.g) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE dwithin(t1.temp, t2.g, 0.01) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE relate(t1.temp, t2.g) IS NOT NULL AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE relate(t1.temp, t2.g, 'T*****FF*') AND t2.k < 10;

SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE contains(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE containsproperly(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE covers(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE coveredby(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE crosses(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE disjoint(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE equals(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE intersects(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE overlaps(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE touches(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE within(t1.temp, t2.np) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE dwithin(t1.temp, t2.np, 0.01) AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE relate(t1.temp, t2.np) IS NOT NULL AND t2.k < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE relate(t1.temp, t2.np, 'T*****FF*') AND t2.k < 10;

SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE contains(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE containsproperly(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE covers(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE coveredby(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE crosses(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE disjoint(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE equals(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE intersects(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE overlaps(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE touches(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE within(t1.temp, t2.temp) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE dwithin(t1.temp, t2.temp, 0.01) AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE relate(t1.temp, t2.temp) IS NOT NULL AND t1.k%25 < 10 AND t2.k%25 < 10;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE relate(t1.temp, t2.temp, 'T*****FF*') AND t1.k%25 < 10 AND t2.k%25 < 10;

-------------------------------------------------------------------------------
set parallel_tuple_cost=100;
set parallel_setup_cost=100;
set force_parallel_mode=off;
-------------------------------------------------------------------------------