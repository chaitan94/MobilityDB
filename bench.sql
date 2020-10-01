-- -------------------------------------------------------------------------------

-- CREATE OR REPLACE FUNCTION random_int(low int, high int)
-- 	RETURNS int AS $$
-- BEGIN
-- 	RETURN floor(random() * (high-low+1) + low);
-- END;
-- $$ LANGUAGE 'plpgsql' STRICT;

-- /*
-- SELECT random_int(1,7), count(*)
-- FROM generate_series(1, 1e3)
-- GROUP BY 1
-- ORDER BY 1
-- */
-- -------------------------------------------------------------------------------

-- CREATE OR REPLACE FUNCTION random_float(low float, high float)
-- 	RETURNS float AS $$
-- BEGIN
-- 	RETURN random() * (high-low) + low;
-- END;
-- $$ LANGUAGE 'plpgsql' STRICT;

-- /*
-- SELECT k, random_float(1, 20) AS f
-- FROM generate_series (1, 15) AS k;
-- */
-- -------------------------------------------------------------------------------

-- CREATE OR REPLACE FUNCTION random_timestamptz(low timestamptz, high timestamptz)
-- 	RETURNS timestamptz AS $$
-- BEGIN
-- 	RETURN date_trunc('minute', (low + random() * (high - low)))::timestamptz(0);
-- END;
-- $$ LANGUAGE 'plpgsql' STRICT;

-- /*
-- SELECT k, random_timestamptz('2001-01-01', '2001-12-31') AS t
-- FROM generate_series (1, 15) AS k;
-- */
-- -------------------------------------------------------------------------------

-- CREATE OR REPLACE FUNCTION random_minutes(low int, high int)
-- 	RETURNS interval AS $$
-- BEGIN
-- 	RETURN random_int(low, high) * '1 minutes'::interval;
-- END;
-- $$ LANGUAGE 'plpgsql' STRICT;

-- /*
-- SELECT k, random_minutes(1, 20) AS m
-- FROM generate_series (1, 15) AS k;
-- */
-- -------------------------------------------------------------------------------

-- CREATE OR REPLACE FUNCTION random_tfloats(lowvalue int, highvalue int,
-- 	lowtime timestamptz, hightime timestamptz,
-- 	maxminutes int, maxcardseq int, maxcard int)
-- 	RETURNS tfloat AS $$
-- DECLARE
-- 	result tfloat[];
-- 	instants tfloat[];
-- 	cardseq int;
-- 	card int;
-- 	t1 timestamptz;
-- 	lower_inc boolean;
-- 	upper_inc boolean;
-- BEGIN
-- 	card = random_int(1, maxcard);
-- 	t1 = random_timestamptz(lowtime, hightime);
-- 	for i in 1..card
-- 	loop
-- 		cardseq = random_int(2, maxcardseq);
-- 		if cardseq = 1 then
-- 			lower_inc = true;
-- 			upper_inc = true;
-- 		else
-- 			lower_inc = random() > 0.5;
-- 			upper_inc = random() > 0.5;
-- 		end if;
-- 		for j in 1..cardseq
-- 		loop
-- 			t1 = t1 + random_minutes(1, maxminutes);
-- 			instants[j] = tfloatinst(random_float(lowvalue, highvalue), t1);
-- 		end loop;
-- 		result[i] = tfloatseq(instants, lower_inc, upper_inc);
-- 		instants = NULL;
-- 		t1 = t1 + random_minutes(1, maxminutes);
-- 	end loop;
-- 	RETURN tfloats(result);
-- END;
-- $$ LANGUAGE 'plpgsql' STRICT;

-- /*
-- SELECT k, random_tfloats(1, 100, '2001-01-01', '2001-12-31', 10, 10, 10) AS ts
-- FROM generate_series (1, 15) AS k;
-- */
-- -------------------------------------------------------------------------------

-- Benchmark for 1K rows

-- DROP TABLE IF EXISTS tbl_tfloats_1K;

-- CREATE TABLE tbl_tfloats_1K AS
-- SELECT
--     k,
--     random_tfloats (1, 100, '2001-01-01'::timestamptz, '2001-12-31'::timestamptz, 10, 1000, 10) AS seqset
-- FROM
--     generate_series(1, 1000) k;

SELECT
    atPeriod(seqset, period '[2001-01-02, 2001-01-05)')
FROM
    tbl_tfloats_1K;

SELECT
    numInstants(atPeriod(seqset, period '[2001-01-02, 2001-01-05)'))
FROM
    tbl_tfloats_1K;

-------------------------------------------------------------------------------

-- Benchmark for 10K rows

-- DROP TABLE IF EXISTS tbl_tfloats_10K;

-- CREATE TABLE tbl_tfloats_10K AS
-- SELECT
--     k,
--     random_tfloats (1, 100, '2001-01-01'::timestamptz, '2001-12-31'::timestamptz, 10, 1000, 10) AS seqset
-- FROM
--     generate_series(1, 10000) k;

SELECT
    atPeriod(seqset, period '[2001-01-02, 2001-01-05)')
FROM
    tbl_tfloats_10K;

SELECT
    numInstants(atPeriod(seqset, period '[2001-01-02, 2001-01-05)'))
FROM
    tbl_tfloats_10K;

-------------------------------------------------------------------------------

-- Benchmark for 100k rows

-- DROP TABLE IF EXISTS tbl_tfloats_100K;

-- CREATE TABLE tbl_tfloats_100K AS
-- SELECT
--     k,
--     random_tfloats (1, 100, '2001-01-01'::timestamptz, '2001-12-31'::timestamptz, 10, 1000, 10) AS seqset
-- FROM
--     generate_series(1, 100000) k;

SELECT
    atPeriod(seqset, period '[2001-01-02, 2001-01-05)')
FROM
    tbl_tfloats_100K;

SELECT
    numInstants(atPeriod(seqset, period '[2001-01-02, 2001-01-05)'))
FROM
    tbl_tfloats_100K;

-------------------------------------------------------------------------------

-- Benchmark for 1M rows

-- DROP TABLE IF EXISTS tbl_tfloats_1M;

-- CREATE TABLE tbl_tfloats_1M AS
-- SELECT
--     k,
--     random_tfloats (1, 100, '2001-01-01'::timestamptz, '2001-12-31'::timestamptz, 10, 1000, 10) AS seqset
-- FROM
--     generate_series(1, 1000000) k;

SELECT
    atPeriod(seqset, period '[2001-01-02, 2001-01-05)')
FROM
    tbl_tfloats_1M;

SELECT
    numInstants(atPeriod(seqset, period '[2001-01-02, 2001-01-05)'))
FROM
    tbl_tfloats_1M;
