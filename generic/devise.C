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
  Revision 1.14  1996/09/05 21:33:11  jussi
  Devise command 'connectData' was renamed getDisplayImage.
  Minor other improvements.

  Revision 1.13  1996/08/29 21:43:39  guangshu
  Added Tcl command SetGetImage to set up data channel then ask server
  to ship the gif files. The gif files can be either put to stdout,
  stderr or saved to files.
  Added -q option for DEVise client to make it quiet. So the gif files
  can be sent to web browser directly.

  Revision 1.12  1996/07/15 14:22:38  jussi
  This code is now compatible with Tcl 7.5 and Tk 4.1. Compatibility
  with older Tcl/Tk code maintained for a short while.

  Revision 1.11  1996/07/11 19:38:27  jussi
  The Tcl variable 'file' is set to the name of the session file
  before the session file is executed. Exported session files
  with data use the 'file' variable.'

  Revision 1.10  1996/07/11 18:18:04  jussi
  In batch mode, the Tcl client no longer uses any Tk or X11 features.
  The client operates in batch mode when an idle script is specified.
  The idle script is interpreted with Tcl. Tk is not needed.

  Revision 1.9  1996/07/09 15:59:36  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.8  1996/05/20 18:46:36  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/05/15 16:40:01  jussi
  Improved support for idle scripts; the client now uses the
  new server synchronization mechanism.

  Revision 1.6  1996/05/14 19:06:28  jussi
  Added checking of return value from DeviseOpen().

  Revision 1.5  1996/05/14 15:08:34  jussi
  Added support for idle scripts, that is, scripts which are
  executed when the server becomes idle. Typically this means
  that the server has drawn all views and they are ready to
  be extracted as images.

  Revision 1.4  1996/05/13 18:08:17  jussi
  The code now accepts API_CTL type messages in between API_CMD
  and API_ACK/API_NAK messages. Control channel was merged with
  the regular socket pair.

  Revision 1.3  1996/05/11 21:30:01  jussi
  Fixed problem with interp->result overrun.

  Revision 1.2  1996/05/11 17:23:14  jussi
  Added command line options for setting host name and port number.

  Revision 1.1  1996/05/11 01:52:02  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <tcl.h>
#include <tk.h>

#include "ClientAPI.h"
#include "Version.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

static char *_progName = 0;
static char *_hostName = "localhost";
static int _portNum = DefaultNetworkPort;
static int _deviseFd = -1;
static int _dataFd = -1;
static int _dataNewFd = -1;
static int _quiet = 0;

static char *_idleScript = 0;
static int   _syncDone = 0;

static Tcl_Interp *_interp = 0;
static Tk_Window _mainWindow = 0;
static char *_restoreFile = 0;

int _useopengl = 0;

static void DoAbort(char *reason)
{
    fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
    char cmd[256];
    sprintf(cmd, "AbortProgram {%s}", reason);
    (void)Tcl_Eval(_interp, cmd);
    (void)NetworkClose(_deviseFd);
    exit(1);
}

static void ControlCmd(int argc, char **argv)
{
    if (argc == 1 && !strcmp(argv[0], "SyncDone")) {
#ifdef DEBUG
        printf("Server synchronized.\n");
#endif
        _syncDone = 1;
        return;
    }
    
    char *cmd = NetworkPaste(argc, argv);
    DOASSERT(cmd, "Out of memory");
#ifdef DEBUG
    printf("Executing control command: \"%s\"\n", cmd);
#endif
    (void)Tcl_Eval(_interp, cmd);
    delete cmd;
}

int DEViseCmd(ClientData clientData, Tcl_Interp *interp,
	      int argc, char *argv[])
{
#ifdef DEBUG	
    printf("Function %s, %d args\n", argv[1], argc - 1);
#endif

    // do not send the DEVise command verb
    if (NetworkSend(_deviseFd, API_CMD, 0, argc - 1, &argv[1]) < 0) {
        fprintf(stderr, "Server has terminated. Client exits.\n");
        exit(1);
    }
    
    u_short flag;
    int rargc;
    char **rargv;
    do {
        if (NetworkReceive(_deviseFd, 1, flag, rargc, rargv) <= 0) {
            fprintf(stderr, "Server has terminated. Client exits.\n");
            exit(1);
        }
        if (flag == API_CTL)
            ControlCmd(rargc, rargv);
    } while (flag != API_ACK && flag != API_NAK);
    
    char *result = NetworkPaste(rargc, rargv);
    DOASSERT(result, "Out of memory");
#ifdef DEBUG
    printf("Received reply: \"%s\"\n", result);
#endif
    Tcl_SetResult(_interp, result, TCL_VOLATILE);
    delete result;
    
    if (flag == API_NAK)
        return TCL_ERROR;
    
    return TCL_OK;
}

