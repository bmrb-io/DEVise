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

#ifndef _INQMDB_API_H_
#define _INQMDB_API_H_

#include <stdio.h>
#include <stdlib.h>

#include "doctypes.h"
#include "in_node.h"
#include "inq_api.h"
#include "inq_err.h"
#include "resource_mon.h"    /* resource utilization headers */
#include "inq_system.h"

/*
 *  C O N S T A N T S
 */
#define INQ_MAX_SEL_DBS 8
#define INQ_MIN_SEL_DBS 3

#define INQ_FIELD_ID       INQ_DOC_FIELD_ID
#define INQ_FIELD_HEADING  INQ_DOC_FIELD_MAX + 1
#define INQ_FIELD_ALL      INQ_DOC_FIELD_MAX + 2

#define INQ_REMAINING   -1


/* for backwards compatibility with version 3.0 */
#define mdb_doc_t in_belief_elt_t

/*
 *  D A T A   S T R U C T U R E S
 */

/*
 *  Multi-Database Belief List Data Structure 
 */
struct mdb_doc{
  int relevant;
  float belief;
  int db_id;
  int doc_id;
  char *doc_xid;
};

typedef struct mdb_doc in_belief_elt_t;
typedef struct doc_list in_belief_list_t;

/*
 *  Multi-Database Query Data Structure
 */
struct per_db_info{
  int db_id;
  char *db_name;
  char *original_query;
  char *parsed_query;
  in_belief_list_t *rel_doc_list;
  char *modified_query;
  float qry_weight;       /* new for collection selection */
};

typedef struct per_db_info in_db_query_t;
typedef struct qry_handle in_qry_handle_t;

enum field_id_tag {int_id=10, name_id};
typedef enum field_id_tag in_field_tag_t;

typedef dbinfo in_db_handle_t;

/*
 *  INQUERY Status Data Type
 *  defines the returned status_t returned by INQUERY MDB APIs.
 *
 */
struct q_status{
      int return_val;
      res_usage_t usage_stats;
};

typedef struct q_status q_status_t;

/*
 *  MACROS to manipulate q_status_t 
 */
#define Q_SET_RETURN(stat, value)     stat.return_val = value; return(stat)
#define Q_CHECK_RETURN(stat, expected)   stat.return_val == expected

#define Q_SET_USAGE(stat, resource)   stat.usage_stats = resource
#define Q_GET_UTIME(stat)             stat.usage_stats.utime
#define Q_GET_STIME(stat)             stat.usage_stats.stime
#define Q_GET_WALLCLOCK(stat)         stat.usage_stats.wallclock
#define Q_GET_PG_FAULTS(stat)         stat.usage_stats.page_faults


/*define to allow backward compatability of mdb_init_inquery,mdb_term_inquery*/
#define inq_mdb_init_inquery(x,y) inq_init_system(x,y,0)
#define inq_mdb_term_inquery(x) inq_term_system(x,0)

/* provide backward compatibilty for 3.0 query_table operations */
#define inq_mdb_query_table_create(x,y,z) \
            inq_mdb_query_handle_create(x,y,z,NULL)

#define inq_mdb_query_table_free inq_mdb_query_handle_free

/*------------------------------------------*
 * SDB compatibility API
 *----------------------------------------------------------------------*/
int inq_mdb_get_dbhandle_by_id(void *session_handle, int db_id, in_db_handle_t **db);
int inq_mdb_get_dbhandle_by_name(void *session_handle, char *db_name, in_db_handle_t **db);

int inq_mdb_doc_list_get_doc(void *session_handle, void *doc_list, 
	                     int doc_index, void *query_info, 
	                     int mode, Doc_t **doc);

int inq_mdb_set_active_db_from_dbinfo(void *session_handle, dbinfo *db);
int inq_mdb_set_active_db_from_dbname(void *session_handle, char *dbname);
int inq_mdb_get_dbname_by_dbid(void *session_handle, int dbid, char **db_name);
int inq_mdb_get_dbid_by_dbname(void *session_handle, char *dbname, int *dbid);

/*------------------------------------------*
 * Database Name List Prototypes
 *----------------------------------------------------------------------*/
int inq_mdb_name_list_create(void **name_list);
int inq_mdb_name_list_free(void **name_list);
int inq_mdb_name_list_add_name(void *name_list, char *name);
int inq_mdb_name_list_del_name(void *name_list, char *name);
int inq_mdb_name_list_get_name(void *name_list, int name_id, char **name);
int inq_mdb_name_list_num_names(void *name_list, int *num_names);

