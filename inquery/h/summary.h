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

/* ............................................................................
module name: ui.c     user interface routines for inquery

Modifications:  
   5/12/93 - This file defines all of the structures used to do 
     AEG     the summary functions, which are contained in summary.c.
             The purpose is to format one screen at a time and make ui.c
	     more readable.
............................................................................ */

#ifndef _SUMMARY_H
#define _SUMMARY_H

static Char_t summary_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

typedef struct _sum_ent {
  Char_t **text;		/* list of pointers to the text */
  Int_t num_lines;		/* number of lines per entry */
} SUM_ENT;

typedef struct {
  SUM_ENT **entries;		/* an array of pointers to summary entries */
  Char_t *query;		/* beginning of query for header line. */
  Int_t   num_ent;		/* number of entries */
} SUMMARY;

typedef struct {
  Int_t entry;			/* pointer to a list of summary entry */
				/* pointers. */
  Char_t  **line;		/* pointer to a list of string pointers. */
} cur_sum_ent;

typedef enum {UP, DOWN} direction;

extern cur_sum_ent adjust_cur_sum_ent(SUMMARY *cursum, cur_sum_ent current, 
				      direction dir, Int_t lines);
extern cur_sum_ent draw_sum_entries(SUMMARY *cursum, cur_sum_ent current, 
				    Int_t lines);
extern SUM_ENT *add_to_sum_ent(SUM_ENT *cur, Char_t *text);
extern SUMMARY *add_to_summary(SUMMARY *cursum, SUM_ENT *new_entry,
			       Char_t *text);
extern void release_summary(SUMMARY **current);
extern cur_sum_ent draw_summary(SUMMARY *cursum, Char_t *topline, 
				Int_t width, cur_sum_ent current);
extern SUMMARY *set_summary(dbinfo *db, Char_t *query, belieflst *bl,
			    SUMMARY *cursum);

#endif
/* Nothing below this line. */
