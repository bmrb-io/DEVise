#include "tipster.h"
#include "smart.h"
#include "inq.h"
#include "network.h"

#define  MAXFILENAME    40
#define  MAXWORDLEN     12
#define   SMARTSOCKET   9325

int mini_sock;
int server_port_num;
int sock;

main(int argc,char **argv)
{
  Tcl_Interp *interp;
  int         code;
  char        tcl_file_name[MAXFILENAME];
  char       *answer;

  if (argc!=4)
  {
    printf("Usage: tipster smart-server-machine inquery-server-name ");
    printf("tk-script-file\n");
    printf("> continuing with default server-name \n");
    printf("> smart  :    cops10  \n");
    printf("> inquery:    jazz-man\n\n");
    printf("> continuing with default tipster-script-file \n");
    printf("> tipster:    tipster_test.tk\n");

    strcpy(smart_mac_name,"cops10");
    strcpy(inquery_mac_name,"jazz-man");
    strcpy(tcl_file_name,"tipster_test.tk");
  }

  else
  { 
     strcpy(smart_mac_name,argv[1]);
     strcpy(inquery_mac_name,argv[2]);
     strcpy(tcl_file_name,argv[3]);
  }

  gethostname(my_name,MAXMACNAME);

  mini_sock=CreateClientTCPSocket(SMARTSOCKET,smart_mac_name);
 
  answer = (char *)malloc(MAXWORDLEN);
  ReadFromTCPSocket(mini_sock,answer,MAXWORDLEN);
  server_port_num  = atoi(answer);
  printf("server port number= %d\n",server_port_num);
  fflush(stdout);
  free(answer);

  while(1)
  {
    sock=CreateClientTCPSocket(server_port_num,smart_mac_name);
    if (sock>0) break;
    sleep(1);
  }

  printf("created socket num = %d\n",sock);
  fflush(stdout);

  interp = Tcl_CreateInterp();

  Tcl_CreateCommand(interp,"smQuery",smQuery,(ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"smAll",smAll,(ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"smQuit",smQuit,(ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand(interp,"Query",inqQuery,(ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"Fetch",inqFetch,(ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand(interp,"Title",inqTitle,(ClientData) NULL,
                    (Tcl_CmdDeleteProc *) NULL);

  Tk_CreateMainWindow(interp, 0, "test", "test");

  Tk_Init(interp);

  code = Tcl_EvalFile(interp,tcl_file_name);

  Tk_MainLoop();

  CloseSocket(mini_sock);

  printf("code = %d\n", code);

  if (*interp->result != 0) ;

  if (code!=TCL_OK) exit(1);
}
