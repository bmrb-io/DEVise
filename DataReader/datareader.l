/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.10  1999/01/18 18:08:06  beyer
  added support for '\r'

  Revision 1.9  1998/11/03 17:53:35  okan
  Fixed Several bugs and changed DataReader to use UtilAtof

  Revision 1.8  1998/10/12 21:24:21  wenger
  Fixed bugs 405, 406, and 408 (all DataReader problems); considerably
  increased the robustness of the DataReader (partly addresses bug 409);
  added test targets in the makefile, and corresponding data and schemas.

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
[ \t]+ ;
\n			{ line_nr++;};
"//".*\n 	{ line_nr++;};
"#".*\n  	{ line_nr++;};
"/*"		BEGIN COMMENT;
<COMMENT>"*/" BEGIN INITIAL;
<COMMENT>. ;
<COMMENT>\n { line_nr++;};

datareader	return SCHEMA_TOKEN;
2.0			return VERSION_TOKEN;
record_separator return REC_SEP_TOKEN; // was 'delimiter'
field_separator	return FLD_SEP_TOKEN; // was 'separator'
nullif		return NULLIF_TOKEN;
maxlen		return MAXLEN_TOKEN;
length		return LENGTH_TOKEN;
quote		return QUOTE_TOKEN;
hidden		return HIDDEN_TOKEN;
int			return INT_TOKEN;
double		return DOUBLE_TOKEN;
string		return STRING_TOKEN;
date		return DATE_TOKEN;
date_format return DATE_FORMAT_TOKEN;
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
						case 'r':
							stringBuf[stringIndex++] = '\r';
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

.			{ return yytext[0]; }
%%
