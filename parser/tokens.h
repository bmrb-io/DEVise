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

  $Log$
  Revision 1.6  1995/10/24 03:12:46  ravim
  Lex changes.

  Revision 1.5  1995/09/23 00:03:55  ravim
  Parser changed to ANSI style - g++ now used uniformly

  Revision 1.4  1995/09/22 15:56:24  jussi
  Added copyright message.
*/

#ifndef TOKENS_H
#define TOKENS_H

#define TOKEN_X 1
#define TOKEN_Y	2
#define TOKEN_COLOR 3
#define TOKEN_SHAPE 4
#define TOKEN_ORIENTATION 5
#define TOKEN_SHAPEATTR 6
#define TOKEN_LEFT_BRACKET 7
#define TOKEN_RIGHT_BRACKET 8
#define TOKEN_MAPPING 9
#define TOKEN_DYNAMIC 10
#define TOKEN_SEMI_COLON 11
#define TOKEN_LEFT_BRACE 12
#define TOKEN_RIGHT_BRACE 13
#define TOKEN_INT 14
#define TOKEN_FLOAT 15
#define TOKEN_IDENT 1
#define TOKEN_STATIC 17
#define TOKEN_COMMA 18
#define TOKEN_SIZE 19
#define TOKEN_PATTERN 20

extern void syntax_error();
extern char *GetText();	                /* Get text until matching '}' */
extern char *GetTokenText();            /* Get input string matching token */
extern int yylex();
#ifdef IBM_POWER_STATION
extern unsigned char yytext[];
#else
extern char *yytext;
#endif

#include <malloc.h>

#endif
