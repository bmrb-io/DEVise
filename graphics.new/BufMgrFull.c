/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.2  1995/09/05 22:14:13  jussi
  Added CVS header.
*/

#include <stdio.h>
#include "Config.h"
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
#include "Init.h"
#include "BufferFifo.h"
#include "BufferFocal.h"
#include "BufferLifo.h"
#include "BufferRnd.h"

//#define DEBUG

extern int debug;
int BUFMGR_FULL_PAGESIZE = DEVISE_PAGESIZE;
static int _rangeHistSize[RANGE_ALLOC_HIST_SIZE] = {
	131072, 65536, 32768, 16384, 8192, 0
};

/* constructor */
BufMgrFull::BufMgrFull(int bufSize){
    _tReturned = _tHits = _gReturned = _gHits = 
	_totalRanges = _totalBuf = _totalData = 
	_totalGetRecBytes = _numGetRecs = 
	_totalGetRecBufSize = 0;

	int i;
	for (i=0;i < RANGE_ALLOC_HIST_SIZE; i++) {
		_totalAllocRange[i] = _totalAllocRangeBytes[i] = 0;
	}

	if (Init::Randomize())
		_tdataFetchOrder = Randomize;
	else _tdataFetchOrder = AnyOrder;

	BUFMGR_FULL_PAGESIZE = Init::PageSize();
	_recPtrs = new void *[BUFMGR_FULL_PAGESIZE];

	if (Init::Policy() == BufPolicy::FOCAL){
		printf("focal policy\n");
		_policy = new BufferFocal();
	}
	else if (Init::Policy() == BufPolicy::LIFO){
		printf("Lifo policy\n");
		_policy = new BufferLifo();
	}
	else if (Init::Policy() == BufPolicy::RND){
		printf("Rnd policy\n");
		_policy = new BufferRnd();
	}
	else {
		printf("Fifo policy\n");
		_policy = new BufferFifo();
	}

	_policy->Info(_numArrays,_policyFlag);
	_rangeAlloc = new RangeInfoAlloc(bufSize);
	_maxPages = bufSize / BUFMGR_FULL_PAGESIZE;
	_rangeArrays = new RangeInfoArrays(_numArrays, _maxPages);
	_bufSize = bufSize;
	_tdataRangeList = new TDataRangeList();
	_recRange = new RecRange(0);	

	for (i=0; i < MaxReturnRanges; i++)
		_returnRanges[i] = NULL;

	/* head of buffer allocation list */
	_allocHead.next = _allocHead.prev = &_allocHead;

	_defaultPolicy = new BufferFifo();
}

void BufMgrFull::InitNextRangeInMem(RangeInfo *listHead){
	_listHeadInMem = listHead;
	_nextRangeInMem = listHead->next;
}

/*************************************************************
Get next valid range in memory
**************************************************************/
Boolean BufMgrFull::GetNextRangeInMem(RangeInfo *&rangeInfo){
	while (_nextRangeInMem != _listHeadInMem){
		RecId curLow, curHigh;
		_nextRangeInMem->RecIds(curLow,curHigh);

		if (!(curLow > _highId || curHigh < _lowId)){
			/* found one */
			rangeInfo = _nextRangeInMem;
			_nextRangeInMem = _nextRangeInMem->next;
			return true;
		}

		if (curLow > _highId)
			/* done */
			break;

		_nextRangeInMem = _nextRangeInMem->next;
	}
	return false;
}

/* Init Getting GData in memory */
void BufMgrFull::InitGetGDataInMem(){
#ifdef DEBUG
	printf("BufMgrFull:InitGetGDataInMem\n");
#endif

	int numRanges;
	RangeInfo *listHead;
	_gRangeList->GetRangeList(numRanges,listHead);
	InitNextRangeInMem(listHead);
}

Boolean BufMgrFull::GetGDataInMem(RecId &startRecId, int &numRecs,void *&buf ){
	RangeInfo *rangeInfo;
	if (GetNextRangeInMem(rangeInfo)){
		_recRange->Insert(rangeInfo->low,rangeInfo->high,NULL,NULL,
					RecRange::MergeIgnoreRecs, NULL);
		CalcRetArgs(false, true,
			_gdata,rangeInfo,_lowId,_highId,buf,startRecId,numRecs);
		SetUse(rangeInfo);
		return true;
	}
	else return false;
}
void BufMgrFull::DoneGetGDataInMem() {}

