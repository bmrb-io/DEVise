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
  Revision 1.9  1996/07/09 15:59:37  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.8  1996/05/20 18:46:37  jussi
  Merged with ClientServer library code.

  Revision 1.7  1996/05/15 16:40:09  jussi
  Improved support for idle scripts; the client now uses the
  new server synchronization mechanism.

  Revision 1.6  1996/05/14 18:56:21  jussi
  Added checking of return value from DeviseOpen().

  Revision 1.5  1996/05/13 21:56:57  jussi
  Removed idleScript variable. Return value of program is 1
  if script file execution succeeded, otherwise 2.

  Revision 1.4  1996/05/13 20:23:51  jussi
  Added support for synchronization points.

  Revision 1.3  1996/05/13 18:07:38  jussi
  The code now accepts API_CTL type messages but ignores them.

  Revision 1.2  1996/05/11 17:23:24  jussi
  Added command line options for setting host name and port number.

  Revision 1.1  1996/05/11 01:51:42  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "ClientAPI.h"
#include "Version.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

static char *_deviseKeyword = "DEVise ";
static char _commentChar = '#';

static char *_progName = 0;
static char *_hostName = "localhost";
static int   _portNum = DefaultNetworkPort;
static int   _deviseFd = -1;

static char *_scriptFile = 0;
static int   _syncDone = 0;

void DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  (void)NetworkClose(_deviseFd);
  exit(2);
}

void ControlCmd(int argc, char **argv)
{
  if (argc == 1 && !strcmp(argv[0], "SyncDone")) {
#ifdef DEBUG
    printf("Server synchronized.\n");
#endif
    _syncDone = 1;
    return;
  }

#ifdef DEBUG
  printf("Ignoring control command: \"%s\"\n", argv[0]);
#endif
}

int DEViseCmd(int argc, char **argv)
{
#ifdef DEBUG	
  printf("Function %s, %d args\n", argv[0], argc);
#endif

  if (NetworkSend(_deviseFd, API_CMD, 0, argc, argv) < 0) {
    fprintf(stderr, "Server has terminated. Client exits.\n");
    exit(3);
  }

  u_short flag;
  do {
    int rargc;
    char **rargv;
    if (NetworkReceive(_deviseFd, 1, flag, rargc, rargv) <= 0) {
      fprintf(stderr, "Server has terminated. Client exits.\n");
      exit(3);
    }
    if (flag == API_CTL)
      ControlCmd(rargc, rargv);
    if (flag == API_NAK)
      printf("Received error message: %s\n", rargv[0]);
  } while (flag != API_ACK && flag != API_NAK);

  if (flag == API_NAK)
    return -1;

  return 1;
}

int ExecuteCommands()
{
  FILE *fp = stdin;

  const int MAXARGC = 64;
  char *argv[MAXARGC];
  int i;
  for(i = 0; i < MAXARGC; i++) {
    argv[i] = new char [64];
    DOASSERT(argv[i], "Out of memory");
  }

#ifdef DEBUG
  printf("Setting server to batch mode\n");
#endif

  strcpy(argv[0], "setBatchMode");
  strcpy(argv[1], "1");
  if (DEViseCmd(2, argv) < 0) {
    fprintf(stderr, "Error setting server to batch mode\n");
    return -1;
  }

  char line[256];
  while(fgets(line, sizeof line, fp)) {
    if (line[strlen(line) - 1] != '\n') {
      fprintf(stderr, "Line too long in script\n");
      return -1;
    }
    line[strlen(line) - 1] = 0;

    char *ptr = line;
    while(isspace(*ptr)) ptr++;

    if (*ptr == _commentChar) {
#ifdef DEBUG
      printf("Read comment line: \"\%s\"\n", ptr);
#endif
      continue;
    }

    if (*ptr == _deviseKeyword[0]
        && !strncmp(ptr, _deviseKeyword, strlen(_deviseKeyword))) {
      ptr += strlen(_deviseKeyword);
      while(isspace(*ptr)) ptr++;
    }

    if (!*ptr) {
#ifdef DEBUG
      printf("Read blank line (maybe with DEVise keyword)\n");
#endif
      continue;
    }

    int argc = 0;
    for(i = 0; i < MAXARGC; i++)
      argv[i][0] = 0;

    int braces = 0;
    int inarg = 0;
    char *argPtr = argv[0];

    while(*ptr) {
      if (*ptr == '{') {
	if (braces > 0)
          *argPtr++ = '{';
	++braces;
        inarg = 1;
        ptr++;
      } else if (*ptr == '}') {
	--braces;
	if (braces > 0)
          *argPtr++ = '}';
        inarg = 1;
        ptr++;
      } else if (*ptr == ' ') {
	if (!braces) {
          *argPtr = 0;
#ifdef DEBUG
	  printf("Argument %d is \"%s\"\n", argc, argv[argc]);
#endif
	  ++argc;
	  if (argc >= MAXARGC) {
	    fprintf(stderr, "Line has too many elements: %s\n", line);
	    return -1;
	  }
          argPtr = argv[argc];            
          while(isspace(*ptr)) ptr++;
          inarg = 0;
	} else {
          *argPtr++ = ' ';
          inarg = 1;
          ptr++;
        }
      }	else {
        *argPtr++ = *ptr++;
        inarg = 1;
      }
    }

    if (inarg) {
      *argPtr = 0;
#ifdef DEBUG
      printf("Argument %d is \"%s\"\n", argc, argv[argc]);
#endif
      ++argc;
    }

    if (argc == 1 && !strcmp(argv[0], "sync"))
      _syncDone = 0;

#ifdef DEBUG
    printf("Sending %d elements\n", argc);
#endif
    if (DEViseCmd(argc, argv) < 0) {
      fprintf(stderr, "Error executing command %s\n", argv[0]);
      return -1;
    }

    if (argc == 1 && !strcmp(argv[0], "sync")) {
      printf("Waiting for server synchronization.\n");
      while(!_syncDone) {
	u_short flag;
	int rargc;
	char **rargv;
	if (NetworkReceive(_deviseFd, 1, flag, rargc, rargv) <= 0) {
	  fprintf(stderr, "Server has terminated. Client exits.\n");
	  exit(3);
	}
	if (flag == API_CTL)
	  ControlCmd(rargc, rargv);
      }
      printf("Continuing.\n");
      _syncDone = 0;
    }
  }

  for(i = 0; i < MAXARGC; i++)
    delete argv[i];

  return 1;
}

void Usage()
{
  fprintf(stderr, "Usage: %s [-h host] [-p port]\n", _progName);
}

int main(int argc, char **argv)
{
  _progName = argv[0];

  for(int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No host name specified with -h option.\n");
	Usage();
	exit(4);
      }
      _hostName = argv[i + 1];
      i++;
    } else if (!strcmp(argv[i], "-p")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No port number specified with -p option.\n");
	Usage();
	exit(4);
      }
      _portNum = atoi(argv[i + 1]);
      i++;
    } else {
      Usage();
      exit(4);
    }
  }

  Version::PrintInfo();
  printf("\n");

  printf("Batch client running.\n");
  printf("\n");

  printf("Connecting to server %s:%d.\n", _hostName, _portNum);

  _deviseFd = NetworkOpen(_hostName, _portNum);
  if (_deviseFd < 0)
    exit(2);
	
  printf("Connection established.\n\n");

  if (ExecuteCommands() < 0)
    exit(5);

  printf("Closing connection.\n");

  (void)NetworkClose(_deviseFd);

  return 1;
}
