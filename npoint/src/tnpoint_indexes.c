/*****************************************************************************
 *
 * tnpoint_indexes.c
 *	  R-tree GiST and SP-GiST indexes for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, Xinyang Li
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_indexes.h"

#include <access/gist.h>
#include "TemporalPoint.h"
#include "tnpoint.h"

/*****************************************************************************
 * GiST compress function
 *****************************************************************************/

PG_FUNCTION_INFO_V1(gist_tnpoint_compress);

PGDLLEXPORT Datum
gist_tnpoint_compress(PG_FUNCTION_ARGS)
{
    GISTENTRY* entry = (GISTENTRY *) PG_GETARG_POINTER(0);
    if (entry->leafkey)
    {
        GISTENTRY *retval = palloc(sizeof(GISTENTRY));
   		Temporal *temp = DatumGetTemporal(entry->key);
		STBOX *box = palloc0(sizeof(STBOX));
		temporal_bbox(box, temp);
		gistentryinit(*retval, PointerGetDatum(box), entry->rel, entry->page, 
			entry->offset, false);
		PG_RETURN_POINTER(retval);
	}
	PG_RETURN_POINTER(entry);
}

/*****************************************************************************
 * SP-GiST compress function
 *****************************************************************************/

PG_FUNCTION_INFO_V1(spgist_tnpoint_compress);

PGDLLEXPORT Datum
spgist_tnpoint_compress(PG_FUNCTION_ARGS)
{
	Temporal *temp = PG_GETARG_TEMPORAL(0);
	STBOX *result = palloc0(sizeof(STBOX));
	temporal_bbox(result, temp);
	PG_FREE_IF_COPY(temp, 0);
	PG_RETURN_STBOX_P(result);
}

/*****************************************************************************/
