/*****************************************************************************
 *
 * tnumber_spgist.c
 *	SP-GiST implementation of 4-dimensional quad tree over temporal
 *	integers and floats.
 *
 * Portions Copyright (c) 2020, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNUMBER_SPGIST_H__
#define __TNUMBER_SPGIST_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum spgist_tnumber_config(PG_FUNCTION_ARGS);
extern Datum spgist_tnumber_choose(PG_FUNCTION_ARGS);
extern Datum spgist_tnumber_picksplit(PG_FUNCTION_ARGS);
extern Datum spgist_tnumber_inner_consistent(PG_FUNCTION_ARGS);
extern Datum spgist_tnumber_leaf_consistent(PG_FUNCTION_ARGS);
extern Datum spgist_tnumber_compress(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif
