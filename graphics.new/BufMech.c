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
  Revision 1.1  1995/09/22 17:04:59  jussi
  Restoring this file; it had disappeared.

  Revision 1.2  1995/09/05 22:14:09  jussi
  Added CVS header.
*/

#include "BufMech.h"
#include "RangeInfo.h"
#include "RangeInfoArray.h"
#include "RangeHash.h"
#include "BufferPolicy.h"
#include "TData.h"
#include "Exit.h"

/* constructor.
bufSize == total buffer space in bytes.
numArrays == # of arrays for policy.
callBack == policy manager.
callbackFlag: decides which callback functions to call.
*/
BufMech::BufMech(int bufSize, BufferPolicy *policy){
	_policy = policy;
	_policy->Info(_numArrays, _policyFlag);
	_bufSize = bufSize;
	RangeInfo::SetMaxMemory(bufSize);
	_rangeHash = new RangeHash;
	_rangeArrays = new RangeInfoArrays(_numArrays, RangeInfo::MaxPages());
	_retRanges = new RangeInfo *[RangeInfo::MaxPages()];
}

/******* Retrieving records *******/

void BufMech::InitGet(TData *tdata, RecId lowId, RecId highId){
	int recSize = tdata->RecSize();
	if (recSize <= 0){
		fprintf(stderr,"can't handle variable size records yet\n");
		Exit::DoExit(2);
	}
	_recsPerPage =  RangeInfo::RecordsPerPage(recSize);
	_nextId = lowId/_recsPerPage * _recsPerPage;
	_highId = highId;
	_tdata = tdata;

}

/* Retrieve next batch of records.  Return TRUE if there still more batches. */
Boolean BufMech::Get(RangeInfo *&rangeInfo){
	if (_nextId > _highId)
		return false;
	
	rangeInfo = FetchRange(_tdata,_recsPerPage, _nextId);
	_nextId = rangeInfo->high+1;
	return true;
}

/* Free batch of records returned by GetRecs(), with buf
as the address returned by GetRecs().
Give buffer manager hint about how to dispose the records.  */
void BufMech::Free(RangeInfo *info){
	/*
	info->ClearUse();
	*/
}

/* Get info about TData ranges */
void BufMech::GetInMemRanges(TData *tdata, RecId lowId, RecId highId,
	RangeInfo **&info, int &numInfo){
	int index = 0;
	int i;
	for (i=0; i < _numArrays; i++){
		for (_rangeArrays->InitIterator(i); _rangeArrays->More(i);){
			RangeInfo *rangeInfo = _rangeArrays->Next(i);
			if (rangeInfo->tdata == tdata && 
				!(rangeInfo->high <= lowId || rangeInfo->low >= highId))
				_retRanges[index++] = rangeInfo;
		}
	}
	info = _retRanges;
	numInfo = index;
}

/* Fetch a range from TData, handling misses */
RangeInfo *BufMech::FetchRange(TData *tdata, int recsPerPage,
	RecId startId){

	RangeInfo *rangeInfo;
	if (_rangeHash->Find(tdata,startId, rangeInfo)){
		/*
		rangeInfo->SetUse();
		*/
		return rangeInfo;
	}

	/* Process buffer miss */
	int arrayNum, pos;	/* victim */
	if (RangeInfo::MoreToAlloc()){
		/* still has more ranges to allocated */
		rangeInfo = new RangeInfo;
	}
	else {
		/* Do replacement */
		if (_policyFlag & ReportVictim){
			/* pick a victim */
			_policy->PickVictim(_rangeArrays, arrayNum,pos);
			RangeInfo *info = _rangeArrays->GetRange(arrayNum, pos);
			if (info->InUse()){
				fprintf(stderr,"BufMech:: victim in use\n");
				Exit::DoExit(2);
			}
		}
		else {
			/* pick one ourselves */
			Boolean found = false;
			int i,j;
			for (i=0; i < _numArrays; i++){
				for (j=0; j < _rangeArrays->Size(i); j++){
					RangeInfo *info = _rangeArrays->GetRange(i,j);
					if (!info->InUse()){
						arrayNum = i;
						pos = j;
						found = true;
						goto done;
					}
				}
			}
			done:
			if (!found){
				fprintf(stderr,"BufMech: can't find victim\n");
				Exit::DoExit(2);
			}
		}
		/* Get rid of it from the array */
		rangeInfo = _rangeArrays->Delete(arrayNum, pos);

		/* Get rid of it from hash table */
		_rangeHash->Delete(rangeInfo);
	}


	/* Read new range from tdata */
	int numRecs; void **recPtrs;
	int dSize, bSize;
	rangeInfo->Sizes(dSize,bSize);
	tdata->GetRecs(startId,startId+recsPerPage-1,rangeInfo->data, 
		bSize, numRecs, NULL);
	/* init rangeInfo */
	rangeInfo->tdata = tdata;
	rangeInfo->low = startId;
	rangeInfo->high = startId + numRecs-1;
/*
printf("setting RangeInfo addr 0x%p, IDs %d,%d\n",
			rangeInfo, rangeInfo->low, rangeInfo->high);
*/
	rangeInfo->dataSize = numRecs*recsPerPage;

	if (_policyFlag & ReportPlacement)
		_policy->Placement(rangeInfo, _rangeArrays, arrayNum,pos);
	else {
		arrayNum = 0;
		pos = 0;
	}

	/* put it back into the arrays */
	rangeInfo->listNum = arrayNum;
	_rangeArrays->Insert(arrayNum, pos, rangeInfo);

	/* put it back into hash table */
	_rangeHash->Insert(rangeInfo);
	/*
	rangeInfo->SetUse();
	*/
	return rangeInfo;
}

/* Print */
void BufMech::Print(){
	int i,j;
	printf("Buffer manager lists:\n");
	for (i=0; i < _numArrays; i++){
		printf("array %d: ",i);
		for (j = 0; j < _rangeArrays->Size(i); j++){
			RangeInfo *rangeInfo = _rangeArrays->GetRange(i,j);
			printf("(%d,%d)",rangeInfo->low, rangeInfo->high);
		}
		printf("\n");
	}
}
