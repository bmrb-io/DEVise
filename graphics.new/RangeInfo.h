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
  Revision 1.5  1996/11/23 21:18:35  jussi
  Simplified code.

  Revision 1.4  1996/09/27 15:53:21  wenger
  Fixed a number of memory leaks.

  Revision 1.3  1996/01/15 21:58:05  jussi
  Added copyright notice and cleaned up the code.

  Revision 1.2  1995/09/05 22:15:23  jussi
  Added CVS header.
*/

#ifndef RangeInfo_h
#define RangeInfo_h

#include <stdio.h>

#include "DeviseTypes.h"
#include "Exit.h"
#include "RecId.h"

class TData;

/* A range of TData. For now we define fixed size buffers
   for each range */

class RangeInfo {
public:
    RangeInfo() {
        _inUse = 0;
        _dirty = 0;
        buf = NULL;
        bufSize = 0;
        tdata = NULL;
        dataSize = 0;
        low = high = 0;
        next = prev = 0;
    }

    /* Get TData for this range */
    TData *GetTData() { return tdata; }

    /* Get record ID associated with this range */
    void RecIds(Coord &lowId, Coord &highId) {
        lowId = low;
        highId = high;
    }

    /* Get data size and buffer size */
    void Sizes(int &retDataSize, int &retBufSize) {
        retDataSize = dataSize;
        retBufSize = bufSize;
    }

    int DataSize() { return dataSize; }
    int BufSize() { return bufSize; }
  
    /* Return size of the gap at the beginning where no data can be
       stored. This gap is caused by truncating a range into two
       on page boundaries */
    int GapSize() { return data - buf; }
    
    /* Get data and buffer area */
    void *GetData() { return data; }
    void *GetBuffer() { return buf; }

    Boolean InUse() { return _inUse > 0; }
    void ClearUse() {
        DOASSERT(_inUse > 0, "Invalid use counter");
        --_inUse;
    }
    void SetUse() { _inUse++; }
    
    Boolean Dirty() { return _dirty; }
    void SetDirty() { _dirty = 1; }
    void ClearDirty() { _dirty = 0; }
    
  protected:
    int listNum;             /* which list we are in */
    int posNum;		     /* position in the list */
    
    TData *tdata;
    char *data;	             /* pointer to beginning of data.
                                There might be a gap in the beginning. */
    int dataSize;	     /* size of data */
    Coord low, high;         /* record IDs */
    
    friend class RangeList;
    friend class BufMgrFull;
    
    RangeInfo *next, *prev;  /* for RangeList */
   
    char *buf;	             /* actual data buffer*/
    int bufSize;	     /* size of data and buffer */
    
  private:
    Boolean _dirty;
    int _inUse;
};

#endif
