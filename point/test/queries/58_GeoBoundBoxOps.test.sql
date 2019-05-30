﻿-------------------------------------------------------------------------------

SELECT geometry 'Point empty'::gbox;
SELECT geometry 'Point(1 1)'::gbox;
SELECT geometry 'Point(1 1 1)'::gbox;
SELECT geography 'Point empty'::gbox;
SELECT geography 'Point(1 1)'::gbox;
SELECT geography 'Point(1 1 1)'::gbox;
SELECT timestamptz '2000-01-01'::gbox;
SELECT timestampset '{2000-01-01, 2000-01-02}'::gbox;
SELECT period '[2000-01-01, 2000-01-02]'::gbox;
SELECT periodset '{[2000-01-01, 2000-01-02],[2000-01-03, 2000-01-04]}'::gbox;
SELECT gbox(geometry 'Point empty', timestamptz '2000-01-01');
SELECT gbox(geometry 'Point(1 1)', timestamptz '2000-01-01');
SELECT gbox(geometry 'Point(1 1 1)', timestamptz '2000-01-01');
SELECT gbox(geometry 'Point empty', period '[2000-01-01, 2000-01-02]');
SELECT gbox(geometry 'Point(1 1)', period '[2000-01-01, 2000-01-02]');
SELECT gbox(geometry 'Point(1 1 1)', period '[2000-01-01, 2000-01-02]');
SELECT gbox(geography 'Point empty', timestamptz '2000-01-01');
SELECT gbox(geography 'Point(1 1)', timestamptz '2000-01-01');
SELECT gbox(geography 'Point(1 1 1)', timestamptz '2000-01-01');
SELECT gbox(geography 'Point empty', period '[2000-01-01, 2000-01-02]');
SELECT gbox(geography 'Point(1 1)', period '[2000-01-01, 2000-01-02]');
SELECT gbox(geography 'Point(1 1 1)', period '[2000-01-01, 2000-01-02]');

SELECT tgeompoint 'Point(1 1)@2000-01-01'::gbox;
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}'::gbox;
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]'::gbox;
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}'::gbox;

SELECT tgeogpoint 'Point(1 1)@2000-01-01'::gbox;
SELECT tgeogpoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}'::gbox;
SELECT tgeogpoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]'::gbox;
SELECT tgeogpoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}'::gbox;

-------------------------------------------------------------------------------

SELECT expandSpatial(gbox 'GBOX((1.0, 2.0), (1.0, 2.0))', 0.5);
SELECT expandSpatial(gbox 'GBOX Z((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))', 0.5);
SELECT expandSpatial(gbox 'GBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))', 0.5);
SELECT expandSpatial(gbox 'GBOX ZM((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0))', 0.5);
SELECT expandSpatial(gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))', 0.5);

SELECT expandTemporal(gbox 'GBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))', '1 day');
SELECT expandTemporal(gbox 'GBOX ZM((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0))', '1 day');
SELECT expandTemporal(gbox 'GEODBOX M((1.0, 2.0, 3.0, 4.0), (1.0, 2.0, 3.0, 4.0))', '1 day');
/* Errors */
SELECT expandTemporal(gbox 'GBOX((1.0, 2.0), (1.0, 2.0))', '1 day');
SELECT expandTemporal(gbox 'GBOX Z((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))', '1 day');
SELECT expandTemporal(gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))', '1 day');

SELECT expandSpatial(tgeompoint 'Point(1 1)@2000-01-01', 0.5);
SELECT expandSpatial(tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}', 0.5);
SELECT expandSpatial(tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]', 0.5);
SELECT expandSpatial(tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}', 0.5);

SELECT expandSpatial(tgeogpoint 'Point(1 1)@2000-01-01', 0.5);
SELECT expandSpatial(tgeogpoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}', 0.5);
SELECT expandSpatial(tgeogpoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]', 0.5);
SELECT expandSpatial(tgeogpoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}', 0.5);

SELECT expandTemporal(tgeompoint 'Point(1 1)@2000-01-01', '1 day');
SELECT expandTemporal(tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}', '1 day');
SELECT expandTemporal(tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]', '1 day');
SELECT expandTemporal(tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}', '1 day');

SELECT expandTemporal(tgeogpoint 'Point(1 1)@2000-01-01', '1 day');
SELECT expandTemporal(tgeogpoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}', '1 day');
SELECT expandTemporal(tgeogpoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]', '1 day');
SELECT expandTemporal(tgeogpoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}', '1 day');

