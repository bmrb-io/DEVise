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
extern char *GetText();	/* Get text until matching '}' */
extern char *GetTokenText(); /* get input string matching token */
extern int yylex();
#ifdef IBM_POWER_STATION
extern unsigned char yytext[];
#else
extern char yytext[];
#endif

#endif
