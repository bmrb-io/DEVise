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
  Revision 1.2  1995/09/05 22:14:23  jussi
  Added CVS header.
*/

#include "RangeInfoArray.h"
#include "BufferFifo.h"
#include "RangeInfo.h"
#include "TDataAttr.h"

BufferFifo::BufferFifo()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}

void BufferFifo::Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
			   int &arrayNum, int &pos)
{
  arrayNum = 0;
  pos = rangeArrays->Size(0);
}

Boolean BufferFifo::PickVictim(RangeInfoArrays *rangeArrays,
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

void BufferFifo::PhaseHint(BufferPolicy::Phase phase)
{ 
  _phase = phase; 
  _hasConverted = false;
}

void BufferFifo::FocusHint(RecId focus, TData *, GData *)
{
}

void BufferFifo::Clear()
{
  _phase = BufferPolicy::ScanPhase;
  _hasConverted = false;
}
