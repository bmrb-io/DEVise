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
  Revision 1.2  1995/09/05 21:12:45  jussi
  Added/updated CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "Init.h"

void Exit::DoExit(int arg)
{
  if (Init::DoAbort()) {
    fflush(stdout);
    fflush(stderr);
    abort();
  }
  exit(2);
}
