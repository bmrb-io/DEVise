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
  Revision 1.12  1996/12/03 20:38:50  jussi
  Revised completely to support concurrent requests and better
  handling of processed record ranges. Collapsed several interfaces
  into one Init/Get/Done interface.

  Revision 1.11  1996/11/23 21:21:39  jussi
  Made code use shared memory in MemMgr. Simplified some routines.

  Revision 1.10  1996/08/08 21:00:50  beyer
  Added some debugging statements

  Revision 1.9  1996/07/12 04:01:02  jussi
  Clenaed up the code.

  Revision 1.8  1996/06/27 15:47:16  jussi
  Replaced method ClearGData() with ClearTData() which allows both
  TData and GData (which derives from TData) to be cleared.

  Revision 1.7  1996/05/14 17:44:06  jussi
  Added checking of the return value from the new operator.

  Revision 1.6  1996/01/15 21:57:10  jussi
  Minor fixes.

  Revision 1.5  1995/12/14 21:09:07  jussi
  Replaced 0x%x with 0x%p.

  Revision 1.4  1995/12/14 18:27:16  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.3  1995/12/14 15:38:32  jussi
  Added copyright notice and made small fixes.

  Revision 1.2  1995/09/05 22:14:13  jussi
  Added CVS header.
*/

#include <stdio.h>

#include "Init.h"
#include "Exit.h"
#include "BufMgrFull.h"
#include "BufferPolicy.h"
#include "TDataRangeList.h"
#include "RangeInfo.h"
#include "RangeInfoArray.h"
#include "RangeList.h"
#include "TDataAttr.h"
#include "GData.h"
#include "BufferFifo.h"
#include "BufferFocal.h"
#include "BufferLifo.h"
#include "BufferRnd.h"
#include "BufferLru.h"
#include "QPRange.h"

#define DEBUGLVL 0

/*
   Number of skips to make in randomized processing.
*/
static const int BM_RANDOM_SKIPS = 10;

BufMgrFull::BufMgrFull(int bufSize)
{
    _tReturned = _tHits = _gReturned = _gHits = 0;
    _totalRanges = _totalBuf = _totalData = 0;
    _numGetRecs = _totalGetRecBytes = _totalGetRecBufSize = 0;
    
    char *policy = "none";
    if (Init::Policy() == BufPolicy::FOCAL) {
        policy = "Focal";
        _policy = new BufferFocal;
    } else if (Init::Policy() == BufPolicy::LIFO) {
        policy = "LIFO";
        printf("Lifo policy\n");
        _policy = new BufferLifo;
    } else if (Init::Policy() == BufPolicy::RND) {
        policy = "Random";
        printf("Rnd policy\n");
        _policy = new BufferRnd;
    } else {
        policy = "FIFO";
        _policy = new BufferFifo;
    }
    DOASSERT(_policy, "Out of memory");
    _policy->Info(_numArrays, _policyFlag);
    
    _pageSize = Init::PageSize();
    int bufPages = MAX(bufSize / _pageSize, 1);
    
    printf("Creating a %.2f MB buffer pool (%s policy)\n",
           bufSize / 1048576.0, policy);

    int status;
    _memMgr = new MemMgr(bufPages, _pageSize, status);
    DOASSERT(_memMgr && status >= 0, "Cannot create memory manager");

    status = _memMgr->SetMaxUsage(MemMgr::Cache, (int)(0.8 * bufPages));
    DOASSERT(status >= 0, "Cannot set cache page usage limit");
    status = _memMgr->SetMaxUsage(MemMgr::Buffer, (int)(0.2 * bufPages));
    DOASSERT(status >= 0, "Cannot set buffer page usage limit");

    _memoryRanges = new RangeInfoArrays(_numArrays, bufPages);
    DOASSERT(_memoryRanges, "Out of memory");
    
    for(int i = 0; i < MaxReturnRanges; i++)
        _returnRanges[i] = NULL;
    
    _defaultPolicy = new BufferFifo;
    DOASSERT(_defaultPolicy, "Out of memory");

    _reqhead.next = 0;
    _reqhead.prev = 0;
    _lastReadyReq = _reqhead.next;
}

/*
   Find next record range in memory that fits in the [req->low,req->high]
   bounds. Return true if found such range, false otherwise. The list
   pointed to by req->inMemoryNext is either the in-memory GData record
   range list, or the in-memory TData record range list.
*/

