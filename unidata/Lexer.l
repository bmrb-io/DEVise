
%{
/*************************************************
 *   UniData Lexer
 *   $Id$
 *
 *   Copyright 1997, Shaun Flisakowski
 *
 *   You may distribute under the terms of either the GNU General Public
 *   License or the Artistic License, as specified in the README file.
 *
 *************************************************/

#undef yywrap
int yywrap();

#define  YY_NO_UNPUT
#define  YY_SKIP_YYWRAP
#define  YY_BUFFER_STATE_DEFD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Lexer.h"
#include "Token.h"

#include "fast_atof.h"
#include "SlideRead.h"

/* For Flex compatibility  */
#define YYSTYPE UniLexType
int  yylex(YYSTYPE *ult);


#ifndef MAX_LN_LEN
#define MAX_LN_LEN    1024
#endif

#ifndef MAX_STR_LEN
#define MAX_STR_LEN    1024
#endif

int      yyin_fd;
ostream *perr;

const char *filename;
int   yylineno;
int   yycolno;
int   yynxtcol;
int   yyoffset;
int   brckt_cnt;

int   err_cnt = 0;

YY_BUFFER_STATE lex_buff = NULL;

static char  str_buff[MAX_STR_LEN+4];
static char *str_end = &str_buff[MAX_STR_LEN-1];
static char *str_ptr;

    /* Helpful for filling a small area with spaces */
static char *xtra_spces =
    "                                                                ";

  /* To assist with arbitrary-sized perl code sections */
#define INIT_PERL_SZE   256

int  pcode_sze;
int  pcode_used;

void  yywarn  (char *s);
int   yyerr   (char *s);
void  chk_pcode(UniLexType *ult, int more);
void  yycnt(char *txt, int len);

#undef YY_INPUT

    // Scan input from a file descriptor
#define YY_INPUT(buf,result,max_size) \
    { \
        if ((result = SlideRead(buf,max_size)) <= 0) { \
           result = YY_NULL; \
        } \
    }

#define YY_USER_ACTION \
    { \
        yycolno = yynxtcol; \
        yynxtcol += yyleng; \
        yyoffset += yyleng; \
    }


YYSTYPE my_var;

/*  To give some args to yylex */
#undef YY_DECL
#define YY_DECL int yylex YY_PROTO((YYSTYPE *lvalp))

%}
%x Start STR RE P_EXPR P_SUB P_SOON
%a 9000
%o 9000

dot		"."
digit		[0-9]
octdigit	[0-7]
hexdigit	[0-9a-fA-F]
alpha		[a-zA-Z_]
alphanum	{alpha}|{digit}
litquote	\\\"

number		{digit}+
numsuffix       [uU]?[lL]?
intnum		{digit}+{numsuffix}
octnum		0{octdigit}+{numsuffix}
hexnum		0[xX]{hexdigit}+{numsuffix}

frac		{dot}{number}
mantissa	{number}|{number}{dot}|{number}{frac}|{frac}
exponent	[Ee][\+\-]?{number}
realsuffix      [fFlL]
real		{mantissa}{exponent}?{realsuffix}?

ws              [ \t\f\v]
wsn             [ \t\f\v\n]

%%

<INITIAL>\'[^\\\']*\' {  /* Simple quoted strings */
                        lvalp->Str = new char[yyleng-1];
                        memcpy(lvalp->Str,yytext+1,yyleng-1);
                        (lvalp->Str)[yyleng-2] = '\0';
                        return STRING;
                      }

<INITIAL>\'           { /* A more complex string, containing \t, etc. */
                        BEGIN(STR);
                        str_ptr = str_buff;
                      }

<INITIAL>\/[^\/]*\/   {  /* Simple quoted regular expression */
                        lvalp->RegExpr = new char[yyleng+1];
                        memcpy(lvalp->RegExpr,yytext,yyleng);
                        (lvalp->RegExpr)[yyleng] = '\0';
                        return REG_EXPR;
                      }

<P_EXPR>[^;]*;        {  /* A perl expression, w/ no semicolons inside it */
                        yycnt(yytext,yyleng);
                        lvalp->PerlCode = new char[yyleng+1];
                        memcpy(lvalp->PerlCode,yytext,yyleng);
                        (lvalp->PerlCode)[yyleng] = '\0';
                        BEGIN(INITIAL);
                        return PERL_EXPR;
                      }

