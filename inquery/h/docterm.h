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



#if !defined(DOCTERM_H)
#define DOCTERM_H

/* structures to provide access to the doc-term mapping */

/* #define MAX_TERMS_PER_DOC 300 (* is 173 (doc 2233) in cacm_dots */

static char docterm_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

typedef struct {
  term_id_type term_id;
  int term_freq;
  int max_freq;
 } term_type;

typedef struct {
  term_id_type term_id;
  int term_freq;
 } file_term_type;

/* initialize the docterm file */
BoolInt_t docterm_init(char *file_name);

/* terminate the docterm file */
void docterm_terminate();

/* return a pointer to the correct info */
term_type *docterm(int doc_id,int query_num);

void doctermeter();
/* print statistics on the performance of docterm module */

#endif
