/**************************************************************************
*                         Copyright (c) 1996                              *
* by Sovereign Hill Software, Inc., Amherst, Massachusetts, U.S.A.        *
*        (413-256-6916;   http://www.sovereign-hill.com)                  *
*                          All Rights Reserved.                           *
*                                                                         *
* The SOFTWARE was developed in part at the Center for Intelligent        *
* Information Retrieval (CIIR) at the University of Massachusetts at      *
* Amherst (For more information, contact 413-545-0463 or                  *
* http://ciir.cs.umass.edu).  The SOFTWARE was acquired by Sovereign      *
* Hill Software Inc. and is the property of Sovereign Hill Software.      *
* ACSIOM is authorized, under an agreement with Sovereign Hill Software,  *
* to license the SOFTWARE for non-commercial, non-revenue-producing       *
* prototyping and internal evaluation purposes only.                      *
*                                                                         *
* This SOFTWARE was developed in significant part with private funding.   *
* The INQUERY SOFTWARE is proprietary to Sovereign Hill Software and may  *
* not be distributed in whole or in part or used in any way without       *
* authorized permission in writing from Sovereign Hill Software.          *
* Possession, use, duplication or dissemination of the INQUERY SOFTWARE   *
* and media is authorized only pursuant to a valid written license from   *
* ACSIOM or Sovereign Hill Software, Inc.                                 *
*                                                                         *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.  *
* Government is subject to restrictions as set forth in subparagraph      *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.              *
**************************************************************************/ 


/*
 * "AFFIX_H" is a header file to hides some of the ugliness of cpp macro 
 * string handling.
 * 
 * To concatenate SCANNER_NAME to yylex:
 *   AFFIX(SCANNER_NAME,yylex) 
 * will do it.  Where SCANNER_NAME is #defined as scan_company
 * you get scan_company_yylex.
 * To get init_scan_company_info, use
 *   INFIX(init,SCANNER_NAME,info)
 */

#if !defined(AFFIX_H)
#define AFFIX_H

/*
 *  RCS identifier for header:
 */
static char affix_h_rcsid[]="$RCSfile$, $Revision$, $Date$";


/* These expanders (there could be an EXPAND3...EXPANDN)
 * ensure that the macro arguments are thoroughly expanded and 
 * resolved before the final operation is done.  This is necessary
 * because the preprocessor has a peculiar evaluation order,
 * so you can't do very much with an argument given only
 * one level of macro call.
 *
 * You can use the EXPANDi functions to roll your own arg-evaluating
 * macros
 */

#define EXPAND1(macro,var1) macro(var1)
#define EXPAND2(macro,var1,var2) macro(var1,var2)

/* Here is where you put the macros that do the real work
 * after their arguments have been "EXPANDed"
 */

#define _NAMED_VAR(name,affix) name##_##affix
#define _STRINGIFY(x) #x

/* These are the "PUBLIC" macros that users actually use
 */
  
/* turn X into "X" */
#define DBL_QUOTE(x) EXPAND1(_STRINGIFY,x)

/* turn PREFIX, AFFIX into PREFIX_AFFIX */
#define AFFIX(prefix,affix) EXPAND2(_NAMED_VAR,prefix,affix)

/* turn PREFIX, STEM, AFFIX into PREFIX_STEM_AFFIX */
#define INFIX(prefix,stem,affix) EXPAND2(_NAMED_VAR,EXPAND2(_NAMED_VAR,prefix,stem),affix)






#endif
