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
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"

void Exit::DoExit(int code)
{
  exit(code);
}

void Exit::DoAbort(char *reason, char *file, int line)
{
  fprintf(stderr, "An internal error has occurred. The reason is:\n");
  fprintf(stderr, "  %s\n", reason);
  fprintf(stderr, "  location is %s: %d\n", file, line);

  exit(2);
}
