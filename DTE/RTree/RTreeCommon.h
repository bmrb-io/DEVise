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

#ifndef RTREE_COMMON_H
#define RTREE_COMMON_H

const int VolumeSize = 1*1024*1024;
static const char* VolumeName = "RTREE";
extern int RTreeFile;

void initialize_system(const char FileName[],
                 int  &RTreeFile,
                 int  VolumeSize);

void shutdown_system(const char FileName[],
               int  RTreeFile,
               int  VolumeSize);

#endif
