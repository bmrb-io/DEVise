
 /*
  *  SCAN_QRY.L is a scanner specification for the LEX lexical
  *  analyzer generator.  The scanner specified in this file converts
  *  a query format to a stream of tokens suitable for a parser.
  *
  *  Note: rules which expect a \n are never fired since the
  *  query input getter changes \n to space.  This behavior
  *  should be adjusted, but it might break a lot of functions
  *  that have never been tested.
  */

%{
#include <string.h>
#include <ctype.h>
#ifdef _MSC_VER
  #include <io.h>
#endif /* _MSC_VER */

#include "doctypes.h"
#include "in_node.h"
#include "dbinfo.h"
#include "query.y.h"
#include "docmem.h"
#include "text-utils.h"
#include "qscan.h"

/*
 *  Macros and Constants
 */
#define SCANNER_NAME scan_qry

/* copy yytext to UChar_t 
 * This can change when scanners are all UChar_t
 */

#define yycopy Ustrccpy 
#define MAKE_YYLVAL_TEXT  do { yylval.text = (char *)IN_malloc ((yyleng+1) * sizeof (UChar_t));\
			    yycopy((UChar_t *)yylval.text, (char *)yytext);}\
			while (0)

#define ON   1
#define OFF  0

/* PAREN OPERATIONS:
 * return_nesting_op(OP) starts a new paren counter and then returns OP
 * NL_LPAREN :   called when a left paren is seen.  If paren_stack is 
 *		empty, we're not inside an operator so this is a 
 *              Natural Language paren.  If paren_stack is not empty, 
 *		but PARENLEVEL is zero, then the paren we're seeing 
 *		is the operator's left paren; if the PARENLEVEL is > 0, 
 *		then again it's a natural language paren we're looking at.
 *              RESULT: returns 1 if this is a Natural Language left paren.
 *              SIDE EFFECT: increments PARENLEVEL
 * NL_RPAREN;   see above.
 *              RESULT: returns 1 if this is a Natural Language right paren.
 *		SIDE EFFECT: decrements PARENLEVEL
 */
#define return_nesting_op(op) do {push_paren_stack();return(op);} while(0)
#define NL_LPAREN (paren_stack_empty() ? 1 : (PARENLEVEL++ > 0))
#define NL_RPAREN (paren_stack_empty() ? 1 : (--PARENLEVEL > 0))

/*
 *  Module variables:
 */
static char  rcsid_l[]="$RCSfile$, $Revision$, $Date$";




/* this was previously automatic in several functions.  A pointer
 * to it was being returned, i.e., a pointer to somewhere in 
 * recently freed function stack space.  Moved out to file scope
 * for the short term.  The functions should be rewritten to 
 * take (dst,src) arguments like c library functions.
 * They will be then faster and safer.
 */
static  UChar_t tmp[128]; 

static   UChar_t  *clip_trailing_dsa (UChar_t *string, Int_t length);
static   UChar_t  *unhyphenate (UChar_t *dst, UChar_t *src);
			    
static Int_t    i, rangeFlag=0;

extern   Int_t    esFlag, hyphenFlag, literalFlag;
extern   Boolean_t resetHyphen;

static UChar_t *unslashify_parens(UChar_t *dst, UChar_t *src);

  
%}

%start LIT

att     ([Aa][Tt][ ]?&[ ]?[Tt]|ATT)

alpha   ([a-zA-Z\300-\326\330-\366\370-\377])
num     ([0-9])
alphanum ({alpha}|{num})

