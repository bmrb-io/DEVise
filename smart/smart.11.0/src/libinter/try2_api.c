#include <stdio.h>
#include <stdlib.h>
#include "inq_api.h"
#include "network.h"

#define  MAXQUERYLENGTH 80

int main (void)
{
  
  dbinfo *db;
  belieflst *blist;
  Doc_t *doc;
  char *title;
  int  numdoc;
  char *query_str;
  char *numbytestosend=(char *)malloc(12);
  int sock;
  int doc_id;
  char mac_name[20];


  while(1) 
    {
      query_str = (char *)malloc(MAXQUERYLENGTH);    
      bzero(query_str,MAXQUERYLENGTH);

      sock=CreateSocket(9335);
      IsMessageAvailableOnSocket(sock,0);
      ReadFromSocket(sock,mac_name,20);
      ReadFromSocket(sock,query_str,MAXQUERYLENGTH);

      db = inq_new_dbinfo ("/local.jazz-man/InQuery/sanj/inquery/tests/time",
			   ".",
			   NULL,
			   NULL,
			   0.4,
			   0.4,
			   FALSE);
      
      if (db == (dbinfo *)NULL)
	{
	  fprintf (stderr, "inq_new_dbinfo failed\n");
	  exit (1);
	}
      
      inq_set_db_hostname(db, "jazz-man");
      inq_set_db_conn_id (db, "1234");
   
      if (inq_init_inquery (db))
	{
	  inq_opendb (db);
	}
      
      else 
	{
	  fprintf (stderr, "Failed to establish system Network Connector\n");
	  exit (1);
	}
   
       printf("doc id = %s\n",query_str);

       doc_id = inq_external_to_internal_doc_id(db, query_str);


       doc =inq_get_doc (db, doc_id, HEADING, NULL);

       title=inq_get_doc_field (doc, INQ_DOC_FIELD_TEXT, NULL, NULL); 
       /*
       printf("%d \n", strlen(title));
       */
       printf("%s \n", title);
       sprintf(numbytestosend,"%d",strlen(title));
       WriteToSocket(sock,9335,mac_name,numbytestosend,12);
       WriteToSocket(sock,9335,mac_name,title,strlen(title));

      inq_free_doc (doc);
      
      sprintf(numbytestosend,"%d",-1);
      WriteToSocket(sock,9335,mac_name,numbytestosend,12);            
     
      inq_closedb(db);
      inq_term_inquery(db);
      inq_free_dbinfo(&db);
      
      free(query_str);
      CloseSocket(sock);
      
   }
}