Boolean BufMgrFull::GetNextRangeInMem(BufMgrRequest *req, RangeInfo *&range)
{
    while (req->inMemoryNext != req->inMemoryHead) {
        range = req->inMemoryNext;
        req->inMemoryNext = range->next;

        RecId low, high;
        range->RecIds(low, high);

        if (!(low > req->high || high < req->low)) {
            /* Return matching in-memory range. */
#if DEBUGLVL >= 3
	    printf("Found in-memory range [%ld,%ld]\n", low, high);
#endif
            return true;
        }
        
        if (low > high)
            break;
    }
    
    return false;
}

/*
   Getting GData from memory is easy because we know that no other
   data has been returned yet, so we don't need to check for
   unprocessed ranges and compute the intersection of in-memory GData
   ranges and the unprocessed ranges. Just return each in-memory GData
   range to the caller.
*/

Boolean BufMgrFull::GetGDataInMem(BufMgrRequest *req, RecId &startRecId,
                                  int &numRecs, char *&buf)
{
#if DEBUGLVL >= 3
    printf("Looking for in-memory GData in range [%ld,%ld]\n",
           req->low, req->high);
#endif
    
    RangeInfo *inMem;
    if (!GetNextRangeInMem(req, inMem)) {
#if DEBUGLVL >= 3
        printf("End of in-memory GData ranges\n");
#endif
        return false;
    }

#if DEBUGLVL >= 3
    printf("Next in-memory GData range is [%ld,%ld]\n",
           inMem->low, inMem->high);
#endif

    CalcRetArgs(false, true, req->gdata, inMem, req->low, req->high,
                buf, startRecId, numRecs);
    SetUse(inMem);

    return true;
}

/*
   Getting TData from memory is more complicated than getting GData.
   For each in-memory TData range, we have to compare it to the
   unprocessed record ranges and compute the intersection. An in-memory
   TData range may overlap several, discontiguous unprocessed ranges,
   so we call GetNextRangeInMem() only when the "current" in-memory
   TData range has been fully consumed.

   The "current" in-memory TData range is indicated by the inMemoryNext
   pointer in the request structure.
*/

Boolean BufMgrFull::GetTDataInMem(BufMgrRequest *req, RecId &startRecId,
                                  int &numRecs, char *&buf)
{
#if DEBUGLVL >= 3
    printf("Looking for in-memory TData in range [%ld,%ld]\n",
           req->low, req->high);
#endif
    
    while (1) {
        RangeInfo *inMem = req->inMemoryCur;

        /*
           See if the last piece of processed data was at the end of
           an in-memory TData range, in which case we need to advance
           to the next in-memory TData range. Also do this if advancing
           is explicitly requested (req->getRange is true).
        */

        if (req->getRange || req->currentRec > inMem->high) {
            if (!GetNextRangeInMem(req, inMem)) {
                /* All in-memory TData ranges have been processed. */
                return false;
            }
            req->inMemoryCur = inMem;
#if DEBUGLVL >= 3
            printf("Next in-memory TData range is [%ld,%ld]\n",
                   inMem->low, inMem->high);
#endif
            req->currentRec = inMem->low;
            if (req->currentRec < req->low)
                req->currentRec = req->low;
            req->getRange = false;
        }

        RecId uLow, uHigh;
        Boolean noHigh = req->processed.NextUnprocessed(req->currentRec,
                                                        uLow, uHigh);
        if (noHigh || uHigh > inMem->high)
            uHigh = inMem->high;
        
#if DEBUGLVL >= 3
        printf("Next unprocessed in-memory TData range is [%ld,%ld]\n",
               uLow, uHigh);
#endif
            
        if (uLow > req->high) {
            /*
               Next unprocessed range is beyond the range requested by
               the caller. In-memory TData ranges are sorted in recid
               order, so there's no point in going further to the "right"
               because none of the unprocessed ranges there will be
               interesting. We're done.
            */
#if DEBUGLVL >= 3
            printf("End of unprocessed in-memory TData ranges\n");
#endif
            return false;
        }

        if (uHigh > req->high)
            uHigh = req->high;
        
        if (uLow <= inMem->high) {
            /*
               Return intersection of in-memory range and unprocessed
               range to the caller.
            */
            CalcRetArgs(true, true, req->tdata, inMem,
                        uLow, uHigh, buf, startRecId, numRecs);
            SetUse(inMem);
            req->currentRec = startRecId + numRecs;
            return true;
        }

        /* Force next in-memory range to be retrieved at top of loop. */
        req->getRange = true;
    }
    
    DOASSERT(0, "Invalid state");
    return false;
}

