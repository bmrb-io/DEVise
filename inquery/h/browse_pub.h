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
 * "BROWSE_PUB_H" is a header file 
 * where the publicly visable Browser MPI is declared.  Only this
 * file should be included above the Browser module.
 */


#ifndef BROWSE_PUB_H
#define BROWSE_PUB_H


/****************/
/* HEADER FILES */
/****************/

#include "doctypes.h"
#include "lines_list.h"


/***********************/
/* FUNCTION PROTOTYPES */
/***********************/

#if (defined(__MWERKS__) && defined(__cplusplus))
extern "C" {
#endif


 /* Create a line for a list */

list_line_t * inq_list_line (char * key, char * detail, int count);

/* Create a lines list */

lines_list_t * inq_make_lines_list (int room);

void inq_free_lines_list (lines_list_t *old);

/* Copy an entire list */

lines_list_t * inq_list_dup (lines_list_t *old);

void inq_free_list_line (list_line_t *old);

/* Add a line to a list */

void
inq_list_add_line (lines_list_t * list, char * key, char * detail, int count);

/* Comparison functions for qsort for sorting lines of a list;
   either keys by strcmp or counts by numeric comparison.
   Note we qsort the array of pointers-to-structs;
   the actual list_line_t structs are NOT an array. */

int list_line_comp_keys (list_line_t **a, list_line_t **b);
int list_line_comp_counts (list_line_t **a, list_line_t **b);

lines_list_t *
merge_lists (lines_list_t ** lists, void * compfun);

lines_list_t *
search_lists (lines_list_t ** lists, char *match, void * compfun);

lines_list_t *
unique_list (lines_list_t * list, void * compfun);

/* Number of lines in list -- access as 0 thru N-1 */

#define Lines_List_Count(ll) (ll->count)

/* Components of each line in list */

#define List_Line_Key(ll,ii) (ll->lines[ii]->key)
#define List_Line_Detail(ll,ii) (ll->lines[ii]->detail)
#define List_Line_Count(ll,ii) (ll->lines[ii]->count)

/* Sugar */

#define inq_free_browse_list inq_free_lines_list
#define inq_free_browse_modes inq_free_lines_list
#define inq_free_read_browse inq_free_lines_list

#define BROWSE_TOP     "most-used"
#define BROWSE_ALPHA   "prefix"
#define BROWSE_PATTERN "substring"
#define BROWSE_TITLES  "titles"
#define BROWSE_QUERY   "query"

/* Mode supporting hierarchy browsing (e.g. MeSH topics).
   For a given entry "expand" will report parent, siblings, children,
   while "explode" will also report more distant generations.
   Since a tag may appear in more than one place in the hierarchy,
   they may be reported by the "places" mode. */

#define BROWSE_EXPAND   "expand"
#define BROWSE_EXPLODE  "explode"
#define BROWSE_PLACES  "places"

Int_t
inq_get_browse_list (dbinfo * db, lines_list_t **list);

Int_t
inq_get_browse_modes (dbinfo * db, Char_t *topic, lines_list_t **modes);

Int_t
inq_read_browse (dbinfo * db, Int_t lines, Char_t *mode, Char_t *search,  Char_t *topic, lines_list_t **window);

#if (defined(__MWERKS__) && defined(__cplusplus))
}
#endif

#endif

