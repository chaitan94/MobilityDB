/*****************************************************************************
 *
 * Parser.c
 *	  Functions for parsing static network types.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "TemporalNPoint.h"

/*****************************************************************************/

npoint *
npoint_parse(char **str)
{
	p_whitespace(str);

	if (strncasecmp(*str,"NPOINT",6) != 0)
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			errmsg("Could not parse network point")));

	*str += 6;
	p_whitespace(str);

	int delim = 0;
	while ((*str)[delim] != ')' && (*str)[delim] != '\0')
		delim++;
	if ((*str)[delim] == '\0')
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			errmsg("Could not parse network point")));

	int64 rid;
	double pos;
	if (sscanf(*str, "(%ld,%lf)", &rid, &pos) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("Could not parse network point")));
	if (pos < 0 || pos > 1)
		ereport(ERROR,
			(errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("the relative position must be a real number between 0 and 1")));

	*str += delim + 1;

	npoint *result = (npoint *)palloc(sizeof(npoint));
	result->rid = rid;
	result->pos = pos;
	return result;
}

nsegment *
nsegment_parse(char **str)
{
	p_whitespace(str);

	if (strncasecmp(*str,"NSEGMENT",8) != 0)
		ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			errmsg("Could not parse network segment")));

	*str += 8;
	p_whitespace(str);

	int delim = 0;
	while ((*str)[delim] != ')' && (*str)[delim] != '\0')
		delim++;
	if ((*str)[delim] == '\0')
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("Could not parse network segment")));

	int64		rid;
	double	  pos1;
	double	  pos2;
	if (sscanf(*str, "(%ld,%lf,%lf)", &rid, &pos1, &pos2) != 3)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("Could not parse network segment")));
	if (pos1 < 0 || pos1 > 1 || pos2 < 0 || pos2 > 1)
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
						errmsg("the relative position must be a real number between 0 and 1")));

	*str += delim + 1;

	nsegment *result = (nsegment *)palloc(sizeof(nsegment));
	result->rid = rid;
	result->pos1 = Min(pos1, pos2);
	result->pos2 = Max(pos1, pos2);
	return result;
}

/*****************************************************************************/
