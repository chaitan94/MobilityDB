#ifndef __TGEO_OUT_H__
#define __TGEO_OUT_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum tgeo_as_text(PG_FUNCTION_ARGS);
extern Datum tgeo_as_ewkt(PG_FUNCTION_ARGS);
extern Datum tgeoarr_as_text(PG_FUNCTION_ARGS);
extern Datum tgeoarr_as_ewkt(PG_FUNCTION_ARGS);

extern char *region_wkt_out(Oid type, Datum value);
extern char *region_ewkt_out(Oid type, Datum value);

/*****************************************************************************/

#endif