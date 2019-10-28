-------------------------------------------------------------------------------
-- STbox
-------------------------------------------------------------------------------

SELECT stbox 'STBOX ZMT((1.0, 2.0, 3.0, 4.0, 2001-01-04), (1.0, 2.0, 3.0, 4.0, 2001-01-04))';
SELECT stbox 'STBOX ZT((1.0, 2.0, 3.0, 2001-01-04), (1.0, 2.0, 3.0, 2001-01-04))';
SELECT stbox 'STBOX MT((1.0, 2.0, 3.0, 2001-01-04), (1.0, 2.0, 3.0, 2001-01-04))';
SELECT stbox 'STBOX ZM((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0))';
SELECT stbox 'STBOX T((, , 2001-01-04), (, , 2001-01-04))';
SELECT stbox 'STBOX T((1.0, 2.0, 2001-01-04), (1.0, 2.0, 2001-01-04))';
SELECT stbox 'STBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT stbox 'STBOX Z((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT stbox 'STBOX((1.0, 2.0), (1.0, 2.0))';

SELECT stbox 'GEODSTBOX ZMT((1.0, 2.0, 3.0, 4.0, 2001-01-04), (1.0, 2.0, 3.0, 4.0, 2001-01-04))';
SELECT stbox 'GEODSTBOX ZT((1.0, 2.0, 3.0, 2001-01-04), (1.0, 2.0, 3.0, 2001-01-04))';
SELECT stbox 'GEODSTBOX ZM((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0))';
SELECT stbox 'GEODSTBOX T((, , 2001-01-04), (, , 2001-01-04))';
SELECT stbox 'GEODSTBOX T((1.0, 2.0, 2001-01-04), (1.0, 2.0, 2001-01-04))';
SELECT stbox 'GEODSTBOX Z((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

/* Errors */
SELECT stbox 'AAA(1, 2, 3)';
SELECT stbox 'stbox(1, 2, 3)';
SELECT stbox 'stbox((AA, 2, 3))';
SELECT stbox 'stbox((1, AA, 3))';
SELECT stbox 'stbox z((1, 2, AA))';
SELECT stbox 'stbox t((1, 2, AA))';
SELECT stbox 'stbox((1, 2, 3))';
SELECT stbox 'stbox t((1, 2, 2001-01-03))';
SELECT stbox 'stbox t((1, 2, 2001-01-03),()';
SELECT stbox 'stbox t((1, 2, 2001-01-03),(1)'; 
SELECT stbox 'stbox z((1, 2, 3),(1,2)'; 
SELECT stbox 'stbox t((1, 2, 2001-01-03),(1,2)'; 
SELECT stbox 'stbox t((1, 2, 2001-01-03),(1,2,2001-01-03)'; 

SELECT stbox(1,2,3,4);
SELECT stbox(1,2,3,4,5,6);
SELECT stbox(1,2,3,4,5,6,7,8);
SELECT stbox(1,2,3,4,'2001-01-04',5,6,7,8,'2001-01-08');
SELECT stboxt('2001-01-03','2001-01-06');
SELECT stboxt(1,2,'2001-01-03',4,5,'2001-01-06');
SELECT stboxzt(1,2,3,'2001-01-03',4,5,6,'2001-01-06');
SELECT stboxmt(1,2,3,'2001-01-03',4,5,6,'2001-01-06');

SELECT geodstboxt('2001-01-04','2001-01-08');
SELECT geodstbox(1,2,3,4,5,6);
SELECT geodstbox(1,2,3,4,5,6,7,8);
SELECT geodstbox(1,2,3,4,'2001-01-04',5,6,7,8,'2001-01-08');
SELECT geodstboxzt(1,2,3,'2001-01-04',5,6,7,'2001-01-08');

SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((2,2,3,2001-01-04), (2,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((2,2,3,2001-01-04), (2,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,3,3,2001-01-04), (1,3,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,3,3,2001-01-04), (1,3,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,4,2001-01-04), (1,2,4,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,4,2001-01-04), (1,2,4,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-05), (1,2,3,2001-01-05))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-05), (1,2,3,2001-01-05))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (2,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (2,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,3,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,3,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,4,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,4,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-05))'); 
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-05))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))');
SELECT stbox_cmp(stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))', stbox 'STBOX ZT((1,2,3,2001-01-04), (1,2,3,2001-01-04))');

-------------------------------------------------------------------------------

SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b = t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b <> t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b < t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b <= t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b > t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b >= t2.b;

SELECT count(*) FROM tbl_tgeompoint WHERE temp::stbox IS NOT NULL;
SELECT count(*) FROM tbl_tgeogpoint WHERE temp::stbox IS NOT NULL;

SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b && t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b @> t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b <@ t2.b;
SELECT count(*) FROM tbl_stbox t1, tbl_stbox t2 where t1.b ~= t2.b;

-------------------------------------------------------------------------------
