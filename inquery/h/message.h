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
 * message.h
 */

#if !defined(MESSAGE_H)
#define MESSAGE_H


#include "byteorder.h"      /* PC: "byteorde.h" */

/*---------------------------------------------------*
** Command Code Range 0..300 	Single Database APIs
**---------------------------------------------------*/
#define QRYS_BELIEFLIST 	0L
#define QRYS_QUERY      	1L
#define QRYS_STATS      	2L
#define QRYS_TERMINATE  	3L
#define QRYS_OPEN_DB		4L
#define QRYS_OPEN_DB_R		5L
#define QRYS_PARSE_QUERY	6L
#define QRYS_PARSE_QUERY_R	7L
#define QRYS_QUERY_RF      	8L
#define QRYS_MODIFIED_QUERY    	9L
#define QRYS_MODIFIED_QUERY_R  	10L
#define QRYS_CHANGE_DB		11L
#define QRYS_CHANGE_DB_R	12L
#define QRYS_EVAL_FB_DOC        13L
#define QRYS_EVAL_FB_DOC_R      14L
#define QRYS_EVAL_PARSED_QUERY  15L
#define QRYS_XID_UPDATE_QUERY   16L
/* the following is used primarily for collection selection dbs */
#define QRYS_EVAL_SCALED        20L

#define DOCS_GET_DOC    	100L
#define DOCS_GET_DOC_XID    	150L
#define DOCS_GET_SUMMARY	101L
#define DOCS_GET_SUMMARY_R	102L
#define DOCS_STATS      	103L
#define DOCS_TERMINATE  	104L
#define DOCS_GET_DOC_R  	105L
#define DOCS_EXTERN_TO_INTERN_DOC_ID	106L
#define DOCS_EXTERNAL_DOC_ID		107L
#define DOCS_INTERN_TO_EXTERN_DOC_ID	108L
#define DOCS_INTERNAL_DOC_ID		109L

#define DOCS_COLLECTION_TF      110L
#define DOCS_COLLECTION_TF_R    111L
#define DOCS_DOCUMENTS_TF       112L
#define DOCS_DOCUMENTS_TF_R     113L

/* Deleted DOCS_LOOKUP_TERM here */
#define DOCS_STEM_TERM          116L
#define DOCS_STEM_TERM_R        117L
#define DOCS_STOP_WORD          118L
#define DOCS_STOP_WORD_R        119L
#define DOCS_GET_FIELD		120L
#define DOCS_GET_FIELD_R        121L
#define DOCS_GET_RAW_DOC	122L
#define DOCS_GET_RAW_DOC_R	123L 
/*
 *** New Command Codes for Compression and (LOC) get_doc_by_offset
 ***   function.
 ***
 *** Merge conflict resolved by re-defining "DOCS_TRANSLATE_COLL*" codes
 */
#define DOCS_GET_DOC_BY_OFFSET	  124L
#define DOCS_GET_DOC_BY_OFFSET_R  125L
#define DOCS_GET_MATCHINFO	126L 
#define DOCS_GET_MATCHINFO_R	127L 
#define DOCS_TRANSLATE_COLL     128L
#define DOCS_TRANSLATE_COLL_R   129L

#define DOCS_EXTERN_TO_INTERN_ID_LIST	128L
#define DOCS_INTERN_TO_EXTERN_ID_LIST	129L

/* New(3.1) document summary operation using *any* field */
#define DOCS_DISPLAY_SUMMARY    130L
#define DOCS_DISPLAY_SUMMARY_R  131L

#define INQUERY_STATS           200L
#define INQUERY_TERMINATE       201L
#define INQ_SEND_DB_INFO        202L

#define INQ_OPEN_BROWSE         203L
#define INQ_OPEN_BROWSE_R       204L

#define INQ_READ_BROWSE         205L
#define INQ_READ_BROWSE_R       206L

#define INQ_CLOSE_BROWSE        207L
#define INQ_CLOSE_BROWSE_R      208L

#define INQ_GET_BROWSE_LIST     209L
#define INQ_GET_BROWSE_LIST_R   210L

#define INQ_GET_BROWSE_MODE     211L
#define INQ_GET_BROWSE_MODE_R   212L

/*!************************************
**  Security Subsystem codes         **
**  Range is 260 - 289               */
#define ACL_GET_USER_ACCESS       260L
#define ACL_GET_USER_ACCESS_R     261L

