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
  Revision 1.4  1996/01/12 17:53:57  jussi
  Replaced call to random() with call to rand().

  Revision 1.3  1996/01/12 15:33:46  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 22:16:28  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "StrData.h"
#include "DevError.h"

int NumRecords= 1000; /* number of records to generate */
const char *fileName = "strData"; /* name of output file */
const int numProcs = 100; /* # of processes */

struct ProcRec {
  char name[StrDataNameSize];
  double meanUser; 
  double meanCpu;
  double meanIo;
} procRecs[numProcs];

main(int argc, char **argv)
{
  if (argc >= 2){
    NumRecords = atoi(argv[1]);
    if (NumRecords <= 0){
      printf("can't generated %d records\n", NumRecords);
      exit(2);
    }
  }

  int fd;
  if ((fd=open(fileName, O_RDWR|O_CREAT|O_TRUNC,S_IRUSR|S_IWUSR))<0){
    perror("can't create file");
    fprintf(stderr,"file name is %s\n",fileName);
    exit(2);
  }
  
  /* Generate records for process names and mean times*/
  int i;
  for (i=0; i < numProcs; i++){
    sprintf(procRecs[i].name,"proc%d",i);
    procRecs[i].meanUser = (double)(rand() % 100);
    procRecs[i].meanCpu = (double)(rand() % 100);
    procRecs[i].meanIo =  (double)(rand() % 100);
  }
  
  /* generate records */
  for (i=0; i < NumRecords; i++){
    int proc = rand() % numProcs;
    struct StrData rec;
    strcpy(rec.name,procRecs[proc].name);
    rec.user = procRecs[proc].meanUser + (rand() % 10)-5;
    rec.cpu= procRecs[proc].meanCpu+ (rand() % 10)-5;
    rec.io= procRecs[proc].meanIo+ (rand() % 10)-5;

    if (write(fd,(char *)&rec, sizeof(rec))!= sizeof(rec)){
      reportErrSys("write");
      exit(2);
    }
  }
  
  close(fd);
}