-------------------------------------------------------------------------------

SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' && gbox 'GBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' @> gbox 'GBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' <@ gbox 'GBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' ~= gbox 'GBOX M((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX Z((1.0, 1.0, 1.0), (2.0, 2.0, 2.0))' ~= gbox 'GBOX Z((1.0, 1.0, 1.0), (2.0, 2.0, 3.0))';

/* Errors */
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT gbox 'GBOX((1.0, 1.0), (2.0, 2.0))' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1)' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point(1 1)' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point(1 1)' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point empty' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point empty' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point empty' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point empty' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point(1 1 1)' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point(1 1 1)' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point(1 1 1)' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geometry 'Point Z empty' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point Z empty' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point Z empty' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point Z empty' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geography 'Point(1 1)' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1)' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1)' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1)' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point empty' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point empty' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point empty' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point empty' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point(1 1 1)' && tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1 1)' && tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1 1)' && tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1 1)' && tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT geography 'Point Z empty' && tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point Z empty' && tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point Z empty' && tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point Z empty' && tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestamptz '2000-01-01' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestamptz '2000-01-01' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestamptz '2000-01-01' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' && tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' && tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' && tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' && tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' && tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' && tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' && tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' && tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && geometry 'Point(1 1)';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && geometry 'Point(1 1)';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && geometry 'Point(1 1)';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && geometry 'Point(1 1)';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && geometry 'Point empty';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && geometry 'Point empty';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && geometry 'Point empty';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && geometry 'Point empty';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && geometry 'Point(1 1 1)';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && geometry 'Point(1 1 1)';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && geometry 'Point(1 1 1)';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && geometry 'Point(1 1 1)';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && geometry 'Point Z empty';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && geometry 'Point Z empty';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && geometry 'Point Z empty';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && geometry 'Point Z empty';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && geography 'Point(1 1)';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && geography 'Point (1 1)';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && geography 'Point(1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && geography 'Point(1 1)';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && geography 'Point empty';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && geography 'Point empty';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && geography 'Point empty';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && geography 'Point empty';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' && geography 'Point(1 1 1)';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' && geography 'Point(1 1 1)';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' && geography 'Point(1 1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' && geography 'Point(1 1 1)';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' && geography 'Point Z empty';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' && geography 'Point Z empty';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' && geography 'Point Z empty';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' && geography 'Point Z empty';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' && timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1)@2000-01-01' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1)@2000-01-01' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1)@2000-01-01' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' && tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' && tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

/* Errors */
SELECT geometry 'SRID=5676;Point(1 1)' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' && geometry 'SRID=5676;Point(1 1)';
SELECT tgeompoint 'Point(1 1)@2000-01-01'&& geometry 'Point(1 1 1)';
SELECT tgeompoint 'SRID=5676;Point(1 1)@2000-01-01' && tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' && tgeompoint 'Point(1 1)@2000-01-01';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1)' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point(1 1)' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point(1 1)' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point empty' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point empty' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point empty' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point empty' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point(1 1 1)' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point(1 1 1)' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point(1 1 1)' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geometry 'Point Z empty' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point Z empty' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point Z empty' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point Z empty' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geography 'Point(1 1)' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1)' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1)' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1)' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point empty' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point empty' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point empty' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point empty' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point(1 1 1)' @> tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1 1)' @> tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1 1)' @> tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1 1)' @> tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT geography 'Point Z empty' @> tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point Z empty' @> tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point Z empty' @> tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point Z empty' @> tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestamptz '2000-01-01' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestamptz '2000-01-01' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestamptz '2000-01-01' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' @> tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' @> tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' @> tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' @> tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' @> tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' @> tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' @> tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' @> tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> geometry 'Point(1 1)';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> geometry 'Point(1 1)';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> geometry 'Point(1 1)';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> geometry 'Point(1 1)';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> geometry 'Point empty';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> geometry 'Point empty';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> geometry 'Point empty';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> geometry 'Point empty';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> geometry 'Point(1 1 1)';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> geometry 'Point(1 1 1)';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> geometry 'Point(1 1 1)';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> geometry 'Point(1 1 1)';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> geometry 'Point Z empty';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> geometry 'Point Z empty';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> geometry 'Point Z empty';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> geometry 'Point Z empty';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> geography 'Point(1 1)';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> geography 'Point(1 1)';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> geography 'Point(1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> geography 'Point(1 1)';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> geography 'Point empty';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> geography 'Point empty';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> geography 'Point empty';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> geography 'Point empty';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' @> geography 'Point(1 1 1)';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' @> geography 'Point(1 1 1)';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' @> geography 'Point(1 1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' @> geography 'Point(1 1 1)';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' @> geography 'Point Z empty';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' @> geography 'Point Z empty';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' @> geography 'Point Z empty';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' @> geography 'Point Z empty';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' @> timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1)@2000-01-01' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1)@2000-01-01' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1)@2000-01-01' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' @> tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' @> tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

