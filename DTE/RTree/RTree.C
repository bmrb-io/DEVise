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
 */

#include "RTree.h"

int NextEmptyPage; // global variable used by genrtree.C
char *FileArr;     // global variable used by genrtree.C
int RTreeFile;	    // mapped file pointer

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize)
{
  FILE *in_file = fopen(FileName, "r");
  if(!in_file){
  	printf("No index file found (Run FormatVolume)\n");
	return;
  }
  fscanf(in_file, "%d", &NextEmptyPage);
  fclose(in_file);
  RTreeFile = open(FileName, O_RDWR|O_CREAT, 0600);
  FileArr = mmap((caddr_t) 0, VolumeSize, PROT_READ|PROT_WRITE,
           MAP_SHARED|MAP_NORESERVE, RTreeFile, 0);
}

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize)
{
  FILE *in_file = fopen(FileName, "r+");
  if(!in_file){
  	printf("No index file found (Run FormatVolume)\n");
	return;
  }
  else{
	  munmap(FileArr, VolumeSize);
	  close(RTreeFile);
  }
  fprintf(in_file, "%d", NextEmptyPage);
  fclose(in_file);
}

char *temp_file_name="bulk_temp";
int using_sort=1;
int num_of_clip_iter=1;

