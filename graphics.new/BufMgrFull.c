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
#include "RecRange.h"
#include "TDataAttr.h"
#include "GData.h"
#include "BufferFifo.h"
#include "BufferFocal.h"
#include "BufferLifo.h"
#include "BufferRnd.h"
#include "BufferLru.h"

//#define DEBUG

#if defined(BUFMGRHIST)
static int _rangeHistSize[RANGE_ALLOC_HIST_SIZE] = {
    131072, 65536, 32768, 16384, 8192, 0
};
#endif

BufMgrFull::BufMgrFull(int bufSize)
{
    _tReturned = _tHits = _gReturned = _gHits = 0;
    _totalRanges = _totalBuf = _totalData = 0;
    _totalGetRecBytes = _numGetRecs = _totalGetRecBufSize = 0;
    
    int i;
#if defined(BUFMGRHIST)
    for(i = 0; i < RANGE_ALLOC_HIST_SIZE; i++)
        _totalAllocRange[i] = _totalAllocRangeBytes[i] = 0;
#endif
    
    if (Init::Randomize())
        _tdataFetchOrder = Randomize;
    else
        _tdataFetchOrder = AnyOrder;
    
    if (Init::Policy() == BufPolicy::FOCAL) {
        printf("focal policy\n");
        _policy = new BufferFocal;
    } else if (Init::Policy() == BufPolicy::LIFO) {
        printf("Lifo policy\n");
        _policy = new BufferLifo;
    } else if (Init::Policy() == BufPolicy::RND) {
        printf("Rnd policy\n");
        _policy = new BufferRnd;
    } else {
        printf("Fifo policy\n");
        _policy = new BufferFifo;
    }
    DOASSERT(_policy, "Out of memory");
    
    _policy->Info(_numArrays, _policyFlag);
    
    _pageSize = Init::PageSize();
    int bufPages = MAX(bufSize / _pageSize, 1);
    
    // destroy all semaphores and shared memory segments
    Semaphore::destroyAll();
    SharedMemory::destroyAll();

    // create space for 16 virtual semaphores
    int status = SemaphoreV::create(16);
    DOASSERT(status >= 0, "Cannot create virtual semaphores");

    printf("Creating a %.2f MB buffer pool\n", bufSize / 1048576.0);

    _memMgr = new MemMgr(bufPages, _pageSize, status);
    DOASSERT(_memMgr && status >= 0, "Cannot create memory manager");

    _rangeArrays = new RangeInfoArrays(_numArrays, bufPages);
    DOASSERT(_rangeArrays, "Out of memory");
    
    _tdataRangeList = new TDataRangeList;
    _recRange = new RecRange(0);	
    DOASSERT(_tdataRangeList && _recRange, "Out of memory");
    
    for(i = 0; i < MaxReturnRanges; i++)
        _returnRanges[i] = NULL;
    
    /* head of buffer allocation list */
    _allocHead.next = _allocHead.prev = &_allocHead;
    
    _defaultPolicy = new BufferFifo;
    DOASSERT(_defaultPolicy, "Out of memory");
}

/*************************************************************
Get next valid range in memory
**************************************************************/

Boolean BufMgrFull::GetNextRangeInMem(RangeInfo *&rangeInfo)
{
    while (_nextRangeInMem != _listHeadInMem) {
        RecId curLow, curHigh;
        _nextRangeInMem->RecIds(curLow,curHigh);
        
        if (!(curLow > _highId || curHigh < _lowId)) {
            /* found one */
            rangeInfo = _nextRangeInMem;
            _nextRangeInMem = _nextRangeInMem->next;
#if defined(DEBUG)
	    printf("BufMgrFull found range in memory (%ld, %ld)\n", 
		   curLow, curHigh);
#endif
            return true;
        }
        
        if (curLow > _highId)
            /* done */
            break;
        
        _nextRangeInMem = _nextRangeInMem->next;
    }
    
    return false;
}

void BufMgrFull::InitGetDataInMem(RangeList *list)
{
#if defined(DEBUG)
    printf("BufMgrFull:InitGetDataInMem\n");
#endif

    int numRanges;
    RangeInfo *listHead;
    list->GetRangeList(numRanges,listHead);
    _listHeadInMem = listHead;
    _nextRangeInMem = listHead->next;
}

