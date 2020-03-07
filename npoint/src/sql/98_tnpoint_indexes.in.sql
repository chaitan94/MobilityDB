/*****************************************************************************
 *
 * tnpoint_indexes.sql
 *	  R-tree GiST and SP-GiST indexes for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

CREATE FUNCTION gist_tnpoint_consistent(internal, tnpoint, smallint, oid, internal)
	RETURNS bool
	AS 'MODULE_PATHNAME', 'gist_stbox_consistent'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE FUNCTION gist_tnpoint_compress(internal)
	RETURNS internal
	AS 'MODULE_PATHNAME', 'gist_tnpoint_compress'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR CLASS gist_tnpoint_ops
	DEFAULT FOR TYPE tnpoint USING gist AS
	STORAGE stbox,
	-- strictly left
	OPERATOR	1		<< (tnpoint, stbox),
	OPERATOR	1		<< (tnpoint, tnpoint),
	-- overlaps or left
	OPERATOR	2		&< (tnpoint, stbox),
	OPERATOR	2		&< (tnpoint, tnpoint),
	-- overlaps
	OPERATOR	3		&& (tnpoint, stbox),
	OPERATOR	3		&& (tnpoint, tnpoint),
	-- overlaps or right
	OPERATOR	4		&> (tnpoint, stbox),
	OPERATOR	4		&> (tnpoint, tnpoint),
  	-- strictly right
	OPERATOR	5		>> (tnpoint, stbox),
	OPERATOR	5		>> (tnpoint, tnpoint),
  	-- same
	OPERATOR	6		~= (tnpoint, stbox),
	OPERATOR	6		~= (tnpoint, tnpoint),
	-- contains
	OPERATOR	7		@> (tnpoint, stbox),
	OPERATOR	7		@> (tnpoint, tnpoint),
	-- contained by
	OPERATOR	8		<@ (tnpoint, stbox),
	OPERATOR	8		<@ (tnpoint, tnpoint),
	-- overlaps or below
	OPERATOR	9		&<| (tnpoint, stbox),
	OPERATOR	9		&<| (tnpoint, tnpoint),
	-- strictly below
	OPERATOR	10		<<| (tnpoint, stbox),
	OPERATOR	10		<<| (tnpoint, tnpoint),
	-- strictly above
	OPERATOR	11		|>> (tnpoint, stbox),
	OPERATOR	11		|>> (tnpoint, tnpoint),
	-- overlaps or above
	OPERATOR	12		|&> (tnpoint, stbox),
	OPERATOR	12		|&> (tnpoint, tnpoint),
	-- distance
--	OPERATOR	25		<-> (tnpoint, stbox) FOR ORDER BY pg_catalog.float_ops,
	-- overlaps or before
	OPERATOR	28		&<# (tnpoint, stbox),
	OPERATOR	28		&<# (tnpoint, tnpoint),
	-- strictly before
	OPERATOR	29		<<# (tnpoint, stbox),
	OPERATOR	29		<<# (tnpoint, tnpoint),
	-- strictly after
	OPERATOR	30		#>> (tnpoint, stbox),
	OPERATOR	30		#>> (tnpoint, tnpoint),
	-- overlaps or after
	OPERATOR	31		#&> (tnpoint, stbox),
	OPERATOR	31		#&> (tnpoint, tnpoint),
	-- functions
	FUNCTION	1	gist_tnpoint_consistent(internal, tnpoint, smallint, oid, internal),
	FUNCTION	2	gist_stbox_union(internal, internal),
	FUNCTION	3	gist_tnpoint_compress(internal),
	FUNCTION	5	gist_stbox_penalty(internal, internal, internal),
	FUNCTION	6	gist_stbox_picksplit(internal, internal),
	FUNCTION	7	gist_stbox_same(stbox, stbox, internal);
--	FUNCTION	8	gist_tnpoint_distance(internal, tnpoint, smallint, oid, internal),
	
/******************************************************************************/

CREATE FUNCTION spgist_tnpoint_compress(internal)
	RETURNS internal
	AS 'MODULE_PATHNAME'
	LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OPERATOR CLASS spgist_tnpoint_ops
	DEFAULT FOR TYPE tnpoint USING spgist AS
	-- strictly left
	OPERATOR	1		<< (tnpoint, stbox),
	OPERATOR	1		<< (tnpoint, tnpoint),
	-- overlaps or left
	OPERATOR	2		&< (tnpoint, stbox),
	OPERATOR	2		&< (tnpoint, tnpoint),
	-- overlaps
	OPERATOR	3		&& (tnpoint, stbox),
	OPERATOR	3		&& (tnpoint, tnpoint),
	-- overlaps or right
	OPERATOR	4		&> (tnpoint, stbox),
	OPERATOR	4		&> (tnpoint, tnpoint),
  	-- strictly right
	OPERATOR	5		>> (tnpoint, stbox),
	OPERATOR	5		>> (tnpoint, tnpoint),
  	-- same
	OPERATOR	6		~= (tnpoint, stbox),
	OPERATOR	6		~= (tnpoint, tnpoint),
	-- contains
	OPERATOR	7		@> (tnpoint, stbox),
	OPERATOR	7		@> (tnpoint, tnpoint),
	-- contained by
	OPERATOR	8		<@ (tnpoint, stbox),
	OPERATOR	8		<@ (tnpoint, tnpoint),
	-- overlaps or below
	OPERATOR	9		&<| (tnpoint, stbox),
	OPERATOR	9		&<| (tnpoint, tnpoint),
	-- strictly below
	OPERATOR	10		<<| (tnpoint, stbox),
	OPERATOR	10		<<| (tnpoint, tnpoint),
	-- strictly above
	OPERATOR	11		|>> (tnpoint, stbox),
	OPERATOR	11		|>> (tnpoint, tnpoint),
	-- overlaps or above
	OPERATOR	12		|&> (tnpoint, stbox),
	OPERATOR	12		|&> (tnpoint, tnpoint),
	-- overlaps or before
	OPERATOR	28		&<# (tnpoint, stbox),
	OPERATOR	28		&<# (tnpoint, tnpoint),
	-- strictly before
	OPERATOR	29		<<# (tnpoint, stbox),
	OPERATOR	29		<<# (tnpoint, tnpoint),
	-- strictly after
	OPERATOR	30		#>> (tnpoint, stbox),
	OPERATOR	30		#>> (tnpoint, tnpoint),
	-- overlaps or after
	OPERATOR	31		#&> (tnpoint, stbox),
	OPERATOR	31		#&> (tnpoint, tnpoint),
	-- functions
	FUNCTION	1	spgist_stbox_config(internal, internal),
	FUNCTION	2	spgist_stbox_choose(internal, internal),
	FUNCTION	3	spgist_stbox_picksplit(internal, internal),
	FUNCTION	4	spgist_stbox_inner_consistent(internal, internal),
	FUNCTION	5	spgist_stbox_leaf_consistent(internal, internal),
	FUNCTION	6	spgist_tnpoint_compress(internal);

/******************************************************************************/
