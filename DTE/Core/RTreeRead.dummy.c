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

#include <iostream.h>

int NextEmptyPage; // global variable used by genrtree.C
char *FileArr;     // global variable used by genrtree.C
int RTreeFile;	    // mapped file pointer

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize)
{
	cout << "RTrees disabled: no initialization done" << endl;
}

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize)
{
	cout << "RTrees disabled: no shutdown done" << endl;
}

char *temp_file_name="bulk_temp";
int using_sort=1;
int num_of_clip_iter=1;