Boolean BufMgrFull::GetGDataInMem(RecId &startRecId, int &numRecs, void *&buf)
{
    RangeInfo *rangeInfo;
    if (!GetNextRangeInMem(rangeInfo))
        return false;

    _recRange->Insert(rangeInfo->low, rangeInfo->high, NULL, NULL,
                    RecRange::MergeIgnoreRecs, NULL);
    CalcRetArgs(false, true, _gdata, rangeInfo, _lowId, _highId,
                buf, startRecId, numRecs);
    SetUse(rangeInfo);
    return true;
}

Boolean BufMgrFull::GetTDataInMem(RecId &startRecId, int &numRecs, void *&buf)
{
    Boolean done = false;
    while (!done) {
        if (_getTInMem) {
            if (!GetNextRangeInMem(_tInMemRange))
                /* no more */
                return false;
            
            /* Set up to return segments within this range not yet processed */
            _tInMemCurLow = _tInMemRange->low;
            if (_tInMemCurLow < _lowId)
                _tInMemCurLow = _lowId;
            _getTInMem = false;
        }
        
        RecId uLow, uHigh;
        Boolean noHigh = _recRange->NextUnprocessed(_tInMemCurLow,
                                                    uLow, uHigh);
        /* Make [uLow,uHigh] fit in [_tInMemRange->low,_tInMemRange->high] */
        if (noHigh || uHigh > _tInMemRange->high)
            uHigh = _tInMemRange->high;
        
        if (uLow > _highId)
            /* done */
            return false;
        
        /* make uHigh fit in [_lowId, _highId] (uLow already >= _lowId) */
        if (uHigh > _highId)
            uHigh = _highId;
        
        if (uLow <= _tInMemRange->high) {
            /* still a valid range to return */
            CalcRetArgs(true, true,_tdata, _tInMemRange,
                        uLow, uHigh, buf, startRecId, numRecs);
            _recRange->Insert(startRecId, startRecId + numRecs - 1,
                              NULL, NULL, RecRange::MergeIgnoreRecs, NULL);
            SetUse(_tInMemRange);
            _tInMemCurLow = startRecId + numRecs;
            return true;
        }

        _getTInMem = true;
    }
    
    return false;
}

void BufMgrFull::InitGDataScan()
{
    _gdata->InitConvertedIterator();
    _gScanState = GetGRangeState;
}

