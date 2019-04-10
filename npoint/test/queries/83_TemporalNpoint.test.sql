/*****************************************************************************
 * Constructors
 *****************************************************************************/

SELECT tnpointinst('npoint(1,0)'::npoint, '2012-01-01'::timestamp);

SELECT tnpointi(ARRAY['npoint(1,0)@2012-01-01'::tnpoint, 'npoint(2,1)@2012-02-01'::tnpoint]);

SELECT tnpointseq(ARRAY['npoint(1,0)@2012-01-01'::tnpoint, 'npoint(1,1)@2012-02-01'::tnpoint], true, false);
--SELECT tnpointseq(ARRAY['npoint(1,0)@2012-01-01'::tnpoint, 'npoint(2,1)@2012-02-01'::tnpoint], true, false);  ERROR

SELECT tnpoints(ARRAY['[npoint(1,0)@2012-01-01, npoint(1,1)@2012-02-01]'::tnpoint, '[npoint(2,0)@2012-03-01, npoint(2,1)@2012-04-01]'::tnpoint]);

/*****************************************************************************
 * Cast functions
 *****************************************************************************/

SELECT tnpointi(inst) FROM tbl_tnpointinst;
SELECT tnpointseq(inst) FROM tbl_tnpointinst;
SELECT tnpoints(inst) FROM tbl_tnpointinst;
SELECT tnpoints(seq) FROM tbl_tnpointseq;

/*****************************************************************************
 * TNpointInst
 *****************************************************************************/

SELECT * FROM tbl_tnpointinst;

SELECT MAX(memSize(inst)) FROM tbl_tnpointinst;

SELECT getValue(inst) FROM tbl_tnpointinst;

SELECT MAX(getTimestamp(inst)) FROM tbl_tnpointinst;

SELECT count(*) FROM tbl_tnpointinst WHERE everEquals(inst, npoint 'npoint(1553,0.904924)');

SELECT count(*) FROM tbl_tnpointinst WHERE alwaysEquals(inst, npoint 'npoint(1553,0.904924)');

SELECT shift(inst, '1 year'::interval) FROM tbl_tnpointinst;

SELECT count(*) FROM tbl_tnpointinst WHERE atValue(inst, npoint 'npoint(1553,0.904924)') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE minusValue(inst, npoint 'npoint(1553,0.904924)') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE atValues(inst, ARRAY[npoint 'npoint(1553,0.904924)', 'npoint(1553,0.904924)']) IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE minusValues(inst, ARRAY[npoint 'npoint(1553,0.904924)', 'npoint(1553,0.904924)']) IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE atTimestamp(inst, timestamp '2012-09-10 02:03:28+02') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE minusTimestamp(inst, timestamp '2012-09-10 02:03:28+02') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE valueAtTimestamp(inst, timestamp '2012-09-10 02:03:28+02') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE atTimestampSet(inst, timestampset '{2012-09-10 02:03:28+02}') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointinst WHERE minusTimestampSet(inst, timestampset '{2012-09-10 02:03:28+02}') IS NOT NULL;

SELECT atPeriod(inst, period '[2012-03-01, 2012-06-01]') FROM tbl_tnpointinst;

SELECT atPeriodSet(inst, periodset '{[2012-03-01, 2012-06-01), [2012-06-01, 2012-09-01]}') FROM tbl_tnpointinst;

SELECT intersectsTimestamp(inst, timestamp '2012-09-10 02:03:28+02') FROM tbl_tnpointinst;

SELECT intersectsTimestampSet(inst, timestampset '{2012-09-10 02:03:28+02}') FROM tbl_tnpointinst;

SELECT intersectsPeriod(inst, period '[2012-03-01, 2012-06-01]') FROM tbl_tnpointinst;

SELECT intersectsPeriodSet(inst, periodset '{[2012-03-01, 2012-06-01), [2012-06-01, 2012-09-01]}') FROM tbl_tnpointinst;

SELECT count(*) FROM tbl_tnpointinst t1, tbl_tnpointinst t2 WHERE t1.inst = t2.inst;
SELECT count(*) FROM tbl_tnpointinst t1, tbl_tnpointinst t2 WHERE t1.inst <> t2.inst;
SELECT count(*) FROM tbl_tnpointinst t1, tbl_tnpointinst t2 WHERE t1.inst < t2.inst;
SELECT count(*) FROM tbl_tnpointinst t1, tbl_tnpointinst t2 WHERE t1.inst <= t2.inst;
SELECT count(*) FROM tbl_tnpointinst t1, tbl_tnpointinst t2 WHERE t1.inst > t2.inst;
SELECT count(*) FROM tbl_tnpointinst t1, tbl_tnpointinst t2 WHERE t1.inst >= t2.inst;