<P_SOON>{wsn}*={wsn}*"{"  {
                            yycnt(yytext,yyleng);
                            BEGIN(P_SUB);
                            yyless(yyleng-1);    /* We don't want the { */
                            return(KY_EQ);
                          }

<P_SOON>{wsn}*={wsn}*   {
                          yycnt(yytext,yyleng);
                          BEGIN(P_EXPR);
                          return(KY_EQ);
                        }

<INITIAL>"/"          {  /* Start of more complex RE */
                          BEGIN(RE);
                          str_ptr = str_buff;
                         *str_ptr++ = '/';
                      }

<INITIAL>"#"[^*].*$ { /* Perl style comment */
                    }

<INITIAL>"schema"   { return(KY_SCHEMA); }
<INITIAL>"attr"     { return(KY_ATTR); }
<INITIAL>"list_of"  { return(KY_LIST_OF); }
<INITIAL>"typedef"  { return(KY_TYPEDEF); }
<INITIAL>"enum"     { return(KY_ENUM); }
<INITIAL>"type"     { return(KY_TYPE); }
<INITIAL>"char_set" { return(KY_CHAR_SET); }
<INITIAL>"binary_order"  { return(KY_BIN_ORDER); }
<INITIAL>"recsze"     { return(KY_RECSZE); }
<INITIAL>"comment"    { return(KY_COMMENT); }
<INITIAL>"separator"  { return(KY_SEPARATOR); }
<INITIAL>"whitespace" { return(KY_WHITESPACE); }
<INITIAL>"delimiter"  { return(KY_DELIMITER); }
<INITIAL>"converter"  { return(KY_CONVERTER); }
<INITIAL>"format"   { return(KY_FORMAT); }
<INITIAL>"filter"   { BEGIN(P_SOON);  return(KY_FILTER); }
<INITIAL>"reader"   { BEGIN(P_SOON);  return(KY_READER); }
<INITIAL>"global"   { BEGIN(P_SOON);  return(KY_GLOBAL); }

<INITIAL>"min"      { return(KY_MIN); }
<INITIAL>"max"      { return(KY_MAX); }
<INITIAL>"maxlen"   { return(KY_MAXLEN); }
<INITIAL>"quote"    { return(KY_QUOTE); }
<INITIAL>"position" { return(KY_POSITION); }
<INITIAL>"length"   { return(KY_LENGTH); }
<INITIAL>"function_of"  { return(KY_FUNC_OF); }
<INITIAL>"value"    { BEGIN(P_SOON);  return(KY_VALUE); }
<INITIAL>"date_format" { return(KY_DATE_FRMT); }

<INITIAL>"text"     { return(KY_TEXT); }
<INITIAL>"binary"   { return(KY_BINARY); }
<INITIAL>"ascii"    { return(KY_ASCII); }
<INITIAL>"native"   { return(KY_NATIVE); }
<INITIAL>"network"  { return(KY_NETWORK); }

<INITIAL>"sorted"   { return(KY_SORTED); }
<INITIAL>"unique"   { return(KY_UNIQUE); }
<INITIAL>"consecutive"  { return(KY_CONSEC); }
<INITIAL>"null_allowed" { return(KY_NULL_ALLOW); }

<INITIAL>"int"     { return(KY_INT); }
<INITIAL>"float"   { return(KY_FLOAT); }
<INITIAL>"double"  { return(KY_DOUBLE); }
<INITIAL>"string"  { return(KY_STRING); }
<INITIAL>"datetime"  { return(KY_DATETIME); }
<INITIAL>"unixtime"  { return(KY_UNIXTIME); }


<INITIAL>"="    { return(KY_EQ); }

<INITIAL>"("    { return(KY_LPAREN); }
<INITIAL>")"    { return(KY_RPAREN); }
<INITIAL>"["    { return(KY_LBRCKT); }
<INITIAL>"]"    { return(KY_RBRCKT); }
<INITIAL>"{"    { return(KY_LBRACE); }
<INITIAL>"}"    { return(KY_RBRACE); }