Boolean BufMgrFull::GDataScan(RecId &startRecId, int &numRecs, void *&buf)
{
    RecId processLow, processHigh; /* what's to be processed next */
    RangeInfo *rangeInfo;
    
#if defined(DEBUG)
    printf("BufMgrFull::GDataScan [%ld,%ld]\n", _lowId, _highId);
#endif
    
    Boolean done = false;
    while (!done) {
        switch(_gScanState) {
            
          case GetGRangeState:
            /* Get next range to be processed */
            while (_gScanState == GetGRangeState) {
                /* Check overlap between [_gScanLow, _gScanHigh] and
                   [_lowId,_highId] */
                if (!_gdata->NextRange(_gScanLow, _gScanHigh) ||
                    _gScanLow > _highId) {
                    /* no more to scan */
                    done = true;
                    _gScanState = GScanDoneState;
#if defined(DEBUG)
		    printf("BufMgrFull::GDataScan no more ranges (%ld,%ld) (%ld,%ld)\n",
			   _gScanLow, _gScanHigh, _lowId, _highId);
#endif
                    return false;
                }
                if (_gScanHigh < _lowId) {
                    /* try next one */
#if defined(DEBUG)
		    printf("BufMgrFull::GDataScan skipping range (%ld,%ld) (%ld,%ld)\n",
			   _gScanLow, _gScanHigh, _lowId, _highId);
#endif
                    continue;
		}
                
                /* truncate [_gScanLow,_gScanHigh] to fit inside
                   [_lowId, _highId] */
                if (_gScanLow < _lowId)
                    _gScanLow = _lowId;
                if (_gScanHigh > _highId)
                    _gScanHigh = _highId;
                
                _gScanCur = _gScanLow;
                _gScanState= CheckOverlapState;
#if defined(DEBUG)
		printf("BufMgrFull::GDataScan moving on to CheckOverlap (%ld,%ld) (%ld,%ld) %ld\n",
		       _gScanLow, _gScanHigh, _lowId, _highId, _gScanCur);
#endif
            }
            break;
            
          case CheckOverlapState:
            while (_gScanState == CheckOverlapState) {
                /* Check overlap between [_gScanCur,_gScanHigh]
                   with what's not yet scanned */
                Boolean noHigh = _recRange->NextUnprocessed(_gScanCur,
                                                            processLow,
                                                            processHigh);
#if defined(DEBUG)
		printf("NextUnprocessed: (%ld, %ld) (%ld, %ld)\n",
		       processLow, processHigh, _gScanLow, _gScanHigh);
#endif
                
                if (processLow > _gScanHigh) {
                    /* [_gScanLow,_gScanHigh] already scanned */
                    _gScanState = GetGRangeState;
                    break;
                }
                
                /* limit processHigh to be within 
                   [_gScanLow, _gScanHigh] */
                if (noHigh ||  processHigh > _gScanHigh)
                    processHigh = _gScanHigh;
                
                InitGetRangeNotInMem(_gdata,_gRangeList, 
                                     processLow, processHigh);
                _gScanCur = processHigh+1;
                _gScanState = GScanState;
#if defined(DEBUG)
		printf("moving on to GScanState: (%ld, %ld) (%ld, %ld) %ld\n",
		       processLow, processHigh, _gScanLow, _gScanHigh,
		       _gScanCur);
#endif
            }
            break;
            
          case GScanState:
            /* Get next range in memory */
            if (GetRangeNotInMem(rangeInfo)) {
                /* Got the next range of buffer */
#if defined(DEBUG)
		printf("found range: (%ld, %ld) (%ld, %ld) %ld\n",
		       _lowId, _highId, _gScanLow, _gScanHigh, _gScanCur);
#endif
                CalcRetArgs(false, false, _gdata, rangeInfo, _lowId,
                            _highId, buf, startRecId, numRecs);
                SetUse(rangeInfo);
                done = true;
            } else {
                /*
                   done with getting this range. Change state
                   to get unprocessed range within [_gScanCur,_gScanHigh]
                   */
                DoneGetRangeNotInMem();
                _gScanState = CheckOverlapState;
#if defined(DEBUG)
		printf("moving back to CheckOverlap: (%ld, %ld) %ld\n",
		       _gScanLow, _gScanHigh, _gScanCur);
#endif
            }
            break;
            
          case GScanDoneState:
            DOASSERT(0, "Invalid state");
            break;
        }
    }
    
    return true;
}

void BufMgrFull::DoneGDataScan()
{
    _gdata->DoneConvertedIterator();
    if (_gScanState == GScanState)
        DoneGetRangeNotInMem();
}

void BufMgrFull::InitTDataScan()
{
    _tScanId = _lowId;
    _getTRange = true;
}

Boolean BufMgrFull::TDataScan(RecId &startRecId, int &numRecs, void *&buf)
{
    Boolean done = false;
    RecId rangeLow, rangeHigh;
    RangeInfo *rangeInfo;

#if defined(DEBUG)
    printf("BufMgrFull::TDataScan(%ld, %d, 0x%p)\n", startRecId, numRecs, buf);
#endif    
    while (!done) {
        if (_getTRange) {
            /* Get next tdata range */
            Boolean noHighId = _recRange->NextUnprocessed(_tScanId,rangeLow,
                                                          rangeHigh);
#if defined(DEBUG)
	    printf("BufMgrFull::TDataScan NextUnprocessed (%ld, %ld)\n",
		   rangeLow, rangeHigh);
#endif    
            if (rangeLow > _highId) {
                /* done */
#if defined(DEBUG)
		printf("BufMgrFull::TDataScan done (%ld, %ld)\n",
		       _lowId, _highId);
#endif    
                return false;
	    }
            
            /* Adjust rangeHigh to fit whitin boundary.
               XXX: in the case where [rangeLow,rangeHigh] is smaller than
               a page, adjust it to fetch up to page boundary. 
               Remember to check to make sure that it doe not overlap
               with what's already in memory. For example,
               suppose [rangeLow,rangeHigh] == [1,1]
               memory contains: [10,10]
               Next unprocessed as returned by _recPage is [1,] (because
               10 has not been processed in memory. Then if we want
               to fetch [1,9].
            */
            
            if (noHighId || rangeHigh > _highId)
                rangeHigh = _highId;
            
            _tScanId = rangeHigh + 1; /* update */
            InitGetRangeNotInMem(_tdata,_tRangeList,rangeLow, rangeHigh);
            _getTRange = false;
#if defined(DEBUG)
	    printf("BufMgrFull::TDataScan adjusted range (%ld, %ld)\n",
		   rangeLow, rangeHigh);
#endif    
        }
        
        if (GetRangeNotInMem(rangeInfo)) {
            CalcRetArgs(true, false, _tdata, rangeInfo, _lowId, _highId,
                        buf, startRecId, numRecs);
            SetUse(rangeInfo);
#if defined(DEBUG)
	    printf("BufMgrFull::TDataScan NotInMem (%ld, %ld)\n",
		   _lowId, _highId);
#endif    
            return true;
        } else {
            DoneGetRangeNotInMem();
            _getTRange = true;
#if defined(DEBUG)
	    printf("BufMgrFull::TDataScan go back for more\n");
#endif    
        }
    }
    
    return false;
}