/*****************************************************************************
 * TNpointI
 *****************************************************************************/

SELECT * FROM tbl_tnpointi;

SELECT memSize(ti) FROM tbl_tnpointi;

SELECT getValues(ti) FROM tbl_tnpointi;

SELECT startValue(ti) FROM tbl_tnpointi;

SELECT endValue(ti) FROM tbl_tnpointi;

SELECT getTime(ti) FROM tbl_tnpointi;

SELECT timespan(ti) FROM tbl_tnpointi;

SELECT numInstants(ti) FROM tbl_tnpointi;

SELECT startInstant(ti) FROM tbl_tnpointi;

SELECT endInstant(ti) FROM tbl_tnpointi;

SELECT instantN(ti, 1) FROM tbl_tnpointi;

SELECT instants(ti) FROM tbl_tnpointi;

SELECT numTimestamps(ti) FROM tbl_tnpointi;

SELECT startTimestamp(ti) FROM tbl_tnpointi;

SELECT endTimestamp(ti) FROM tbl_tnpointi;

SELECT timestampN(ti, 1) FROM tbl_tnpointi;

SELECT timestamps(ti) FROM tbl_tnpointi;

SELECT ti &= npoint 'npoint(868,0.900912)' FROM tbl_tnpointi;

SELECT ti @= npoint 'npoint(868,0.900912)' FROM tbl_tnpointi;

SELECT shift(ti, '1 day'::interval) FROM tbl_tnpointi;

SELECT atValue(ti, npoint 'npoint(868,0.900912)') FROM tbl_tnpointi;

SELECT minusValue(ti, npoint 'npoint(868,0.900912)') FROM tbl_tnpointi;

SELECT atValues(ti, ARRAY[npoint 'npoint(868,0.900912)', 'npoint(340,0.457458)']) FROM tbl_tnpointi;

SELECT minusValues(ti, ARRAY[npoint 'npoint(868,0.900912)', 'npoint(340,0.457458)']) FROM tbl_tnpointi;

SELECT count(*) FROM tbl_tnpointi WHERE atTimestamp(ti, timestamp '2012-02-09 05:50:35+01') IS NOT NULL;

SELECT minusTimestamp(ti, timestamp '2012-02-09 05:50:35+01') FROM tbl_tnpointi;

SELECT valueAtTimestamp(ti, timestamp '2012-02-09 05:50:35+01') FROM tbl_tnpointi;

SELECT count(*) FROM tbl_tnpointi WHERE atTimestampSet(ti, timestampset '{2012-02-09 05:50:35+01, 2012-03-08 09:37:40+01}') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointi WHERE minusTimestampSet(ti, timestampset '{2012-02-09 05:50:35+01, 2012-03-08 09:37:40+01}') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointi WHERE atPeriod(ti, period '[2012-02-01, 2012-09-01]') IS NOT NULL;

SELECT count(*) FROM tbl_tnpointi WHERE atPeriodSet(ti, periodset '{[2012-02-01, 2012-06-01), [2012-06-01, 2012-09-01]}') IS NOT NULL;

SELECT intersectsTimestamp(ti, '2012-02-09 05:50:35+01'::timestamp) FROM tbl_tnpointi;

SELECT intersectsTimestampSet(ti, timestampset(ARRAY['2012-02-09 05:50:35+01'::timestamp, '2012-03-08 09:37:40+01'::timestamp])) FROM tbl_tnpointi;

SELECT intersectsPeriod(ti, '[2012-02-01, 2012-09-01]'::period) FROM tbl_tnpointi;

SELECT intersectsPeriodSet(ti, periodset(ARRAY['[2012-02-01, 2012-06-01)'::period, '[2012-06-01, 2012-09-01]'::period])) FROM tbl_tnpointi;

SELECT count(*) FROM tbl_tnpointi t1, tbl_tnpointi t2 WHERE t1.ti = t2.ti;
SELECT count(*) FROM tbl_tnpointi t1, tbl_tnpointi t2 WHERE t1.ti <> t2.ti;
SELECT count(*) FROM tbl_tnpointi t1, tbl_tnpointi t2 WHERE t1.ti < t2.ti;
SELECT count(*) FROM tbl_tnpointi t1, tbl_tnpointi t2 WHERE t1.ti <= t2.ti;
SELECT count(*) FROM tbl_tnpointi t1, tbl_tnpointi t2 WHERE t1.ti > t2.ti;
SELECT count(*) FROM tbl_tnpointi t1, tbl_tnpointi t2 WHERE t1.ti >= t2.ti;

/*****************************************************************************
 * TNpointSeq
 *****************************************************************************/