w1      ({alpha}{alphanum}*)
w2      ({alpha}+(\'{alpha}*)*)
w3      ({num}+{alpha}{alphanum}*)

s       ({num}+\.{alphanum}+(\.{alphanum}+)+)
a       (({alpha}+\.)+({alpha}+\.?)) 
h       ({alpha}+\-\n{alphanum}+)
ch_char ([\200-\377][\001-\377])
     
wordobj	({w1}|{w2}|{w3}|{s}|{a}|{ch_char}+)

es1     (["']?[;:\.\?\!][ \n\t]+)
es2     ([;:\.\?\!]["']?[ \n\t]+)
es      ({es1}|{es2})

n1      ({num}+(\.{num}+)?)
n2      ({num}{1,3}(\,{num}{num}{num})+(\.{num}+)?)
n3	(\.{num}+)
numobj	({n1}|{n2}|{n3})

%%

<LIT>[^ \n\t()!]*\\"("[^ \n\t()]*(\\")"[^ \n\t)(]*)? { 
                          /* escaped literal parens don't count */
			MAKE_YYLVAL_TEXT;
			yylval.text=unslashify_parens(yylval.text,yylval.text); 
			return(WORD);}

<LIT>[^ \n\t()!]*\\")"[^ \n\t()]* {/* escaped literal parens don't count */
				MAKE_YYLVAL_TEXT; 
				yylval.text=unslashify_parens(yylval.text,yylval.text); 
				return(WORD);}


<LIT>[^ \n\t()]*"("[^ \n\t)(]*(")"[^ \n\t()]*)?  { /* words and parens jammed together */
                          if (NL_LPAREN)
 			  {
			    MAKE_YYLVAL_TEXT;
                            if ( strchr(yytext,')') ) (void) NL_RPAREN;
		     	    return(WORD);
			   }
			  else if ( *yytext != '(' )
                           { /* garbage between #LIT and opening paren
			      * let it cause a syntax error
                              */
                             MAKE_YYLVAL_TEXT; 
                             return(WORD);
			    }
			  else  /* reprocess alphanums, if any 
				* and let next rule handle closing paren.
				*/
                             {  yyless(1);  
				return(LEFT_PAREN);
                             } 
			}

<LIT>[^ \n\t)(]+")"     { /* NL paren? */ /* words and parens jammed together */
 		           if (NL_RPAREN)
 			  {
			    MAKE_YYLVAL_TEXT;
			    return(WORD);
			   }
			   else
                          { /* end of literal, process word */
			    if (yyleng > 1)
			    { /* we must return the literal WD w/o rparen */
				MAKE_YYLVAL_TEXT;	
				yylval.text[yyleng-1]='\0';
                                yyless(yyleng-1);
				return(WORD);
			     }
			  }}


<LIT>[ \t\n]*")"	    {pop_paren_stack();
                            literalFlag = OFF;
                            BEGIN 0;
                            return(RIGHT_PAREN);
			    }

<LIT>{numobj}           { /* treat number in literal mode as a word */
			    MAKE_YYLVAL_TEXT;
                            return (WORD);
                        }

<LIT>[ \t\n]  		{; /* delimiters */ }

	 
<LIT>[^ \n\t)(]+        { MAKE_YYLVAL_TEXT;   
			  return(WORD); }

\\"("                   |
\\")"			{; /* ignore escaped parens */}

\(			{if (! NL_LPAREN)
                            return (LEFT_PAREN);
                          /* else just erase it */ }

\)			{ /* reset the range flag at end paren */
                          if (rangeFlag)  rangeFlag = 0;
			  if (! NL_RPAREN)
                          { pop_paren_stack();
                            return (RIGHT_PAREN);
                          }
		 	  /* else just erase it */
                        }

#[Ee][Qq]      |
#==                    { /* equals operator */
			  UChar_t * p;

			  MAKE_YYLVAL_TEXT;
                          for (p=yylval.text; *p; p++) *p=tolower(*p);
                          return (EQ_RELOP);
                        }

#!=            |
#[Nn][Ee][Qq]? |
#[Ll][Tt][Ee]? |
#\<=?          |
#[Gg][Tt][Ee]? |
#\>=?                   { /* relational operators */
			  UChar_t * p;

			  MAKE_YYLVAL_TEXT;
                          for (p=yylval.text; *p; p++) *p=tolower(*p);
                          return (RELOP);
                        }

!c!(.*)\n                 { /* Ignore comments */ }

#\+			{return (WGTplus);}

#\-			{return (WGTminus);}

#AND			|
#(A|a)nd		{ return_nesting_op (AND); }
#BAND			|
#(B|b)and		{ return_nesting_op (BOOLEAN_AND); }
#BANDNOT		|
#(B|b)(A|a)nd(n|N)ot	{ return_nesting_op (BOOLEAN_AND_NOT); }
#FILTER_REQUIRE         |
#(F|f)ilter_(R|r)equire |
#FILREQ                   |
#(F|f)il(R|r)eq		{ return_nesting_op (FILTER_REQUIRE); }
#FILTER_REJECT          |
#(F|f)ilter_(R|r)eject  |
#FILREJ                   |	 
#(F|f)il(R|r)ej		{ return_nesting_op (FILTER_REJECT); }

#CITY			|
#(C|c)ity		|
#COMPANY		|
#(C|c)ompany		| 
#FOREIGNCOUNTRY		|
#(F|f)oreigncountry	| 
#LOCATION		|
#(L|l)ocation		|
\*[A-Za-z0-9\_\-]+      { yylval.text = (char*)IN_malloc ((yyleng+1)* sizeof (UChar_t));
			  /* User keyword/generics are prefixed with a 
			   * "*".   "#" is old-style, retained for 
			   * compatibility.
			   */
                          yycopy((UChar_t*)yylval.text, (char*)yytext);
			  return (WORD); }

#FIELD                  |
#(F|f)ield              { /* field operator */
                          return_nesting_op (FIELD_OP);
                        }

#LIT                    |
#(L|l)it                { /* literal operator */
                          BEGIN LIT;
                          push_paren_stack();
                          literalFlag = ON;
                          return(LITERAL);
                        }

#MAX			|
#(M|m)ax		{ return_nesting_op (MAX_OP); }

#FMAX			|
#(F|f)max		{ return (MAX_RELOP); }

#FMIN			|
#(F|f)min		{ return (MIN_RELOP); }

#RANGE                  |
#(R|r)ange              |
#\<\>                   { /* Range relop */
                          rangeFlag = 1;
                          return (RANGE_RELOP);
                         }

#NOT			|
#(N|n)ot		{ return_nesting_op (NOT); }

#[0-9]+			{ yylval.text = (char*)IN_malloc ((yyleng+1)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+1);
			  return_nesting_op (ODn);
			}

