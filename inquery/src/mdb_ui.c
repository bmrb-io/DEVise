/*
 * Example INQUERY Multi-Database Interface illustrating use of INQUERY
 * multi-database API functions.
 *
 * Compile this code using the following commandline::
 *
 *   gcc -DINQ_ARCH_alpha -I$PROJECT/3.0/h -L$PROJECT/3.0/ib/alpha \
 *       -o mdb_ui mdb_ui.c -linqtcp
 * 
 * or use the gmake file inquery.make and "gmake -f inquery.make mdb_ui".
 *
 * Note that this compilation is using gcc on an DEC alpha machine.  The
 * architecture value may or may not need to be defined depending on your
 * environment.  Other possibilities are INQ_ARCH_solaris, INQ_ARCH_mips
 * INQ_ARCH_rs6000 or INQ_ARCH_sun4 in the UMASS/CIIR development environment.
 *
 * Location of header files (-I) or libraries (-L) will vary by site but 
 * refer to the INQUERY release header files and library locations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "inq_api.h"
#include "inqmdb_api.h"
#include "inq_err.h"
// #include "inq_feedback.h"
#include "inq_feedback_info.h"
#include "net_ui.h"

#if defined(INQ_ARCH_sun4)
#define strerror(a) ""
#endif

/* define the protocol we are using */
#define DECNET 1
#define TCPIP  2
#define LOCAL  3

#define med_query "Hhs Secretary Louis Sullivan said in a statement that the \
numbers disclosed a continuing but alarming trend. Steep"

#define big_query "Hhs Secretary Louis Sullivan said in a statement that the \
numbers disclosed a continuing but alarming trend. Steep \
increases in health costs are limiting access to health care \
and imposing a heavy burden on individuals, industries and \
government, he said. \
   It doesn't come as a surprise, said Gail Wilensky, \
administrator of the department's Health Care Financing \
Administration. It shows our continuing inability to reduce \
high rates of spending on health care. \
   In 1989, Americans spent $530.7 billion on personal \
health-care services directly benefiting the patient -- an \
increase of 10.6% from 1988. The U.S. inflation rate during \
the same period was only 4.8%."

location_t loc_broker;

void usage(char *progname)
{

  fprintf(stderr,"\nUSAGE:  %s [<hostname>] [<portnum>]\n\n", progname);
  fprintf(stderr,"\t <hostname> is the name of the host running a Connection Server\n");
  fprintf(stderr,"\t <portnum> is necessary only if you require a non-default port\n\n");
  return;
}

int process_commandline(int argc, char *argv[])
{
  int i, args_left;
  char *arg;

  if(argc < 2)
    return(0);

  if (!strcmp(argv[1],"-h"))
    return(-1);

  memset((void *) &loc_broker,'\0',sizeof(location_t));

  loc_broker.hostname = (char *)malloc(strlen(argv[1]) + 1);
  strcpy(loc_broker.hostname, argv[1]);

  if (argc > 2) {

    loc_broker.connection_id = (char *)malloc(strlen(argv[2]) + 1);
    strcpy(loc_broker.connection_id, argv[2]);
  }
  return(0);
}


