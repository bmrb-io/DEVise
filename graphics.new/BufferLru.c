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
  Revision 1.2  1995/09/05 22:14:29  jussi
  Added CVS header.
*/

#include "RangeInfoArray.h"
#include "BufferLru.h"
#include "RangeInfo.h"
#include "TDataAttr.h"

BufferLru::BufferLru()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}

void BufferLru::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
			  int &arrayNum, int &pos)
{
  arrayNum = 0;
  pos = rangeArrays->Size(0);
}

Boolean BufferLru::PickVictim(RangeInfoArrays *rangeArrays,
			      int &arrayNum, int &pos)
{
  /* Find least recently used by picking one from the end */
  for(i = rangeArrays->Size(0)-1; i >= 0 ; i--) {
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

/* Report information about usage */
void BufferLru::Usage(RangeInfoArrays *rangeArrays, int arrayNum, int pos)
{
  /* move this most recently used element to the front */
}

void BufferLru::PhaseHint(BufferPolicy::Phase phase)
{
  _phase = phase; 
  _hasConverted = false;
}

void BufferLru::FocusHint(RecId focus, TData *, GData *)
{
}

void BufferLru::Clear()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}
