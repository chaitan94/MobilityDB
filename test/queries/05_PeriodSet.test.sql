﻿-------------------------------------------------------------------------------
-- Tests for period set data type.
-- File PeriodSet.c
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Constructor
-------------------------------------------------------------------------------

SELECT periodset(ARRAY [period '[2000-01-01, 2000-01-02]', '[2000-01-03,2000-01-04]']);
-- Error
SELECT periodset(ARRAY [period '[2000-01-01, 2000-01-03]', '[2000-01-02,2000-01-04]']);
SELECT periodset('{}'::period[]);

-------------------------------------------------------------------------------
-- Casting
-------------------------------------------------------------------------------

SELECT periodset(timestamptz '2000-01-01');
SELECT periodset(timestampset '{2000-01-01,2000-01-02}');
SELECT periodset(period '[2000-01-01,2000-01-02]');

SELECT timestamptz '2000-01-01'::periodset;
SELECT timestampset '{2000-01-01,2000-01-02}'::periodset;
SELECT period '[2000-01-01,2000-01-02]'::periodset;

-------------------------------------------------------------------------------
-- Functions
-------------------------------------------------------------------------------

SELECT memSize(periodset '{[2000-01-01,2000-01-01]}');
SELECT memSize(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT memSize(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT memSize(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT memSize(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT memSize(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT timespan(periodset '{[2000-01-01,2000-01-01]}');
SELECT timespan(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT timespan(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT timespan(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT timespan(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT timespan(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT duration(periodset '{[2000-01-01,2000-01-01]}');
SELECT duration(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT duration(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT duration(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT duration(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT duration(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT numPeriods(periodset '{[2000-01-01,2000-01-01]}');
SELECT numPeriods(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT numPeriods(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT numPeriods(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT numPeriods(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT numPeriods(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT startPeriod(periodset '{[2000-01-01,2000-01-01]}');
SELECT startPeriod(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT startPeriod(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT startPeriod(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT startPeriod(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT startPeriod(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT endPeriod(periodset '{[2000-01-01,2000-01-01]}');
SELECT endPeriod(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT endPeriod(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT endPeriod(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT endPeriod(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT endPeriod(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT periodN(periodset '{[2000-01-01,2000-01-01]}', 1);
SELECT periodN(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}', 1);
SELECT periodN(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}', 2);
SELECT periodN(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}', 3);
SELECT periodN(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}', 4);
SELECT periodN(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}', 0);

SELECT periods(periodset '{[2000-01-01,2000-01-01]}');
SELECT periods(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT periods(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT periods(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT periods(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT periods(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT numTimestamps(periodset '{[2000-01-01,2000-01-01]}');
SELECT numTimestamps(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT numTimestamps(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT numTimestamps(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT numTimestamps(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT numTimestamps(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT startTimestamp(periodset '{[2000-01-01,2000-01-01]}');
SELECT startTimestamp(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT startTimestamp(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT startTimestamp(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT startTimestamp(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT startTimestamp(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT endTimestamp(periodset '{[2000-01-01,2000-01-01]}');
SELECT endTimestamp(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT endTimestamp(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT endTimestamp(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT endTimestamp(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT endTimestamp(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT timestampN(periodset '{[2000-01-01,2000-01-01]}', 1);
SELECT timestampN(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}', 1);
SELECT timestampN(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}', 2);
SELECT timestampN(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}', 3);
SELECT timestampN(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}', 4);
SELECT timestampN(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}', 0);
SELECT timestampN(periodset '{[2000-01-01,2000-01-01],[2000-01-02,2000-01-02]}',3);

SELECT timestamps(periodset '{[2000-01-01,2000-01-01]}');
SELECT timestamps(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT timestamps(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT timestamps(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}');
SELECT timestamps(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');
SELECT timestamps(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}');

SELECT shift(periodset '{[2000-01-01,2000-01-01]}', '5 min');
SELECT shift(periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}', '5 min');
SELECT shift(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}', '5 min');
SELECT shift(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06)}', '5 min');
SELECT shift(periodset '{(2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}', '5 min');
SELECT shift(periodset '{[2000-01-01,2000-01-02),(2000-01-03,2000-01-04),(2000-01-05,2000-01-06]}', '5 min');

SELECT periodset_cmp(periodset '{[2000-01-01,2000-01-01]}', periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}');
SELECT periodset '{[2000-01-01,2000-01-01]}' = periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}';
SELECT periodset '{[2000-01-01,2000-01-01]}' <> periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}';
SELECT periodset '{[2000-01-01,2000-01-01]}' < periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}';
SELECT periodset '{[2000-01-01,2000-01-02]}' < periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' < periodset '{[2000-01-01,2000-01-02]}';
SELECT periodset '{[2000-01-01,2000-01-01]}' <= periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}';
SELECT periodset '{[2000-01-01,2000-01-01]}' > periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}';
SELECT periodset '{[2000-01-01,2000-01-01]}' >= periodset '{(2000-01-01,2000-01-02),(2000-01-02,2000-01-03),(2000-01-03,2000-01-04)}';

-------------------------------------------------------------------------------
