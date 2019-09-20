/******************************************************************************
 * TemporalInst
 ******************************************************************************/		

select ttextarrinst('{A, B}','2001-01-01');

select getValue(ttextarrinst('{A, B}','2001-01-01'));

select getTime(ttextarrinst('{A, B}','2001-01-01'));

select getTimestamp(ttextarrinst('{A, B}','2001-01-01'));

select ttextarrinst('{A, B}','2001-01-01') &= '{A, B}';

select ttextarrinst('{A, B}','2001-01-01') &= '{A}';

select ttextarrinst('{A, B}','2001-01-01') @= '{A, B}';

select atValue(ttextarrinst('{A, B}','2001-01-01'), '{A, B}');

select atValue(ttextarrinst('{A, B}','2001-01-01'), '{A}');

select atTimestamp(ttextarrinst('{A, B}','2001-01-01'), '2001-01-01');

select atTimestamp(ttextarrinst('{A, B}','2001-01-01'), '2001-01-02');

select valueAtTimestamp(ttextarrinst('{A, B}','2001-01-01'), '2001-01-01');

select valueAtTimestamp(ttextarrinst('{A, B}','2001-01-01'), '2001-01-02');

select atTimestampSet(ttextarrinst('{A, B}','2001-01-01'), timestampset '{2001-01-01, 2001-01-02}');

select atTimestampSet(ttextarrinst('{A, B}','2001-01-01'), timestampset '{2001-01-02, 2001-01-03}');

select atPeriod(ttextarrinst('{A, B}','2001-01-02'), '[2001-01-01, 2001-01-03)');

select atPeriod(ttextarrinst('{A, B}','2001-01-02'), '[2001-01-03, 2001-01-04)');

select atPeriodSet(ttextarrinst('{A, B}','2001-01-01'), periodset '{[2001-01-01, 2001-01-02), [2001-01-03, 2001-01-04)}');

select atPeriodSet(ttextarrinst('{A, B}','2001-01-01'), periodset '{[2001-01-03, 2001-01-04), [2001-01-05, 2001-01-06)}');

select intersectsTimestamp(ttextarrinst('{A, B}','2001-01-01'), '2001-01-01');

select intersectsTimestamp(ttextarrinst('{A, B}','2001-01-01'), '2001-01-02');

select intersectsTimestampSet(ttextarrinst('{A, B}','2001-01-01'), timestampset '{2001-01-01, 2001-01-02}');

select intersectsTimestampSet(ttextarrinst('{A, B}','2001-01-01'), timestampset '{2001-01-02, 2001-01-03}');

select intersectsPeriod(ttextarrinst('{A, B}','2001-01-02'), '[2001-01-01, 2001-01-03)');

select intersectsPeriod(ttextarrinst('{A, B}','2001-01-02'), '[2001-01-03, 2001-01-04)');

select intersectsPeriodSet(ttextarrinst('{A, B}','2001-01-01'), periodset '{[2001-01-01, 2001-01-02), [2001-01-03, 2001-01-04)}');

select intersectsPeriodSet(ttextarrinst('{A, B}','2001-01-01'), periodset '{[2001-01-03, 2001-01-04), [2001-01-05, 2001-01-06)}');

-------------------------------------------------------------------------------
-- Comparison functions and B-tree indexing
-------------------------------------------------------------------------------

select ttextarrinst('{AA,BB}', '2001-01-01') =
	ttextarrinst('{AA,BB}', '2001-01-01');

select ttextarrinst('{AA,BB}', '2001-01-01') <
	ttextarrinst('{AA,BB}', '2001-01-01');

WITH Values(value) AS(
select ttextarrinst('{AA,BB}', '2001-01-02') union
select ttextarrinst('{AA}', '2001-01-02') union
select ttextarrinst('{BB}', '2001-01-02') union
select ttextarrinst('{A}', '2001-01-02') union
select ttextarrinst('{ZZZZ}', '2001-01-01')
)
select *
from Values
order by value;

/******************************************************************************
 * TemporalI
 ******************************************************************************/		

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]);

select getValues(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select startValue(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select endValue(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select getTime(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select timespan(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select numInstants(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select startInstant(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select endInstant(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select instantN(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),1);

select instantN(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),2);

select instantN(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),3);

select instants(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select numTimestamps(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select startTimestamp(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select endTimestamp(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select timestampN(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),1);

select timestampN(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),2);

select timestampN(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),3);

select timestamps(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]));

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]) &= '{A, B}';

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]) &= '{A}';

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01')]) @= '{A, B}';

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]) @= '{A, B}';

