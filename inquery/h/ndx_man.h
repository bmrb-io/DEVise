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
 *  ndx_man.h:  Declarations and macros for use with ndx_man.c.
 *
 *  Created:  Spring, 1994
 */

#if !defined(NDX_MAN_H)
  #define NDX_MAN_H

  #define INITIAL_INDEX_TABLE_SIZE     999
  #define NDX_PREFIX_LENGTH            4
  #define MAX_NDX_TERM_LENGTH        128
  #define MAX_NUM_OF_FTAG            899

  #include "keyref.h"
  #include "keycalls.h"
  #include "in_node.h"
  #include "inq_api.h"
  #include "il.h"

  Char_t *get_ndx_term_prefix(void *ndx_handle);
  Int_t  addNewIndex (dbinfo *db, Char_t *indexName, Char_t *description,
		      Char_t field_index);
  Int_t  allocate_index_table (dbinfo *db);
  Int_t  get_index_count (dbinfo *db);
  Int_t  initialize_index_table (dbinfo *db);
  Int_t  initialize_ndx (dbinfo *db, Char_t *index_filename,
			 int mode);
  Int_t refresh_ndx(dbinfo *db);

  Int_t  getIndexPrefix (dbinfo *db, Char_t *indexName, Char_t *indexPrefix);
  Char_t *getIndexName (dbinfo *db, Int_t ndx_prefix);
  Int_t  get_max_ndx_record_size (dbinfo *db);

  Char_t *addPrefixToIndexKey (Char_t *prefix, Char_t *key);
  Char_t *ndx_id2term (Int_t id);
  Char_t *show_current_index_prefix (dbinfo *db);
  void   set_current_index_prefix (dbinfo *db, Char_t *prefix);
  Char_t *printIndexTable (dbinfo *db);
  void writeIndexTableToIndexFile (dbinfo *db);

  void   set_ndx_fcb_to_9s (dbinfo *db);
  BoolInt_t is_ndx_fcb_open (dbinfo *db);
  void   terminate_index (dbinfo *db);

  Char_t *enum2relop (relop_enum_type rel_op);
  relop_enum_type relop2enum (Char_t *op_string);

  void InitializeGenFtagFlags (dbinfo *db);
  void SetGenFtagFlags (dbinfo *db, Int_t ftag_num);
  Char_t GetGenFtagFlags (dbinfo *db, Int_t ftag_num);

  Int_t get_field_min_by_name (dbinfo *db, Char_t *field_name,
                               Char_t **field_buf, Int_t  *buf_len);
  Int_t get_field_min_by_prefix (dbinfo *db, Char_t *field_prefix,
                                  Char_t **field_buf, Int_t  *buf_len);
  Int_t get_field_max_by_name (dbinfo *db, Char_t *field_name,
                               Char_t **field_buf, Int_t  *buf_len);
  Int_t get_field_max_by_prefix (dbinfo *db, Char_t *field_prefix,
                                 Char_t **field_buf, Int_t  *buf_len);
  Int_t get_field_description_by_name (dbinfo *db, Char_t *field_name,
                                       Char_t **field_buf, Int_t  *buf_len);
  Int_t get_field_description_by_prefix (dbinfo *db, Char_t *field_prefix,
                                         Char_t **field_buf, Int_t  *buf_len);
  Int_t get_field_name_by_prefix (dbinfo *db, Char_t *field_prefix,
                                  Char_t **field_buf, Int_t  *buf_len);
  Int_t get_index_field_list (dbinfo *db, Char_t **field_buf, Int_t *buf_len);
  Int_t free_field_buffer(dbinfo *db, Char_t *field_buf);
  Int_t  getGT(dbinfo *db, Int_t prefix, Char_t *key, Int_t equal, Char_t *new_term);
Int_t  getLT(dbinfo *db, Int_t prfx, Char_t *key, Int_t equal,
	     Char_t *new_term);

Int_t  getRANGE(dbinfo *db, Int_t prfx, Char_t *key, Char_t *new_term,
		Int_t initialize, Int_t exclude_high, Int_t exclude_low);

Int_t  getNEQ(dbinfo *db, Int_t prfx, Char_t *exclude_key, Char_t *key,
	      Char_t *new_term, Int_t initialize);
  
#endif



