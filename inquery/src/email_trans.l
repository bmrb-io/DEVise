/****************************************************************************\)
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
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
  #include <io.h>
#endif /* _MSC_VER */

#include "docman.h"
#include "inerror.h"
#include "flexbuff.h"

static void set_current_doc(in_Document doc);
static void end_of_document(char *doc_end);
static int  finish_old_field(char *field_beg, char *field_end);

static in_Document curr_doc = (in_Document)NULL;
    
static char *doc_beg;
static char *beg_ptr;
static char field_name[256] = {'\0'};
static int  at_eof, i;
static char field_desc[280] = {'\0'};
static Boolean_t recog_mode;
static char disp_mode;
static in_Collection coll;

/*****************/
/*   MACROS      */
/*****************/

#define YY_INPUT(buf, result, max_size) \
{\
   old_buff = YY_CURRENT_BUFFER;\
   yy_switch_to_buffer(new_buff);\
\
   if(old_buff)\
	free(old_buff);\
    result=new_buff->yy_buf_size;\
}

%}

blank   (\n\n)
msgid   ^(Message-Id:)
field   ^([A-Z][^: ]*[:])
bod     ^(From[ ])


%Start BODY HEADER NEW

%%
<INITIAL,NEW>{bod}  { /*
                       * Start of document.  The first "From "
                       * represents the start of an email document.
                       * This puts the scanner in HEADER state for
                       * reading email headers.  Some will be saved,
                       * others ignored.  doc_beg will point to the
                       * start of the current email document.  This is
                       * used by the document manager later when
                       * saving document location.
                       */
                      BEGIN HEADER; 
                      doc_beg = yytext;
                    }

{bod}               { /*
                       * End of current document and beginning of next
                       * document.  email documents don't have end
                       * tags, so the end of a field or document is
                       * recognized by the beginning of the next field
                       * or document or end of file.  Change state to
                       * NEW after terminating processing of current
                       * document.  Note that the scanner returns at
                       * this point.  The scan will be restarted at
                       * this point if more text remains in the text
                       * buffer.
                       */
                      end_of_document(yytext);
                      BEGIN NEW;
                      return INQ_SUCCESS;
                    }

<HEADER>{msgid}     { /*
                       * Message id field.  First call
                       * finish_old_field() with beg_ptr set to start
                       * of the previous field (if present) and yytext
                       * pointing the start of the current text token.
                       * The field span will be saved under the
                       * currently defined field_name if non-NULL.
                       * Then reset the value of field_name to "DOCID"
                       * and reset beg_ptr to point to the start of
                       * the current text token.  Note that if one
                       * wishes save the email tag as part of the
                       * text, beg_ptr should point to yytext.  If the
                       * tag is not to be saved, as in this case,
                       * beg_ptr points to yytext+yyleng, or beyond
                       * the end of the current text starting
                       * location.
                       */
                      finish_old_field(beg_ptr, yytext); 
                      strcpy(field_name, "DOCID"); 

                      /* don't save the tag with the id */
                      beg_ptr = yytext+yyleng;
                    }

<HEADER>{field}     { /*
                       * Display header.  The index field name is the
                       * same as the email tag, only in upper case.
                       * As usual, finish the previous field location
                       * information, copy the tag to field_name and
                       * convert to upper case, and reset beg_ptr to
                       * point to the start of the tag, thus
                       * preserving the tag with the field text when
                       * displayed.
                       */
                      finish_old_field(beg_ptr, yytext); 
                      strncpy(field_name, yytext, yyleng); 

                      /* eliminate the ':' and make upper case */
                      field_name[yyleng-1] = '\0'; 
                      for (i=0; field_name[i];
                        field_name[i]=toupper(field_name[i]),i++);

		      if (strcmp(field_name, "SUBJECT") == 0)
			disp_mode = INQ_TITLE;
		      else if (strcmp(field_name, "DATE") == 0)
			disp_mode = INQ_SOURCE;
		      else
			disp_mode = INQ_TEXT;

		      if (strcmp(field_name, "DATE") == 0)
			recog_mode = ON;
		      else
			recog_mode = OFF;

		      sprintf(field_desc, "%s field", field_name);
		      dm_collection_add_field(coll, field_name, 
					      field_desc,
					      ON, INQ_NORMAL, disp_mode,
					      recog_mode, INQ_COOP);

                      /* save the tag with the text */
                      beg_ptr = yytext; 
                    }

