#include <stdio.h>
#include "inq_api.h"

int main (void)
{
dbinfo *db;
belieflst *blist;
Doc_t *doc;
char *title;
int  numdoc;

/*==============================================================================

This example assumes that the database is stored in directory
/usr/users1/inquery/data/cadcm_dots. It also assumes that your server's name
is "canberra", and the connection to the server is via TCP/IP port number
1200. The output is to be written to the current working directory ("."). No
relevance file is provided. The default term frequency and belief values are
0.4. The program is not being run in batch mode.
*============================================================================ */

db = inq_new_dbinfo ("/local.jazz-man/InQuery/sanj/inquery/tests/time",
        ".",
        NULL,
        NULL,
        0.4,
        0.4,
        FALSE);
if (db == (dbinfo *)NULL) {
        fprintf (stderr, "inq_new_dbinfo failed\n");
        exit (1);
}

/*==========================================================================

Define the Connection host and port number.

============================================================================ */

inq_set_db_hostname(db, "jazz-man");
inq_set_db_conn_id (db, "1234");

/*===========================================================================

Open a connection to the host specified. If the connection is successful
open the INQUERY database.

=========================================================================== */

if (inq_init_inquery (db)) {
inq_opendb (db);
}

/*==========================================================================

Otherwise, print a warning and return the exit code of 1.

============================================================================
*/

else {
        fprintf (stderr, "Failed to establish system Network Connector\n");
        exit (1);
}

/*==========================================================================

The query is "operating systems". No feedback function for display of
processing progress is defined. The system is instructed to return 100
documents or less.

===========================================================================
*/

blist = inq_eval_query_top_n (db, "time tested ", (void *)NULL, 100);

/*=========================================================================

Check that something was found.

===========================================================================
*/

if (blist == (belieflst *)NULL) {
        fprintf (stderr, "The query failed to return anything\n");
        exit (0);
}
fprintf (stdout, "%d documents were retrieved\n", blist->doc_cnt);

/*============================================================================

Get the top (0th) ranked document. The document text fields are placed in a
Doc_t data structure. This structure is then accessed so that text may be
displayed. For this example, only the TITLE field of the retrieved document
is displayed. It is obtained by getting the HEADING (a predefined set of
components. It could also be obtained singly.

No highlighting information is required, so the parsed query argument is
defined as NULL.

=============================================================================
*/

for(numdoc=0;numdoc<100;numdoc++){
doc = inq_get_doc (db, blist->list[numdoc].doc_id, HEADING, NULL);

/*===========================================================================

Extract the title field from the Doc_t structure. The title field is one of
the components of the predefined HEADING document component.

Since no highlighting information was requested in the inq_get_doc() call,
the number of matches and matchinfo_t pointer arguments are left as NULL.

=============================================================================
*/

/* title = inq_get_doc_field (doc, INQ_DOC_FIELD_TITLE, NULL, NULL); */
title = inq_get_doc_field (doc, INQ_DOC_FIELD_TITLE, NULL, NULL); 
fprintf (stdout, "The most highly ranked document is:\n %s\n", title);
title = inq_get_doc_field (doc, INQ_DOC_FIELD_TEXT, NULL, NULL); 
fprintf (stdout, "The most highly ranked document is:\n %s\n", title);
}

/*===========================================================================

We're finished with the document, so free the Doc_t data structure.

=============================================================================
*/

inq_free_doc (doc);

/*=========================================================================

Close the INQUERY network connection and database. Free data structures.

===========================================================================
*/

inq_closedb(db);
inq_term_inquery(db);
inq_free_dbinfo(&db);
inq_free_belief_list(blist);
exit(0);
}
