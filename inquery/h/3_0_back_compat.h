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
 * "3_0_BACK_COMPAT_H" is a header file to 
 *         provide compatibility for customized parser sources and
 *         interfaces written for INQUERY version 3.0.
 * 
 *
 */

#if !defined(DM_3_0_COMP_H)
#define DM_3_0_COMP_H

/*
 *  RCS identifier for header:
 */
static char dm_3_0_comp_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

/* Token types changed for 3.1 ...
 */
#include "token_types.h"

#define INQ_WORD_TYPE INQ_WORD_TOK
#define INQ_NUM_TYPE  INQ_NUM_TOK
#define INQ_ID_TYPE   INQ_STATIC_TOK

/* In the 3.0 everything was a word ... */
#define add_tok_to_list(tab, off, len, norm) add_tok_to_list(tab, INQ_WORD_TOK, off, len, norm)

/* NOTE: Add this function to normalize.c for full back compatibility.

Int_t
normalize_static(dbinfo *db, Char_t *tok_begin, Int_t tok_len, 
           Char_t *norm_tok, Boolean_t bo_sent_flag, 
           Boolean_t sub_word_flag)
{
  strncpy(norm_tok,tok_begin,tok_len);
  norm_tok[tok_len] = '\0';

  return INQ_SUCCESS;
}

*/

/* The stemming was changed for 3.1 ...*/

#define stem_mode stemmer

#define stem(stemmer, norm_tok, stem_tok) \
   {\
     if (stemmer)\
      {\
	stemmer(norm_tok, stem_tok);\
	strcpy ((char *)norm_tok, (char *)stem_tok);\
      }\
   }	

/*
 *      The new dm data types are prefixed by "in_"
 *             in_Document == (3.0)Document
 *             in_Collection == (3.0)Collection
 *             in_ByteSequence == (3.0)ByteSequence
 */

#define Document in_Document
#define Collection in_Collection
#define ByteSequence in_ByteSequence

/* Language definitions have changed */

typedef enum {english, spanish, japanese} base_lang;

#define language old_lang_var

#endif
