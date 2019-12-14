/*****************************************************************************
 *
 * tnpoint_posops.c
 *	  Relative position operators for temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_POSOPS_H__
#define __TNPOINT_POSOPS_H__

#include <postgres.h>
#include <catalog/pg_type.h>

/*****************************************************************************/

extern Datum left_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overleft_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum right_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overright_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum below_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overbelow_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum above_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum overabove_npoint_tnpoint(PG_FUNCTION_ARGS);
extern Datum left_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overleft_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum right_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overright_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum below_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overbelow_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum above_tnpoint_npoint(PG_FUNCTION_ARGS);
extern Datum overabove_tnpoint_npoint(PG_FUNCTION_ARGS);

/*****************************************************************************/

#endif /* __TNPOINT_POSOPS_H__ */