#define ADMIN_ACL_SET_USER        262L
#define ADMIN_ACL_SET_USER_R      263L

#define ADMIN_ACL_DELETE_USER     264L
#define ADMIN_ACL_DELETE_USER_R   265L

#define SYS_LOGIN                 268L
#define SYS_LOGIN_R               269L

#define SYS_LOGOUT                270L
#define SYS_LOGOUT_R              271L

#define SYS_USE_SECURITY          272L
#define SYS_USE_SECURITY_R        273L

#define ADMIN_SYS_USE_SECURITY    274L
#define ADMIN_SYS_USE_SECURITY_R  275L

#define ADMIN_SYS_ADD_USER        276L
#define ADMIN_SYS_ADD_USER_R      277L

#define ADMIN_SYS_DELETE_USER     278L
#define ADMIN_SYS_DELETE_USER_R   279L

#define ACL_USE_SECURITY          280L
#define ACL_USE_SECURITY_R        281L

#define ADMIN_SYS_IGNORE_PASSWDS  282L
#define ADMIN_SYS_IGNORE_PASSWDS_R 283L

/*------------------------------------
 *  OBSOLETE BROWSE FUNCTIONS
 *
 * #define INQ_BROWSE_LAST_LINE    213L
 * #define INQ_BROWSE_LAST_LINE_R  214L
 * 
 * #define INQ_BROWSE_WINDOW       215L
 * #define INQ_BROWSE_WINDOW_R     216L
 *------------------------------------*/

#define INQ_CHECK_QUERY         219L
#define INQ_CHECK_QUERY_R       220L

/*
 *  INDEX TABLE FUNCTIONS START AT 250
 */
#define INQ_MINFIELD_BYNAME     250L
#define INQ_MINFIELD_BYNAME_R   251L

#define INQ_MAXFIELD_BYNAME     252L
#define INQ_MAXFIELD_BYNAME_R   253L

#define INQ_FIELD_TABLE         254L
#define INQ_FIELD_TABLE_R       255L

#define INQ_FIELD_DESCR         256L
#define INQ_FIELD_DESCR_R       257L

#define INQ_INDEX_LIST          258L
#define INQ_INDEX_LIST_R        259L

/* NOTE -- 260-269 taken by the security server */
#define INQ_TERM_COLLECT       270L
#define INQ_TERM_COLLECT_R     271L

#define INQ_FEEDBACK           290L
#define INQ_FEEDBACK_R         291L

#define INQ_CANCEL_REQUEST     292L
#define INQ_CANCEL_REQUEST_R   293L


/* 
 * Query Based Stemming 
 */
#define INQ_QBS_GET_TERM_VARIANTS    294L
#define INQ_QBS_GET_TERM_VARIANTS_R  295L

/*----------------------------------------------------------------------*
**  Command Code Range 300..400 	System Level Control Commands
**----------------------------------------------------------------------*/

#define CONN_AVAIL_DBS		    300L
#define CONN_AVAIL_DBS_R	    301L

#define CONN_TERMINATE              302L
#define CONN_TERMINATE_R            303L

#define CONN_START_SVR              304L
#define CONN_START_SVR_R            305L

#define CONN_ACTIVE_DBS             306L
#define CONN_ACTIVE_DBS_R           307L

#define CONN_SAVE_CONFIG            308L
#define CONN_SAVE_CONFIG_R          309L

#define CONN_TERM_SVR               310L
#define CONN_TERM_SVR_R             311L

#define CONN_REREAD_CONFIG          312L
#define CONN_REREAD_CONFIG_R        313L

#define CONN_REGISTER_DB            314L
#define CONN_REGISTER_DB_R          315L

#define CONN_UNREGISTER_DB          316L
#define CONN_UNREGISTER_DB_R        317L

#define CONN_LIST_DBS               318L
#define CONN_LIST_DBS_R             319L

/* Field access calls */

#define INQ_DB_DOC_TYPES       320L
#define INQ_DB_DOC_TYPES_R     321L

#define INQ_DB_DOC_TYPE_FIELDS     322L
#define INQ_DB_DOC_TYPE_FIELDS_R   323L

#define INQ_DB_FIELD_DEFINITION     324L
#define INQ_DB_FIELD_DEFINITION_R   325L