<INITIAL>".."   { return(KY_TO); }

<INITIAL>","    { return(KY_COMMA); }
<INITIAL>";"    { return(KY_SEMICOLON); }


<INITIAL>[a-zA-Z_][a-zA-Z0-9_]*  {    /* Identifier */
                     lvalp->Str = new char[yyleng+1];
                     memcpy(lvalp->Str,yytext,yyleng);
                     (lvalp->Str)[yyleng] = '\0';
                     return(SYMBOL);
                   }

<INITIAL>{intnum}  {  /* An integer */
                        lvalp->Int
                               = strtol(yytext,(char **) NULL,0);
                        return(INUM);
                   }
          
<INITIAL>[0-9]*"."?[0-9]*[eE][+-]?[0-9]+? |
<INITIAL>[0-9]*"."[0-9]+([eE][+-]?[0-9]+)? {
                         lvalp->Float = UtilStrtod(yytext);
                         return(RNUM);
                       }

    
<P_SUB>"{"     {   /* An opening bracket */
                   if (brckt_cnt++ == 0) {
                      /* We just started, set up a buffer */
                      lvalp->PerlCode = new char [INIT_PERL_SZE];
                      pcode_used = 0;
                      pcode_sze  = INIT_PERL_SZE;
                      chk_pcode(lvalp,XTRA_SPCE);

                      strncpy(lvalp->PerlCode, xtra_spces, XTRA_SPCE);
                      str_ptr = &((lvalp->PerlCode)[XTRA_SPCE]);
                      pcode_used += XTRA_SPCE;
                   }

                    chk_pcode(lvalp,1);
                   *str_ptr++ = *yytext;
                    pcode_used++;
               }
       
<P_SUB>[^}{\n]+ {    /* Slurp it up */
                 int n = yyleng;

                 chk_pcode(lvalp,n);
                 memcpy(str_ptr,yytext,n);
                 str_ptr += n; 
                 pcode_used += n;
               }

<P_SUB>\n|\r   {
                   chk_pcode(lvalp,1);
                  *str_ptr++ = *yytext;
                   pcode_used++;
                   yylineno++;
                   yynxtcol = 0;
               }

<P_SUB>"}"     {   /* A closing bracket */
                    chk_pcode(lvalp,2);
                   *str_ptr++ = *yytext;
                    pcode_used++;

                   if (--brckt_cnt == 0) {
                      *str_ptr = '\0';
                       BEGIN(INITIAL);
                       return(PERL_SUBR);
                   }
               }

<RE>"/"        {  /* Closing RE */
                   if (str_ptr < str_end) {
                     *str_ptr++ = '/';
                   } else
                     yywarn("Regular expression too long");

                  *str_ptr = '\0';
                   lvalp->RegExpr = new char [(str_ptr-str_buff)+1];
                   memcpy(lvalp->RegExpr,str_buff,(str_ptr-str_buff)+1);
                   BEGIN(INITIAL);
                   return REG_EXPR;
               }

<RE>\\"/"       { /* Escaped slash */
                 if (str_ptr < str_end-1) {
                   *str_ptr++ = '\\';
                   *str_ptr++ = '/';
                 } else
                    yywarn("Regular expression too long");
               }

<RE>[^/]+     {
                 char *yytxt_ptr = yytext;

                 while (*yytxt_ptr && (str_ptr < str_end))
                   *str_ptr++ = *yytxt_ptr++;
 
                 if (str_ptr == str_end)
                    yywarn("String constant too long");
               }

<STR>\'        { /* Closing quote */
                   *str_ptr = '\0';
                   lvalp->Str = new char [(str_ptr-str_buff)+1];
                   memcpy(lvalp->Str,str_buff,(str_ptr-str_buff)+1);
                   BEGIN(INITIAL);
                   return STRING;
               }

<STR>\\n       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\n';
                 else
                    yywarn("String constant too long");
               }

<STR>\\t       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\t';
                 else
                    yywarn("String constant too long");
               }

<STR>\\r       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\r';
                 else
                    yywarn("String constant too long");
               }

<STR>\\b       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\b';
                 else
                    yywarn("String constant too long");
               }

