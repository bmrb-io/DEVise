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


#ifndef SPEC_SCAN_H
#define SPEC_SCAN_H

/*  
 *   This is the public header file for the spec_scan module
 */

typedef struct ScanInfo ScanInfo_t;

#define SR_ON      1
#define SR_OFF     2
#define SR_DEFAULT 3

#define SR_INITIALIZE 0
#define SR_TERMINATE  1

/* Allocated new table */
Int_t sr_initialize_table(ScanInfo_t **spec_scanner);

/* Frees the table allocated by sr_initialize() */
Int_t sr_free_table(ScanInfo_t *spec_scanner);

/* Sets all the scanners to: SR_ON, SR_OFF, or SR_DEFAULT */
Int_t sr_set_all(ScanInfo_t *spec_scanner, Int_t value);

/* Get a string containing a list of active recognizers (space separated) */
Int_t sr_get_active_recog_list(ScanInfo_t *spec_scanner, Char_t **active);

/* Free the string alloced by sr_get_active_recog_list */
Int_t sr_free_active_recog_list(Char_t *active);

/* Set the named scanner to: SR_ON, SR_OFF, or SR_DEFAULT */
Int_t sr_set_scanner(ScanInfo_t *spec_scanner, Char_t *name, Int_t value);

#endif
