// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
//
//   $Id$
//
//   Copyright 1997, Shaun Flisakowski
//
//   You may distribute under the terms of either the GNU General Public
//   License or the Artistic License, as specified in the README file.
//
// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o

/* Tokens for Schema Lexer */

#ifndef _TOKEN_H
#define _TOKEN_H

 /* Data items */
#define INUM          200
#define RNUM          201
#define SYMBOL        202    /* An identifier */
#define STRING        203
#define REG_EXPR      204
#define PERL_EXPR     205    /* A perl expression */
#define PERL_SUBR     206    /* A perl subroutine */

 /* Keywords */
#define KY_SCHEMA     300
#define KY_ATTR       301
#define KY_LIST_OF    302
#define KY_TYPEDEF    303
#define KY_ENUM       304
#define KY_TYPE       305

#define KY_CHAR_SET   306
#define KY_BIN_ORDER  307
#define KY_RECSZE     308
#define KY_COMMENT    309
#define KY_SEPARATOR  310
#define KY_WHITESPACE 311
#define KY_DELIMITER  312
#define KY_CONVERTER  313
#define KY_FORMAT     314
#define KY_FILTER     315
#define KY_READER     316
#define KY_GLOBAL     317

#define KY_MIN        325
#define KY_MAX        326
#define KY_MAXLEN     327
#define KY_QUOTE      328
#define KY_POSITION   329
#define KY_LENGTH     330
#define KY_FUNC_OF    331
#define KY_VALUE      332
#define KY_DATE_FRMT  333

 /* Symbols */
#define KY_EQ         350    /*  = */
#define KY_LPAREN     351    /*  ( */
#define KY_RPAREN     352    /*  ) */
#define KY_LBRCKT     353    /*  [ */
#define KY_RBRCKT     354    /*  ] */
#define KY_LBRACE     355    /*  { */
#define KY_RBRACE     356    /*  } */
#define KY_COMMA      357    /*  , */
#define KY_SEMICOLON  358    /*  ; */
#define KY_TO         359    /* .. */

 /* Option keywords */
#define KY_TEXT       400
#define KY_BINARY     401
#define KY_ASCII      402

#define KY_NATIVE     405
#define KY_NETWORK    406

#define KY_SORTED     410
#define KY_UNIQUE     411
#define KY_CONSEC     412
#define KY_NULL_ALLOW 413


 /* Internal Types */
#define KY_INT        500
#define KY_FLOAT      501
#define KY_DOUBLE     502
#define KY_STRING     503
#define KY_DATETIME   504
#define KY_UNIXTIME   505

#endif // _TOKEN_H