/*------------------------------------------*
 * Field_ids Function Prototypes
 *----------------------------------------------------------------------*/
#define inq_mdb_field_ids_add_field inq_mdb_field_ids_add_id

int inq_mdb_field_ids_create(in_field_ids_t **field_ids, int max_fields);
int inq_mdb_field_ids_free(in_field_ids_t **field_ids);
int inq_mdb_field_ids_add_id(in_field_ids_t *field_ids, int field_flag);
int inq_mdb_field_ids_add_name(in_field_ids_t *field_ids, char *field_name);
int inq_mdb_field_ids_num_elts(in_field_ids_t *field_ids, int *num_fields);
int inq_mdb_field_ids_Nth(in_field_ids_t *field_ids, int position, 
			  in_field_tag_t *field_type, int *field_id, 
		  	  char **field_name);

/*-----------------------------------------*
 * MDB Document (& Collection Selection) Belief List Function Prototypes
 *----------------------------------------------------------------------*/
int inq_mdb_doc_list_set_fields(void *session_handle, void *doc_fields,
				in_belief_list_t *doc_list);

int inq_mdb_doc_list_get_field(in_belief_list_t *doc_list, int doc_index,
			       int field_id, char **field_value);

int inq_mdb_doc_list_create(in_belief_list_t **doc_list, int num_docs);
int inq_mdb_doc_list_create_from_belief_list(in_belief_list_t **doc_list,
					     int db_id, 
					     belieflst *belief_list);
int inq_mdb_doc_list_free(in_belief_list_t **doc_list);
int inq_mdb_doc_list_add_doc(in_belief_list_t *doc_list, 
			     in_belief_elt_t doc_info);

int inq_mdb_doc_list_get_info(in_belief_list_t *doc_list, 
			      int elt_id, in_belief_elt_t *doc);

int inq_mdb_doc_list_info_by_xid(in_belief_list_t *doc_list, 
				 char *doc_xid,  in_belief_elt_t *doc);

int inq_mdb_doc_list_num_elts(in_belief_list_t *doc_list, int *num_docs);

int inq_mdb_doc_list_mark_relevant(in_belief_list_t *doc_list,int elt_id);

int inq_mdb_doc_list_extract_relevant(in_belief_list_t *doc_list, 
				      in_belief_list_t **relevant_doc_list);

int inq_mdb_doc_list_copy(in_belief_list_t *doc_list, 
			  int start_doc, int end_doc,
			  in_belief_list_t **new_list);

int inq_mdb_doc_list_get_total_hits(in_belief_list_t *doc_list,
				    int *total_hits);

/*-----------------------------------------*
 * DB Belief List Function Prototypes
 *----------------------------------------------------------------------*/
#define inq_mdb_belief_list_num_elts     inq_mdb_doc_list_num_elts
#define inq_mdb_belief_list_mark_relevant     inq_mdb_doc_list_mark_relevant
#define inq_mdb_belief_list_free        inq_mdb_doc_list_free
#define inq_mdb_belief_list_Nth         inq_mdb_doc_list_get_info
#define inq_mdb_belief_list_elt_by_name inq_mdb_doc_list_info_by_xid

/*--------------------------------------------------*
 *  General Session Function Prototypes
 *-----------------------------------------------------------------------*/
int inq_session_init_dbinfo(void *session_handle, dbinfo *db, Login_t *login);
int inq_init_system(void **session_handle, void *location, Login_t *key);
int inq_term_system(void **session_handle, Login_t *key);
int inq_mdb_set_system_resource(void *session_handle, Int_t resource_type,
				Int_t resource_value);

int inq_mdb_set_avail_dbs(void *session_handle, char *dir_name, void **dbname_list);
int inq_mdb_get_avail_dbs(void *session_handle, void **dbname_list);

int inq_mdb_set_active_dbs(void *session_handle, void *dbname_list);
int inq_mdb_set_active_dbs_by_uid(void *handle, void *dbnames_list);
int inq_mdb_get_active_dbs(void *session_handle, void **dbname_list);
int inq_mdb_del_active_dbs(void *session_handle, void *dbname_list);

int inq_mdb_set_feedback_function(void *session_handle, feedback_function_t feedback_function, void *client_data);
int inq_mdb_get_feedback_function(void *session_handle, feedback_function_t *feedback_function);
int inq_mdb_get_feedback_client_data(void *session_handle, void **client_data);

/*--------------------------------------------*
 * QUERY Function Prototypes
 *------------------------------------------------------------------------*/
