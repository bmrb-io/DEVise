/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.6  1996/12/19 08:25:47  kmurli
  Changes to include the with predicate in sequences.

  Revision 1.5  1996/12/15 06:41:08  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/09 10:01:52  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.3  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.2  1996/12/05 16:06:02  wenger
  Added standard Devise file headers.

 */

%{
#include <String.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#include "myopt.h"
#include "my.yacc.tab.h"

extern char* queryString;

static int global_counter_ = 0;

#define YY_INPUT(buf,result,max_size) \
    { \
    if(global_counter_){ \
	    result = YY_NULL; \
    } \
    int queryLength = strlen(queryString);\
    assert(queryLength < max_size); \
    memcpy(buf, queryString, queryLength); \
    result = queryLength; \
    global_counter_++; \
    }

char* stripquotes(char* text){
	char* retVal = strdup(text + 1);
	int len = strlen(retVal);
	int j = 0;
	for(int i = 0; i < len - 1; i++){
		if(retVal[i] == '"'){
			assert(j > 0);
			retVal[--j] = retVal[i];
		}
		else{
			retVal[j] = retVal[i];
		}
		j++;
	}
	retVal[j] = '\0';
	return retVal;
}
		
%}

Digit        [0-9]
IntLit       {Digit}+
DecLit       {Digit}*"."{Digit}+
SignedIntLit [+\-]{Digit}+
String       [A-Za-z][A-Za-z0-9_]*
LessGreat    ">="|">"|"<="|"<"
%%
[ \t\n]+     {}
[Ss][Ee][Ll][Ee][Cc][Tt]       {return SELECT;}
[Ff][Rr][Oo][Mm]         {return FROM;}
[Aa][Ss]				{return AS;}
[Ww][Hh][Ee][Rr][Ee]        {return WHERE;}
[Ss][eE][qQ][uU][Ee][Nn][cC][eE][bB][yY]  {return SEQUENCEBY;}
[Oo][Vv][Ee][rR]         {return OVER;}
[Ww][Ii][Tt][Hh]         {return WITH;}
[Aa][Nn][Dd]          {return AND;}
[Oo][Rr]           {return OR;}
[Cc][Rr][Ee][Aa][Tt][Ee]		{return CREATE;}
[Ii][Nn][Dd][Ee][Xx]		{return INDEX;}
[Oo][Nn]				{return ON;}
{String}     {yylval.string = new String(yytext); return STRING;}
{IntLit}     {yylval.integer = atoi(yytext); return INT;}
{DecLit}     {yylval.real = atof(yytext); return DOUBLE;}
{SignedIntLit}  {yylval.integer = atoi(yytext); return INT;}
{LessGreat}  {yylval.string = new String(yytext); return LESSGREATER;}
\"([^\"]|\\\")*\" {
             yylval.string = new String(stripquotes(yytext)); 
		   return STRING_CONST;
		   }
.            {return yytext[0];}
%%
int yywrap(){
     return 1;
}