void BufMgrFull::DoneTDataScan()
{
    if (!_getTRange)
        DoneGetRangeNotInMem();
}

/* Init Getting Records */
void BufMgrFull::InitGetRecs(TData *tdata, GData *gdata,
			     RecId lowId, RecId highId,
                             RecordOrder recOrder,
			     Boolean tdataOnly)
{
#if defined(DEBUG)
    printf("BufMgrFull::InitGetRecs (%ld,%ld) with tdata 0x%p, gdata 0x%p\n",
           lowId, highId, tdata, gdata);
#endif

    DOASSERT(tdata->RecSize() >= 0, "Cannot handle variable records yet");

    RecId firstId, lastId;
    Boolean hasFirst = tdata->HeadID(firstId);
    Boolean hasLast = tdata->LastID(lastId);
    DOASSERT(hasFirst && hasLast && lowId >= firstId && highId <= lastId,
             "Invalid record ids");

    _tdata = tdata;
    _gdata = gdata;
    _tdataOnly = tdataOnly;
    _recRange->Clear();
    _lowId = lowId;
    _highId = highId;

    /* Set up parameters */
    /* need to process what's in memory first */
    _tRangeList = _tdataRangeList->Get(tdata);
    if (gdata != NULL)
        _gRangeList = _tdataRangeList->Get(gdata);

    if (!_tdataOnly && gdata != NULL) {
        /* start with getting in memory GData */
        InitGetDataInMem(_gRangeList);
        _getTInMem = false;
        _state = DoGDataInMem;
    } else {
        /* start with getting in memory TData */
        InitGetDataInMem(_tRangeList);
        _getTInMem = true;
        _state = DoTDataInMem;
    }
}

/* Retrieve next batch of records.  Return TRUE if still more batches. */
Boolean BufMgrFull::GetRecs(Boolean &isTData, RecId &startRecId,
			    int &numRecs, void *&buf)
{
    Boolean done = false;

    while (!done) {
        switch(_state) {

          case DoGDataInMem:
            if (GetGDataInMem(startRecId, numRecs, buf)) {
                isTData = false;
                return true;
            }
            DoneGetDataInMem();
            InitGetDataInMem(_tRangeList);
            _getTInMem = true;
            _state = DoTDataInMem;
            break;

          case DoTDataInMem:
            if (GetTDataInMem(startRecId, numRecs, buf)) {
                isTData = true;
                return true;
            }
            DoneGetDataInMem();
            if (!_tdataOnly && _gdata != NULL) {
                InitGDataScan();
                _state = DoGDataScan;
            } else {
                InitTDataScan();
                _state = DoTDataScan;
            }
            break;

          case DoGDataScan:
            if (GDataScan(startRecId, numRecs, buf)) {
                isTData = false;
                return true;
            }
            DoneGDataScan();
            InitTDataScan();
            _state = DoTDataScan;
            break;

          case DoTDataScan:
            if (TDataScan(startRecId, numRecs, buf)) {
                isTData = true;
                return true;
            }
            DoneTDataScan();
            _state = DoneBuffer;
            return false;

          case DoneBuffer:
            DOASSERT(0, "Invalid action");
        }
    }
    
    return false;
}

void BufMgrFull::DoneGetRecs()
{
    switch(_state) {
      case DoGDataInMem:
        DoneGetDataInMem();
        break;
      case DoTDataInMem:
        DoneGetDataInMem();
        break;
      case DoGDataScan:
        DoneGDataScan();
        break;
      case DoTDataScan:
        DoneTDataScan();
        break;
      default:
        break;
    }
    _state = DoneBuffer;
}

