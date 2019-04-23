﻿--SELECT st_makepoint(0.0,0.0);
--select bbox_statistics_validate();
--vacuum analyse tbl_tintinst;
--vacuum analyse tbl_tfloatinst;
--vacuum analyse tbl_tgeompointinst;
--vacuum analyse tbl_tinti;
--vacuum analyse tbl_tfloati;
--vacuum analyse tbl_tgeompointi;
--vacuum analyse tbl_tintseq;
--vacuum analyse tbl_tfloatseq;
--vacuum analyse tbl_tgeompointseq;
--vacuum analyse tbl_tints;
--vacuum analyse tbl_tfloats;
--vacuum analyse tbl_tgeompoints;
--SELECT * FROM execution_stats WHERE PlanRows::text NOT like '%nan' AND abs(PlanRows::text::int - ActualRows::text::int)>10
-- 91/251
-- 80/251
-- 825/2510
-- STATISTICS COLLECTION FUNCTIONS
--SELECT * FROM execution_stats WHERE PlanRows::text = '-nan'
DROP FUNCTION IF EXISTS bbox_statistics_validate;
CREATE OR REPLACE FUNCTION bbox_statistics_validate()
RETURNS XML AS $$
DECLARE
	Query char(5);
	PlanRows xml;
	ActualRows xml;
	QFilter  xml;
	RowsRemovedbyFilter xml;
	J XML;
	StartTime timestamp;
	RandTimestamp timestamptz;
	RandPeriod period;
	RandTimestampset timestampset;
	RandPeriodset periodset;

	Randtintinst tint(TimestampTz);
	Randtinti tint(TimestampSet);
	Randtintseq tint(Period);
	Randtints tint(PeriodSet);

	Randtfloatinst tfloat(TimestampTz);
	Randtfloati tfloat(TimestampSet);
	Randtfloatseq tfloat(Period);
	Randtfloats tfloat(PeriodSet);

	Randtgeompointinst tgeompoint(TimestampTz, Point);
	Randtgeompointi tgeompoint(TimestampSet, Point);
	Randtgeompointseq tgeompoint(Period, Point);
	Randtgeompoints tgeompoint(PeriodSet, Point);

	Randint int;
	Randfloat float;
	Randgeompoint geometry;
	
	k int;	
BEGIN
DROP TABLE IF EXISTS execution_stats;
CREATE TABLE IF NOT EXISTS execution_stats 
(Query char(5), 
StartTime timestamp, 
QFilter xml, 
PlanRows xml, 
ActualRows xml, 
RowsRemovedByFilter xml, 
J XML);


TRUNCATE TABLE execution_stats;

SET log_error_verbosity to terse;
k:= 0;

-----------------------------------------------
---- OPERATOR &&-------------------------------
-----------------------------------------------
--Q1
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q2
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q3
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q4
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q5
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q6
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q7
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q8
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q9
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q10
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q11
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q12
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q13
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q14
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q15
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q16
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q17
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q18
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q19
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q20
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q21
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q22
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q23
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q24
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q25
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintseq
	WHERE seq && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q26
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q27
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q28
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q29
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q30
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q31
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q32
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q33
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q34
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q35
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q36
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q37
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tints
	WHERE ts && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q38
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q39
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts && RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q40
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q41
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q42
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts && RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q43
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q44
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q45
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts && RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q46
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q47
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q48
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts && RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR @>-------------------------------
-----------------------------------------------

