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
 * "DM_PUB_H" is a header file for the public types and functions 
 * of the Document Manager.
 */

#if !defined(DM_PUB_H)
#define DM_PUB_H

/*
 *  RCS identifier for header:
 */
static char dm_pub_h_rcsid[]="$RCSfile$, $Revision$, $Date$";


/********************/
/* TYPE DEFINITIONS */
/********************/

struct field_desc
{
    Char_t   *name;
    Char_t   *description;
    char      norm_index;
    char      field_index;
    char      display_text;
    Boolean_t run_recog;
    char      coop_mode;
};

typedef struct field_desc FieldDesc;

struct doc_format
{
    Char_t    *type_name;
    Int_t    (*trans_funct)();
    Int_t      num_field_desc;
    FieldDesc *field_desc;
    Int_t      type_id;
};

typedef struct doc_format DocFormat_t;

/**********/
/* MACROS */
/**********/

/* Document types */
#define INQ_NULL_TYPE -1

/* Display types */
#define INQ_ID     0
#define INQ_SOURCE 1
#define INQ_TEXT   2
#define INQ_TITLE  3

/* Field Coop Mode types */
#define INQ_COOP     0
#define INQ_OVERRIDE 1
#define INQ_SINGLY   2

#define ON  1
#define OFF 0

/* Field Index Mode types */
#define INQ_NONE         0
#define INQ_NORMAL       1
#define INQ_NO_RANGE     2
#define INQ_FLD_NO_LOCS  4


#endif

