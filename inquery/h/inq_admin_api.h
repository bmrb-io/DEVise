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

#ifndef _INQ_ADMIN_H_
#define _INQ_ADMIN_H_

#include <stdio.h>
#include <stdlib.h>

#include "cs_db_table_type.h"

/*
 *  F U N C T I O N    P R O T O T Y P E S 
 */

int inq_admin_inquery_svr_stop(void *handle, char *db_name);
int inq_admin_inquery_svr_start(void *handle, char *db_name);
int inq_admin_conn_svr_stop(void *handle);
int inq_admin_inquery_svr_list_active(void *handle, void **dbname_list);
int inq_admin_conn_save_config(void *session_handle);
int inq_admin_conn_reread_config(void *session_handle, int force);

int inq_admin_register_db(void *handle, cs_db_info_elt_t *db);
int inq_admin_unregister_db(void *handle, cs_db_info_elt_t *db);
int inq_admin_list_dbs(void *handle, cs_db_info_elt_t ***dbs, int *num_dbs);
int inq_admin_free_dbs(cs_db_info_elt_t **dbs, int num_dbs);

#endif /* _INQ_ADMIN_H_ */