/*
   Getting GData from disk is similar to getting TData from memory.
   For each disk-resident GData range, we have to compare it to the
   unprocessed record ranges and compute the intersection. A disk-resident
   GData range may overlap several, discontiguous unprocessed ranges,
   so we call NextRange() only when the "current" disk-resident GData
   range has been fully consumed.

   The "current" disk-resident GData range is indicated by the scanLow
   and scanHigh variables in the request structure.
*/

Boolean BufMgrFull::InitGDataScan(BufMgrRequest *req)
{
#if DEBUGLVL >= 3
    printf("Looking for disk-resident GData in range [%ld,%ld]\n",
           req->low, req->high);
#endif
    
    RecId rangeLow, rangeHigh;

    while (1) {

        if (req->getRange) {
            /* Find next GData range that overlaps [req->low,req->high]. */
            do {
                if (!req->gdata->NextRange(req->scanLow, req->scanHigh) ||
                    req->scanLow > req->high) {
#if DEBUGLVL >= 3
		    printf("End of disk-resident GData ranges\n");
#endif
                    return false;
                }
            } while (req->scanHigh < req->low);
                
            /* Truncate [scanLow,scanHigh] to fit inside [low,high]. */
            if (req->scanLow < req->low)
                req->scanLow = req->low;
            if (req->scanHigh > req->high)
                req->scanHigh = req->high;
                
            req->currentRec = req->scanLow;
            req->getRange = false;
        }

#if DEBUGLVL >= 3
        printf("Checking GData overlap [%ld,%ld] [%ld,%ld] %ld\n",
               req->scanLow, req->scanHigh, req->low, req->high,
               req->currentRec);
#endif
        /*
           Check overlap between [scanLow,scanHigh] and unprocessed
           ranges.
        */
        Boolean noHigh = req->processed.NextUnprocessed(req->currentRec,
                                                        rangeLow, rangeHigh);
        if (noHigh ||  rangeHigh > req->scanHigh)
            rangeHigh = req->scanHigh;
        
#if DEBUGLVL >= 3
        printf("Next unprocessed GData range is [%ld,%ld] [%ld,%ld]\n",
               rangeLow, rangeHigh, req->scanLow, req->scanHigh);
#endif
            
        req->currentRec = req->scanHigh + 1;

        if (rangeLow <= req->scanHigh)
            break;

        /*
           No more unprocessed ranges that can be fulfilled from the
           current GData range. Go back and get another GData range.
        */
        req->getRange = true;
    }
            
    if (rangeHigh > req->high)
        rangeHigh = req->high;
        
    req->bytesLeft = (rangeHigh - rangeLow + 1) * req->gdata->RecSize();
    req->tdhandle = req->gdata->InitGetRecs(rangeLow, rangeHigh,
                                            req->asyncAllowed, this);
#if DEBUGLVL >= 3
    printf("Submitted I/O request 0x%p for GData 0x%p [%ld,%ld]\n",
           (void *)req->tdhandle, req->gdata, rangeLow, rangeHigh);
#endif

    return true;
}

/*
   Getting TData from disk is similar to getting GData from disk, except
   that all records are available in TData (for disk-resident GData, we
   used gdata->NextRange() to figure out which GData record ranges were
   available). So, we just take each unprocessed range and initiate the
   corresponding TData I/O.
*/

Boolean BufMgrFull::InitTDataScan(BufMgrRequest *req)
{
#if DEBUGLVL >= 3
    printf("Looking for disk-resident TData in range [%ld,%ld]\n",
           req->low, req->high);
#endif    

    RecId rangeLow, rangeHigh;

    Boolean noHighId = req->processed.NextUnprocessed(req->currentRec,
                                                      rangeLow, rangeHigh);

    if (rangeLow > req->high) {
#if DEBUGLVL >= 3
        printf("End of disk-resident TData ranges\n");
#endif    
        return false;
    }
            
    if (noHighId || rangeHigh > req->high)
        rangeHigh = req->high;

#if DEBUGLVL >= 3
    printf("Next unprocessed TData range is [%ld,%ld]\n", rangeLow, rangeHigh);
#endif    

    req->currentRec = rangeHigh + 1;
    /* In randomized mode, skip over a large number of records */
    if (req->randomized)
        req->currentRec += (req->high - req->low) / BM_RANDOM_SKIPS;
    /* Wrap around if we moved past end of range */
    if (req->currentRec > req->high)
        req->currentRec = req->low;

    req->bytesLeft = (rangeHigh - rangeLow + 1) * req->tdata->RecSize();
    req->tdhandle = req->tdata->InitGetRecs(rangeLow, rangeHigh,
                                            req->asyncAllowed, this);
#if DEBUGLVL >= 3
    printf("Submitted I/O request 0x%p for TData 0x%p [%ld,%ld]\n",
           (void *)req->tdhandle, req->tdata, rangeLow, rangeHigh);
#endif

    return true;
}

