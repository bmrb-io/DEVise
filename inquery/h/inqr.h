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

#if !defined(_INQR_H_)
#define _INQR_H_


/*************/
/*   TYPES   */
/*************/

typedef struct {
  UChar_t *text;
  Int_t number;
} query_list_el;

typedef query_list_el *query_list;

typedef struct {
  query_list_el *list;
  Int_t size;
} query_array;

typedef query_array *query_array_p_t;

/* The TermTab and TermEnt structures are used to return info about terms
   found in a particular document.
 */

struct term_ent
{
    Char_t *term_str;
    Int_t   num_occur;
    Int_t  *position;
};

typedef struct term_ent TermEnt;

struct doc_term_tab
{
    Int_t    num_terms;
    TermEnt *terms;
    Int_t    alph_sorted;
};


typedef struct doc_term_tab TermTab;


/*************************/
/*  FUNCTION PROTOTYPES  */
/*************************/

UChar_t *inqr_get_query_text(FILE *);
Boolean_t inqr_free_query_text (UChar_t *);
query_array_p_t inqr_get_query_array(FILE *);
Boolean_t inqr_free_query_array (query_array_p_t);
void inqr_remove_duplicate_docs (dbinfo *,Int_t, belieflst *);
void inqr_rank_all_docs(dbinfo *, belieflst *);
UChar_t *inqr_get_query_by_count(query_array_p_t, Int_t);
UChar_t *inqr_get_query_by_number(query_array_p_t, Int_t);
Int_t inqr_get_count_of_queries(query_array_p_t);
Int_t inqr_get_highest_query_number(query_array_p_t);
void inqr_write_top_n_docs(dbinfo *, FILE *, belieflst *, Int_t, Boolean_t);
Boolean_t inqr_apply_to_query_net(dbinfo *,Char_t *,void (*applied_function)(),Int_t,Int_t *);
void produce_tf_stats (dbinfo *, IN_node_type *, Int_t, Int_t *);

/* Functions for getting info about terms within a document from the
   parsing system.  These are implemented in parse_rsrch.c.
 */

int  inqr_get_doc_termtab(dbinfo *db, Int_t doc_id, TermTab **termtab);
int  inqr_free_termtab(TermTab *termtab);

int  inqr_termtab_num_terms(TermTab *termtab, Int_t *num_terms);
int  inqr_termtab_find_term(TermTab *termtab, Char_t *term, 
			    Int_t *index, Int_t *num_occur);
int  inqr_termtab_nth_term(TermTab *termtab, int n, Char_t **term, 
			   Int_t *num_occur);
int  inqr_termtab_nth_term_pos(TermTab *termtab, int n, Int_t **pos_list);
int  inqr_termtab_sort_by_freq(TermTab *termtab);


#endif
