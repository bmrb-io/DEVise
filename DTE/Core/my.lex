/*======================================================================== 
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
  Revision 1.23  1997/11/12 23:17:39  donjerko
  Improved error checking.

  Revision 1.22  1997/11/08 21:02:29  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

  Revision 1.21  1997/11/05 00:19:46  donjerko
  Separated typechecking from optimization.

  Revision 1.20  1997/09/05 22:20:17  donjerko
  Made changes for port to NT.

  Revision 1.19  1997/08/25 15:28:13  donjerko
  Added minmax table

  Revision 1.18  1997/08/22 23:13:06  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.17  1997/08/21 21:04:31  donjerko
  Implemented view materialization

  Revision 1.16  1997/04/08 01:47:35  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.15  1997/03/28 16:07:30  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

  Revision 1.14  1997/03/14 18:36:11  donjerko
  Making space for the SQL UNION operator.

  Revision 1.13  1997/03/11 22:49:26  donjerko
  *** empty log message ***

  Revision 1.12  1997/02/21 01:38:06  donjerko
  Fixed some problems with "group by" clause.

  Revision 1.11  1997/02/18 18:06:04  donjerko
  Added skeleton files for sorting.

  Revision 1.10  1997/02/03 04:11:34  donjerko
  Catalog management moved to DTE

  Revision 1.9  1996/12/27 04:38:00  kmurli
  Nodified joins.h to remove erros in joinprev in case of more than one table

  Revision 1.8  1996/12/24 21:00:52  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.7  1996/12/21 22:21:49  donjerko
  Added hierarchical namespace.

  Revision 1.6  1996/12/19 08:25:47  kmurli
  Changes to include the with predicate in sequences.

  Revision 1.5  1996/12/15 06:41:08  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/09 10:01:52  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

 */

%{
#include <string>
#include <string>
//#include <stdlib.h>   erased for sysdep.h
#include <assert.h>
//#include <memory.h>   erased for sysdep.h
#include "myopt.h"
#include "ParseTree.h"
#include "my.yacc.tab.h"
#include "Utility.h"

extern const char* queryString;
extern bool rescan;

static int my_yyinput(char* buf, int max_size){
	int retVal = YY_NULL;
	if(rescan){
		rescan = false;
		int queryLength = strlen(queryString) + 1;
		assert(queryLength < max_size);
		memcpy(buf, queryString, queryLength);
		retVal = queryLength;
	}
	return retVal;
}

#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) (result = my_yyinput(buf, max_size))

%}

Digit        [0-9]
IntLit       {Digit}+
DecLit       {Digit}*"."{Digit}+
SignedIntLit [+\-]{Digit}+
String       [A-Za-z][A-Za-z0-9_]*
LessGreat    ">="|">"|"<="|"<"
%%
[ \t\n]+     {}
[Tt][Yy][Pp][Ee][Cc][Hh][Ee][Cc][Kk]	{return TYPE_CHECK;}
[Ss][Ee][Ll][Ee][Cc][Tt]       {return SELECT;}
[Ff][Rr][Oo][Mm]         {return FROM;}
[Aa][Ss]				{return AS;}
[Ww][Hh][Ee][Rr][Ee]        {return WHERE;}
[Ss][eE][qQ][uU][Ee][Nn][cC][eE] {return SEQUENCE;}
[Gg][Rr][Oo][uU][Pp]  {return GROUP;}
[Oo][Rr][Dd][Ee][Rr]  {return ORDER;}
[Bb][Yy]			  {return BY;}
[Aa][Ss][Cc] {return ASC;}
[Dd][Ee][Ss][Cc] {return DESC;}
[Jj][Oo][Ii][Nn][Pp][Rr][Ee][Vv]  {return JOINPREV;}
[Jj][Oo][Ii][Nn][Nn][Ee][Xx][Tt]  {return JOINNEXT;}
[Oo][Vv][Ee][rR]         {return OVER;}
[Ww][Ii][Tt][Hh]         {return WITH;}
[Hh][Aa][Vv][Ii][Nn][Gg] {return HAVING;}
[Aa][Nn][Dd]          {return AND;}
[Oo][Rr]           {return OR;}
[Cc][Rr][Ee][Aa][Tt][Ee]		{return CREATE;}
[Dd][Rr][Oo][Pp]			{return DROP;}
[Ii][Nn][Dd][Ee][Xx]		{return INDEX;}
[Oo][Nn]				{return ON;}
[Ii][Nn][Ss][Ee][Rr][Tt]		{return INSERT;}
[Vv][Aa][Ll][Uu][Ee][Ss]		{return VALUES;}
[Ii][Nn][Tt][Oo]			{return INTO;}
[Dd][Ee][Ll][Ee][Tt][Ee]		{return DELETEY;}
[Ss][Cc][Hh][Ee][Mm][Aa]		{return SCHEMA;}
[Mm][Aa][Tt][Ee][Rr][Ii][Aa][Ll][Ii][Zz][Ee]	{return MATERIALIZE;}
[Aa][Dd][Dd]				{return ADD;}
[Uu][Nn][Ii][Oo][Nn]		{return UNION;}

{String}     {yylval.stringLit = new string(yytext); return STRING;}
{IntLit}     {yylval.integer = atoi(yytext); return INTY;}
{DecLit}     {yylval.real = atof(yytext); return DOUBLEY;}
{SignedIntLit}  {yylval.integer = atoi(yytext); return INTY;}
{LessGreat}  {yylval.stringLit = new string(yytext); return LESSGREATER;}
\"([^\"]|\\\")*\" {
             size_t textLen = strlen(yytext) + 1;
             char* tmp = new char[textLen];
             stripQuotes(yytext, tmp, textLen);
             yylval.stringLit = new string(tmp); 
             delete [] tmp;
		   return STRING_CONST;
		   }
.            {return yytext[0];}
%%
int yywrap(){
	yy_flush_buffer(YY_CURRENT_BUFFER);
	yyrestart(yyin);	// no effect warning?
     return 1;
}

extern int yyparse();

int my_yyparse(){
	yyrestart(yyin);
	return yyparse();
}
