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
  Revision 1.9  1997/10/10 21:13:40  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.8  1997/10/07 17:05:58  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.7  1996/12/18 15:35:12  jussi
  Merged GetGDataInMem() and GetTDataInMem().

  Revision 1.6  1996/12/12 22:02:17  jussi
  Added destructor.

  Revision 1.5  1996/12/03 20:38:51  jussi
  Revised completely to support concurrent requests and better
  handling of processed record ranges. Collapsed several interfaces
  into one Init/Get/Done interface.

  Revision 1.4  1996/11/23 21:21:40  jussi
  Made code use shared memory in MemMgr. Simplified some routines.

  Revision 1.3  1996/06/27 15:47:17  jussi
  Replaced method ClearGData() with ClearTData() which allows both
  TData and GData (which derives from TData) to be cleared.

  Revision 1.2  1995/09/05 22:14:14  jussi
  Added CVS header.
*/

#ifndef BufMgrFull_h
#define BufMgrFull_h

#include "BufMgr.h"
#include "RangeInfo.h"
#include "Init.h"
#include "MemMgr.h"
#include "TDataRangeList.h"
#include "RangeInfo.h"
#include "RangeInfoArray.h"

class BufMgrFull: public BufMgr {
  public:
    BufMgrFull(int bufSize);
    virtual ~BufMgrFull();

    /*
       For documentation on the public methods of this class, see
       the header file of the base class (BufMgr.h).
    */

    virtual void Clear();

    void ClearData(TData *data, char *attrName="recId", Coord granularity=1);

    virtual BMHandle InitGetRecs(TData *tdata, GData *gdata,
				 Interval *interval,
                                 Boolean tdataOnly = false,
                                 Boolean inMemoryOnly = false,
                                 Boolean randomized = false,
                                 Boolean asyncAllowed = false);

    virtual BMHandle SelectReady();

    virtual Boolean GetRecs(BMHandle handle, Boolean &isTData,
			    Interval *interval,
                            char *&buf);
	
    virtual void DoneGetRecs(BMHandle handle);

    virtual void FreeRecs(char *buf, Boolean dirty = false);

    virtual QPRange *GetProcessedRange(BMHandle handle);

    virtual void InitPolicy(BufPolicy::policy policy) {}

    virtual void PhaseHint(BufferPolicy::Phase phase) {
        _policy->PhaseHint(phase);
    }

    virtual void FocusHint(Coord id, TData *tdata, GData *gdata) {
        _policy->FocusHint(id, tdata, gdata);
    }

    virtual void ReleaseMemory(MemMgr::PageType type, char *buf, int pages) {
        int status = _memMgr->Deallocate(type, buf, pages);
        DOASSERT(status >= 0, "Could not deallocate memory");
    }

    void Print(TData *tdata);

    virtual void PrintStat();
    
  protected:
    /***********************************************************************
      Internal utility functions for getting data out of memory and
      manipulating record ranges
    ***********************************************************************/

    /* Calculate return parameters for a record range to be returned */
    void CalcRetArgs(Boolean isTData, Boolean isInMem,
                     TData *tdata, RangeInfo *rangeInfo,
                     Coord lowId, Coord highId, 
                     char *&buf, Coord &starRid,int &numRecs);
    
    /* Advance to next range of in-memory range list */
    Boolean GetNextRangeInMem(BufMgrRequest *req, RangeInfo *&range);
    
    /* Get in-memory GData/TData */
    Boolean GetDataInMem(BufMgrRequest *req, Coord &startVal, Coord &endVal,
                         int &numRecs, char *&buf);
    
    /* Initiate I/O to get disk-resident GData */
    Boolean InitGDataScan(BufMgrRequest *req);
    
    /* Initiate I/O to get disk-resident TData */
    Boolean InitTDataScan(BufMgrRequest *req);
    
    /* Consume data produced by I/O initiated in InitGDataScan/InitTDataScan */
    Boolean ScanDiskData(BufMgrRequest *req, Coord &startVal,
                         Coord &endVal, int &numRecs, char *&buf);

    /* Consume data from the pipe of a TData */
    Boolean ConsumePipeData(BufMgrRequest *req, DataSource *data,
                            char *buf, Coord &startRid, int &numRecs,
                            int &dataSize);

    /* Mark range as being in use (pinned) */
    void SetUse(RangeInfo *rangeInfo);
    
    /* Mark range as not being in use (unpinned) */
    void ClearUse(char *buf, Boolean dirty = false);
    
    /* Check integrity of range */
    void CheckRange(RangeInfo *range);
    
    /* Given data size, find buffer size that is a multiple of page size */
    int BufSize(int dataSize) {
        int pages = dataSize / _pageSize;
        if (dataSize % _pageSize != 0)
            pages++;
        return pages * _pageSize;
    }
    
    /*
       Allocate a range suitable for up to "size" bytes. 
       Return a range with size <= size bytes.
       Fail assertion if no space found.
    */
    RangeInfo *AllocRange(int size);

    /*
       Truncate rangeInfo by the buffer size from the beginning, 
       and return a new one for the truncated portion. 
       Arrange the pointers so that the remaining data are
       pointing to the right place.
    */
    RangeInfo *Truncate(RangeInfo *rangeInfo, int bufSize);

    /***********************************************************************
      Data structures for buffer manager's internal state
    ***********************************************************************/
    
    /* Arrays of TData/GData record ranges stored in memory */
    int _numArrays;
    RangeInfoArrays *_memoryRanges;
    
    /* Mapping of TData/GData to lists in _memoryRanges */
    TDataRangeList _tdataInMemory;
    
    /* List of buffer manager requests */
    BufMgrRequest _reqhead;
    
    /* List of "pinned" record ranges. Same as a page descriptor table */
    const int MaxReturnRanges = 2;
    RangeInfo *_returnRanges[MaxReturnRanges];
    
    /* Memory manager */
    MemMgr *_memMgr;
    
    /* Page size */
    int _pageSize;
    
    /* Last ready request */
    BufMgrRequest *_lastReadyReq;

    /***********************************************************************
      Policy data structures and victim eviction
    ***********************************************************************/
    
    /* Current and default buffer management policy */
    int _policyFlag;
    BufferPolicy *_policy;
    BufferPolicy *_defaultPolicy;
    
    /***********************************************************************
      Buffer manager statistics
    ***********************************************************************/

    int _tReturned;           /* # of TData bytes returned */
    int _tHits;	              /* # of TData bytes hit in buffer */
    int _gReturned;           /* # GData bytes returned */
    int _gHits;	              /* # of GData bytes hit in buffer */
    int _totalRanges;         /* # of ranges returned */
    int _totalBuf;            /* # of buffers occupied by ranges returned */
    int _totalData;           /* # of data bytes occupied by ranges returned */
    
    int _numGetRecs;          /* # of tdata->GetRecs() calls */
    int _totalGetRecBytes;    /* bytes received in tdata->GetRecs() */
    int _totalGetRecBufSize;  /* buffer space used for tdata->GetRecs() */

    int _seqIOs;              /* number of sequential I/Os */
    int _parIOs;              /* number of parallel I/Os */
};

#endif