SELECT * FROM tbl_tnpointseq;

SELECT memSize(seq) FROM tbl_tnpointseq;

SELECT getValues(seq) FROM tbl_tnpointseq;

SELECT startValue(seq) FROM tbl_tnpointseq;

SELECT endValue(seq) FROM tbl_tnpointseq;

SELECT getTime(seq) FROM tbl_tnpointseq;

SELECT duration(seq) FROM tbl_tnpointseq;

SELECT numInstants(seq) FROM tbl_tnpointseq;

SELECT startInstant(seq) FROM tbl_tnpointseq;

SELECT endInstant(seq) FROM tbl_tnpointseq;

SELECT instantN(seq, 1) FROM tbl_tnpointseq;

SELECT instants(seq) FROM tbl_tnpointseq;

SELECT numTimestamps(seq) FROM tbl_tnpointseq;

SELECT startTimestamp(seq) FROM tbl_tnpointseq;

SELECT endTimestamp(seq) FROM tbl_tnpointseq;

SELECT timestampN(seq, 1) FROM tbl_tnpointseq;

SELECT timestamps(seq) FROM tbl_tnpointseq;

SELECT count(*) FROM tbl_tnpointseq WHERE seq &= 'npoint(325,0.5)';

SELECT count(*) FROM tbl_tnpointseq WHERE seq @= 'npoint(325,0.5)';

SELECT shift(seq, '1 day'::interval) FROM tbl_tnpointseq;

SELECT atValue(seq, npoint 'npoint(325,0.5)') FROM tbl_tnpointseq;

SELECT minusValue(seq, npoint 'npoint(325,0.5)') FROM tbl_tnpointseq;

SELECT atValues(seq, ARRAY[npoint 'npoint(325,0.5)', 'npoint(325,0.4)']) FROM tbl_tnpointseq;

SELECT minusValues(seq, ARRAY[npoint 'npoint(325,0.5)', 'npoint(325,0.4)']) FROM tbl_tnpointseq;

SELECT atTimestamp(seq, timestamp '2012-05-01') FROM tbl_tnpointseq;

SELECT minusTimestamp(seq, timestamp '2012-05-01') FROM tbl_tnpointseq;

SELECT valueAtTimestamp(seq, '2012-05-01'::timestamp) FROM tbl_tnpointseq;

SELECT atTimestampSet(seq, timestampset '{2012-05-01, 2012-09-01}') FROM tbl_tnpointseq;

SELECT minusTimestampSet(seq, timestampset '{2012-05-01, 2012-09-01}') FROM tbl_tnpointseq;

SELECT atPeriod(seq, '[2012-02-01, 2012-09-01]'::period) FROM tbl_tnpointseq;

SELECT atPeriodSet(seq, periodset(ARRAY['[2012-02-01, 2012-06-01)'::period, '[2012-06-01, 2012-09-01]'::period])) FROM tbl_tnpointseq;

SELECT intersectsTimestamp(seq, '2012-05-01'::timestamp) FROM tbl_tnpointseq;

SELECT intersectsTimestampSet(seq, timestampset(ARRAY['2012-05-01'::timestamp, '2012-09-01'::timestamp])) FROM tbl_tnpointseq;

SELECT intersectsPeriod(seq, '[2012-02-01, 2012-09-01]'::period) FROM tbl_tnpointseq;

SELECT intersectsPeriodSet(seq, periodset(ARRAY['[2012-02-01, 2012-06-01)'::period, '[2012-06-01, 2012-09-01]'::period])) FROM tbl_tnpointseq;

SELECT count(*) FROM tbl_tnpointseq t1, tbl_tnpointseq t2 WHERE t1.seq = t2.seq;
SELECT count(*) FROM tbl_tnpointseq t1, tbl_tnpointseq t2 WHERE t1.seq <> t2.seq;
SELECT count(*) FROM tbl_tnpointseq t1, tbl_tnpointseq t2 WHERE t1.seq < t2.seq;
SELECT count(*) FROM tbl_tnpointseq t1, tbl_tnpointseq t2 WHERE t1.seq <= t2.seq;
SELECT count(*) FROM tbl_tnpointseq t1, tbl_tnpointseq t2 WHERE t1.seq > t2.seq;
SELECT count(*) FROM tbl_tnpointseq t1, tbl_tnpointseq t2 WHERE t1.seq >= t2.seq;

/*****************************************************************************
 * TNpointS
 *****************************************************************************/

SELECT * FROM tbl_tnpoints;

SELECT memSize(ts) FROM tbl_tnpoints;

SELECT getValues(ts) FROM tbl_tnpoints;

SELECT startValue(ts) FROM tbl_tnpoints;

