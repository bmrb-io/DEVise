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


#if !defined(TEXT_UTILS_H)
  #define TEXT_UTILS_H

static char text_utils_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

  /*
   *  text-utils.h:  Useful functions on pieces of text.
   *
   *  Created:  11/19/92
   */

  UChar_t *Ustrccpy (UChar_t *dest, const char *src);

  Char_t **add_to_string_list(Char_t **strings, Int_t *num, Char_t *new);

  void strcpy_lowercase (UChar_t *target, UChar_t *source);

/* These SLIST macros and the slist_t data structure provide a convenient
 * way of parsing a list of elements out of a simple character array.  The
 * characters that separate the list are defined by the caller.  The array
 * being traversed is not touched.  The destination string for elements must
 * allocated by the caller (it'll at least be no longer than the original).
 * Leading and trailing whitespace is stripped off the elements.  When no
 * elements are left, the destination will be set to a null string.
 *
 * An example,
 *
 *   char *list = "A; B,, C";
 *   char dest[10];
 *   slist_t s;
 *
 *   SLIST_LOAD(s, list, ",;");
 *   while(1) {
 *     SLIST_GET_NEXT(s,dest);
 *     if (!*dest) break;
 *     printf("   > %s\n",dest);
 *   }
 *
 * yeilds:
 *    > A
 *    > B
 *    > C
 */
struct slist {
  char *base;
  char *cur;
  char *separators;
};
typedef struct slist slist_t;

#define SLIST_LOAD(s,l,seps) (s).base = (s).cur = l; \
                             (s).separators = (seps) ? (seps) : ","
#define SLIST_RESET(s) (s).cur = (s).base
#define SLIST_GET_NEXT(s,d) if ((s).cur) { char *p; int len; \
          for(;isspace(*((s).cur)) || strchr((s).separators,*((s).cur));(s).cur++); \
          p = strpbrk((s).cur, (s).separators); \
	  len = p ? p - (s).cur : strlen((s).cur); \
          strncpy(d, (s).cur, len); \
	  d[len] = '\0'; \
          (s).cur = p; \
	  p = strchr(d,'\0'); for(p--;isspace(*p);p--) *p = '\0'; \
          } else d[0] = '\0';


#endif