void BufMgrFull::FreeRecs(void *buf, BufHint hint, Boolean dirty)
{
#if defined(DEBUG)
    printf("FreeRecs 0x%p\n", buf);
#endif
    ClearUse(buf, dirty);
}

void BufMgrFull::SetUse(RangeInfo *rangeInfo)
{
    for(int i = 0; i < MaxReturnRanges; i++) {
        if (_returnRanges[i] == NULL) {
            rangeInfo->SetUse();
            _returnRanges[i] = rangeInfo;
            return;
        }
    }

    DOASSERT(0, "Cannot find free slot");
}

void BufMgrFull::ClearUse(void *buf, Boolean dirty)
{
    for(int i = 0; i < MaxReturnRanges; i++) {
        RangeInfo *rangeInfo = _returnRanges[i];
        if (rangeInfo != NULL) {
            char *start = (char *)_returnRanges[i]->GetData();
            char *end = start+ rangeInfo->DataSize()-1;
            if ((char *)buf >= start && (char *)buf <= end) {
                if (dirty)
                    rangeInfo->SetDirty();
                rangeInfo->ClearUse();
                _returnRanges[i] = NULL;
                return;
            }
        } 
    }

    DOASSERT(0, "Cannot find buffer");
}

/* Calculate the return parameter given rangeInfo */

void BufMgrFull::CalcRetArgs(Boolean isTData, Boolean isInMem,
                             TData *tdata, RangeInfo *rangeInfo,
                             RecId lowId, RecId highId,
                             void *&buf, RecId &startRid, int &numRecs)
{
    CheckRange(rangeInfo);

    RecId low,high;
    rangeInfo->RecIds(low,high);
    numRecs = high-low+1;

    /* set up initial return parameter */
    buf = rangeInfo->GetData();
    int recSize = tdata->RecSize();
    startRid = low;

    /* modify return parameters */
    if (lowId > low) {
        int diff = lowId - low;
        startRid = lowId;
        buf =  ((char *) buf)+ diff*recSize;
        numRecs -= diff;
        
        DOASSERT(numRecs >= 0, "Invalid number of records");
    }
        
    if (highId < high) {
        int diff = high - highId;
        numRecs -= diff;
        
        DOASSERT(numRecs >= 0, "Invalid number of records");
    }

    int bytes = numRecs * recSize;
    _totalBuf += rangeInfo->BufSize();
    _totalData += rangeInfo->DataSize();
    _totalRanges++;
    if (isTData ) {
        _tReturned += bytes;
        if (isInMem)
            _tHits += bytes;
    } else {
        _gReturned += bytes;
        if (isInMem)
            _gHits += bytes;
    }
}

/*
   Allocate a range suitable for up to "size" bytes.
   Minimum size returned _pageSize.
*/

