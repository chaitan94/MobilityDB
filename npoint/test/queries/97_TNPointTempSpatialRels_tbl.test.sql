﻿/*****************************************************************************
 * Geometry rel temporal npoint
 *****************************************************************************/

SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tcontains(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tcovers(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tcoveredby(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tdisjoint(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tequals(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tintersects(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE ttouches(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE twithin(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE tdwithin(t1.g, t2.temp, 0.01) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE trelate(t1.g, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_geomcollection t1, tbl_tnpoint t2 WHERE trelate(t1.g, t2.temp, 'T*****FF*') IS NOT NULL LIMIT 10;

/*****************************************************************************
 * tnpoint rel <Type>
 *****************************************************************************/

SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tcontains(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tcovers(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tcoveredby(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tdisjoint(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tequals(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tintersects(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE ttouches(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE twithin(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE tdwithin(t1.temp, t2.g, 0.01) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE trelate(t1.temp, t2.g) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_geomcollection t2 WHERE trelate(t1.temp, t2.g, 'T*****FF*') IS NOT NULL LIMIT 10;

SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tcontains(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tcovers(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tcoveredby(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tdisjoint(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tequals(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tintersects(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE ttouches(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE twithin(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE tdwithin(t1.temp, t2.temp, 0.01) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE trelate(t1.temp, t2.temp) IS NOT NULL LIMIT 10;
SELECT t1.k, t2.k FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE trelate(t1.temp, t2.temp, 'T*****FF*') IS NOT NULL LIMIT 10;

/*****************************************************************************/
