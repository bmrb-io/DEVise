/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/
%{

 /*
  *  QSCAN_LOC_SIMP.L is a more simple location query trasformer.  It
  *  recognizes various forms of USA, USSR, UN, and England and replaces
  *  them with an appropriate #syn.  It also replaces "city" "location"
  *  and "country" with #city or #foreigncountry.
  */

/* updated 10/25/92 for lexmacs header  */
/*
 *  Id string
 */
#define SCANNER_NAME qscan_loc_simp           /* Set for use with lexmacs.h */


static char rcsid[]="$RCSfile$, $Revision$, $Date$";


#include <string.h>				/* String functions */
#include <ctype.h>
#ifdef _MSC_VER
  #include <io.h>
#endif /* _MSC_VER */
#include "doctypes.h"
#include "docdef.h"
#include "inerror.h"

#ifdef SOLO
#undef YY_DECL
#define YY_DECL int main (void)
#define qecho(text) Q_inform(" %s ",text)
#define save_transformer(orig,xformed) while(0)
#else
#include "qscan.h"
#endif

static void report_usa(void);
static void report_other(char *string);
static void report_special_country(char *affix);
static void report_UN(void);

static Boolean_t city_printed=0;
static Boolean_t country_printed=0;
static char temp[100];

#define USSRSYN "russia ussr u.s.s.r. soviet"
#define UKSYN "britain #1(united kingdom) u.k. uk england"

%}

%Start COUNTRY USA 

ws	[ \n]+
dot  \.[ \n]*|[ \n]*

ussr	(U{dot}S{dot}S{dot}R{dot})|Russian?|Soviet({ws}Union)?|Union{ws}of{ws}Soviet{ws}Socialist{ws}Republics
   
UK  England|Britain|(Great{ws})?Britain|U{dot}K{dot}|([Tt]he{ws})?United{ws}Kingdom

of		(Of|of)
united		(United|(U\.?))
states		(States|(S\.?))
america		(America|(A\.?))

usa1		{united}{ws}?{states}(({ws}{of})?{ws}?{america})?
usa2		u[. \/\n]+s([. \/\n]+(a[). \/\n]+)?|\))|(usa)
usa		{usa1}|{usa2}

token		[A-Za-z0-9#]+
paren           [()]

%% 

<<EOF>>		{
		 city_printed=0;
		 country_printed=0;
		 yyterminate();
                }

CIT(Y|IES)			|
[Cc]it(y|ies)			|
TOWN				|
[Tt]own				|
LOCATION(S)?			|
(L|l)ocation(s)?		{ if (!city_printed) 
                                  { city_printed=1;
                                    report_other("#city");
                                   }
                                }

COUNTR(Y|IES)         |
[Cc]ountr(y|ies)      {   if (!country_printed) 
                          { country_printed=1;
                            report_other("#foreigncountry");
                          }
                      }

U{dot}N{dot}	|
United[ \n]Nations	{ report_UN();}


{UK}		{
		 report_special_country(UKSYN);
	         if (isspace(yytext[yyleng-1]))
		      yyless(yyleng-1);

		}

{ussr}		{
		 report_special_country(USSRSYN);
		if (isspace(yytext[yyleng-1]))
		      yyless(yyleng-1);
		}

{usa}		{ report_usa();}

{paren}         ECHO;

{token}         ECHO;
   
.		ECHO;

\n		ECHO;

 
%%


static void report_special_country(char *str)
{
      sprintf(temp," #SYN (%s) ", str);
      qecho(temp);
      save_transformation(yytext,temp);
}
	
static void report_usa(void)
{
   qecho("#USA");
   save_transformation(yytext,"#USA");
}

static void report_UN(void)
{
   qecho(" #SYN ( u.n. #1( u n ) #1(united nations))");
   save_transformation(yytext,"#SYN ( u.n. #1( u n ) #1(united nations))");
}

static void report_other(char *string)
{
    qecho(string);
    save_transformation(yytext,string);
}


int
qscan_loc_simp_wrap(void)
{
   return 1;
}
