/*
  $Id$

  $Log$*/

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
	int flag= DisplayTDataInMem | DisplayGDataInMem | DisplayGDataConverted,
	Boolean hor= true);

#endif
