/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/
%{
/*   cparse.l
 *   Lexical analyzer for cacm_dots data files.  *
*/
#include <stdio.h>
#include <string.h>
#include "docman.h"
#include "inerror.h"
#include "flexbuff.h"

static char rcsid_l[]="$RCSfile$, $Revision$, $Date$";


/* Private Functions */
static int process_tag();
static void set_current_doc(in_Document doc);
static void end_of_document(char *doc_end);
static int finish_old_field(char *field_beg, char *field_end);

/* Module Variables */
static char numberStr[36], currField[80], litStr[80];
static in_Document curr_doc = (in_Document)NULL;
static char *doc_beg;
static char *beg_ptr;
static char field_name[256] = {'\0'};
static int curr_field = 0;
static int at_eof;

%}

w1                        ([a-zA-Z][a-zA-Z0-9]*)
w2                        ([a-zA-Z]+\'[a-zA-Z]+)
a                         ([a-zA-Z]+\.)+
s                         (([a-zA-Z][/])([a-zA-Z][/])+[a-zA-Z])
n1                        ([0-9]+)
n2                        ([+\-]?[0-9]+\.[0-9]+)
n3                        ([0-9]?[0-9]?[0-9](\,[0-9][0-9][0-9])+)
n4                        ({n3}\.[0-9]+)
t                         (\.[ITWBAKCNX])
id                        (\.I)



%start DOC NEW

%%
<INITIAL,NEW>\.I          { BEGIN DOC;
                           doc_beg = yytext; 
                           if(process_tag() != INQ_SUCCESS)
				return INQ_SYNTAX_ERR;
                          }
\.I                       { BEGIN NEW;
                           end_of_document(yytext);
                           return INQ_SUCCESS;
                         }
^{t}                      { if(process_tag() != INQ_SUCCESS)
					return INQ_SYNTAX_ERR;
                          }

{w1}|{w2}                 |
{a}                       |
{s}                       |
{n1}|{n2}|{n3}|{n4}       |
[\;\,\:\'\"\(\)\$\-]      |
[ \t\n]                   |
[\.]                      |
.                         { /* Do nothing */ }

<<EOF>>       { /* Didn't find a complete document */
                if(at_eof)
                {
                  end_of_document(yytext);
		  return INQ_SUCCESS; 
                }
                else
                {
                  BEGIN NEW; /* We'll have to start over next time round */
		  return INQ_END_NOT_FOUND;
                }
              }

%%

#include "flexbuff.c"

Int_t
cacm_dots_trans(Char_t *inBuff, Int_t numChars, in_Document doc, 
		   int eof, Int_t mode)
{
    int status;

    if(numChars < 2 ) /* A minimum doc has to have a ".I" in it*/
       return INQ_END_NOT_FOUND;

    fill_flex_buff(inBuff, numChars);
    set_current_doc(doc);
    at_eof = eof;
    field_name[0] = '\0';
    
    status = cparse_dots_lex();

    flush_flex_buff();
    return(status);
}

static int 
process_tag()
{

  if(yytext[0] != '.')
  {
     Q_warning("Bad tag: %s", yytext);
     return INQ_FAILED;
  }
  finish_old_field(beg_ptr, yytext);
  beg_ptr = yytext + yyleng;

  switch(yytext[1])
  {
  case 'T':
      strcpy(field_name, "TITLE");
      break;
  case 'W':
    strcpy(field_name, "ABSTRACT");
    break;
  case 'B':
      strcpy(field_name, "SOURCE");
      break;
  case 'A':
      strcpy(field_name, "AUTHOR");
      break;
  case 'K':
      strcpy(field_name, "KEYWORDS");
      break;
  case 'C':
      strcpy(field_name, "CATEGORY");
      break;
  case 'I':
      strcpy(field_name, "DOCID");
      break;
  case 'N':
      strcpy(field_name, "DATA-ENTRY");
      break;
  case 'X':
      strcpy(field_name, "IGNORED");
      break;
  default:
    Q_warning("Unidentified Tag:%s\n", yytext); 
    return INQ_FAILED;
  }
  return INQ_SUCCESS;
}

static void
set_current_doc(in_Document doc)
{
    curr_doc = doc;
}

static void
end_of_document(char *doc_end)
{
   in_ByteSequence doc_bs;
    
    finish_old_field(beg_ptr, doc_end);

    doc_bs.bytes = doc_beg;
    doc_bs.length = doc_end - doc_beg;
    
    dm_document_set_raw_data(curr_doc, doc_bs);

    return;
}

static int
finish_old_field(char *field_beg, char *field_end)
{
    int ret;
    
    if(strlen(field_name))
    {
	ret = dm_document_mark_span(curr_doc, field_name, 
				    field_beg - doc_beg, 
				    field_end - field_beg);
	
	if(ret != INQ_SUCCESS)
	    Q_warning("dm_document_mark_span returned a code of %d.", ret);
    }

    return 1;
}

int
cparse_dots_wrap(void )
{
  return 1;
}
