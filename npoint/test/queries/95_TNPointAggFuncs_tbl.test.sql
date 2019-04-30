/******************************************************************************/

SELECT count(inst) FROM tbl_tnpointinst;
SELECT count(ti) FROM tbl_tnpointi;
SELECT count(seq) FROM tbl_tnpointseq;
SELECT count(ts) FROM tbl_tnpoints;

SELECT wcount(inst, '1 hour') FROM tbl_tnpointinst;
SELECT wcount(ti, '1 hour') FROM tbl_tnpointi;
SELECT wcount(seq, '1 hour') FROM tbl_tnpointseq;
SELECT wcount(ts, '1 hour') FROM tbl_tnpoints;

SELECT astext(tcentroid(inst)) FROM tbl_tnpointinst;
SELECT astext(tcentroid(ti)) FROM tbl_tnpointi;
SELECT astext(tcentroid(seq)) FROM tbl_tnpointseq;
SELECT astext(tcentroid(ts)) FROM tbl_tnpoints;

/******************************************************************************/