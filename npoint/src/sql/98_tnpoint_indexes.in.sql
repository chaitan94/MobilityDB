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
	OPERATOR	1		<< (stbox, stbox),
	-- overlaps or left
	OPERATOR	2		&< (stbox, stbox),
	-- overlaps
	OPERATOR	3		&& (stbox, stbox),
	-- overlaps or right
	OPERATOR	4		&> (stbox, stbox),
  	-- strictly right
	OPERATOR	5		>> (stbox, stbox),
  	-- same
	OPERATOR	6		~= (stbox, stbox),
	-- contains
	OPERATOR	7		@> (stbox, stbox),
	-- contained by
	OPERATOR	8		<@ (stbox, stbox),
	-- overlaps or below
	OPERATOR	9		&<| (stbox, stbox),
	-- strictly below
	OPERATOR	10		<<| (stbox, stbox),
	-- strictly above
	OPERATOR	11		|>> (stbox, stbox),
	-- overlaps or above
	OPERATOR	12		|&> (stbox, stbox),
	-- distance
--	OPERATOR	25		<-> (stbox, stbox) FOR ORDER BY pg_catalog.float_ops,
	-- overlaps or before
	OPERATOR	28		&<# (stbox, stbox),
	-- strictly before
	OPERATOR	29		<<# (stbox, stbox),
	-- strictly after
	OPERATOR	30		#>> (stbox, stbox),
	-- overlaps or after
	OPERATOR	31		#&> (stbox, stbox),
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
	OPERATOR	1		<< (stbox, stbox),
	-- overlaps or left
	OPERATOR	2		&< (stbox, stbox),
	-- overlaps
	OPERATOR	3		&& (stbox, stbox),
	-- overlaps or right
	OPERATOR	4		&> (stbox, stbox),
  	-- strictly right
	OPERATOR	5		>> (stbox, stbox),
  	-- same
	OPERATOR	6		~= (stbox, stbox),
	-- contains
	OPERATOR	7		@> (stbox, stbox),
	-- contained by
	OPERATOR	8		<@ (stbox, stbox),
	-- overlaps or below
	OPERATOR	9		&<| (stbox, stbox),
	-- strictly below
	OPERATOR	10		<<| (stbox, stbox),
	-- strictly above
	OPERATOR	11		|>> (stbox, stbox),
	-- overlaps or above
	OPERATOR	12		|&> (stbox, stbox),
	-- overlaps or before
	OPERATOR	28		&<# (stbox, stbox),
	-- strictly before
	OPERATOR	29		<<# (stbox, stbox),
	-- strictly after
	OPERATOR	30		#>> (stbox, stbox),
	-- overlaps or after
	OPERATOR	31		#&> (stbox, stbox),
	-- functions
	FUNCTION	1	spgist_stbox_config(internal, internal),
	FUNCTION	2	spgist_stbox_choose(internal, internal),
	FUNCTION	3	spgist_stbox_picksplit(internal, internal),
	FUNCTION	4	spgist_stbox_inner_consistent(internal, internal),
	FUNCTION	5	spgist_stbox_leaf_consistent(internal, internal),
	FUNCTION	6	spgist_tnpoint_compress(internal);

/******************************************************************************/
