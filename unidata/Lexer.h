// *************************************************************************
//
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// *************************************************************************

#ifndef _Lexer_H_
#define _Lexer_H_

#include <iostream.h>

// *************************************************************************

// Leave some space at the start of perl subroutines, so we can attach
// a name onto them easily.

#define    XTRA_SPCE    16

void unilex_init(const char *filename);
void unilex_end();

// *************************************************************************

union UniLexType {
    int            Int;
    double         Float;

    char          *Str;
    char          *RegExpr;

    char          *PerlCode;
};


int  uni_yylex(union UniLexType *ult);

extern int yyin_fd;
extern ostream *perr;

extern int yylineno;
extern int yycolno;
extern int yyoffset;
extern const char *filename;

void show_token(FILE *fp, int tok, union UniLexType *ult);
void show_token(ostream *os, int tok, union UniLexType *ult);

// *************************************************************************

#endif // _Lexer_H_
