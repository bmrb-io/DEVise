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

%{

#include <stdlib.h>
#ifdef _MSC_VER
  #include <io.h>
#endif /* _MSC_VER */

#include "doctypes.h"
#include "docmem.h"
#include "inerror.h"
#include "parse_sys.h"
#include "docman.h" /* For error codes which should be moved elsewhere */

#include "flexbuff.h" /* For no-copy buffering */
#include "token_types.h"

#define OFF            0
#define ON             1


static int found_token(char *text, int length, int type);
static UChar_t *unhyphenate(UChar_t *string, Int_t length);
static Int_t tokenize_id(Char_t *text, Int_t num_chars);

static TokenList *tok_table;
static void *index_ptr;
static Char_t *doc_beg;
static Boolean_t write_buff = TRUE;

static Int_t esFlag=ON;
static Int_t hyphenFlag=OFF;

%}

alpha   ([a-zA-Z\300-\326\330-\366\370-\377])
num     ([0-9])
alphanum ({alpha}|{num})

w1      ({alpha}{alphanum}*)
w2      ({alpha}+(\'{alpha}*)*)
w3      ({num}+{alpha}{alphanum}*)

s       ({num}+\.{alphanum}+(\.{alphanum}+)+)
a       (({alpha}+\.)+({alpha}+\.?)) 
h       ({alpha}+\-\n{alphanum}+)

wordobj	({w1}|{w2}|{w3}|{s}|{a})

amp     ((@amp\;)|(&amp\;))

compound ({wordobj}[-/&])+{wordobj}
compoundamp ({wordobj}{amp})
	  
es1     (["']?[;:\.\?\!][ \n\t]+)
es2     ([;:\.\?\!]["']?[ \n\t]+)
es      ({es1}|{es2})

n1      ({num}+(\.{num}+)?)
n2      ({num}{1,3}(\,{num}{num}{num})+(\.{num}+)?)
n3	(\.{num}+)
numobj	({n1}|{n2}|{n3})

white ([ \n\t]{1,5})

%%

{es}          { /* Set flags on end of sentence. */
                hyphenFlag=OFF; esFlag=ON; }

{amp}         { /* Sad but true, we have to have a special rule for
                   SGML ampersands so that they won't get tokenized as
                   a word.
                 */
               }

{h}           { /* 
                 * unhyphenate the word then process as text
                 */
                 unhyphenate (yytext, yyleng);

                 if(found_token(yytext, yyleng, INQ_WORD_TOK) != INQ_SUCCESS)
	              return INQ_FAILED;
              }

{numobj}    { /* 
               * <indexing> handle simple number object
               */
               if(found_token(yytext, yyleng, INQ_NUM_TOK) != INQ_SUCCESS)
	          return INQ_FAILED;
            }

{compound}  {  /* Each sub element is a separate token */
	       char *tok_ptr = yytext;
	       int i;

	       hyphenFlag = ON;
	       for(i=0; i < yyleng; i++)
	       {
	          if(yytext[i] == '/' || yytext[i] == '-' ||
		     yytext[i] == '&' )
		  {
			found_token(tok_ptr, (yytext + i - tok_ptr), 
				    INQ_WORD_TOK);
		        tok_ptr = yytext + i + 1;
		  }
	       }
	       found_token(tok_ptr, (yytext + i - tok_ptr), 
			   INQ_WORD_TOK);
	       hyphenFlag = OFF;
	    }

{compoundamp}  {  /* The trailing amp is not a part of the token */
	       found_token(yytext, yyleng - 5, INQ_WORD_TOK);
	    }

{wordobj}  { /* Handle normal terms */
              if(found_token(yytext, yyleng, INQ_WORD_TOK) != INQ_SUCCESS)
	         return INQ_FAILED;
	    }

{white}    { /* Ignore */ }
.          { /* Ignore */ }

<<EOF>>    {  return INQ_SUCCESS;  }
            
%%

#include "flexbuff.c"  /* For no-copy buffering */

Int_t
tokenize(void *index_config, Char_t *text_beg, Int_t num_chars, 
		TokenList **tok_table_ptr)
{
    Int_t status;
    Int_t mode;
    in_Document doc;
    Char_t *field;
    in_ByteSequence doc_bs;

    if(num_chars <= 0)
	return INQ_FAILED;

    fill_flex_buff(text_beg, num_chars);
    tok_table = *tok_table_ptr;
    index_ptr = index_config;
    parse_get_doc(index_ptr, &doc);
    dm_document_get_raw_data(doc, &doc_bs);
    doc_beg = (Char_t *)byte_sequence_data(doc_bs);
    parse_get_current_field(index_config, &field);

    parse_get_mode(index_config, &mode);

    if(CHECK_FLAG(mode, INQ_STATIC_BUFF))
       write_buff = FALSE;

    esFlag = ON;
    hyphenFlag = OFF;

    if(strcmp(field, "DOCID") != 0)
       status = tokenize_lex();  /* Normal case */
    else
       status = tokenize_id(text_beg + 1, num_chars - 1);
    
    flush_flex_buff();
    *tok_table_ptr = tok_table;

    return status;
}

static int
found_token(char *text, int length, int type)
{
    Token *tok;

    if((tok = add_tok_to_list(tok_table, type, text - (char *)doc_beg, length, NULL)) 
       == (Token *)NULL)
        return INQ_FAILED;

    tok->beg_of_sent = esFlag;

    normalize_token(index_ptr, tok, text, type, hyphenFlag);

    esFlag = OFF;

    return INQ_SUCCESS;
}


static Int_t
tokenize_id(Char_t *text, Int_t num_chars)
{
   Char_t *beg_ptr, *end_ptr;

   beg_ptr = text;
   end_ptr = text + num_chars;

   while( beg_ptr < end_ptr && 
	 (*beg_ptr == ' ' || *beg_ptr == '\t' || *beg_ptr == '\n' ) )
      beg_ptr++;

   while( beg_ptr < end_ptr && 
	 (*(end_ptr-1) == ' ' || *(end_ptr-1) == '\t' || *(end_ptr-1) == '\n' ) )
      end_ptr--;

   return(found_token(beg_ptr, end_ptr - beg_ptr, INQ_STATIC_TOK));
}

static UChar_t *
unhyphenate(UChar_t *string, Int_t length) 
{
    Int_t   i, j, hyp_offset;
    UChar_t *hyp_ptr;

    if(!write_buff)
       return string;

    /* Find the hyphen */
    for(i=0; string[i] != '-' && i<length; i++);

    hyp_offset = i;
  
    for( ; string[i]!='\n' && i<length; i++);

    /* Eliminate the hyphen and newline */  
    for(i++, j = hyp_offset; i<length; string[j++]=string[i++]);

    /*
     * Mark the end of the de-hyphenedped word, then pad with blanks 
     * if necessary.
     */
    if(j<length)
	for( ; j<length; string[j++]=' ');

    /*
     * Return the de-hyphenated word.
     */

    return string;
} /* end of unhyphenate */  

int
tokenize_wrap()
{
    return 1;
}
