/******************************************************************************
 * Input
 ******************************************************************************/

SELECT npoint 'npoint(1,0.5)';

SELECT nsegment 'nsegment(1,0.5,0.7)';


/******************************************************************************
 * Constructors
 ******************************************************************************/

SELECT npoint(1, 0.5);

SELECT nsegment(1, 0.2, 0.6);

/******************************************************************************
 * Accessing values
 ******************************************************************************/

SELECT route(npoint 'npoint(1,0.5)');
SELECT getPosition(npoint 'npoint(1,0.5)');

SELECT route(np) FROM tbl_npoint;
SELECT getPosition(np) FROM tbl_npoint;

SELECT route(nsegment 'nsegment(1,0.5,0.7)');
SELECT startPosition(nsegment 'nsegment(1,0.5,0.7)');
SELECT endPosition(nsegment 'nsegment(1,0.5,0.7)');

SELECT route(ns) FROM tbl_nsegment;
SELECT startPosition(ns) FROM tbl_nsegment;
SELECT endPosition(ns) FROM tbl_nsegment;

/******************************************************************************
 * Cast functions
 ******************************************************************************/

SELECT npoint 'npoint(1,0.2)'::geometry;

SELECT in_space(nsegment 'nsegment(1,0.5,0.7)');

SELECT np::geometry FROM tbl_npoint;
SELECT ns::geometry FROM tbl_nsegment;

SELECT count(*) FROM tbl_npoint WHERE np <> (np::geometry)::npoint;

/******************************************************************************
 * Conversions between network and space
 ******************************************************************************/



SELECT point_in_network(ST_MakePoint(4.3560493, 50.8504975));

SELECT count(*) FROM tbl_geompoint t WHERE point_in_network(t.g) IS NOT NULL;

SELECT segment_in_network(ST_MakeEnvelope(4.3, 50.8, 4.4, 50.9));

SELECT count(*) FROM tbl_geompoint t WHERE segment_in_network(t.g) IS NOT NULL;

/******************************************************************************/
