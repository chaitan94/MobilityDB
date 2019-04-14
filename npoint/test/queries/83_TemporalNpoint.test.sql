/*****************************************************************************
 * Constructors
 *****************************************************************************/

SELECT tnpointinst('npoint(1,0)'::npoint, '2012-01-01'::timestamp);

SELECT tnpointinst(t1.np, t2.t) FROM tbl_npoint t1, tbl_timestamptz t2;

SELECT tnpointi(ARRAY['npoint(1,0)@2012-01-01'::tnpoint, 'npoint(2,1)@2012-02-01'::tnpoint]);

SELECT tnpointi(array_agg(t.inst ORDER BY t.inst)) FROM tbl_tnpointinst t GROUP BY k%10;

SELECT tnpointseq(ARRAY['npoint(1,0)@2012-01-01'::tnpoint, 'npoint(1,1)@2012-02-01'::tnpoint], true, false);
--SELECT tnpointseq(ARRAY['npoint(1,0)@2012-01-01'::tnpoint, 'npoint(2,1)@2012-02-01'::tnpoint], true, false);  ERROR

SELECT tnpointseq(array_agg(t.inst ORDER BY t.inst)) FROM tbl_tnpointinst t GROUP BY route(t.inst);

SELECT tnpoints(ARRAY[tnpoint '[npoint(1,0)@2012-01-01, npoint(1,1)@2012-02-01]', '[npoint(2,0)@2012-03-01, npoint(2,1)@2012-04-01]']);

/*****************************************************************************
 * Transformation functions
 *****************************************************************************/

SELECT tnpointi(inst) FROM tbl_tnpointinst;
SELECT tnpointseq(inst) FROM tbl_tnpointinst;
SELECT tnpoints(inst) FROM tbl_tnpointinst;
SELECT tnpoints(seq) FROM tbl_tnpointseq;

/******************************************************************************
 * Cast functions
 ******************************************************************************/

SELECT astext(temp::tgeompoint) FROM tbl_tnpoint;

SELECT count(*) FROM tbl_tnpoint WHERE temp = (temp::tgeompoint)::tnpoint;

/*****************************************************************************
 * Accessor Functions
 *****************************************************************************/

SELECT DISTINCT temporalType(temp) FROM tbl_tnpoint;

SELECT MAX(memSize(inst)) FROM tbl_tnpointinst;

SELECT getValue(inst) FROM tbl_tnpointinst;

SELECT MAX(array_length(getValues(temp),1)) FROM tbl_tnpoint;

SELECT MAX(array_length(positions(temp),1)) FROM tbl_tnpoint;

SELECT MAX(route(inst)) FROM tbl_tnpointinst;

SELECT MAX(array_length(routes(temp),1)) FROM tbl_tnpoint;

SELECT MAX(getTimestamp(inst)) FROM tbl_tnpointinst;

SELECT MAX(duration(getTime(temp))) FROM tbl_tnpoint;

SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE t1.temp &= t2.np;

SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE t1.temp @= t2.np;

SELECT count(*) FROM tbl_tnpointinst WHERE everEquals(inst, npoint 'npoint(1553,0.904924)');

SELECT count(*) FROM tbl_tnpointinst WHERE alwaysEquals(inst, npoint 'npoint(1553,0.904924)');

SELECT shift(inst, '1 year'::interval) FROM tbl_tnpointinst;

SELECT MAX(startTimestamp(shift(t1.temp, t2.i))) FROM tbl_tnpoint t1, tbl_interval t2;

SELECT MAX(Route(startValue(temp))) FROM tbl_tnpoint;

SELECT MAX(Route(endValue(temp))) FROM tbl_tnpoint;

SELECT MAX(duration(timespan(temp))) FROM tbl_tnpoint;

SELECT MAX(numInstants(temp)) FROM tbl_tnpoint;

SELECT MAX(Route(startInstant(temp))) FROM tbl_tnpoint;

SELECT MAX(Route(endInstant(temp))) FROM tbl_tnpoint;

SELECT MAX(Route(instantN(temp, 1))) FROM tbl_tnpoint;

SELECT MAX(array_length(instants(temp),1)) FROM tbl_tnpoint;

SELECT MAX(numTimestamps(temp)) FROM tbl_tnpoint;

