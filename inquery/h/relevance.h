/**************************************************************************
*                         Copyright (c) 1996                              *
* by Sovereign Hill Software, Inc., Amherst, Massachusetts, U.S.A.        *
*        (413-256-6916;   http://www.sovereign-hill.com)                  *
*                          All Rights Reserved.                           *
*                                                                         *
* The SOFTWARE was developed in part at the Center for Intelligent        *
* Information Retrieval (CIIR) at the University of Massachusetts at      *
* Amherst (For more information, contact 413-545-0463 or                  *
* http://ciir.cs.umass.edu).  The SOFTWARE was acquired by Sovereign      *
* Hill Software Inc. and is the property of Sovereign Hill Software.      *
* ACSIOM is authorized, under an agreement with Sovereign Hill Software,  *
* to license the SOFTWARE for non-commercial, non-revenue-producing       *
* prototyping and internal evaluation purposes only.                      *
*                                                                         *
* This SOFTWARE was developed in significant part with private funding.   *
* The INQUERY SOFTWARE is proprietary to Sovereign Hill Software and may  *
* not be distributed in whole or in part or used in any way without       *
* authorized permission in writing from Sovereign Hill Software.          *
* Possession, use, duplication or dissemination of the INQUERY SOFTWARE   *
* and media is authorized only pursuant to a valid written license from   *
* ACSIOM or Sovereign Hill Software, Inc.                                 *
*                                                                         *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.  *
* Government is subject to restrictions as set forth in subparagraph      *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.              *
**************************************************************************/ 

#if !defined(RELEVANCE_H)
#define RELEVANCE_H

#include "inengine.h"
#define max_sort_stack 20

static char relevance_h_rcsid[]="$RCSfile$, $Revision$, $Date$";


extern Int_t 
  docs_seen, 
  query_cnt, 
  i_max_rel,
  total_rel_cnt,
  rel_query_id,        /* query being read from relevance file */
  rel_doc_id,          /* current relevant doc_id */
  *rel_ranks;			/* ranks for relevant docs */

extern Boolean_t save_ranks, save_top_docs, switch_verb_warn;


extern FILE
  *rank_file,
  *top_file;

extern float
  rank_denominator,    /* holds n(n^2-1) for rank coeff computation */
  rank_coeff_sum;  /* holds sum of rank coefficients when both prob */
  /*    and booleans are being combined */

extern Int_t 
  rel_cnt,           /* the number of entries in the rel_list */
  *rel_list;        /* list of the ids of relevant documents */

extern BoolInt_t *rel_status;        /* relevant or not? */

/* prototypes */
void write_result_summary(Int_t rel_cnt, Int_t rel_list[],
			  struct belief_list_type *r);
void write_combined_summary(Int_t rel_cnt, Int_t rel_list[],
			    struct belief_list_type *p, 
			    struct belief_list_type *b,
			    struct belief_list_type *r);
void write_results(Int_t query_id, struct belief_list_type *result,
		   Int_t rel_cnt, Int_t *rel_list, 
		   FILE *eval_file);
void read_relevance_list(dbinfo *db, Int_t query_id, Int_t *rel_cnt, Int_t rel_list[],
			 FILE *rel_file);
void write_top_n_docs(FILE *stream, struct belief_list_type *r,Int_t n);
/* write out the query, iteration, doc id, rank and state */
/* (the document's relevance, and whether or not it was judged) */
void write_doc_ranks(Int_t query_id, Int_t query_iteration, FILE *evl2_file,
		     Int_t rel_cnt, Int_t rel_list[], 
		     struct belief_list_type *r);

/* predicate to tell if this document has been judged relevant */
/* looks at the file of relevance judgements */
BoolInt_t relevant(Int_t doc_id, Int_t rel_cnt, Int_t *rel_list);

/* sort a belief list by its beliefs */
void rank_sort_belief_list(dbinfo *, 
			   struct belief_list_type *r);

/* init the file of relevance judgements */
void init_rel_file(dbinfo *db, FILE *rel_file);

/* turn on/off and check option to lowercase external ids in relevance file */
Int_t get_lowercase_xids_flag();
Int_t set_lowercase_xids_flag(BoolInt_t flag);

void free_rel_file(FILE *relf);
#endif

