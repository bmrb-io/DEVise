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

#ifndef _MDB_MESSAGE_H_
#define _MDB_MESSAGE_H_

#include <stdio.h>
#include <stdlib.h>

#include "docdef.h"
#include "message.h"

#define DBNAME_SIZE 120
#define CSNAME_SIZE 120
#define SHORT_NAME_SIZE 20

/*------------------------------------------------*
 *  Message Version 2.1 is defined in message.h.
 *------------------------------------------------*/
#define MSG_VSN_2_1 9021
#define OLD_VERSION 9021

/*------------------------------------------------*
 * Message Versions 3.0 and later are defined in
 *    mdb_message.h.
 * INQUERY Data Structures change b/t releases
 *    therefore, there is a need to attach a new
 *    version with each release where data types
 *    have been modified.
 *------------------------------------------------*/
#define MSG_VSN_3_0 9022
#define NEW_VERSION 9022

#define MSG_VSN_3_1 9031
#define MSG_VSN_3_1p 9032

#define MSG_VERSION MSG_VSN_3_1p

/*
 *  MDB_MESSAGE RETURN VALUES
 */
#define INQ_MDB_MSG_SUCCESS     0
#define INQ_MDB_MSG_OLD         1
#define INQ_MDB_MSG_PARTIAL     2
#define INQ_MDB_CANCEL_PARTIAL  3
#define INQ_MDB_MSG_ERROR      -1
#define INQ_MDB_MSG_NO_MSG     -2
#define INQ_MDB_MSG_EOF        -3

/*
 *  MULTI-DATABASE COMMAND CODES
 *  (for other commands - see message.h)
 */
/*--------------------------------------------------------------*
**  Command Code Range   500...1000    Multi-Database APIs
*---------------------------------------------------------------*/
#define INQ_MDB_OPEN_DBS 500L
#define INQ_MDB_OPEN_DBS_R 501L

#define INQ_MDB_PARSE_QUERY    502L
#define INQ_MDB_PARSE_QUERY_R  503L
#define INQ_MDB_MOD_QUERY_R    504L

#define INQ_MDB_EVAL_QUERY 505L
#define INQ_MDB_EVAL_QUERY_R 506L

#define INQ_MDB_QRYHANDLE_EVAL   507L
/*  reply code == INQ_MDB_EVAL_QUERY_R **/

#define INQ_MDB_QRYHANDLE_UPDATE  508L
/*  reply code == INQ_MDB_PARSE_QUERY_R **/

#define INQ_MDB_DOC_SUMMARY 510L
#define INQ_MDB_DOC_SUMMARY_R 511L

#define INQ_MDB_RF_EVAL_QUERY 515L
#define INQ_MDB_RF_EVAL_QUERY_T 516L

#define INQ_MDB_BROWSE_READ         520L

#define INQ_MDB_BROWSE_LIST     525L

#define INQ_MDB_BROWSE_MODE     530L
#define INQ_MDB_BROWSE_R            531L

#define INQ_MDB_DISPLAY_SUMMARY    550L
#define INQ_MDB_DISPLAY_SUMMARY_R  551L

#define INQ_MDB_FEEDBACK           590L
#define INQ_MDB_FEEDBACK_R         591L

#define INQ_MDB_CANCEL_REQUEST     592L
#define INQ_MDB_CANCEL_REQUEST_R   593L

/*
 *  INROUTE COMMAND CODES
 */
/*--------------------------------------------------------------*
**  Command Code Range   1001 .. 1500  INROUTE APIs
*---------------------------------------------------------------*/
#define INR_OPEN                   1001L
#define INR_OPEN_R                 1002L

#define INR_CLOSE                  1003L
#define INR_CLOSE_R                1004L

#define INR_FLUSH                  1005L
#define INR_FLUSH_R                1006L

#define INR_STAT_MODE_SET          1007L
#define INR_STAT_MODE_SET_R        1008L

#define INR_STAT_MODE_GET          1009L
#define INR_STAT_MODE_GET_R        1010L

#define INR_PROFILE_ADD            1011L
#define INR_PROFILE_ADD_R          1012L

#define INR_PROFILE_REPLACE        1013L
#define INR_PROFILE_REPLACE_R      1014L

#define INR_PROFILE_DELETE         1015L
#define INR_PROFILE_DELETE_R       1016L

#define INR_FEEDBACK               1017L
#define INR_FEEDBACK_R             1018L

#define INR_COMPARE_DOC            1019L
#define INR_COMPARE_DOC_R          1020L

#define INR_MATCHES                1021L
#define INR_MATCHES_R              1022L

#define INR_PROFILE_PARSED_QUERY   1023L
#define INR_PROFILE_PARSED_QUERY_R 1024L

#define INR_PARSED_QUERY_PREVIEW   1025L
#define INR_PARSED_QUERY_PREVIEW_R 1026L

/*
 * INROUTE Database APIs 1101 to 1200
 */