int inq_mdb_query_eval(void *session_handle, char *querystring, int num_elts,
		       int override /* unused */, 
		       in_belief_list_t **belief_list,
		       in_qry_handle_t **qry_handle);

int  inq_mdb_query_eval_rf(void *session_handle, in_qry_handle_t *qry_handle,
                           in_belief_list_t *relevant_doc_list, int num_docs,
                           int override /* unused */,
                           in_belief_list_t **rf_doc_list);

/* new query handle prototypes for query_handle API calls. */

q_status_t inq_mdb_query_handle_eval(void *session_handle, 
				     in_qry_handle_t *qry_handle, 
				     int num_docs, int override, 
				     in_belief_list_t **doc_list);

int inq_mdb_query_handle_parse(void *session_handle, 
			       in_qry_handle_t *qry_handle);

/*----------------------------------------------------*
 *  BROWSE Function Prototypes
 *-----------------------------------------------------------------*/
int
inq_mdb_get_browse_list (void *session_handle, lines_list_t **browse_list);


int
inq_mdb_get_browse_modes (void *session_handle, char *topic, lines_list_t **browse_modes);


int
inq_mdb_read_browse (void *session_handle, int lines, char *mode, 
		     char *search,  char *topic, lines_list_t **window);

/*------------------------------------------------*
 *  Collection Selection API
 *------------------------------------------------*/
Int_t inq_mdb_select_dbs(void *session_handle,  in_qry_handle_t *qry_handle,
                            Boolean_t set_dbs_active,
                            in_belief_list_t **rel_dbs);

int inq_mdb_activate_selected_dbs(void *session_handle, 
				  in_belief_list_t *rel_dbs);

/*------------------------------------------------*
 *  Display Document Summaries....
 *------------------------------------------------*/
int inq_display_doclist_get_field_by_doc(in_disp_doclist_t *doclist, 
				  char *dbname, int doc_id, 
				  int field_id, char *field_name, 
				  char **text, matchinfo_t **matches);

int inq_display_doclist_get_field(in_disp_doclist_t *doclist, int doc_pos, 
				  int field_id, char *field_name, 
				  char **text, matchinfo_t **matches);

int inq_display_doclist_num_docs(in_disp_doclist_t *doclist, int *num_docs);

int inq_display_doclist_free(in_disp_doclist_t **doclist);

Int_t inq_mdb_display_fields(void *session_handle, in_field_ids_t *field_ids,
			     in_qry_handle_t *qry_handle, 
			     in_belief_list_t *doclist,
			     int start_index, int end_index, 
			     in_disp_doclist_t **disp_doclist);

/*------------------------------------------------*
 *  User configurable options
 *------------------------------------------------*/
int inq_mdb_set_include_xids(void *handle, Boolean_t inc_xids);

/*------------------------------------------------*
 *  Query Handle API
 *------------------------------------------------*/
int 
inq_mdb_query_handle_create(in_qry_handle_t **qry_handle,
			    int num_dbs,
			    char *querystring,
			    in_belief_list_t *rel_doc_list);

int inq_mdb_query_handle_copy(in_qry_handle_t *qry_handle, 
			      in_qry_handle_t **new_handle);

int inq_mdb_query_handle_free(in_qry_handle_t **query_table);

int inq_mdb_query_handle_get_original_query(in_qry_handle_t *qry_handle, 
					   char **querystring);

int inq_mdb_query_handle_get_parsed_query(in_qry_handle_t *qry_handle,
					  char *db_name, char **parsed_query);

int inq_mdb_query_handle_get_modified_query(in_qry_handle_t *qry_handle,
					    char *db_name, 
					    char **modified_qry);

int inq_mdb_query_handle_add_rel_docs(in_qry_handle_t *qry_handle, 
				      in_belief_list_t *rel_doc_list, int replace_flag);

int inq_mdb_query_handle_get_dbquery_by_id(in_qry_handle_t *qry_handle,
					   int db_id, 
					   in_db_query_t **qry_info);

int inq_mdb_query_handle_get_dbquery_by_name(in_qry_handle_t *qry_handle,
					     char *db_name,
					     in_db_query_t **qry_info);

int inq_mdb_query_handle_set_weight(in_qry_handle_t *qry_handle,
				    Char_t *db_name, Float_t weight);

int inq_mdb_query_handle_init_weights(in_qry_handle_t *qry_handle);

int inq_mdb_db_query_free(in_db_query_t **qry_info);


#endif /* _INQMDB_API_H_ */