/* Errors */
SELECT geometry 'SRID=5676;Point(1 1)' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' @> geometry 'SRID=5676;Point(1 1)';
SELECT tgeompoint 'Point(1 1)@2000-01-01' @> geometry 'Point(1 1 1)';
SELECT tgeompoint 'SRID=5676;Point(1 1)@2000-01-01' @> tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' @> tgeompoint 'Point(1 1)@2000-01-01';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1)' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point(1 1)' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point(1 1)' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point empty' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point empty' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point empty' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point empty' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point(1 1 1)' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point(1 1 1)' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point(1 1 1)' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geometry 'Point Z empty' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point Z empty' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point Z empty' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point Z empty' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geography 'Point(1 1)' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1)' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1)' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1)' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point empty' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point empty' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point empty' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point empty' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point(1 1 1)' <@ tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1 1)' <@ tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1 1)' <@ tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1 1)' <@ tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT geography 'Point Z empty' <@ tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point Z empty' <@ tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point Z empty' <@ tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point Z empty' <@ tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestamptz '2000-01-01' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestamptz '2000-01-01' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestamptz '2000-01-01' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' <@ tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' <@ tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' <@ tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' <@ tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' <@ tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' <@ tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <@ tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' <@ tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ geometry 'Point(1 1)';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ geometry 'Point(1 1)';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ geometry 'Point(1 1)';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ geometry 'Point(1 1)';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ geometry 'Point empty';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ geometry 'Point empty';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ geometry 'Point empty';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ geometry 'Point empty';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ geometry 'Point(1 1 1)';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ geometry 'Point(1 1 1)';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ geometry 'Point(1 1 1)';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ geometry 'Point(1 1 1)';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ geometry 'Point Z empty';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ geometry 'Point Z empty';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ geometry 'Point Z empty';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ geometry 'Point Z empty';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ geography 'Point(1 1)';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ geography 'Point(1 1)';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ geography 'Point(1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ geography 'Point(1 1)';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ geography 'Point empty';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ geography 'Point empty';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ geography 'Point empty';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ geography 'Point empty';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' <@ geography 'Point(1 1 1)';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' <@ geography 'Point(1 1 1)';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' <@ geography 'Point(1 1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' <@ geography 'Point(1 1 1)';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' <@ geography 'Point Z empty';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' <@ geography 'Point Z empty';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' <@ geography 'Point Z empty';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' <@ geography 'Point Z empty';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' <@ timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' <@ tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' <@ tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

/* Errors */
SELECT geometry 'SRID=5676;Point(1 1)' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ geometry 'SRID=5676;Point(1 1)';
SELECT tgeompoint 'Point(1 1)@2000-01-01' <@ geometry 'Point(1 1 1)';
SELECT tgeompoint 'SRID=5676;Point(1 1)@2000-01-01' <@ tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' <@ tgeompoint 'Point(1 1)@2000-01-01';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1)' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point(1 1)' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point(1 1)' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point empty' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point empty' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT geometry 'Point empty' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT geometry 'Point empty' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT geometry 'Point(1 1 1)' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point(1 1 1)' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point(1 1 1)' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geometry 'Point Z empty' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT geometry 'Point Z empty' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT geometry 'Point Z empty' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT geometry 'Point Z empty' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT geography 'Point(1 1)' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1)' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1)' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1)' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point empty' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT geography 'Point empty' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT geography 'Point empty' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT geography 'Point empty' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT geography 'Point(1 1 1)' ~= tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point(1 1 1)' ~= tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point(1 1 1)' ~= tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point(1 1 1)' ~= tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT geography 'Point Z empty' ~= tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01';
SELECT geography 'Point Z empty' ~= tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}';
SELECT geography 'Point Z empty' ~= tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]';
SELECT geography 'Point Z empty' ~= tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestamptz '2000-01-01' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestamptz '2000-01-01' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestamptz '2000-01-01' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestamptz '2000-01-01' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestamptz '2000-01-01' ~= tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestamptz '2000-01-01' ~= tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestamptz '2000-01-01' ~= tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestamptz '2000-01-01' ~= tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' ~= tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' ~= tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' ~= tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT gbox 'GBOX((1.0, 2.0), (1.0, 2.0))' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint 'Point(1.5 1.5)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}';

SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint 'Point(1 1 1)@2000-01-01';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))' ~= tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= geometry 'Point(1 1)';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= geometry 'Point(1 1)';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= geometry 'Point(1 1)';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= geometry 'Point(1 1)';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= geometry 'Point empty';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= geometry 'Point empty';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= geometry 'Point empty';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= geometry 'Point empty';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= geometry 'Point(1 1 1)';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= geometry 'Point(1 1 1)';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= geometry 'Point(1 1 1)';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= geometry 'Point(1 1 1)';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= geometry 'Point Z empty';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= geometry 'Point Z empty';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= geometry 'Point Z empty';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= geometry 'Point Z empty';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= geography 'Point(1 1)';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= geography 'Point(1 1)';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= geography 'Point(1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= geography 'Point(1 1)';

SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= geography 'Point empty';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= geography 'Point empty';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= geography 'Point empty';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= geography 'Point empty';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' ~= geography 'Point(1 1 1)';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' ~= geography 'Point(1 1 1)';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' ~= geography 'Point(1 1 1)';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' ~= geography 'Point(1 1 1)';

SELECT tgeogpoint 'Point(1.5 1.5 1.5)@2000-01-01' ~= geography 'Point Z empty';
SELECT tgeogpoint '{Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03}' ~= geography 'Point Z empty';
SELECT tgeogpoint '[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03]' ~= geography 'Point Z empty';
SELECT tgeogpoint '{[Point(1.5 1.5 1.5)@2000-01-01, Point(2.5 2.5 2.5)@2000-01-02, Point(1.5 1.5 1.5)@2000-01-03],[Point(3.5 3.5 3.5)@2000-01-04, Point(3.5 3.5 3.5)@2000-01-05]}' ~= geography 'Point Z empty';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= timestamptz '2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= timestamptz '2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= timestamptz '2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= timestamptz '2000-01-01';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' ~= timestamptz '2000-01-01';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= timestamptz '2000-01-01';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= timestamptz '2000-01-01';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= timestamptz '2000-01-01';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= timestampset '{2000-01-01, 2000-01-03}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= timestampset '{2000-01-01, 2000-01-03}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= period '[2000-01-01,2000-01-02]';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= period '[2000-01-01,2000-01-02]';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= gbox 'GBOX((1.0, 2.0), (1.0, 2.0))';
SELECT tgeogpoint 'Point(1.5 1.5)@2000-01-01' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03}' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03]' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1.5 1.5)@2000-01-01, Point(2.5 2.5)@2000-01-02, Point(1.5 1.5)@2000-01-03],[Point(3.5 3.5)@2000-01-04, Point(3.5 3.5)@2000-01-05]}' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= gbox 'GBOX Z((1.0, 2.0, 2.0), (1.0, 2.0, 2.0))';
SELECT tgeogpoint 'Point(1 1 1)@2000-01-01' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';
SELECT tgeogpoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= gbox 'GEODBOX((1.0, 2.0, 3.0), (1.0, 2.0, 3.0))';

SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03}' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03]' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}' ~= tgeompoint '{[Point(1 1)@2000-01-01, Point(2 2)@2000-01-02, Point(1 1)@2000-01-03],[Point(3 3)@2000-01-04, Point(3 3)@2000-01-05]}';

SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= tgeompoint 'Point(1 1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03}' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03]' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';
SELECT tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}' ~= tgeompoint '{[Point(1 1 1)@2000-01-01, Point(2 2 2)@2000-01-02, Point(1 1 1)@2000-01-03],[Point(3 3 3)@2000-01-04, Point(3 3 3)@2000-01-05]}';

/* Errors */
SELECT geometry 'SRID=5676;Point(1 1)' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT geometry 'Point(1 1 1)' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= geometry 'SRID=5676;Point(1 1)';
SELECT tgeompoint 'Point(1 1)@2000-01-01' ~= geometry 'Point(1 1 1)';
SELECT tgeompoint 'SRID=5676;Point(1 1)@2000-01-01' ~= tgeompoint 'Point(1 1)@2000-01-01';
SELECT tgeompoint 'Point(1 1 1)@2000-01-01' ~= tgeompoint 'Point(1 1)@2000-01-01';

-------------------------------------------------------------------------------
