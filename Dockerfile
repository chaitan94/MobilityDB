FROM kartoza/postgis:11.0-2.5
ENV POSTGRES_DBNAME=mobilitydb
ENV POSTGRES_MULTIPLE_EXTENSIONS=postgis,hstore,postgis_topology,mobilitydb
ENV EXTRA_CONF="shared_preload_libraries = 'pg_cron,postgis-2.5'\nmax_locks_per_transaction = 150\n"
#WORKDIR /usr/local/src
#ADD . MobilityDB
RUN apt-get update
RUN apt-get remove -y postgresql-11-postgis-3 postgresql-11-postgis-3-scripts
RUN apt-get install -y cmake build-essential libpq-dev liblwgeom-dev libproj-dev libjson-c-dev libgsl-dev libgslcblas0
RUN rm -f /etc/apt/sources.list.d/pgdg.list
RUN apt-get update
RUN apt-get install -y postgresql-server-dev-11
# MobilityDB
RUN git clone https://github.com/ULB-CoDE-WIT/MobilityDB.git -b master /usr/local/src/MobilityDB
RUN rm -rf /usr/local/src/MobilityDB/build  
RUN mkdir /usr/local/src/MobilityDB/build
RUN cd /usr/local/src/MobilityDB/build && \
	cmake .. && \
	make && \
	make install
# PgRouting the develop branch
RUN apt-get update && \
    apt-get install -y git && \
    apt-get install -y cmake    
RUN rm -rf /usr/local/src/pgrouting
RUN apt-get install -y osm2pgsql osm2pgrouting libboost-all-dev sphinxsearch
RUN git clone https://github.com/pgRouting/pgrouting.git -b develop /usr/local/src/pgrouting
RUN rm -rf /usr/local/src/pgrouting/build  
RUN mkdir /usr/local/src/pgrouting/build
RUN cd /usr/local/src/pgrouting/build && \
	cmake .. && \
	make && \
	make install

