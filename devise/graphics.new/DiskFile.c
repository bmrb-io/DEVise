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
  Revision 1.2  1995/09/05 22:14:43  jussi
  Added CVS header.
*/

#ifdef USE_AMSDISKFILE
#include <stdio.h>
#include "AmsDiskFile.h"
#else
#include "UnixDiskFile.h"
#endif

DiskFile *DiskFile::CreateFile(char *name)
{
#ifdef USE_AMSDISKFILE
  return NULL;
#else
  return UnixDiskFile::CreateFile(name);
#endif
}

DiskFile *DiskFile::OpenFile(char *name)
{
#ifdef USE_AMSDISKFILE
  return new AmsDiskFile();
#else
  return UnixDiskFile::OpenFile(name);
#endif
}
