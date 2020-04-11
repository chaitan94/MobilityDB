-------------------------------------------------------------------------------
-- Input/output functions
-------------------------------------------------------------------------------

-- Temporal instant

SELECT asText(tgeometry 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2012-01-01 08:00:00');
SELECT asText(tgeometry '  Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2012-01-01 08:00:00  ');
SELECT asText(tgeography 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2012-01-01 08:00:00');
SELECT asText(tgeography '  Polygon((0 0, 2 0, 2 2, 0 2, 0 0)) @ 2012-01-01 08:00:00  ');
/* Errors */
SELECT tgeometry 'TRUE@2012-01-01 08:00:00';
SELECT tgeography 'ABC@2012-01-01 08:00:00';
SELECT tgeometry 'Polygon empty@2000-01-01 00:00:00+01';
SELECT tgeography 'Polygon empty@2000-01-01 00:00:00+01';
SELECT tgeometry 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01 00:00:00+01 ,';
SELECT tgeography 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01 00:00:00+01 ,';

-------------------------------------------------------------------------------

-- Temporal instant set

SELECT asText(tgeometry ' { Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 } ');
SELECT asText(tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00}');
SELECT asText(tgeography ' { Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 } ');
SELECT asText(tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00}');
/* Errors */
SELECT asText(tgeometry '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}');
SELECT asText(tgeography '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00}');

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

SELECT asText(tgeometry ' [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ] ');
SELECT asText(tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]');
SELECT asText(tgeography ' [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ] ');
SELECT asText(tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00]');
/* Errors */
SELECT asText(tgeometry '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]');
SELECT asText(tgeography '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00]');
SELECT asText(tgeometry '[Polygon((0 0 0, 1 0 0, 1 1 0, 0 1 0, 0 0 0))@2001-01-01, Polygon((0 0 0, 2 0 0, 2 2 0, 0 2 0, 0 0 0))@2001-01-02, Polygon((0 0 0, 3 0 0, 3 3 0, 0 3 0, 0 0 0))@2001-01-03]');

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

SELECT asText(tgeometry '  { [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ],
 [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00 , Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00 ] } ');
SELECT asText(tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');

SELECT asText(tgeography '  { [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00 , Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00 ],
 [ Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00 , Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00 , Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00 ] } ');
SELECT asText(tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 08:05:00,Polygon((1 1, 0 1, 0 0, 1 0, 1 1))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((1 0, 1 1, 0 1, 0 0, 1 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');

/* Errors */
SELECT asText(tgeometry '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00],
 [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 09:05:00,Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 09:06:00]}');
SELECT asText(tgeography '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2001-01-01 08:00:00,Polygon((0 0, 2 0, 2 2, 0 2, 0 0))@2001-01-01 08:05:00,Polygon((0 0, 3 0, 3 3, 0 3, 0 0))@2001-01-01 08:06:00],
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
-- SRID
-------------------------------------------------------------------------------

SELECT asewkt(tgeometry 'SRID=4326;[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry '[SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry '[SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, SRID=4326;Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');

SELECT asewkt(tgeography 'SRID=4326;[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeography '[SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeography '[SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, SRID=4326;Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');

-------------------------------------------------------------------------------
-- typmod
-------------------------------------------------------------------------------

SELECT format_type(oid, -1) FROM (SELECT oid FROM pg_type WHERE typname = 'tgeometry') t;
SELECT format_type(oid, tgeometry_typmod_in(ARRAY[cstring 'Instant','Polygon','5676']))
FROM (SELECT oid FROM pg_type WHERE typname = 'tgeometry') t;

/* Errors */
SELECT tgeometry_typmod_in(ARRAY[cstring 'Instant', NULL,'5676']);
SELECT tgeometry_typmod_in(ARRAY[[cstring 'Instant'],[cstring 'Polygon'],[cstring '5676']]);

-------------------------------------------------------------------------------

SELECT asewkt(tgeometry(Instant) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(Instant, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(Polygon, 4326) 'SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(Instant, Polygon, 4326) 'SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');

SELECT asewkt(tgeography(Polygon,4326) 'SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeography(Instant,Polygon,4326) 'SRID=4326;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');

SELECT asewkt(tgeometry(InstantSet) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(InstantSet, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(Polygon, 4326) 'SRID=4326;{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(InstantSet, Polygon, 4326) 'SRID=4326;{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');

SELECT asewkt(tgeometry(Sequence) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(Sequence, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(Polygon, 4326) 'SRID=4326;[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(Sequence, Polygon, 4326) 'SRID=4326;[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');

SELECT asewkt(tgeometry(SequenceSet) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02], [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(SequenceSet, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02], [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(Polygon, 4326) 'SRID=4326;{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02], [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(SequenceSet, Polygon, 4326) 'SRID=4326;{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02], [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');

/* Errors */
SELECT tgeometry(Instant,Polygon,5676,1234) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Instan,Polygon,5676) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Instant,PolygonZZ,5676) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Instant,Point,5676) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(PolygonZ,5676) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Point,5676) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Instant,PolygonZ) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Instant,Point) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(PolygonZ) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(Point) 'SRID=5676;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01';
SELECT tgeometry(1, 2) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-02}';
/* Errors */
SELECT asewkt(tgeometry(Instant, Polygon, 4326) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(Instant, Polygon, 4326) 'SRID=5434;Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(InstantSet, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(Sequence, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeometry(SequenceSet, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
/* Errors */
SELECT asewkt(tgeometry(Instant, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(InstantSet, Polygon, 4326) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(InstantSet, Polygon, 4326) 'SRID=5434;{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(Sequence, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeometry(SequenceSet, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
/* Errors */
SELECT asewkt(tgeometry(Instant, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(InstantSet, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(Sequence, Polygon, 4326) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(Sequence, Polygon, 4326) 'SRID=5434;[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeometry(SequenceSet, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
/* Errors */
SELECT asewkt(tgeometry(Instant, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(InstantSet, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(Sequence, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(SequenceSet, Polygon, 4326) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeometry(SequenceSet, Polygon, 4326) 'SRID=5434;{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');

-------------------------------------------------------------------------------

SELECT asewkt(tgeography(Instant, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeography(InstantSet, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeography(Sequence, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeography(SequenceSet, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');

/* Errors */
SELECT asewkt(tgeography(InstantSet, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeography(Sequence, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
SELECT asewkt(tgeography(SequenceSet, Polygon) 'Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01');
/* Errors */
SELECT asewkt(tgeography(Instant, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeography(Sequence, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
SELECT asewkt(tgeography(SequenceSet, Polygon) '{Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02}');
/* Errors */
SELECT asewkt(tgeography(Instant, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeography(InstantSet, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
SELECT asewkt(tgeography(SequenceSet, Polygon) '[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02]');
/* Errors */
SELECT asewkt(tgeography(Instant, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeography(InstantSet, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');
SELECT asewkt(tgeography(Sequence, Polygon) '{[Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-01, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-02],
    [Polygon((0 0, 1 0, 1 1, 0 1, 0 0))@2000-01-03, Polygon((5 5, 6 5, 6 6, 5 6, 5 5))@2000-01-04]}');

-------------------------------------------------------------------------------
-- Constructor functions
-------------------------------------------------------------------------------

SELECT asewkt(tgeometryinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'));
SELECT asewkt(tgeometryinst(NULL, '2012-01-01 08:00:00'));
SELECT asewkt(tgeographyinst(ST_MakePolygon('LineString(0 0, 1 0, 0 1, 0 0)'::geometry), '2012-01-01 08:00:00'));
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
