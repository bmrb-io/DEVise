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
  Revision 1.4  1997/10/07 17:05:59  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.3  1996/11/23 21:11:22  jussi
  Pushed all policy-independent code up to BufferPolicy.h. Then moved
  remaining code from .c file to .h. Then removed empty policy .c files.

  Revision 1.2  1995/09/05 22:14:31  jussi
  Added CVS header.
*/

#ifndef BufferPolicy_h
#define BufferPolicy_h

#include "DeviseTypes.h"
#include "RecId.h"
#include "RangeInfoArray.h"

class TData;
class GData;

class BufferPolicy {
  public:
    /* flags for policy */
    static const int ReportVictim = 1;
    static const int ReportPlacement = 0x2;

    enum Phase { BinSearchPhase, ScanPhase, MemConvertPhase,
                 DiskConvertPhase, PrefetchPhase};
    
    BufferPolicy() {
        Clear();
    }
    
    virtual void Clear() {
        _phase = BufferPolicy::ScanPhase;
        _hasConverted = false;
    }

    virtual void PhaseHint(BufferPolicy::Phase phase) { 
        _phase = phase; 
        _hasConverted = false;
    }
    
    virtual void FocusHint(Coord focus, TData *tdata, GData *gdata) {}
    
    /* Return the info about this buffer policy */
    virtual void Info(int &numArrays, int &policyFlag) {
        numArrays = 1;
        policyFlag = ReportVictim | ReportPlacement;
    }
    
    /* Pick the next victim. Return false if a victim can not be found.*/
    virtual Boolean PickVictim(RangeInfoArrays *rangeArrays,
                               int &arrayNum, int &pos) = 0;
    
    /* A new range has been read into memory. Decide where to put it
       by setting listNum and pos */
    virtual void Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
                           int &arrayNum, int &pos) {
        arrayNum = 0;
        pos = rangeArrays->Size(0);
    }

  protected:
    BufferPolicy::Phase _phase;
    Boolean _hasConverted; 
};

#endif
