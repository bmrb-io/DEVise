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


#ifndef _CLISRV_SECKEY_H_
#define _CLISRV_SECKEY_H_

#include "docdef.h"
#include "doctypes.h"
#include "security_constants.h"
#include "mdb_message.h"

struct securitykey{
   Char_t user_key[MAX_NAME];   /* the username - must be unique in system */
   Int32_t day_key;
   Int32_t hidden_key;
   Int32_t num_groups;          /* adjusted... 10 groups -> num_groups = 9 */
   Char_t user_groups[MAX_GROUPS][MAX_NAME];
};

typedef struct securitykey SecKey_t;

Int_t message_push_seckey(msg_handle_t *msg, SecKey_t *key);
Int_t msg_push_seckey(message_t **msg_in, Int_t *index, SecKey_t *key);
Int_t msg_pop_seckey_data(message_t *msg, Int_t *data_index, SecKey_t **key);
Int_t message_pop_seckey(msg_handle_t *msg, SecKey_t **key);
Int_t seckey_free(SecKey_t *key);
Int_t seckey_create(SecKey_t **key);
Int32_t daykey();
Int32_t hiddenkey();
Int_t seckey_use_security(Int_t value, Int_t get_or_set);

#define SECKEY_SIZE sizeof(SecKey_t)
#endif

