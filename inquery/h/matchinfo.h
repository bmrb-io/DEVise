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
 * MATCHINFO_H is a header file for the matchinfo data structure and all
 * the access and manipulation function for that data structure as
 * found in matchinfo.c
 * 
 */

#if !defined(MATCHINFO_H)
#define MATCHINFO_H

static char matchinfo_h_rcsid[]="$Id$";

#include "doctypes.h"

/* The matchinfo structure contains infomation about how a
 * particular query matched a particular document.  This infomation
 * is made available to the user via the api.
 */

/*************/
/*   TYPES   */
/*************/

typedef enum {INQ_OBJ_WORD, INQ_OBJ_PASSAGE} textobj_type_t;

typedef struct
{
    textobj_type_t type;
    Char_t        *term;
    Int_t          passid;
    Int_t          offset;
    Int_t          length;
} match_t;

typedef struct
{
    Int_t          num_terms;
    Char_t       **term_list;
    Int_t          num_matches;
    Int_t          curr_match;
    match_t       *match_list;
} matchinfo_t;


/*************/
/*   MACROS  */
/*************/

#define get_match_type(m) ((m == (match_t *)NULL)? -1 : m->type)
#define get_match_term(m) ((m == (match_t *)NULL)? (term_id_type)NULL : m->term)
#define get_match_passid(m) ((m == (match_t *)NULL)? -1 : m->passid)
#define get_match_offset(m) ((m == (match_t *)NULL)? -1 : m->offset)
#define get_match_length(m) ((m == (match_t *)NULL)? -1 : m->length)
#define get_match_end(m)    ((m == (match_t *)NULL)? -1 : m->offset + m->length - 1)
#define set_match_type(m,t) { if(m != (match_t *)NULL) m->type = t;}
#define set_match_term(m,t) { if(m != (match_t *)NULL) m->term = t;}
#define set_match_passid(m,p) { if(m != (match_t *)NULL) m->passid = p;}
#define set_match_offset(m,o) { if(m != (match_t *)NULL) m->offset = o;}
#define set_match_length(m,l) { if(m != (match_t *)NULL) m->length = l;}

#define set_curr_match(mi,c)  {if(mi != (matchinfo_t *)NULL) mi->curr_match = c; }



/***************************/
/*  FUNCTION DECLARATIONS  */
/***************************/

#if (defined(__MWERKS__) && defined(__cplusplus))
extern "C" {
#endif


matchinfo_t *new_matchinfo(void);
matchinfo_t *copy_matchinfo(matchinfo_t *origMI);
void free_matchinfo(matchinfo_t *matchStruct);
Boolean_t add_term_match(matchinfo_t *matchStruct, term_id_type term, 
			 Int_t offset, Int_t length);
Boolean_t add_passage_match(matchinfo_t *matchStruct, Int_t pass_id, 
			    Int_t offset, Int_t length);
void sort_matches_by_position(matchinfo_t *matchStruct);
void rewind_match_list(matchinfo_t *matchStruct);
match_t *get_next_match(matchinfo_t *matchStruct);
match_t *get_next_type_match(matchinfo_t *matchStruct, textobj_type_t type);
match_t *get_prev_match(matchinfo_t *matchStruct);
match_t *get_prev_type_match(matchinfo_t *matchStruct, textobj_type_t type);
match_t *get_match_by_position(matchinfo_t *matchStruct, Int_t pos);
Int_t get_num_matches(matchinfo_t *matchStruct);
Int_t get_num_term_matches(matchinfo_t *matchStruct);
Int_t get_num_passage_matches(matchinfo_t *matchStruct);

#if (defined(__MWERKS__) && defined(__cplusplus))
}
#endif

#endif /* MATCHINFO_H */

