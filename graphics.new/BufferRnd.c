/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:14:32  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "RangeInfoArray.h"
#include "BufferRnd.h"
#include "RangeInfo.h"
#include "TDataAttr.h"

extern "C" long random();

BufferRnd::BufferRnd()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}

void BufferRnd::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
			  int &arrayNum, int &pos)
{
  arrayNum = 0;
  pos = rangeArrays->Size(0);
}

Boolean BufferRnd::PickVictim(RangeInfoArrays *rangeArrays,
			      int &arrayNum,int &pos)
{
  int maxTimes, size;
  size = maxTimes = rangeArrays->Size(0);

  for(; maxTimes > 0; maxTimes--) {
    int testPos = random()%  size;
    RangeInfo *rangeInfo = rangeArrays->GetRange(0, testPos);
    if ( !rangeInfo->InUse()) {
      /* found one */
      arrayNum = 0;
      pos = testPos;
      return true;
    }
  }
  
  /* still can't find a good one. Go through array one by one to look for
     a good one */
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

void BufferRnd::PhaseHint(BufferPolicy::Phase phase)
{ 
  _phase = phase; 
  _hasConverted = false;
}

void BufferRnd::FocusHint(RecId focus, TData *, GData *)
{
}

void BufferRnd::Clear()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}