<STR>\\f       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\f';
                 else
                    yywarn("String constant too long");
               }

<STR>\\v       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\v';
                 else
                    yywarn("String constant too long");
               }

<STR>\\a       {
                 if (str_ptr < str_end)
                   *str_ptr++ = '\a';
                 else
                    yywarn("String constant too long");
               }

<STR>\\.       {
                 if (str_ptr < str_end)
                   *str_ptr++ = yytext[1];
                 else
                    yywarn("String constant too long");
               }

<STR>[^\\\']+  {
                 char *yytxt_ptr = yytext;
                 int   n = yyleng;

                 while (*yytxt_ptr && n--)
                   *str_ptr++ = *yytxt_ptr++;
 
                 if (str_ptr == str_end)
                    yywarn("String constant too long");
               }

<INITIAL>{ws}+     { /* space/tab/formfeed/vertical tab (ignore) */
                   }

<INITIAL>\n|\r     {
                     yylineno++;
                     yynxtcol = 0;
                   }

<STR><<EOF>>       { 
                     yyerr("EOF reached inside string constant");
                     return(0);
                   }

.                  { /* Any unknown char is an error */
                    *perr << filename << ":" << yylineno
                          << ": Error - Illegal Character";

                    if ((yytext[0] >= ' ') && (yytext[0] <= '~'))
                        *perr << ": '" << yytext[0] << "'\n";
                    else
                        *perr << ", ASCII: " << oct << (int)yytext[0]
                              << " (octal)\n";
                   }

%%

/************************************************************************
//
// yywrap() - This function is called by the lexer [yylex() or sslex()] when
//            the end-of-file (or end-of-string for sslex()) is reached.
//            It gets the next file/string ready (if any) and returns 1
//            which indicates to the lexer that there are no more files
//            remaining.  The lexer returns 0 to the parser, indicating
//            no more tokens are remaining. The parser function, yyparse(),
//            then returns 0, indicating that the parse is done.  This
//            behaviour allows a single item to be parsed to be placed
//            on the stack and parsed without the entire stack being consumed.
//
// **********************************************************************/

int yywrap()
{
    /* Prepare the next file if there is one but return 1 */
    /* so that yyparse returns anyway. */
    return(1);
}

/***********************************************************************/
void unilex_init(const char *file)
{
    filename = file;
    yylineno = 1;
    yycolno  = 0;
    yynxtcol = 0;
    yyoffset = 0;

    brckt_cnt = 0;

    if (lex_buff)
        yy_delete_buffer(lex_buff);

    lex_buff = yy_create_buffer(NULL,YY_BUF_SIZE);
    yy_switch_to_buffer(lex_buff);
}

void unilex_end()
{
    if (lex_buff)
        yy_delete_buffer(lex_buff);

    lex_buff = NULL;
}

/***********************************************************************/
void  chk_pcode(UniLexType *ult, int more)
{
    if (pcode_used + more > pcode_sze) {
        char *old_code = ult->PerlCode;
        int old_sze = pcode_sze;
        int offst = (str_ptr - ult->PerlCode);

        pcode_sze += pcode_sze;
        ult->PerlCode = new char [pcode_sze];
        memcpy(ult->PerlCode, old_code, old_sze);
        str_ptr = ult->PerlCode + offst;
        delete [] old_code;

        /* Might still not be enough room,
           this can't recurse more than a couple times. */
        chk_pcode(ult,more);
    }
}

/***********************************************************************/
/* Count newlines to keep yylineno correct */
void  yycnt(char *txt, int len)
{
    while(len--) {
        if (*txt++ == '\n') {
            yylineno++;
            yynxtcol = 0;
        }
    } 
}

/***********************************************************************/
void yywarn(char *s)
{
    *perr << filename << ":" << yylineno << ": Warning - " << s << endl;
}

/***********************************************************************/
int yyerr(char *s)
{
    *perr << filename << ":" << yylineno << ": " << s << endl;

    if (++err_cnt >= 10){
        *perr << "Too many errors (" << err_cnt << " detected) - quitting.\n";
        fclose(yyin);
        return(1);
    }
    return(0); 
}

/***********************************************************************/
