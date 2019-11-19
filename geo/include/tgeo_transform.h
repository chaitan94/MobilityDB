#ifndef __TGEO_TRANSFORM_H__
#define __TGEO_TRANSFORM_H__

#include <postgres.h>
#include <catalog/pg_type.h>

#include "tgeo.h"

/*****************************************************************************
 * In/Output
 *****************************************************************************/

extern Datum rtransform_in(PG_FUNCTION_ARGS);
extern Datum rtransform_out(PG_FUNCTION_ARGS);
extern Datum rtransform_recv(PG_FUNCTION_ARGS);
extern Datum rtransform_send(PG_FUNCTION_ARGS);

/*****************************************************************************
 * Constructor functions
 *****************************************************************************/

extern rtransform *rtransform_make(double theta, double tx, double ty);

/*****************************************************************************
 * Cast functions
 *****************************************************************************/

extern TemporalInst **geo_instarr_to_rtransform(TemporalInst **instants, int count);

#endif /* __TGEO_TRANSFORM_H__ */