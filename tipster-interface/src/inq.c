#include "inq.h"
#define   MAXWORDLEN  12
#define   MAXQUERYLEN 80
#define   INQ_QUERY_SOCKET   9654 
#define   INQ_FETCH_SOCKET   9664 

#define   STOREINFILE(f,r,a)     fprintf(f,"%d %s ",r,a);
#define   STORESTRINGFILE(f,r)   fprintf(f,"%s ",r);
#define   INSERTEOLN(f)          fprintf(f,"\n");

#define   CHECKFORBL(s,num)        if (s[num-1] == '\n')\
	                            s[num-1] = '\0';\
                                   else \
	                            s[num] = '\0';\


#define   SKIPBLANKS(s)           if (s[0] == '\n') s ++; \
                                  i=0; \
                                  while(s[i++]==' ') s ++; \

int inqQuery(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
 
  int  sock,numbytestoexpect,rank,i;
  char *str,*answer,*ans2;
  FILE *fp;

  /* the initialization part */
  rank=1;
  str = (char *)malloc(MAXQUERYLEN);
  sock=CreateClientTCPSocket(INQ_QUERY_SOCKET,inquery_mac_name);

  /* send the query to the server */
  strcpy(str,argv[1]);

  answer = (char *)malloc(MAXWORDLEN);
  bzero(answer,MAXWORDLEN);
  sprintf(answer,"%d",strlen(str));
  WriteToTCPSocket(sock,answer,MAXWORDLEN);
  free(answer);

  WriteToTCPSocket(sock,str,strlen(str));

  /* the file where the retrieved data is to be stored */
  fp = fopen(INQ_TMPFILE,"w");

  fprintf(stdout,"Retrieving documents");
  fflush(stdout);

  while(1)
    {
      IsMessageAvailableOnSocket(sock,0);
      
      /* get the field - DOCID */
      answer = (char *)malloc(MAXWORDLEN);
      if (ReadFromTCPSocket(sock,answer,MAXWORDLEN)< 0) break;
      numbytestoexpect = atoi(answer);
      free(answer);
      printf("%d ",numbytestoexpect);
      fflush(stdout);
      if (numbytestoexpect < 0) break;
      answer = (char *)malloc(numbytestoexpect+1);
      ReadFromTCPSocket(sock,answer,numbytestoexpect);
      answer[numbytestoexpect] = '\0';
      printf("%s ",answer);
      fflush(stdout);
      CHECKFORBL(answer,numbytestoexpect)
      ans2 = answer;
      SKIPBLANKS(ans2)
      // STOREINFILE(fp,rank,ans2)
      for(i=0;i<numbytestoexpect;i++) 
      {
       fprintf(fp,"%c",answer[i]);
       printf("%c",answer[i]);
      }
     fflush(stdout);
          
      ans2 = NULL;
      free(answer);
      
      /* get the field - DOC DATE */
      answer = (char *)malloc(MAXWORDLEN);
      if (ReadFromTCPSocket(sock,answer,MAXWORDLEN)< 0) break;
      numbytestoexpect = atoi(answer);
      free(answer);
      if (numbytestoexpect < 0) break;
      answer = (char *)malloc(numbytestoexpect+1);
      ReadFromTCPSocket(sock,answer,numbytestoexpect);
      CHECKFORBL(answer,numbytestoexpect)
      ans2 = answer;
      SKIPBLANKS(ans2)
      STORESTRINGFILE(fp,ans2)
      ans2 = NULL;
      free(answer);
      
      /* get the field DOC TITLE */
      answer = (char *)malloc(MAXWORDLEN);
      if (ReadFromTCPSocket(sock,answer,MAXWORDLEN)< 0) break;
      numbytestoexpect = atoi(answer);
      free(answer);
      answer = (char *)malloc(numbytestoexpect+1);
      ReadFromTCPSocket(sock,answer,numbytestoexpect);
      CHECKFORBL(answer,numbytestoexpect)
      ans2 = answer;
      SKIPBLANKS(ans2)
      STORESTRINGFILE(fp,ans2)
      INSERTEOLN(fp)
      ans2 = NULL;
      free(answer);

#ifdef INQ_DEBUG
      printf("Rank %d= %s\n",rank,answer);
#endif

      fprintf(stdout,".");
      fflush(stdout);

      rank++;
    }
  
  fprintf(stdout,"\nDone\n");
  
  CloseSocket(sock);
  free(str);
  
  fclose(fp);

  /*
    Now the idea is to pack it off the contents of the file onto devise
    for visualization     
    And then once the devise visualizes the file, the user can state a
    particular document and that document will be requested from the
    mini-client running on the inquery machine jazz-man     
   */
  
  *interp->result  = 1;
  return TCL_OK;  
}

int inqFetch(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  
  int  sock,numbytestoexpect,rank,i;
  char *str,*answer,*ans2;
  FILE *fp_fetch;

#ifdef INQ_DEBUG
  printf(" inquery server name => %s \n",inquery_mac_name);
#endif 

  /* the initialazation part */
  str = (char *)malloc(MAXQUERYLEN);
  sock=CreateClientTCPSocket(INQ_FETCH_SOCKET,inquery_mac_name);

  /* send the query to the server */
  strcpy(str,argv[1]);

  answer = (char *)malloc(MAXWORDLEN);
  bzero(answer,MAXWORDLEN);
  sprintf(answer,"%d",strlen(str));
  WriteToTCPSocket(sock,answer,MAXWORDLEN);
  free(answer);

  WriteToTCPSocket(sock,str,strlen(str));

  /* the file where the retrieved data is to be stored */
  fp_fetch = fopen(INQ_DOC_TMPFILE,"w");

  fprintf(stdout,"Retrieving documents");
  fflush(stdout);

  while(1)
    {
      IsMessageAvailableOnSocket(sock,0);

      /* get the DOC contents */
      answer = (char *)malloc(MAXWORDLEN);
      bzero(answer,MAXWORDLEN);
      if (ReadFromTCPSocket(sock,answer,MAXWORDLEN)< 0)break;
      numbytestoexpect = atoi(answer);
      printf("numbytestoexpect = %d\n",numbytestoexpect);
      free(answer);
      if (numbytestoexpect < 0) break;
      answer = (char *)malloc(numbytestoexpect+1);
      bzero(answer,numbytestoexpect+1);
      printf("\n%d\n",ReadFromTCPSocket(sock,answer,numbytestoexpect));
      answer[numbytestoexpect] = '\0';
      CHECKFORBL(answer,numbytestoexpect)
      ans2 = answer;
      SKIPBLANKS(ans2)
      STORESTRINGFILE(fp_fetch,ans2)
      ans2 = NULL;
      free(answer);

#ifdef INQ_DEBUG
      fflush(stdout);
#endif
   }

  fprintf(stdout,"\nDone\n");
  CloseSocket(sock);
  fclose(fp_fetch);

  *interp->result  = 1;
  return TCL_OK;  
}

int inqTitle(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  *interp->result  = 1;
  return TCL_OK;
}

