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
   5/12/93 - These functions are used in summary.c/h.  The main purpose
      AEG    is to format one screen of the summary at a time, as well
             as make ui.c more readable.

............................................................................ */

#ifndef _UI_H
#define _UI_H

static Char_t ui_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#define BEEP      printf("\007")
#define CLEAR     printf("\033[2J\033[H")
/* #define CLEAR     sleep(2); printf("\033[2J\033[H") */
#define MAXINDENT (40)		/* maximum indent on any one line */

extern Int_t maxdocs;
extern Char_t msg_str[MAXLEN];

extern Char_t   *chopline(Char_t *s, Int_t maxlen, Int_t do_space); 
extern Int_t     screen_width(void);
extern Int_t     screen_length(void);
extern Char_t   *highlight(Char_t *instr, Int_t width);


#endif
/* Nothing below this line. */
