﻿/*****************************************************************************/

DROP INDEX IF EXISTS tbl_tnpoint_gist_idx;
DROP INDEX IF EXISTS tbl_tnpoint_spgist_idx;

/*****************************************************************************/

DROP TABLE if exists test_tnpointboundboxops;
CREATE TABLE test_tnpointboundboxops(
	op char(3), 
	leftarg text, 
	rightarg text, 
	noidx bigint,
	gistidx bigint,
	spgistidx bigint );

/*****************************************************************************/
-- <type> op tnpoint

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'geometry', 'tnpoint', count(*) FROM tbl_geometry, tbl_tnpoint WHERE g && temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'geometry', 'tnpoint', count(*) FROM tbl_geometry, tbl_tnpoint WHERE g @> temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'geometry', 'tnpoint', count(*) FROM tbl_geometry, tbl_tnpoint WHERE g <@ temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'geometry', 'tnpoint', count(*) FROM tbl_geometry, tbl_tnpoint WHERE g ~= temp;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'timestamptz', 'tnpoint', count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t && temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'timestamptz', 'tnpoint', count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t @> temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'timestamptz', 'tnpoint', count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t <@ temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'timestamptz', 'tnpoint', count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t ~= temp;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'timestampset', 'tnpoint', count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts && temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'timestampset', 'tnpoint', count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts @> temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'timestampset', 'tnpoint', count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts <@ temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'timestampset', 'tnpoint', count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts ~= temp;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'period', 'tnpoint', count(*) FROM tbl_period, tbl_tnpoint WHERE p && temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'period', 'tnpoint', count(*) FROM tbl_period, tbl_tnpoint WHERE p @> temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'period', 'tnpoint', count(*) FROM tbl_period, tbl_tnpoint WHERE p <@ temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'period', 'tnpoint', count(*) FROM tbl_period, tbl_tnpoint WHERE p ~= temp;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'periodset', 'tnpoint', count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps && temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'periodset', 'tnpoint', count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps @> temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'periodset', 'tnpoint', count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps <@ temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'periodset', 'tnpoint', count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps ~= temp;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'stbox', 'tnpoint', count(*) FROM tbl_stbox, tbl_tnpoint WHERE b && temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'stbox', 'tnpoint', count(*) FROM tbl_stbox, tbl_tnpoint WHERE b @> temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'stbox', 'tnpoint', count(*) FROM tbl_stbox, tbl_tnpoint WHERE b <@ temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'stbox', 'tnpoint', count(*) FROM tbl_stbox, tbl_tnpoint WHERE b ~= temp;

/*****************************************************************************/
--  tnpoint op <type>

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'geometry', count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp && g;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'geometry', count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp @> g;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'geometry', count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp <@ g;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'geometry', count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp ~= g;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'timestamptz', count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp && t;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'timestamptz', count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp @> t;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'timestamptz', count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp <@ t;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'timestamptz', count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp ~= t;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'timestampset', count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp && ts;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'timestampset', count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp @> ts;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'timestampset', count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp <@ ts;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'timestampset', count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp ~= ts;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'period', count(*) FROM tbl_tnpoint, tbl_period WHERE temp && p;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'period', count(*) FROM tbl_tnpoint, tbl_period WHERE temp @> p;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'period', count(*) FROM tbl_tnpoint, tbl_period WHERE temp <@ p;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'period', count(*) FROM tbl_tnpoint, tbl_period WHERE temp ~= p;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'periodset', count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp && ps;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'periodset', count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp @> ps;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'periodset', count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp <@ ps;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'periodset', count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp ~= ps;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'stbox', count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp && b;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'stbox', count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp @> b;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'stbox', count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp <@ b;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'stbox', count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp ~= b;

INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '&&', 'tnpoint', 'tnpoint', count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp && t2.temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '@>', 'tnpoint', 'tnpoint', count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp @> t2.temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '<@', 'tnpoint', 'tnpoint', count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <@ t2.temp;
INSERT INTO test_tnpointboundboxops(op, leftarg, rightarg, noidx)
SELECT '~=', 'tnpoint', 'tnpoint', count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp ~= t2.temp;

/*****************************************************************************/

CREATE INDEX tbl_tnpoint_gist_idx ON tbl_tnpoint USING GIST(temp);

/*****************************************************************************/
-- <type> op tnpoint

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g && temp )
WHERE op = '&&' and leftarg = 'geometry' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g @> temp )
WHERE op = '@>' and leftarg = 'geometry' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g <@ temp )
WHERE op = '<@' and leftarg = 'geometry' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g ~= temp )
WHERE op = '~=' and leftarg = 'geometry' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t && temp )
WHERE op = '&&' and leftarg = 'timestamptz' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t @> temp )
WHERE op = '@>' and leftarg = 'timestamptz' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t <@ temp )
WHERE op = '<@' and leftarg = 'timestamptz' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t ~= temp )
WHERE op = '~=' and leftarg = 'timestamptz' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts && temp )
WHERE op = '&&' and leftarg = 'timestampset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts @> temp )
WHERE op = '@>' and leftarg = 'timestampset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts <@ temp )
WHERE op = '<@' and leftarg = 'timestampset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts ~= temp )
WHERE op = '~=' and leftarg = 'timestampset' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p && temp )
WHERE op = '&&' and leftarg = 'period' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p @> temp )
WHERE op = '@>' and leftarg = 'period' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p <@ temp )
WHERE op = '<@' and leftarg = 'period' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p ~= temp )
WHERE op = '~=' and leftarg = 'period' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps && temp )
WHERE op = '&&' and leftarg = 'periodset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps @> temp )
WHERE op = '@>' and leftarg = 'periodset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps <@ temp )
WHERE op = '<@' and leftarg = 'periodset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps ~= temp )
WHERE op = '~=' and leftarg = 'periodset' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b && temp )
WHERE op = '&&' and leftarg = 'stbox' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b @> temp )
WHERE op = '@>' and leftarg = 'stbox' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b <@ temp )
WHERE op = '<@' and leftarg = 'stbox' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b ~= temp )
WHERE op = '~=' and leftarg = 'stbox' and rightarg = 'tnpoint';

