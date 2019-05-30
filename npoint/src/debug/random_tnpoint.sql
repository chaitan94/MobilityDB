-------------------------------------------------------------------------------
-- Network points
-------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION random_fraction()
	RETURNS float AS $$
BEGIN
	RETURN random();
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_fraction() AS f
FROM generate_series(1,10) k;
*/

CREATE OR REPLACE FUNCTION random_npoint(lown integer, highn integer) 
	RETURNS npoint AS $$
BEGIN
	RETURN npoint(random_int(lown, highn), random_fraction());
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_npoint(1, 1000) AS g
FROM generate_series(1,10) k;
*/

CREATE OR REPLACE FUNCTION random_nsegment(lown integer, highn integer) 
	RETURNS nsegment AS $$
DECLARE
	random1 float;
	random2 float;
	tmp float;
BEGIN
	random1 = random_fraction();
	random2 = random_fraction();
	IF random1 > random2 THEN
		tmp = random1;
		random1 = random2;
		random2 = tmp;
	END IF;
	RETURN nsegment(random_int(lown, highn), random1, random2);
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_nsegment(1, 1000) AS g
FROM generate_series(1,10) k;
*/

------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION random_tnpointinst(lown integer, highn integer, 
	lowtime timestamptz, hightime timestamptz) 
	RETURNS tnpoint AS $$
BEGIN
	RETURN tnpointinst(random_npoint(lown, highn), random_timestamptz(lowtime, hightime));
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_tnpointinst(0, 1000, '2001-01-01', '2001-12-31') AS inst
FROM generate_series(1,10) k;
*/

-------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION random_tnpointi(lown integer, highn integer, 
	lowtime timestamptz, hightime timestamptz, maxminutes int, maxcard int)
	RETURNS tnpoint AS $$
DECLARE
	result tnpoint[];
	card int;
	t timestamptz;
BEGIN
	card = random_int(1, maxcard);
	t = random_timestamptz(lowtime, hightime);
	for i in 1..card 
	loop
		result[i] = tnpointinst(random_npoint(lown, highn), t);
		t = t + random_minutes(1, maxminutes);
	end loop;
	RETURN tnpointi(result);
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_tnpointi(0, 1000, '2001-01-01', '2001-12-31', 10, 10) AS ti
FROM generate_series(1,10) k;
*/

-------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION random_tnpointseq(lown integer, highn integer, 
	lowtime timestamptz, hightime timestamptz, maxminutes int, maxcard int) 
	RETURNS tnpoint AS $$
DECLARE
	result tnpoint[];
	card int;
	rid int;
	t1 timestamptz;
	lower_inc boolean;
	upper_inc boolean;
BEGIN
	card = random_int(1, maxcard);
	if card = 1 then
		lower_inc = true;
		upper_inc = true;
	else
		lower_inc = random() > 0.5;
		upper_inc = random() > 0.5;
	end if;
	t1 = random_timestamptz(lowtime, hightime);
	rid = random_int(lown, highn);
	for i in 1..card 
	loop
		t1 = t1 + random_minutes(1, maxminutes);
		result[i] = tnpointinst(npoint(rid, random()), t1);
	end loop;
	RETURN tnpointseq(result, lower_inc, upper_inc);
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_tnpointseq(0, 1000, '2001-01-01', '2001-12-31', 10, 10)
FROM generate_series (1, 15) AS k;
*/

-------------------------------------------------------------------------------

CREATE OR REPLACE FUNCTION random_tnpoints(lown integer, highn integer, 
	lowtime timestamptz, hightime timestamptz, 
	maxminutes int, maxcardseq int, maxcard int) 
	RETURNS tnpoint AS $$
DECLARE
	result tnpoint[];
	instants tnpoint[];
	cardseq int;
	card int;
	rid int;
	t1 timestamptz;
	lower_inc boolean;
	upper_inc boolean;
BEGIN
	card = random_int(1, maxcard);
	t1 = random_timestamptz(lowtime, hightime);
	for i in 1..card 
	loop
		cardseq = random_int(1, maxcardseq);
		if cardseq = 1 then
			lower_inc = true;
			upper_inc = true;
		else
			lower_inc = random() > 0.5;
			upper_inc = random() > 0.5;
		end if;
		rid = random_int(lown, highn);
		for j in 1..cardseq
		loop
			t1 = t1 + random_minutes(1, maxminutes);
			instants[j] = tnpointinst(npoint(rid, random()), t1);
		end loop;
		result[i] = tnpointseq(instants, lower_inc, upper_inc);
		instants = NULL;
		t1 = t1 + random_minutes(1, maxminutes);
	end loop;
	RETURN tnpoints(result);
END;
$$ LANGUAGE 'plpgsql' STRICT;

/*
SELECT k, random_tnpoints(0, 1000, '2001-01-01', '2001-12-31', 10, 10, 10) AS ts
FROM generate_series (1, 15) AS k;
*/

-------------------------------------------------------------------------------
