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
  Revision 1.3  1996/01/15 21:58:59  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:14:32  jussi
  Added CVS header.
*/

#ifndef BuferRnd_h
#define BuferRnd_h

#include <stdlib.h>

#include "BufferPolicy.h"

class BufferRnd: public BufferPolicy {
public:
    /* Return the next victim */
    virtual Boolean PickVictim(RangeInfoArrays *rangeArrays,
                               int &arrayNum, int &pos) {
        int maxTimes, size;
        size = maxTimes = rangeArrays->Size(0);
        
        for(; maxTimes > 0; maxTimes--) {
            int testPos = rand() % size;
            RangeInfo *rangeInfo = rangeArrays->GetRange(0, testPos);
            if ( !rangeInfo->InUse()) {
                /* found one */
                arrayNum = 0;
                pos = testPos;
                return true;
            }
        }
        
        /* still can't find a good one. Go through array one by one
           to look for a good one */
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
};

#endif
