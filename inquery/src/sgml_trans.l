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
#include "parse_sys.h"

#include "flexbuff.h" /* Flex no-copy buffering */

/**********/
/* MACROS */
/**********/

/* Customize the Generic field display type here: */
#define GEN_FLD_DISP  INQ_TEXT

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
static int at_eof;
static int status;
static int start_cond;
static int stack_depth = 0;
static struct
{
    Char_t f_name[MAX_FNAME];
    char *begin;
} field_stack[MAX_STACK_DEPTH];

static int skip_field_depth = 0;
static char *skip_start_pos = (char *)NULL;

static int write_buff = 1;

/*
 *  Function Prototypes:
 */
static UChar_t *convert_ampersand(UChar_t *text_string, Int_t length);
static UChar_t *convert_stupid_sgmlamp(UChar_t *text_string, Int_t length);
static int end_doc(char *doc_end);
static int begin_doc(char *doc_end);
static int begin_field(Char_t *field_name, char *beg_pos);
static int finish_field(Char_t *field_name, char *end_pos);
static int finish_generic_field(Char_t *field_name, char *end_pos);
static int start_skipping(char *text);
static int stop_skipping(char *text);
static int skip_text(char *text, int leng);

%}

/*  Start conditions */
%p 4000
%a 4000
%o 5000
%n 1000

%Start TXT COMM

/*  Regular definitions */

