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


#if !defined(LANGUAGE_H)
#define LANGUAGE_H

#include "doctypes.h"

static char language_h_rcsid[]="$RCSfile$, $Revision$, $Date $";

enum language {ENGLISH, SPANISH, JAPANESE, CHINESE, LANGUAGE_UNKNOWN};
typedef enum language language_t;
#define LANGUAGE_DEFAULT ENGLISH

enum encoding {LATIN_1, EUC, BIG5, GB, STC, ENCODING_UNKNOWN};
typedef enum encoding encoding_t;
#define ENCODING_DEFAULT LATIN_1


struct lang_encoding {
  language_t lang;
  encoding_t encoding;
};
typedef struct lang_encoding lang_encoding_t;

struct lang_init {
  language_t lang;
  int (*initializer)(void);
};
typedef struct lang_init lang_init_t;

extern lang_init_t language_initializers[];

const char *language_name(language_t language);
language_t  language(char *name);
const char *language_encoding_name(encoding_t encoding);
encoding_t  language_encoding(char *name);
language_t  encoding2language(encoding_t encoding);
int         language_initialize(lang_encoding_t lang, lang_init_t init[]);

/* extern language_t language; */ 

#define is_widechar_language(lang) (((lang)==JAPANESE)|| ((lang)== CHINESE))
#define is_widechar(char) ((UChar_t) (char) & (UChar_t)'\200')
#define ch_char(char) ((UChar_t) (char) & (UChar_t)'\200')
#define japan_char(char) ((UChar_t) (char) & (UChar_t)'\200')
#define eng_char(char) (char &&( ((UChar_t)(char) & (UChar_t)'\200') ^ (UChar_t)'\200'))

#endif /* LANGUAGE_H */
