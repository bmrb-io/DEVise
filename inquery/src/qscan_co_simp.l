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
  *  QSCAN_CO_SIMP.L is a simple company query trasformer.  It replaces
  *  occurence of "company" in the query with "#syn(company #company)"
  */

/*
 *  Id string
 */
#define SCANNER_NAME qscan_co_simp           /* Set for use with lexmacs.h */


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

static void report_company(void);

static char temp[100];
static YY_CHAR *p;

%}

token		[A-Za-z0-9#]+
paren           [()]

%% 

<<EOF>>		{
		 yyterminate();
                }

COMPAN(Y|(IES))			|
(C|c)ompan(y|(ies))		{
                                   report_company(); 
                                }

{paren}         ECHO;

{token}    	ECHO;
   
.		ECHO;

\n		ECHO;

 
%%


static void report_company(void)
{
    qecho("#SYN ( company #company )");
    save_transformation(yytext,"#SYN ( company #company )");
}


int
qscan_co_simp_wrap(void)
{
   return 1;
}