#define INRDB_CREATE               1101L
#define INRDB_CREATE_R             1102L

#define INRDB_OPEN                 1103L
#define INRDB_OPEN_R               1104L

#define INRDB_NAME_GET             1105L
#define INRDB_NAME_GET_R           1106L

#define INRDB_STEMMING_GET         1107L
#define INRDB_STEMMING_GET_R       1108L

#define INRDB_STEMMING_SET         1109L
#define INRDB_STEMMING_SET_R       1110L

#define INRDB_GET_STOPPING         1111L
#define INRDB_GET_STOPPING_R       1112L

#define INRDB_STOPPING_SET         1113L
#define INRDB_STOPPING_SET_R       1114L

#define INRDB_ADD_STATS_COLLECTION   1115L
#define INRDB_ADD_STATS_COLLECTION_R 1116L

#define INRDB_ADD_STATS_INQUERY    1117L
#define INRDB_ADD_STATS_INQUERY_R  1118L

#define INRDB_ADD_STATS_INROUTE    1119L
#define INRDB_ADD_STATS_INROUTE_R  1120L

#define INRDB_COMMIT               1121L
#define INRDB_COMMIT_R             1122L

#define INRDB_FREE                 1123L
#define INRDB_FREE_R               1124L

#define INRDB_DESTROY              1125L
#define INRDB_DESTROY_R            1126L

/* The last message code for InRoute */
#define INR_MAX                    1500L

/*--------------------------------------------------------------------*
 *  DATA STRUCTURES
 *
 *  These Data Types are *never* to be exported as modifiable.
 *   Therefore, we can make them visible to our internal modules
 *   with the understanding that all fields are visible *only* via
 *   the provided accessor functions and MACROS.!
 *--------------------------------------------------------------------*/
/*
**  This is the part of the message which is actually passed across 
**   the network to the client or server.
**   It contains the INQUERY COMMAND CODE, and the parameter list.
*/
struct msg{
  Int32_t msg_length;
  Int32_t version;
  Int32_t command;
  Int32_t timeout;       /* max time to wait for a reply */
  Int32_t num_dbs;
  Int32_t db_id;         /* if the num_dbs = 1 */
  Int32_t data_length;
  Char_t  data[1];       /* ------------------------------------*
			  *   db_uinfo_t dbs[num_dbs];
                          *   Bytes *data;
                          *-------------------------------------*/
};
/*
**  This is the administrative part of the message. It manages how
**   many bytes of data have been read, what the current offset into
**   the data structure is, and whether the message headers are in
**   network canonical form.
**   
**  This part of the message is NEVER passed across the network.
*/
struct msg_handle{
  int MAGIC;
  int version;
  int max_size;                 /* allocated message size size */
  int msg_length;               /* the total size of the attached message */
  int offset;                   /* offset used for reading/sending MSG */
  int push_index;               /* index for adding DATA */
  int pop_index;                /* index for extracting DATA */
  int canonical_form;
  void *msg;
};

typedef struct msg mdb_message_t;
typedef struct msg_handle msg_handle_t;

/*--------------------------------------------------------------------*/
/*  Message Header data structure used primarily for logging
 *  Message information to a debugging log file.
 *--------------------------------------------------------------------*/
struct header{
  int msg_length;
  int version;
  int dbid;
  int command;
  int data_length;
};

typedef struct header header_t;

typedef in_data_t msg_data_t;

/*--------------------------------------------------------------------*/
/*
 *  MULTI-DATABASE MESSAGE MACROS
 *

mdb_message_create()
mdb_message_free()
mdb_message_set_*()
mdb_message_get_*()
 */
#define msg_canonical_field(x) *((Int32_t *)canonical_Nbytes(x, sizeof(Int32_t)))

#define MSG_30_CANONICAL(in_msg)\
    in_msg->msg_length = msg_canonical_field(&in_msg->msg_length);\
    in_msg->command = msg_canonical_field(&in_msg->command);\
    in_msg->version = msg_canonical_field(&in_msg->version);\
    in_msg->db_id = msg_canonical_field(&in_msg->db_id);\
    in_msg->num_dbs = msg_canonical_field(&in_msg->num_dbs);\
    in_msg->data_length = msg_canonical_field(&in_msg->data_length)

#define MSG_21_CANONICAL(msg)\
    msg->message_length = msg_canonical_field(&msg->message_length);\
    msg->command = msg_canonical_field(&msg->command);\
    msg->db_id = msg_canonical_field(&msg->db_id);\
    msg->data_length = msg_canonical_field(&msg->data_length)

#define mdb_message_get_version(msg, vsn) *vsn = msg->version

