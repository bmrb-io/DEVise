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
  char *numbytestosend=(char *)malloc(4);
  char mac_name[20];

  int sock;
  
  while(1)
    {
      sock=CreateSocket(9325);
      query_str = (char *)malloc(MAXQUERYLENGTH);    
      bzero(query_str,MAXQUERYLENGTH);

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
   
      blist = inq_eval_query_top_n (db,query_str, (void *)NULL, 100);
      
      if (blist == (belieflst *)NULL) 
	{
	  fprintf (stderr, "The query failed to return anything\n");
	}

      else  
	{
	  fprintf(stdout," Inquery: Query String --> %s \n",query_str);
	  fprintf (stdout, " Documents Retrieved => %d \n \n", blist->doc_cnt);
	
	  for(numdoc=0;(numdoc<100) && (numdoc<blist->doc_cnt);numdoc++)
	    {
	      doc =inq_get_doc (db, blist->list[numdoc].doc_id, HEADING, NULL);
	      /*
		fprintf(stdout,"The retrieved  documents are:\n %s\n", title);	      	      */
	      fprintf(stdout,"The retrieved  docid :\n %d\n", blist->list[numdoc].doc_id);	      	     
	      title = inq_get_doc_field (doc,INQ_DOC_FIELD_ID, NULL, NULL);
	      sprintf(numbytestosend,"%d",strlen(title));
	      WriteToSocket(sock,9325,mac_name,numbytestosend,4);
	      WriteToSocket(sock,9325,mac_name,title,strlen(title));

	      title = inq_get_doc_field (doc,INQ_DOC_FIELD_SOURCE, NULL, NULL);
	      sprintf(numbytestosend,"%d",strlen(title));
	      WriteToSocket(sock,9325,mac_name,numbytestosend,4);
	      WriteToSocket(sock,9325,mac_name,title,strlen(title));

	      title=inq_get_doc_field(doc,INQ_DOC_FIELD_TITLE,NULL,NULL);
	      sprintf(numbytestosend,"%d",strlen(title));
	      WriteToSocket(sock,9325,mac_name,numbytestosend,4);
	      WriteToSocket(sock,9325,mac_name,title,strlen(title));


	      /*
		title=inq_get_doc_field (doc, INQ_DOC_FIELD_TEXT, NULL, NULL); 
	       */
	      
	      inq_free_doc (doc);
	    }
	}
      
      sprintf(numbytestosend,"%d",-1);
      WriteToSocket(sock,9325,mac_name,numbytestosend,4);            
     
      inq_closedb(db);
      inq_term_inquery(db);
      inq_free_dbinfo(&db);
      inq_free_belief_list(blist);
      
      free(query_str);
      CloseSocket(sock); 
    }
  exit(0);
}
