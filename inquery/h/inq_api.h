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
 *  The INQUERY application programmer's interface (API) consists of
 *  two types of definitions.  "Core" definitions are those definitions
 *  that are absolutely necessary to use INQUERY.  "Service" definitions
 *  are those definitions that are necessary only to use some particular
 *  service that INQUERY provides.  For example, definitions that describe
 *  important INQUERY data structures are "core", while definitions that
 *  describe how to communicate with an INQUERY server are "service"
 *  definitions (because not all applications will want that service).
 *
 *  This file defines the "core" definitions of the INQUERY API.
 *
 *  Our intent is to keep the core definitions small, so that people can
 *  create small applications (in size and complexity).  The INQUERY
 *  services are partitioned into self-contained modules, so that you
 *  can create an application from just those services you need.
 */

#if !defined (INQ_API_H)
#define INQ_API_H

/*
 *  Include files:
 */
#include <stdio.h>


#include "docdef.h"
#include "doctypes.h"
#include "dbinfo.h"
#include "matchinfo.h"
#include "in_node.h"
#include "hashobj.h"
#include "stemmer.h"
#include "inq_system.h"
#include "browse_pub.h"
#include "net_ui.h"
#include "tl.h"
#include "sys_specific.h" /* for definition of the INQ_API modifier */
#include "dm_pub.h"
#include "security_p.h"
#include "inq_err.h"
#include "inq_feedback_info.h"

/*
 *  Literals:
 */
/*
 *   System Resources
 */
#define Q_BUFFER_QRY_MGR    90
#define Q_BUFFER_INDX_MGR   91
#define Q_LAST_RESOURCE     99


/* Modes for inq_get_doc subroutine (these are used in a bit vector,
 * so they must be power's of 2).
 */
#define ID       1		/* Get ID only */
#define HEADING  2		/* Get ID, title, & source text */
#define ALL      4		/* Get entire document */

#ifndef MATCH_LOCS
  #define MATCH_LOCS	  (8)
  #define MATCH_PASSAGES  (16)
#endif

/* Indicator for ending doc_rel_t lists (relevance feedback) */
#define INQ_END_RF_LIST -1

/* Field values for inq_get_doc_field subroutine */
#define INQ_DOC_FIELD_ID	0
#define INQ_DOC_FIELD_SOURCE	1
#define INQ_DOC_FIELD_TEXT	2
#define INQ_DOC_FIELD_TITLE	3

#define INQ_DOC_FIELD_MAX	3

/* Error Numbers */
#define INQ_ERROR_NONE		0

/* Limits */
#define MAX_DOCS_JUDGED 1000      /* Relevance feedback limit. */

/* Indices into a matchinfo_t datatype  */
#define INQ_MATCHINFO_TYPE	0
#define INQ_MATCHINFO_TERM	1
#define INQ_MATCHINFO_ID	1
#define INQ_MATCHINFO_BEGIN	2
#define INQ_MATCHINFO_LENGTH	3

/* Query term information codes */
#define NORMAL_TERM             1
#define TERM_NOT_FOUND          2
#define STOP_WORD               3
#define TRANSFORMER             4

/* Query info error codes */
#undef NO_ERROR
#define NO_ERROR                0
#define SYNTAX_ERROR           -1
#define NO_INDEXED_TERMS       -2
#define EVALUATION_ERROR       -3

/* 
 * INQUERY ERROR CODES
 */
#define INQ_DEADLOCK -92

/* 
 * Macros:
 */

#define inq_set_flag(x,y)   ( x | y )

/*
 *  Data structures:
 */

/* Query term structures */

typedef struct
{
  Int_t code;                   /* Identifies type of token */
  Int_t len;                    /* Number of strings in list */
  UChar_t **list;               /* Contains information determined by code */
} query_term_list_t;

typedef struct
{
  Char_t *q_str;                    /* query resulting from transformer */ 
  Char_t *name;                     /* name of this transformer */
} xformer_info_t;

