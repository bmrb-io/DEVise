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
  Revision 1.6  1997/10/07 17:06:06  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

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
#include "TData.h"
#include "AttrList.h"

static const int BMFULL_RECS_PER_BATCH = 1024;

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
        next = prev = 0;
	interval.Low = 0;
	interval.High = 0;
	interval.NumRecs = 0;
	interval.has_left = interval.has_right = false;
    }

    Boolean HasLeft() { return interval.has_left; }
    Boolean HasRight() { return interval.has_right; }
    Coord LeftAdjacent() { return interval.left_adjacent; }
    Coord RightAdjacent() { return interval.right_adjacent; }

    /* Get TData for this range */
    TData *GetTData() { return tdata; }

    /* Get record ID associated with this range */
    void RecIds(Coord &lowId, Coord &highId) {
        lowId = interval.Low;
        highId = interval.High;
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

    // given a low/high closed interval, return the records in this interval.
    // limit the number of records to BMFULL_RECS_PER_BATCH.
    // set numRecs to 0 if no such record found
    // return false only if something is wrong
    // nextVal is the right adjacent value of endVal or endVal+1 if endVal is
    // 		already the largest possible value of this attr of this TData 
    Boolean ReturnRecs(Coord lowVal, Coord highVal, char *&buf,
			Coord &startVal, Coord &endVal, int &numRecs, 
			Coord nextVal);

    // given pointer to a record and AttrName, find the value of this attribute
    Boolean GetAttrVal(char *record, char *attrName, double &value);

	// check the integrity of this RangeInfo
	void CheckIntegrity();

  protected:
    int listNum;             /* which list we are in */
    int posNum;		     /* position in the list */
    
    TData *tdata;

    char *data;	             /* pointer to beginning of data.
                                There might be a gap in the beginning. */
    int dataSize;	     /* size of data */
    char *buf;	             /* actual data buffer*/
    int bufSize;	     /* size of data and buffer */

    Interval interval;	// AttrName, Granularity, Low and High are all used

    friend class RangeList;
    friend class BufMgrFull;
    
    RangeInfo *next, *prev;  /* for RangeList */
   
  private:
    Boolean _dirty;
    int _inUse;
};

#endif