--Q49
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q50
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q51
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q52
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst @> RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q53
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q54
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q55
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q56
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q57
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q58
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q59
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q60
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q61
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q62
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q63
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q64
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti @> RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q65
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q66
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q67
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q68
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q69
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q70
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q71
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q72
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q73
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintseq
	WHERE seq @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q74
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q75
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q76
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq @> RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q77
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q78
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q79
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q80
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q81
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q82
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q83
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q84
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q85
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tints
	WHERE ts @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q86
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q87
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q88
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts @> RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q89
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q90
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts @> RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q91
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q92
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q93
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts @> RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q94
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q95
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q96
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts @> RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR <@-------------------------------
-----------------------------------------------
--Q97
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q98
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q99
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q100
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst <@ RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q101
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q102
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q103
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q104
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q105
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q106
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q107
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q108
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q109
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q110
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q111
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q112
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti <@ RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q113
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q114
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q115
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q116
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q117
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q118
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q119
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q120
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q121
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintseq
	WHERE seq <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q122
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q123
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q124
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq <@ RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q125
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q126
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q127
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q128
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q129
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q130
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q131
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q132
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q133
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tints
	WHERE ts <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q134
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q135
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q136
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts <@ RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q137
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q138
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts <@ RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q139
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q140
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q141
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts <@ RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q142
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q143
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q144
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts <@ RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR ~=-------------------------------
-----------------------------------------------
--Q145
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q146
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q147
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q148
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst ~= RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q149
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q150
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q151
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q152
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q153
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q154
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintinst
	WHERE inst ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q155
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q156
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q157
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q158
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q159
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q160
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti ~= RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q161
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q162
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q163
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q164
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q165
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q166
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tinti
	WHERE ti ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q167
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q168
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q169
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintseq
	WHERE seq ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q170
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q171
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q172
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq ~= RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q173
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q174
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q175
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q176
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q177
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q178
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tintseq
	WHERE seq ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q179
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatseq
	WHERE seq ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q180
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointseq
	WHERE seq ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q181
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tints
	WHERE ts ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q182
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q183
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestamp:= random_timestamptz('2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q184
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts ~= RandTimestampset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q185
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q186
k:= k+1;
FOR i IN 1..10 LOOP
	RandTimestampset:= random_timestampset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts ~= RandTimestamp
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q187
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q188
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q189
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriod:= random_period('2000-10-01', '2002-1-31', 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts ~= RandPeriod
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q190
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tints
	WHERE ts ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q191
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloats
	WHERE ts ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q192
k:= k+1;
FOR i IN 1..10 LOOP
	RandPeriodset:= random_periodset('2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompoints
	WHERE ts ~= RandPeriodset
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR &&-------------------------------
-----------------------------------------------
--Q193
k:= k+1;
FOR i IN 1..10 LOOP
	Randtintinst:= random_tintinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst && Randtintinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q194
k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatinst:= random_tfloatinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && Randtfloatinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q195
k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointinst:= random_tgeompointinst(-10, 120, -10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst && Randtgeompointinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR @>-------------------------------
-----------------------------------------------
--Q196
k:= k+1;
FOR i IN 1..10 LOOP
	Randtintinst:= random_tintinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst @> Randtintinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q197
k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatinst:= random_tfloatinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst @> Randtfloatinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q198
k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointinst:= random_tgeompointinst(-10, 120, -10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst @> Randtgeompointinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR <@-------------------------------
-----------------------------------------------
--Q199
k:= k+1;
FOR i IN 1..10 LOOP
	Randtintinst:= random_tintinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst <@ Randtintinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q200
k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatinst:= random_tfloatinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst <@ Randtfloatinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q201
k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointinst:= random_tgeompointinst(-10, 120, -10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst <@ Randtgeompointinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR ~=-------------------------------
-----------------------------------------------
--Q202
k:= k+1;
FOR i IN 1..10 LOOP
	Randtintinst:= random_tintinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst ~= Randtintinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q203
k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatinst:= random_tfloatinst(-10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst ~= Randtfloatinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q204
k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointinst:= random_tgeompointinst(-10, 120, -10, 120, '2000-10-01', '2002-1-31');
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointinst
	WHERE inst ~= Randtgeompointinst
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR &&-------------------------------
-----------------------------------------------
--Q205
k:= k+1;
FOR i IN 1..10 LOOP
	Randtinti:= random_tinti(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst && Randtinti
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q206
k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloati:= random_tfloati(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && Randtfloati
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q207
k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointi:= random_tgeompointi(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && Randtgeompointi
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR @>-------------------------------
-----------------------------------------------
--Q208
k:= k+1;
FOR i IN 1..10 LOOP
	Randtinti:= random_tinti(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti @> Randtinti
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q209
k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloati:= random_tfloati(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> Randtfloati
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--Q210
k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointi:= random_tgeompointi(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> Randtgeompointi
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR <@-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtinti:= random_tinti(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti <@ Randtinti
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloati:= random_tfloati(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ Randtfloati
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointi:= random_tgeompointi(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ Randtgeompointi
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR ~=-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtinti:= random_tinti(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti ~= Randtinti
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloati:= random_tfloati(-10, 120, '2000-10-01', '2002-1-31',10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= Randtfloati
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointi:= random_tgeompointi(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= Randtgeompointi
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR &&-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtintseq:= random_tintseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst && Randtintseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatseq:= random_tfloatseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && Randtfloatseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

--k:= k+1;
--FOR i IN 1..10 LOOP
--	Randtgeompointseq:= random_tgeompointseq(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
--	EXPLAIN (ANALYZE, FORMAT XML)
--	SELECT *
--	FROM tbl_tgeompointi
--	WHERE ti && Randtgeompointseq
--	INTO J;
--
--	StartTime := clock_timestamp();
--	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
--	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
--	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
--	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
--
--	Query:= 'Q' || k;
--	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, NULL);
--END LOOP;


-----------------------------------------------
---- OPERATOR @>-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtintseq:= random_tintseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti @> Randtintseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatseq:= random_tfloatseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> Randtfloatseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointseq:= random_tgeompointseq(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> Randtgeompointseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR <@-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtintseq:= random_tintseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti <@ Randtintseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatseq:= random_tfloatseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ Randtfloatseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointseq:= random_tgeompointseq(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ Randtgeompointseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR ~=-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtintseq:= random_tintseq(-10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti ~= Randtintseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloatseq:= random_tfloatseq(-10, 120, '2000-10-01', '2002-1-31',10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= Randtfloatseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompointseq:= random_tgeompointseq(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= Randtgeompointseq
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR &&-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtints:= random_tints(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst && Randtints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloats:= random_tfloats(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && Randtfloats
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompoints:= random_tgeompoints(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && Randtgeompoints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR @>-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtints:= random_tints(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti @> Randtints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloats:= random_tfloats(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> Randtfloats
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompoints:= random_tgeompoints(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> Randtgeompoints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR <@-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtints:= random_tints(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti <@ Randtints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloats:= random_tfloats(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ Randtfloats
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompoints:= random_tgeompoints(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ Randtgeompoints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR ~=-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randtints:= random_tints(-10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti ~= Randtints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtfloats:= random_tfloats(-10, 120, '2000-10-01', '2002-1-31',10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= Randtfloats
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randtgeompoints:= random_tgeompoints(-10, 120, -10, 120, '2000-10-01', '2002-1-31', 10, 10, 10);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= Randtgeompoints
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR &&-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randint:= random_int(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tintinst
	WHERE inst && Randint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randfloat:= random_float(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloatinst
	WHERE inst && Randfloat
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randgeompoint:= random_geompoint(-10, 120, -10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti && Randgeompoint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;


-----------------------------------------------
---- OPERATOR @>-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randint:= random_int(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti @> Randint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randfloat:= random_float(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti @> Randfloat
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randgeompoint:= random_geompoint(-10, 120, -10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti @> Randgeompoint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR <@-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randint:= random_int(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti <@ Randint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randfloat:= random_float(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti <@ Randfloat
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randgeompoint:= random_geompoint(-10, 120, -10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti <@ Randgeompoint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

-----------------------------------------------
---- OPERATOR ~=-------------------------------
-----------------------------------------------

k:= k+1;
FOR i IN 1..10 LOOP
	Randint:= random_int(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT * 
	FROM tbl_tinti
	WHERE ti ~= Randint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;		
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randfloat:= random_float(-10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tfloati
	WHERE ti ~= Randfloat
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

k:= k+1;
FOR i IN 1..10 LOOP
	Randgeompoint:= random_geompoint(-10, 120, -10, 120);
	EXPLAIN (ANALYZE, FORMAT XML)
	SELECT *
	FROM tbl_tgeompointi
	WHERE ti ~= Randgeompoint
	INTO J;

	StartTime := clock_timestamp();
	PlanRows:= (xpath('/n:explain/n:Query/n:Plan/n:Plan-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	ActualRows:=  (xpath('/n:explain/n:Query/n:Plan/n:Actual-Rows/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	QFilter:=  (xpath('/n:explain/n:Query/n:Plan/n:Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];
	RowsRemovedbyFilter:= (xpath('/n:explain/n:Query/n:Plan/n:Rows-Removed-by-Filter/text()', j, '{{n,http://www.postgresql.org/2009/explain}}'))[1];

	Query:= 'Q' || k;
	INSERT INTO execution_stats VALUES (Query, StartTime, QFilter, PlanRows, ActualRows, RowsRemovedByFilter, J);
END LOOP;

RETURN 'THE END'; 
END;
$$ LANGUAGE 'plpgsql';