/*--------------------------------------------------------------------*
 *  Command Code Range 8000...9000   Error Codes
 *--------------------------------------------------------------------*/
#define INQSRV_ERROR            9000L
#define INQSRV_SEC_PROTECT      9002L

/*--------------------------------------------------------------------*
 *  Command Code Range 9001...    Message Version Numbers
 *  
 *  This will prevent the case where "command 22" isn't a message of
 *       "version 22"
 *--------------------------------------------------------------------*/

/*-----------------------------------------------------------*
 *   The sizeof() operator returns a value of 20 
 *       for message_t.  
 *   The value we want to know for "sending" the base type  is 16.
 *       (4 * sizeof(Int32_t)) + sizeof(Char_t)
 *-----------------------------------------------------------*/
#define MESSAGE_SIZE 16

typedef struct
{
  Int32_t   message_length;  /* = message_length+command+data_length+data */
  Int32_t   command;
  Int32_t   db_id;
  Int32_t   data_length;
  Char_t  data[1];
} message_t;


/*
 *  MSG_POP_NUMERIC_DATA pops numeric data (int, short int, float, char)
 *  from the specified portion of the data field of a message_t data
 *  structure.  The caller must specify the message (msg_ptr), an index
 *  into the data field (index), the data_type (e.g. int), and a location
 *  in which to store the number (destination).  (I used the comma operator
 *  to separate statements so that msg_pop_numeric_data could always be
 *  terminated by a semicolon.)
 */
#define msg_pop_numeric_data(msg_ptr, index, data_type, destination)\
  ( \
   memcpy((destination), canonical_Nbytes (((msg_ptr)->data + (index)), sizeof(data_type)), sizeof(data_type)) \
  , \
  (index) += sizeof (data_type) \
  )

/*
 *  MSG_PUSH_NUMERIC_DATA pushes numeric data (int, short int, float, char)
 *  onto the end of the data field of a message_t data structure.  The
 *  caller must specify the message (msg_ptr), how much of the data field
 *  has been filled (index), the data_type (e.g. int), and the number to
 *  store.  The macro will increase the length of the message if necessary.
 */
#define msg_push_numeric_data(msg_ptr, index, data_type, number)\
  { \
  register Int_t data_size  = sizeof (data_type); \
  register Int_t new_length = (index) + data_size; \
\
  if (new_length > (msg_ptr)->data_length) { \
    reallocate_message (&(msg_ptr), new_length); \
    update_lengths(msg_ptr, new_length); \
  } \
\
  memcpy ((void *) ((msg_ptr)->data + (index)), \
	  (void *) canonical_Nbytes ((number), data_size), \
	  data_size); \
  (index) += data_size; \
  }

/*
 *  MSG_PUSH_STRING_DATA pushes a character string onto the end of the data
 *  field of a message_t data structure.  The caller must specify the message
 *  (msg_ptr), how much of the data field has been filled (index), the
 *  character_type (e.g. char, or unsigned char), and the string to store.
 *  The macro will increase the length of the message if necessary.
 */
#define msg_push_string_data(msg_ptr, index, char_type, string)\
  { \
  register Int_t data_size  = (1 + strlen ((char *)string)) * sizeof (char_type); \
  register Int_t new_length = (index) + data_size; \
\
  if (new_length > (msg_ptr)->data_length) { \
    reallocate_message (&(msg_ptr), new_length); \
    update_lengths(msg_ptr, new_length); \
  } \
\
  memcpy (((msg_ptr)->data + (index)), (string), data_size); \
  (index) += data_size; \
  }

#define msg_pop_string_data(msg_ptr, index, char_type, string)\
 { \
   register Int_t num_chars = 1 + strlen(msg_ptr->data + index);\
   register Int_t data_size = num_chars * sizeof(char_type);\
\
   string = msg_ptr->data + index;\
   string[num_chars-1] = '\0';\
   index += data_size;\
 }

message_t *allocate_message (Int_t data_length);
Boolean_t free_message (message_t *m);
void pop_from_message_Int_t (message_t *msg_ptr, Int_t *data_index,
			     Int_t *the_int);
void push_on_message_Int_t (message_t **msg_ptr, Int_t *data_index,
			    Int_t the_int);
void reallocate_message (message_t **m, Int_t data_length);
void update_lengths (message_t *m, Int_t data_length);


#endif