/*
   This function consumes data produced by an I/O which was initiated
   in InitGDataScan() or InitTDataScan(). False is returned when
   all data produced by I/O has been consumed.
*/

Boolean BufMgrFull::ScanDiskData(BMHandle req, RecId &startRid,
                                 int &numRecs, char *&buf)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::ScanDiskData handle 0x%p\n", req);
#endif
    DOASSERT(req, "Invalid request handle");
    DOASSERT(req->haveIO, "No I/O outstanding");
    DOASSERT(req->bytesLeft >= 0, "Inconsistent state");

    Boolean isTData = (req->gdata == NULL);
    TData *tdata = (isTData ? req->tdata : req->gdata);
    DOASSERT(tdata, "Invalid TData/GData");

    if (req->bytesLeft == 0) {
#if DEBUGLVL >= 3
        printf("End of I/O on TData %s\n", tdata->GetName());
#endif
        return false;
    }

    RangeInfo *range = AllocRange(BufSize(req->bytesLeft));

    int dataSize;
    Boolean status = tdata->GetRecs(req->tdhandle, range->buf,
                                    range->bufSize, startRid,
                                    numRecs, dataSize);
    DOASSERT(status && dataSize > 0, "Cannot get TData");

#if DEBUGLVL >= 3
    printf("Got range [%ld,%ld] from TData %s\n",
           startRid, startRid + numRecs - 1, tdata->GetName());
#endif

    _numGetRecs++;
    _totalGetRecBufSize += range->bufSize;
    _totalGetRecBytes += dataSize;
        
    range->dataSize = dataSize;
    range->low = startRid;
    range->high = startRid + numRecs - 1;
    range->tdata = tdata;
    range->data = range->buf;
        
    CalcRetArgs(isTData, false, tdata, range, range->low,
                range->high, buf, startRid, numRecs);
    SetUse(range);

    /*
       Buffer size required for next I/O (if one exists) is somewhat
       smaller.
    */
    req->bytesLeft -= dataSize;

    /* Place new range into policy arrays */
    int arrayNum, pos;
    if (_policyFlag & BufferPolicy::ReportPlacement)
        _policy->Placement(range, _memoryRanges, arrayNum, pos);
    else
        _defaultPolicy->Placement(range, _memoryRanges, arrayNum, pos);
    
    _memoryRanges->Insert(arrayNum, pos, range);
    range->listNum = arrayNum;
    
    /* Put into list of ranges for this TData */
    int numDisposed;
    RangeInfo **disposed;
    RangeList *rangeList = _tdataInMemory.Get(tdata);
    rangeList->Insert(range, RangeList::MergeIgnore,
                      numDisposed, disposed);
    
    ReportInserted(tdata, range->low, range->high);
    
    return true;
}

BufMgr::BMHandle BufMgrFull::InitGetRecs(TData *tdata, GData *gdata,
                                         RecId lowId, RecId highId,
                                         Boolean tdataOnly,
                                         Boolean inMemoryOnly,
                                         Boolean randomized,
                                         Boolean asyncAllowed)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::InitGetRecs [%ld,%ld] with tdata 0x%p, gdata 0x%p\n",
           lowId, highId, tdata, gdata);
#endif

    DOASSERT(tdata->RecSize() >= 0, "Cannot handle variable records");

    RecId firstId;
    RecId lastId;
    Boolean hasFirst = tdata->HeadID(firstId);
    Boolean hasLast = tdata->LastID(lastId);
    DOASSERT(hasFirst && hasLast && lowId >= firstId && highId <= lastId,
             "Invalid record IDs");

    BufMgrRequest *req = new BufMgrRequest;
    DOASSERT(req, "Out of memory");
    if (_reqhead.next)
        _reqhead.next->prev = req;
    req->next = _reqhead.next;
    req->prev = &_reqhead;
    _reqhead.next = req;

    req->tdata = tdata;
    req->gdata = (tdataOnly ? NULL : gdata);
    req->low = lowId;
    req->high = highId;
    req->asyncAllowed = asyncAllowed;
    req->inMemoryOnly = inMemoryOnly;
    req->randomized = randomized;
    req->needScanInit = true;
    req->haveIO = false;
    req->tdhandle = 0;
    req->bytesLeft = 0;
    req->tdataInMemory = NULL;
    req->gdataInMemory = NULL;
    req->inMemoryHead = NULL;
    req->inMemoryNext = NULL;
    req->inMemoryCur = NULL;
    req->currentRec = 0;
    req->getRange = false;
    req->scanLow = 0;
    req->scanHigh = 0;

    /* Set up parameters so that what's in memory is processed first. */
    req->tdataInMemory = _tdataInMemory.Get(tdata);
    if (req->gdata != NULL)
        req->gdataInMemory = _tdataInMemory.Get(req->gdata);

    if (req->gdata != NULL)
        req->gdata->InitConvertedIterator();

    return req;
}

