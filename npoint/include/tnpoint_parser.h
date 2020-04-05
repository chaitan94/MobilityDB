/*****************************************************************************
 *
 * tnpoint_parser.h
 *	  Functions for parsing static and temporal network points.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2020, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINT_PARSER_H__
#define __TNPOINT_PARSER_H__

#include "temporal.h"
#include "tnpoint.h"

/*****************************************************************************/

extern npoint *npoint_parse(char **str);
extern nsegment *nsegment_parse(char **str);
extern Temporal *tnpoint_parse(char **str, Oid basetype);

/*****************************************************************************/

#endif /* __TNPOINT_PARSER_H__ */