RangeInfo *BufMgrFull::AllocRange(int size)
{
#if defined(DEBUG)
    printf("BufMgrFull::AllocRange(%d)\n", size);
#endif

#if defined(BUFMGRHIST)
    for(int i = 0; i < RANGE_ALLOC_HIST_SIZE; i++) {
        if (size >= _rangeHistSize[i]) {
            _totalAllocRange[i]++;
            _totalAllocRangeBytes[i] += size;
            break;
        }
    }
#endif

    size = BufSize(size); /* force allocation to page boundary */

    char *buf;
    int pages = size / _pageSize;
    int status = _memMgr->Try(MemMgr::Cache, buf, pages);

    // Did we successfully get at least some memory?
    if (status >= 0) {
        RangeInfo *newRange = new RangeInfo;
        newRange->buf = buf;
        newRange->bufSize = pages * _pageSize;
#if defined(DEBUG)
        printf("returning %d bytes\n", newRange->bufSize);
#endif
        return newRange;
    }
    
    /* No more memory available. Pick a victim. */

#if defined(DEBUG)
    printf("pick victim\n");
#endif

    int arrayNum, pos;
    Boolean found = false;
    if (_policyFlag & BufferPolicy::ReportVictim)
        found = _policy->PickVictim(_rangeArrays, arrayNum, pos);
    else
        found = _defaultPolicy->PickVictim(_rangeArrays, arrayNum, pos);
        
    DOASSERT(found, "Cannot find buffer victim");
        
    RangeInfo *rangeInfo = _rangeArrays->GetRange(arrayNum, pos);
    DOASSERT(!rangeInfo->InUse(), "Buffer victim in use");
    
#if defined(DEBUG)
    printf("victim (%ld,%ld), data %d, buf %d, gap %d\n",
           rangeInfo->low, rangeInfo->high, rangeInfo->DataSize(),
           rangeInfo->BufSize(), rangeInfo->GapSize());
#endif
    
    if (rangeInfo->Dirty()) {
        /* write dirty range out*/
        rangeInfo->tdata->WriteRecs(rangeInfo->low,
                                    rangeInfo->high - rangeInfo->low + 1,
                                    rangeInfo->GetData());
        rangeInfo->ClearDirty();
    }

    /*
       if victim range is larger than requested size by at least
       one page, then go and return the excess space
    */

    if (rangeInfo->DataSize() + rangeInfo->GapSize() - size
        >= _pageSize) {
        /* still room left for data in this range */
        rangeInfo = Truncate(rangeInfo, size);
#if defined(DEBUG)
        printf("truncate victim by %d bytes\n", size);
#endif
    } else {
        UnlinkRange(rangeInfo, arrayNum, pos);
        
        ReportDeleted(rangeInfo->GetTData(),
                      rangeInfo->low, rangeInfo->high);
    }
    
#if defined(DEBUG)
    printf("returning %d bytes\n", rangeInfo->bufSize);
#endif
    
    /* sanity check */
    DOASSERT(rangeInfo->bufSize % _pageSize == 0,
             "Invalid buffer size");
    
    return rangeInfo;
}

void BufMgrFull::UnlinkRange(RangeInfo *range, int arrayNum, int pos)
{
    /* get rid of victim from the lists */
    _tdataRangeList->Get(range->GetTData())->Delete(range);
    (void)_rangeArrays->Delete(arrayNum, pos);
}

/**** Iterator to get range not in memory */

/* Init getting a range not in memory */

void BufMgrFull::InitGetRangeNotInMem(TData *tdata, RangeList *rangeList,
				      RecId lowId, RecId highId)
{
    DOASSERT(lowId <= highId, "Invalid record range");

    _tdataN = tdata;
    _rangeListN = rangeList;
    _lowN = lowId;
    _highN = highId;
    _dSizeN = (highId-lowId+1)*tdata->RecSize();	/* current data size */
    
    _tdataN->InitGetRecs(_lowN,_highN, _tdataFetchOrder );
    _rangeN = NULL;
}

Boolean BufMgrFull::GetRangeNotInMem(RangeInfo *&retRangeInfo)
{
#if defined(DEBUG)
    printf("BufMgrFul::GetRangeNotInMem: _dSize %d\n", _dSizeN);
#endif

    if (_dSizeN == 0) {
        /* done */
        return false;
    }

    if (_rangeN == NULL)
        _rangeN = AllocRange(BufSize(_dSizeN));
	
    RecId startRid;
    int numRecs;
    int dataSize;
    Boolean stat = _tdataN->GetRecs(_rangeN->buf, _rangeN->bufSize,
                                    startRid, numRecs, dataSize);
    
    DOASSERT(stat, "Invalid status");

#if defined(DEBUG)
    printf("Got recs %ld,%ld\n", startRid, startRid + numRecs - 1);
#endif

    _totalGetRecBufSize += _rangeN->bufSize;
    _totalGetRecBytes += dataSize;
    _numGetRecs++;
    
    if (_rangeN->bufSize - BufSize(dataSize) >= _pageSize) {
        /* truncate range -- don't know how to do it yet */
        retRangeInfo = _rangeN;
        _rangeN = NULL;
    } else {
        retRangeInfo = _rangeN;
        _rangeN = NULL;
    }
    
    retRangeInfo->dataSize = dataSize;
    retRangeInfo->low = startRid;
    retRangeInfo->high = startRid + numRecs - 1;
    retRangeInfo->tdata = _tdataN;
    retRangeInfo->data = retRangeInfo->buf;
  
    _dSizeN -= dataSize;
  
    /* Place new range into policy arrays */
    int arrayNum, pos;
    if (_policyFlag & BufferPolicy::ReportPlacement)
        _policy->Placement(retRangeInfo, _rangeArrays, arrayNum, pos);
    else
        _defaultPolicy->Placement(retRangeInfo, _rangeArrays, arrayNum, pos);
    
    _rangeArrays->Insert(arrayNum, pos, retRangeInfo);
    retRangeInfo->listNum = arrayNum;
    
    /* put into list of ranges for this TData */
    int numDisposed;
    RangeInfo **disposed;
    _rangeListN->Insert(retRangeInfo, RangeList::MergeIgnore,
                        numDisposed, disposed);
    
    /* put into list of records already processed */
    _recRange->Insert(retRangeInfo->low, retRangeInfo->high, NULL, NULL,
                      RecRange::MergeIgnoreRecs, NULL);
    
    ReportInserted(_tdataN, retRangeInfo->low, retRangeInfo->high);
    
    return true;
}