/*
   Return buffer manager handle for first open data stream
   that can return data. Similar to select() except returns
   only one ready stream.
*/

BufMgr::BMHandle BufMgrFull::SelectReady()
{
#ifndef CONCURRENT_IO
    // For now, return first request on the list. Later enable
    // code below for concurrent I/O.
    return _reqhead.next;
#endif

    /*
       Make one loop through list of requests to see if any one
       of them is ready to deliver data. Start from request that
       follows last ready request -- we don't want the same request
       to be processed before processing the other requests.
       Wrap around when end of list is reached, and stop when
       we reach the place we started from.
    */

    if (_reqhead.next == NULL || !_lastReadyReq) {
        /* List of requests is empty */
        return NULL;
    }

    BufMgrRequest *last = _lastReadyReq;
    DOASSERT(last, "Inconsistent state");

    do {
        _lastReadyReq = _lastReadyReq->next;
        if (!_lastReadyReq)
            _lastReadyReq = _reqhead.next;
        DOASSERT(_lastReadyReq, "Inconsistent state");

        /* Use req as a shorthand for _lastReadyReq */
        BufMgrRequest *req = _lastReadyReq;

        if (req->gdataInMemory || req->tdataInMemory) {
#if DEBUGLVL >= 3
            printf("Request 0x%p has unprocessed in-memory data\n", req);
#endif
            return req;
        }
        if (req->haveIO) {
            if (req->tdhandle->IsDirectIO()) {
#if DEBUGLVL >= 3
                printf("Request 0x%p can perform direct I/O\n", req);
#endif
                return req;
            }
            DataSource *source = req->tdata->GetDataSource();
            if (req->gdata)
                source = req->gdata->GetDataSource();
            if (source->SupportsAsyncIO() && source->NumPipeData() > 0) {
#if DEBUGLVL >= 3
                printf("Request 0x%p has %d units of data in pipe\n", req,
                       source->NumPipeData());
#endif
                return req;
            }
        }
    } while (_lastReadyReq != last);

#if DEBUGLVL >= 3
    printf("Did not find any active request\n");
#endif

    /* Don't know what to do yet -- block on first request */
    return _reqhead.next;
}

/*
   Retrieve next batch of records.  Return TRUE if still more batches.
   Return true value in inMemory if buf holds the records. If inMemory
   is false, the caller must call FetchRecs() to get the buf pointer.
*/

Boolean BufMgrFull::GetRecs(BMHandle req, Boolean &isTData,
                            RecId &startRid, int &numRecs, char *&buf)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::GetRecs handle 0x%p\n", req);
#endif
    DOASSERT(req, "Invalid request handle");

    /* See if we can find GData in memory */

    if (req->gdataInMemory) {
        if (!req->inMemoryHead) {
            /* Initialize for in-memory GData access */
            int numRanges;
            req->gdataInMemory->GetRangeList(numRanges, req->inMemoryHead);
            req->inMemoryNext = req->inMemoryHead->next;
        }
        if (GetGDataInMem(req, startRid, numRecs, buf)) {
            isTData = false;
            return true;
        }
        req->gdataInMemory = NULL;
        req->inMemoryHead = NULL;
        req->inMemoryNext = NULL;
    }

    /* See if we can find TData in memory */

    if (req->tdataInMemory) {
        if (!req->inMemoryHead) {
            /* Initialize for in-memory TData access */
            int numRanges;
            req->tdataInMemory->GetRangeList(numRanges, req->inMemoryHead);
            req->inMemoryNext = req->inMemoryHead->next;
            req->getRange = true;
        }
        if (GetTDataInMem(req, startRid, numRecs, buf)) {
            isTData = true;
            return true;
        }
        req->tdataInMemory = NULL;
        req->inMemoryHead = NULL;
        req->inMemoryNext = NULL;
        req->inMemoryCur = NULL;
    }

    if (req->inMemoryOnly)
        return false;

    /* See if we can find GData on disk */

    if (req->gdata != NULL) {
        if (req->needScanInit) {
            /* Initialize for disk-resident GData scan */
            req->getRange = true;
            req->haveIO = InitGDataScan(req);
            req->needScanInit = false;
        }
        while (req->haveIO) {
            if (ScanDiskData(req, startRid, numRecs, buf)) {
                isTData = false;
                return true;
            }
            req->gdata->DoneGetRecs(req->tdhandle);
            req->haveIO = InitGDataScan(req);
        }
        req->gdata->DoneConvertedIterator();
        req->gdata = NULL;
        req->needScanInit = true;
    }

    /* See if we can find TData on disk */

    if (req->tdata != NULL) {
        if (req->needScanInit) {
            /* Initialize for disk-resident TData scan */
            req->currentRec = req->low;
            req->haveIO = InitTDataScan(req);
            req->needScanInit = false;
        }
        while (req->haveIO) {
            if (ScanDiskData(req, startRid, numRecs, buf)) {
                isTData = true;
                return true;
            }
            req->tdata->DoneGetRecs(req->tdhandle);
            req->haveIO = InitTDataScan(req);
        }
        req->tdata = NULL;
    }
    
    return false;
}

