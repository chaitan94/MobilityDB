/*****************************************************************************
 * Constructors
 *****************************************************************************/

SELECT tnpointinst(t1.np, t2.t) FROM tbl_npoint t1, tbl_timestamptz t2;

SELECT tnpointi(array_agg(t.inst ORDER BY t.inst)) FROM tbl_tnpointinst t GROUP BY k%10;

SELECT tnpointseq(array_agg(t.inst ORDER BY t.inst)) FROM tbl_tnpointinst t GROUP BY route(t.inst);

SELECT tnpoints(array_agg(t.seq ORDER BY startTimestamp(t.seq))) FROM tbl_tnpointseq t GROUP BY k%10;

/******************************************************************************
 * Transformation functions
 ******************************************************************************/

SELECT DISTINCT temporalType(tnpointinst(inst)) FROM tbl_tnpointinst;
SELECT DISTINCT temporalType(tnpointi(inst)) FROM tbl_tnpointinst;
SELECT DISTINCT temporalType(tnpointseq(inst)) FROM tbl_tnpointinst;
SELECT DISTINCT temporalType(tnpoints(inst)) FROM tbl_tnpointinst;

/******************************************************************************/

SELECT DISTINCT temporalType(tnpointinst(ti)) FROM tbl_tnpointi WHERE numInstants(ti) = 1;
SELECT DISTINCT temporalType(tnpointi(ti)) FROM tbl_tnpointi;
SELECT DISTINCT temporalType(tnpointseq(ti)) FROM tbl_tnpointi WHERE numInstants(ti) = 1;
SELECT DISTINCT temporalType(tnpoints(ti)) FROM tbl_tnpointi;

/******************************************************************************/

SELECT DISTINCT temporalType(tnpointinst(seq)) FROM tbl_tnpointseq WHERE numInstants(seq) = 1;
SELECT DISTINCT temporalType(tnpointi(seq)) FROM tbl_tnpointseq WHERE numInstants(seq) = 1;
SELECT DISTINCT temporalType(tnpointseq(seq)) FROM tbl_tnpointseq;
SELECT DISTINCT temporalType(tnpoints(seq)) FROM tbl_tnpointseq;

/******************************************************************************/

SELECT DISTINCT temporalType(tnpointinst(ts)) FROM tbl_tnpoints WHERE numInstants(ts) = 1;
SELECT DISTINCT temporalType(tnpointi(ts)) FROM tbl_tnpoints WHERE duration(ts) = '00:00:00';
SELECT DISTINCT temporalType(tnpointseq(ts)) FROM tbl_tnpoints WHERE numSequences(ts) = 1;
SELECT DISTINCT temporalType(tnpoints(ts)) FROM tbl_tnpoints;

/******************************************************************************
 * Cast functions
 ******************************************************************************/

SELECT astext(temp::tgeompoint) FROM tbl_tnpoint;

SELECT count(*) FROM tbl_tnpoint WHERE temp = (temp::tgeompoint)::tnpoint;

/******************************************************************************
 * Accessor functions
 ******************************************************************************/

SELECT DISTINCT temporalType(temp) FROM tbl_tnpoint;

SELECT MAX(memSize(temp)) FROM tbl_tnpoint;

/*
SELECT gbox(temp) FROM tbl_tnpoint;
*/

SELECT getValue(inst) FROM tbl_tnpointinst ORDER BY getValue(inst) LIMIT 1;

SELECT MAX(array_length(getValues(temp), 1)) FROM tbl_tnpoint;

SELECT MAX(array_length(positions(temp), 1)) FROM tbl_tnpoint;

SELECT MAX(route(inst)) FROM tbl_tnpointinst;

SELECT MAX(array_length(routes(temp), 1)) FROM tbl_tnpoint;

SELECT MAX(duration(getTime(temp))) FROM tbl_tnpoint;

SELECT MAX(getTimestamp(inst)) FROM tbl_tnpointinst;

SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE t1.temp &= t2.np;

SELECT count(*) FROM tbl_tnpoint t1, tbl_npoint t2 WHERE t1.temp @= t2.np;

SELECT count(*) FROM tbl_tnpointinst t1, tbl_npoint t2 WHERE everEquals(t1.inst, t2.np);

SELECT count(*) FROM tbl_tnpointinst t1, tbl_npoint t2 WHERE alwaysEquals(t1.inst, t2.np);

SELECT MAX(startTimestamp(shift(t1.temp, t2.i))) FROM tbl_tnpoint t1, tbl_interval t2;

SELECT DISTINCT startValue(temp) FROM tbl_tnpoint;

SELECT DISTINCT endValue(temp) FROM tbl_tnpoint;

SELECT MAX(duration(timespan(temp))) FROM tbl_tnpoint;

SELECT MAX(duration(temp)) FROM tbl_tnpoint;

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

SELECT MAX(duration(startSequence(ts))) FROM tbl_tnpoints;

SELECT MAX(duration(endSequence(ts))) FROM tbl_tnpoints;

SELECT MAX(duration(sequenceN(ts, numSequences(ts)))) FROM tbl_tnpoints;

SELECT MAX(array_length(sequences(ts),1)) FROM tbl_tnpoints;

/******************************************************************************
 * Restriction functions
 ******************************************************************************/

SELECT COUNT(*) FROM tbl_tnpoint, tbl_npoint 
WHERE atValue(temp, np) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, tbl_npoint 
WHERE minusValue(temp, np) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, 
( SELECT array_agg(np) AS valuearr FROM tbl_npoint) tmp 
WHERE atValues(temp, valuearr) IS NOT NULL;

SELECT COUNT(*) FROM tbl_tnpoint, 
( SELECT array_agg(np) AS valuearr FROM tbl_npoint) tmp 
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

/******************************************************************************
 * Comparison functions and B-tree indexing
 ******************************************************************************/

SELECT COUNT(*) FROM tbl_tnpoint t1, tbl_tnpoint t2
WHERE t1.temp = t2.temp;
SELECT COUNT(*) FROM tbl_tnpoint t1, tbl_tnpoint t2
WHERE t1.temp <> t2.temp;
SELECT COUNT(*) FROM tbl_tnpoint t1, tbl_tnpoint t2
WHERE t1.temp < t2.temp;
SELECT COUNT(*) FROM tbl_tnpoint t1, tbl_tnpoint t2
WHERE t1.temp <= t2.temp;
SELECT COUNT(*) FROM tbl_tnpoint t1, tbl_tnpoint t2
WHERE t1.temp > t2.temp;
SELECT COUNT(*) FROM tbl_tnpoint t1, tbl_tnpoint t2
WHERE t1.temp >= t2.temp;

------------------------------------------------------------------------------
