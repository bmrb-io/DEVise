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

/* ............................................................................
module name:  inengine.h

This module contains #defines and declarations for the inference net
engine interface.  Functions and variables declared here are defined
in inengine.c.
.............................................................................*/
#if !defined(INENGINE_H)
#define INENGINE_H

#include "docdef.h"
#include "doctypes.h"
#include "in_node.h"
#include "inq_api.h"

static char inengine_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#include "keyref.h"
#include "keycalls.h"

#if !defined (INT_MAX)
#include <limits.h>
#endif

/*
 *  Old inquery.h stuff.
 */

#define maxint          INT_MAX
#define node_name_lc    16
#define int_lc sizeof(int)
#define fcb_lc min_fcb_lc+5*block_lc
#define buffer_lc      128
#define stage_lc       128
#define int_lc sizeof(int)
#define max_query 300

/* #define max_rel removed -- multiply #defined here and in relevance.h;
 * moved to a command line option in inqr-b. 
 */
/*#define max_docs 20000         (* 3500 */

/* #define save_ranks       false   */ /* used excl. by inqueryb/relevance - moved 
   to new inqueryb code */
/* #define save_top_docs    false  */ /* used excl. by inqueryb/relevance - moded
   to new inqueryb code */
#define trace_booleans   false   /* true */
#define trace_allocation false

#define trace_top_docs  0

#define UPDATE_CONCURRENCY(db) \
     if (db->concurrency) { \
       int last_doc_cnt = db->doc_cnt;\
       update_db_from_inf(db); \
       if (db->doc_cnt != last_doc_cnt) {\
	 dm_refresh(db->docman_handle);\
	 if (db->ndx_handle)\
	   refresh_ndx(db);\
       }\
     }

/*
 *  Global variables (defined in inengine.c):
 */
extern dbinfo *current_db;	 /* stats for, and ptrs to, current database */
extern  char  btl_fcb[fcb_lc];   /* fcb for belief file */

/*
 *  INQ_NUM_WORDS_IN_COLLECTION returns the number of words in the open
 *  document collection.
 */
#define inq_num_words_in_collection() (current_db->word_cnt)

/*
 *  INQ_PROX_FREQUENCY_IN_DOCS returns the number of times that the specified
 *  proximity operator matches a specified list of documents.  The list_of_docs
 *  must be in sorted order, otherwise results will be inaccurate.  The caller
 *  provides an empty list in which to store the list of frequencies.  Note
 *  that the prox_net argument is destructively altered by this function (it
 *  is consumed and then freed).
 */
#define inq_prox_frequency_in_docs(prox_query_net,  number_of_docs, \
				   list_of_docs,    list_of_frequencies, \
				   collection_freq, doc_cnt) \
       inq_term_frequency_in_docs (prox_query_net,  number_of_docs, \
				   list_of_docs,    list_of_frequencies, \
				   collection_freq, doc_cnt)

Boolean_t opendb(dbinfo *db);
int is_database_open(dbinfo *db);  /* determines whether db is opened */
void   closedb(dbinfo *db);
void rank_all_docs(struct belief_list_type *r);
void rank_docs(struct belief_list_type *r);
void init_highlighting_fix(BoolInt_t flag);
BoolInt_t use_highlighting_fix();
extern IN_node_type *monitor_eval_query(IN_node_type *n, void (*function)());
extern belieflst *eval_query_net(IN_node_type *n, feedback_function_t feedback_function);
extern belieflst *eval_query(Char_t *q, feedback_function_t feedback_function);
extern belieflst *eval_query_top_n(Char_t *q, feedback_function_t feedback_function, Int_t);
extern belieflst *re_eval_query_all(Char_t *q, Int_t iterations,
		                doc_rel_t *documents,
				feedback_function_t feedback_function);
void   inq_term_frequency_in_docs (IN_node_type *term_query_net,
	  			   Int_t          number_of_docs,
				   Int_t          *list_of_docs,
				   Int_t          *list_of_frequencies,
				   Int_t          *collection_freq,
				   Int_t          *doc_cnt);
belieflst *inq_re_eval_query_top_n (dbinfo *db, Char_t *q, Int_t iterations, 
				    doc_rel_t *docs,
				    feedback_function_t feedback_function,
				    Int_t max_docs_to_retrieve);


/* Transaction functions: */
Int_t in_trans_begin(dbinfo *db, int trans_type);
Int_t in_trans_end(dbinfo *db);
Int_t in_trans_abort(dbinfo *db);

/* 
 *  GET_QUERYNET returns a query network corresponding to the input string.
 *  If the input string is not a valid query, a NULL query net is returned.
 */
#define get_querynet(query_string) (parse_qry (query_string))
IN_node_type *parse_qry (UChar_t *query_string);

/*
 *  Values for MATCH parameter of Q_WHERE_QUERY_MATCHES_DOC function.
 *  Note that all values must be a power of 2, to allow for logical and.
 */
#ifndef MATCH_LOCS
  #define MATCH_LOCS	  (8)
  #define MATCH_PASSAGES  (16)
#endif

extern void initialize_query_info();
extern void finish_query_info();
extern query_info_t *get_query_info();

Boolean_t mft_existence(char *term);

#endif