void BufMgrFull::InitGetTDataInMem(){
#ifdef DEBUG
	printf("BufMgrFull:InitGetTDataInMem\n");
#endif

	int numRanges;
	RangeInfo *listHead;
	_tRangeList->GetRangeList(numRanges,listHead);
	InitNextRangeInMem(listHead);
	_getTInMem = true;
}

Boolean BufMgrFull::GetTDataInMem(RecId &startRecId, int &numRecs, void *&buf){
	Boolean done = false;
	while (!done){
		if (_getTInMem){
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
		Boolean noHigh = _recRange->NextUnprocessed(_tInMemCurLow, uLow, uHigh);
		/* Make [uLow,uHigh] fit in [_tInMemRange->low,_tInMemRange->high]*/
		if (noHigh || uHigh > _tInMemRange->high)
			uHigh = _tInMemRange->high;
		
		if (uLow > _highId)
			/* done */
			return false;

		/* make uHigh fit in [_lowId, _highId]. 
		(uLow already >= _lowId) */
		if (uHigh > _highId)
			uHigh = _highId;

		if (uLow <= _tInMemRange->high){
			/* still a valid range to return */
			/*
			CalcRetArgs(_tdata,_tInMemRange,
				_lowId,_highId,buf,startRecId,numRecs);
			*/
			CalcRetArgs(true, true,_tdata,_tInMemRange,
				uLow,uHigh,buf,startRecId,numRecs);
			_recRange->Insert(startRecId, startRecId+numRecs-1,NULL,NULL,
					RecRange::MergeIgnoreRecs, NULL);
			SetUse(_tInMemRange);
			_tInMemCurLow = startRecId+numRecs;
			return true;
		}
		else _getTInMem = true;
	}
}

void BufMgrFull::DoneGetTDataInMem(){
	/*
	printf("DoneTDataInMem: procesed ranges: \n");
	_recRange->Print();

	printf("TData in mem: \n");
	_tRangeList->Print();
	*/
}

void BufMgrFull::InitGDataScan(){
	_gdata->InitConvertedIterator();
	_gScanState = GetGRangeState;
}

Boolean BufMgrFull::GDataScan(RecId &startRecId, int &numRecs, void *&buf){
	RecId convertedLow, convertedHigh; /* What's converted */
	RecId processLow, processHigh; /* what's to be processed next */
	RangeInfo *rangeInfo;
/*
printf("BufMgrFull::GDataScan [%d,%d]\n",_lowId, _highId);
*/

	Boolean done = false;
	while (!done){
		switch(_gScanState){
			case GetGRangeState:
				/* Get next range to be processed */
				while (_gScanState == GetGRangeState){
					/* Check overlap between [_gScanLow, _gScanHigh] and
					[_lowId,_highId] */
					if (!_gdata->NextRange(_gScanLow, _gScanHigh) ||
						_gScanLow > _highId){
						/* no more to scan */
						done = true;
						_gScanState = GScanDoneState;
						return false;
					}
					if (_gScanHigh < _lowId)
						/* try next one */
						continue;

					/* truncate [_gScanLow,_gScanHigh] to fit inside
					[_lowId, _highId] */
					if (_gScanLow < _lowId)
						_gScanLow = _lowId;
					if (_gScanHigh > _highId)
						_gScanHigh = _highId;
					
					_gScanCur = _gScanLow;
					_gScanState= CheckOverlapState;
				}
				break;
			case CheckOverlapState:
				while (_gScanState == CheckOverlapState){
					/* Check overlap between [_gScanCur,_gScanHigh]
					with what's not yet scanned */
					Boolean noHigh = _recRange->NextUnprocessed(_gScanCur,
						processLow, processHigh);

					if (processLow > _gScanHigh){
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
				}
				break;
			case GScanState:
				/* Get next range in memory */
				if (GetRangeNotInMem(rangeInfo)){
					/* Got the next range of buffer */
					CalcRetArgs(false, false,_gdata,rangeInfo,_lowId,_highId,
						buf,startRecId,numRecs);
					SetUse(rangeInfo);
					done = true;
				}
				else {
					/* done with getting this range. Change state
					to get unprocessed range within [_gScanCur,_gScanHigh]*/
					DoneGetRangeNotInMem();
					_gScanState = CheckOverlapState;
				}
				break;
			case GScanDoneState:
				fprintf(stderr,"BufMgrFull::GDataScan: state == Done\n");
				Exit::DoExit(2);
				break;
		}
	}
	return true;
}

void BufMgrFull::DoneGDataScan(){
	_gdata->DoneConvertedIterator();
	if (_gScanState == GScanState)
		DoneGetRangeNotInMem();
}

void BufMgrFull::InitTDataScan(){
/*
printf("InitTDcan[%d,%d]\n",_lowId, _highId);
*/
	_tScanId = _lowId;
	_getTRange = true;
}

Boolean BufMgrFull::TDataScan(RecId &startRecId, int &numRecs, void *&buf){
	Boolean done = false;
	RecId rangeLow, rangeHigh;
	RangeInfo *rangeInfo;
	while (!done){
		if (_getTRange){
			/* Get next tdata range */
			Boolean noHighId = _recRange->NextUnprocessed(_tScanId,rangeLow,
					rangeHigh);
			if (rangeLow > _highId)
				/* done */
				return false;
			
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

			_tScanId = rangeHigh+1; /* update */
			InitGetRangeNotInMem(_tdata,_tRangeList,rangeLow, rangeHigh);
			_getTRange = false;
		}
		if (GetRangeNotInMem(rangeInfo)){
			CalcRetArgs(true, false,
				_tdata,rangeInfo,_lowId,_highId,buf,startRecId,numRecs);
			SetUse(rangeInfo);
			return true;
		}
		else {
			DoneGetRangeNotInMem();
			_getTRange = true;
		}
	}
}

void BufMgrFull::DoneTDataScan(){
	if (!_getTRange)
		DoneGetRangeNotInMem();
}

/* Init Getting Records */
void BufMgrFull::InitGetRecs(TData *tdata, GData *gdata,
	RecId lowId, RecId highId, RecordOrder recOrder,
	Boolean tdataOnly){
#ifdef DEBUG
  printf("BufMgrFull::InitGetRecs (%d,%d) with tdata 0x%x, gdata 0x%x\n",
	 lowId, highId, tdata, gdata);
#endif

	RangeInfo *info;
	if (tdata->RecSize() < 0){
		fprintf(stderr,"BufMgrFull: can't handle var records yet\n");
		Exit::DoExit(2);
	}
	RecId firstId, lastId;
	Boolean hasFirst = tdata->HeadID(firstId);
	Boolean hasLast = tdata->LastID(lastId);
	if (!hasFirst || !hasLast || lowId < firstId || highId > lastId){
		fprintf(stderr,"BufMgrFull::GetRecs: tdata ids (%d,%d) out of range\n",
			lowId,highId);
		Exit::DoExit(2);
	}
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

	if (!_tdataOnly && gdata != NULL){
		/* start with getting in memory GData */
		InitGetGDataInMem();
		_state = DoGDataInMem;
	}
	else {
		/* start with getting in memory TData */
		InitGetTDataInMem();
		_state = DoTDataInMem;
	}
}


/* Retrieve next batch of records.  Return TRUE if still more batches. */
Boolean BufMgrFull::GetRecs(Boolean &isTData,
	RecId &startRecId, int &numRecs, void *&buf, void **&recs){
	recs = _recPtrs;
	Boolean done = false;
	while (!done){
		switch(_state){
			case DoGDataInMem:
				if (GetGDataInMem(startRecId, numRecs, buf)){
					isTData = false;
					return true;
				}
				else{
					DoneGetGDataInMem();
					InitGetTDataInMem();
					_state = DoTDataInMem;
				}
				break;
			case DoTDataInMem:
				if (GetTDataInMem(startRecId, numRecs, buf)){
					isTData = true;
					return true;
				}
				else {
					DoneGetTDataInMem();
					if (!_tdataOnly && _gdata != NULL){
						InitGDataScan();
						_state = DoGDataScan;
					}
					else {
						InitTDataScan();
						_state = DoTDataScan;
					}
				}
				break;
			case DoGDataScan:
				if (GDataScan(startRecId, numRecs, buf)){
					isTData = false;
					return true;
				}
				else {
					DoneGDataScan();
					InitTDataScan();
					_state = DoTDataScan;
				}
				break;
			case DoTDataScan:
				if (TDataScan(startRecId, numRecs, buf)){
					isTData = true;
					return true;
				}
				else {
					DoneTDataScan();
					_state = DoneBuffer;
					return false;
				}
				break;
			case DoneBuffer:
				fprintf(stderr,"BufMgrFull::GetRecs called after done\n");
				Exit::DoExit(2);
		}
	}
}

void BufMgrFull::DoneGetRecs(){
	switch(_state){
		case DoGDataInMem:
			DoneGetGDataInMem();
			break;
		case DoTDataInMem:
			DoneGetTDataInMem();
			break;
		case DoGDataScan:
			DoneGDataScan();
			break;
		case DoTDataScan:
			DoneTDataScan();
			break;
	}
	_state = DoneBuffer;
}

void BufMgrFull::FreeRecs(void *buf, BufHint hint, Boolean dirty){
	/*
	printf("FreeRecs 0x%x\n", buf);
	*/
	ClearUse(buf, dirty);

}

void BufMgrFull::SetUse(RangeInfo *rangeInfo){
	for (int i=0; i < MaxReturnRanges; i++){
		if (_returnRanges[i] == NULL){
			rangeInfo->SetUse();
			_returnRanges[i] = rangeInfo;
			return;
		}
	}
	fprintf(stderr,"BufMgrFull::SetUse: can't find free slot\n");
	Exit::DoExit(2);
}

void BufMgrFull::ClearUse(void *buf, Boolean dirty){

	for (int i=0; i < MaxReturnRanges; i++){
		RangeInfo *rangeInfo = _returnRanges[i];
		if (rangeInfo != NULL){
			char *start = (char *)_returnRanges[i]->GetData();
			char *end = start+ rangeInfo->DataSize()-1;
			if ((char *)buf >= start && (char *)buf <= end){
				if (dirty)
					rangeInfo->SetDirty();
				rangeInfo->ClearUse();
				_returnRanges[i] = NULL;
				return;
			}
		} 
	}
	fprintf(stderr,"BufMgrFull::FreeRecs: can't find buf 0x%x\n");
	Exit::DoExit(2);
}

/*****  Prefetch Interface ****/
/* Init prefetch */
void BufMgrFull::InitPrefetch(TData *tdata){}

/* Prefetch record whose ID is "current".
Return TRUE if there is still buffer for prefetch,
and set "next" to the next Id after "current" to be prefetched. */
Boolean BufMgrFull::DoPrefetch(RecId current, RecId &next){ return false;}

/**** Policy ****/
/* Init buffer manager policy */
void BufMgrFull::InitPolicy(BufPolicy::policy policy){};


/* Calculate the return parameter given rangeInfo */
void BufMgrFull::CalcRetArgs(Boolean isTData, Boolean isInMem,
	TData *tdata, RangeInfo *rangeInfo,RecId lowId, RecId highId,
	void *&buf,RecId &startRid,int &numRecs){

	CheckRange(rangeInfo);

	RecId low,high;
	rangeInfo->RecIds(low,high);
	numRecs = high-low+1;

	/* set up initial return parameter */
	buf = rangeInfo->GetData();
	int recSize = tdata->RecSize();
	startRid = low;

	/* modify return parameters */
	if (lowId > low){
		int diff = lowId - low;
		startRid = lowId;
		buf =  ((char *) buf)+ diff*recSize;
		numRecs -= diff;

		if (numRecs < 0){
			fprintf(stderr,"BufMgrFull::CalcRetArgs: internal error: low numRecs = %d\n",numRecs);
			fprintf(stderr,"rangeInfo (%d,%d),data %d, buf %d\n",
				rangeInfo->low, rangeInfo->high,
				rangeInfo->DataSize(), rangeInfo->BufSize());
			Exit::DoExit(2);
		}
	}

	if (highId < high){
		int diff = high - highId;
		numRecs -= diff;

		if (numRecs < 0){
			fprintf(stderr,"BufMgrFull::CalcRetArgs: internal error: high numRecs = %d\n",numRecs);
			Exit::DoExit(2);
		}
	}

	int bytes = numRecs * recSize;
	_totalBuf += rangeInfo->BufSize();
	_totalData += rangeInfo->DataSize();
	_totalRanges++;
	if (isTData ){
		_tReturned += bytes;
		if (isInMem)
			_tHits += bytes;
	} else {
		_gReturned += bytes;
		if (isInMem)
			_gHits += bytes;
	}
}

/* Allocate a range suitable for up to "size" bytes.
Minimum size returned BUFMGR_FULL_PAGESIZE */
RangeInfo *BufMgrFull::AllocRange(int size, Boolean firm){
/*
if (debug)
	printf("BufMgrFull::AllocRange(%d): ",size);
*/
	for (int i=0; i < RANGE_ALLOC_HIST_SIZE; i++){
		if (size >= _rangeHistSize[i]){
			_totalAllocRange[i]++;
			_totalAllocRangeBytes[i] += size;
			break;
		}
	}

	size = BufSize(size); /* force allocation to page boundary */
	RangeInfo *rangeInfo;
	if ((rangeInfo=_rangeAlloc->AllocRange(size)) == NULL){
		/* no more to allocate. Pick a victim */
/*
if (debug)
	printf("pick victim\n");
*/
		int arrayNum, pos;
		Boolean found;
		if (_policyFlag & ReportVictim)
			found = _policy->PickVictim(_rangeArrays, arrayNum, pos);
		else found = _defaultPolicy->PickVictim(_rangeArrays, arrayNum, pos);

		if (!found){
			/* no victim found */
			if (firm){
				fprintf(stderr,"BufMgrFull:: can't find victim\n");
				Exit::DoExit(2);
			}
			else return NULL;
		}
		rangeInfo = _rangeArrays->GetRange(arrayNum, pos);
		if (rangeInfo->InUse()){
			fprintf(stderr,"BufMgrFull: victim in use\n");
			Exit::DoExit(2);
		}
/*
if (debug)
printf("victim (%d,%d), data %d, buf %d, gap %d\n",
		rangeInfo->low, rangeInfo->high,
		rangeInfo->DataSize(), rangeInfo->BufSize(),
		rangeInfo->GapSize());
*/

		if (rangeInfo->Dirty()){
			/* write dirty range out*/
			rangeInfo->tdata->WriteRecs(rangeInfo->low,
				rangeInfo->high-rangeInfo->low+1,
				rangeInfo->GetData());
			rangeInfo->ClearDirty();
		}

		if (rangeInfo->DataSize()+rangeInfo->GapSize() - size >=
			BUFMGR_FULL_PAGESIZE
			/* rangeInfo->tdata->RecSize() */){
			/* XXX: Should we keep just a tiny bit in memory?
			For example, how about changing the above 
			into "(...) >= BUFMGR_FULL_PAGESIZE" get rid of
			very small ranges? */
			/* still room left for data in this range */
			rangeInfo = Truncate(rangeInfo,size);
/*
if (debug)
		printf("truncate victim by %d bytes\n",size);
*/
		}
		else {
			UnlinkRange(rangeInfo, arrayNum, pos);

			ReportDeleted(rangeInfo->GetTData(),
				rangeInfo->low, rangeInfo->high);
		}
	}
/*
if (debug)
	printf("returning %d bytes\n",rangeInfo->bufSize);
*/

	/* sanity check */
	if (rangeInfo->bufSize % BUFMGR_FULL_PAGESIZE != 0){
		fprintf(stderr,
			"BufMgrFull::AllocRange got %d bytes, not multiple of %d\n",
			rangeInfo->bufSize, BUFMGR_FULL_PAGESIZE);
		Exit::DoExit(2);
	}
	return rangeInfo;
}

void BufMgrFull::UnlinkRange(RangeInfo *range, int arrayNum, int pos){
	/* get rid of victim from the lists */
	_tdataRangeList->Get(range->GetTData())->Delete(range);
	(void)_rangeArrays->Delete(arrayNum, pos);
}

/**** Iterator to get range not in memory */
/* Init getting a range not in memory */
void BufMgrFull::InitGetRangeNotInMem(TData *tdata, RangeList *rangeList, 
	RecId lowId, RecId highId){

	if (lowId > highId){
		fprintf(stderr,"BufMgrFull::InitGetRangeNotInMem: low %d >high %d\n",
			lowId, highId);
		Exit::DoExit(2);
	}

	_tdataN = tdata;
	_rangeListN = rangeList;
	_lowN = lowId;
	_highN = highId;
	_dSizeN = (highId-lowId+1)*tdata->RecSize();	/* current data size */

	_tdataN->InitGetRecs(_lowN,_highN, _tdataFetchOrder );
	_rangeN = NULL;
/*
printf("BufMgrFull::InitGetRangeNotINMem\n");
printf("lowId %d, highId %d, dataSize %d, bufSize %d\n",
	_lowN, _highN, _dSizeN, BufSize(_dSizeN));
*/
}

Boolean BufMgrFull::GetRangeNotInMem(RangeInfo *&retRangeInfo){
/*
printf("BufMgrFul::GetRangeNotInMem: _dSize %d\n", _dSizeN);
*/
	if (_dSizeN == 0){
		/* done */
		return false;
	}
	if (_rangeN == NULL)
		_rangeN = AllocRange(BufSize(_dSizeN));
	
	RecId startRid;
	int numRecs;
	int dataSize;
	Boolean stat = _tdataN->GetRecs(_rangeN->buf,
		_rangeN->bufSize, startRid,numRecs,dataSize,_recPtrs);
	
	if (!stat){
		fprintf(stderr,"BufMgrFull::GetRangeNotInMem:: no more data\n");
		Exit::DoExit(2);
	}
/*
if (debug)
	printf("Got recs %d,%d\n", startRid,startRid+numRecs-1);
*/


	_totalGetRecBufSize += _rangeN->bufSize;
	_totalGetRecBytes += dataSize;
	_numGetRecs++;

	if (_rangeN->bufSize - BufSize(dataSize) >= BUFMGR_FULL_PAGESIZE){
		/* truncate range */
		/*
		printf("GetRangeNotInMem: truncate %d bytes\n", BufSize(dataSize));
		*/
		retRangeInfo = _rangeAlloc->Truncate(_rangeN, BufSize(dataSize));
	}
	else {
		retRangeInfo = _rangeN;
		_rangeN = NULL;
	}
	retRangeInfo->dataSize = dataSize;
	retRangeInfo->low = startRid;
	retRangeInfo->high = startRid+numRecs-1;
	retRangeInfo->tdata = _tdataN;
	retRangeInfo->data = retRangeInfo->buf;

	_dSizeN -= dataSize;

	/* Place new range into pllicy arrays */
	int arrayNum,pos;
	if (_policyFlag & ReportPlacement)
		_policy->Placement(retRangeInfo,_rangeArrays, arrayNum, pos);
	else _defaultPolicy->Placement(retRangeInfo,_rangeArrays, arrayNum, pos);

	_rangeArrays->Insert(arrayNum, pos, retRangeInfo);
	retRangeInfo->listNum = arrayNum;

	/* put into list of ranges for this TData */
	int numDisposed;
	RangeInfo **disposed;
	_rangeListN->Insert(retRangeInfo, RangeList::MergeIgnore,
		numDisposed, disposed);

	/* put into list of records already processed */
	_recRange->Insert(retRangeInfo->low,retRangeInfo->high,NULL,NULL,
					RecRange::MergeIgnoreRecs, NULL);

	ReportInserted(_tdataN, retRangeInfo->low, retRangeInfo->high);
	return true;
}

void BufMgrFull::DoneGetRangeNotInMem(){
	_tdataN->DoneGetRecs();
	if (_rangeN != NULL){
		/* put it back on the list */
		/*
		printf("BufMgrFull::DoneGetRangeNotInMem: remaining range not NULL\n");
		printf("%d bytes in buffer\n",_rangeN->bufSize);
		char buf[20];
		printf("ret"); gets(buf);
		*/
		_rangeAlloc->FreeRange(_rangeN);
		_rangeN = NULL;
	}
}

/* Truncate rangeInfo by the buffer size from the beginning,
and return a new one for the truncated portion.
Arrange the pointers so that the remaining data are
pointing to the right place. */
RangeInfo *BufMgrFull::Truncate(RangeInfo *rangeInfo, int bufSize){
	TData *tdata = rangeInfo->GetTData();
	int recSize = tdata->RecSize();
/*
if (debug)
	printf("Truncate %d, recSize = %d,\n", bufSize,recSize);
*/

	/* Figure out # of records truncated */
	int dataTruncated = (bufSize-rangeInfo->GapSize());
	if (dataTruncated > 0){
		int numRecs = (dataTruncated % recSize == 0?
			dataTruncated/recSize: dataTruncated/recSize+1);
		
		if (numRecs > 0)
			ReportDeleted(rangeInfo->GetTData(),
				rangeInfo->low, rangeInfo->low+numRecs-1);

		rangeInfo->low += numRecs;
		rangeInfo->data += numRecs*recSize;
		rangeInfo->dataSize -= numRecs*recSize;
	}

	if (rangeInfo->dataSize <= 0){
		fprintf(stderr,"internal error: BufMgrFull::Truncate\n");
		Exit::DoExit(2);
	}

	return _rangeAlloc->Truncate(rangeInfo, bufSize);
}


void BufMgrFull::Print(TData *tdata){
	RangeList *rangeList = _tdataRangeList->Get(tdata);
	RangeInfo *head;
	int num;
	rangeList->GetRangeList(num,head);
	RangeInfo *cur;
	printf("low\thigh\tdata\tbuf\tgap\n");
	for (cur = head->next; cur != head; cur = cur->next){
		printf("%d\t%d\t%d\t%d\t%d\n",cur->low,cur->high,
			cur->DataSize(),cur->BufSize(),cur->GapSize());
	}
}

RecId BufMgrFull::RoundToPage(TData *tdata, RecId low, RecId high, RecId maxId){
	/* Round off high to page boundary */
	int numRecs = high-low+1;
	int recSize = tdata->RecSize();
	int recBytes = numRecs*recSize;
	RecId newHigh = high;
	int remain;
	if ((remain= recBytes % BUFMGR_FULL_PAGESIZE) != 0){
		int bytesLeft = BUFMGR_FULL_PAGESIZE - remain;
		newHigh += bytesLeft/recSize;
	}

	if (newHigh > maxId)
		newHigh = maxId;
		
	return newHigh;
}

void BufMgrFull::CheckRange(RangeInfo *rangeInfo){
	if (rangeInfo->data < rangeInfo->buf ||
		rangeInfo->data >= rangeInfo->buf+rangeInfo->bufSize)
		goto error;
	if (rangeInfo->DataSize()+rangeInfo->GapSize() > rangeInfo->bufSize)
		goto error;
	if (rangeInfo->high < rangeInfo->low)
		goto error;
	return;
error:
	printf("Range error:buf 0x%x, data 0x%x, id (%d,%d), bsizes %d, dSize %d\n",rangeInfo->buf,rangeInfo->data,rangeInfo->low,rangeInfo->high,
	rangeInfo->BufSize(), rangeInfo->DataSize());
	Exit::DoExit(2);
}

/*** Buffer allocation for appending to TData */
void BufMgrFull::InitAllocBuf(int recSize,  int numRecs){
	_allocRecs = numRecs; /* # of records left */
	_allocRecSize = recSize;
	_allocDone = false;
	if (_allocRecSize <= 0){
		fprintf(stderr,"BufMgrFuill::InitAllocBuf: tdata recsize %d\n",
			_allocRecSize);
		Exit::DoExit(2);
	}
}

Boolean BufMgrFull::GetBuf(void *&buf, int &numRecs){
	if (_allocRecs <= 0 || _allocDone)
		return false;

	RangeInfo *rangeInfo = AllocRange(_allocRecSize*_allocRecs, false);
	if (rangeInfo == NULL){
		/* done */
		_allocDone = true;
		return false;
	}
	numRecs = rangeInfo->bufSize/_allocRecSize;
	if (numRecs > _allocRecs)
		numRecs = _allocRecs;
	_allocRecs -= numRecs;

	buf = rangeInfo->buf;
	rangeInfo->data = rangeInfo->buf;
	rangeInfo->dataSize = numRecs*_allocRecSize;

	/* insert as head of list */
	rangeInfo->next = _allocHead.next;
	rangeInfo->prev = &_allocHead;
	rangeInfo->next->prev = rangeInfo;
	rangeInfo->prev->next = rangeInfo;

	return true;
}

void BufMgrFull::FreeBuf(void *buf, TData *tdata, RecId startId){
	/* find the range for this buffer */
	RangeInfo *rangeInfo;
	for (rangeInfo = _allocHead.next; rangeInfo != &_allocHead;
						rangeInfo = rangeInfo->next){
		if (rangeInfo->buf == buf){
			/* found */
			/* remove from list */
			rangeInfo->prev->next = rangeInfo->next;
			rangeInfo->next->prev = rangeInfo->prev;
			rangeInfo->prev = rangeInfo->next = NULL;

			/* update fields */
			rangeInfo->tdata = tdata;
			rangeInfo->low = startId;
			rangeInfo->high = startId + rangeInfo->dataSize/_allocRecSize -1;

			/* Place new range into pllicy arrays */
			int arrayNum,pos;
			if (_policyFlag & ReportPlacement){
				_policy->Placement(rangeInfo,_rangeArrays, arrayNum, pos);
			}
			else 
				_defaultPolicy->Placement(rangeInfo,_rangeArrays,arrayNum,pos);
			_rangeArrays->Insert(arrayNum, pos, rangeInfo);
			rangeInfo->listNum = arrayNum;

			/* put into list of ranges for this TData */
			int numDisposed;
			RangeInfo **disposed;
			(_tdataRangeList->Get(tdata))->
				Insert(rangeInfo,RangeList::MergeIgnore, numDisposed, disposed);

			/* clean range immediately */
			rangeInfo->tdata->WriteRecs(rangeInfo->low,
				rangeInfo->high-rangeInfo->low+1,
				rangeInfo->GetData());
			
			ReportInserted(tdata, rangeInfo->low, rangeInfo->high);

			return;
		}
	}
}

void BufMgrFull::DoneAllocBuf(){ }

void BufMgrFull::InitTDataInMem(TData *tdata){
	int numRanges;
	_tdataRangeList->Get(tdata)->GetRangeList(numRanges,_inMemHead);

	/* Note: we must set this variable to the current one so that
	it's never reclaimed from the list by the buffer manager */
	_inMemCur = _inMemHead;
}

Boolean BufMgrFull::GetInMemRecs(void *&buf, RecId &low, RecId &high){
	_inMemCur = _inMemCur->next;
	if (_inMemCur == _inMemHead)
		return false;
	
	SetUse(_inMemCur);
	buf = _inMemCur->GetData();
	low = _inMemCur->low;
	high = _inMemCur->high;
	return true;
}

void BufMgrFull::FreeInMemRecs(void *buf, Boolean dirty){
	ClearUse(buf, dirty);
}
void BufMgrFull::DoneTDataInMem(){ }

void BufMgrFull::PhaseHint(BufferPolicy::Phase phase){
	_policy->PhaseHint(phase);
}

void BufMgrFull::FocusHint(RecId id, TData *tdata, GData *gdata){
	_policy->FocusHint(id,tdata, gdata);
}

void BufMgrFull::PrintStat(){
	/*
	_rangeArrays->PrintStat();
	*/
	printf("%d bytes retrieved\n",_totalGetRecBytes);
	if (_tReturned > 0)
		printf("TData: returned %d, hits %d, %.2f%%\n",
			_tReturned, _tHits, (double)_tHits/(double)_tReturned*100.0);
	if (_gReturned > 0)
		printf("GData: returned %d, hits %d, %.2f%%\n",
			_gReturned, _gHits, (double)_gHits/(double)_gReturned*100.0);
	if ((_tReturned+_gReturned) > 0)
		printf("Total: returned %d, hits %d, %.2f%%\n",
			_tReturned+_gReturned, _tHits+_gHits,
			(double)(_tHits+_gHits)/(double)(_tReturned+_gReturned)*100.0);
	/*
	printf("%d bufs returned: avg buf size %f, avg data size: %f\n",
		_totalRanges,
		(double)_totalBuf/(double)_totalRanges,
		(double)_totalData/(double)_totalRanges);

	int i;
	for (i=0; i < RANGE_ALLOC_HIST_SIZE; i++){
		printf("%d AllocRange() >= %d, avg size %f\n",
		_totalAllocRange[i], _rangeHistSize[i],
		(double)_totalAllocRangeBytes[i]/(double)_totalAllocRange[i]);
	}

	printf("%d totalGetRecs, avg input Buf %f, avg size %f\n",
		_numGetRecs, (double)_totalGetRecBufSize/(double)_numGetRecs,
		(double)_totalGetRecBytes/(double)_numGetRecs);
	*/
}

void BufMgrFull::Clear(){
	/* Free all RangeInfos that have been allocated */
	int numArrays = _rangeArrays->NumArrays();
	int i;
	for (i=0; i < numArrays; i++) {
		for (_rangeArrays->InitIterator(i); _rangeArrays->More(i); ){
			RangeInfo *rInfo = _rangeArrays->Next(i);
			_rangeAlloc->FreeRange(rInfo);
		}
	}

	/* Clear the arrays of RangeInfos used by buffer manager policy */
	_rangeArrays->Clear();

	_tdataRangeList->Clear();

	_policy->Clear();
	_defaultPolicy->Clear();


	/* Clear range allocator to go back to 1 contiguous block of memory */
	_rangeAlloc->Clear();
}

/* Clear buffers occupied by GData */
void BufMgrFull::ClearGData(GData *gdata) {
	int numArrays = _rangeArrays->NumArrays();
	int i;
	for (i=0; i < numArrays; i++){
		int size = _rangeArrays->Size(i);
		int j;
		while ( j < size) {
			RangeInfo *info = _rangeArrays->GetRange(i,j);
			if (info->GetTData() == gdata){
				/* clear this one */
				UnlinkRange(info,i,j);
				ReportDeleted(info->GetTData(),info->low, info->high);
				_rangeAlloc->FreeRange(info);
				size--;
			}
			else j++;
		}
	}
}
