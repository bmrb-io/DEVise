/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.3  1996/05/11 21:30:01  jussi
  Fixed problem with interp->result overrun.

  Revision 1.2  1996/05/11 17:23:14  jussi
  Added command line options for setting host name and port number.

  Revision 1.1  1996/05/11 01:52:02  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <tcl.h>
#include <tk.h>

#include "ClientAPI.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

static char *_progName = 0;
static char *_hostName = "localhost";
static int _portNum = DefaultDevisePort;
static int _deviseFd = -1;

static Tcl_Interp *_interp = 0;
static Tk_Window _mainWindow = 0;
static char *_restoreFile = 0;
static char _buffer[10 * 1024];

void DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  (void)Tcl_Eval(_interp, cmd);
  (void)DeviseClose(_deviseFd);
  exit(1);
}

int DEViseCmd(ClientData clientData, Tcl_Interp *interp,
	      int argc, char *argv[])
{
#ifdef DEBUG	
  printf("Function %s, %d args\n", argv[1], argc - 1);
#endif

  // do not send the DEVise command verb
  if (DeviseSend(_deviseFd, &argv[1], argc - 1) < 0)
    DOASSERT(0, "Server has terminated");

  u_short flag;
  do {
    if (DeviseReceive(_deviseFd, _buffer, flag, argv[1]) < 0)
      DOASSERT(0, "Server has terminated");
    if (flag == API_CTL) {
#ifdef DEBUG
      printf("Executing Tcl command: \"%s\"\n", _buffer);
#endif
      (void)Tcl_Eval(_interp, _buffer);
    }
  } while (flag != API_ACK && flag != API_NAK);

#ifdef DEBUG
  printf("Received reply: \"%s\"\n", _buffer);
#endif

  interp->result = _buffer;

  if (flag == API_NAK)
    return TCL_ERROR;

  return TCL_OK;
}

void ReadServer(ClientData cd, int mask)
{
#ifdef DEBUG
  printf("Receiving command from server\n");
#endif

  u_short flag;
  if (DeviseReceive(_deviseFd, _buffer, flag, "Control command") < 0)
    DOASSERT(0, "Server has terminated");

  if (flag != API_CTL) {
    fprintf(stderr, "Received unexpected type of message: %u\n", flag);
    return;
  }

#ifdef DEBUG
  printf("Executing Tcl command: \"%s\"\n", _buffer);
#endif

  (void)Tcl_Eval(_interp, _buffer);
}

void SetupConnection()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

  _interp = Tcl_CreateInterp();
  _mainWindow = Tk_CreateMainWindow(_interp, NULL, "DEVise", "DEVise");
  if (!_mainWindow) {
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
  Tk_MoveWindow(_mainWindow, 0, 0);
  Tk_GeometryRequest(_mainWindow, 100, 200);

  if (Tcl_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tcl.\n");
    exit(1);
  }
  if (Tk_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tk.\n");
    exit(1);
  }

  Tcl_LinkVar(_interp, "argv0", (char *)&_progName, TCL_LINK_STRING);
  Tcl_CreateCommand(_interp, "DEVise", DEViseCmd, 0, 0);

  printf("Client running.\n");
  printf("\n");

  printf("Connecting to server %s:%d.\n", _hostName, _portNum);

  _deviseFd = DeviseOpen(_hostName, _portNum, 1);
  Tk_CreateFileHandler(_deviseFd, TK_READABLE, ReadServer, 0);
	
  printf("Connection established.\n\n");

  char *controlFile = "control.tk";

  char *envPath = getenv("DEVISE_LIB");
  char *control;
  char buf[256];
  if (envPath) {
    sprintf(buf, "%s/%s", envPath, controlFile);
    control = buf;
  } else
    control = controlFile;

  printf("Control panel file is: %s\n", control);

  int code = Tcl_EvalFile(_interp, control);
  if (code != TCL_OK) {
    fprintf(stderr, "Cannot start up control panel.\n");
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }

  if (_restoreFile) {
    Tcl_SetVar(_interp, "restoring", "1", 0);
    int code = Tcl_EvalFile(_interp,_restoreFile);
    Tcl_SetVar(_interp, "restoring", "0", 0);
    if (code != TCL_OK) {
      fprintf(stderr, "Can't restore session file %s\n", _restoreFile);
      fprintf(stderr, "%s\n", _interp->result);
    } else {
      Tcl_SetVar(_interp, "sessionName", _restoreFile, 0);
    }
  }
}

void Usage()
{
  fprintf(stderr, "Usage: %s [-h host] [-p port] [session]\n", _progName);
}

int main(int argc, char **argv)
{
  _progName = argv[0];

  for(int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No host name specified with -h option.\n");
	Usage();
	exit(1);
      }
      _hostName = argv[i + 1];
      i++;
    } else if (!strcmp(argv[i], "-p")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No port number specified with -p option.\n");
	Usage();
	exit(1);
      }
      _portNum = atoi(argv[i + 1]);
      i++;
    } else {
      if (_restoreFile) {
	Usage();
	exit(1);
      }
      _restoreFile = argv[i];
    }
  }

  SetupConnection();
  Tk_MainLoop();
#ifdef DEBUG
  printf("Returned from Tk loop\n");
#endif
  Tk_DeleteFileHandler(_deviseFd);
  (void)DeviseClose(_deviseFd);

  return 1;
}
