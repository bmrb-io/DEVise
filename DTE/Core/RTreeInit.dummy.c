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
  Revision 1.2  1997/09/05 22:20:09  donjerko
  Made changes for port to NT.

  Revision 1.1  1997/08/10 20:30:54  donjerko
  Fixed the NO_RTREE option.

  Revision 1.3  1997/03/28 16:07:26  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

//#include <iostream.h>   erased for sysdep.h
#include "sysdep.h"

using namespace std;

int NextEmptyPage; // global variable used by genrtree.C
char *FileArr;     // global variable used by genrtree.C
int RTreeFile;	    // mapped file pointer

/* These functions are already Implemented in SBMInit.dummy.c

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
} */

char *temp_file_name="bulk_temp";
int using_sort=1;
int num_of_clip_iter=1;

