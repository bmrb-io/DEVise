/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/

/*
 * QSCAN_TABLE.C is a C file where the query text scanners are
 * declared and defined.  A query parser should declare
 *
 *   extern void (*transformers[])()
 * and call them one by one (the array has a NULL terminal sentinal
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "doctypes.h"
#include "qscan.h"
#include "language.h"

/*
 *  RCS identifier for module:
 */
static char qscan_table_c_rcsid[]="$RCSfile$, $Revision$, $Date$";

/* transformers = text processors */ 

extern 	void qscan_stop_phrase_lex (char *query);
extern 	void qscan_split_compounds_lex (char *query);
/* extern 	void qscan_keywords_yylex (char *query);*/
extern void qscan_location_lex(char *query);
extern void qscan_loc_simp_lex(char *query);
extern void qscan_co_simp_lex(char *query);
extern void qco_tags_lex(char *query);
extern void qscan_retain_capstops (char *query);
extern void qscan_squash_locops_lex (char *query);

qscaninfo transformers[] = {  	
  { 0, qscan_stop_phrase_lex, "stop_phrases" },
  { 0, qscan_location_lex,"location"  },
  { 1, qscan_loc_simp_lex,"simple_location"  },
  { 1, qscan_co_simp_lex,"simple_company"  },
  { 0, qscan_split_compounds_lex, "hyphens_caps" },
  { 0, qco_tags_lex, "company_tags" },
  { 0, qscan_retain_capstops, "retain_capitalized_stopwords"},
  { 0, qscan_squash_locops_lex, "squash_loc_ops"},
  { 0, NULL, ""}
};

lang_init_t language_initializers[]=
{  
  { LANGUAGE_UNKNOWN, NULL }
};


