FROM kartoza/postgis:11.5-2.8
ENV POSTGRES_DBNAME=mobilitydb
ENV POSTGRES_MULTIPLE_EXTENSIONS=postgis,hstore,postgis_topology,mobilitydb
ENV EXTRA_CONF="shared_preload_libraries = 'pg_cron,postgis-2.5'\nmax_locks_per_transaction = 150\n"
RUN apt-get update \
 && apt-get install -y \
    build-essential \
    cmake \
    git \
    libpq-dev \
    liblwgeom-dev \
    libproj-dev \
    libjson-c-dev \
    libgsl-dev \
    libgslcblas0 \
    postgresql-server-dev-11
# RUN git clone https://github.com/MobilityDB/MobilityDB.git -b develop /usr/local/src/MobilityDB --depth=1
ADD . /usr/local/src/MobilityDB
RUN mkdir /usr/local/src/MobilityDB/build && \
    cd /usr/local/src/MobilityDB/build && \
    cmake .. && \
    make && \
    make install && \
    rm -rf /usr/local/src/MobilityDB
