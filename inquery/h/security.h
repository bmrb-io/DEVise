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
*!  Module    : security.h        (the public .h file for security)
*!  Purpose   : To define the API calls and public data structures for the 
*!              Inquery security subsystem.  (see also inq_security_p.h)
*!  Creator   : Jeff Catlin
*!  Date      : 21 Jan, 1995           (Original)
*!  Updated   :
*!
****************************************************************************/

#if !defined SECURITY_H
#define SECURITY_H

#include "clisrv_seckey.h"
#include "security_p.h"

#if !defined SYSUSERS_T
typedef struct {char dummy;} SysUsers_t;
#endif

#if !defined ACLLIST_T
typedef struct {char dummy;} AclList_t;
#endif

/**********************************************************\
*!                Public API functions                     *
\**********************************************************/

/* system level mpi calls */
Int_t SEC_sys_login(SecKey_t **key, Char_t *user, Char_t *passwd);
Int_t SEC_sys_logout(void *session_handle);
Int_t SEC_admin_sys_add_user(Char_t *sapasswd,Char_t *user, Char_t *passwd);
Int_t SEC_admin_sys_delete_user(Char_t *sapasswd, Char_t *user);
Int_t SEC_admin_sys_modify_user(Char_t *sapasswd, Char_t *user,
				Char_t *characteristic, Char_t *value);
Int_t SEC_admin_sys_use_security(Char_t *sapasswd, Int_t onoff);
Int_t SEC_sys_use_security();
Int_t SEC_admin_sys_ignore_passwds(Char_t *sapass,Int_t val,Int_t get_or_set);

/* database level api calls */
Int_t SEC_acl_get_user_access(SecKey_t *key, Char_t *dbname, 
			      Int_t doc_id, Char_t *field);
Int_t SEC_admin_acl_set_access(SecKey_t *key, Char_t *user, Access_t access,
			       Char_t *dbname, Int_t doc_id, Char_t *field);
Int_t SEC_admin_acl_delete_user(SecKey_t *key, Char_t *user, Char_t *dbname); 
Int_t SEC_admin_acl_flush_hash(SecKey_t *key);

#endif