w1      ([a-zA-Z][a-zA-Z0-9]*)
w2      ([a-zA-Z]+(\'[a-zA-Z]*)*)
w3      ([0-9]+[A-Za-z][a-zA-Z0-9]*)

s       ([0-9]+\.[0-9a-zA-Z]+(\.[0-9a-zA-Z]+)+)
a       (([a-zA-Z]+\.)([a-zA-Z]+\.)+)
h       ([A-Za-z]+\-\n[A-Za-z0-9]+)
wordobj	({w1}|{w2}|{w3}|{s}|{a})

att     ((AT&T)|(AT[ ]&[ ]T)|(AT{amp}T)|(AT[ ]{amp}[ ]T)|ATT)

amp     ((@amp\;)|(&amp\;))
ba      ((&O\;)|(&P\;)|(&M\;)|(&C[A-Za-z0-9]\.[0-9]{3}))

n1      ([0-9]+(\.[0-9]+)?)
n2      ([0-9]{1,3}(\,[0-9][0-9][0-9])+(\.[0-9]+)?)
n3	(\.[0-9]+)
numobj	({n1}|{n2}|{n3})

textobj	({wordobj}|{numobj})

beg_comm (\<!\-\-)
end_comm (\-\-\>)
%%

\<DOCNO\>		{ /* identify start of doc id field */
			  begin_field("DOCID", yytext + yyleng);
                        }
\<OREF\>      |
\<INVT\>      |
\<ASSG\>      |
\<SECTION\>   |
\<SO\>        |
\<SOURCE\>    |
\<DATELINE\>  |
\<JOURNAL\>             { /* process source fields */
                          BEGIN TXT; 
                          skip_text(yytext, yyleng);
			  status = begin_field("SOURCE", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<doctitle\>  |
\<HEAD\>      |
\<HEADLINE\>  |
\<HL\>        |
\<TITLE\>     |
\<TTL\>                 { /* start tag for title field */
                          BEGIN TXT;
                          skip_text(yytext, yyleng);
			  status = begin_field("TITLE", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<AUTHOR\>              { /* start tag for author field */
                          BEGIN TXT;
                          skip_text(yytext, yyleng);
			  status = begin_field("AUTHOR", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<MESH\>                { /* start tag for mesh field */
                          BEGIN TXT;
                          skip_text(yytext, yyleng);
			  status = begin_field("MESH", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }



\<NOINDEX\>		|
\<BYLINE\>              { /* Non indexed fields.  Usually nested.*/
                          skip_text(yytext, yyleng);
			  status = begin_field("NOINDEX", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<DD\>  |    
\<DATE\>                { /* start tag for document date field */
                          skip_text(yytext, yyleng);
			  status = begin_field("DDATE", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

<TXT>\<txt\> {
               skip_text(yytext, yyleng);
              }
\<txt\>            |
\<LEADPARA\>       |
\<LP\>	           |
\<TEXT\>	   |
\<SUMMARY\>             { /* start tag for text field */
                          BEGIN TXT; 
                          skip_text(yytext, yyleng);
			  status = begin_field("TEXT", yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        } 

\<ABSTRACT\>   |
\<ABST\>       |
\<PAL\>                 { /* start tag for text subfield */
                           skip_text(yytext, yyleng);
                        }

\<PAR\>                 { /* start tag for paragraph */
                           skip_text(yytext, yyleng);
                        }

\<PA1\>                 { /* start tag for text subfield */
                           skip_text(yytext, yyleng);
                        }


\<FLD[0-9][0-9]?[0-9]?\> { /* start tag for generic field */
                           char fld_name[7];
                           strncpy(fld_name, yytext + 1, yyleng - 2);
                           skip_text(yytext, yyleng);
                           
			   status = begin_field(fld_name, yytext + yyleng);
                           if(status != INQ_SUCCESS)
                             return status;
                        }

\<\/FLD[0-9][0-9]?[0-9]?\> { /* end tag for generic field */
                           char fld_name[7];
                           strncpy(fld_name, yytext + 2, yyleng - 3);
			   status = finish_generic_field(fld_name, yytext);
                           if(status != INQ_SUCCESS)
                             return status;
                           skip_text(yytext, yyleng);
                        }

\<\/DOCNO\>		{ /* end tag for doc id field */
                          status = finish_field("DOCID", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
                        }

\<region\>     |
\<\/HEAD\>     |
\<\/HEADLINE\> |
\<\/HL\>       |
\<\/TTL\>      |
\<\/TITLE\>             { /* end tag for title field */
                          BEGIN 0; 
                          status = finish_field("TITLE", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
                        }

\<\/MESH\>              { /* end tag for author or mesh field */
                          BEGIN 0; 
                          status = finish_field("MESH", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
                        }

\<\/AUTHOR\>            { /* end tag for author or mesh field */
                          BEGIN 0; 
                          status = finish_field("AUTHOR", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
                        }


\<\/OREF\>     |
\<\/INVT\>     |
\<\/ASSG\>     |
\<\/SECTION\>  |
\<\/SO\>       |
\<\/SOURCE\>   |
\<\/DATELINE\> |
\<\/JOURNAL>            { /* handle end of source fields */
                          BEGIN 0;
                          status = finish_field("SOURCE", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
                        }

\<\/NOINDEX\>		|
\<\/BYLINE\>            { /* end tag for source field */
                          status = finish_field("NOINDEX", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
                        }

\<\/DD\>    |
\<\/DATE\>              { /* end tag for document date field */
                          BEGIN 0;
                          status = finish_field("DDATE", yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                          skip_text(yytext, yyleng);
		        }


\<chksum\>           |
\<\/LEADPARA\>       |
\<\/LP\>             |
\<\/TEXT\>	     |
\<\/SUMMARY\>           { /* end tag for text field */
                          BEGIN 0; 
                          status = finish_field("TEXT", yytext);
                          skip_text(yytext, yyleng);
		        }

\<\/ABSTRACT\> |
\<\/ABST\>     |
\<\/PAR\>      |
\<\/PAL\>      |
\<\/PA1\>               { /* end tag for text subfield */
                          skip_text(yytext, yyleng);
                        }
\<\/article\> |
\<\/DOC\>		{ /* end of doc tag */
                          status = end_doc(yytext + yyleng);
                          return status;
                        }

\<DOC\>			{ /* begin tag for document */
                          BEGIN 0;
                          status = begin_doc(yytext);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<ISD\> |
\<WKU\> |
\<SRC\> |
\<APT\> |
\<ART\> |
\<APD\> |
\<APN\> |
\<NCL\> |
\<ECL\> |
\<EXA\> |
\<EXP\> |
\<NDR\> |
\<NFG\>                 { /*
                           * These tags represent fields which should be
                           * erased entirely, tags and all.
                           */
                           start_skipping(yytext);
                        }

\<\/ISD\> |
\<\/WKU\> |
\<\/SRC\> |
\<\/APN\> |
\<\/APT\> |
\<\/ART\> |
\<\/APD\> |
\<\/NCL\> |
\<\/ECL\> |
\<\/EXA\> |
\<\/EXP\> |
\<\/NDR\> |
\<\/NFG\>               { /* stop erasing these fields */
                           status = stop_skipping(yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<PRIR\>     |
\<RLAP\>     |
\<LREP\>     |
\<UREF\>     |
\<CLAS\>               { /* Erase all the text fields within these fields */
                           start_skipping(yytext);
                        }


\<\/PRIR\>   |
\<\/RLAP\>   |
\<\/LREP\>   |
\<\/UREF\>   |
\<\/CLAS\>             { /* erase these tags */
                           status = stop_skipping(yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

<TXT>\<PAC\>  |
<TXT>\<NUM\>  |
<TXT>\<STM\>            { /* 
                           * these fields are completely erased even though
                           * they are in the text field.  Don't change state.
                           */
                           start_skipping(yytext);
                        }

<TXT>\<\/PAC\>  |
<TXT>\<\/NUM\>  |
<TXT>\<\/STM\>          { /* stop erasing and stay in text state */
                           status = stop_skipping(yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }

\<ACCESS\>           |
\<DESCRIPT\>         |
\<MEMO\>             |
\<COUNTRY\>          | 
\<CITY\>             | 
\<EDITION\>          |
\<CODE\>             |
\<NAME\>             |
\<PUBDATE\>          |
\<DAY\>              |
\<MONTH\>            |
\<PG\.COL\>          |
\<PUBYEAR\>          |
\<REGION\>           |
\<STATE\>            |
\<WORD\.CT\>         |
\<COPYRGHT\>         |
\<LIMLEN\>           |
\<LANGUAGE\>            { /* erase the entire field */
                           start_skipping(yytext);
                        }

\<\/ACCESS\>           |
\<\/DESCRIPT\>         |
\<\/MEMO\>             |
\<\/CITY\>             |
\<\/COUNTRY\>          |
\<\/EDITION\>          |
\<\/CODE\>             |
\<\/NAME\>             |
\<\/PUBDATE\>          |
\<\/DAY\>              |
\<\/MONTH\>            |
\<\/PG\.COL\>          |
\<\/PUBYEAR\>          |
\<\/REGION\>           |
\<\/STATE\>            |
\<\/WORD\.CT\>         |
\<\/COPYRGHT\>         |
\<\/LIMLEN\>           |
\<\/LANGUAGE\>          { /* stop erasing these fields */
                           status = stop_skipping(yytext + yyleng);
                          if(status != INQ_SUCCESS)
                             return status;
                        }


\<FTAG[ ]+tagnum=[0-9]+\> |
\<ITAG[ ]+tagnum=[0-9]+\> |
\<G7\.U\>                |
\<[A-Z0-9]+\>          { /* erase the tag if erasing is off, otherwise keep erasing */
                           skip_text(yytext, yyleng);
                        }

\<\/ITAG\>               |
\<\/FTAG\>               |
\<\/G&\.U\>              |
\<\/[A-Z0-9]+\>         { /* finish erasing the tag and return nothing */
                           skip_text(yytext, yyleng);
                        }

{beg_comm}           { /* skip over comments */
                       start_skipping(yytext);
		       start_cond = YY_START;
                       BEGIN COMM;
                     }
                      
<COMM>{end_comm}     { stop_skipping(yytext + yyleng);
                       BEGIN start_cond;
                     }

\<[a-z][A-Za-z0-9 =]*\>    |
\<\/[a-z][a-zA-Z0-9 =]*\>    { /* don't index lowercase tags in the text */
            if((stack_depth > 0) &&
	       strcmp("NOINDEX", field_stack[stack_depth-1].f_name) != 0) {
	       /* save this one in the NOINDEX field */
	       if ((status = begin_field("NOINDEX",yytext)) != INQ_SUCCESS)
	         return(status);
	       if ((status = finish_field("NOINDEX",yytext+yyleng)) != INQ_SUCCESS)
	         return(status);
	     }

          }

<TXT>{textobj}[ ]?{amp}  { /*
                           * <indexing>
                           * Convert number objects with trailing stupid sgml
                           * ampersands.  This requires that the sgml amp
                           * be converted in the text buffer to an & so special
                           * recognizers will treat it correctly.
                           */
                          strcpy (yytext, 
                                  (char *)convert_stupid_sgmlamp (yytext, yyleng));
                        }

<TXT>{amp}              { /* 
                           * <indexing>
                           * convert occurrences of SGML ampersands in text
                           * buffer so recognizers can do their thing.
                           */
                          strcpy ((char *)&yytext[0], 
                                  (char *)convert_ampersand (yytext, yyleng));
                          /* don't return anything */
 		        }


{textobj}		{ 
                        }

{ba}                    { /* erase SGML symbols containing & that aren't amps */
                         /*  replace_marker(BLANK); */
                        }

[ \t\n]			{ /* Ignore whitespace but leave esflag as is */ 
                        }

.			{ /* Ignore unexpected stuff but reset special flags */
                        }


<<EOF>>       { 
                  /* Didn't find a complete document */
		  return INQ_END_NOT_FOUND;
              }
%%

#include "flexbuff.c"  /* Flex no-copy buffering */

Int_t
sgml_trans(Char_t *inBuff, Int_t numChars, in_Document doc, 
		   int eof, Int_t mode)
{
    int status;

    if(numChars < 6 ) /* A minimum document must have a <DOC> tag */
       return INQ_END_NOT_FOUND;

    fill_flex_buff(inBuff, numChars);
    curr_doc = doc;
    at_eof = eof;
    stack_depth = 0;
    skip_field_depth = 0;
    skip_start_pos = (char *)NULL;
    if(CHECK_FLAG(mode, INQ_STATIC_BUFF))
       write_buff = 0;
    
    status = sgml_trans_lex();

    flush_flex_buff();
    return(status);
}

static UChar_t *
convert_stupid_sgmlamp(UChar_t *string, Int_t length) 
{
    register Int_t  i;
    static   UChar_t tmp[128];
    UChar_t *cptr, *cmark;

    if(write_buff == 0)
        return string;

    /* work from a copy */
    strcpy ((char *)&tmp[0], (char *)string);

    /* find the trailing "@amp;" or "&amp;" */
    if ((cptr = (UChar_t *)strstr ((char *)&tmp[0], (char *)"@amp;")) == (UChar_t *)NULL) {
	if ((cptr = (UChar_t *)strstr ((char *)&tmp[0], (char *)"&amp;")) == (UChar_t *)NULL) {
	    return ((UChar_t *)&tmp[0]);
	}
    }

    /*
     * Build a new ampersanded string for replacement into the text buffer for
     * later use by special recognizers.  Cptr now points to the start of the 
     * stupid sgml ampersand.  Backtract from this point to the first non-blank
     * character.  This will be the string returned by this function.  From 
     * cptr, convert "@amp;" or "&amp;" to "    &".  The total converted string
     * will be rewritten back to the text buffer for recognizer use.
     */
    for (cmark=cptr-1; *cmark == ' '; cmark--);
    cmark++;

    /* replace stupid sgml ampersand and pad leftovers with blanks */
    for (i=0; i<=3; *cptr = ' ', cptr++,i++);
    *cptr = '&';

    return((UChar_t *)&(tmp[0]));
} /* end of convert_stupid_sgmlamp */  

/* convert_ampersand():  Change the stupid SGML symbols for ampersand in
 * TIPSTER docs to the real thing in the scan_man text buffer.  This is done
 * so that special purpose recognizers such as scan_company will be able to
 * better do their jobs.
 */
static UChar_t *
convert_ampersand(UChar_t *string, Int_t length) 
{
  register Int_t i;
  static UChar_t  tmp[128];

  if(write_buff == 0)
     return string;

  tmp[0]='&';

  /* pad with blanks as needed */
  for(i=1; i<length; tmp[i++]=' ');
  tmp[i]='\0';

  return((UChar_t *)&(tmp[0]));
}  /* end of convert_apersand */

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
    
    if(stack_depth != 0)
    {
	Q_warning("%d field(s) unended at end of document.", stack_depth);
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
	Q_warning("End of field %s not found.", 
		  field_stack[stack_depth].f_name);
	return INQ_SYNTAX_ERR;
    }
    
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

static int
finish_generic_field(Char_t *field_name, char *end_pos)
{
    Int_t status;
    in_Collection coll;

    if(stack_depth == 0)
    {
	Q_warning("End of %s field found before beginning.", field_name);
	return INQ_SYNTAX_ERR;
    }
    
    stack_depth--;
    if(strcmp(field_name, field_stack[stack_depth].f_name) != 0)
    {
	Q_warning("End of field %s not found.", 
		  field_stack[stack_depth].f_name);
	return INQ_SYNTAX_ERR;
    }
    
    if((status = dm_document_mark_span(curr_doc, field_name, 
			     field_stack[stack_depth].begin - doc_begin_pos, 
			     end_pos - field_stack[stack_depth].begin) )
       != INQ_SUCCESS)
    {
	/* If the generic field does not exist, we create it */
        if(status == INQ_NOT_AVAIL)
	{
	  dm_document_get_collection(curr_doc, &coll);
	  status = dm_collection_add_field(coll, field_name, "Generic Field",
	                          ON, INQ_NORMAL, GEN_FLD_DISP,
				  ON, INQ_COOP);
          if(status == INQ_SUCCESS)
	      status = dm_document_mark_span(curr_doc, field_name, 
			     field_stack[stack_depth].begin - doc_begin_pos, 
			     end_pos - field_stack[stack_depth].begin);
	}
	if(status != INQ_SUCCESS)
	{
	  Q_warning("dm_document_mark_span returned an error for field %s.",
	             field_name);
	  return INQ_FAILED;
	}
    }

    return INQ_SUCCESS;
}

/* To just skip a section of text we close all open fields at the
 * beginning of the text and reopen them at the end.
 */
static int
skip_text(char *text, int leng)
{
    int i;

    if(stack_depth == 0 || skip_field_depth)
    {
        /* No open fields or already skipping this*/
	return INQ_SUCCESS;
    }

    /* Close the fields  -- Note that we do this in two passes because
     * ordering of opening and closing fields may be important 
     */
    for(i=stack_depth - 1; i >= 0 ; i--)    
    {
	if(dm_document_mark_span(curr_doc, field_stack[i].f_name, 
				 field_stack[i].begin - doc_begin_pos, 
				 text - field_stack[i].begin) 
	   != INQ_SUCCESS)
	{
	    Q_warning("dm_document_mark_span returned an error.");
	    return INQ_FAILED;
	}
    }
    
    /*Reopen the fields */
    for(i = 0 ; i < stack_depth ; i++)    
    {
	field_stack[i].begin = text + leng;
    }

    return INQ_SUCCESS;
}

static int
start_skipping(char *text)
{
  if(!skip_field_depth)
     skip_start_pos = text;

  skip_field_depth++;

  return INQ_SUCCESS;
}

static int
stop_skipping(char *text)
{
   if(!skip_field_depth || text < skip_start_pos)
     return INQ_SYNTAX_ERR;

   skip_field_depth--;
   if(skip_field_depth == 0)
   {
      skip_text(skip_start_pos, text - skip_start_pos);
   }

   return INQ_SUCCESS;
}

int
sgml_trans_wrap()
{
    return 1;
}

