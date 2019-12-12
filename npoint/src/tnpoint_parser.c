/*****************************************************************************
 *
 * tnpoint_parser.c
 *	  Functions for parsing static network types.
 *
 * Portions Copyright (c) 2019, Esteban Zimanyi, Arthur Lesuisse,
 *		Universite Libre de Bruxelles
 * Portions Copyright (c) 1996-2019, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *****************************************************************************/

#include "tnpoint_parser.h"

#include "temporaltypes.h"
#include "temporal_parser.h"
#include "tnpoint.h"
#include "tnpoint_static.h"

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
	if (sscanf(*str, "( %ld , %lf )", &rid, &pos) != 2)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("Could not parse network point")));
	if (pos < 0 || pos > 1)
		ereport(ERROR,
			(errcode(ERRCODE_INTERNAL_ERROR),
			errmsg("the relative position must be a real number between 0 and 1")));

	*str += delim + 1;

	return npoint_make(rid, pos);
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

	int64 rid;
	double pos1;
	double pos2;
	if (sscanf(*str, "( %ld , %lf , %lf )", &rid, &pos1, &pos2) != 3)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
						errmsg("Could not parse network segment")));
	if (pos1 < 0 || pos1 > 1 || pos2 < 0 || pos2 > 1)
		ereport(ERROR,
				(errcode(ERRCODE_INTERNAL_ERROR),
						errmsg("the relative position must be a real number between 0 and 1")));

	*str += delim + 1;

	return nsegment_make(rid, pos1, pos2);
}

static TemporalSeq *
tnpointseq_parse(char **str, Oid basetype, bool end)
{
	p_whitespace(str);
	bool lower_inc = false, upper_inc = false;
	if (p_obracket(str))
		lower_inc = true;
	else if (p_oparen(str))
		lower_inc = false;
	else
		ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				errmsg("Could not parse temporal value")));

	//FIXME: parsing twice
	char *bak = *str;
	TemporalInst *inst = temporalinst_parse(str, basetype, false);
	int64 rid0 = DatumGetNpoint(temporalinst_value(inst))->rid;
	int count = 1;
	while (p_comma(str))
	{
		count++;
		pfree(inst);
		inst = temporalinst_parse(str, basetype, false);
		int64 rid = DatumGetNpoint(temporalinst_value(inst))->rid;
		if (rid != rid0)
		{
			pfree(inst);
			ereport(ERROR, (errcode(ERRCODE_RESTRICT_VIOLATION),
					errmsg("Temporal sequence must have same rid")));
		}
	}
	pfree(inst);
	if (p_cbracket(str))
		upper_inc = true;
	else if (p_cparen(str))
		upper_inc = false;
	else
		ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				errmsg("Could not parse temporal value")));
	if (end)
	{
		/* Ensure there is no more input */
		p_whitespace(str);
		if (**str != 0)
			ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
				errmsg("Could not parse temporal value")));
	}
	/* Second parsing */
	*str = bak;
	TemporalInst **insts = palloc(sizeof(TemporalInst *) * count);
	for (int i = 0; i < count; i++)
	{
		p_comma(str);
		insts[i] = temporalinst_parse(str, basetype, false);
	}
	p_cbracket(str);
	p_cparen(str);
	TemporalSeq *result = temporalseq_from_temporalinstarr(insts,
		count, lower_inc, upper_inc, true, true);

	for (int i = 0; i < count; i++)
		pfree(insts[i]);
	pfree(insts);

	return result;
}

static TemporalS *
tnpoints_parse(char **str, Oid basetype)
{
	p_whitespace(str);
	if (!p_obrace(str))
		ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
			errmsg("Could not parse temporal value")));

	//FIXME: parsing twice
	char *bak = *str;
	TemporalSeq *seq = tnpointseq_parse(str, basetype, false);
	int count = 1;
	while (p_comma(str))
	{
		count++;
		pfree(seq);
		seq = tnpointseq_parse(str, basetype, false);
	}
	pfree(seq);
	if (!p_cbrace(str))
		ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				errmsg("Could not parse temporal value")));
	/* Ensure there is no more input */
	p_whitespace(str);
	if (**str != 0)
		ereport(ERROR, (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION), 
			errmsg("Could not parse temporal value")));
	/* Second parsing */
	*str = bak;
	TemporalSeq **seqs = palloc(sizeof(TemporalSeq *) * count);
	for (int i = 0; i < count; i++)
	{
		p_comma(str);
		seqs[i] = tnpointseq_parse(str, basetype, false);
	}
	p_cbrace(str);
	TemporalS *result = temporals_from_temporalseqarr(seqs, count, 
		true, true);

	for (int i = 0; i < count; i++)
		pfree(seqs[i]);
	pfree(seqs);

	return result;
}

Temporal *
tnpoint_parse(char **str, Oid basetype) 
{
	p_whitespace(str);
	
	/* Determine the type of the temporal point */
	if (**str != '{' && **str != '[' && **str != '(')
		return (Temporal *)temporalinst_parse(str, basetype, true);
	else if (**str == '[' || **str == '(')
		return (Temporal *)tnpointseq_parse(str, basetype, true);		
	else if (**str == '{')
	{
		char *bak = *str;
		p_obrace(str);
		p_whitespace(str);
		if (**str == '[' || **str == '(')
		{
			*str = bak;
			return (Temporal *)tnpoints_parse(str, basetype);
		}
		else
		{
			*str = bak;
			return (Temporal *)temporali_parse(str, basetype);		
		}
	}
	return NULL; /* keep compiler quiet */
}

/*****************************************************************************/