int SetGetImage(ClientData clientData, Tcl_Interp *interp,
                int argc, char *argv[]) 
{ 
   _dataFd = socket(AF_INET, SOCK_STREAM, 0);
   if(_dataFd < 0) 
      perror("socket");
   DOASSERT(_dataFd >= 0, "Cannot create socket");

   struct sockaddr_in dataAddr;
   memset(&dataAddr, 0, sizeof dataAddr);
   dataAddr.sin_family = AF_INET;

   dataAddr.sin_port = htons(DefaultDataPort);
#ifdef DEBUG
   printf("Client Listening at port%u.\n", DefaultDataPort);
#endif

   dataAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   
   int result = bind(_dataFd, (struct sockaddr *)&dataAddr,
			sizeof(struct sockaddr));
   if (result < 0) {
       perror("bind");
       close(_dataFd);
   }
   DOASSERT(result >= 0, "Cannot bind to address");

   result = listen(_dataFd, 1);
   if (result < 0) {
       perror("listen");
       close(_dataFd);
   }
   DOASSERT(result >= 0, "Cannot listen");

#ifdef DEBUG
   printf("\nData client waiting for server to connect\n");
#endif

   char *mArgv[3] = { "getDisplayImage", "0", argv[1] };
   if (NetworkSend(_deviseFd, API_CMD, 0, 3, mArgv) < 0) {
       fprintf(stderr, "Server has terminated. Client exits.\n");
       close(_dataFd);
       exit(1);
   }

   struct sockaddr_in servAddr;
   memset(&servAddr, 0, sizeof(struct sockaddr));
   int len = sizeof(servAddr);

#ifdef DEBUG
   printf("Waiting for server to connect data channnel.\n");
#endif
   do {
      _dataNewFd = accept(_dataFd, (struct sockaddr *)&servAddr, &len);
   } while (_dataNewFd < 0 && errno == EINTR);

   close(_dataFd);

   if (_dataNewFd < 0) {
      perror("data accept");
      close(_dataFd);
      DOASSERT(0, "Error in network interface when connecting data channel");
   }
#ifdef DEBUG
   printf("Data channel established.\n");
#endif
   
   int file;
   if (!strcmp(argv[2], "stderr"))
       file = 2;
   else if(!strcmp(argv[2],"stdout"))
       file = 1; 
   else
       file = open(argv[2], O_WRONLY);

   char buf[BUF_SIZE];

   do {
      result = read(_dataNewFd, buf, BUF_SIZE);
#ifdef DEBUG
      printf("Client got data %s\n", buf);
#endif
      if (result > 0) write(file, buf, result);
   } while (result > 0);

   close(_dataNewFd);

   if (result < 0) {
      perror("Read error when read data from the data channel.\n");
      return TCL_ERROR;
   }
#ifdef DEBUG
   printf("Done getting GIF file");
#endif

   u_short flag;
   int rargc;
   char **rargv;
   do {
     if (NetworkReceive(_deviseFd, 1, flag, rargc, rargv) <= 0) {
         fprintf(stderr, "Server has terminated. Client exits.\n");
         exit(1);
     }
     if (flag == API_CTL)
         ControlCmd(rargc, rargv);
   } while (flag != API_ACK && flag != API_NAK);
    
   char *res = NetworkPaste(rargc, rargv);
   DOASSERT(res, "Out of memory");
#ifdef DEBUG
   printf("Received reply: \"%s\"\n", res);
#endif
   Tcl_SetResult(_interp, res, TCL_VOLATILE);
   delete res;
    
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
    int argc;
    char **argv;
    if (NetworkReceive(_deviseFd, 1, flag, argc, argv) <= 0) {
        fprintf(stderr, "Server has terminated. Client exits.\n");
        exit(1);
    }
    
    if (flag != API_CTL) {
        fprintf(stderr, "Ignoring unexpected message type: %u\n", flag);
        return;
    }

    ControlCmd(argc, argv);
}