SELECT MAX(startTimestamp(temp)) FROM tbl_tnpoint;

SELECT MAX(endTimestamp(temp)) FROM tbl_tnpoint;

SELECT MAX(timestampN(temp,1)) FROM tbl_tnpoint;

SELECT MAX(array_length(timestamps(temp),1)) FROM tbl_tnpoint;

SELECT MAX(numSequences(ts)) FROM tbl_tnpoints;

SELECT MAX(NumInstants(startSequence(ts))) FROM tbl_tnpoints;

SELECT MAX(NumInstants(endSequence(ts))) FROM tbl_tnpoints;

SELECT MAX(NumInstants(sequenceN(ts, 1))) FROM tbl_tnpoints;

SELECT MAX(array_length(sequences(ts),1)) FROM tbl_tnpoints;

/*****************************************************************************
 * Restriction Functions
 *****************************************************************************/

SELECT COUNT(*) FROM tbl_tnpoint, tbl_npoint
WHERE atValue(temp, np) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_npoint
WHERE minusValue(temp, np) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, 
( SELECT array_agg(np) AS valuearr FROM tbl_npoint) tmp 
WHERE atValues(temp, valuearr) IS NOT NULL;

SELECT COUNT(*) FROM
( SELECT * FROM tbl_tnpoint limit 10) tbl,
( SELECT array_agg(np) AS valuearr FROM tbl_npoint LIMIT 10) tmp
WHERE minusValues(temp, valuearr) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestamptz
WHERE atTimestamp(temp, t) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestamptz
WHERE minusTimestamp(temp, t) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestamptz
WHERE valueAtTimestamp(temp, t) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestampset
WHERE atTimestampSet(temp, ts) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestampset
WHERE minusTimestampSet(temp, ts) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_period
WHERE atPeriod(temp, p) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_period
WHERE minusPeriod(temp, p) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_periodset
WHERE atPeriodSet(temp, ps) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_periodset
WHERE minusPeriodSet(temp, ps) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestamptz
WHERE intersectsTimestamp(temp, t) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_timestampset
WHERE intersectsTimestampSet(temp, ts) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_period
WHERE intersectsPeriod(temp, p) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_periodset
WHERE intersectsPeriodSet(temp, ps) IS NOT NULL;


SELECT count(*) FROM tbl_tnpoint WHERE atValue(temp, npoint 'npoint(1553,0.904924)') IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE minusValue(temp, npoint 'npoint(1553,0.904924)') IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE atValues(temp, ARRAY[npoint 'npoint(1553,0.904924)', 'npoint(1553,0.904924)']) IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE minusValues(temp, ARRAY[npoint 'npoint(1553,0.904924)', 'npoint(1553,0.904924)']) IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE atTimestamp(temp, timestamp '2012-09-10 02:03:28+02') IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE minusTimestamp(temp, timestamp '2012-09-10 02:03:28+02') IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE valueAtTimestamp(temp, timestamp '2012-09-10 02:03:28+02') IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE atTimestampSet(temp, timestampset '{2012-09-10 02:03:28+02}') IS NOT NULL;

SELECT count(*) FROM tbl_tnpoint WHERE minusTimestampSet(temp, timestampset '{2012-09-10 02:03:28+02}') IS NOT NULL;

SELECT atPeriod(temp, period '[2012-03-01, 2012-06-01]') FROM tbl_tnpoint;

SELECT atPeriodSet(temp, periodset '{[2012-03-01, 2012-06-01), [2012-06-01, 2012-09-01]}') FROM tbl_tnpoint;

SELECT intersectsTimestamp(temp, timestamp '2012-09-10 02:03:28+02') FROM tbl_tnpoint;

SELECT intersectsTimestampSet(temp, timestampset '{2012-09-10 02:03:28+02}') FROM tbl_tnpoint;

SELECT intersectsPeriod(temp, period '[2012-03-01, 2012-06-01]') FROM tbl_tnpoint;

SELECT intersectsPeriodSet(temp, periodset '{[2012-03-01, 2012-06-01), [2012-06-01, 2012-09-01]}') FROM tbl_tnpoint;

/******************************************************************************
 * Comparison functions and B-tree indexing
 ******************************************************************************/

SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp = t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <> t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp < t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <= t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp > t2.temp;
SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp >= t2.temp;

/*****************************************************************************/