/*
   The only termination needed is when we are in the middle of returning
   disk-resident GData or TData data. Tell the GData or TData object
   that the fetch is done. For GData, terminate the iterator of the
   conversion list.
*/

void BufMgrFull::DoneGetRecs(BufMgr::BMHandle req)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::DoneGetRecs handle 0x%p\n", req);
#endif
    DOASSERT(req, "Invalid request handle");

    if (!req->tdataInMemory && !req->gdataInMemory) {
        if (req->gdata != NULL) {
            if (req->haveIO)
                req->gdata->DoneGetRecs(req->tdhandle);
            req->gdata->DoneConvertedIterator();
        } else if (req->tdata != NULL) {
            if (req->haveIO)
                req->tdata->DoneGetRecs(req->tdhandle);
        }
    }

    req->prev->next = req->next;
    if (req->next)
        req->next->prev = req->prev;

    _lastReadyReq = _reqhead.next;

    delete req;
}

/*
   Free buffer space and mark records dirty or clean.
*/

void BufMgrFull::FreeRecs(char *buf, Boolean dirty)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::FreeRecs buf 0x%p, dirty %d\n", buf, dirty);
#endif
    ClearUse(buf, dirty);
}

/*
   Return pointer to processed range.
*/

QPRange *BufMgrFull::GetProcessedRange(BMHandle req)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::GetProcessedRange handle 0x%p\n", req);
#endif
    DOASSERT(req, "Invalid request handle");

    return &req->processed;
}

/*
   Mark record range as being in use. This is the same as a Pin operation
   in most database systems.
*/

void BufMgrFull::SetUse(RangeInfo *range)
{
    for(int i = 0; i < MaxReturnRanges; i++) {
        if (_returnRanges[i] == NULL) {
            range->SetUse();
            _returnRanges[i] = range;
            return;
        }
    }

    DOASSERT(0, "Cannot find free slot");
}

/*
   Mark record range as not being used anymore. This is the same as an
   UnPin operation in most database systems.
*/

void BufMgrFull::ClearUse(char *buf, Boolean dirty)
{
    for(int i = 0; i < MaxReturnRanges; i++) {
        RangeInfo *range = _returnRanges[i];
        if (!range)
            continue;
        char *start = (char *)range->GetData();
        char *end = start + range->DataSize() - 1;
        if (buf >= start && buf <= end) {
            if (dirty)
                range->SetDirty();
            range->ClearUse();
            _returnRanges[i] = NULL;
            return;
        }
    }

    DOASSERT(0, "Cannot find buffer");
}

/*
   Calculate the return parameters, given a range to return to
   the buffer manager's client.
*/

void BufMgrFull::CalcRetArgs(Boolean isTData, Boolean isInMemory,
                             TData *tdata, RangeInfo *range,
                             RecId lowId, RecId highId, char *&buf,
                             RecId &startRid, int &numRecs)
{
    CheckRange(range);

    RecId low, high;
    range->RecIds(low, high);
    numRecs = high - low + 1;

    /* Set up initial values of return parameters */
    buf = (char *)range->GetData();
    int recSize = tdata->RecSize();
    startRid = low;

    /* Modify return parameters */
    if (lowId > low) {
        int diff = lowId - low;
        startRid = lowId;
        buf += diff * recSize;
        numRecs -= diff;
    }
        
    if (highId < high) {
        int diff = high - highId;
        numRecs -= diff;
    }

    DOASSERT(numRecs >= 0, "Invalid number of records");

    int bytes = numRecs * recSize;
    _totalBuf += range->BufSize();
    _totalData += range->DataSize();
    _totalRanges++;

    if (isTData ) {
        _tReturned += bytes;
        if (isInMemory)
            _tHits += bytes;
    } else {
        _gReturned += bytes;
        if (isInMemory)
            _gHits += bytes;
    }
}

