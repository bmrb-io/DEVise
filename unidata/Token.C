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

#include <stdio.h>
#include <iostream.h>

#include "Lexer.h"
#include "Token.h"

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void show_token(FILE *fp, int tok, UniLexType *ult)
{
    switch(tok) {

    case INUM:
        if (ult)
            fprintf(fp, "<integer>: %d ", ult->Int);
        else
            fprintf(fp, "<integer> ");
        break;

    case RNUM:
        if (ult)
            fprintf(fp, "<real>: %f ", ult->Float);
        else
            fprintf(fp, "<real> ");
        break;

    case SYMBOL:    /* An identifier */
        if (ult)
            fprintf(fp, "<symbol>: %s ", ult->Str);
        else
            fprintf(fp, "<symbol> ");
        break;

    case STRING:
        if (ult)
            fprintf(fp, "<string>: %s ", ult->Str);
        else
            fprintf(fp, "<string> ");
        break;

    case REG_EXPR:
        if (ult)
            fprintf(fp, "<regexpr>: %s ", ult->RegExpr);
        else
            fprintf(fp, "<regexpr> ");
        break;

    case PERL_EXPR:    /* A perl expression */
        if (ult)
            fprintf(fp, "<perlexpr>: %s ", ult->PerlCode);
        else
            fprintf(fp, "<perlexpr> ");
        break;

    case PERL_SUBR:    /* A perl subroutine */
        if (ult)
            fprintf(fp, "<perlsubr>: %s ", ult->PerlCode);
        else
            fprintf(fp, "<perlsubr> ");
        break;


     /* Keywords */
    case KY_SCHEMA:
        fputs("schema ",fp);
        break;

    case KY_ATTR: 
        fputs("attr ",fp);
        break;

    case KY_LIST_OF: 
        fputs("list_of ",fp);
        break;

    case KY_TYPEDEF:
        fputs("typedef ",fp);
        break;

    case KY_ENUM: 
        fputs("enum ",fp);
        break;

    case KY_TYPE: 
        fputs("type ",fp);
        break;


    case KY_CHAR_SET:
        fputs("char_set ",fp);
        break;

    case KY_BIN_ORDER:
        fputs("binary_order ",fp);
        break;

    case KY_RECSZE: 
        fputs("recsze ",fp);
        break;

    case KY_COMMENT:
        fputs("comment ",fp);
        break;

    case KY_SEPARATOR:
        fputs("separator ",fp);
        break;

    case KY_DELIMITER:
        fputs("delimiter ",fp);
        break;

    case KY_CONVERTER:
        fputs("converter ",fp);
        break;

    case KY_FORMAT: 
        fputs("format ",fp);
        break;

    case KY_FILTER: 
        fputs("filter ",fp);
        break;

    case KY_READER: 
        fputs("reader ",fp);
        break;

    case KY_GLOBAL: 
        fputs("global ",fp);
        break;


    case KY_MIN:
        fputs("min ",fp);
        break;

    case KY_MAX:
        fputs("max ",fp);
        break;

    case KY_MAXLEN: 
        fputs("maxlen ",fp);
        break;

    case KY_QUOTE: 
        fputs("quote ",fp);
        break;

    case KY_POSITION:
        fputs("position ",fp);
        break;

    case KY_LENGTH:
        fputs("length ",fp);
        break;

    case KY_FUNC_OF:
        fputs("function_of ",fp);
        break;

    case KY_VALUE:
        fputs("value ",fp);
        break;

    case KY_DATE_FRMT:
        fputs("date_format ",fp);
        break;


     /* Symbols */
    case KY_EQ:         /*  = */
        fputs("= ",fp);
        break;

    case KY_LPAREN:     /*  ( */
        fputs("( ",fp);
        break;

    case KY_RPAREN:     /*  ) */
        fputs(") ",fp);
        break;

    case KY_LBRCKT:     /*  [ */
        fputs("[ ",fp);
        break;

    case KY_RBRCKT:     /*  ] */
        fputs("] ",fp);
        break;

    case KY_LBRACE:     /*  { */
        fputs("{ ",fp);
        break;

    case KY_RBRACE:     /*  } */
        fputs("} ",fp);
        break;

    case KY_COMMA:        /*  , */
        fputs(", ",fp);
        break;

    case KY_SEMICOLON:    /*  ; */
        fputs("; ",fp);
        break;

    case KY_TO:         /* .. */
        fputs(".. ",fp);
        break;


     /* Option keywords */
    case KY_TEXT: 
        fputs("text ",fp);
        break;

    case KY_BINARY: 
        fputs("binary ",fp);
        break;

    case KY_ASCII:
        fputs("ascii ",fp);
        break;


    case KY_NATIVE: 
        fputs("native ",fp);
        break;

    case KY_NETWORK:
        fputs("network ",fp);
        break;


    case KY_SORTED: 
        fputs("sorted ",fp);
        break;

    case KY_UNIQUE: 
        fputs("unique ",fp);
        break;

    case KY_CONSEC: 
        fputs("consecutive ",fp);
        break;

    case KY_NULL_ALLOW:
        fputs("null_allowed ",fp);
        break;



     /* Internal Types */
    case KY_INT:
        fputs("int ",fp);
        break;

    case KY_FLOAT: 
        fputs("float ",fp);
        break;

    case KY_DOUBLE: 
        fputs("double ",fp);
        break;

    case KY_STRING: 
        fputs("string ",fp);
        break;

    case KY_DATETIME:
        fputs("datetime ",fp);
        break;

    case KY_UNIXTIME:
        fputs("unixtime ",fp);
        break;

    default:
        fprintf(fp, "Unknown token: %d\n", tok);
        break;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
void show_token(ostream *os, int tok, UniLexType *ult)
{

    switch(tok) {

    case INUM:
        if (ult)
            *os << "<integer>: " << ult->Int;
        else
            *os << "<integer>";
        break;

    case RNUM:
        if (ult)
            *os << "<real>: " << ult->Float;
        else
            *os << "<real>";
        break;

    case SYMBOL:    /* An identifier */
        if (ult)
            *os << "<symbol>: " << ult->Str;
        else
            *os << "<symbol>";
        break;

    case STRING:
        if (ult)
            *os << "<string>: " << ult->Str;
        else
            *os << "<string>";
        break;

    case REG_EXPR:
        if (ult)
            *os << "<regexpr>: " << ult->RegExpr;
        else
            *os << "<regexpr>";
        break;

    case PERL_EXPR:    /* A perl expression */
        if (ult)
            *os << "<perlexpr>: " << ult->PerlCode;
        else
            *os << "<perlexpr>";
        break;

    case PERL_SUBR:    /* A perl subroutine */
        if (ult)
            *os << "<perlsubr>: " << ult->PerlCode;
        else
            *os << "<perlsubr>";
        break;


     /* Keywords */
    case KY_SCHEMA:
        *os << "schema ";
        break;

    case KY_ATTR: 
        *os << "attr ";
        break;

    case KY_TYPEDEF:
        *os << "typedef ";
        break;

    case KY_ENUM: 
        *os << "enum ";
        break;

    case KY_TYPE: 
        *os << "type ";
        break;


    case KY_CHAR_SET:
        *os << "char_set ";
        break;

    case KY_BIN_ORDER:
        *os << "binary_order ";
        break;

    case KY_RECSZE: 
        *os << "recsze ";
        break;

    case KY_COMMENT:
        *os << "comment ";
        break;

    case KY_SEPARATOR:
        *os << "separator ";
        break;

    case KY_DELIMITER:
        *os << "delimiter ";
        break;

    case KY_CONVERTER:
        *os << "converter ";
        break;

    case KY_FORMAT: 
        *os << "format ";
        break;

    case KY_FILTER: 
        *os << "filter ";
        break;

    case KY_READER: 
        *os << "reader ";
        break;

    case KY_GLOBAL: 
        *os << "global ";
        break;


    case KY_MIN:
        *os << "min ";
        break;

    case KY_MAX:
        *os << "max ";
        break;

    case KY_MAXLEN: 
        *os << "maxlen ";
        break;

    case KY_QUOTE: 
        *os << "quote ";
        break;

    case KY_POSITION:
        *os << "position ";
        break;

    case KY_LENGTH:
        *os << "length ";
        break;

    case KY_FUNC_OF:
        *os << "function_of ";
        break;

    case KY_VALUE:
        *os << "value ";
        break;


     /* Symbols */
    case KY_EQ:         /*  = */
        *os << "= ";
        break;

    case KY_LPAREN:     /*  ( */
        *os << "( ";
        break;

    case KY_RPAREN:     /*  ) */
        *os << ") ";
        break;

    case KY_LBRCKT:     /*  [ */
        *os << "[ ";
        break;

    case KY_RBRCKT:     /*  ] */
        *os << "] ";
        break;

    case KY_LBRACE:     /*  { */
        *os << "{ ";
        break;

    case KY_RBRACE:     /*  } */
        *os << "} ";
        break;

    case KY_COMMA:      /*  , */
        *os << ", ";
        break;

    case KY_SEMICOLON:  /*  ; */
        *os << "; ";
        break;

    case KY_TO:         /* .. */
        *os << ".. ";
        break;


     /* Option keywords */
    case KY_TEXT: 
        *os << "text ";
        break;

    case KY_BINARY: 
        *os << "binary ";
        break;

    case KY_ASCII:
        *os << "ascii ";
        break;


    case KY_NATIVE: 
        *os << "native ";
        break;

    case KY_NETWORK:
        *os << "network ";
        break;


    case KY_SORTED: 
        *os << "sorted ";
        break;

    case KY_UNIQUE: 
        *os << "unique ";
        break;

    case KY_CONSEC: 
        *os << "consecutive ";
        break;

    case KY_NULL_ALLOW:
        *os << "null_allowed ";
        break;



     /* Internal Types */
    case KY_INT:
        *os << "int ";
        break;

    case KY_FLOAT: 
        *os << "float ";
        break;

    case KY_DOUBLE: 
        *os << "double ";
        break;

    case KY_STRING: 
        *os << "string ";
        break;

    case KY_DATETIME:
        *os << "datetime ";
        break;

    case KY_UNIXTIME:
        *os << "unixtime ";
        break;

    default:
        *os << "Unknown token: " << tok << endl;
        break;
    }
}

// o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o+o
