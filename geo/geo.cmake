target_link_libraries(${CMAKE_PROJECT_NAME} ${HAS_LWGEOM}) # postgis)
add_definitions(-DWITH_POSTGIS)

include_directories("geo/include")

file(GLOB SRCGEOM "geo/src/*.c")
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${SRCGEOM})

file(GLOB SQLGEOM "geo/src/sql/*.in.sql")
list(SORT SQLGEOM)
set(SQL "${SQL};${SQLGEOM}")
set(CONTROLIN "${CONTROLIN};point/control.in;geo/control.in")
