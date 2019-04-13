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
 * Cast functions
 ******************************************************************************/

SELECT st_astext(npoint 'npoint(1,0.2)'::geometry);

SELECT st_astext(nsegment 'nsegment(1,0.5,0.7)'::geometry);

SELECT (npoint 'npoint(1,0.2)'::geometry)::npoint;

SELECT (nsegment 'nsegment(1,0.5,0.7)'::geometry)::nsegment;

/******************************************************************************
 * Conversions between network and space
 ******************************************************************************/

SELECT point_in_network(ST_MakePoint(4.3560493, 50.8504975));

SELECT segment_in_network(ST_MakeEnvelope(4.3, 50.8, 4.4, 50.9));

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
