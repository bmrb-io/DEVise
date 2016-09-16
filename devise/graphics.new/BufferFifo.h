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
  Revision 1.3  1996/01/15 21:58:46  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:14:24  jussi
  Added CVS header.
*/

#ifndef BuferFifo_h
#define BuferFifo_h

#include "BufferPolicy.h"

class BufferFifo: public BufferPolicy {
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
};

#endif
