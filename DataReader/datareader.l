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
  Revision 1.7  1998/09/30 17:26:59  beyer
  remove 'y' and 'n' as key words.

  Revision 1.6  1998/06/25 23:41:20  okan
  *** empty log message ***

  Revision 1.5  1998/06/24 09:24:15  okan
  *** empty log message ***

  Revision 1.4  1998/06/16 16:30:54  wenger
  Added standard headers to DataReader sources.

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
comment     return COMMENT_TOKEN;

\[			{ 	stringIndex = 0;
				escCheck = 0;
				BEGIN BRACKET;
			}

<BRACKET>\][\+]? { if (yytext[1] == '+') {
					stringBuf[stringIndex++] = '+' ;
				}
				stringBuf[stringIndex++] = '\0' ;
				yylval.string_val = stringBuf;
				escCheck = 0;
				BEGIN INITIAL;
				return BRACKET_TOKEN;
			}
<BRACKET>.	{	if (escCheck == 1) {
					switch (yytext[0]) {
						case 'n':
							stringBuf[stringIndex++] = '\n';
							break;
						case 't':
							stringBuf[stringIndex++] = '\t';
							break;
						default:
							stringBuf[stringIndex++] = yytext[0];
					}
					escCheck = 0;
				} else {
					if (yytext[0] == '\\')
						escCheck = 1;
					else {
						stringBuf[stringIndex++] = yytext[0] ;
					}
				}
			}

\"			{	stringIndex = 0;
				BEGIN STR;
			}

<STR>\"		{	stringBuf[stringIndex++] = '\0';
				yylval.string_val = stringBuf;
				BEGIN INITIAL;
				return STRINGVAL_TOKEN;
			}
<STR>.		{	stringBuf[stringIndex++] = yytext[0];
			}

{digit}		{	yylval.int_val = atoi(yytext);
				return INTVAL_TOKEN;
			}

{ident}		{	yylval.string_val = yytext;
				return IDENT_TOKEN;
			}

.|\n		{ return yytext[0]; }

%%
