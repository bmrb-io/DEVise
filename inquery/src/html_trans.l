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


#include <string.h>
#ifdef _MSC_VER
  #include <io.h>
#endif /* _MSC_VER */
#include "docman.h"
#include "inerror.h"

#include "flexbuff.h" /* Flex no-copy buffering */

/**********/
/* MACROS */
/**********/

#define BLANK          ' '
#define MAX_STACK_DEPTH 10
#define MAX_FNAME 128

#define YY_INPUT(buf, result, max_size) \
{\
   old_buff = YY_CURRENT_BUFFER;\
   yy_switch_to_buffer(new_buff);\
\
   if(old_buff)\
	free(old_buff);\
    result=new_buff->yy_buf_size;\
}

/*
 *  Module variables:
 */
static char  rcsid_l[]="$ID$";

static in_Document curr_doc = (in_Document)NULL;
static char *doc_begin_pos = (char *)NULL;
static int at_eof, in_title = 0;
static int status;
static int stack_depth = 0;
static int save_depth = 0;
static struct
{
    Char_t f_name[MAX_FNAME];
    char *begin;
} field_stack[MAX_STACK_DEPTH], save_stack[MAX_STACK_DEPTH];


/*
 *  Function Prototypes:
 */
static int skip_tag(char *text, int leng);
static int end_doc(char *doc_end);
static int begin_doc(char *doc_end);
static int begin_field(Char_t *field_name, char *beg_pos);
static int finish_field(Char_t *field_name, char *end_pos);

static int first_head = 1;

%}

/*  Start conditions */
%p 4000
%a 4000
%o 5000
%n 1000

%Start HEAD BODY

/*  Regular definitions */

