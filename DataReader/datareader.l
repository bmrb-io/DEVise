/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
 */

%option c++
%option prefix="DataReader"
%option yyclass="DataReaderParser"
%option noyywrap
%{
#include "sysdep.h"
#include "DataReaderParser.h"
#include "datareader.tab.h"
int escCheck = 0;

%}

ws		[ \t\r\n]+
eol		("\r")?\n
comment \/\/.*
ident	[a-zA-Z_][a-zA-Z_0-9]*
format  [a-zA-Z_%][a-zA-Z_%]*
sdigit  [0-9]
digit	[0-9]+
confirm [yYnN]

%x	STR
%x  BRACKET
%x	COMMENT

%%
[ \t\n]+ ;
"//".*\n ;
"#".*\n ;
"/*"		BEGIN COMMENT;
<COMMENT>"*/" BEGIN INITIAL;
<COMMENT>. ;
<COMMENT>\n	;

schema		return SCHEMA_TOKEN;
delimiter	return DELIM_TOKEN;
separator	return SEPARATOR_TOKEN;
nullif		return NULLIF_TOKEN;
attr		return ATTR_TOKEN;
skip		return SKIP_TOKEN;
maxlen		return MAXLEN_TOKEN;
length		return LENGTH_TOKEN;
quote		return QUOTE_TOKEN;
hidden		return HIDDEN_TOKEN;
type		return TYPE_TOKEN;
int			return INT_TOKEN;
double		return DOUBLE_TOKEN;
string		return STRING_TOKEN;
date		return DATE_TOKEN;
date_format return DATE_FORMAT;
key			return KEY_TOKEN;
sorted		return SORTED_TOKEN;
encoding	return ENCODING_TOKEN;
C-style		return CSTYLE_TOKEN;
quoted		return QUOTEDSTYLE_TOKEN;
url			return URLSTYLE_TOKEN;
noescape	return NOESCAPESTYLE_TOKEN;

\[			{ 	myString.seekp(0);
				escCheck = 0;
				BEGIN BRACKET;
			}

<BRACKET>\][\+]? {	myString << "+" << "\0" ;
				yylval.string_val = myString.str();
				escCheck = 0;
				BEGIN INITIAL;
				return BRACKET_TOKEN;
			}
<BRACKET>.	{	if (escCheck == 1) {
					switch (yytext[0]) {
						case 'n':
							myString << '\n';
							break;
						case 't':
							myString << '\t';
							break;
						default:
							myString << yytext[0];
					}
					escCheck = 0;
				} else {
					if (yytext[0] == '\\')
						escCheck = 1;
					else 
						myString << yytext[0] ;
				}
			}

\"			{	myString.seekp(0);
				BEGIN STR;
			}

<STR>\"		{	myString << "\0";
				yylval.string_val = myString.str();
				BEGIN INITIAL;
				return STRING_TOKEN;
			}
<STR>.		{	myString << yytext[0];
			}

{digit}		{	yylval.int_val = atoi(yytext);
				return INTVAL_TOKEN;
			}
{confirm}   { 
				yylval.string_val = yytext;
				return CONFIRM_TOKEN;
			}

{ident}		{	yylval.string_val = yytext;
				return IDENT_TOKEN;
			}
.|\n		{ return yytext[0]; }
%%