typedef struct
{
  Int_t             error_code;      /* zero for no error */
  Int_t             error_offset;    /* only meaningfull if error_code != 0 */
  Char_t            *parsed_query;   /* final parsed query */
  xformer_info_t    *xformed_queries;/* intermediate changes to query */
  Int_t             num_terms;       /* total number of terms in query */
  Int_t             num_stops;       /* number of stop words in query */
  Int_t             num_not_found;   /* number of non-indexed words */
  Int_t             num_xformers;    /* number of identified transformers */
  query_term_list_t *term_list;      /* list of query term structures */
} query_info_t;


typedef struct
  {
  Int_t     doc_id;
  Boolean_t judged;
  Boolean_t relevant;
  } judged_docs_info_type;

typedef struct
  {
  Int_t     id;
  Boolean_t relevant;
  } doc_rel_t;

typedef struct
  {
  UChar_t	*text;
  Int_t		number_of_matches;
  matchinfo_t	*matches;
  } Doc_field_t;

typedef struct
  {
  Int_t         db_id;                  /* the Database ID of the doc */
  Int_t         id;
  void         *dm_doc;
  IN_node_type *query;
  matchinfo_t  *matches;
  matchinfo_t **match_ptr;
  Int_t         mp_num;
  UChar_t     **field_ptr;
  Int_t         fp_num;
  Int_t         match_mode;
  } Doc_t;    /* Document returned by inq_get_doc */

/*
 *   Typedefs for new document summary API
 */
typedef struct field_ids in_field_ids_t;
typedef struct disp_doclist in_disp_doclist_t;

/*
 *  Typedefs for Term Stats Collection
 */
typedef struct db_stat in_db_stat_t;  

/*
 *  Global variables:
 */
extern dbinfo *current_db;	 /* stats for, and ptrs to, current database */
extern BoolInt_t switch_stem_words;  /* Default = TRUE; can be set to false
				      * in by user, (see ui_utils or inqueryb)
				      * perhaps we need a user query profile
				      * analogous to dbinfo?
				      */