w1      ([a-zA-Z][a-zA-Z0-9]*)
w2      ([a-zA-Z]+(\'[a-zA-Z]*)*)
w3      ([0-9]+[A-Za-z][a-zA-Z0-9]*)

s       ([0-9]+\.[0-9a-zA-Z]+(\.[0-9a-zA-Z]+)+)
a       (([a-zA-Z]+\.)([a-zA-Z]+\.)+)
h       ([A-Za-z]+\-\n[A-Za-z0-9]+)
wordobj	({w1}|{w2}|{w3}|{s}|{a})

n1      ([0-9]+(\.[0-9]+)?)
n2      ([0-9]{1,3}(\,[0-9][0-9][0-9])+(\.[0-9]+)?)
n3	(\.[0-9]+)
numobj	({n1}|{n2}|{n3})

textobj	({wordobj}|{numobj})

title ([tT][iI][tT][lL][eE]([ ][^<>]+)?)
url   ([uU][Rr][lL])
html  ([Hh][Tt][Mm][Ll])
head  ([Hh][Ee][Aa][Dd])
body  ([Bb][Oo][Dd][Yy])
base  ([Bb][Aa][Ss][Ee])

%%

\<{head}\>              {  /* Begining of the header */
                          if (!first_head)
                            skip_tag(yytext, yyleng);
                          else {
                            first_head = 0;
                            BEGIN HEAD;
                            status = finish_field("TEXT", yytext);
                            if(status != INQ_SUCCESS)
                               return status;
                          }
                        }

<HEAD>\<\/{url}\>             { /* process end source fields */
			  status = finish_field("SOURCE", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

<HEAD>\<{url}\>             { /* process source fields */
			  status = begin_field("SOURCE", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

<BODY,HEAD>\<{title}\>      { /* start tag for title field */
                          skip_tag(yytext, yyleng);
			  status = begin_field("TITLE", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                          in_title = 1;
                       }

<BODY,HEAD>\<\/{title}\>     { /* end tag for title field */
                       if (in_title) {
                          status = finish_field("TITLE", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                        }
                        skip_tag(yytext, yyleng);
                        in_title = 0;
                      }

<HEAD>\<\/{head}\>            {  /* end of header */
                           BEGIN 0;
                        }

<BODY>\<{body}\>        {  /* begining of the body */
                           skip_tag(yytext, yyleng);
                        }

\<{body}\>              {  /* begining of the body */
                           BEGIN BODY;
  			   status = begin_field("TEXT", yytext + yyleng);
                           if(status != INQ_SUCCESS)
                              return status;
                        }

<BODY>\<\/{body}>       { /* end of body */
                          BEGIN 0;
                          status = finish_field("TEXT", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                        }
<BODY>\<\/{html}\>      { /* end of doc tag if there was no <body> */
                          BEGIN 0;
                          status = finish_field("TEXT", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          status = end_doc(yytext + yyleng);
                          return status;
                        }

<BODY,HEAD>\<([\/]?[a-zA-Z][^<>]*)(\n([^<>]*))*\> |
<BODY,HEAD>\<!--[^<>]*--\>    { /* erase the tag and return nothing */
                            skip_tag(yytext, yyleng); 
                          }


\<\/{html}\>		{ /* end of doc tag if ther was a <body> */ 
                          status = end_doc(yytext + yyleng);
                          return status;
                        }

\<{html}([ ]+[^<>]*)?\>  { /* begin tag for document */
                          BEGIN BODY;
                          first_head = 1;
                          status = begin_doc(yytext);
                          if(status != INQ_SUCCESS)
                             return status;
  			  status = begin_field("TEXT", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }


[ \t\n]			{ /* Ignore whitespace */
                        }

.			{ /* Ignore unexpected stuff */
                        }


<<EOF>>       { 
                  /* Didn't find a complete document */
                  BEGIN 0;
		  return INQ_END_NOT_FOUND;
              }
%%

#include "flexbuff.c"  /* Flex no-copy buffering */

Int_t
html_trans(Char_t *inBuff, Int_t numChars, in_Document doc, 
		   int eof, Int_t mode)
{
    int status;

    if(numChars < 6 ) /* A minimum document must have a <DOC> tag */
       return INQ_END_NOT_FOUND;

    fill_flex_buff(inBuff, numChars);
    curr_doc = doc;
    at_eof = eof;
    stack_depth = 0;
    
    status = html_trans_lex();

    flush_flex_buff();
    return(status);
}


static int
begin_doc(char *beg_pos)
{
    doc_begin_pos = beg_pos;
    return INQ_SUCCESS;
}

static int
end_doc(char *end_pos)
{
    in_ByteSequence doc_bs;
    
    if (doc_begin_pos == (char *)NULL) 
    {
	Q_warning("Begin doc not found before this end doc");
	return INQ_SYNTAX_ERR;
    }

    if(stack_depth != 0)
    {
	Q_warning("%d field(s) unended at end of document. (last is %s)", 
        stack_depth,field_stack[stack_depth].f_name);
	return INQ_SYNTAX_ERR;
    }
    
    doc_bs.bytes = doc_begin_pos;
    doc_bs.length = end_pos - doc_begin_pos;
    
    dm_document_set_raw_data(curr_doc, doc_bs);

    doc_begin_pos = (char *)NULL;
    return INQ_SUCCESS;
}

static int
begin_field(Char_t *field_name, char *beg_pos)
{
    if(stack_depth == MAX_STACK_DEPTH)
    {
	Q_warning("Fields stacked too deep.");
	return INQ_SYNTAX_ERR;
    }
/* printf("starting field %s\n",field_name); */
    strcpy(field_stack[stack_depth].f_name, field_name);
    field_stack[stack_depth].begin = beg_pos;
    stack_depth++;
    
    return INQ_SUCCESS;
}

static int
finish_field(Char_t *field_name, char *end_pos)
{

    if(stack_depth == 0)
    {
	Q_warning("End of %s field found before beginning.", field_name);
	return INQ_SYNTAX_ERR;
    }
    
    stack_depth--;
    if(strcmp(field_name, field_stack[stack_depth].f_name) != 0)
    {
	Q_warning("End of field %s not found. Can't finish %s.", 
		  field_stack[stack_depth].f_name,field_name);
	return INQ_SYNTAX_ERR;
    }
    
/* printf("finishing field %s\n",field_name); */
    if(dm_document_mark_span(curr_doc, field_name, 
			     field_stack[stack_depth].begin - doc_begin_pos, 
			     end_pos - field_stack[stack_depth].begin) 
       != INQ_SUCCESS)
    {
	Q_warning("dm_document_mark_span returned an error.");
	return INQ_FAILED;
    }

    return INQ_SUCCESS;
}

int
html_trans_wrap()
{
    return 1;
}

static int
skip_tag(char *text, int leng)
{
    int status,i;


    /* save this one in the NOINDEX field */
    if ((status = begin_field("NOINDEX",text)) != INQ_SUCCESS)
       return(status);
    if ((status = finish_field("NOINDEX",text+leng)) != INQ_SUCCESS)
       return(status);

    return (INQ_SUCCESS);
}