SELECT endValue(ts) FROM tbl_tnpoints;

SELECT getTime(ts) FROM tbl_tnpoints;

SELECT timespan(ts) FROM tbl_tnpoints;

SELECT duration(ts) FROM tbl_tnpoints;

SELECT numSequences(ts) FROM tbl_tnpoints;

SELECT startSequence(ts) FROM tbl_tnpoints;

SELECT endSequence(ts) FROM tbl_tnpoints;

SELECT SequenceN(ts, 1) FROM tbl_tnpoints;

SELECT sequences(ts) FROM tbl_tnpoints;

SELECT numInstants(ts) FROM tbl_tnpoints;

SELECT startInstant(ts) FROM tbl_tnpoints;

SELECT endInstant(ts) FROM tbl_tnpoints;

SELECT instantN(ts, 1) FROM tbl_tnpoints;

SELECT instants(ts) FROM tbl_tnpoints;

SELECT numTimestamps(ts) FROM tbl_tnpoints;

SELECT startTimestamp(ts) FROM tbl_tnpoints;

SELECT endTimestamp(ts) FROM tbl_tnpoints;

SELECT timestampN(ts, 1) FROM tbl_tnpoints;

SELECT timestamps(ts) FROM tbl_tnpoints;

SELECT ts &= npoint 'npoint(1038,0.5)' FROM tbl_tnpoints;

SELECT ts @= npoint 'npoint(1038,0.5)' FROM tbl_tnpoints;

SELECT shift(ts, '1 day'::interval) FROM tbl_tnpoints;

-- SELECT isValueContinuous(ts) FROM tbl_tnpoints;

-- SELECT isTimeContinuous(ts) FROM tbl_tnpoints;

SELECT atValue(ts, npoint 'npoint(1038,0.5)') FROM tbl_tnpoints;

SELECT minusValue(ts, npoint 'npoint(1038,0.5)') FROM tbl_tnpoints;

SELECT atValues(ts, ARRAY[npoint 'npoint(1038,0.5)', 'npoint(1038,0.2)']) FROM tbl_tnpoints;

SELECT minusValues(ts, ARRAY[npoint 'npoint(1038,0.5)', 'npoint(1038,0.2)']) FROM tbl_tnpoints;

SELECT atTimestamp(ts, '2012-05-01'::timestamp) FROM tbl_tnpoints;

SELECT minusTimestamp(ts, '2012-05-01'::timestamp) FROM tbl_tnpoints;

SELECT valueAtTimestamp(ts, '2012-05-01'::timestamp) FROM tbl_tnpoints;

SELECT atTimestampSet(ts, timestampset(ARRAY['2012-05-01'::timestamp, '2012-09-01'::timestamp])) FROM tbl_tnpoints;

SELECT minusTimestampSet(ts, timestampset(ARRAY['2012-05-01'::timestamp, '2012-09-01'::timestamp])) FROM tbl_tnpoints;

SELECT atPeriod(ts, '[2012-02-01, 2012-09-01]'::period) FROM tbl_tnpoints;

SELECT atPeriodSet(ts, periodset(ARRAY['[2012-02-01, 2012-06-01)'::period, '[2012-06-01, 2012-09-01]'::period])) FROM tbl_tnpoints;

SELECT intersectsTimestamp(ts, '2012-05-01'::timestamp) FROM tbl_tnpoints;

SELECT intersectsTimestampSet(ts, timestampset(ARRAY['2012-05-01'::timestamp, '2012-09-01'::timestamp])) FROM tbl_tnpoints;

SELECT intersectsPeriod(ts, '[2012-02-01, 2012-09-01]'::period) FROM tbl_tnpoints;

SELECT intersectsPeriodSet(ts, periodset(ARRAY['[2012-02-01, 2012-06-01)'::period, '[2012-06-01, 2012-09-01]'::period])) FROM tbl_tnpoints;

SELECT count(*) FROM tbl_tnpoints t1, tbl_tnpoints t2 WHERE t1.ts = t2.ts;
SELECT count(*) FROM tbl_tnpoints t1, tbl_tnpoints t2 WHERE t1.ts <> t2.ts;
SELECT count(*) FROM tbl_tnpoints t1, tbl_tnpoints t2 WHERE t1.ts < t2.ts;
SELECT count(*) FROM tbl_tnpoints t1, tbl_tnpoints t2 WHERE t1.ts <= t2.ts;
SELECT count(*) FROM tbl_tnpoints t1, tbl_tnpoints t2 WHERE t1.ts > t2.ts;
SELECT count(*) FROM tbl_tnpoints t1, tbl_tnpoints t2 WHERE t1.ts >= t2.ts;

/******************************************************************************/
