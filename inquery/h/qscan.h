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
 *  This header file defines the interface between the query parser and
 *  a query transformation lexical scanner (a "query scanner").  The
 *  query scanners are lexical scanners that transform specific parts of
 *  a query into INQUERY canonical form.  For example, they may insert
 *  synonyms, construct phrases, or replace keywords with wildcard
 *  operators ("company" -> #company).  The input stream for a query
 *  scanner is defined (by this header file) to be a text buffer supplied
 *  by the query parser.  When a query scanner wishes to transform (replace)
 *  text in the input stream, it uses the replace_query_text routine to do
 *  so.  Changes take place "after" the query scanner has finished reading
 *  the buffer, to prevent problems that might arise by changing text
 *  while LEX (or whatever) is reading the buffer.
 *
 *  Note that the qscan_loc_minus_n macro enables a scanner to find the
 *  location of yytext in the query buffer.  This macro is both faster
 *  and more accurate (because of potential duplicates) than searching
 *  for yytext.  Normally yyleng is used for "n".
 */

#if !defined(QSCAN_H)
#define QSCAN_H

static char qscan_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#include "affix.h"
#include "in_node.h"

typedef IN_node_type *IN_node_ptr;


/*
 *  Macros:
 */

#ifndef YY_CHAR
#define YY_CHAR UChar_t
#endif

/* QUERY_BUFFERED_INPUT  
 * feeds characters to the lexical scanner, one per call. 
 * Returns: (UChar_t *)
 */
#define query_buffered_input() (*qscan_input_ptr++)

/* QUERY_BUFFERED_UNPUT 
 * enables the lexical scanner to return a character to the
 *  input stream.  
 * INPUT: c : UChar_t
 */
#define query_buffered_unput (c) (void) (qscan_input_ptr > current_query)? qscan_input_ptr-- : 0)


/* redefine ECHO and qecho to do something intelligent 
 *    Note: qecho puts spaces around the items.
 */

#undef ECHO
#define ECHO append_query_text((char*)yytext,yyleng)
#define qecho(text) do {append_query_text(" ",1);append_query_text((char*)text,strlen((char*)text));append_query_text(" ",1); } while (0)
#define qecho_ns(text) append_query_text((char*)text,strlen((char*)text))
    /*
     *  INPUT gets the next character for a query scanner.  LEX requires
     *  that it be implemented as a conditional expression (or a subroutine
     *  call, but that would be slow). FLEX requires a more complicated
     *  macro.
     */

#ifdef FLEX_SCANNER
  #undef YY_INPUT
  #define YY_INPUT(buf, result,max_size)\
    { \
      int c = query_buffered_input(); \
      result = (c==EOF || c == '\0') ? YY_NULL : (buf[0] = c, 1); \
    }
  #undef yyterminate
  #define yyterminate() yyrestart(stdin); return (YY_NULL)
#else  /* It's a lex scanner */
  #undef  input
  #define input()  query_buffered_input()
  #undef  unput
  #define unput(c) \
    { \
    unsigned char local_c = (c); \
    qscan_input_ptr--; \
    }
#endif /* [F]LEX_SCANNER */

/* For FLEX we redefine yyterminate to set up for the next run.
 */

    /*
     *  UNPUT returns 1 character to the query input buffer.  LEX requires
     *  that it be a statement and that c be evaluated once (and only once)
     *  so that LEX's pointers are updated properly.  FLEX doesn't 
     *  need it since it buffers its text independently.
     */


/* macros for the #+, #- operators. These may be made into functions
 * when we know what they should really look like.
 */
/* if these are plus/minus signs, then the facs are not factors, sorry */
#define GAIN_WGT(x) (x + wgt_plus_fac)
#define LOSE_WGT(x) (x + wgt_minus_fac)

/* structures */
typedef struct
{
   BoolInt_t  	on_off; 	/* 1=scanner can be run, 0=don't run scanner*/ 
   void 	(*func)();  	/* Function call that scans a query */
   char     	name[64]; 	/* Name to be printed during debugging */
} qscaninfo;

/*
 *  External References:
 */
extern UChar_t 	*qscan_input_ptr;
extern UChar_t     *current_query;
extern void 	 append_query_text(char *, Int_t);
/*
 *  Function Prototypes:
 */
IN_node_type *parsed_query_to_querynet(UChar_t *query_string);
extern void append_query_text (char *text, Int_t length);
IN_node_ptr query_new_op (inNodeEnum_t type);
IN_node_ptr query_new_term (UChar_t *term, Int_t ind_prefix, relop_enum_type ind_op, Boolean_t stop_stem);
extern IN_node_ptr query_make_weighted (IN_node_ptr node);
extern IN_node_ptr query_maybe_wgt_query (IN_node_ptr  query_net, 
				    Boolean_t wgt_change);
extern UChar_t *transform_query (const UChar_t *query);
extern char *qry_parse_version ();
extern IN_node_type *parse_qry (UChar_t *query_string);
extern qscaninfo transformers[];
extern BoolInt_t init_qtransformer(char *name, BoolInt_t on_off);
void init_all_qtransformers(BoolInt_t on_off);
void save_transformation(Char_t *orig, Char_t *xformed);
extern IN_node_type *parsed_query_to_querynet(UChar_t *query_string);
UChar_t *query_stop_stem( UChar_t *stemmed,UChar_t *term);


void initialize_query_info();
IN_node_type *parse_qry (UChar_t *query_string);
void finish_query_info();

BoolInt_t get_query_info_flag();
void set_query_info_error(Int_t this_error, Int_t this_offset);

Boolean_t inq_get_retain_capitalized_stopwords (void);
void push_paren_stack(void);
void pop_paren_stack(void);
Int_t paren_stack_empty(void);

/* variables */
extern Boolean_t wgt_change;
typedef Int_t paren_stack_t;

extern paren_stack_t *paren_stack_top;
#define PARENLEVEL (*paren_stack_top)

#endif

