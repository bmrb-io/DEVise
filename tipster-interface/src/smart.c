#include "smart.h"
#define   SMARTSOCKET   9325

extern int mini_sock;
extern int server_port_num;
extern int sock;

int smQuery(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  int  numbytestoexpect;
  int  rank=1,i;
  char *str = (char *)malloc(MAXQUERYLEN);
  char *answer;
  char *ans2;
  FILE *fp;

//  mini_sock=CreateClientTCPSocket(SMARTSOCKET,smart_mac_name);
  
  printf(" Waiting to hear from mini-server \n");
  fflush(stdout);
  /*

  answer = (char *)malloc(MAXWORDLEN);
  ReadFromTCPSocket(mini_sock,answer,MAXWORDLEN);
  server_port_num  = atoi(answer);
  printf("server port number= %d\n",server_port_num);
  fflush(stdout);
  free(answer);

  sock=CreateClientTCPSocket(server_port_num,smart_mac_name);
  printf("created socket num = %d\n",sock);
  fflush(stdout);

   */

  strcpy(str,"r");  /* for the smart server - knows its a run request */
  WriteToTCPSocket(sock,str,strlen(str));
  printf("write %s to  %d\n",str,sock);
  fflush(stdout);

  answer = (char *)malloc(MAXWORDLEN);
  ReadFromTCPSocket(sock,answer,MAXWORDLEN);
  printf("read %s from  %d\n",answer,sock);
  fflush(stdout);
  free(answer);

  /* 
    Now the smart guy knows that the request is to run the query and 
    is waiting for the query string 
   */
    
  /*
    Create the socket again .... 
   */

  strcpy(str,argv[1]);

  /* now send in the actual query */
  WriteToTCPSocket(sock,str,strlen(str));

  fp = fopen(SMART_TMPFILE,"w");

  fprintf(stdout,"Retrieving documents");
  fflush(stdout);

  // IsMessageAvailableOnSocket(sock,0);

  answer = (char *)malloc(MAXWORDLEN);
  if (ReadFromTCPSocket(sock,answer,MAXWORDLEN)< 0) 
  {
     CloseSocket(sock);
     free(str);
     free(answer);
    *interp->result  = 1;
     return TCL_OK;
  }

  numbytestoexpect = atoi(answer);
  free(answer);

  if (numbytestoexpect < 0)
  {
   //  CloseSocket(sock);
     free(str);
    *interp->result  = 1;
     return TCL_OK;
  }

  fprintf(stdout,".");
  fflush(stdout);

  answer = (char *)malloc(numbytestoexpect+1);
  ReadFromTCPSocket(sock,answer,numbytestoexpect);

  if (answer[numbytestoexpect-1] == '\n')
      answer[numbytestoexpect-1] = '\0';
  else
      answer[numbytestoexpect] = '\0';

  ans2 = answer;

  if (ans2[0] == '\n') ans2 ++;
  i=0;
  while(ans2[i++]==' ') ans2++;

  fprintf(fp,"%s",ans2);

  ans2 = NULL;
  free(answer);

  fprintf(stdout,"\nDone\n");

 // CloseSocket(sock); 
 // CloseSocket(mini_sock); 
  free(str);

  fclose(fp);

  *interp->result  = 1;
  return TCL_OK;
}

int smAll(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  int  numbytestoexpect;
  int  rank=1,i;
  char *str = (char *)malloc(MAXQUERYLEN);
  char *answer;
  char *ans2;
  FILE *fp;
  int server_port_num;

 /*
     deal with this case now
  */

  if ((strcmp(argv[1],"q") == 0) || (strcmp(argv[1],"x") == 0))
  {
    *interp->result  = 1;
     return TCL_OK;
  }

 // mini_sock=CreateClientTCPSocket(SMARTSOCKET,smart_mac_name);
  /*

  answer = (char *)malloc(MAXWORDLEN);
  ReadFromTCPSocket(mini_sock,answer,MAXWORDLEN);
  server_port_num  = atoi(answer);
  printf("server port number= %d\n",server_port_num);
  free(answer);

  sock=CreateClientTCPSocket(server_port_num,smart_mac_name);

    */
  strcpy(str,argv[1]);

  /* now send in the actual query */
  WriteToTCPSocket(sock,str,strlen(str));
  answer = (char *)malloc(MAXWORDLEN);
  ReadFromTCPSocket(sock,answer,MAXWORDLEN);
  free(answer);

  fp = fopen(SMART_DOC_TMPFILE,"w");

  fprintf(stdout,"Fetching the documents");
  fflush(stdout);

  // IsMessageAvailableOnSocket(sock,0);

  answer = (char *)malloc(MAXWORDLEN);

  if (ReadFromTCPSocket(sock,answer,MAXWORDLEN)< 0) 
  {
     CloseSocket(sock);
     free(str);
     free(answer);
    *interp->result  = 1;
     return TCL_OK;
  }

  numbytestoexpect = atoi(answer);
  free(answer);

  if (numbytestoexpect < 0)
  {
     // CloseSocket(sock);

     free(str);
    *interp->result  = 1;
     return TCL_OK;
  }

  answer = (char *)malloc(numbytestoexpect+1);
  ReadFromTCPSocket(sock,answer,numbytestoexpect);

  if (answer[numbytestoexpect-1] == '\n')
      answer[numbytestoexpect-1] = '\0';
  else
      answer[numbytestoexpect] = '\0';

  ans2 = answer;

  if (ans2[0] == '\n') ans2 ++;
  i=0;
  while(ans2[i++]==' ') ans2++;

  fprintf(fp,"%s",ans2);

  ans2 = NULL;
  free(answer);

  fprintf(stdout,"\nDone\n");

//  CloseSocket(sock); 
//  CloseSocket(mini_sock); 
  free(str);

  fclose(fp);

  *interp->result  = 1;
  return TCL_OK;
}

int smQuit(ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  int  numbytestoexpect;
  int  rank=1,i;
  char *str = (char *)malloc(MAXQUERYLEN);
  char *answer;
  char *ans2;
  FILE *fp;
  int server_port_num;

  strcpy(str,argv[1]);
  WriteToTCPSocket(sock,str,strlen(str));
  free(str);
  *interp->result  = 1;
  return TCL_OK;
 
}
