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
  Revision 1.2  1997/03/28 16:07:47  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef RTREE_COMMON_H
#define RTREE_COMMON_H

const int VolumeSize = 10*1024*1024;
static const char* VolumeName = "RTREE";
extern int RTreeFile;

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize);

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize);

#endif
