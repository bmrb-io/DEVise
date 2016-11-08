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
  Revision 1.2  1995/09/05 22:15:28  jussi
  Added CVS header.
*/

#include "UnixRecFile.h"

RecFile *RecFile::CreateFile(char *name, int recSize)
{
  return UnixRecFile::CreateFile(name, recSize);
}

RecFile *RecFile::OpenFile(char *name, int recSize, Boolean trunc)
{
  return UnixRecFile::OpenFile(name, recSize, trunc);
}
