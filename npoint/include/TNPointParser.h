/*****************************************************************************
 *
 * TNPointParser.h
 *	  Functions for parsing static network types.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse, 
 * 		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#ifndef __TNPOINTPARSER_H__
#define __TNPOINTPARSER_H__

#include "Temporal.h"
#include "TNPoint.h"

/*****************************************************************************/

extern npoint *npoint_parse(char **str);
extern nsegment *nsegment_parse(char **str);
extern Temporal *tnpoint_parse(char **str, Oid basetype);

/*****************************************************************************/

#endif
