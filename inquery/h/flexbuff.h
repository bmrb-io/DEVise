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
 * "FLEXBUFF_H" is a header file 
 */

/*
 *  RCS identifier for header:
 */
static char flexbuff_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#if !defined(FLEXBUFF)
#define FLEXBUFF

/********************/
/* MODULE VARIABLES */
/********************/

static YY_BUFFER_STATE new_buff = (YY_BUFFER_STATE)NULL;
static YY_BUFFER_STATE old_buff;
static char save_1;
static char save_2;

/*********************/
/* PRIVATE FUNCTIONS */
/*********************/

static void fill_flex_buff(Char_t *buff, Int_t num_in_buff);
static void flush_flex_buff(void);

/**********/
/* MACROS */
/**********/

#define YY_INPUT(buf, result, max_size) \
{\
   old_buff = YY_CURRENT_BUFFER;\
   yy_switch_to_buffer(new_buff);\
\
   if(old_buff)\
	free(old_buff);\
    result=new_buff->yy_buf_size;\
}

#endif /* !defined FLEXBUFF */
