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
  Revision 1.3  1996/01/15 21:59:07  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:14:28  jussi
  Added CVS header.
*/

#ifndef BuferLifo_h
#define BuferLifo_h

#include "BufferPolicy.h"

class BufferLifo: public BufferPolicy {
public:
    /* Return the next victim */
    virtual Boolean PickVictim(RangeInfoArrays *rangeArrays,
                               int &arrayNum, int &pos) {
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

    /* A new range has been read into memory. Decide where to put it
       by setting listNum and pos*/
    virtual void Placement(RangeInfo *info, RangeInfoArrays *rangeArrays,
                           int &arrayNum, int &pos) {
        /* Insert at head of list */
        arrayNum = 0;
        pos = -1;
    }
};

#endif
