%{
/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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

  $Log$*/

#include "tokens.h"
%}

%%
"x"			return(TOKEN_X);
"y"			return(TOKEN_Y);
"color" 		return(TOKEN_COLOR);
"shape"			return(TOKEN_SHAPE);
"orientation"		return(TOKEN_ORIENTATION);
"shapeAttr"		return(TOKEN_SHAPEATTR);
"["			return(TOKEN_LEFT_BRACKET);
"]"			return(TOKEN_RIGHT_BRACKET);
"mapping"		return(TOKEN_MAPPING);
"dynamic"		return(TOKEN_DYNAMIC);
"static"		return(TOKEN_STATIC);
";"			return(TOKEN_SEMI_COLON);
","			return(TOKEN_COMMA);
"{"			return(TOKEN_LEFT_BRACE);
"}"			return(TOKEN_RIGHT_BRACE);
[+-][0-9]+		return(TOKEN_INT);
[0-9]+			return(TOKEN_INT);
[a-zA-Z_][A-Za-z0-9_]+	return(TOKEN_IDENT);
" "	;
\t	;
\n	;
.	syntax_error();
%%

/* user subroutines */

void syntax_error()
{
  printf("syntax error, buffer contains %s\n", yytext);
  exit(2);
}

/*******************************************************************
Read text until a matching '}' is found
********************************************************************/

char *GetText()
{
  char buf[2048];
  int level = 1;
  int done = 0;
  int index = 0;
  char *text;

  while (!done) {
    char c = yyinput();
    if (c == 0)
      syntax_error();
    else if (c == '{') {
      level++;
      buf[index++] = c;
    }
    else if (c == '}') {
      if (--level == 0) {
	unput(c);
	done = 1;
      } else {
	buf[index++] = c;
      }
    } else {
      buf[index++] = c;
    }
  }
  buf[index++] = '\0';
  
  if (!(text = (char *)malloc(index))) {
    printf("no memory\n");
    exit(2);
  }

  memcpy(text, buf, index);

  return text;
}

/********************************************************************
Return the current text
*********************************************************************/

char *GetTokenText()
{
  char *text;

  if (!(text = (char *)malloc(yyleng + 1))) {
    printf("no memory\n");
    exit(2);
  }
  strcpy(text, yytext);
  return text;
}

yywrap()
{
}
