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
  Revision 1.3  1996/01/15 21:58:25  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:14:26  jussi
  Added CVS header.
*/

#ifndef BufferFocal_h
#define BufferFocal_h

#include "BufferPolicy.h"

class TData;
class GData;

class BufferFocal: public BufferPolicy {
public:
    virtual void Clear() {
        BufferPolicy::Clear();
        _hasFocus = false;
    }

    /* Return the next victim */
    virtual Boolean PickVictim(RangeInfoArrays *rangeArrays,
                               int &arrayNum, int &pos) {
        if (!_hasFocus) {
            /* no focus. Just pick the 1st one */
            return PickFifo(rangeArrays, arrayNum, pos);
        }
        
        RecId low, high;
        int i;
        
        /* find 1st valid range */
        Boolean found = false;
        int index = 0;
        RecId furthestId = 0;
        for(i = 0; i < rangeArrays->Size(0); i++) {
            RangeInfo *rangeInfo = rangeArrays->GetRange(0, i);
            if (!rangeInfo->InUse() && 
                (rangeInfo->GetTData() ==_focusTData
                 || rangeInfo->GetTData() ==_focusGData)) {
                index = i;
                found = true; 
                rangeInfo->RecIds(furthestId, high);
                break;
            }
        }
        if (!found)
            return PickFifo(rangeArrays, arrayNum, pos);
        
        /* continue and find the one furthest from the focus */
        for(i = index + 1; i < rangeArrays->Size(0); i++) {
            RangeInfo *rangeInfo = rangeArrays->GetRange(0, i);
            rangeInfo->RecIds(low, high);
            if (!rangeInfo->InUse() &&
                (rangeInfo->GetTData()== _focusTData
                 || rangeInfo->GetTData()== _focusGData) &&
                Distance(low, _focusId) >
                Distance(furthestId, _focusId)) {
                index = i;
                furthestId = low;
            }
        }
        
        arrayNum = 0;
        pos = index;
        return true;
    }

    virtual void PhaseHint(BufferPolicy::Phase phase) { 
        _phase = phase; 
    }
    
    virtual void FocusHint(RecId focus, TData *tdata, GData *gdata) {
        _hasFocus = true;
        _focusId = focus;
        _focusTData = tdata;
        _focusGData = gdata;
    }
    
  protected:
    /* Return the next victim using FIFO policy */
    virtual Boolean PickFifo(RangeInfoArrays *rangeArrays,
                             int &arrayNum, int &pos) {
        /* do FIFO */
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
    
    unsigned Distance(unsigned num1, unsigned num2) {
        if (num1 > num2)
            return num1 - num2;
        return num2 - num1;
    }
    
    Boolean _hasFocus;                  /* has focus point */
    RecId _focusId;	                /* id that's the center of focus */
    TData *_focusTData;
    GData *_focusGData;
};

#endif
