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


/*
 *  This header file defines a set of high-level datatypes.  These are the
 *  datatypes that almost everyone needs to know in order to use INQUERY.
 */

#if !defined(QSTRUCTS_H)
#define QSTRUCTS_H

#include "doctypes.h"			/* INQUERY's low-level datatypes */


static char Qstructs_h_rcsid[]="$RCSfile$, $Revision$";


/****************************************************************************
 *  BELIEF LISTS
 ****************************************************************************/
typedef struct belief_element belief_elt;

struct belief_element
  {
  Int_t	doc_id;
  Float_t belief;
  };

struct belief_list_type
  {
  Float_t	    default_belief;
  Int_t	    term_freq;	/* Number of docs (+ citations) term occures in */
  Int_t	     doc_cnt;	/* Number of docs the term occurs in */
  Int_t total_hits;
  belief_elt *list;	/* Belief assigned to each document.  This vector */
                        /* must be big enough to store a belief_elt for   */
                        /* every doc in collection.                       */
  };

typedef struct belief_list_type belieflst;
typedef struct belief_list_type belief_list_t;

#define belief_element_lc (sizeof(belief_elt))


/****************************************************************************
 *  INVERTED (PROX) LISTS
 ****************************************************************************/
/*
 *  If you update this data structure, be sure to update the PUT_PROX_LIST
 *  and GET_PROX_LIST functions that do I/O on them.  Otherwise the data
 *  files may not be readable on some machines.
 */


typedef struct inv_list_elem INV_LIST_ELEM;
struct inv_list_elem		/* inverted list entry for a document */
{
  Int_t  doc_id;		/* document id */
  Int_t  max_freq;		/* max # of occurrences of any term in doc */
  Int_t  term_freq;		/* # of occurrences of this term in doc */
  Int_t  posts[1];		/* locations of term occurrences */
};
/* number of integers in inverted list element header */
#define ILE_HDR_INTS ((sizeof (INV_LIST_ELEM) / sizeof (int)) - 1)


typedef struct doc_wght doc_wght_t;
struct doc_wght
{
  Int_t  doc_id;
  Int_t  max_freq;
  Int_t  term_freq;
};

#define DOC_WGHT_INTS ((Int_t)(sizeof (doc_wght_t) / sizeof (Int_t)))


struct inverted_list_type
{
  Int_t     int_alloc;
  Int_t     collection_freq;   /* number of occurences in the corpus */
  Int_t     doc_cnt;
  Boolean_t has_locs;          /* Do we have token location info? */
  Boolean_t has_lengths;       /* Do we have token length info? */
  Int_t     list[1];
};

typedef struct inverted_list_type inverted_list_t;
typedef struct inverted_list_type prox_list_t;

typedef struct
{
    Int_t doc_id;
    Int_t max_tf;
    Int_t term_freq;
} prox_doc_header_t;


/*
 *  MAX_INVERTED_LIST returns the length of the longest possible
 *  inverted (prox) list for the open database.
 */
#define max_inverted_list() \
  ((current_db == NULL) ? \
   (0) : \
   (current_db->collection_max_tf + (3 * current_db->doc_cnt)))

/*
 *  INVERTED_LIST_LC returns the length of the longest possible
 *  inverted (prox) list for the open database, in characters (bytes).
 */
#define inverted_list_lc() \
   (sizeof (inverted_list_t) + sizeof (Int_t) * (max_inverted_list()))



/****************************************************************************
 *  QUERY NETWORKS
 ****************************************************************************/
#define MAX_INNODE_TYPES 25
#define MAX_TYPE_NAME_LENGTH 13

#define NODE_WEIGHT_MAX +1e30
#define NODE_WEIGHT_MIN -1e30
/* weight[0] is the scaling weight the user might supply e.g., 2.0 computer 
 * weight[1] is the weight that comes right after the paren in #wsum ( 1.0 ...)
 * weight[2] is probably not used
 */
#define rank_field 0
#define select_field 1
#define tf_field 2
#define total_field 3
#define NUM_IN_NODE_WEIGHTS 4
#define MAX_NODE_COUNT 500

/* the possible types of IN_node */

enum IN_node_enum { sum_op, or_op, and_op, not_op, term_op,  
                    max_op, 
                    syn_op, wsum_op, phraseb_op, 
                    odn_op, uwn_op, phrase_op_v2,
                    wgt_plus_op, wgt_minus_op, boolean_and_op,
        	    parsumn_op, field_op, field_term_op, boolean_and_not_op,
		    filter_require_op, filter_reject_op,
		    saved_prox_op

                  };

/* be sure to update the operator table in ineval.c also */

typedef enum IN_node_enum inNodeEnum_t;

/*
 *  Relational operators used on indexes
 */
enum relop_enum {
  ne_relop, eq_relop, gt_relop, gte_relop, 
  lt_relop, lte_relop, min_relop, max_relop, 
  range_relop 
};

typedef enum relop_enum relop_enum_type;

typedef struct
{ Int_t num;
  BoolInt_t no_ors;
} buffer_t;

struct IN_node {inNodeEnum_t type;
		union
		  {
		  term_id_type            term;
		  int                     int_arg;
		  } op_arg;

                relop_enum_type           ndx_op;
                Int_t                     ndx_prefix;
                UChar_t                   *ndx_term_string;
		Int_t                     doc_cnt;
                Int_t                     max_doc_cnt;
		Int_t                     tf;
		float                     idf_score;
		Int_t                     buf_index;
		struct IN_node            *saved_prox;
		struct belief_list_type   *bel_ptr;
		struct inverted_list_type *prox_ptr;
		float                     mim;
		node_weight_type          weight[NUM_IN_NODE_WEIGHTS];

		/* see weight array description above */
		int entries;        /* number of children used */
		int allocated;      /* number of children allocated */
		struct IN_node **children;
 		struct IN_node *parent;
                Int_t  *eval_order;
                Int_t  next_doc;    /* the next document for this node */
		void *il_dscr;      /* the inverted list handle */
                BoolInt_t depth_more_than_1;
		Boolean_t always_eval; /* Set if we evaluate the document
					  independent of the next_doc field */
		Boolean_t lit_term;    /* Set if literalFlag is set */
	      };

typedef struct IN_node IN_node_type;
typedef struct IN_node IN_node_t;

typedef struct {
    Int_t        num_nodes;
    IN_node_type **nodes;
} IN_node_list;

/* IN_node access macros: */

#define set_innode_weight_rank(innode, w) \
{  if(innode->parent != (IN_node_type *)NULL && \
      (innode->parent->type == wsum_op || innode->parent->type == parsumn_op)) \
      { \
	  innode->parent->weight[total_field] -= innode->weight[rank_field]; \
	  innode->weight[rank_field] = w; \
	  innode->parent->weight[total_field] += innode->weight[rank_field]; \
      } \
   else \
	  innode->weight[rank_field] = w; \
}
    
#define set_innode_weight_select(innode, w) (innode->weight[select_field] = w)
#define set_innode_weight_total(innode, w) (innode->weight[total_field] = w)

#endif