select atValue(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'{A, B}');

select atValue(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'{A}');

select atTimestamp(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'2001-01-02');

select atTimestamp(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'2001-01-03');

select atTimestampSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'{2001-01-01, 2001-01-03}');

select atTimestampSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'{2001-01-03, 2001-01-04}');

select atPeriod(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'[2001-01-02, 2001-01-04)');

select atPeriod(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'[2001-01-03, 2001-01-04)');

select atPeriodSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'{[2001-01-02, 2001-01-03), [2001-01-03, 2001-01-04)}');

select atPeriodSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'{[2001-01-03, 2001-01-04), [2001-01-04, 2001-01-05)}');

select intersectsTimestamp(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'2001-01-02');

select intersectsTimestamp(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),'2001-01-03');

select intersectsTimestampSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]), '{2001-01-02,2001-01-03}');

select intersectsTimestampSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]), '{2001-01-03,2001-01-04}');

select intersectsPeriod(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]), '[2001-01-02,2001-01-03)');

select intersectsPeriod(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]), '[2001-01-03,2001-01-04)');

select intersectsPeriodSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),
'{[2001-01-01,2001-01-02),[2001-01-05,2001-01-06)}');

select intersectsPeriodSet(ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]),
'{[2001-01-03,2001-01-04),[2001-01-05,2001-01-06)}');

-------------------------------------------------------------------------------
-- Comparison functions and B-tree indexing
-------------------------------------------------------------------------------

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]) =
ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]);

select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]) <
ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')]);

WITH Values(value) AS(
select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-02'),
ttextarrinst('{B, C}','2001-01-03')]) union
select ttextarri(ARRAY[
ttextarrinst('{AA, B}','2001-01-02'),
ttextarrinst('{B, C}','2001-01-03')]) union
select ttextarri(ARRAY[
ttextarrinst('{A, B}','2001-01-02'),
ttextarrinst('{BB, C}','2001-01-03')]) union
select ttextarri(ARRAY[
ttextarrinst('{Z, B}','2001-01-01'),
ttextarrinst('{B, C}','2001-01-02')])
)
select *
from Values
order by value;

/******************************************************************************
 * TemporalSeq
 ******************************************************************************/		

select ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)';

select getValueS(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)');

select getTime(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)')	;

select duration(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)');

select startTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)');

select endTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)');

select startInstant(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)');

select endInstant(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)');

select ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)' &= '{A, B}';

select ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)' &= '{A}';

select ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)' @= '{A, B}';

select ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)' @= '{A}';

select atValue(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '{A, B}');

select atValue(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '{A}');

select atTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '2001-01-01');

select atTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '2001-01-02');

select atTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '2001-01-03');

select valueAtTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '2001-01-01');

select valueAtTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '2001-01-02');

select valueAtTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '2001-01-03');

select atTimestampSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '{2001-01-01, 2001-01-02}');

select atTimestampSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '{2001-01-02, 2001-01-03}');

select atPeriod(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '[2001-01-01, 2001-01-02)');

select atPeriod(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '[2001-01-01, 2001-01-02)');

select atPeriod(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', '[2001-01-02, 2001-01-03)');

select atPeriodSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', periodset '{[2001-01-01, 2001-01-02), [2001-01-03, 2001-01-04)}');

select atPeriodSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', periodset '{[2001-01-01, 2001-01-02), [2001-01-03, 2001-01-04)}');

select atPeriodSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', periodset '{[2001-01-03, 2001-01-04), [2001-01-05, 2001-01-06)}');

select intersectsTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '2001-01-01');

select intersectsTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '2001-01-02');

select intersectsTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '2001-01-03');

select intersectsTimestamp(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '2001-01-04');

select intersectsTimestampSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '{2001-01-01, 2001-01-04}');

select intersectsTimestampSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '{2001-01-01, 2001-01-04}');

select intersectsPeriod(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '[2001-01-03, 2001-01-04)');

select intersectsPeriod(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', '[2001-01-01, 2001-01-02)');

select intersectsPeriodSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)', 
periodset '{[2001-01-01, 2001-01-02), [2001-01-03, 2001-01-04)}');

select intersectsPeriodSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', 
periodset '{[2001-01-01, 2001-01-02), [2001-01-03, 2001-01-04)}');

select intersectsPeriodSet(ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)', 
periodset '{[2001-01-03, 2001-01-04), [2001-01-05, 2001-01-06)}');

