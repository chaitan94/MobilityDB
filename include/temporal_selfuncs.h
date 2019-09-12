/*****************************************************************************
 *
 * temporal_selfuncs.h
 * 	Selectivity functions for the temporal types
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Mahmoud Sakr, Mohamed Bakli
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TEMPORAL_SELFUNCS_H__
#define __TEMPORAL_SELFUNCS_H__

#include <postgres.h>
#include <catalog/pg_operator.h>
#include <commands/vacuum.h>
#include <utils/lsyscache.h>
#include <utils/rangetypes.h>
#include <utils/selfuncs.h>
#include <utils/typcache.h>

#include "temporal.h"
#include "oidcache.h"

#define BTREE_AM_OID   403

#define DEFAULT_TEMP_SELECTIVITY 0.001

/*****************************************************************************
 * Internal selectivity functions for Temporal types.
 *****************************************************************************/

extern Selectivity temporal_bbox_sel(PlannerInfo *root, VariableStatData *vardata,
	Period *period, CachedOp cachedOp);
extern Selectivity temporal_position_sel(PlannerInfo *root, VariableStatData *vardata,
	Period *period, bool isgt, bool iseq, CachedOp operator);

extern Selectivity temporalinst_sel(PlannerInfo *root, VariableStatData *vardata,
	Period *period, CachedOp cachedOp);
extern Selectivity temporali_sel(PlannerInfo *root, VariableStatData *vardata,
	Datum constvalue, CachedOp cachedOp);
extern Selectivity temporals_sel(PlannerInfo *root, VariableStatData *vardata,
	Period *period, CachedOp cachedOp);


/*****************************************************************************/

extern Datum temporal_sel(PG_FUNCTION_ARGS);
extern Datum temporal_joinsel(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif