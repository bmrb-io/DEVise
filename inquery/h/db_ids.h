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
 * "DB_IDS_H" is a header file for create and manipulate unique db identifiers.
 * The db id is a timestamp.pid touple.  It is saved in string form in
 * the .inf file and is accessible through the dbinfo structure.
 * 
 * The network push and pop routines for db ids are in db_msg.c
 */

#if !defined(DB_IDS_H)
#define DB_IDS_H

#include "mdb_message.h"

typedef struct db_id db_id_t;

/* Create a db id.  This id should be unique */
int db_id_create(db_id_t **db_id);

/* Convert the db id to a string representation */
int db_id_to_string(db_id_t *db_id, char **id_str);

/* Create a db_id structure from a string representation */
int db_id_from_string(char *id_str, db_id_t **db_id);

/* Free a db_id structure */
int db_id_free_id(db_id_t *db_id);

/* Copy a db_id structure */
int db_id_make_copy(db_id_t *from, db_id_t **to);

/* Return non-zero if these 2 ids are equal */
int db_id_equal(db_id_t *id1, db_id_t *id2);

/* Compare two db ids.  The equal parameter will be non zero if the
   two ids are identical */
int db_id_compare(db_id_t *id1, db_id_t *id2, int *equal);

int message_push_db_id_type(msg_handle_t *msg, db_id_t *db_id);
int message_pop_db_id_type(msg_handle_t *msg, db_id_t **db_id_ptr);
int msg_pop_db_id(message_t *msg, Int_t *data_index, db_id_t **db_id_ptr);
int msg_push_db_id(message_t **msg_in, Int_t *index, db_id_t *db_id);
#endif
