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


#ifndef IL_H
#define IL_H

/*****************/
/* INCLUDE FILES */
/*****************/
#include <stdio.h>
#include "in_node.h"  /* In a perfect world the ILM would not know about
			 prox lists ... */

/*****************/
/* MACRO DEFINES */
/*****************/

#define INQ_TXN_READ   0
#define INQ_TXN_WRITE  1

#define INQ_NUM_RETRY 3

#define IL_READ_ONLY  0
#define IL_READ_WRITE 1
#define IL_CREATE     2


typedef struct doc_range doc_range_t;
struct doc_range
{
  Int_t  first;
  Int_t  last;
};


typedef enum {IL_KEYFILE, IL_MNEME} il_type_t;

typedef enum {IL_BTL_DB, IL_NDX_DB} ildb_type_t;


/***********************/
/* FUNCTION PROTOTYPES */
/***********************/

void   *il_open(char *filename, ildb_type_t type, int mode); 
int     il_close( void *btl_dscr_arg, int mode);
int     il_btl_init( );
int     il_btl_term();
int     il_set_btl_extension (Char_t *name );
int     il_get_list (void *btl_dscr_arg, Char_t *term_str, void **il_dscr_arg,
		     Int_t *coll_freq, Int_t *doc_freq, Int_t *next_doc);
int     il_get_doc_tf(void *il_ptr, Int_t doc_id,
		      Int_t *max_tf, Int_t *next_doc);
int     il_get_doc_prox(void *il_ptr, Int_t doc_id, prox_list_t **prox, 
			Int_t *next_doc);
int     il_reset_list (void *il_ptr);
int     il_free (void *il_ptr);
float il_mark_docs (void *il_dscr_arg, belief_elt *doc_list,
			   Int_t max_top);
il_type_t il_get_type (void);

Int_t il_purge_deleted_docs (void *btl_dscr_arg, Int_t num_docs,
				    doc_range_t *range_list, void *crf_handle,
                                    FILE *crf_file, Int_t batch_n);
Int_t il_get_first_term_for_prev(void *btl_dscr_arg, Char_t *ref_term);
Int_t il_get_first_term_for_next(void *btl_dscr_arg, Char_t *ref_term, 
				 Char_t **first_term);

/*
void *il_open(char *filename, int mode); 
int   il_btl_init( );
Int_t  il_set_btl_extension (Char_t *name );
int   il_close( void *btl_dscr_arg, int mode);
int   il_btl_term();
*/

void   *il_open_f(char *filename, int mode); 

int	il_close_f (void *ildb_handle, int mode);
int     il_get_list_f (void *btl_dscr_arg, char *term_str, void **il_dscr_arg,
		     int *coll_freq, int *doc_freq, int *next_doc,
		     Boolean_t isField, Boolean_t isRange, relop_enum_type ind_po);
int     il_get_doc_tf_f(void *il_ptr, int doc_id,
		      int *max_tf, int *next_doc);
int     il_get_doc_prox_f(void *il_ptr, int doc_id, prox_list_t **prox, int *next_doc);
int     il_reset_list_f (void *il_ptr);

int il_begin_transaction(void *ildb_handle, int trans_type);
int il_abort_transaction(void *ildb_handle);
int il_end_transaction(void *ildb_handle);
int il_cmp_version(void *ildb_handle, double version);
Int_t il_next_term_stats (void *btl_dscr_arg, Char_t *ref_term, 
                          Int_t *ndoc_cnt, Int_t *ncoll_freq, Int_t *nonterm);

Int_t il_next_term (void *btl_dscr_arg, Char_t *ref_term, Char_t **term);
Int_t il_prev_term(void *btl_dscr_arg, Char_t *ref_term, Char_t *term, int size);
int il_free_list (void *il_ptr);
Int_t il_get_term_stats(void *ilm_handle, char *term, Int_t *tf, Int_t *df);
#endif





