/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.9  1997/10/10 21:13:38  liping
  The interface between TData and BufMgr and the interface between BufMgr and
  QueryProc were changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
          4. Granularity in the structure "Range"

  Revision 1.8  1997/10/07 17:05:56  liping
  RecId to Coord(double) changes of the BufMgr/QureyProc interface

  Revision 1.7  1997/01/11 20:59:31  jussi
  Fix for bug #106. Simplified processing of record links.

  Revision 1.6  1996/12/03 20:37:53  jussi
  Added definition of BufMgrRequest, and improved Init/Get/Done interface.

  Revision 1.5  1996/11/23 21:19:52  jussi
  Simplified code.

  Revision 1.4  1996/06/27 15:47:16  jussi
  Replaced method ClearGData() with ClearTData() which allows both
  TData and GData (which derives from TData) to be cleared.

  Revision 1.3  1996/01/15 16:53:48  jussi
  Added copyright notice and cleaned up the code a bit.

  Revision 1.2  1995/09/05 22:14:12  jussi
  Added CVS header.
*/

#ifndef BufMgr_h
#define BufMgr_h

#include "DeviseTypes.h"
#include "BufPolicy.h"
#include "RecId.h"
#include "BufferPolicy.h"
#include "DList.h"
#include "TData.h"
#include "GData.h"
#include "RangeList.h"
#include "QPRange.h"

/* Report info about what has been inserted/delete from the buffer manager */
class BufMgrCallback {
public:
    virtual void Inserted(TData *tdata, Coord low, Coord high) = 0;
    virtual void Deleted(TData *tdata, Coord low, Coord high) = 0;
};

DefinePtrDList(BufMgrCallbackList, BufMgrCallback *)

class BufMgr : public ReleaseMemoryCallback {
public:
    void RegisterCallback(BufMgrCallback *c);

    /* Reinitialize and free all buffers */
    virtual void Clear() = 0;

    /* Clear buffers occupied by given TData/GData */
    virtual void ClearData(TData *data, char *attrName = "recId", Coord granularith = 1) = 0;

    /* Information stored about a buffer manager request */
    struct BufMgrRequest {
        BufMgrRequest *next;              /* next request in list */
        BufMgrRequest *prev;              /* previous request in list */
        TData *tdata;                     /* requested TData */
        GData *gdata;                     /* requested GData */
        QPRange processed;                /* list of processed ranges */
        Coord low;                        /* start of requested record range */
        Coord high;                       /* end of requested record range */
	char *attrName;			  // name of sorted attr requested
	Coord granularity;		  /* requested granularity */
        Boolean asyncAllowed;             /* asynchronous I/O allowed */
        Boolean inMemoryOnly;             /* return in-memory data only */
        Boolean randomized;               /* return records in random order */
        Boolean needScanInit;             /* need initialization before scan */
        Boolean haveIO;                   /* true if I/O outstanding */
        TData::TDHandle tdhandle;         /* TData I/O handle */
        int bytesLeft;                    /* remaining bytes to transfer */
        RangeList *tdataInMemory;         /* TData ranges in memory */
        RangeList *gdataInMemory;         /* GData ranges in memory */
        RangeInfo *inMemoryHead;          /* head of in-memory range list */
        RangeInfo *inMemoryCur;           /* current in-memory record range */
        Coord currentRec;                 /* current record being considered */
        Boolean getRange;                 /* need to get scanLow, scanHigh */
        Coord scanLow;                    /* low ID of current GData range */
        Coord scanHigh;                   /* high ID of current GData range */
    };
    
    typedef BufMgrRequest *BMHandle;
    
    /*
       Prepare to retrieve records from TData and GData. If
       tdataOnly is false, return TData records only. If
       inMemoryOnly is false, return in-memory records only.
       if randomized is true, return records in random order.
       If asyncAllowed is true, asynchronous I/O can be used.
    */
    virtual BMHandle InitGetRecs(TData *tdata, GData *gdata,
				 Interval *interval,
                                 Boolean tdataOnly = false,
                                 Boolean inMemoryOnly = false,
                                 Boolean randomized = false,
                                 Boolean asyncAllowed = false) = 0;
    
    /*
       Return buffer manager handle for first open data stream
       that can return data. Similar to select() except returns
       only one ready stream.
    */
    virtual BMHandle SelectReady() = 0;

    /*
       Retrieve next batch of records. Return false if no more records
       are available. Flag isTData indicates type of records returned.
       The record range is indicated by startRecId and numRecs.
    */
    virtual Boolean GetRecs(BMHandle handle, Boolean &isTData,
			    Interval *interval,
                            // Coord &startRecId, int &numRecs,
                            char *&buf) = 0;
    
    /*
       Tell buffer manager that no more calls to GetRecs() are planned.
    */
    virtual void DoneGetRecs(BMHandle handle) = 0;
    
    /*
       Free memory space taken by recrods returned by GetRecs(). Buf
       is the address return by GetRecs().
    */
    virtual void FreeRecs(char *buf, Boolean dirty = false) = 0;
    
    /* Return pointer to processed range */
    virtual QPRange *GetProcessedRange(BMHandle handle) = 0;

    /* Initialize buffer manager policy */
    virtual void InitPolicy(BufPolicy::policy policy) = 0;
    
    /* Provide a hint about which phase we are in */
    virtual void PhaseHint(BufferPolicy::Phase phase) = 0;
    
    /* Provide a hint about the center of focus */
    virtual void FocusHint(Coord id, TData *tdata, GData *gdata) = 0;
    
    /* Receive notification for released chunk of memory */
    virtual void ReleaseMemory(MemMgr::PageType type,
                               char *buf, int pages) = 0;

    /* Print in-memory record ranges of given TData */
    virtual void Print(TData *tdata) = 0;
    
    /* Print buffer maager statistics */
    virtual void PrintStat() = 0;
    
  protected:
    /*
       These methods are used by derived class to report records
       inserted/deleted from buffer.
    */
    void ReportInserted(TData *tdata, Coord low, Coord high);
    void ReportDeleted(TData *tdata, Coord low, Coord high);
    
    BufMgrCallbackList _callbacks;
};

#endif