<HEADER>{blank}     { /*
                       * Start body of message.  The mbox form of an
                       * email message has double newlines to separate
                       * header lines from the message body.  Change
                       * state to BODY so that contents will not be
                       * interpreted as header lines.  Finish previous
                       * field processing and define the current field
                       * name, "BODY" and update the start of the
                       * field.
                       */
                      BEGIN BODY; 
                      finish_old_field(beg_ptr, yytext); 
                      strcpy(field_name, "BODY"); 
                      beg_ptr = yytext;
                    }

.|\n                { /*
                       * Do nothing.  This rule allows the text
                       * contents of a field to be skipped over.  We
                       * are only interested in start and end
                       * positions of the the fields, not the text
                       * itself.  The contents of the fields are
                       * presented to the tokenizer for indexing in a
                       * later step of the parsing process.  The only
                       * defines the fields of interest to the
                       * user...as in the translator's field table in
                       * dm_trans_tab.c.
                       */
                    }

<<EOF>>             { /* 
                       * EOF can represent actual end of file, or just
                       * the end of the text buffer.  If the at_eof
                       * flag is set, we are really done.  Process the
                       * end of the last document since there will be
                       * no start tag for another email document.  If
                       * the at_eof flag is not set, we didn't find a
                       * complete document in the buffer.  Set the
                       * state to NEW to indicate we are ready for
                       * another document and return a status flag
                       * indicating the parse is not complete.
                       */
                      if(at_eof) 
                      {
                        end_of_document(yytext);
                        return INQ_SUCCESS; 
                      }
                      else
                      {
                        /* we'll have to start over next time round */
                        BEGIN NEW; 
                        return INQ_END_NOT_FOUND;
                      }
                    }
%%

/*
 * Always include flexbuff.c if using flex to build a scanner.  This
 * file contains functions that allow a user defined text buffer to be
 * manipulated as a flex buffer.  Don't forget to include flexbuff.h
 * up top!
 */
#include "flexbuff.c"


/*
 * This is our defacto-main function as for document translation.  It
 * substitutes a user defined buffer as flex's text buffer using 
 * fill_flex_buff().  Then set the provided Document document
 * structure as the current one to be processed.  email_trans_lex() is
 * the flex scanner produced by this flex specification.  Finally,
 * clean up flex's buffer structure with a call to flush_flex_buff().
 */
Int_t email_trans(Char_t *inBuff, Int_t numChars, in_Document doc, int
                  eof, Int_t mode) { 
    int status;

    if(numChars < 4 ) /* A minimum message has to have "from" in it*/
       return INQ_END_NOT_FOUND;

    dm_document_get_collection(doc, &coll);

    fill_flex_buff(inBuff, numChars);
    set_current_doc(doc);
    at_eof = eof;
    field_name[0] = '\0';
    
    status = email_trans_lex();
    flush_flex_buff();
    return(status);
}


static void set_current_doc(in_Document doc)
{
    curr_doc = doc;
}


/*
 * This function is called to define the document span.  It is saved
 * in a ByteSequence structure and stored by the document manager.
 */
static void end_of_document(char *doc_end)
{
    in_ByteSequence doc_bs;
    
    finish_old_field(beg_ptr, doc_end);

    doc_bs.bytes = doc_beg;
    doc_bs.length = doc_end - doc_beg;
    
    dm_document_set_raw_data(curr_doc, doc_bs);

    return;
}

/*
 * Finish the previously active field.  Finishing a field involves
 * saving its span (begin and ending offsets) for the document manager.
 */
static int finish_old_field(char *field_beg, char *field_end)
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

/* flex wants a yywrap function defined */
int email_trans_wrap()
{
    return 1;
}

