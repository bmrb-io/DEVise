/****************************************************************************\
*	            Copyright (c) 1990-1995 by the                           *
*    Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*			 All rights reserved.                                *
*	       The INQUERY Software was provided by the                      *
*	 Center for Intelligent Information Retrieval (CIIR),                *
*       University of Massachusetts Computer Science Department,             *
*		       Amherst, Massachusetts.                               *
*	 For more information, contact ACSIOM at 413-545-6311                *
\****************************************************************************/

#include <stdlib.h>

#include "inerror.h"
#include "doctypes.h"
#include "parse-utils.h"
#include "parse_sys.h"
#include "docman.h"
#include "inq_api.h"
#include "lock_inq.h"
#include "huffman.h"
#include "bufman.h"

#define PARSE_MEMORY_USAGE 15.0

dbinfo *
inparse_init(int argc, char *argv[], Char_t **typename)
{
  int status;
  dbinfo *db;
  void *dm_handle;
  void *hfd_handle;
  in_Collection coll_ptr;
  Int_t dm_mode = 0;
  char name[MAX_FILENAME];
  char name2[MAX_FILENAME];

  /* initialize the Buffer Manager */
  if(bufman_init(PARSE_MEMORY_USAGE, BUFMAN_MODE_INDEXING)){
    Q_warning("Unable to Work in Requested Memory (%i MB)", 
	      (int)PARSE_MEMORY_USAGE);
    exit(EXIT_FAILURE);
  }

  if( (db = inq_new_db(NULL)) == (dbinfo *)NULL)
  {
      Q_warning("Failed to create dbinfo structure.");
      exit(1);
  }
  
  /*
   *  Identify the program immediately, because doing so makes it easier
   *  to diagnose errors.
   */
#if !defined(__MWERKS__)
  Q_inform("\nParser System version %s", (char *)parse_version());
#endif

   /* concurrency is always on for parsing since it wastes very little time */
   db->concurrency = TRUE;
  /* 
   *  If the user gets the command-line arguments wrong, remind him of the
   *  proper format, and then abort.
   */
  sr_initialize_table(&db->spec_scanner);

  if (! pu_process_commandline (argc, argv, db, typename))
  {
      pu_usage (argv[0]);
      exit (1);
  }

  /* We better be in write mode */
  db->write_mode = TRUE;
  
  /* Get a unique id */
  status = db_id_create(&(db->db_uid));
  if (status) {
    Q_error("Failed to get a unique id for this database (status = %d)\n",
	    status);
    return NULL;
  }

  /*
   *  Echoing back the parameters is useful when the parser is run in a
   *  batch file (good documentation).  Echoing back the subsystem versions
   *  is helpful for debugging, but the users probably don't care.
   */
#if !defined(__MWERKS__)
  pu_identify_args (db);
  pu_identify_subsystems ();
  Q_inform("");
#endif

  /* Initialize the locking module -- this will provide for concurrency
   * control in non-mneme files.
   */

  if(db->concurrency)
  {
      sprintf((char *)name, "%s", db->name);
      set_extension (name, (Char_t *)"lck");

      if(init_locking((char *)name, &(db->lock_handle)) != 0)
      {
	  Q_warning("Failed to initialize lock file %s.", name);
	  return (dbinfo *)NULL;
      }
  }
  else
      db->lock_handle = NULL;
  

  if(db->use_compression)
  {
    sprintf((char *)name, "%s", db->name);
    set_extension (name, (Char_t *)"hdc");
    sprintf((char *)name2, "%s", db->name);
    set_extension (name2, (Char_t *)"cod");

      if(huff_init_tokenizing(name, name2, &hfd_handle) != INQ_SUCCESS)
      {
        Q_warning("Failed to initialize Huffman tokenizer");
        return (dbinfo *)NULL;
      }
  }

  else hfd_handle = (void*)NULL;

  /*
   *  Call the parser.  Output is flushed after each call to ensure that
   *  the log file reflects the current state of the program.
   */
  SET_FLAG(dm_mode, DM_READ_WRITE);

  if(db->provide_recovery)
      SET_FLAG(dm_mode, DM_PROVIDE_RECOVERY);
  
  if(dm_initialize(db->docman_ident, dm_mode, db->lock_handle,
		   hfd_handle, &dm_handle) != INQ_SUCCESS)
  {   
       {Q_warning("Failed to initialize Document Manager %s", db->docman_ident);
        return (dbinfo *)NULL;}
  }
  
  db->docman_handle = dm_handle;

  db->hfd_handle = hfd_handle;


#if !defined(__MWERKS__)
  fflush (stdout);
#endif

  return db;
}

void 
inparse_term(dbinfo *db)
{
    if(db == (dbinfo *)NULL)
	return;

    if (sr_free_table(db->spec_scanner) != INQ_SUCCESS)
       Q_warning("Failed to free scanner table.");

    if(dm_terminate(db->docman_handle) != INQ_SUCCESS)
	Q_warning("Failed to terminate Document Manager.");

    if(db->lock_handle != NULL)
	term_locking(&db->lock_handle);
    
    return;
}

