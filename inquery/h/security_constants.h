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


#if !defined(SECURITY_CONSTANTS_H)
#define SECURITY_CONSTANTS_H

/* #define values for security */

#define        SECURITY_OFF 0
#define        SECURITY_ON  1

#define        MAX_NAME 100
#define        MAX_GROUPS 100
#define        MAX_ACL_USERS 10000
#define        MAX_RECORD_SIZE ((MAX_NAME)*2)


#define        INQ_NONE 0
#define        INQ_READ 1
#define        INQ_WRITE 2
#define        INQ_FULL 4
#define        INQ_DENY 8

#define        INQ_DATABASE 1
#define        INQ_DOCUMENT 2
#define        INQ_FIELD 4

#define        SA_PASS_DEFAULT "superpass"
#define        INQ_SYS_FILE "sysusers.dat"

#endif
