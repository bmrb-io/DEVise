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
 *  stemmer.h:  Declarations and macros for use with the stemmers.
 *
 *  Created:  February 8, 1993
 */

#if !defined(STEMMER_H)
#define STEMMER_H

static char stemmer_h_rcsid[] = "$RCSfile$, $Revision$, $Date$";

#include <stddef.h>

#define STEMMER_DEFAULT "default"
#define STEMMER_OFF     "off"

typedef void (*stemmer_t)(char *term, char *stem);

struct stem_tab {
  stemmer_t   stemmer;
  const char *name;
};
typedef struct stem_tab stem_tab_t;

/* ACSIOM provided stemmer functions */
void porter_stemmer(char *term, char *stem);
void kstem_stemmer(char *term, char *stem);
void spanish_stemmer(char *term, char *stem);

/* return FALSE if name is not defined in stemmers (see stem_tab.c) or
 * bad arguments, otherwise set stemmer and return TRUE
 */
int stem_name_to_stemmer(const char *stemmer_name, stemmer_t *stemmer);

/* return FALSE if stemmer is not defined in stemmers (see stem_tab.c) or
 * bad arguments, otherwise set stemmer_name and return TRUE
 */
int stem_stemmer_name(stemmer_t stemmer, const char **stemmer_name);

#endif /* STEMMER_H */
