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


#if !defined(PARSE_SYS_H)
#define PARSE_SYS_H

#include "spec_scanI.h"
#include "dbinfo.h"
#include "docman.h"
#include "gentrans.h"
#include "token_types.h"


/*********/
/* TYPES */
/*********/

struct token
{
    char      type;
    Int_t     tok_offset;
    short     num_bytes;
    short     num_toks;
    Char_t   *norm_tok;
    short     nt_buff_size;
    Boolean_t beg_of_sent;
    struct token *next;
};

typedef struct token Token;

struct token_list
{
    Int_t num_toks;
    Int_t num_alloced;
    Int_t tok_type;
    Token *toks;
};

typedef struct token_list TokenList;
struct text_section
{
    Int_t   field_id;
    Char_t *beg_ptr;
    Char_t *end_ptr;
    Int_t   beg_tok;
    Int_t   end_tok;
    Int_t   norm_index;
    Int_t   field_index;
    Int_t   run_recog;
    char    coop_mode;
    struct text_section *first_kid;
    struct text_section *last_kid;
    struct text_section *next;
    struct text_section *prev;
    struct text_section *parent;
};

typedef struct text_section TextSection;
struct index_config
{
    dbinfo    *db;
    in_Collection coll;
    Int_t      doc_cnt;
    TokenList *tok_list;
    Boolean_t  tl_pub;   /* This is set if the token list has be given out */
    int        mode;
    in_Document     curr_doc;
    TextSection *curr_sect;
    Int_t      num_indexed_fields;
    Int_t      num_fields;
    Char_t   **field_prefix;
    Int_t      flush;
    Boolean_t  run_recog;
    Boolean_t  save_locs;
    Int_t      magic_num;
};

typedef struct index_config IndexConfig;


/**********/
/* MACROS */
/**********/

/* Parsing/Indexing modes: */
#define INQ_INDEX     1
#define INQ_TOK_ONLY  2
#define INQ_NO_WRITE  4
#define INQ_STATIC_BUFF 8       /* Keep the indexing buffer as it */
#define INQ_USE_COMPRESSION  ( 16 | INQ_STATIC_BUFF) /* Huffman compression */
#define INQ_IN_RECOVERY 32
#define INQ_INDEX_NO_LOCS 64

#define INQ_DEF_TOK_NUM 1000

/***********************/
/* FUNCTION PROTOTYPES */
/***********************/

Char_t *parse_version(void);
Int_t parse_collection_init(dbinfo *db, in_Collection coll, int mode,
			    int buff_size, void **ps_handle);
Int_t parse_update_fields(void *parse_handle);
Int_t parse_index_term_as_field(void *parse_handle, Char_t *fieldname, Char_t *term);
Int_t parse_terminate(void *parse_handle);

Int_t parse_document(void *ic_ptr, in_Document doc);
Int_t parse_document_text(void *ic_ptr, Char_t *text, Int_t index, 
			     Int_t run_recog);
Int_t parse_get_token_list(void *ic_ptr, TokenList **tl);
Int_t parse_get_current_field(void *ic_ptr, Char_t **field_name);
Int_t parse_get_dbinfo(void *ic_ptr, dbinfo **db);
Int_t parse_get_mode(void *ic_ptr, Int_t *mode);
Int_t parse_get_doc(void *ic_ptr, in_Document *doc);
Int_t parse_add_field_section(void *ic_ptr, Char_t *field_name, Boolean_t index, 
			      char field_index, Boolean_t recog,
			      Char_t *beg_ptr, Char_t *end_ptr);

Int_t parse_set_save_locs(void *ic_ptr, Boolean_t save_locs);

Int_t tokenize(void *ic_ptr, Char_t *text_beg, Int_t num_chars, 
	       TokenList **tok_table_ptr);

Int_t normalize_token(void *ic_ptr, Token *tok, Char_t *tok_beg, 
		      Int_t tok_type, Boolean_t sub_word_flag);
Int_t normalize_word(dbinfo *db, Char_t *tok_begin, Int_t tok_len, 
		     Char_t *norm_tok, Boolean_t bo_sent_flag, 
		     Boolean_t sub_word_flag);
Int_t normalize_number(dbinfo *db, Char_t *tok_begin, Int_t tok_len, 
		       Char_t *norm_tok, Boolean_t bo_sent_flag, 
		       Boolean_t sub_word_flag);
Int_t normalize_static(dbinfo *db, Char_t *tok_begin, Int_t tok_len, 
		   Char_t *norm_tok, Boolean_t bo_sent_flag, 
		   Boolean_t sub_word_flag);

Token *found_new_token(void *ic_ptr, char type, 
		       Char_t *beg_ptr, Int_t num_bytes,
		       Char_t *norm_tok);

Int_t parse_index_token_to_field(void *ic_ptr, Char_t *field_name, 
				 char tok_type, Char_t *beg_ptr, 
				 Int_t num_bytes, Char_t *term);

/* Token list functions: */
TokenList *new_token_list(void);
Token *add_tok_to_list(TokenList *tok_list, char type, 
		       Int_t beg_offset, Int_t num_bytes,
		       Char_t *norm_tok);
Token *get_ith_token(TokenList *tok_list, Int_t i);
Int_t token_list_size(TokenList *tok_list);
Int_t token_copy_norm_tok(Token *tok, Char_t *norm_tok);
Int_t find_nearest_tok_by_position(TokenList *tok_list, Int_t pos);
Int_t find_tok_by_position(TokenList *tok_list, Int_t pos);
Int_t clear_token_list(TokenList *tl);
Int_t free_token_list(TokenList **tl_ptr);

/* Special Scanner Processing  Functions: */

/* Initialize any scanners that are to be run and that have initialize 
   functions */
Int_t sr_initialize_scanners(ScanInfo_t spec_scanner[], IndexConfig *ic);

/* Terminate any scanners that have been run and have initialize functions*/
Int_t sr_terminate_scanners(ScanInfo_t spec_scanner[], IndexConfig *ic);

/* Run the active scanners on the givin text section */
Int_t sr_run_scanners(IndexConfig *ic, TextSection *text_sec, 
		      TokenList *tok_table);

/* Run the active scanners indicating the end of document */
Int_t sr_end_doc_run_scanners(IndexConfig *ic, Char_t *end_ptr, 
			      TokenList *tok_table);

#endif /* PARSE_SYS_H */

