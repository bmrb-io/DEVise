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
  Revision 1.2  1995/09/05 22:15:44  jussi
  Added CVS header.
*/

#include <stdio.h>

#include "TData.h"
#include "Exit.h"

TData::~TData()
{
}

/* For writing records. Default: not implemented. */

void TData::WriteRecs(RecId startId, int numRecs, void *buf)
{
  fprintf(stderr, "TData::WRiteRecs: not implemented\n");
  Exit::DoExit(2);
}

