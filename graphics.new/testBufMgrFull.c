/*
  $Id$

  $Log$*/

/* test buffer mechanism */

#include <stdio.h>
#include "DeviseTypes.h"
#include "Exit.h"
#include "Parse.h"
#include "TData.h"
#include "RangeInfo.h"
#include "RangeInfoArray.h"
#include "BufferPolicy.h"
#include "BufMgrFull.h"

struct TestRec {
	int val;
	char padding[508];
};

const int NUM_RECORDS = 512;
const int REC_SIZE = sizeof(TestRec);
int retIndex[1];
class TestTData: public TData {
public:
	/**** MetaData about TData ****/

	/* Return # of dimensions and the size of each dimension,
	or -1 if unknown */
	virtual int Dimensions(int *sizeDimension){
		sizeDimension[0] = NUM_RECORDS;
		return 1;
	};

	/* Return record size, or -1 if variable record size */
	virtual int RecSize(){ return REC_SIZE; }

	/* Return page size of TData, or -1 if no paging structure */
	virtual int PageSize(){ return -1; }

	/* Return true if TData deletes records from the beginning
	due to limited disk/memory buffer. */
	virtual Boolean HasDeletion(){ return 0; }

	/* Return true if TData appends records */
	virtual Boolean HasAppend(){ return 0; }

	/* convert RecId into index */
	virtual void GetIndex(RecId id, int *&indices){
		retIndex[0] = id;
	};

	/**** Getting record Id's ****/

	/* Get RecId of 1st available record, return true if available */
	virtual Boolean HeadID(RecId &recId){ recId = 0; return 1; }

	/* Get RecId of last record, return true if available */
	virtual Boolean LastID(RecId &recId){ recId = NUM_RECORDS-1; return 1;}

	/**** Getting Records ****/

	virtual void InitGetRecs(RecId lowId, RecId highId,RecordOrder order){
		nextId = lowId;
		endId = highId;
	};

	/* Get as many records as can fit into the provided buffer.
	Return number of records. For varialbe size records, set recPtrs to point to
	address of individual records. */
	virtual Boolean GetRecs(void *buf, int bufSize, RecId &startRid,
				int &numRecs, int &dataSize, void **recPtrs){
		if (nextId > endId)
			return false;

		TestRec *ptr = (TestRec *)buf;
		numRecs = bufSize / REC_SIZE;
		if (numRecs > endId-nextId+1)
			numRecs = endId-nextId+1;
		int i;
		for (i=0; i < numRecs; i++){
			ptr->val = (nextId+i);
			ptr++;
		}

		startRid = nextId;
		nextId += numRecs;

		dataSize = numRecs*REC_SIZE;
		return true;
	}

	virtual void DoneGetRecs() {}

	/* Given buffer space and RecId, set the array "recPtrs" to
	the address of individual records. For varialbe size records. */
	virtual void GetRecPointers(RecId startId, int numRecs,
		void *buf, void **recPtrs){
		fprintf(stderr,"TestTData::GetRecPointers() not implemented\n");
		Exit::DoExit(1);
	}

private:
	RecId nextId, endId;
};

int debug =0 ;
/* Get records from buffer mechanism */
void GetRecs(BufMgr *mgr, TData *tdata, RecId getLow, RecId getHigh){
	printf("getting records\n");
	RangeInfo *rangeInfo;
	RecId startRid;
	int numRecs;
	void *buf;
	void **recs;
	mgr->InitGetRecs(tdata,getLow,getHigh,AnyOrder);
	while (mgr->GetRecs(startRid, numRecs, buf, recs)){
		printf("(%d,%d,%d,%d)",startRid,startRid+numRecs-1,
			((TestRec *)buf)->val, (((TestRec *)buf)+numRecs-1)->val);
	}
	printf("\n");
	mgr->Print(tdata);
}

main(){
	TData *tdata = new TestTData;
	BufMgr *mgr = new BufMgrFull(4096*10);

	for (; ;){
		char buf[256];
		printf("enter ranges: \n");
		gets(buf);
		int numArgs; char **args;
		Parse(buf,numArgs, args);
		if (numArgs == 2){
			RecId low = atoi(args[0]);
			RecId high = atoi(args[1]);
			if (low >= 0 && low < NUM_RECORDS && high >= 0 
				&& high < NUM_RECORDS)
				GetRecs(mgr, tdata, low, high);
			else {
				printf("invalid recId %d %d\n", low, high);
			}
		}
		else 
			printf("need at least 2 args\n");
	}
}
