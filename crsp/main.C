/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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

  $Log$*/

// Tests the crsp extraction routines
#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sec.h"

#define FILENAME "/afs/cs.wisc.edu/p/miron/miron/DEVise/crsp/dsm94.dat"

main(int argc, char **argv)
{
  // Try to extract the info for the first security

  int fd = open(FILENAME, O_RDONLY);
  
  Security *sec = new Security(fd);

  sec->print_security();
}
