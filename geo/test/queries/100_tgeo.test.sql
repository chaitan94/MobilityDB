-------------------------------------------------------------------------------
-- Input/output functions
-------------------------------------------------------------------------------

-- Temporal instant

SELECT astext(tgeometry 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2012-01-01 08:00:00');
SELECT astext(tgeometry '  Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2012-01-01 08:00:00  ');
SELECT astext(tgeography 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2012-01-01 08:00:00');
SELECT astext(tgeography '  Polygon((0 0, 2 0, 2 2, 0 2, 0 0)) @ 2012-01-01 08:00:00  ');
/* Errors */
SELECT tgeometry 'TRUE@2012-01-01 08:00:00';
SELECT tgeography 'ABC@2012-01-01 08:00:00';
SELECT tgeometry 'Polygon empty@2000-01-01 00:00:00+01';
SELECT tgeography 'Polygon empty@2000-01-01 00:00:00+01';
SELECT tgeometry 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01 00:00:00+01 ,';
SELECT tgeography 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01 00:00:00+01 ,';

-------------------------------------------------------------------------------

-- Temporal instant set

SELECT astext(tgeometry ' { Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 } ');
SELECT astext(tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00}');
SELECT astext(tgeography ' { Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 } ');
SELECT astext(tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00}');
/* Errors */
SELECT astext(tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}');
SELECT astext(tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}');

SELECT tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon empty@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}';
SELECT tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon empty@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}';
SELECT tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0 0, 2 0 0, 2 2 0, 0 2 0, 0 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}';
SELECT tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0 0, 2 0 0, 2 2 0, 0 2 0, 0 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}';
SELECT tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]';
SELECT tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]';
SELECT tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00} xxx';
SELECT tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00} xxx';

-------------------------------------------------------------------------------

-- Temporal sequence

SELECT astext(tgeometry ' [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ] ');
SELECT astext(tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]');
SELECT astext(tgeography ' [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ] ');
SELECT astext(tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]');
/* Errors */
SELECT astext(tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]');
SELECT astext(tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]');
SELECT astext(tgeometry '[Polygon((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))@2001-01-01, Polygon((0 0 0, 2 0 0, 2 2 0, 0 2 0, 0 0 0))@2001-01-02, Polygon((0 0 0, 3 0 0, 3 3 0, 0 3 0, 0 0 0))@2001-01-03]');

SELECT tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon empty@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]';
SELECT tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon empty@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]';
SELECT tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0 0, 2 0 0, 2 2 0, 0 2 0, 0 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]';
SELECT tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0 0, 2 0 0, 2 2 0, 0 2 0, 0 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]';
SELECT tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}';
SELECT tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}';
SELECT tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00] xxx';
SELECT tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00] xxx';

-------------------------------------------------------------------------------

-- Temporal sequence set

SELECT astext(tgeometry '  { [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ],
 [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00 , Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00 ] } ');
SELECT astext(tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');

SELECT astext(tgeography '  { [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ],
 [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00 , Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00 ] } ');
SELECT astext(tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');

/* Errors */
SELECT astext(tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');
SELECT astext(tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');

SELECT tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00, Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00, Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00, Polygon empty@2001-01-01 09:05:00, Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}';
SELECT tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00, Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00, Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00, Polygon empty@2001-01-01 09:05:00, Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}';
SELECT tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00],[Polygon((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]}';
SELECT tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00],[Polygon((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]}';
SELECT tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00],[Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]';
SELECT tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00],[Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]';
SELECT tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00],[Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]} xxx';
SELECT tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00],[Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]} xxx';

-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Constructor functions
-------------------------------------------------------------------------------

SELECT asewkt(tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'));
SELECT asewkt(tgeometryinst(NULL, '2012-01-01 08:00:00'));
SELECT asewkt(tgeographyinst(ST_Polygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry, 4326), '2012-01-01 08:00:00'));
SELECT asewkt(tgeographyinst(NULL, '2012-01-01 08:00:00'));
/* Errors */
SELECT asewkt(tgeometryinst(geometry 'polygon empty', timestamptz '2000-01-01'));
SELECT asewkt(tgeographyinst(geography 'polygon empty', timestamptz '2000-01-01'));

-------------------------------------------------------------------------------

SELECT asewkt(tgeometryi(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));
SELECT asewkt(tgeographyi(ARRAY[
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeographyinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));

/* Errors */
SELECT asewkt(tgeometryi(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 1 1, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));
SELECT asewkt(tgeometryi(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0 0, 1 0 0, 0 1 0, 0 0 0)'::geometry), '2012-01-01 08:20:00')
]));
SELECT asewkt(tgeometryi(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 2 0, 0 2, 0 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));

-------------------------------------------------------------------------------

SELECT asewkt(tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));
SELECT asewkt(tgeographyseq(ARRAY[
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeographyinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));

/* Errors */
SELECT asewkt(tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 1 1, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));
SELECT asewkt(tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0 0, 1 0 0, 0 1 0, 0 0 0)'::geometry), '2012-01-01 08:20:00')
]));
SELECT asewkt(tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 2 0, 0 2, 0 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]));

-------------------------------------------------------------------------------

SELECT asewkt(tgeometrys(ARRAY[
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]),
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 09:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 09:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 09:20:00')
])]));
SELECT asewkt(tgeometrys(ARRAY[
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]),
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:30:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:40:00')
], FALSE, TRUE)]));
SELECT asewkt(tgeographys(ARRAY[
tgeographyseq(ARRAY[
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeographyinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]),
tgeographyseq(ARRAY[
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 09:00:00'),
tgeographyinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 09:10:00'),
tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 09:20:00')
])]));

/* Errors */
SELECT asewkt(tgeometrys(ARRAY[
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 1 1, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]),
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 09:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 1 1, 0 1, 0 0)'::geometry), '2012-01-01 09:20:00')
])]));
SELECT asewkt(tgeometrys(ARRAY[
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0 0, 1 0 0, 0 1 0, 0 0 0)'::geometry), '2012-01-01 08:20:00')
]),
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 09:00:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0 0, 1 0 0, 0 1 0, 0 0 0)'::geometry), '2012-01-01 09:20:00')
])]));
SELECT asewkt(tgeometrys(ARRAY[
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 2 0, 1 1, 1 0)'::geometry), '2012-01-01 08:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:20:00')
]),
tgeometryseq(ARRAY[
tgeometryinst(ST_MakePolygon('LineString(0 0, 2 0, 0 2, 0 0)'::geometry), '2012-01-01 09:00:00'),
tgeometryinst(ST_MakePolygon('LineString(1 0, 3 0, 1 2, 1 0)'::geometry), '2012-01-01 09:10:00'),
tgeometryinst(ST_MakePolygon('LineString(0 0, 2 0, 0 2, 0 0)'::geometry), '2012-01-01 09:20:00')
])]));