/*****************************************************************************/
-- tnpoint op <type>

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp && g )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'geometry';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp @> g )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'geometry';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp <@ g )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'geometry';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp ~= g )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'geometry';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp && t )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'timestamptz';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp @> t )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'timestamptz';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp <@ t )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'timestamptz';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp ~= t )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'timestamptz';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp && ts )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'timestampset';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp @> ts )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'timestampset';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp <@ ts )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'timestampset';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp ~= ts )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'timestampset';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp && p )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'period';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp @> p )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'period';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp <@ p )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'period';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp ~= p )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'period';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp && ps )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'periodset';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp @> ps )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'periodset';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp <@ ps )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'periodset';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp ~= ps )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'periodset';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp && b )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'stbox';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp @> b )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'stbox';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp <@ b )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'stbox';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp ~= b )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'stbox';

UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp && t2.temp )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp @> t2.temp )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <@ t2.temp )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET gistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp ~= t2.temp )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'tnpoint';

/*****************************************************************************/

DROP INDEX IF EXISTS tbl_tnpoint_gist_idx;

CREATE INDEX tbl_tnpoint_spgist_idx ON tbl_tnpoint USING SPGIST(temp);

/*****************************************************************************/
-- <type> op tnpoint

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g && temp )
WHERE op = '&&' and leftarg = 'geometry' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g @> temp )
WHERE op = '@>' and leftarg = 'geometry' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g <@ temp )
WHERE op = '<@' and leftarg = 'geometry' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_geometry, tbl_tnpoint WHERE g ~= temp )
WHERE op = '~=' and leftarg = 'geometry' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t && temp )
WHERE op = '&&' and leftarg = 'timestamptz' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t @> temp )
WHERE op = '@>' and leftarg = 'timestamptz' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t <@ temp )
WHERE op = '<@' and leftarg = 'timestamptz' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestamptz, tbl_tnpoint WHERE t ~= temp )
WHERE op = '~=' and leftarg = 'timestamptz' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts && temp )
WHERE op = '&&' and leftarg = 'timestampset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts @> temp )
WHERE op = '@>' and leftarg = 'timestampset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts <@ temp )
WHERE op = '<@' and leftarg = 'timestampset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_timestampset, tbl_tnpoint WHERE ts ~= temp )
WHERE op = '~=' and leftarg = 'timestampset' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p && temp )
WHERE op = '&&' and leftarg = 'period' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p @> temp )
WHERE op = '@>' and leftarg = 'period' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p <@ temp )
WHERE op = '<@' and leftarg = 'period' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_period, tbl_tnpoint WHERE p ~= temp )
WHERE op = '~=' and leftarg = 'period' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps && temp )
WHERE op = '&&' and leftarg = 'periodset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps @> temp )
WHERE op = '@>' and leftarg = 'periodset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps <@ temp )
WHERE op = '<@' and leftarg = 'periodset' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_periodset, tbl_tnpoint WHERE ps ~= temp )
WHERE op = '~=' and leftarg = 'periodset' and rightarg = 'tnpoint';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b && temp )
WHERE op = '&&' and leftarg = 'stbox' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b @> temp )
WHERE op = '@>' and leftarg = 'stbox' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b <@ temp )
WHERE op = '<@' and leftarg = 'stbox' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_stbox, tbl_tnpoint WHERE b ~= temp )
WHERE op = '~=' and leftarg = 'stbox' and rightarg = 'tnpoint';

/*****************************************************************************/
-- tnpoint op <type>

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp && g )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'geometry';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp @> g )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'geometry';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp <@ g )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'geometry';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_geometry WHERE temp ~= g )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'geometry';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp && t )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'timestamptz';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp @> t )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'timestamptz';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp <@ t )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'timestamptz';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestamptz WHERE temp ~= t )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'timestamptz';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp && ts )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'timestampset';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp @> ts )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'timestampset';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp <@ ts )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'timestampset';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_timestampset WHERE temp ~= ts )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'timestampset';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp && p )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'period';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp @> p )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'period';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp <@ p )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'period';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_period WHERE temp ~= p )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'period';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp && ps )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'periodset';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp @> ps )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'periodset';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp <@ ps )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'periodset';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_periodset WHERE temp ~= ps )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'periodset';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp && b )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'stbox';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp @> b )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'stbox';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp <@ b )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'stbox';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint, tbl_stbox WHERE temp ~= b )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'stbox';

UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp && t2.temp )
WHERE op = '&&' and leftarg = 'tnpoint' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp @> t2.temp )
WHERE op = '@>' and leftarg = 'tnpoint' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp <@ t2.temp )
WHERE op = '<@' and leftarg = 'tnpoint' and rightarg = 'tnpoint';
UPDATE test_tnpointboundboxops
SET spgistidx = ( SELECT count(*) FROM tbl_tnpoint t1, tbl_tnpoint t2 WHERE t1.temp ~= t2.temp )
WHERE op = '~=' and leftarg = 'tnpoint' and rightarg = 'tnpoint';

/*****************************************************************************/

SELECT * FROM test_tnpointboundboxops
WHERE noidx <> gistidx or noidx <> spgistidx or gistidx <> spgistidx; 

/*****************************************************************************/

DROP INDEX IF EXISTS tbl_tnpoint_spgist_idx;
DROP TABLE if exists test_tnpointboundboxops;

/*****************************************************************************/
