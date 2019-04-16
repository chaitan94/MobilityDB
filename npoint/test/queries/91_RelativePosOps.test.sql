﻿-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' << tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' << tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' << tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' << tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' << geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' << geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' << geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' << geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' << tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' << tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' << tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' << tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' << tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' << tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' << tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' << tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' << tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' << tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' << tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' << tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' << tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' << tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' << tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' << tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' >> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' >> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' >> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' >> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' >> geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' >> geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' >> geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' >> geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' >> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' >> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' >> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' >> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' >> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' >> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' >> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' >> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' >> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' >> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' >> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' >> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' >> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' >> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' >> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' >> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' &< tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' &< tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' &< tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' &< tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &< geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &< geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &< geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &< geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &< tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &< tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &< tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &< tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &< tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &< tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &< tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &< tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &< tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &< tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &< tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &< tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &< tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &< tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &< tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &< tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' &> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' &> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' &> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' &> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &> geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &> geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &> geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &> geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' <<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' <<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' <<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' <<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<| geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<| geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<| geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<| geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' |>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' |>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' |>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' |>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |>> geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |>> geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |>> geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |>> geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' &<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' &<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' &<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' &<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<| geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<| geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<| geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<| geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<| tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<| tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<| tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<| tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

-------------------------------------------------------------------------------

SELECT geometry 'Point(1 1)' |&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT geometry 'Point(1 1)' |&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT geometry 'Point(1 1)' |&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT geometry 'Point(1 1)' |&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |&> geometry 'Point(1 1)';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |&> geometry 'Point(1 1)';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |&> geometry 'Point(1 1)';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |&> geometry 'Point(1 1)';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' |&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' |&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' |&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' |&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT timestamp '2000-01-01' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestamp '2000-01-01' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestamp '2000-01-01' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestamp '2000-01-01' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# timestamp '2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# timestamp '2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# timestamp '2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# timestamp '2000-01-01';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# timestampset '{2000-01-01, 2000-01-03}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# period '[2000-01-01,2000-01-02]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# period '[2000-01-01,2000-01-02]';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' <<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT timestamp '2000-01-01' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestamp '2000-01-01' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestamp '2000-01-01' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestamp '2000-01-01' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> timestamp '2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> timestamp '2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> timestamp '2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> timestamp '2000-01-01';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> timestampset '{2000-01-01, 2000-01-03}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> period '[2000-01-01,2000-01-02]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> period '[2000-01-01,2000-01-02]';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #>> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT timestamp '2000-01-01' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestamp '2000-01-01' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestamp '2000-01-01' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestamp '2000-01-01' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# timestamp '2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# timestamp '2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# timestamp '2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# timestamp '2000-01-01';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# timestampset '{2000-01-01, 2000-01-03}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# period '[2000-01-01,2000-01-02]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# period '[2000-01-01,2000-01-02]';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' &<# tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------

SELECT timestamp '2000-01-01' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestamp '2000-01-01' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestamp '2000-01-01' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestamp '2000-01-01' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT timestampset '{2000-01-01, 2000-01-03}' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT timestampset '{2000-01-01, 2000-01-03}' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT timestampset '{2000-01-01, 2000-01-03}' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT timestampset '{2000-01-01, 2000-01-03}' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT period '[2000-01-01,2000-01-02]' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT period '[2000-01-01,2000-01-02]' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT period '[2000-01-01,2000-01-02]' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT period '[2000-01-01,2000-01-02]' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> timestamp '2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> timestamp '2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> timestamp '2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> timestamp '2000-01-01';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> timestampset '{2000-01-01, 2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> timestampset '{2000-01-01, 2000-01-03}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> period '[2000-01-01,2000-01-02]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> period '[2000-01-01,2000-01-02]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> period '[2000-01-01,2000-01-02]';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> periodset '{[2000-01-01,2000-01-02],[2000-01-03,2000-01-04]}';

SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> tnpoint 'NPoint(1,0.5)@2000-01-01';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]';
SELECT tnpoint 'NPoint(1,0.5)@2000-01-01' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{NPoint(1,0.5)@2000-01-01, NPoint(2,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03}' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03]' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';
SELECT tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}' #&> tnpoint '{[NPoint(1,0.4)@2000-01-01, NPoint(1,0.5)@2000-01-02, NPoint(1,0.7)@2000-01-03],[Npoint(3,0.5)@2000-01-04, NPoint(3,0.5)@2000-01-05]}';

-------------------------------------------------------------------------------
