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


#if !defined(INCALC_H)
#define INCALC_H

static char incalc_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#include "in_node.h"
#include "dbinfo.h"

int set_incalc_db(dbinfo *db);
void tf_defaults (float default1,float default2);
float tf_score(Int_t term_freq, Int_t max_freq, BoolInt_t tf_penalty);
float tf_score_max(Int_t term_freq, Int_t max_freq, BoolInt_t tf_penalty);

void belief_defaults(float baseline, float factor, Int_t doc_cnt);
float belief_score(float tf, float idf);
float default_belief(void);
float idf_score(Int_t term_freq);
float mim_score (Int_t collect_freq_x, Int_t collect_freq_y, Int_t collect_freq_xy);
void node_mim_score (IN_node_type *node);

#endif
