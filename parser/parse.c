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
  Revision 1.5  1995/11/25 19:01:13  jussi
  Updated copyright notice and cleaned up the code a bit.

  Revision 1.4  1995/09/23  00:03:53  ravim
  Parser changed to ANSI style - g++ now used uniformly

  Revision 1.3  1995/09/22 15:56:22  jussi
  Added copyright message.

  Revision 1.2  1995/09/05 20:35:17  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"
#include "tokens.h"
#include "codegen.h"

static int token;

/*******************************************************************
Get next token. Store in global variable token.
*********************************************************************/

int GetToken()
{
  return (token = yylex());
}

/*******************************************************************
Print a mapping records
********************************************************************/

void PrintMappingRec(MappingRec *rec)
{
  printf("name: %s\n", rec->name);
  printf("recordType: %s\n", rec->recType);
  printf("dynamic fields: \n");
  if (rec->dynamicFields & BIT_X)
    printf("x ");
  if (rec->dynamicFields & BIT_Y)
    printf("y ");
  if (rec->dynamicFields & BIT_COLOR)
    printf("color ");
  if (rec->dynamicFields & BIT_PATTERN)
    printf("pattern ");
  if (rec->dynamicFields & BIT_SHAPE)
    printf("shape ");
  if (rec->dynamicFields & BIT_ORIENTATION)
    printf("orientation ");
  printf("\ndefault text:\n");
  printf("%s",rec->defaultText);
  printf("\nmapping text:\n");
  printf("%s", rec->mappingText);
  printf("%d shapeNames: ",rec->numShapeNames);
  for(int i = 0; i < rec->numShapeNames; i++)
    printf("%s ", rec->shapeNames[i]);
  printf("\n");
}

/************************************************************
Parse shape name fields
**************************************************************/

void ParseShapes(MappingRec *rec)
{
  if (token != TOKEN_SHAPE)
    syntax_error();
  
  if (GetToken() != TOKEN_LEFT_BRACE)
    syntax_error();
  
  rec->numShapeNames = 0;
  while (GetToken()!= TOKEN_RIGHT_BRACE) {
    if (token == TOKEN_COMMA)
      /* skip comma */
      continue;
    if (token != TOKEN_IDENT)
      syntax_error();
    if (rec->numShapeNames >= MAX_SHAPE_DEFS) {
      fprintf(stderr,"too many shapes\n");
      exit(1);
    }
    rec->shapeNames[rec->numShapeNames++]= GetTokenText();
  }
}

/*************************************************************
Parse static fields
**************************************************************/

void ParseStatic(MappingRec *rec)
{
  if (token != TOKEN_STATIC)
    syntax_error();
  
  if (GetToken() != TOKEN_LEFT_BRACE)
    syntax_error();
  
  rec->defaultText = GetText();
  
  if (GetToken() != TOKEN_RIGHT_BRACE)
    syntax_error();
}

/***************************************************************
Parse dynamic attributes
*****************************************************************/

void ParseAttrs(MappingRec *rec)
{
  int num = 0;

  while (token != TOKEN_LEFT_BRACE) {
    switch(token) {

    case TOKEN_X:
      rec->dynamicFields |= BIT_X;
      break;

    case TOKEN_Y:
      rec->dynamicFields |= BIT_Y;
      break;

    case TOKEN_COLOR:
      rec->dynamicFields |= BIT_COLOR;
      break;

    case TOKEN_SHAPE:
      rec->dynamicFields |= BIT_SHAPE;
      break;

    case TOKEN_PATTERN:
      rec->dynamicFields |= BIT_PATTERN;
      break;

    case TOKEN_SIZE:
      rec->dynamicFields |= BIT_SIZE;
      break;

    case TOKEN_ORIENTATION:
      rec->dynamicFields |= BIT_ORIENTATION;
      break;

    case TOKEN_SHAPEATTR:
      if (GetToken()  != TOKEN_LEFT_BRACKET)
	syntax_error();
      if (GetToken() != TOKEN_INT) {
	syntax_error();
      }
      num = atoi(yytext);
      if (num < 0 || num > MAX_ATTRS) {
	printf("shape attr %d invalid\n", num);
	exit(2);
      }
      rec->dynamicAttrs |= (1 << num);
      if (GetToken() != TOKEN_RIGHT_BRACKET)
	syntax_error();
      break;

    default:
      syntax_error();
      break;
    }

    if (GetToken() == TOKEN_COMMA)
      GetToken();
  }
}

/****************************************************************
Parse dynamic fields
****************************************************************/

void ParseDynamic(MappingRec *rec)
{
  if (token != TOKEN_DYNAMIC)
    syntax_error();
	
  GetToken();
  ParseAttrs(rec);
  
  if (token != TOKEN_LEFT_BRACE)
    syntax_error();
  
  rec->mappingText = GetText();
  
  if (GetToken() != TOKEN_RIGHT_BRACE)
    syntax_error();
}

/*************************************************************
Parse a mapping and return the mapping data structures 
Return NULL if EOF has been reached.
***************************************************************/

MappingRec *ParseMapping()
{
  MappingRec *rec;

  if (token != TOKEN_MAPPING) {
    if (token == 0)
      return NULL;
    syntax_error();
  }
  
  if ((rec = (MappingRec *)malloc(sizeof(MappingRec))) == NULL) {
    printf("out of memory\n");
    exit(1);
  }
  rec->dynamicFields = 0;
  rec->dynamicAttrs = 0;
  
  if (GetToken() != TOKEN_IDENT)
    syntax_error();
  rec->name = GetTokenText();
	
  
  if (GetToken() != TOKEN_IDENT)
    syntax_error();
  rec->recType = GetTokenText();
  
  if (GetToken()!= TOKEN_LEFT_BRACE)
    syntax_error();
  
  GetToken();
  ParseShapes(rec);
  
  GetToken();
  ParseStatic(rec);
  
  GetToken();
  ParseDynamic(rec);
  
  if (GetToken() != TOKEN_RIGHT_BRACE)
    syntax_error();
  
  return rec;
}

void Parse(FILE *mapFile)
{
  MappingRec *mapping;

  while (GetToken() != 0) {
    mapping = ParseMapping();
#if 0
    PrintMappingRec(mapping);
#endif
    CodeGen(mapping, mapFile);
  }
}
