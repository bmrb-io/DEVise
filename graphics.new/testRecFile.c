/*
  $Id$

  $Log$*/

#include "RecFile.h"
#include <stdio.h>
#include "TestData.h"

const int NUM_RECS = 100;

main()
{
  int i;
  
  struct Data buf;
  /* Create a new rec file */
  RecFile *rfile = RecFile::CreateFile("testFile", sizeof(Data));
  if (rfile == NULL){
    printf("can't create existing file\n");
    exit(1);
  }
  
  /* insert records in there */
  printf("insert records\n");
  for (i=0; i < NUM_RECS; i++){
    buf.x = i; buf.y = i*2;
    rfile->WriteRec(i,1,&buf);
  }
  
  /* Get records */
  printf("getting records\n");
  for (i=0; i < NUM_RECS; i++){
    rfile->ReadRec(i,1,&buf);
    printf("(%d,%d)\n", buf.x,buf.y);
  }
  
	printf("closing file and reopening\n");
  delete rfile;
  rfile = RecFile::OpenFile("testFile", sizeof(Data));
  
  /* Get records in even order */
  for (i=0; i < NUM_RECS; i+= 2){
    rfile->ReadRec(i,1,&buf);
    printf("(%d,%d)\n", buf.x,buf.y);
  }
  
  /* Get records in odd order */
  for (i=1; i < NUM_RECS; i+= 2){
    rfile->ReadRec(i,1,&buf);
    printf("(%d,%d)\n", buf.x,buf.y);
  }
}