/*
   Check consistency of range.
*/

void BufMgrFull::CheckRange(RangeInfo *range)
{
    if (range->data >= range->buf
        && range->data < range->buf + range->bufSize
        && range->DataSize() + range->GapSize() <= range->bufSize
        && range->high >= range->low)
    return;
    
    printf("Range error: buf 0x%p, data 0x%p, id [%ld,%ld], buf %d, data %d\n",
           range->buf, range->data, range->low, range->high,
           range->BufSize(), range->DataSize());
    DOASSERT(0, "Invalid range");
}

void BufMgrFull::Print(TData *tdata)
{
    RangeList *rangeList = _tdataInMemory.Get(tdata);
    int num;
    RangeInfo *head;
    rangeList->GetRangeList(num, head);

    printf("Low\tHigh\tData\tBuf\tGap\n");

    for(RangeInfo *cur = head->next; cur != head; cur = cur->next) {
        printf("%ld\t%ld\t%d\t%d\t%d\n", cur->low, cur->high,
               cur->DataSize(), cur->BufSize(), cur->GapSize());
    }
}

void BufMgrFull::PrintStat()
{
#if DEBUGLVL >= 3
    _memoryRanges->PrintStat();
#endif

    printf("%d bytes retrieved\n", _totalGetRecBytes);
    if (_tReturned > 0)
        printf("TData: returned %d, hits %d, %.2f%%\n",
               _tReturned, _tHits, 100.0 * _tHits / _tReturned);
    if (_gReturned > 0)
        printf("GData: returned %d, hits %d, %.2f%%\n",
               _gReturned, _gHits, 100.0 * _gHits / _gReturned);
    if (_tReturned + _gReturned > 0)
        printf("Total: returned %d, hits %d, %.2f%%\n",
               _tReturned + _gReturned, _tHits + _gHits,
               100.0 * (_tHits + _gHits) / (_tReturned + _gReturned));
    
#if DEBUGLVL >= 3
    printf("%d bufs returned: avg buf size %f, avg data size: %f\n",
           _totalRanges, 1.0 * _totalBuf / _totalRanges,
           1.0 * _totalData / _totalRanges);
    
    printf("%d totalGetRecs, avg input Buf %.2f, avg size %.2f\n",
           _numGetRecs, 1.0 * _totalGetRecBufSize / _numGetRecs,
           1.0 * _totalGetRecBytes / _numGetRecs);
#endif
}

/*
   Clear and deallocate all memory buffers occupied by TData and GData.
*/

void BufMgrFull::Clear()
{
    /* Free all ranges that have been allocated. */
    int numArrays = _memoryRanges->NumArrays();
    for(int i = 0; i < numArrays; i++) {
        _memoryRanges->InitIterator(i);
        while (_memoryRanges->More(i)) {
            RangeInfo *range = _memoryRanges->Next(i);
            int status = _memMgr->Deallocate(MemMgr::Cache,
                                             (char *)range->GetBuffer(),
                                             range->BufSize() / _pageSize);
            DOASSERT(status >= 0, "Could not deallocate memory");
        }
    }
    
    _memoryRanges->Clear();
    
    _tdataInMemory.Clear();
    
    _policy->Clear();
    _defaultPolicy->Clear();

#if DEBUGLVL >= 5
    printf("Memory manager's allocation table:\n");
    _memMgr->Dump();
#endif
}

/*
   Clear and deallocate memory buffers occupied by specified TData/GData.
*/

void BufMgrFull::ClearData(TData *data)
{
    int numArrays = _memoryRanges->NumArrays();
    for(int i = 0; i < numArrays; i++) {
        int size = _memoryRanges->Size(i);
        int j = 0;
        while (j < size) {
            RangeInfo *range = _memoryRanges->GetRange(i,j);
            if (range->GetTData() != data) {
                j++;
                continue;
            }

            /* clear this one */
            _tdataInMemory.Get(range->GetTData())->Delete(range);
            (void)_memoryRanges->Delete(i, j);
            ReportDeleted(data, range->low, range->high);
            int status = _memMgr->Deallocate(MemMgr::Cache,
                                             (char *)range->GetBuffer(),
                                             range->BufSize() / _pageSize);
            DOASSERT(status >= 0, "Could not deallocate memory");
            size--;
        }
    }
}