#OD[0-9]+		|
#(O|o)d[0-9]+		{ yylval.text = (char*)IN_malloc ((yyleng+4)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+3);
			  return_nesting_op (ODn);
			}

#OR			|
#(O|o)r			{ return_nesting_op (OR); }

#PARSUM[0-9]+		|
#(P|p)arsum[0-9]+	{ yylval.text = (char*)IN_malloc ((yyleng+4)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+7);
			  return_nesting_op (PARSUMn);
			}

#PASSAGE[0-9]+		|
#(P|p)assage[0-9]+	{ yylval.text = (char*)IN_malloc ((yyleng+4)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+8);
			  return_nesting_op (PARSUMn);
			}

#PHRASE			|
#(P|p)hrase		{ return_nesting_op (PHRASE); }

#PHRASEB		|
#(P|p)hraseb		{ return_nesting_op (PHRASEB); }

#SUM			|
#(S|s)um		{ return_nesting_op (SUM); }

#SYN			|
#(S|s)yn		{ return_nesting_op (SYN); }

#USA			|
#(U|u)sa		{ yylval.text = (char*)IN_malloc ((yyleng+1)* sizeof (UChar_t));
                          yycopy((UChar_t*)yylval.text, (char*)yytext);
			  return (WORD); }

#UW[0-9]+		|
#(U|u)w[0-9]+		{ yylval.text = (char*)IN_malloc ((yyleng+4)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+3);
			  return_nesting_op (UWn);
			}

#WPARSUM[0-9]+		|
#(W|w)parsum[0-9]+	{ yylval.text = (char *)IN_malloc ((yyleng+4)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+8);
			  return_nesting_op (WPARSUMn);
                        }

#WPASSAGE[0-9]+		|
#(W|w)passage[0-9]+	{ yylval.text = (char *)IN_malloc ((yyleng+4)* sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,(char*)yytext+9);
			  return_nesting_op (WPARSUMn);
                        }

#WSUM			|
#(W|w)sum		{ return_nesting_op (WSUM); }

{h}                     { /* unhyphenate capitalized word when needed */
                          resetHyphen=ON;
			  MAKE_YYLVAL_TEXT;
 		          (void)unhyphenate (yylval.text,yylval.text);
                          /* if range flag is set, return range token,
                             otherwise a word */
                          if (rangeFlag) 
                            return (RANGE_LIMIT);
                          else
                            return (WORD);                          
                        }

