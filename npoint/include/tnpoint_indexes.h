/*****************************************************************************
 *
 * tnpoint_indexes.h
 *	  R-tree GiST and SP-GiST indexes for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_INDEXES_H__
#define __TNPOINT_INDEXES_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum gist_tnpoint_compress(PG_FUNCTION_ARGS);
extern Datum spgist_tnpoint_compress(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif 
