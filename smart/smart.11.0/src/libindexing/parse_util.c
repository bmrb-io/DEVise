#ifdef RCSID
static char rcsid[] = "$Header$";
#endif

/* Copyright (c) 1991, 1990, 1984 - Gerard Salton, Chris Buckley. 

   Permission is granted for use of this file in unmodified form for
   research purposes. Please contact the SMART project to obtain 
   permission for other uses.
*/

/********************   PROCEDURE DESCRIPTION   ************************
 *0 Return 1 if current token (among a sequence of tokens) ends a sentence
 *2 check_sentence (token_ptr, single_case_flag)
 *3   SM_TOKENTYPE *token_ptr;
 *3   int single_case_flag;
 *7 token_ptr is a pointer to a punctuation token.  If token is 
 *7   '?', or '!' then return 1.  If token is '.', then
 *7   return 1 if followed by 
 *7      whitespace and then uppercase or punc
 *7      punc (but not ','), whitespace and then uppercase or punc.
 *7   However, if single_case_flag is set, always assume sentence boundary.
 *7   Return 1 to indicate start of new sentence, 0 otherwise.
 *9  WARNING: Feature: a '.' followed by space followed by number is
 *9  not a sentence. (eg. "Dec. 25" is not a sentence boundary).
***********************************************************************/

#include "common.h"
#include "param.h"
#include "functions.h"
#include "smart_error.h"
#include "proc.h"
#include "spec.h"
#include "docindex.h"

int
check_sentence (token_ptr, single_case_flag)
SM_TOKENTYPE *token_ptr;
int single_case_flag;
{
    char *ptr = token_ptr->token;
    SM_TOKENTYPE *new_token;   /* points to next token */
    if (*ptr == '.') {
        new_token = token_ptr+1;
        while (new_token->tokentype == SM_INDEX_PUNC) {
            if (*new_token->token == ',')
                return (0);
            new_token ++;
        }
        if (new_token->tokentype != SM_INDEX_SPACE) {
            return (0);
        }
        new_token++;
        if (new_token->tokentype == SM_INDEX_MIXED ||
            new_token->tokentype == SM_INDEX_PUNC ||
            (single_case_flag && new_token->tokentype == SM_INDEX_LOWER)) {
            return (1);
        }
    }
    else if (*ptr == '?' || *ptr == '!') {
        return (1);
    }
    
    return (0);
}

/********************   PROCEDURE DESCRIPTION   ************************
 *0 Return 1 if current token (among a sequence of tokens) ends a paragraph
 *2 check_paragraph (token_ptr, last_word_ended_sent)
 *3   SM_TOKENTYPE *token_ptr;
 *3   int last_word_ended_sent;
 *7 token_ptr is a pointer to whitespace. Check to see if token_ptr 
 *7 includes 2 '\n', in which case a blank line indicates new paragraph.
 *7 Or, if last_word_ended_sent then a '\n' followed by any whitespace
 *7 indicates new para.
 *7 Return 1 to indicate start of new paragraph, 0 otherwise.
***********************************************************************/

int
check_paragraph (token_ptr, last_word_ended_sent)
SM_TOKENTYPE *token_ptr;
int last_word_ended_sent;
{
    char *ptr = token_ptr->token;
    if (*(ptr+1)) {
        /* Have optimized away the most common case (interior single blank) */
        while (*ptr && *ptr != '\n')
            ptr++;
        if (*ptr) {
            ptr++;
            if (*ptr && last_word_ended_sent) {
                return (1);
            }
            while (*ptr && *ptr != '\n')
                ptr++;
            if (*ptr) {
                return (1);
            }
        }
    }
    return (0);
}




