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


/* contains the table that directs evaluation of an inference net */

#if !defined(INEVAL_H)
#define INEVAL_H

#include "in_node.h"
#include "doctypes.h"    /* needed for feedback_function_t definition */
#include "matchinfo.h"


static char ineval_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

/* define a pointer to an eval function */
/*typedef IN_node_type *(*eval_function_type)(); */

typedef struct
{
   float I_thresh;  /* mim threshold */
   Int_t tf_thresh; /* doc-count threshold */
} mim_threshold_t;

enum result_type {belief_list_val, prox_list_val};

/**********/
/* MACROS */
/**********/

#define INQ_ALL_DOCS  -1  /* This can be used as the third argument to
                           * inq_ineval
			   */

IN_node_type *ineval(IN_node_type *arg, enum result_type rtype);
IN_node_type *ineval_with_feedback(IN_node_type *arg, enum result_type rtype, feedback_function_t function, int num_req);

/* to override the default tf_score() calculation */
Int_t ineval_set_tf_function(float (*new_tf_fn)());

/* to set the sort mode to be used when evaluating a query */
/*  Sorting is ON or OFF */
Int_t ineval_set_blsort_mode(Boolean_t on_off);

/* The operator functions */
/* be sure to add eval functions to the eval_function table in ineval.c */
IN_node_type *eval_boolean_and(IN_node_type *arg, enum result_type rtype, Int_t docid);
IN_node_type *eval_term(IN_node_type *arg, enum result_type rtype, Int_t docid);

IN_node_type *inq_ineval(IN_node_type *arg, enum result_type rtype, int num_requested);

struct inverted_list_type *allocate_prox_list(Int_t max_inv_l);
prox_list_t               *allocate_small_prox_list (Int_t num_locs);
struct inverted_list_type *realloc_prox_list(struct inverted_list_type *pl, 
					     Int_t count);
void                      sum_merge(struct belief_list_type *in1, 
                                    struct belief_list_type *in2, 
                                    struct belief_list_type *out);
void                      max_merge(struct belief_list_type *in1, 
                                    struct belief_list_type *in2, 
                                    struct belief_list_type *out);
void                      copy_list(struct belief_list_type *work, 
	                            struct belief_list_type *result);
IN_node_type              *copy_IN_tree(IN_node_type *enode);
void                      scale_list(struct belief_list_type *result, 
                                     float s);
void                      free_prox_list(struct inverted_list_type *in);
IN_node_type              *allocate_IN_node();
void                      print_nodes_allocated();

Char_t                    *ineval_operator_name (enum IN_node_enum op);


void			  set_field_subtree (IN_node_type *enode, 
					   relop_enum_type operator, 
					   Int_t prefix);

void  Q_locs_where_query_matches_doc (IN_node_type *query_net, Int_t doc_id,
				      Int_t *number_of_matches,
				      matchinfo_t *matches);
#endif