void BufMgrFull::DoneGetRangeNotInMem()
{
    _tdataN->DoneGetRecs();
    if (_rangeN) {
        /* release memory */
        int pages = _rangeN->BufSize() / _pageSize;
        int status = _memMgr->Deallocate(MemMgr::Cache,
                                         (char *)_rangeN->GetBuffer(),
                                         pages);
        _rangeN = NULL;
    }
}

/* Truncate rangeInfo by the buffer size from the beginning,
   and return a new one for the truncated portion.
   Arrange the pointers so that the remaining data are
   pointing to the right place. */

RangeInfo *BufMgrFull::Truncate(RangeInfo *rangeInfo, int bufSize)
{
    int recSize = rangeInfo->GetTData()->RecSize();

#if defined(DEBUG)
    printf("Truncate %d, recSize = %d\n", bufSize, recSize);
#endif
    
    DOASSERT(bufSize > 0, "Invalid truncation request");

    /* Figure out # of records truncated */
    int dataTruncated = (bufSize - rangeInfo->GapSize());
    if (dataTruncated > 0) {
        int numRecs = dataTruncated / recSize;
        if (dataTruncated % recSize != 0)
            numRecs++;

        if (numRecs > 0)
            ReportDeleted(rangeInfo->GetTData(), rangeInfo->low,
                          rangeInfo->low + numRecs - 1);
        
        rangeInfo->low += numRecs;
        rangeInfo->data += numRecs * recSize;
        rangeInfo->dataSize -= numRecs * recSize;
    }
    
    DOASSERT(rangeInfo->dataSize > 0, "Invalid data size");
    
    RangeInfo *newInfo = new RangeInfo;
    newInfo->buf = rangeInfo->buf;
    newInfo->bufSize = bufSize;
  
    rangeInfo->bufSize -= bufSize;
    rangeInfo->buf += bufSize;
  
    return newInfo;
}


void BufMgrFull::Print(TData *tdata)
{
    RangeList *rangeList = _tdataRangeList->Get(tdata);
    RangeInfo *head;
    int num;
    rangeList->GetRangeList(num, head);
    printf("low\thigh\tdata\tbuf\tgap\n");
    for(RangeInfo *cur = head->next; cur != head; cur = cur->next) {
        printf("%ld\t%ld\t%d\t%d\t%d\n", cur->low, cur->high,
               cur->DataSize(), cur->BufSize(), cur->GapSize());
    }
}

RecId BufMgrFull::RoundToPage(TData *tdata, RecId low, RecId high,
			      RecId maxId)
{
    /* Round off high to page boundary */
    int numRecs = high - low + 1;
    int recSize = tdata->RecSize();
    int recBytes = numRecs * recSize;
    RecId newHigh = high;
    int remain;
    if ((remain = recBytes % _pageSize) != 0) {
        int bytesLeft = _pageSize - remain;
        newHigh += bytesLeft / recSize;
    }
    
    if (newHigh > maxId)
        newHigh = maxId;
    
    return newHigh;
}

void BufMgrFull::CheckRange(RangeInfo *rangeInfo)
{
    if (rangeInfo->data < rangeInfo->buf ||
        rangeInfo->data >= rangeInfo->buf+rangeInfo->bufSize)
        goto error;
    if (rangeInfo->DataSize()+rangeInfo->GapSize() > rangeInfo->bufSize)
        goto error;
    if (rangeInfo->high < rangeInfo->low)
        goto error;
    return;
    
  error:
    printf("Range error:buf 0x%p, data 0x%p, id (%ld,%ld), buf %d, data %d\n",
           rangeInfo->buf, rangeInfo->data, rangeInfo->low, rangeInfo->high,
           rangeInfo->BufSize(), rangeInfo->DataSize());
    DOASSERT(0, "Invalid range");
}