void SetupConnection()
{
    if(!_quiet) {
      Version::PrintInfo();
      printf("\n");
    }

    _interp = Tcl_CreateInterp();
    if (Tcl_Init(_interp) == TCL_ERROR) {
        fprintf(stderr, "Cannot initialize Tcl.\n");
        exit(1);
    }

    if (!_idleScript) {
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
        _mainWindow = Tk_CreateMainWindow(_interp, NULL, "DEVise", "DEVise");
        if (!_mainWindow) {
            fprintf(stderr, "%s\n", _interp->result);
            exit(1);
        }
        Tk_MoveWindow(_mainWindow, 0, 0);
        Tk_GeometryRequest(_mainWindow, 100, 200);
#endif
        if (Tk_Init(_interp) == TCL_ERROR) {
            fprintf(stderr, "Cannot initialize Tk.\n");
            exit(1);
        }
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION > 0
        _mainWindow = Tk_MainWindow(_interp);
#endif
    }
    
    Tcl_LinkVar(_interp, "argv0", (char *)&_progName, TCL_LINK_STRING);
    Tcl_LinkVar(_interp, "quiet", (char *)&_quiet, TCL_LINK_INT);
    Tcl_CreateCommand(_interp, "DEVise", DEViseCmd, 0, 0);
    Tcl_CreateCommand(_interp, "DEViseGetImage", SetGetImage, 0, 0);
    
    if(!_quiet) {
	 printf("Client running.\n");
         printf("\n");
    }
    
    if(!_quiet) printf("Connecting to server %s:%d.\n", _hostName, _portNum);
    
    _deviseFd = NetworkOpen(_hostName, _portNum);
    if (_deviseFd < 0)
        exit(1);
    
    if (!_idleScript) {
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
        Tk_CreateFileHandler(_deviseFd, TK_READABLE, ReadServer, 0);
#else
        Tcl_CreateFileHandler(Tcl_GetFile((void *)_deviseFd, TCL_UNIX_FD),
                              TCL_READABLE, ReadServer, 0);
#endif
    }
    
    if(!_quiet) printf("Connection established.\n\n");
    
    char *controlFile = "control.tk";
    if (_idleScript) {
        controlFile = "batch.tcl";
        (void)Tcl_Eval(_interp, "DEVise setBatchMode 1");
    }
    
    char *envPath = getenv("DEVISE_LIB");
    char *control;
    char buf[256];
    if (envPath) {
        sprintf(buf, "%s/%s", envPath, controlFile);
        control = buf;
    } else
        control = controlFile;
    
    if(!_quiet) printf("Control panel file is: %s\n", control);
    
    int code = Tcl_EvalFile(_interp, control);
    if (code != TCL_OK) {
        fprintf(stderr, "Cannot start up control panel.\n");
        fprintf(stderr, "%s\n", _interp->result);
        exit(1);
    }
    
    if (_restoreFile) {
        if(!_quiet) printf("Restoring session file %s\n", _restoreFile);
        Tcl_SetVar(_interp, "restoring", "1", 0);
        Tcl_SetVar(_interp, "file", _restoreFile, 0);
        int code = Tcl_EvalFile(_interp, _restoreFile);
        Tcl_SetVar(_interp, "restoring", "0", 0);
        if (code != TCL_OK) {
            fprintf(stderr, "Cannot restore session file %s\n", _restoreFile);
            fprintf(stderr, "%s\n", _interp->result);
        } else {
            Tcl_SetVar(_interp, "sessionName", _restoreFile, 0);
        }
    }
}

void Usage()
{
    fprintf(stderr, "Usage: %s [-h host] [-p port] [session] [idlescript]\n",
            _progName);
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
        } else if (!strcmp(argv[i], "-q")) {
	    _quiet = 1;
//	    i ++;
	} else if (!strcmp(argv[i], "-gl")) {
            _useopengl=1;
        }
	else {
            if (!_restoreFile)
                _restoreFile = argv[i];
            else if(!_idleScript)
                _idleScript = argv[i];
            else {
                Usage();
                exit(1);
            }
        }
    }

    SetupConnection();

    if (_idleScript) {
        if (!_quiet) printf("Waiting for server synchronization.\n");
        _syncDone = 0;
        (void)Tcl_Eval(_interp, "DEVise sync");
        while(!_syncDone) {
            u_short flag;
            int argc;
            char **argv;
            if (NetworkReceive(_deviseFd, 1, flag, argc, argv) <= 0) {
                fprintf(stderr, "Server has terminated. Client exits.\n");
                exit(1);
            }
            if (flag == API_CTL)
                ControlCmd(argc, argv);
        }
        if (!_quiet) printf("Executing script file %s\n", _idleScript);
        int code = Tcl_EvalFile(_interp, _idleScript);
        if (code != TCL_OK) {
            fprintf(stderr, "Cannot execute script file %s\n", _idleScript);
            fprintf(stderr, "%s\n", _interp->result);
            (void)NetworkClose(_deviseFd);
            return 2;
        }
    } else {
        Tk_MainLoop();
#ifdef DEBUG
        printf("Returned from Tk loop\n");
#endif
    }
    
    if (!_idleScript) {
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
        Tk_DeleteFileHandler(_deviseFd);
#else
        Tcl_DeleteFileHandler(Tcl_GetFile((void *)_deviseFd, TCL_UNIX_FD));
#endif
    }

    (void)NetworkClose(_deviseFd);
    
    return 1;
}
