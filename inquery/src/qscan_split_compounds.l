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

 /*
  *  QSCAN_SPLIT_COMPOUNDS.L is a scanner specification for the LEX
  *  lexical analyzer generator.  The scanner specified in this file
  *  splits compound noun phrases, defined here to be words connected
  *  by dashes and/or slashes (e.g. VAX-11/780).  When the scanner
  *  finds something to change, it calls REPLACE_QUERY_TEXT to make
  *  the change.  Note that changes do *not* affect LEX's input stream.
  *
  *  This scanner is complicated slightly by *where* it finds the compound.
  *  If the compound occurs inside of an operator, the dash or slash is
  *  replaced with a space, and then an additional copy of the compound is
  *  written as a prox immediately after the operator is completed.  So,
  *  		#PHRASE (data-base management)
  *			becomes
  *		#PHRASE (data base management) #1 (data base)
  */

%{

#define SCANNER_NAME qscan_split_compounds
#ifdef _MSC_VER
  #include <io.h>
#endif /* _MSC_VER */
#include <stdlib.h>
#include <string.h>

#include "doctypes.h"

#ifdef SOLO
#undef YY_DECL
#define YY_DECL int main(void)
#define qecho(text) (Q_inform(" %s ",text);
#else
#include "qscan.h"
#endif

/*
 *  Module Variables:
 */
static Int_t  num_parens = 0;
static char printable_text[4096]={ '\0' };
static char rcsid[]="$RCSfile$, $Revision$, $Date$";

static char saved_text_stack[16][256]={ {'\0'} };
static Int_t  token_count_stack[16]={0};
static Boolean_t in_prox_op=0;
static Boolean_t in_field_op=0;	      
static Int_t prox_parens=0;
static Int_t field_tag=0;

static YY_CHAR temp;
%}

proxop  \#([0-9]+|[Uu][Ww][0-9]+|[Pp][Hh][Rr][Aa][Ss][Ee]|[Oo][Dd][0-9]+)
wsumop  \#([Ww][Ss][Uu][Mm])
fieldop \#([Ff][Ii][Ee][Ll][Dd])

captok  [A-Z][a-z0-9]*
solid	[A-Z]+
ws      [ \t\n]+

token   [a-z][A-Za-z0-9]*
caphy   {captok}([-/][A-Za-z0-9]+)+
tokhy   {token}(([-][A-Za-z0-9]+)+|([/][0-9]+)+)
toksl   {token}[/]

capseq	({captok}|{caphy})({ws}({captok}|{caphy}))+

%%

!(.*)\n                         { ECHO; /* Pass on comments */ }

{solid} 	{ECHO;}
{proxop}	{ in_prox_op = 1; ECHO;}
{fieldop}       { field_tag = 1; ECHO;}

{token}|{captok} { 
                  ECHO;
		  if(field_tag)
		    field_tag = 0;
		  else
		    token_count_stack[num_parens]++; }

{tokhy}|{caphy}	{ register char *cp;

		  for (cp=(char*)yytext; *cp != '\0'; cp++)
		    if ((*cp == '-') || (*cp == '/'))
		      *cp = ' ';

		  strcat (saved_text_stack[num_parens], " #1(");
		  strcat (saved_text_stack[num_parens], (char*)yytext);
		  strcat (saved_text_stack[num_parens], ") ");

		  if (prox_parens > 0)
		    ECHO;
		  else
		    {
		    qecho (saved_text_stack[num_parens]);
		    saved_text_stack[num_parens][0] = '\0';
		    }

	        token_count_stack[num_parens] ++;
		}

{toksl}		{temp=yytext[yyleng-1];
		 yytext[yyleng-1]='\0';
		 ECHO;
		 yytext[yyleng-1]=temp;
		}


\(		{ num_parens ++;
		  if (in_prox_op) prox_parens++;
		  token_count_stack[num_parens] = 0;
		  saved_text_stack[num_parens][0] = '\0';
		  ECHO;
		}

\)		{ if ((saved_text_stack[num_parens][0] != '\0') &&
		      (token_count_stack[num_parens] > 1))
		    strcat (printable_text, saved_text_stack[num_parens]);

		  ECHO;
 
  		  if ((in_prox_op) && (--prox_parens==0))
		    in_prox_op=0;

		  if (--num_parens == 0)
		    {
		    qecho(printable_text);
		    *printable_text = '\0';
		    }
		  
		}

{ws}|.          { /* Leave all other text alone */ 
		ECHO; }

%%

int
qscan_split_compounds_wrap(void)
{
   return 1;
}
