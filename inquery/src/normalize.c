/****************************************************************************\
*	            Copyright (c) 1995 by the                                *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/

/* NORMALIZE.C
 *
 * This file contains our standard token normalizaion functions
 * used at parsing time.  Once a token is identified to be indexed,
 * we normalize the token and then pass it to the indexing system.
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "doctypes.h"
#include "parse_sys.h"
#include "dbinfo.h"
#include "docman.h"
#include "inerror.h"
#include "docstop.h"
#include "docmem.h"

/* NORMALIZE_WORD
 *
 *   Produce the canonical form of a word.
 *
 * ARGUMENTS:
 *
 *   db            -- The database info structure.
 *   tok_begin     -- A pointer to the original word.
 *   tok_len       -- The length in characters of the original word.
 *   norm_tok      -- A pointer to allocated space for the normalized
 *                    word (this is sized to 2 * tok_len)
 *   bo_sent_flag  -- TRUE if this word is the beginning of a sentence.
 *   sub_word_flag -- TRUE if this word is part of a compound word
 *                    (like "hands-off" or "on/off")   
 *
 * RETURNS:
 *
 *   INQ_SUCCESS on successful completion.
 *   INQ_FAILED  on error.
 */

Int_t
normalize_word(dbinfo *db, Char_t *tok_begin, Int_t tok_len, Char_t *norm_tok,
	       Boolean_t bo_sent_flag, Boolean_t sub_word_flag)
{
    Int_t word_len, i;
    Char_t *src, *tmp;
    Boolean_t capsFlag = FALSE;
    static Char_t *stem_tok = 0;

    if (!vector_make_room_for_large_n (&stem_tok, (3 * tok_len) + 1, 
				       sizeof (Char_t)))
      {
	Q_warning ("Couldn't allocate memory in file: %s line: %d\n", 
		   __FILE__, __LINE__);
	return INQ_ALLOC_ERR;
      }
    
    src =tok_begin;
    tmp = norm_tok;
    
    if( isupper((int)src[0]) )
	capsFlag = TRUE;

    /* Lower case word if the preserve_case mode is not on */
    if (db->preserve_case)
	for (i = 0; i < tok_len; i++)
	    *tmp++ = *src++;
    else
	for (i = 0; i < tok_len; i++)
	    *tmp++ = tolower(*src++);

    *tmp = '\0';

    /* get rid of any diacritics */
    src = tok_begin;
    tmp = norm_tok;
    for (i = 0; i < tok_len; i++)
    { 
	switch(*src)
	{
	case 0301:          /*substitute uppercase for lowercase (A' -> a')*/
	    *tmp = 0341;
	    break;
	case 0311:          /* E' -> e' */
	    *tmp = 0351;
	    break;
	case 0315:          /* I' -> i' */
	    *tmp = 0355;
	    break;
	case 0323:         /* O' -> o' */
	    *tmp = 0363;
	    break;
	case 0332:         /* U' -> u' */
	    *tmp = 0372;
	    break;
	case 0321:         /* N~ -> n~ */
	    *tmp = 0361;
	    break;
	case 0334:         /* U" -> u" */
	    *tmp = 0374;
	    break;
	}
	tmp++; 
	src++;			/* point to next char */
    }

    tmp--;      /* We are now at the last character of the copied word */
    
    /* Clip off trailing white space */
    while( tmp > norm_tok && 
	  ((*tmp==' ') || (*tmp=='\t') || (*tmp=='\n')) )
	tmp--;

    if(tmp < norm_tok) /* We've stripped off everyting */
    {
	norm_tok[0] = '\0';
	return INQ_SUCCESS;
    }

    /* Clip off some trailing punctuation */
    while( tmp > norm_tok && 
	  ((*tmp=='\'') || (*tmp=='-') || (*tmp=='/') ||( *tmp=='&')) )
	tmp--;

    *(tmp+1) = '\0';

    if(tmp < norm_tok) /* We've stripped off everyting */
    {
	norm_tok[0] = '\0';
	return INQ_SUCCESS;
    }

    /*  Map possesives to non-possesive form. */
    if  ( (tmp > norm_tok + 1) && 
	 ((*tmp == 's') || (*tmp == 'S')) && 
	 (*(tmp-1) == '\''))
    {
	tmp -= 2;
	*(tmp + 1) = '\0'; 
    }
    

    if(tmp < norm_tok) /* We've stripped off everyting */
    {
	norm_tok[0] = '\0';
	return INQ_SUCCESS;
    }

    /* Clip off contraction endings */
    while( (tmp > norm_tok) && 
	  (*tmp != '\'') )
	tmp--;

    if(tmp != norm_tok) /* we found an ' */
    {
	if((strcmp(tmp+1, "ll") == 0) ||
	   (strcmp(tmp+1, "d") == 0)  ||
	   (strcmp(tmp+1, "ve") == 0) ||
	   (strcmp(tmp+1, "re") == 0) ||
	   (strcmp(tmp+1, "LL") == 0) ||
	   (strcmp(tmp+1, "D") == 0)  ||
	   (strcmp(tmp+1, "VE") == 0) ||
	   (strcmp(tmp+1, "RE") == 0) 
	   )
	{
	    *(tmp) = '\0';
	}
    }
    
    /* Remove stop words if requested and not part of a hyphenated word
     * or a capitalized word not at the beginning of a sentence. */
    if(db->stops_removed && !sub_word_flag && (!capsFlag || bo_sent_flag) )
    {
	if(stop_word(db->stop_ht, norm_tok)) /* It's a stop word */
	{
	    norm_tok[0] = '\0';
	    return INQ_SUCCESS;
	}
    }


    /* Stem the word to achieve its final form */
    if (db->stemmer)
      {
	db->stemmer(norm_tok, stem_tok);
	strcpy (norm_tok, stem_tok);
      }

    return INQ_SUCCESS;
}


/* NORMALIZE_NUMBER
 *
 *   Produce the canonical form of a number.
 *
 * ARGUMENTS:
 *
 *   db            -- The database info structure.
 *   tok_begin     -- A pointer to the original number.
 *   tok_len       -- The length in characters of the original number.
 *   norm_tok      -- A pointer to allocated space for the normalized
 *                    number (this is sized to 2 * tok_len)
 *   bo_sent_flag  -- TRUE if this number is the beginning of a sentence.
 *   sub_word_flag -- TRUE if this number is part of a compound word
 *                    (like "2-3")   
 *
 * RETURNS:
 *
 *   INQ_SUCCESS on successful completion.
 *   INQ_FAILED  on error.
 */

Int_t
normalize_number(dbinfo *db, Char_t *tok_begin, Int_t tok_len, 
		 Char_t *norm_tok, Boolean_t bo_sent_flag, 
		 Boolean_t sub_word_flag)
{
    Char_t *ptr1, *ptr2;
    
    /*
     *  Canonical form is without commas.
     */
    ptr1 = tok_begin;
    ptr2 = norm_tok;
    
    while (*ptr1 != '\0')
    {
	if (*ptr1 != ',')
	    *ptr2++ = *ptr1;
	ptr1++;
    }
    
    *ptr2 = '\0';

    return INQ_SUCCESS;
}

Int_t
normalize_static(dbinfo *db, Char_t *tok_begin, Int_t tok_len, 
           Char_t *norm_tok, Boolean_t bo_sent_flag, 
           Boolean_t sub_word_flag)
{
  /* don't do anything.  just copy the id as it was found */
  strncpy(norm_tok,tok_begin,tok_len);
  norm_tok[tok_len] = '\0';

  return INQ_SUCCESS;
}