-------------------------------------------------------------------------------
-- Comparison functions and B-tree indexing
-------------------------------------------------------------------------------

select ttextarr '[{AA,BB}@2001-01-01, {AA,BB}@2001-01-02)' =
	ttextarr '[{AA,BB}@2001-01-01, {AA,BB}@2001-01-02)';

select ttextarr '[{AA,BB}@2001-01-01, {AA,BB}@2001-01-02)' <
	ttextarr '[{AA,BB}@2001-01-01, {AA,BB}@2001-01-02)';

WITH Values(value) AS(
select ttextarr  '[{AA,BB}@2001-01-01, {AA,BB}@2001-01-02)' union
select ttextarr  '[{AA}@2001-01-01, {AA}@2001-01-02)' union
select ttextarr  '[{BB}@2001-01-01, {BB}@2001-01-02)' union
select ttextarr  '[{A}@2001-01-01, {A}@2001-01-02)' union
select ttextarr '[{ZZZ}@2000-01-01]' 
)
select *
from Values
order by value;

/******************************************************************************
 * TemporalS
 ******************************************************************************/		

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']);

select getValues(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select startValue(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select endValue(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select getTime(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select duration(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select numSequences(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select startSequence(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select endSequence(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select sequenceN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),1);

select sequenceN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),2);

select sequenceN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),3);

select sequences(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select numInstants(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select startInstant(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select endInstant(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select instantN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),1);

select instantN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),3);

select instantN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),5);

select instants(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select numTimestamps(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select startTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select endTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select timestampN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),1);

select timestampN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),2);

select timestampN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),3);

select timestampN(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),4);

select timestamps(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']));

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) &= '{A, B}';

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) &= '{B, C}';

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) &= '{A, C}';

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) @= '{A, B}';

select atValue(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '{A, B}');

select atValue(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '{B, C}');

select atValue(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '{A, C}');

select atTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'2001-01-01');

select atTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'2001-01-02');

select atTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'2001-01-03');

select valueAtTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'2001-01-01');

select valueAtTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'2001-01-02');

select valueAtTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'2001-01-03');

select atTimestampSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'{2001-01-01, 2001-01-02, 2001-01-04}');

select atTimestampSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']),'{2001-01-03, 2001-01-04}');

select atPeriod(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']),'[2001-01-02, 2001-01-05)');

select atPeriod(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']),'[2001-01-05, 2001-01-06)');

select atPeriodSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']),'{[2001-01-02, 2001-01-03), [2001-01-03, 2001-01-04)}');

select atPeriodSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']),'{[2001-01-02, 2001-01-04), [2001-01-04, 2001-01-05)}');

select atPeriodSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']),'{[2001-01-05, 2001-01-06), [2001-01-07, 2001-01-08)}');

select intersectsTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '2001-01-02');

select intersectsTimestamp(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '2001-01-03');

select intersectsTimestampSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '{2001-01-02,2001-01-03}');

select intersectsTimestampSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), '{2001-01-03,2001-01-04}');

select intersectsPeriod(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']), '[2001-01-03,2001-01-04)');

select intersectsPeriod(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04]']), '[2001-01-03,2001-01-04)');

select intersectsPeriodSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-03)',
ttextarr '[{B, C}@2001-01-04, {B, C}@2001-01-05)']), 
periodset '{[2001-01-03,2001-01-04), [2001-01-05,2001-01-06)}');

select intersectsPeriodSet(ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']), 
periodset '{[2001-01-03,2001-01-04), [2001-01-05,2001-01-06)}');

-------------------------------------------------------------------------------
-- Comparison functions and B-tree indexing
-------------------------------------------------------------------------------

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) =
ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']);

select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) <
ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']);

WITH Values(value) AS(
select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) union
select ttextarrs(ARRAY[
ttextarr '[{AA, BB}@2001-01-01, {AA, BB}@2001-01-02)',
ttextarr '[{B, C}@2001-01-03, {B, C}@2001-01-04)']) union
select ttextarrs(ARRAY[
ttextarr '[{A, B}@2001-01-01, {A, B}@2001-01-02)',
ttextarr '[{BB, CC}@2001-01-02, {BB, CC}@2001-01-03)']) union
select ttextarrs(ARRAY[
ttextarr '[{Z, B}@2001-01-01]',
ttextarr '({B, C}@2001-01-01, {B, C}@2001-01-03]'])
)
select *
from Values
order by value;

/******************************************************************************/