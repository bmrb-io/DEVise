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
  Revision 1.2  1995/09/05 22:15:37  jussi
  Added CVS header.
*/

#ifndef Snapshot_h

#define Snapshot_h
#include "DeviseTypes.h"

class TDataMap;
class BufMgr;
class ViewWin;

/* flags for this function */
const int DisplayTDataInMem = 1;
const int DisplayGDataInMem = 2;
const int DisplayGDataConverted = 4;

/* Make a snapshot. hor == true if display horizontal line */
extern void MakeSnapshotView(TDataMap *map, BufMgr *mgr, ViewWin *parent,
			     int flag = DisplayTDataInMem | DisplayGDataInMem
			     | DisplayGDataConverted,
			     Boolean hor = true);

#endif
