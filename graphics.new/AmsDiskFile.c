/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.3.46.1  2005/09/06 21:20:16  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.3  1996/01/12 15:11:08  jussi
  Replaced libc.h with stdlib.h. Added copyright statement.

  Revision 1.2  1995/09/05 22:14:03  jussi
  Added CVS header.
*/

#include <stdlib.h>
#include <string.h>

#include "AmsDiskFile.h"
#include "TDataAppend.h"
#include "ams.h"

const int NumPages = 16721;
const int NumRecs  = 2140160;

void AmsDiskFile::WritePage(int pageNum, void *buf)
{
  /* don't write any pages */
}

/********************************************************
Read a page
*********************************************************/

void AmsDiskFile::ReadPage(int pageNum, void *buf)
{
  struct FileHeader header;
  if (pageNum == 0) {
    /* return header */
    header.numRecs = NumRecs;
    header.recSize = sizeof(struct AmsData);
    (*(int *)buf) = NumPages;
    bcopy(&header,((char *)buf) + sizeof(int), sizeof(header));
  } else {
    /* fill page with data */
    struct AmsData *ptr = (AmsData *)buf;
    int recs = 4096/sizeof(AmsData);
    double time = (pageNum - 1) * recs * 30.0 + 15.0;
    double timeIncr = 30.0;
    for(int i = 0; i < recs; i++) {
      ptr->time = time;
      ptr->solDn = 500.0;
      time += timeIncr;
      ptr++;
    }
  }
}

