/***************************************************************************
*                              Copyright (c)                               *
*  Applied Computing Systems Institute of Massachusetts, Inc. (ACSIOM)     *
*                              1990 - 1996.                                *
*                                                                          *
*                          All Rights Reserved.                            *
*                                                                          *
* The INQUERY SOFTWARE was developed at the University of Massachusetts    *
* at Amherst Computer Science Department within the National Center for    *
* Intelligent Information Retrieval (CIIR). For more information, contact  *
* 413-545-0463 or http://ciir.cs.umass.edu.                                *
*                                                                          *
* This SOFTWARE was developed in significant part with private funding.    *
* The INQUERY SOFTWARE is proprietary to ACSIOM and may not be distributed *
* in whole or in part or used in any way without authorized permission in  *
* writing from ACSIOM.  Possession, use, duplication or dissemination of   *
* the INQUERY SOFTWARE and media is authorized only pursuant to a valid    *
* written license from ACSIOM.                                             *
*                                                                          *
* RESTRICTED RIGHTS LEGEND:  Use, duplication, or disclosure by the U.S.   *
* Government is subject to restrictions as set forth in subparagraph       *
* (c)(l)(ii) of the Rights in Technical Data and Computer Software clause  *
* at DFARS 252.227-7013, or in FAR 52.227-19, as applicable.               *
*                                                                          *
*        Contact ACSIOM at (413)545-6311 for more information              *
***************************************************************************/ 

/*
 * "INPARSE_MM_C" is a C file that is the
 *  main module for the SGML-to-transactions parser.
 *
 *  Created:  April 2, 1992.
 */

/*
 *  RCS identifier for module:
 */
static char          rcsid[]="$RCSfile$, $Revision$, $Date$";


#include <stdio.h>
#include "inerror.h"
#include "doctypes.h"
#include "parse-utils.h"
#include "docman.h"
#include "parse_sys.h"



/*
 *  External references:
 */

extern dbinfo *inparse_init(int, char **, Char_t **);
extern void inparse_term(dbinfo *db);

/*
 *  MAIN is a main routine for the SGML-to-transactions parser
 *  (sgml-parse).  The SGML-to-transactions parser is supposed to be
 *  callable, so this module should only contain the code necessary to
 *  convert command-line arguments into paramters for the callable
 *  interface.
 */
int main (int argc, char *argv[])

{
  int status=0;
  dbinfo *db;
  in_Collection coll_ptr;
  Char_t *typename = "sgml";
  Int_t doc_cnt;
 
  db = inparse_init(argc, argv, &typename); 

  if (db == NULL)
      {
      Q_warning("Failed to initialize parsing. ");
      return -1;
      }

  if (typename == NULL)
    typename = "sgml";   /* default is sgml */

  if((status = dm_collection_init(db->docman_handle, db->input_filename, 
			typename, &coll_ptr))
       != INQ_SUCCESS)
  {
      if(status == INQ_NOT_AVAIL)
      {
	  Q_warning("Unknown document format type: \"%s\".  Unable to process data file %s.", typename, db->input_filename);
      }
      else
	  Q_warning("Failed to initialize collection %s. (status=%d)", 
		    db->input_filename,status);
      return -1;
  }


  if(db->in_crash_recovery && db->use_compression)
      status = dm_collection_add_docs(db->docman_handle, coll_ptr, 
				      db->input_filename, 
				      db, INQ_INDEX | INQ_IN_RECOVERY | 
                                      INQ_USE_COMPRESSION);
  else if(db->use_compression)
      status = dm_collection_add_docs(db->docman_handle, coll_ptr, 
				      db->input_filename, 
				      db, INQ_INDEX | INQ_USE_COMPRESSION);
  else  if(db->in_crash_recovery)
      status = dm_collection_add_docs(db->docman_handle, coll_ptr, 
				      db->input_filename, 
				      db, INQ_INDEX | INQ_IN_RECOVERY);
  else
      status = dm_collection_add_docs(db->docman_handle, coll_ptr, 
				      db->input_filename, db, INQ_INDEX);
  
  if(status != INQ_SUCCESS)
  {
      Q_warning("Failed to parse collection %s. (status=%d)", 
		db->input_filename,status);
      return -1;
  }

  inparse_term(db);
  gtr_remove_crashfiles();
  fflush (stdout);

  
  Q_inform("\n\n           S U M M A R Y");
  Q_inform("Parse Status                     = %d", status);
  fflush (stdout);

  exit (status);
 
} /* end of main */


