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
 * "DBINFO_H" is a header file for 
 * the primary data-structure of the INQUERY API. 
 * It is defined below and accessor functions are prototyped.
 */

#if !defined (DBINFO_H)
#define DBINFO_H

/************/
/* INCLUDES */
/************/

#include "doctypes.h"
#include "stemmer.h"
#include "hashobj.h"
#include "clisrv_seckey.h"
#include "spec_scan.h"
#include "language.h"
#include "docsum.h"
#include "db_ids.h"

/*********/
/* TYPES */
/*********/

#define CACHE_TERM_LEN 16
#define MAX_NUM_OF_FTAG            899

typedef struct hslot {
 char word[CACHE_TERM_LEN];
 Int32_t tf;
 Int32_t df;
} MFT_HSLOT;

typedef struct {
  Char_t name[128];
  Char_t description[256];
  Char_t first[128];  
  Char_t last[128];
  char   field_index;   /* From the dm_trans_tab entry */
} indexTableEntry;

struct ct_stat
  {
   char     term[CACHE_TERM_LEN+1];
   Int_t     tf;
   Int_t     df;
};

typedef struct ct_stat CtStat_t;

enum db_type {STANDARD = 1, COLLECTION_SELECTION};
typedef enum db_type db_type_t;

typedef struct                    /* database information structure */
{
  char name[MAX_FILENAME];      /* [path/]name of db */
  char stopname[MAX_FILENAME];  /* [path/]name of stoplist */
  char input_filename[MAX_FILENAME]; /* pathname of the input file */
  char output_filename[MAX_FILENAME]; /* pathname of the output file */
  FILE *inf_file;		/* file ptr for inf file */
  FILE *mft_file;		/* the most-frequent term file */
  FILE *dlt_file;               /* delete doc list */
  FILE *input_file;              
  
  void *btl_handle;             /* handle for the btl il-db */
  void *ndx_handle;             /* handle for the ndx il-db */
  void *inf_handle;		/* handle to the in-memory inf table */
  void *stm_handle;             /* handle for the stemfile st_handle_t */
  Char_t *docman_ident;         /* Document Manager identifier */
  void *docman_handle;          /* handle to the Document Manager */
  void *hfd_handle;		/* handle to the huffdict */
  void *lock_handle;		/* File descriptor for lock file */
  docsum_t *docsum_handle;
  
  /* client/server fields */
  Char_t *dbname;		/* actual database pathname */
  Char_t *hostname;		/* server hostname */
  Char_t *conn_id;		/* connection id */
  Int32_t  db_id;		/* database id */
  void *comm_link;              /* connection information */

  Int_t batch;			/* true if processing in batch mode */
  Int_t write_mode;             /* true if we can change the database files */
  Int_t doc_cnt;		/* number of docs in db */
  Int_t word_cnt;		/* number of word occurrences in db */
  Int_t collection_max_tf;	/* frequency of most frequent term */
  Int_t collection_ave_doclen;	/* average document length in terms */

  Int_t max_doc_id_fields;      /* max doc id with fields */
  Int_t max_doc_id_parse;       /* max_doc id for parsing */
  Int_t max_doc_id_filter;      /* max doc id after merge */
  Float_t tf;			/* default tf */
  Float_t bel;			/* default belief */
  BoolInt_t boolean_retrieval;	/* true if INQUERY should do Boolean IR */
  Boolean_t stops_removed;	/* true if stop words were removed */
  Boolean_t stemming_on;        /* true for query-based stemming */
  stemmer_t stemmer;	        /* the stemmer */
  Boolean_t preserve_case;	/* true if case is preserved */
  Boolean_t use_fields;		/* true if fields are indexed */
  Boolean_t use_compression;	/* true if the collection is compressed */
  ScanInfo_t *spec_scanner;       /* Table of special scanners used */
  Boolean_t use_db_scanner;	/* true if the db-scanner should be run */
  Boolean_t build_infind;       /* true if infinder dbs should be built */
  Int_t       default_passage_size; /* default number of words in a passage */
  Boolean_t inc_xids;           /* if true, include xids in mdb doc structures */

  /* Stuff djp added when cleaning up globals */

  UChar_t *docbuff;		/* doc buffer */
  Boolean_t spell_check_flag;	/* true if spelling is to be checked */
  Boolean_t qbs_on;             /* true if query-based-stemming is on */
  stemmer_t qbs_stemmer;        /* to temporarily store the stemmer type */
  Int_t maxRecSize;		/* larget record length in keyfile */
  BoolInt_t highlighting_fix;	/* ??? */
  MFT_HSLOT *mftc;		/* most frequent term cache */
  void *btl_fcb;	 /* fcb for belief file */
  void *ndx_fcb;    /* fcb for field index */
  Char_t ndxFileName[MAX_FILENAME]; /* name of ndx file */
  Char_t GenFtagFlags[MAX_NUM_OF_FTAG];
  indexTableEntry *indexTable;
  Int_t indexCount;
  Int_t maxIndexRecSize;
  Char_t *currIndexPrefix;
  UChar_t *encodedBuff;		/* the encoded list of
				   offset-length pairs */
  float max_idf;
  float belief_baseline;
  float belief_factor;
  double tf_baseline;
  double tf_factor;
  hashobj *stop_ht;

  void *browse_list;

  Int_t err_code;
  Char_t *err_string;              /* used primarily for client-server */

  BoolInt_t top_docs_only;
  Int_t num_top_docs;              /* Used for Top Docs Optimization       */
                                   /* If > 0, optimization is ON, else OFF */
  BoolInt_t concurrency;           /* TRUE if we have concurrency control */
  Int_t usage_stats;               /* TRUE if reporting usage_stats */
  Boolean_t use_feedback;          /* Tell the server to use feedback */
  Int_t  provide_recovery;         /* TRUE if providing recovery */
  Int_t  in_crash_recovery;         /* TRUE if in crash recovery */
  lang_encoding_t language;         /* language and character encoding */
  db_type_t dbtype;                /* type of db, presently used for
				      collection selection */
  stemmer_t *list_of_stemmers;     /* used for collection selection */
  Boolean_t parse_use_locs;        /* FALSE if no locations are indexed */
  SecKey_t *key;                   /* see clisrv_seckey.h for desc */
  db_id_t *db_uid;                 /* unique id of this database */
} dbinfo;



/***********************/
/*       MACROS        */
/***********************/

#define dbinfo_num_words_in_collection(db) (db->word_cnt)
#define SET_CURRENT_DBS(db) current_db = db; set_incalc_db(db)

#define CONCURRENCY_DEFAULT  FALSE

/***********************/
/* FUNCTION PROTOTYPES */
/***********************/
/* these are the functions which are not part of the api. */

int inq_reset_db_error_status(dbinfo *db);
int inq_set_db_error_status(dbinfo *db, Int_t err_code, Char_t *err_string);

Int_t read_inf_to_db(dbinfo *db);
Int_t read_inf_save_to_db(dbinfo *db);
Int_t update_db_from_inf(dbinfo *db);
Int_t write_inf_from_db(dbinfo *db);
Boolean_t open_inf_rw(dbinfo *db);
int inq_set_db_usage_stats(dbinfo *db, int on_off);
Int_t remove_inf_save(dbinfo *db);
Int_t write_inf_save_from_db(dbinfo *db);
#endif

