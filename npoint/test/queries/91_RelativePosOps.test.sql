﻿/*****************************************************************************
 * geometry op tnpoint
 *****************************************************************************/

SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g << t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g &< t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g >> t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g &> t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g <<| t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g &<| t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g |>> t2.temp;
SELECT count(*) FROM tbl_geompoint t1, tbl_tnpoint t2 WHERE t1.g |&> t2.temp;

/*****************************************************************************
 * timestamptz op tnpoint
 *****************************************************************************/

SELECT count(*) FROM tbl_timestamptz t1, tbl_tnpoint t2 WHERE t1.t <<# t2.temp;
SELECT count(*) FROM tbl_timestamptz t1, tbl_tnpoint t2 WHERE t1.t #>> t2.temp;
SELECT count(*) FROM tbl_timestamptz t1, tbl_tnpoint t2 WHERE t1.t &<# t2.temp;
SELECT count(*) FROM tbl_timestamptz t1, tbl_tnpoint t2 WHERE t1.t #&> t2.temp;

/*****************************************************************************
 * timestampset op tnpoint
 *****************************************************************************/

SELECT count(*) FROM tbl_timestampset t1, tbl_tnpoint t2 WHERE t1.ts <<# t2.temp;
SELECT count(*) FROM tbl_timestampset t1, tbl_tnpoint t2 WHERE t1.ts #>> t2.temp;
SELECT count(*) FROM tbl_timestampset t1, tbl_tnpoint t2 WHERE t1.ts &<# t2.temp;
SELECT count(*) FROM tbl_timestampset t1, tbl_tnpoint t2 WHERE t1.ts #&> t2.temp;

/*****************************************************************************
 * period op tnpoint
 *****************************************************************************/

SELECT count(*) FROM tbl_period t1, tbl_tnpoint t2 WHERE t1.p <<# t2.temp;
SELECT count(*) FROM tbl_period t1, tbl_tnpoint t2 WHERE t1.p #>> t2.temp;
SELECT count(*) FROM tbl_period t1, tbl_tnpoint t2 WHERE t1.p &<# t2.temp;
SELECT count(*) FROM tbl_period t1, tbl_tnpoint t2 WHERE t1.p #&> t2.temp;

/*****************************************************************************
 * periodset op tnpoint
 *****************************************************************************/

SELECT count(*) FROM tbl_periodset t1, tbl_tnpoint t2 WHERE t1.ps <<# t2.temp;
SELECT count(*) FROM tbl_periodset t1, tbl_tnpoint t2 WHERE t1.ps #>> t2.temp;
SELECT count(*) FROM tbl_periodset t1, tbl_tnpoint t2 WHERE t1.ps &<# t2.temp;
SELECT count(*) FROM tbl_periodset t1, tbl_tnpoint t2 WHERE t1.ps #&> t2.temp;

/*****************************************************************************
 * tnpoint op <type>
 *****************************************************************************/

SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp << t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp >> t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp &< t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp &> t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp <<| t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp |>> t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp &<| t2.g;
SELECT count(*) FROM tbl_tnpoint t1, tbl_geompoint t2 WHERE t1.temp |&> t2.g;

SELECT count(*) FROM tbl_tnpoint t1, tbl_timestamptz t2 WHERE t1.temp <<# t2.t;
SELECT count(*) FROM tbl_tnpoint t1, tbl_timestamptz t2 WHERE t1.temp #>> t2.t;
SELECT count(*) FROM tbl_tnpoint t1, tbl_timestamptz t2 WHERE t1.temp &<# t2.t;
SELECT count(*) FROM tbl_tnpoint t1, tbl_timestamptz t2 WHERE t1.temp #&> t2.t;

SELECT count(*) FROM tbl_tnpoint t1, tbl_timestampset t2 WHERE t1.temp <<# t2.ts;
SELECT count(*) FROM tbl_tnpoint t1, tbl_timestampset t2 WHERE t1.temp #>> t2.ts;
SELECT count(*) FROM tbl_tnpoint t1, tbl_timestampset t2 WHERE t1.temp &<# t2.ts;
SELECT count(*) FROM tbl_tnpoint t1, tbl_timestampset t2 WHERE t1.temp #&> t2.ts;

SELECT count(*) FROM tbl_tnpoint t1, tbl_period t2 WHERE t1.temp <<# t2.p;
SELECT count(*) FROM tbl_tnpoint t1, tbl_period t2 WHERE t1.temp #>> t2.p;
SELECT count(*) FROM tbl_tnpoint t1, tbl_period t2 WHERE t1.temp &<# t2.p;
SELECT count(*) FROM tbl_tnpoint t1, tbl_period t2 WHERE t1.temp #&> t2.p;

SELECT count(*) FROM tbl_tnpoint t1, tbl_periodset t2 WHERE t1.temp <<# t2.ps;
SELECT count(*) FROM tbl_tnpoint t1, tbl_periodset t2 WHERE t1.temp #>> t2.ps;
SELECT count(*) FROM tbl_tnpoint t1, tbl_periodset t2 WHERE t1.temp &<# t2.ps;
SELECT count(*) FROM tbl_tnpoint t1, tbl_periodset t2 WHERE t1.temp #&> t2.ps;

SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp << t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp >> t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp &< t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp &> t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <<| t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp |>> t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp &<| t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp |&> t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <<# t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp #>> t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp &<# t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp #&> t2.temp;

/*****************************************************************************/