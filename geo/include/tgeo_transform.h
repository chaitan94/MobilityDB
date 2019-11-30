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
extern rtransform *rtransform_compute(LWGEOM *region_1, LWGEOM *region_2);

/*****************************************************************************
 * Utility functions
 *****************************************************************************/

extern void apply_rtransform(LWGEOM *region, const rtransform *rt);
extern rtransform *rtransform_interpolate(const rtransform *rt1, const rtransform *rt2, double ratio);

/*****************************************************************************
 * Cast functions
 *****************************************************************************/

extern TemporalInst **geo_instarr_to_rtransform(TemporalInst **instants, int count);
extern TemporalSeq **geo_seqarr_to_rtransform(TemporalSeq **sequences, int count);

#endif /* __TGEO_TRANSFORM_H__ */