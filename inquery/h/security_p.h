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


/*!**************************************************************************
*!
*!  Module    : security_p.h        (the private .h file for security)
*!  Purpose   : To define the private data structures for the Inquery security
*!              subsystem.  (see also security.h - the public file)
*!  Creator   : Jeff Catlin
*!  Date      : 21 Jan, 1995           (Original)
*!  Updated   :
*!
****************************************************************************/

#if !defined(SECURITY_P_H)
#define SECURITY_P_H

#include "doctypes.h"
#include "docdef.h"
#include "security_constants.h"

#define        SECURITY_OFF 0
#define        SECURITY_ON  1

/**********************************************************
*!          definition of private structures              *
**********************************************************/
typedef Int_t Access_t;
typedef struct sysusers SysUsers_t;

struct sysusers {
  Char_t       name[MAX_NAME];
  Char_t       password[MAX_NAME];
  Char_t       groups[MAX_GROUPS][MAX_NAME];
  Access_t     access;              /* this is system level access only */
  Int_t        numgroups;
  SysUsers_t   *next;
  SysUsers_t   *prev;
};
#define SYSUSERS_T

typedef struct acllist AclList_t;

struct acllist {
  Int_t        accesslevel;         /* database, document, field */
  Access_t     access;
  Char_t       name[MAX_NAME];      /* a user or group name */
  AclList_t    *next;
  AclList_t    *prev;
};
#define ACLLIST_T

/********************************************************************\
*! Semi-public (MPI) functions - Visible to other Inquery code      **
\********************************************************************/

static Int_t security_on = SECURITY_OFF;

#endif
