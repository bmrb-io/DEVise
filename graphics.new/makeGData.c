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
  Revision 1.2  1995/09/05 22:16:30  jussi
  Added CVS header.
*/

/*
   make GData from ams TData.
   usage: makeGData tdata gdata
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "BufMgr.h"
#include "TDataAppend.h"
#include "GData.h"
#include "ams.h"

void Usage(char *name)
{
  printf("Usage: %s tdata gdata\n",name);
  exit(1);
}

main(int argc, char **argv)
{
  char *infile, *outfile;
  
  if (argc < 3)
    Usage(argv[0]);
  
  infile = argv[1];
  outfile = argv[2];
  
  /* Open output file */
  BufMgr *mgr = new BufMgr();
  TDataAppend *tdata = new TDataAppend(infile,mgr,sizeof(AmsData));
  TDataAppend *gdata = new TDataAppend(outfile,mgr,sizeof(GData));
  if (gdata->NumRecords() > 0){
    fprintf(stderr,"Gdata already has %d records\n", gdata->NumRecords());
    exit(2);
  }
  
  int numPages = tdata->NumPages();
  int page = tdata->FirstPage();
  GData gbuf;
  int i,j;
  for (i=0; i < numPages; i++, page++){
    int numRecs;
    void **recs;
    RecId startRid;
    tdata->GetPage(page,numRecs, startRid,recs);
    
    for (j=0; j < numRecs; j++){
      AmsData *data = (AmsData *)recs[j];
      gbuf.x = data->time;
      gbuf.y = data->solDn;
      gdata->InsertRec(&gbuf);
    }
    
    tdata->FreePage(page);
  }
  
  delete tdata;
  delete gdata;
}

