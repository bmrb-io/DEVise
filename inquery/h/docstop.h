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
 * module name:  docstop.h
 *
 * It really would make sense to rename this module to stopword.h!
 *
 * This module contains #defines and declarations needed by stopword code.
 * Functions and variables declared here are defined in stopword.c.
 */

#if !defined(DOCSTOP_H)
#define DOCSTOP_H

static char docstop_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#include "docdef.h"
#include "hashobj.h"

#define max_stop_words 701
#define min_term_lc 1              /* changed from 2 on 7/06/92 - SMH */
#define STOP_ID -2                 /* term id reserved for stopwords */

BoolInt_t stop_word(hashobj *stop_ht, char *b);
void free_stop_words(hashobj *stop_ht);
hashobj *initialize_stop_words(char *file_name);

int read_stopword_dictionary (hashobj **stop_ht, char *fname);
void name_stopword_dictionary (hashobj *stop_ht, char *name);

#endif
