/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.2  1995/09/05 22:16:59  jussi
  Added CVS header.
*/

/* test hole in unix disk file */

#include "UnixDiskFile.h"
#include <stdio.h>

main()
{
  DiskFile *dfile = UnixDiskFile::CreateFile("holedata");
  if (dfile == NULL){
    printf("can't create data file\n");
    exit(2);
  }
  
  char buf[DISKFILE_PAGESIZE];
  
  printf("writing page\n");
  int i;
  for (i=0; i < 30; i += 2){
    bcopy((char *)&i, buf, sizeof(int));
    dfile->WritePage(i,buf);
  }
  
  printf("reading page\n");
  for (i=0; i < 100; i += 2){
    dfile->ReadPage(i,buf);
    int tmp;
    bcopy(buf,(char *)&tmp, sizeof(int));
    printf("%d\n", i);
  }
}
