/******************************************************************************
 * Input
 ******************************************************************************/

SELECT npoint 'npoint(1,0.5)';
SELECT npoint ' npoint   (   1   ,	0.5   )   ';

SELECT nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment '  nsegment  (  1  ,  0.5  ,  0.7 ) ';

/******************************************************************************
 * Constructors
 ******************************************************************************/

SELECT npoint(1, 0.5);

SELECT nsegment(1, 0.2, 0.6);
SELECT nsegment(1);
SELECT nsegment(1, 0.2);
SELECT nsegment(npoint(1, 0.5));

/******************************************************************************
 * Accessing values
 ******************************************************************************/

SELECT route(npoint 'npoint(1,0.5)');
SELECT getPosition(npoint 'npoint(1,0.5)');

SELECT route(nsegment 'nsegment(1,0.5,0.7)');
SELECT startPosition(nsegment 'nsegment(1,0.5,0.7)');
SELECT endPosition(nsegment 'nsegment(1,0.5,0.7)');

/******************************************************************************
 * Cast functions between network and space
 ******************************************************************************/

SELECT st_astext(npoint 'npoint(1,0.2)'::geometry);

SELECT st_astext(nsegment 'nsegment(1,0.5,0.7)'::geometry);

SELECT (npoint 'npoint(1,0.2)'::geometry)::npoint;

SELECT (nsegment 'nsegment(1,0.5,0.7)'::geometry)::nsegment;

SELECT geometry 'Point(610.455019399524 528.508247341961)'::npoint;

SELECT geometry 'LINESTRING(416.346567736997 528.335344322874,610.455019399524 528.508247341961,476.989195102204 642.550969672973)'::nsegment;

/******************************************************************************
 * Comparisons
 ******************************************************************************/

SELECT npoint 'npoint(1,0.5)' = npoint 'npoint(1,0.5)';
SELECT npoint 'npoint(1,0.5)' = npoint 'npoint(1,0.7)';
SELECT npoint 'npoint(1,0.5)' = npoint 'npoint(2,0.5)';

SELECT npoint 'npoint(1,0.5)' != npoint 'npoint(1,0.5)';
SELECT npoint 'npoint(1,0.5)' != npoint 'npoint(1,0.7)';
SELECT npoint 'npoint(1,0.5)' != npoint 'npoint(2,0.5)';

SELECT npoint 'npoint(1,0.5)' < npoint 'npoint(1,0.5)';
SELECT npoint 'npoint(1,0.5)' < npoint 'npoint(1,0.7)';
SELECT npoint 'npoint(1,0.5)' < npoint 'npoint(2,0.5)';

SELECT npoint 'npoint(1,0.5)' <= npoint 'npoint(1,0.5)';
SELECT npoint 'npoint(1,0.5)' <= npoint 'npoint(1,0.7)';
SELECT npoint 'npoint(1,0.5)' <= npoint 'npoint(2,0.5)';

SELECT npoint 'npoint(1,0.5)' > npoint 'npoint(1,0.5)';
SELECT npoint 'npoint(1,0.5)' > npoint 'npoint(1,0.7)';
SELECT npoint 'npoint(1,0.5)' > npoint 'npoint(2,0.5)';

SELECT npoint 'npoint(1,0.5)' >= npoint 'npoint(1,0.5)';
SELECT npoint 'npoint(1,0.5)' >= npoint 'npoint(1,0.7)';
SELECT npoint 'npoint(1,0.5)' >= npoint 'npoint(2,0.5)';

SELECT nsegment 'nsegment(1,0.3,0.5)' = nsegment 'nsegment(1,0.3,0.5)';
SELECT nsegment 'nsegment(1,0.3,0.5)' = nsegment 'nsegment(1,0.3,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' = nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' = nsegment 'nsegment(2,0.3,0.5)';

SELECT nsegment 'nsegment(1,0.3,0.5)' != nsegment 'nsegment(1,0.3,0.5)';
SELECT nsegment 'nsegment(1,0.3,0.5)' != nsegment 'nsegment(1,0.3,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' != nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' != nsegment 'nsegment(2,0.3,0.5)';

SELECT nsegment 'nsegment(1,0.3,0.5)' < nsegment 'nsegment(1,0.3,0.5)';
SELECT nsegment 'nsegment(1,0.3,0.5)' < nsegment 'nsegment(1,0.3,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' < nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' < nsegment 'nsegment(2,0.3,0.5)';

SELECT nsegment 'nsegment(1,0.3,0.5)' <= nsegment 'nsegment(1,0.3,0.5)';
SELECT nsegment 'nsegment(1,0.3,0.5)' <= nsegment 'nsegment(1,0.3,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' <= nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' <= nsegment 'nsegment(2,0.3,0.5)';

SELECT nsegment 'nsegment(1,0.3,0.5)' > nsegment 'nsegment(1,0.3,0.5)';
SELECT nsegment 'nsegment(1,0.3,0.5)' > nsegment 'nsegment(1,0.3,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' > nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' > nsegment 'nsegment(2,0.3,0.5)';

SELECT nsegment 'nsegment(1,0.3,0.5)' >= nsegment 'nsegment(1,0.3,0.5)';
SELECT nsegment 'nsegment(1,0.3,0.5)' >= nsegment 'nsegment(1,0.3,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' >= nsegment 'nsegment(1,0.5,0.7)';
SELECT nsegment 'nsegment(1,0.3,0.5)' >= nsegment 'nsegment(2,0.3,0.5)';

/******************************************************************************/
