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

SELECT nregion(1, 0.2, 0.6);
SELECT nregion(1);
SELECT nregion(1, 0.5);
SELECT nregion(npoint 'npoint(1,0.5)');
SELECT nregion_agg(nregion(gid)) FROM ways WHERE gid <= 10;

/******************************************************************************
 * Accessing values
 ******************************************************************************/

SELECT route(npoint 'npoint(1,0.5)');
SELECT pos(npoint 'npoint(1,0.5)');
SELECT * FROM segments(nregion '{npoint(1,0.2,0.6),npoint(2,0,1)}');

/******************************************************************************
 * Conversions between network and space
 ******************************************************************************/

SELECT in_space(npoint 'npoint(1,0.2)');
SELECT in_space(nregion '{npoint(1,0.2,0.6),npoint(2,0,1)}');
SELECT point_in_network(ST_SetSRID(ST_MakePoint(4.3560493, 50.8504975), 4326));
SELECT geometry_in_network(ST_MakeEnvelope(4.3, 50.8, 4.4, 50.9, 4326));

/******************************************************************************/