/*-------------------------------------------------*
**  The "get_data_length" function returns the 
**   size (in bytes) of the unread data
**   remaining in the message.
*-------------------------------------------------*/
#define mdb_message_get_data_length(x, ln)\
    if((x)->version == OLD_VERSION)\
        *ln = ((message_t *)(x)->msg)->data_length - (x)->pop_index;\
    else *ln = ((mdb_message_t *)(x)->msg)->data_length - (x)->pop_index;\
    if((x)->canonical_form) *ln = msg_canonical_field(ln)

#define mdb_message_set_command(x, cmd)\
{ int val=cmd;\
    if((x)->canonical_form)  val = msg_canonical_field(&val);\
    if((x)->version == OLD_VERSION)\
     ((message_t *)(x)->msg)->command = val;\
    else\
      ((mdb_message_t *)(x)->msg)->command = val;\
}      

#define mdb_message_get_command(x, cmd)\
    if((x)->version == OLD_VERSION)\
       *cmd = ((message_t *)(x)->msg)->command;\
    else *cmd = ((mdb_message_t *)(x)->msg)->command;\
    if((x)->canonical_form) *cmd = msg_canonical_field(cmd)

#define mdb_message_incr_dbs(x, num)\
     ((mdb_message_t *)(x)->msg)->num_dbs += num

#define mdb_message_get_numdbs(x, num)\
      *num = ((mdb_message_t *)(x)->msg)->num_dbs

#define mdb_message_set_dbid(x, dbid)\
{int new_dbid=dbid;\
     if((x)->canonical_form) new_dbid = msg_canonical_field(&new_dbid);\
     if((x)->version == OLD_VERSION)\
       ((message_t *)(x)->msg)->db_id = new_dbid;\
     else ((mdb_message_t *)(x)->msg)->db_id = new_dbid;\
}
    
#define mdb_message_get_dbid(x, dbid)\
    if((x)->version == OLD_VERSION)\
       *dbid = ((message_t *)(x)->msg)->db_id;\
    else *dbid = ((mdb_message_t *)(x)->msg)->db_id;\
    if((x)->canonical_form) *dbid = msg_canonical_field(dbid)
      
#define mdb_message_make_canonical(x)\
    if((x)->version == OLD_VERSION){ MSG_21_CANONICAL(((message_t *)(x)->msg));}\
    else {   MSG_30_CANONICAL(((mdb_message_t *)(x)->msg));} \
    (x)->canonical_form = ((x)->canonical_form)^1

#define mdb_message_reverse_canonical(x) mdb_message_make_canonical(x)

#define mdb_message_reset(handle, indices_only)\
    handle->push_index = 0;\
    handle->pop_index = 0;\
    if(!indices_only) \
     if(handle->version == OLD_VERSION)\
        memset(handle->msg,0, sizeof(message_t) + ((message_t *)handle->msg)->data_length);\
     else \
       memset(handle->msg,0, sizeof(mdb_message_t) + ((mdb_message_t *)handle->msg)->data_length)

/*--------------------------------------------------------------------*/
/*
 *  MULTI-DATABASE MESSAGE FUNCTION PROTOTYPES
 */

int mdb_message_create(msg_handle_t **msg, int num_dbs, int data_length);
int mdb_message_create_from_old(msg_handle_t **msg, message_t *old_msg);
int mdb_message_create_old(msg_handle_t *message, message_t **old_msg);
int mdb_message_free(msg_handle_t **msg);

int mdb_message_add_dbs(msg_handle_t *msg, void *db_list);
int mdb_message_get_dbs(msg_handle_t *msg, int *num_dbs, void **dbs);

int mdb_message_push_string(msg_handle_t *msg, msg_data_t compressed, char *string);
int mdb_message_pop_string(msg_handle_t *msg, msg_data_t *compressed, int *num_bytes, 
			   char **string);

int mdb_message_push_number(msg_handle_t *msg, msg_data_t num_type, void *number);
int mdb_message_pop_number(msg_handle_t *msg, msg_data_t num_type, void *number);

int mdb_message_push_bytes(msg_handle_t *msg, int num_bytes, int canonical, void *bytes);
int mdb_message_pop_bytes(msg_handle_t *msg, int *num_bytes, int canonical, void **bytes);

int mdb_message_push_data(msg_handle_t *msg, int num_bytes, void *bytes);
int mdb_message_pop_data(msg_handle_t *msg, int num_bytes, void **bytes);

int mdb_message_send(msg_handle_t *msg, void *conn, int version);
int mdb_message_read(msg_handle_t **msg, void *conn, int *num_bytes, int need_new);
int mdb_message_read_full(msg_handle_t **msg, void *conn, int *command);
int mdb_message_send_full(void *conn, msg_handle_t *msg);

/*--------------------------------------------------------------------*
 *   L O G G I N G     P R O T O T Y P E S 
 *--------------------------------------------------------------------*/
int mdb_message_log_open(char *filename, char *dbname, int debug_lvl, int toggle);
int mdb_message_log_close(void);
int mdb_message_log_headers(void *msg);
int mdb_message_log_string(char *info_string);
int mdb_message_log_time(void);

#endif






