/*
   Allocate a range suitable for up to "size" bytes. 
   Return a range with size <= size bytes.
   Fail assertion if no space found.
*/

RangeInfo *BufMgrFull::AllocRange(int size)
{
#if DEBUGLVL >= 3
    printf("BufMgrFull::AllocRange size %d\n", size);
#endif

    /* Allocate full pages */
    size = BufSize(size);

    char *buf;
    int pages = size / _pageSize;
    int status = _memMgr->Try(MemMgr::Cache, buf, pages);

    /* Did we successfully get at least some memory? */
    if (status >= 0) {
        RangeInfo *range = new RangeInfo;
        DOASSERT(range, "Out of memory");
        range->buf = buf;
        range->bufSize = pages * _pageSize;
#if DEBUGLVL >= 5
        printf("Returning %d bytes of cache space\n", range->bufSize);
#endif
        return range;
    }
    
    /* No more memory available. Pick a victim. */

#if DEBUGLVL >= 5
    printf("Picking victim\n");
#endif

    int arrayNum, pos;
    Boolean found = false;
    if (_policyFlag & BufferPolicy::ReportVictim)
        found = _policy->PickVictim(_memoryRanges, arrayNum, pos);
    else
        found = _defaultPolicy->PickVictim(_memoryRanges, arrayNum, pos);
    DOASSERT(found, "Cannot find buffer victim");
        
    RangeInfo *range = _memoryRanges->GetRange(arrayNum, pos);
    DOASSERT(!range->InUse(), "Buffer victim in use");
    
#if DEBUGLVL >= 5
    printf("Victim is [%ld,%ld], data %d, buf %d, gap %d\n",
           range->low, range->high, range->DataSize(),
           range->BufSize(), range->GapSize());
#endif
    
    if (range->Dirty()) {
        /* Write dirty range out*/
        range->tdata->WriteRecs(range->low,
                                range->high - range->low + 1,
                                range->GetData());
        range->ClearDirty();
    }

    /*
       If victim range is larger than requested size by at least
       one page, then go and return the excess space.
    */

    if (range->DataSize() + range->GapSize() - size >= _pageSize) {
        /* Still room left for data in this range */
#if DEBUGLVL >= 5
        printf("Truncating victim by %d bytes, leaving %d bytes\n",
               size, range->DataSize() + range->GapSize() - size);
#endif
        range = Truncate(range, size);
    } else {
        _tdataInMemory.Get(range->GetTData())->Delete(range);
        (void)_memoryRanges->Delete(arrayNum, pos);
        ReportDeleted(range->GetTData(), range->low, range->high);
    }
    
#if DEBUGLVL >= 3
    printf("Returning %d bytes of cache space\n", range->bufSize);
#endif
    
    DOASSERT(range->bufSize % _pageSize == 0, "Invalid buffer size");
    
    return range;
}


/*
   Truncate rangeInfo by the buffer size from the beginning,
   and return a new one for the truncated portion.
   Arrange the pointers so that the remaining data are
   pointing to the right place.
*/

RangeInfo *BufMgrFull::Truncate(RangeInfo *oldRange, int bufSize)
{
    int recSize = oldRange->GetTData()->RecSize();

#if DEBUGLVL >= 5
    printf("Truncate %d, recSize = %d\n", bufSize, recSize);
#endif
    
    DOASSERT(bufSize > 0 && bufSize % _pageSize == 0,
             "Invalid truncation request");

    /* Figure out # of records truncated */
    int dataTruncated = (bufSize - oldRange->GapSize());
    if (dataTruncated > 0) {
        int numRecs = dataTruncated / recSize;
        if (dataTruncated % recSize != 0)
            numRecs++;
        if (numRecs > 0)
            ReportDeleted(oldRange->GetTData(), oldRange->low,
                          oldRange->low + numRecs - 1);
        oldRange->low += numRecs;
        oldRange->data += numRecs * recSize;
        oldRange->dataSize -= numRecs * recSize;
        DOASSERT(oldRange->GapSize() >= bufSize, "Invalid gap size");
    }
    
    DOASSERT(oldRange->dataSize > 0, "Invalid data size");
    
    RangeInfo *newRange = new RangeInfo;
    DOASSERT(newRange, "Out of memory");
    newRange->buf = oldRange->buf;
    newRange->bufSize = bufSize;
  
    oldRange->bufSize -= bufSize;
    oldRange->buf += bufSize;

    return newRange;
}