{es}                    { /* end of sentence token */
                          resetHyphen=ON;
			  esFlag=ON;
                        }

{wordobj}[-/&]        { /* Dashed, slashed, and ampersanded words are processed
                           * in parts, even if stopwords.  
                           */
                          hyphenFlag=ON;
                          resetHyphen = OFF;

                          yylval.text = (char*)IN_malloc ((yyleng+1)*
							  sizeof (UChar_t));
                          yycopy ((UChar_t*)yylval.text,
				  (char *)clip_trailing_dsa ((UChar_t *)yytext,
							     yyleng));
                          return (WORD);
                        }

{numobj}		{ resetHyphen=ON;
			  MAKE_YYLVAL_TEXT;
                          /* if range flag is set, return range token,
                             otherwise a word */
                          if (rangeFlag) 
                            return (RANGE_LIMIT);
                          else
 			    return (NUMBER);
			}

{att}		        { /* special mini at&t recognizer */
                          resetHyphen=ON;
 			  yylval.text =
			    (char *)IN_malloc(sizeof((UChar_t *)"at&t"));
 			  yycopy((UChar_t *)yylval.text,   "at&t");
			  return (WORD); 
                        }
   
{wordobj}		{ /* standard old word object */
                          resetHyphen=ON;
			  MAKE_YYLVAL_TEXT;

                          /* if range flag is set, return range token,
                             otherwise a word */
                          if (rangeFlag) 
                            return (RANGE_LIMIT);
                          else
                            return (WORD);
                        }

[ \n\t]			{ /* Ignore whitesspace */
                          resetHyphen=ON;
                        }

.			{ /* Ignore unexpected stuff */ 
                          resetHyphen=ON;
                        }

%%


/*
 *  SCAN_QRY_YYTEXT is an external interface to the yytext variable.  
 */
 char *scan_qry_yytext ()
  {

  return ((char*)yytext);

  } /* end of scan_query_yytext */


/*
 * RESTART_FLEX allows the flex scanner to be restart from an
 * external call.  If we're using lex, this should be empty
 */
int
restart_flex(void)
{
 BEGIN 0;
 literalFlag = OFF;

#ifdef FLEX_SCANNER
    yyrestart(stdin);
#endif
    /* this is a clean hack for a global that is set in 
     * a subprogram and read in a calling program and never
     * initialized.  There may be other such flags.
     */
    return 1;
}


/*  
 * clip_trailing_dsa : remove trailing dashes, slashes, or ampersands from words.
 */
static UChar_t *clip_trailing_dsa (UChar_t *string, Int_t length) {
  register int  i;

  strcpy ((char *)&tmp[0], (char *)string);

  /* check that the string really does end with -/& and process */
  if (tmp[length-1] == '-') {
    for (i=length-2; i>=0 && tmp[i]==' '; i--);
  }    
  else if (tmp[length-1] == '/') {
    for (i=length-2; i>=0 && tmp[i]==' '; i--);
  }
  else if (tmp[length-1] == '&') {
    for (i=length-2; i>=0 && tmp[i]==' '; i--);
  }  
  else 
    /* doesn't have trailing -/& chars, so return original */
    return((UChar_t *)string);

  /* return clipped string */
  tmp[i+1] = '\0';
  return ((UChar_t *)&tmp[0]);
}  /* end of clip_trailing-/& */

/*  
 * unhyphenate - remove the hyphen and newline from a hypenated word occurring
 * at end of sentence.
 */
static UChar_t *unhyphenate(UChar_t *dst, UChar_t *src)
{
   UChar_t *p,*q;

   for(p=src,q=dst; *p; p++,q++)
      if ((*p != '-') && (*p != '\n'))
	 *q = *p;

   *q='\0';
   return(dst);
}  /* end of unhyphenate */  

static UChar_t *unslashify_parens(UChar_t *dst, UChar_t *src)
{
	
   UChar_t *p,*q;

   for(p=src,q=dst; *p; p++,q++)
   {
      if ((*p == '\\') 
	&&( (p[1] == '(' ) 
            || (p[1] == ')' ) ))
         p++;
       *q = *p;
    }
   *q='\0';
   return(dst);
}

      
int
scan_qry_wrap(void)
{
   return 1;
}
