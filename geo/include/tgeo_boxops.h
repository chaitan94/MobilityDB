#ifndef __TGEO_BOXOPS_H__
#define __TGEO_BOXOPS_H__

#include <postgres.h>
#include <catalog/pg_type.h>
#include <liblwgeom.h>
#include "temporal.h"

/*****************************************************************************/

extern void tregioninst_make_stbox(STBOX *box, Datum value, TimestampTz t);
extern void tregioninstarr_to_stbox(STBOX *box, TemporalInst **inst, int count);

/*****************************************************************************/

#endif