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
  Revision 1.1  1996/10/17 20:42:17  jussi
  Initial revision.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ClientServer.h"

static char *_progName = 0;

void Usage()
{
  fprintf(stderr, "Usage: %s [-h host] [-p port]\n", _progName);
}

int main(int argc, char **argv)
{
  _progName = argv[0];

  char *hostname = "localhost";
  int portnum = DefaultNetworkPort;

  for(int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-h")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No host name specified with -h option.\n");
	Usage();
	exit(1);
      }
      hostname = argv[i + 1];
      i++;
    } else if (!strcmp(argv[i], "-p")) {
      if (i + 1 >= argc) {
	fprintf(stderr, "No port number specified with -p option.\n");
	Usage();
	exit(1);
      }
      portnum = atoi(argv[i + 1]);
      i++;
    } else {
      Usage();
      exit(1);
    }
  }

  TclClient client("Client", hostname, portnum, "client.tcl");
  client.MainLoop();

  return 1;
}