int main (int argc, char *argv[]) {
  void *session_handle;
  in_qry_handle_t *query_info;
  in_belief_list_t *doc_list, *rf_doc_list;
  in_field_ids_t *doc_fields;

  Doc_t *curr_doc;

  void  *avail_dbs=(void *)NULL;
  void  *active_dbs=(void *)NULL;

  char  tmp_input[100], ignore;
  char  *path, *dbname;
  char  *id, *title, *text;
  char  yorn[5];

  int   element, num_docs, num_avail_dbs;
  int   status, i, rel_count=0;
  int   matches=0;
  int   total_hits=0;

  lines_list_t *browse_list;
  in_belief_elt_t mdb_curr_doc;
  FILE *outfile;

  if(process_commandline(argc, argv)){
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  outfile=stdout;

#ifdef DEBUG
  if ((outfile=fopen("debug.log","w")) == NULL)
  {
    fprintf(stderr,"Failed to open file, debug.log\nSystem error: %s\n",
	    strerror(errno));
    exit(1);
  }
#endif

  

  /*------------------------------------------------------*
   *  Your UI initialization will go here                 *
   *   - part of the initialization MUST fill in the      *
   *     "hostname" field of "loc_broker".                *
   *   - the location_broker defines the location of the  *
   *     machine (name and port number) running the       *
   *     INQUERY connection server the client wishes to   *
   *     connect to.                                      *
   *------------------------------------------------------*/

  /*-------------------------------------------------------*
   *  Define the machine name running the INQUERY servers. *
   *                                                       *
   *  Here we are using a host name of "hobart".           *
   *-------------------------------------------------------*/
  if (!loc_broker.hostname)
    loc_broker.hostname = "hobart";

  /*-------------------------------------------------------*
   *  Define the protocol to use when connecting to        &*
   *  the INQUERY servers.                                 *
   *                                                       *
   *  Here we are using the TCPIP protocol                 *
   *    validate protocols are in 
   *-------------------------------------------------------*/
  loc_broker.protocol = TCPIP;      

  /*--------------------------------------------------------* 
   * The port number defined for client connection          *
   * (-client-portnum commandline switch) when invoking the *
   * connection server.  If not defined, the default value  *
   * will be used.                                          *
   *                                                        *
   * Here we use the default by leaving undefined the       *
   * location broker connection_id field.                   *
   *--------------------------------------------------------*/
  if (!loc_broker.connection_id)
    loc_broker.connection_id = "1234";


  /*------------------------------------------------------*
   *  Establish a connection to the host specified in     *
   *   loc_broker                                         *
   *------------------------------------------------------*/
  if (inq_mdb_init_inquery (&session_handle, &loc_broker)) {
    fprintf (stderr,"\tERROR: Failed to initialize INQUERY SESSION\n");
     exit (EXIT_FAILURE);
  }

  /*------------------------------------------------------*
   *  Get a list of available databases &                 *
   *      Print the list to the stderr                    *
   *------------------------------------------------------*/
  path = getenv("INQDATA");

  if (inq_mdb_set_avail_dbs (session_handle, path, &avail_dbs)) {
    fprintf (stderr,"\tMDB_SET_AVAIL_DBS: Error\n");
  }

  inq_mdb_name_list_num_names (avail_dbs, &num_avail_dbs);
  fprintf (stderr,"\n\n");
  for (i=0; i < num_avail_dbs; i++){
    inq_mdb_name_list_get_name (avail_dbs,i, &dbname);
    fprintf (stderr,"%2ld.) %-s\n", i+1, dbname);
  }

  /*------------------------------------------------------*
   *  Create a name list of databases to be set           *
   *     as active in the session handle                  *
   *------------------------------------------------------*/
  if (!active_dbs) {
    if (inq_mdb_name_list_create (&active_dbs)) {
      fprintf (stderr,"\tERROR: DBLIST_CREATE\n");
    }
  }

  element = 0;
  while (element >= 0) {

    fprintf (stderr, "Select an active database ID (-1 to quit): ");
    fscanf (stdin, "%i", &element);

    if (element < 0)
      break;

    inq_mdb_name_list_get_name (avail_dbs, element-1, &dbname);

    if (inq_mdb_name_list_add_name (active_dbs, dbname)){
      fprintf (stderr,"ERROR: dblist_add_name \n");
    }
  }

  if (inq_mdb_set_active_dbs (session_handle, active_dbs)){
    fprintf (stderr,"ERROR: inqmdb_set_active\n");
  }

  inq_mdb_name_list_free (&active_dbs);

  /*------------------------------------------------------*
   * Now that the active databases are set, we            *
   *     can evaluate a query                             *
   *------------------------------------------------------*/
  ignore = getchar ();
  fprintf (stderr,"Enter your query here :  ");
  gets (tmp_input);
  fprintf (stderr,"Enter the number of documents to collect :  ");
  scanf ("%i", &num_docs);

  /* inq_mdb_get_browse_list(session_handle, &browse_list);*/

  /* inq_mdb_set_feedback_function(session_handle, inq_feedback, NULL); */

  if (inq_mdb_query_eval (session_handle, tmp_input /* query */, 
                          num_docs, 0, &doc_list, &query_info)) {

    fprintf (stderr,"\tINQMDB_EVAL_QUERY:  FAILED\n");
    exit (EXIT_FAILURE);
  }

  /*------------------------------------------------------*
   * Now get summary data for the returned document list  *
   *                                                      *
   *  1. Create a field id list specifying which fields   *
   *     to include in the summary.                       *
   *                                                      *
   *  2. Get summary information for the doc list         *
   *                                                      *
   *------------------------------------------------------*/
  inq_mdb_field_ids_create (&doc_fields, 5);
  inq_mdb_field_ids_add_field (doc_fields, INQ_DOC_FIELD_ID);
  inq_mdb_field_ids_add_field (doc_fields, INQ_DOC_FIELD_SOURCE);
/*  inq_mdb_field_ids_add_field (doc_fields, INQ_DOC_FIELD_TEXT); */
  inq_mdb_field_ids_add_field (doc_fields, INQ_DOC_FIELD_TITLE);

   if (inq_mdb_doc_list_set_fields (session_handle, doc_fields, doc_list)) {
     fprintf (stderr,"\tINQ_MDB_DOC_LIST_SET_FIELDS: Error\n");
   }

  /*------------------------------------------------------*
   *  Display the retrieved docs                          *
   *------------------------------------------------------*/
  inq_mdb_doc_list_num_elts (doc_list, &num_docs);

  inq_mdb_doc_list_get_total_hits(doc_list, &total_hits);

  fprintf(outfile, "TOTAL Number of hits: %d\n", total_hits);


  for (i=0; i<num_docs; i++) {

    /* get the info on retrieved docs...allocate space for the mdb_doc_t */
    status = inq_mdb_doc_list_get_info (doc_list, i, &mdb_curr_doc);

    if (status != INQ_MDB_SUCCESS) {
      fprintf (stderr,"\tINQ_MDB_DOC_LIST_GET_INFO: Error\n");
      continue;
    }

    fprintf (outfile, "DB ID: %d    Doc ID: %d    Rank: %d    Belief: %f\n",
                     mdb_curr_doc.db_id, mdb_curr_doc.doc_id, i+1, 
                     mdb_curr_doc.belief);


    /*
     * NOTE CONCERNING PRINTING DOCUMENT TEXT
     *   The API call, "inq_mdb_doc_list_set_fields()" should only be used 
     *   to set summary information as it will fill in the document list with
     *   the text for ALL the fields set, for ALL the documents returned in 
     *   the document list.
     *
     *   To view the full document text, the recommended procedure is:
     *
     *     Call "inq_mdb_doc_list_get_doc()"
     *
     *     then use one of:
     *
     *       inq_get_doc_field()  OR
     *       inq_get_doc_field_by_name()  OR
     *       inq_get_raw_doc()
     *
     *   This latter procedure will be illustrated to view documents retrieved
     *   during a relevance feedback loop.
     */

    /* print the ith ranked document ID */
    status = inq_mdb_doc_list_get_field (doc_list, i, INQ_DOC_FIELD_ID, &text);
    if ((status == INQ_MDB_SUCCESS) && text)
    {
      fprintf (outfile, "\tID   : %s\n", text);
      free(text);
      text = (char *)NULL;
    }
    else
      fprintf (outfile, "\tID   : NO ID\n");


    /* print the ith ranked document title */
    status = inq_mdb_doc_list_get_field (doc_list, i, INQ_DOC_FIELD_TITLE, &text);

    /* print title field */
    if ((status == INQ_MDB_SUCCESS) && text)
    {
      fprintf (outfile, "\tTITLE: %s\n", text);
      free(text);
      text = (char *)NULL;
    }
    else
      fprintf (outfile, "\tTITLE: NO TITLE\n");

    /* print the ith ranked document text */
    status = inq_mdb_doc_list_get_field (doc_list, i, INQ_DOC_FIELD_TEXT, &text);

    /* print text field */
    if ((status == INQ_MDB_SUCCESS) && text)
    {
      fprintf (outfile, "\tTEXT : %s\n\n\n", text);
      free(text);
      text = (char *)NULL;
    }
    else
      fprintf (outfile, "\tTEXT : NO TEXT\n\n\n");

    fflush(outfile);

    /*----------------------------------------------------*
     *  Mark the relevant documents, and re-evaluate      *
     *       the query                                    *
     *----------------------------------------------------*/

    yorn[0] = '\0';
    ignore = getchar ();
    fprintf (stderr,"\n\tWas this document relevant [y/n]? ");
    scanf("%c", yorn);

    if (yorn[0] == '\0')
      break;

    if (tolower(yorn[0]) == 'y') {

      if (inq_mdb_doc_list_mark_relevant (doc_list, i))
        fprintf (stderr,"\tERROR: inq_mdb_doc_list_mark_relevant\n");
      else
        rel_count++;
    }

  }



  /*----------------------------------------------------------*
   * Do relevance feedback with the relevance marked doc_list *
   *----------------------------------------------------------*/
  rf_doc_list = NULL;

  if (rel_count > 0 )
    if (inq_mdb_query_eval_rf (session_handle, query_info, doc_list,
                               num_docs, 0, &rf_doc_list)) {
      fprintf (stderr,"\tNO DOCS: inq_mdb_query_eval_rf\n");
      exit (EXIT_FAILURE);
    }

  /*-------------------------------------------------------*
   * Print the results from the relevance feedback search. *
   * This code is just a repeat of what was done above.    *
   *-------------------------------------------------------*/
  inq_mdb_doc_list_num_elts (rf_doc_list, &num_docs);

  fprintf(outfile, "TOTAL Number of hits: %d\n", total_hits);

  for (i=0; i<num_docs; i++) {

    /* get the info on retrieved docs...allocate space for the mdb_doc_t */
    status = inq_mdb_doc_list_get_info (rf_doc_list, i, &mdb_curr_doc);

    if (status != INQ_MDB_SUCCESS) {
      fprintf (stderr,"\tINQ_MDB_DOC_LIST_GET_INFO: Error\n");
      continue;
    }

    fprintf (outfile, "DB ID: %d    Doc ID: %d    Rank: %d    Belief: %f\n",
                     mdb_curr_doc.db_id, mdb_curr_doc.doc_id, i+1, mdb_curr_doc.belief);

    status = inq_mdb_doc_list_get_doc ((void *)session_handle, rf_doc_list, i, 
                                       (void *)query_info, ALL, 
                                       (Doc_t **)&curr_doc);

    if (status != INQ_MDB_SUCCESS) {
      fprintf (stderr, "Failure getting %dth document\n", i);
      continue;
    }

    id = inq_get_doc_field_by_name (curr_doc, (Char_t *)"DOCID", (Int_t *)&matches, 
                                    (matchinfo_t **)NULL);
    title = inq_get_doc_field_by_name (curr_doc, (Char_t *)"TITLE", (Int_t *)&matches,
                                       (matchinfo_t **)NULL);
    text = inq_get_doc_field_by_name (curr_doc, (Char_t *)"TEXT", (Int_t *)&matches,
                                      (matchinfo_t **)NULL);

    if (id)
      fprintf (outfile, "\tID : %s\n", id);
    else
      fprintf (outfile, "\tID : NO ID\n");

    if (title)
      fprintf (outfile, "\tTITLE: %s\n", title);
    else
      fprintf (outfile, "\tTITLE: NO TITLE\n");

/********************
    if (text)
      fprintf (outfile, "\tTEXT: %s\n\n\n", text);
    else
      fprintf (outfile, "\tTEXT: NO TEXT\n\\n\n");
 *******************/

    fprintf (stderr, "\n\nHit any key to Continue");
    gets (yorn);

    inq_free_doc (curr_doc);
  }

  /*------------------------------------*
   *  CLEAN UP & Terminate our session  *
   *------------------------------------*/
  if (avail_dbs)
    inq_mdb_name_list_free (&avail_dbs);

  if (active_dbs)
    inq_mdb_name_list_free (&active_dbs);

  if (doc_list)
    inq_mdb_doc_list_free (&doc_list);

  if (rf_doc_list)
    inq_mdb_doc_list_free (&rf_doc_list);

  if (doc_fields)
    inq_mdb_field_ids_free (&doc_fields);

  if (inq_mdb_term_inquery (&session_handle)){
    fprintf (stderr,"\tERROR: Failed to terminate INQUERY SESSION\n");
    exit (EXIT_FAILURE);
  }

  if (session_handle) {
    fprintf (stderr,"\tWARNING: Session Handle NON_NULL\n");
  }

  fprintf (outfile, "MDB Example UI Done\n");

  exit (EXIT_SUCCESS);

}  /* end mdb_ui */
