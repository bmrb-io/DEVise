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
 * JAPANESE_H is a header file to for the japanese portion of the
 * inquery interface.
 */

#if !defined(JAPANESE_H)
#define JAPANESE_H

static char japanese_h_rcsid[]="$Id$";

#define  MAXQRYWORDS  (300)
#define  MAXLEN (256)

#define KANJI_1    '\xb0'
#define KATAKANA_1 '\xa5'
#define ALPHNUM_1  '\xa3'
#define KIGOU1_1   '\xa1'
#define CHOUON_2   '\xbc'
#define KANSPACE   '\xa1'

/* Types */

struct jq_opt
{
    int        jq_indexing;	/* 1:Kanji-Char/Kana-Word  2:Word-based(JUMAN)
				   3:char/word-mixed */
    int        jq_method; /* 0: Manual, 1: NLQ, 2:SHORT, 3: LONG, 4: JOINED */
    int        jq_method1;
    int        jq_method2;
    char       jq_operator[20];
    char       jq_operator1[20];
    char       jq_operator2[20];
    int        jq_fork;
};

/*    int juman_load_flg;
    int jq_method;
    int i_nat;
    int i_short;
    int i_long;
    int i_joined;           
*/

#endif
