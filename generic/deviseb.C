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
static int _portNum = DefaultDevisePort;

static char *_idleScript = 0;
static char *_scriptFile = 0;

void DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  exit(1);
}

int DEViseCmd(int argc, char **argv, char *result)
{
#ifdef DEBUG	
  printf("Function %s, %d args\n", argv[1], argc - 1);
#endif

  if (DeviseSend(argv, argc) < 0)
    DOASSERT(0, "Server has terminated");

  int errorFlag;
  if (DeviseReceive(result, errorFlag, argv[0]) < 0)
    DOASSERT(0, "Server has terminated");

  if (errorFlag)
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
#ifdef DEBUG
    printf("Sending %d elements\n", argc);
#endif
    char result[10 * 1024];
    if (DEViseCmd(argc, argv, result) < 0) {
      fprintf(stderr, "Error executing command: %s\n", result);
    }
  }

  fclose(fp);

  for(i = 0; i < MAXARGC; i++) {
    delete argv[i];
  }

  return 1;
}

void SetupConnection()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

  printf("Batch client running.\n");
  printf("\n");

  printf("Connecting to server %s:%d.\n", _hostName, _portNum);

  (void)DeviseOpen(_hostName, _portNum, 0);
	
  printf("Connection established.\n\n");

  (void)ExecuteFile(_scriptFile);

#if 0
  // need to send idleScript to server when server becomes idle
#endif
}

void Usage()
{
  fprintf(stderr, "Usage: %s [-h host] [-p port] script idle\n", _progName);
}

int main(int argc, char **argv)
{
  _progName = argv[0];

  if (argc < 3) {
    Usage();
    exit(1);
  }

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
      if (!_scriptFile)
	_scriptFile = argv[i];
      else if (!_idleScript)
	_idleScript = argv[i];
      else {
	Usage();
	exit(1);
      }
    }
  }

  SetupConnection();
  (void)DeviseClose();

  return 1;
}
