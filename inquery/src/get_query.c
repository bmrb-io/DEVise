#include <stdio.h>
#include <stdlib.h>
#include "inq_api.h"
#include "network.h"

#define  MAXQUERYLENGTH 80
#define  INQ_Q_SOCKET   9654

void get_query(int sock);

int main (void)
{
  int sock,newsock;

  sock=CreateServerTCPSocket(INQ_Q_SOCKET);

  while(1)
  {
      IsMessageAvailableOnSocket(sock,0);
      newsock = AcceptConnectOnTCPSocket(sock);
      
      if(fork()==0) 
      {
           close(sock);
           get_query(newsock);
           exit(0);
      }

      close(newsock);
  }
}

/* 
  needs to be modified if any of these are changed and recompiled
     has hard coded info about the
                machine-name : jazz-man,
                port-number  : 1234
                database-name: /local.jazz-man/InQuery/sanj/inquery/tests/time     
     returns a max of 100 matches - can be changed easily 
 */

void get_query(int sock)
{
  
  dbinfo *db;
  belieflst *blist;
  Doc_t *doc;
  char *title;
  int  numdoc;
  char *query_str;
  char *numbytestosend=(char *)malloc(4);
  char mac_name[20];
  int nbytestoexpect=0;


      query_str = (char *)malloc(MAXQUERYLENGTH);
      bzero(query_str,MAXQUERYLENGTH);

      ReadFromTCPSocket(sock,query_str,12);
      nbytestoexpect = atoi(query_str);
      printf("nbytestoexpect = %d\n",nbytestoexpect);
      bzero(query_str,MAXQUERYLENGTH);
      ReadFromTCPSocket(sock,query_str,nbytestoexpect);

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
          char  current_answer[20000];
          bzero(current_answer,20000);
          current_answer[0] = '\0';

	  fprintf(stdout," Inquery: Query String --> %s \n",query_str);
	  fprintf (stdout, " Documents Retrieved => %d \n \n", blist->doc_cnt);
	
	  for(numdoc=0;(numdoc<100) && (numdoc<blist->doc_cnt);numdoc++)
	    {
	      doc =inq_get_doc (db, blist->list[numdoc].doc_id, HEADING, NULL);
	      /*
		fprintf(stdout,"The retrieved  documents are:\n %s\n", title);	      	      */
	      fprintf(stdout,"The retrieved  docid :\n %d\n", blist->list[numdoc].doc_id);	      	     
              /* -> to one big write
	      title = inq_get_doc_field (doc,INQ_DOC_FIELD_ID, NULL, NULL);
	      sprintf(numbytestosend,"%d",strlen(title));
	      WriteToTCPSocket(sock,numbytestosend,4);
	      WriteToTCPSocket(sock,title,strlen(title));

	      title = inq_get_doc_field (doc,INQ_DOC_FIELD_SOURCE, NULL, NULL);
	      sprintf(numbytestosend,"%d",strlen(title));
	      WriteToTCPSocket(sock,numbytestosend,4);
	      WriteToTCPSocket(sock,title,strlen(title));

	      title=inq_get_doc_field(doc,INQ_DOC_FIELD_TITLE,NULL,NULL);
	      sprintf(numbytestosend,"%d",strlen(title));
	      WriteToTCPSocket(sock,numbytestosend,4);
	      WriteToTCPSocket(sock,title,strlen(title));
               */

	      title = inq_get_doc_field (doc,INQ_DOC_FIELD_ID, NULL, NULL);
              strcat(current_answer,title);
	      title = inq_get_doc_field (doc,INQ_DOC_FIELD_SOURCE, NULL, NULL);
              strcat(current_answer,title);
	      title = inq_get_doc_field(doc,INQ_DOC_FIELD_TITLE,NULL,NULL);
              strcat(current_answer,title);

	      /*
		title=inq_get_doc_field (doc, INQ_DOC_FIELD_TEXT, NULL, NULL); 
	       */
	      
	      inq_free_doc (doc);
	    }
	      sprintf(numbytestosend,"%d",strlen(current_answer));
	      WriteToTCPSocket(sock,numbytestosend,12);
              printf("%% %s \n",current_answer);
	      WriteToTCPSocket(sock,current_answer,strlen(current_answer));
 
	}
      
      sprintf(numbytestosend,"%d",-1);
      WriteToTCPSocket(sock,numbytestosend,12);            
     
      inq_closedb(db);
      inq_term_inquery(db);
      inq_free_dbinfo(&db);
      inq_free_belief_list(blist);
      
      free(query_str);
      CloseSocket(sock); 
}
