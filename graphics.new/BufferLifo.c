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
  Revision 1.2  1995/09/05 22:14:27  jussi
  Added CVS header.
*/

#include "RangeInfoArray.h"
#include "BufferLifo.h"
#include "RangeInfo.h"
#include "TDataAttr.h"

BufferLifo::BufferLifo()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}

void BufferLifo::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
			   int &arrayNum, int &pos)
{
  /* Insert at head of list */
  arrayNum = 0;
  pos = -1;
}

Boolean BufferLifo::PickVictim(RangeInfoArrays *rangeArrays,
			       int &arrayNum, int &pos)
{
  for(int i = 0; i < rangeArrays->Size(0); i++) {
    RangeInfo *rangeInfo = rangeArrays->GetRange(0, i);
    if (!rangeInfo->InUse()) {
      /* found one */
      arrayNum = 0;
      pos = i;
      return true;
    }
  }

  /* can't find anything as victim */
  return false;
}

void BufferLifo::PhaseHint(BufferPolicy::Phase phase)
{
  _phase = phase; 
  _hasConverted = false;
}

void BufferLifo::FocusHint(RecId focus, TData *, GData *)
{
}

void BufferLifo::Clear()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}
