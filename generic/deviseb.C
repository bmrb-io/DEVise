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

#include "ClientAPI.h"

#define DOASSERT(c,r) { if (!(c)) DoAbort(r); }
//#define DEBUG

static char *_progName = 0;
static char *_hostName = "localhost";
static int   _portNum = DefaultDevisePort;
static int   _deviseFd = -1;

static char *_scriptFile = 0;
static int   _syncDone = 0;

void DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  (void)DeviseClose(_deviseFd);
  exit(1);
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

  if (DeviseSend(_deviseFd, API_CMD, 0, argc, argv) < 0) {
    fprintf(stderr, "Server has terminated. Client exits.\n");
    exit(1);
  }

  u_short flag;
  do {
    int rargc;
    char **rargv;
    if (DeviseReceive(_deviseFd, 1, flag, rargc, rargv) <= 0) {
      fprintf(stderr, "Server has terminated. Client exits.\n");
      exit(1);
    }
    if (flag == API_CTL)
      ControlCmd(rargc, rargv);
  } while (flag != API_ACK && flag != API_NAK);

  if (flag == API_NAK)
    return -1;

  return 1;
}

int ExecuteFile(char *script)
{
  FILE *fp = fopen(script, "r");
  if (!fp) {
    printf("Cannot open scrip file: %s\n", script);
    perror("fopen");
    return -1;
  }

  const int MAXARGC = 20;
  char *argv[MAXARGC];
  int i;
  for(i = 0; i < MAXARGC; i++) {
    argv[i] = new char [64];
    DOASSERT(argv[i], "Out of memory");
  }

  char line[256];
  while(fgets(line, sizeof line, fp)) {
    if (line[strlen(line) - 1] != '\n') {
      fprintf(stderr, "Line too long in script\n");
      return -1;
    }
    line[strlen(line) - 1] = 0;
    if (!strlen(line))
      continue;

    int argc = 0;
    for(i = 0; i < MAXARGC; i++)
      argv[i][0] = 0;

    char *ptr = line;
    int braces = 0;
    while(*ptr) {
      if (*ptr == '{') {
	if (braces > 0)
	  strcat(argv[argc], "{");
	++braces;
      } else if (*ptr == '}') {
	--braces;
	if (braces > 0)
	  strcat(argv[argc], "}");
      } else if (*ptr == ' ') {
	if (!braces) {
#ifdef DEBUG
	  printf("Argument %d is \"%s\"\n", argc, argv[argc]);
#endif
	  ++argc;
	  if (argc >= MAXARGC) {
	    fprintf(stderr, "Line has too many elements: %s\n", line);
	    return -1;
	  }
	} else
	  strcat(argv[argc], " ");
      }	else {
	char two[2];
	two[0] = *ptr;
	two[1] = 0;
	strcat(argv[argc], two);
      }
      ptr++;
    }

    ++argc;

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
	if (DeviseReceive(_deviseFd, 1, flag, rargc, rargv) <= 0) {
	  fprintf(stderr, "Server has terminated. Client exits.\n");
	  exit(1);
	}
	if (flag == API_CTL)
	  ControlCmd(rargc, rargv);
      }
      printf("Continuing.\n");
      _syncDone = 0;
    }
  }

  fclose(fp);

  for(i = 0; i < MAXARGC; i++)
    delete argv[i];

  return 1;
}

void Usage()
{
  fprintf(stderr, "Usage: %s [-h host] [-p port] script\n",
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
    } else {
      if (_scriptFile) {
	Usage();
	exit(1);
      }
      _scriptFile = argv[i];
    }
  }

  if (!_scriptFile) {
    Usage();
    exit(1);
  }

  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

  printf("Batch client running.\n");
  printf("\n");

  printf("Connecting to server %s:%d.\n", _hostName, _portNum);

  _deviseFd = DeviseOpen(_hostName, _portNum);
  if (_deviseFd < 0)
    exit(1);
	
  printf("Connection established.\n\n");

  if (ExecuteFile(_scriptFile) < 0)
    return 2;

  printf("Closing connection.\n");

  (void)DeviseClose(_deviseFd);

  return 1;
}
