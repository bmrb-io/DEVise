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


#if !defined(INQ_ERR_H)
#define INQ_ERR_H

#include <stdio.h>
#include <stdlib.h>

/* INQ_ERR.H
 *
 * This file defines the error codes used in INQUERY and returned my
 * many of the MPIs (Module Programmer Interfaces).
 *
 * In addition, it defines the returned status_t returned by INQUERY
 *   APIs.
 */

/*-----------------------------------*
 *  INQUERY Error Codes
 *----------------------------------------------------------------------*/

#define INQ_SUCCESS       0
#define INQ_FAILED        1
#define INQ_ARG_ERR       2
#define INQ_FILE_ERR      3
#define INQ_ALLOC_ERR     4
#define INQ_NOT_AVAIL     5
#define INQ_SYNTAX_ERR    6
#define INQ_END_NOT_FOUND 7
#define INQ_VERS_ERR      8
#define INQ_MISMATCH      9
#define INQ_NO_FILE      10
#define INQ_NOT_QBS_DB   11
#define INQ_PRE_EXIST    12

/* Possible return values for cancelation and feedback routines -- RCC 1/6/96*/
#define INQ_PROCEED                 900
#define INQ_CANCEL                  901
#define INQ_INVALID_RETURN_STATUS   999
#define INQ_NET_ERROR              -998

/*-----------------------------------*
 *  Multi-Database Error Codes
 *----------------------------------------------------------------------*/
#define INQ_MDB_ERROR         -1
#define INQ_MDB_NULL_PARAM    10
#define INQ_MDB_INVALID_PARAM 11
#define INQ_MDB_MALLOC        12
#define INQ_MDB_OUT_OF_RANGE  13
#define INQ_MDB_NOT_FOUND     14
#define INQ_MDB_CONN_ERROR    15
#define INQ_MDB_INVALID_HANDLE 16
#define INQ_MDB_NO_ACTIVE_DBS  17
#define INQ_MDB_SUCCESS        0

#endif  /* INQ_ERR_H */