#if (defined(__MWERKS__) && defined(__cplusplus))
extern "C" {
#endif

/*
 *  Global subroutines:
 */

/* init_all_qtransformers turns all query text speciall recognizers
 * on=1 or off=0
 * init_qtransformer : turns a recognizer on=1 or off=0 by name
 * calling with a bad name returns a 1 and issues a Q_warning
 * with a list of the names.
 */
Char_t ** inq_list_qtransformers(void);
Int_t inq_get_qtransformer_state(char *name);
void inq_set_qtransformer_state(char *name, BoolInt_t on_off);
void init_all_qtransformers(BoolInt_t on_off);
BoolInt_t init_qtransformer(char *name, BoolInt_t on_off);
/* set_query_stemming turns user query stemming on/off (on=1 off=0)
 * get_query_stemming returns the last value set
 */

void set_query_stemming(BoolInt_t on_off);
BoolInt_t get_query_stemming();

Boolean_t inq_init_inquery(dbinfo *db);
Boolean_t inq_term_inquery(dbinfo *db);
Boolean_t inq_closedb (dbinfo *db);
belieflst *inq_eval_parsed_query (dbinfo *db,
				  Char_t *query_text,
				  feedback_function_t feedback_function);
INQ_API(belieflst *) inq_eval_query (dbinfo *db,
			   Char_t *query_text,
			   feedback_function_t feedback_function);
belieflst *inq_eval_query_net (dbinfo *db,
			       IN_node_type *n,
			       feedback_function_t feedback_function);

belieflst *inq_eval_query_top_n (dbinfo *db,
				 Char_t *query_text,
				 feedback_function_t feedback_function,
				 Int_t max_docs_to_retrieve);
belieflst *inq_eval_parsed_query_top_n (dbinfo *db,
					Char_t *query_text,
					feedback_function_t feedback_function,
					Int_t max_docs_to_retrieve);
belieflst *inq_eval_query_net_top_n (dbinfo *db,
				     IN_node_type *n,
				     feedback_function_t feedback_function,
				     Int_t max_docs_to_retrieve);

belieflst *inq_re_eval_query_top_n (dbinfo *db, Char_t *q, Int_t iterations,
                                    doc_rel_t *docs,
				    feedback_function_t feedback_function,
                                    Int_t max_docs_to_retrieve);

/* added to support Collection Selection databases */
Int_t inq_eval_query_scaled(dbinfo *db, Char_t *q, void **doc_belief_list);

Int_t     inq_external_to_internal_doc_id (dbinfo *db, UChar_t *external_id);
Int_t     inqsrv_external_to_internal_doc_id_list (dbinfo *db, Int_t num_docs,
                                    UChar_t **xid_list, Int_t **iid_list);
Int_t     inqsrv_internal_to_external_doc_id_list (dbinfo *db, Int_t num_docs,
                                    Int_t *iid_list, UChar_t ***xid_list);
Int_t     inq_external_to_internal_doc_id_list (dbinfo *db, Int_t num_docs, 
                                    UChar_t **xid_list, Int_t **iid_list);
Int_t     inq_internal_to_external_doc_id_list (dbinfo *db, Int_t num_docs, 
				    Int_t *iid_list, UChar_t ***xid_list);


Boolean_t inq_free_database_list (Char_t **database_vector);
Boolean_t inq_free_doc (Doc_t *doc);
Boolean_t inq_free_doc_list_fields (Char_t **field_vector);
Boolean_t inq_free_parsed_query(Char_t *qstring);
Boolean_t inq_free_query_net(IN_node_type *n);
Char_t    **inq_get_database_list(dbinfo *db, Char_t *dir_name);
Doc_t     *inq_get_doc (dbinfo *db, Int_t doc_id, Int_t mode,
			UChar_t *parsed_query_string);
/*
**  Added for Tipster - if all I know is the external ID...
*/
Int_t inq_get_doc_by_xid(dbinfo *db, char *doc_xid, Int_t mode,
			 UChar_t *parsed_query_string, Doc_t **doc);


Int_t     inq_get_doc_collection_ident(Doc_t *doc, char **coll_name);
Char_t    *inq_get_doc_field (Doc_t *doc, Int_t field,
			      Int_t *number_of_matches,
			      matchinfo_t **matches);
UChar_t   *inq_get_doc_field_by_name (Doc_t *doc, Char_t *field, 
				    Int_t *num_matches, matchinfo_t **matches);
Int_t     inq_get_raw_doc (Doc_t *docs, UChar_t **text);
Int_t     inq_get_doc_collection_ident(Doc_t *doc, char **coll_name);

Int_t inq_doclist_get_display_fields(dbinfo *db, Int_t doc_ids[],
				     in_field_ids_t *field_ids,
				     Char_t *parsed_query,
				     in_disp_doclist_t **display_fields);

Char_t    **inq_get_doc_list_fields (dbinfo *db,
				     Int_t num_docs,
				     Int_t doc_id[],
				     Int_t num_fields,
				     Int_t field[]);
Int_t     inq_get_doc_by_offset(dbinfo *db, Int_t doc_id, Int_t field,
  	              Int_t offset, Int_t length, char *g_buf, Int_t *result);
Int_t     inq_get_matchinfo(dbinfo *db, Int_t doc_id, char *parsed_query,
		      Int_t mode, Int_t field, matchinfo_t **matches);
Int_t     inq_get_errno (void);
Char_t    *inq_get_error_string (Int_t error_number);
Char_t    *inq_get_modified_parsed_query(dbinfo *db, Char_t *q, Int_t iterations, 
					 doc_rel_t *documents);
Char_t    *inq_get_modified_query (dbinfo *db, Char_t *query_text, Int_t iterations,
				   doc_rel_t *docs);
IN_node_type *inq_get_modified_query_net(dbinfo *db, IN_node_type *n, Int_t iterations, 
					 doc_rel_t *documents);
Char_t    *inq_get_parsed_query(dbinfo *db, Char_t *qstring);
IN_node_type *inq_get_query_net (dbinfo *db, Char_t *qstring);

Int_t inq_get_eval_feedback_on_doc(dbinfo *db, Char_t *query, Int_t docid,
                                   void **query_contrib);
Int_t inq_query_contrib_num_parts(void *qc_ptr, int *num_parts);
Int_t inq_query_contrib_ith_part(void *qc_ptr, int i, char **part_string);
Int_t inq_query_contrib_ith_value(void *qc_ptr, int i, float *contrib_val);
Int_t inq_free_query_contrib(void *qc_ptr);


Int_t     inq_lookup_term(dbinfo *db, Char_t *term);
UChar_t   *inq_internal_to_external_doc_id (dbinfo *db, Int_t internal_id);
Int_t     inq_num_docs_in_collection (dbinfo *db);
Int32_t   inq_num_term_postings_in_collection(dbinfo *db, term_id_type term_id);
Boolean_t inq_opendb (dbinfo *db);
Char_t    *inq_query_net_to_string (dbinfo *db, IN_node_type *n);
belieflst *inq_re_eval_parsed_query (dbinfo *db, Char_t *query_text, Int_t iterations,
				     doc_rel_t *docs, 
				     feedback_function_t feedback_function);
belieflst *inq_re_eval_query (dbinfo *db, Char_t *query_text, Int_t iterations,
			      doc_rel_t *docs, 
			      feedback_function_t feedback_function);
belieflst *inq_re_eval_query_top_n (dbinfo *db, Char_t *q, Int_t iterations, 
				    doc_rel_t *docs,
				    feedback_function_t feedback_function,
				    Int_t max_docs_to_retrieve);
belieflst *inq_re_eval_query_net (dbinfo *db, IN_node_type *n, Int_t iterations,
				  doc_rel_t *docs,
				  feedback_function_t feedback_function);
Boolean_t inq_set_errno (Int_t error_number);

Boolean_t inq_stem_term (dbinfo *db, Char_t *term, Char_t *stemmed_term);
Boolean_t inq_stop_word (dbinfo *db, Char_t *word);
Int32_t inq_term_frequency_in_collection (dbinfo *db, Char_t *term_id);
Int_t  inq_get_ave_doclen (dbinfo *db);
UChar_t *inq_txtu_canonical_word (dbinfo *db, UChar_t * dest, UChar_t *src);
BoolInt_t inq_get_db_stopping(dbinfo *db);
stemmer_t inq_get_db_stemming(dbinfo *db);
Int_t inq_get_passage_size (dbinfo *db);
Int_t inq_get_average_doclen (dbinfo *db);


/* field index table access */
Int_t inq_get_index_table (dbinfo *db, Char_t **tableStr);
Int_t inq_free_index_table(Char_t *tableStr);

Int_t inq_get_field_min_by_name (dbinfo *db, Char_t *field_name,
                                 Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_field_min_by_prefix (dbinfo *db, Char_t *field_prefix,
                                   Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_field_max_by_name (dbinfo *db, Char_t *field_name,
                                 Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_field_max_by_prefix (dbinfo *db, Char_t *field_prefix,
                                   Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_field_name_by_prefix (dbinfo *db, Char_t *field_prefix,
                                    Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_field_description_by_name (dbinfo *db, Char_t *field_name,
                                         Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_field_description_by_prefix (dbinfo *db, Char_t *field_prefix,
                                           Char_t **field_buf, Int_t  *buf_len);
Int_t inq_get_index_field_list (dbinfo *db, Char_t **field_buf, 
                                Int_t *buf_len);
Int_t inq_free_field_buffer (dbinfo *db, Char_t *field_buf);


/* Field description accessor functions */

Int_t inq_get_doc_field_names(Doc_t *doc, Int_t *num_fields, Char_t ***field_names);
Int_t inq_free_doc_field_names(Doc_t *doc, Char_t **field_names);
Int_t inq_get_db_doc_types(dbinfo *db, Int_t *num_types, Char_t ***type_names);
Int_t inq_free_db_doc_types(dbinfo *db, Char_t **type_names);
Int_t inq_get_db_type_field_names(dbinfo *db, Char_t *type_name, 
				  Int_t *num_fields, Char_t ***field_names);
Int_t inq_free_db_type_field_names(dbinfo *db, Char_t **field_names);
Int_t inq_get_db_field_description(dbinfo *db, Char_t *type_name, 
				  Char_t *field_name, FieldDesc **description);
Int_t inq_free_db_field_description(dbinfo *db, FieldDesc *description);


/*
 * DBINFO acess functions:
 * These are to create and set the public fields of the dbinfo
 * data structure.  
 */
dbinfo *inq_new_db(char *dbname);
dbinfo *inq_new_dbinfo(char *dbname, char *outdir, char *stopname,
		       char *relname, float bel, float tf, Int_t batchflag);

void inq_free_dbinfo(dbinfo **db);

Int_t inq_get_db_uid(dbinfo *db, Char_t **uid_str);
Int_t inq_set_db_uid(dbinfo *db, Char_t *uid_str);
Int_t inq_set_db_hostname(dbinfo *db, Char_t *hostname);
Int_t inq_get_db_hostname(dbinfo *db, Char_t **hostname);
Int_t inq_set_db_conn_id(dbinfo *db, Char_t *conn_id);
Int_t inq_get_db_conn_id(dbinfo *db, Char_t **conn_id);

Int_t inq_get_num_top_docs(dbinfo *db);
void inq_set_num_top_docs(dbinfo *db, Int_t num);
Int_t inq_set_db_name(dbinfo *db, char *name);
Int_t inq_set_db_stopname(dbinfo *db, char *stname);
Int_t inq_set_db_relname(dbinfo *db, char *rlname);
Int_t inq_set_db_outdir(dbinfo *db, char *outdir);
void  inq_set_db_output_file(dbinfo *db, char *filename);
Int_t inq_set_batch_mode(dbinfo *db, Int_t mode);
Int_t inq_set_db_tf(dbinfo *db, Float_t newtf);
Int_t inq_set_db_bel(dbinfo *db, Float_t newbel);
Int_t inq_set_db_passage_size (dbinfo *db, Int_t length);
Int_t inq_set_db_docman_ident(dbinfo *db, Char_t *string);
void inq_set_db_stopping(dbinfo *db, BoolInt_t value);
void inq_set_db_stemming(dbinfo *db, stemmer_t stemmer);
void inq_set_db_query_stemming(dbinfo *db, Boolean_t value);
void inq_set_top_docs_only (dbinfo *db, BoolInt_t flag);
void inq_set_db_qbs_flag (dbinfo *db, Boolean_t flag);
Boolean_t inq_change_rf_topnterms (dbinfo *db, Int_t topnterms);
Int_t inq_set_db_concurrency(dbinfo *db, BoolInt_t value);

query_info_t *inq_check_query (dbinfo *db, Char_t *qstring);
void inq_set_spell_check_flag(dbinfo *db, BoolInt_t on_off);
BoolInt_t inq_get_spell_check_flag(dbinfo *db);
void inq_free_query_info(query_info_t **a_query_info);

Boolean_t inq_free_belief_list(belieflst *bl);

/*
** Term Stats Collection API
*/
Int_t
inq_term_stats_collect(dbinfo *db, char *start_term, Int_t num_terms,
                       in_db_stat_t **db_stats);

int inq_db_stat_get_dbname(in_db_stat_t *db_stat, Char_t **db_name);
int inq_db_stat_get_description(in_db_stat_t *db_stat, Char_t **description);

int inq_db_stat_get_inf_entry(in_db_stat_t *db_stat, Char_t *key, Char_t **value);

int inq_db_stat_get_num_terms(in_db_stat_t *db_stat, Int_t *num_terms);
int inq_db_stat_get_Nth_term(in_db_stat_t *db_stat, Int_t indx, Char_t **term,
			     Int_t *term_freq, Int_t *doc_freq, Int_t *type,
			     Int_t *eot);

Int_t inq_read_browse (dbinfo * db, Int_t lines, Char_t *mode, Char_t *search,  Char_t *topic, lines_list_t **window);

int inq_get_db_error_status(dbinfo *db, Int_t *err_code, Char_t **err_string);

/* Document Summary calls */

Int_t inq_open_db_docsum(dbinfo *db, char *docsum_file);
Int_t inq_get_docsum(dbinfo *db, Int_t docid, Char_t **docsum);
Int_t inq_close_db_docsum(dbinfo *db);

/* query based stemming */
Int_t inq_qbs_get_term_variants (dbinfo *db, Char_t *term, terms_t **termlist);


/* Infinder API calls */
Int_t inq_infind_get_concepts(dbinfo *db, Char_t *query, Int_t max, 
			      Char_t ***c_ptr, Int_t *num_concepts);
Int_t inq_infind_free_concepts(Char_t **concepts, Int_t num_concepts);


/* Macros for API consistency */
#define inq_qbs_terms_count               terms_count
#define inq_qbs_terms_free                terms_free

/*!========================================================================
 *!                     Security Subsystem API calls
 *!
 *!======================================================================*/
Int_t inq_sys_login(Login_t **login, location_t *location, Char_t *name, 
		       Char_t *passwd);
Int_t inq_sys_logout(Login_t *login);
Int_t inq_use_security(Login_t *login);
Int_t inq_admin_sys_use_security(Login_t *login, Char_t *sapasswd, 
				    Int_t onoff);
Int_t inq_admin_sys_ignore_passwords(Login_t *login, Char_t *sapasswd, 
				    Int_t onoff);
Int_t inq_admin_sys_add_user(Login_t *login, Char_t *sapasswd,
				Char_t *user, Char_t *passwd);
Int_t inq_admin_sys_delete_user(Login_t *login, Char_t *sapasswd, 
				   Char_t *user);
/*Int_t inqsrv_admin_sys_modify_user(Login_t *login, Char_t *sapasswd, 
				   Char_t *user, Char_t *characteristic, 
				   Char_t *value);*/
Int_t inq_admin_acl_add_access(dbinfo *db, Char_t *user, 
				  Char_t *dbname, Access_t access);
Int_t inq_admin_acl_delete_access(dbinfo *db, Char_t *user,
				     Char_t *dbname);
Int_t inq_admin_acl_modify_user(dbinfo *db, Char_t *user, 
				Char_t *dbname, Access_t access);
Int_t inq_acl_get_user_access(dbinfo *db, Char_t *dbname,
				 Int_t docid, Char_t *field);

/*================== End Security Subsystem Area ===================*/

Int_t inq_term_stats_collect(dbinfo *db, char *start_term, Int_t num_terms,
			     in_db_stat_t **db_stats);

/* Macros For API consistancy: */

#define inq_free_matchinfo            free_matchinfo
#define inq_sort_matches_by_position  sort_matches_by_position
#define inq_rewind_match_list         rewind_match_list
#define inq_get_next_match            get_next_match
#define inq_get_next_type_match       get_next_type_match
#define inq_get_prev_match            get_prev_match
#define inq_get_prev_type_match       get_prev_type_match
#define inq_get_num_matches           get_num_matches
#define inq_get_num_term_matches      get_num_term_matches
#define inq_get_num_passage_matches   get_num_passage_matches

#define inq_next_type_match           get_next_type_match

#define inq_get_match_type    get_match_type
#define inq_get_match_term    get_match_term
#define inq_get_match_passid  get_match_passid
#define inq_get_match_offset  get_match_offset
#define inq_get_match_length  get_match_length
#define inq_get_match_end     get_match_end

#if (defined(__MWERKS__) && defined(__cplusplus))
}
#endif
/*
 *  RCS identifier (included in every object and executable):
 */

static char inq_api_h_rcsid[]="$RCSfile$, $Revision$, $Date$";

#endif










