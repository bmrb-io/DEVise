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


#include "doctypes.h"
#include "docman.h"
#include "inq_api.h"

/*
 *  Literals:
 */
#if !defined(GENTRANS_H)
#define GENTRANS_H

  /*
   *  Sometimes we want to store objects of different types in the same
   *  dictionary.  This can be accomplished by concatenating the object
   *  type and the object string.  A concatenated object must follow the
   *  following convention:
   *    "<SPR_PREFIX> <OBJ_xxx> <OBJECT_STRING>"
   *  The SPR_PREFIX must be an unprintable character that could not appear
   *  in the input, otherwise there is always the possibility that a
   *  concatenated object could match (collide with) a text object.
   */
#define SPR_PREFIX 1
#define prefix_and_suffix_length (1 + 1 + 2 + 1 + 1)


/**************/
/*   TYPES    */
/**************/

/*
 * This is a linked list which will contain the compiled information
 * for a number of documents when gentrans is building a btl file
 * directly.
 */ 
struct list_tag
  { Int_t id;
    struct inverted_list_type *stat;
    Int_t stat_length;
    struct list_tag *next;
    Char_t   flush;
  };

typedef struct post Post_t;
struct post
{
  int     position;
  int     length;
  Post_t   *next;
};


typedef struct doc_term DocTerm_t;
struct doc_term
{
  DocTerm_t   *left;
  DocTerm_t   *right;
  char        type;
  Post_t      *post_list;
  int         tf;
  char        term[sizeof (int)];
};

#define DOC_TERM_HDR_BYTES (sizeof (DocTerm_t) - sizeof (int))

#define DOC_TERM_SIZE(term) \
  (((DOC_TERM_HDR_BYTES + strlen (term) + 1) + sizeof (long) - 1) \
   & ~(sizeof (long) - 1))





/*
 *  External references:
 */

extern Int_t word_cnt;                 /* number of words in current doc */
extern Int_t collection_max_tf;        /* for the inf file. */
extern Int_t term_cnt;                 /* # of terms already occured */

/*
 * Public Interface Functions:
 */

Char_t *gentransv(void);

BoolInt_t gtrinit(dbinfo *db, in_Collection coll);
void gtrterm(Int_t record_cnt, dbinfo *db);
void gtr_remove_crashfiles();
int indx_save_word(Char_t *term, char type, Int_t position, Int_t length);
int indx_save_word_to_field(Char_t *index_prefix, Char_t *term, 
			     char type, Int_t position, Int_t length);

void indx_colsel_save_word(Char_t *word, char type, Int_t tf);
Int_t write_postings(Int_t record_cnt, Int_t *flush);
void init_parsing_db(dbinfo *db);

/* document length variable access */
Int_t get_doc_length (void);
void  increment_doc_length (Int_t val);
void  set_doc_length (Int_t val);
Int_t get_total_doc_lengths (void);
void  increment_total_doc_lengths (Int_t val);
void  set_total_doc_lengths (Int_t val);
Int_t get_fld_doc_length (void);
void  increment_fld_doc_length (Int_t val);
void  set_fld_doc_length (Int_t val);
Int_t get_fld_total_doc_lengths (void);
void  increment_fld_total_doc_lengths (Int_t val);
void  set_fld_total_doc_lengths (Int_t val);

void set_gtr_bufsize(float buf_size);
 			          

/* Doc-Term and Batch-Term interface */

DocTerm_t *get_trans_tree(void);
void clear_doc_buf (void);
Boolean_t init_doc_buf(void);
Boolean_t init_batch_buf(void);
DocTerm_t*   get_doc_term (DocTerm_t **root, unsigned char *term, char type);
DocTerm_t*   next_doc_term (DocTerm_t **curr_term);


static Char_t gentrans_h_rcsid[]="$RCSfile$, $Revision$, $Date$";


/*
 *  Macros
 */
#define inq_gtr_init(d,sf,sp,sm)	gtrinit(d,sf,sp,sm)
#define inq_gtr_save_paragraph(p)	Q_gtr_save_paragraph_boundary(p)
#define inq_gtr_save_word(t,p)		save_word(t,p)
#define inq_gtr_write_postings(r,f)	write_postings(r,f)
#define inq_gtr_term(r,sp,sm)		gtrterm(r,sp,sm)

#define inq_gtr_set_bufsize(x)          set_gtr_bufsize(x)

#endif