void BufMgrFull::InitTDataInMem(TData *tdata)
{
    int numRanges;
    _tdataRangeList->Get(tdata)->GetRangeList(numRanges, _inMemHead);
    
    /* Note: we must set this variable to the current one so that
       it's never reclaimed from the list by the buffer manager */
    _inMemCur = _inMemHead;
}

Boolean BufMgrFull::GetInMemRecs(void *&buf, RecId &low, RecId &high)
{
    _inMemCur = _inMemCur->next;
    if (_inMemCur == _inMemHead)
        return false;
    
    SetUse(_inMemCur);
    buf = _inMemCur->GetData();
    low = _inMemCur->low;
    high = _inMemCur->high;
    return true;
}

void BufMgrFull::FreeInMemRecs(void *buf, Boolean dirty)
{
    ClearUse(buf, dirty);
}

void BufMgrFull::DoneTDataInMem()
{
}

void BufMgrFull::PrintStat()
{
#if defined(DEBUG)
    _rangeArrays->PrintStat();
#endif

    printf("%d bytes retrieved\n", _totalGetRecBytes);
    if (_tReturned > 0)
        printf("TData: returned %d, hits %d, %.2f%%\n",
               _tReturned, _tHits, (double)_tHits/(double)_tReturned * 100.0);
    if (_gReturned > 0)
        printf("GData: returned %d, hits %d, %.2f%%\n",
               _gReturned, _gHits, (double)_gHits/(double)_gReturned * 100.0);
    if ((_tReturned+_gReturned) > 0)
        printf("Total: returned %d, hits %d, %.2f%%\n",
               _tReturned+_gReturned, _tHits+_gHits,
              (double)(_tHits+_gHits)/(double)(_tReturned+_gReturned) * 100.0);
    
#if defined(DEBUG)
    printf("%d bufs returned: avg buf size %f, avg data size: %f\n",
           _totalRanges, (double)_totalBuf/(double)_totalRanges,
           (double)_totalData/(double)_totalRanges);
    
#if defined(BUFMGRHIST)
    for(int i = 0; i < RANGE_ALLOC_HIST_SIZE; i++) {
        printf("%d AllocRange() >= %d, avg size %.2f\n",
               _totalAllocRange[i], _rangeHistSize[i],
               (double)_totalAllocRangeBytes[i]/(double)_totalAllocRange[i]);
    }
#endif
    
    printf("%d totalGetRecs, avg input Buf %.2f, avg size %.2f\n",
           _numGetRecs, (double)_totalGetRecBufSize/(double)_numGetRecs,
           (double)_totalGetRecBytes/(double)_numGetRecs);
#endif
}

void BufMgrFull::Clear()
{
    /* Free all RangeInfos that have been allocated */
    int numArrays = _rangeArrays->NumArrays();
    for(int i = 0; i < numArrays; i++) {
        for(_rangeArrays->InitIterator(i); _rangeArrays->More(i);) {
            RangeInfo *rInfo = _rangeArrays->Next(i);
            int pages = rInfo->BufSize() / _pageSize;
            int status = _memMgr->Deallocate(MemMgr::Cache,
                                             (char *)rInfo->GetBuffer(),
                                             pages);
        }
    }
    
    /* Clear the arrays of RangeInfos used by buffer manager policy */
    _rangeArrays->Clear();
    
    _tdataRangeList->Clear();
    
    _policy->Clear();
    _defaultPolicy->Clear();
}

/* Clear buffers occupied by TData/GData */

void BufMgrFull::ClearData(TData *data)
{
    int numArrays = _rangeArrays->NumArrays();
    for(int i = 0; i < numArrays; i++) {
        int size = _rangeArrays->Size(i);
        int j = 0;
        while ( j < size) {
            RangeInfo *info = _rangeArrays->GetRange(i,j);
            if (info->GetTData() == data) {
                /* clear this one */
                UnlinkRange(info, i, j);
                ReportDeleted(data, info->low, info->high);
                int pages = info->BufSize() / _pageSize;
                int status = _memMgr->Deallocate(MemMgr::Cache,
                                                 (char *)info->GetBuffer(),
                                                 pages);
                size--;
            } else
                j++;
        }
    }
}
