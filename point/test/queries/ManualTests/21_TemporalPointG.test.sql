/*****************************************************************************
 * TPointInst
 *****************************************************************************/
select tgeogpoint(TimestampTz) 'Point(0 0)@2012-01-01 08:00:00';
select tgeogpoint(TimestampTz, Point) 'Point(0 0)@2012-01-01 08:00:00';

select srid(tgeogpoint(TimestampTz) 'Point(0 0)@2012-01-01 08:00:00');
-- 4326
select srid(tgeogpoint(TimestampTz, Point) 'Point(0 0)@2012-01-01 08:00:00');
-- 4326

select tgeogpointinst(ST_Point(0,0), timestamp '2012-01-01 08:00:00');
/*****************************************************************************
 * TPointI
 *****************************************************************************/
select tgeogpoint(TimestampSet) '{Point(0 0)@2012-01-01 08:00:00,
    Point(0 1)@2012-01-01 08:05:00}';
select tgeogpoint(TimestampSet, Point) '{Point(0 0)@2012-01-01 08:00:00,
    Point(0 1)@2012-01-01 08:05:00}';

select tgeogpointi(ARRAY[
tgeogpoint(TimestampTz) 'Point(0 0)@2012-01-01 08:00:00',
tgeogpoint(TimestampTz) 'Point(1 1)@2012-01-01 08:05:00'
]);

select tgeogpointi(ARRAY[
tgeogpointinst(ST_Point(0,0), timestamp '2012-01-01 08:00:00'),
tgeogpointinst(ST_Point(1,1), timestamp '2012-01-01 08:00:05')
]);


/*****************************************************************************
 * TPointSeq
 *****************************************************************************/
select tgeogpoint(Period) 'Point(0 0)->Point(0 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)';
select tgeogpoint(Period, Point) 'Point(0 0)->Point(0 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)';
select tgeogpointseq(ST_Point(0,0), ST_Point(1,1), period '[2012-01-01 08:00:00, 2012-01-01 08:00:05)');


/*****************************************************************************
 * TPointS
 *****************************************************************************/
select tgeogpoint(PeriodSet) '{Point(0 0)->Point(0 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00),
    Point(0 1)->Point(1 1)@[2012-01-01 08:05:00, 2012-01-01 08:15:00)}';

select tgeogpoint(PeriodSet, Point) '{Point(0 0)->Point(0 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00),
    Point(0 1)->Point(1 1)@[2012-01-01 08:05:00, 2012-01-01 08:15:00)}';

select tgeompoints(ARRAY[
tgeompointper 'srid=4326;Point(0 0)->srid=4326;Point(1 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)',
tgeompointper 'srid=4326;Point(1 1)->srid=4326;Point(0 0)@[2012-01-01 08:05:00, 2012-01-01 08:10:00)'
]);

select tgeogpoints(ARRAY[
tgeogpoint(Period) 'Point(0 0)->Point(1 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)',
tgeogpoint(Period) 'Point(1 1)->Point(0 0)@[2012-01-01 08:05:00, 2012-01-01 08:10:00)'
]);
select tgeogpoints(ARRAY[
tgeogpointseq(ST_Point(0,0), ST_Point(1,1), period '[2012-01-01 08:00:00, 2012-01-01 08:00:05)'),
tgeogpointseq(ST_Point(1,1), ST_Point(0,0), period '[2012-01-01 08:00:05, 2012-01-01 08:00:10)')
]);


/*****************************************************************************/
/*
-- Erroneous input

select tgeogpointinst 'Linestring(0 0,1 1)@2012-01-01 08:00:00';

select tgeogpointseq 'Point(0 0)->Linestring(0 1,1 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)';

select tgeogpointseq 'srid=5676;Point(0 0)->Point(0 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)';

select astext(tgeompointper 'srid=4326;Point(0 0)->srid=5676;Point(0 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)');

select tgeompointper(st_Setsrid(st_Point(0,0),4326),st_setsrid(st_Point(0,1),5676),'[2012-01-01 08:00:00, 2012-01-01 08:05:00)');

select tgeompointp(ARRAY[
tgeompointper 'srid=4326;Point(0 0)->Point(1 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)',
tgeompointper 'srid=5676;Point(1 1)->Point(0 0)@[2012-01-01 08:05:00, 2012-01-01 08:10:00)'
]);

select tgeogpoints(ARRAY[
tgeogpointseq(ST_Point(0,0), ST_Point(1,1), period '[2012-01-01 08:00:00, 2012-01-01 08:00:05)'),
tgeogpointseq(ST_Point(1,1), ST_Point(0,0), period '[2012-01-01 08:00:05, 2012-01-01 08:00:10)')
]);

select tgeompoints(ARRAY[
tgeompointper 'srid=4326;Point(0 0)->srid=4326;Point(1 1)@[2012-01-01 08:00:00, 2012-01-01 08:05:00)',
tgeompointper 'srid=4326;Point(1 1)->srid=4326;Point(0 0)@[2012-01-01 08:05:00, 2012-01-01 08:10:00)'
]);

select tgeogpointseq(ST_Point(0,0), ST_Point(1,1), period '[2012-01-01 08:00:00, 2012-01-01 08:00:05)');

select tgeogpointi(ARRAY[
tgeogpointinst(ST_Point(0,0), timestamp '2012-01-01 08:00:00'),
tgeogpointinst(ST_Point(1,1), timestamp '2012-01-01 08:00:05')
]);

*/

/*****************************************************************************/
