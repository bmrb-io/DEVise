/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:14:46  jussi
  Added CVS header.
*/

#include "Exit.h"
#include "TDataAttr.h"
#include "GData.h"
#include "GDataRangeMap.h"
#include "UnixRecFile.h"
#include "Init.h"

GData::GData(TData *tdata, char *fname, int recSize, int maxBuf){
	_tdata = tdata;
	/*
	Dispatcher::Current()->Register(this, AllState, true);
	*/

	_recSize = recSize;
	Boolean trunc = true; /* TRUE if we need to truncate file */
	_recFile = UnixRecFile::OpenFile(fname,recSize, trunc);
/*
	if (_recFile != NULL){
		if (_recFile->GetModTime() < Init::ProgModTime() ||
			_recFile->GetModTime() < tdata->GetModTime()){
			delete _recFile;
			trunc = true;
			_recFile = UnixRecFile::OpenFile(fname,recSize, trunc);
		}
	}
*/
	if (_recFile == NULL){
		trunc = true;
		_recFile = UnixRecFile::CreateFile(fname,recSize);
	}
	if (_recFile == NULL){
		fprintf(stderr,"GData: can't create file %s\n", fname);
		Exit::DoExit(2);
	}
	char buf[256];
	sprintf(buf,"%s.Range",fname);
	_rangeMap = new GDataRangeMap(recSize,buf, trunc);
	_totalRecs = _recFile->NumRecs();
	if (maxBuf < 0)
		_recsLeft = -1;
	else _recsLeft = (maxBuf/recSize)- _totalRecs;
}

GData::~GData(){
	/*
	Dispatcher::Current()->Unregister(this);
	printf("GData destructor %s, 0x%x \n", GetName(),this);
	*/

	if (_rangeMap != NULL){
		delete _rangeMap;
	}
	if (_recFile != NULL){
		delete _recFile;
	}
}

int GData::Dimensions(int *sizeDimension){
	RecId id;
	if (_rangeMap->highID(id)){
		sizeDimension[0] = id;
	}
	else sizeDimension[0] = 0;
	return 1;
}


/* Return record size, or -1 if variable record size */
int GData::RecSize(){ return _recSize; }

/* Return page size of TData, or -1 if no paging structure */
int GData::PageSize(){ return -1; }

/* Return true if TData deletes records from the beginning
due to limited disk/memory buffer. */
Boolean GData::HasDeletion(){ return 0; }

Boolean GData::HasAppend() { return true; }

void GData::GetIndex(RecId id, int *&indices){
static indexArray[1];
	indices = indexArray;
	indexArray[0] = id;
}

Boolean GData::HeadID(RecId &recId){
	/*
	return _rangeMap->lowID(recId);
	*/
	return _tdata->HeadID(recId);
}


Boolean GData::LastID(RecId &recId){
	/*
	return _rangeMap->highID(recId);
	*/
	return _tdata->LastID(recId);
}

char *GData::GetName(){
	return _recFile->GetName();
}

/**************************************************************
Init getting records.
***************************************************************/
void GData::InitGetRecs(RecId lowId, RecId highId,RecordOrder order){
/*
printf("GData::InitGetRecs(%d,%d)\n",lowId, highId);
*/
	_nextId = lowId;
	_highId = highId;
	_numRecs = highId-lowId+1;
	_rec = _rangeMap->FindMaxLower(_nextId);
	if (_rec == NULL){
		fprintf(stderr,"GData::InitGetRecs(): %d,%d out of range\n",
			lowId, highId);
		Exit::DoExit(2);
	}
}

Boolean GData::GetRecs(void *buf, int bufSize, RecId &startRid,
		int &numFetched, int &dataFetched, void **recPtrs){
/*
printf("GData::GetRecs bufSize %d, %d recs left, nex Id %d\n", 
	bufSize, _numRecs, _nextId);
*/

	if (_numRecs <= 0)
		return false;
	
	/* Get all the ranges we need to get */
	int num = bufSize/_recSize;
	if (num <= 0){
		fprintf(stderr,"GData::GetRecs: not enough buffer %d\n", bufSize);
		Exit::DoExit(2);
	}

	if (num > _numRecs)
		num = _numRecs;
	
	char *bufAddr = (char *)buf;

	/* Set return param and update internal vars */
	startRid = _nextId;
	numFetched = num;
	_numRecs -= num;
	dataFetched = numFetched *_recSize;

/*
printf("%d fetched, %d bytes returned, %d left\n", numFetched, dataFetched,
	_numRecs);
*/
	while (num > 0){
		if (_nextId > _rec->tHigh){
			/* finished this range. try next range */
			_rec = _rangeMap->NextRangeRec(_rec);
			if (_rec == NULL){
				fprintf(stderr,"GData::GetRecs: reading beyond end of range\n");
				Exit::DoExit(2);
			}
			continue;
		}

		/* Get data in this range */
		if (_nextId < _rec->tLow || _nextId > _rec->tHigh){
			fprintf(stderr,"GData::GetRecs: internal error\n");
			Exit::DoExit(2);
		}

		int numToReturn = _rec->tHigh- _nextId+1;
		if (numToReturn > num )
			numToReturn = num;
		RecId gId = _rec->gLow + (_nextId- _rec->tLow);
/*
printf("getting gId %d, num %d\n", gId, numToReturn);
*/
		_recFile->ReadRec(gId, numToReturn, bufAddr);

		bufAddr += numToReturn*_recSize;
		num -= numToReturn;
		_nextId += numToReturn;

	}

	return true;
}

void GData::DoneGetRecs(){
}

/* For writing records. Default: not implemented. */
void GData::WriteRecs(RecId startId, int numRecs, void *buf){
	GDataRangeMapRec *rec = _rangeMap->FindMaxLower(startId);
	RecId nextId = startId;
	char *curBuf = (char *)buf;
	while (numRecs > 0){
		if (rec == NULL || nextId < rec->tLow || nextId > rec->tHigh){
			if (rec != NULL)
				fprintf(stderr,
					"GData::Write Recs (%d,%d) into gap T(%d,%d),G(%d,%d)\n",
					startId, startId+numRecs-1,
					rec->tLow, rec->tHigh, rec->gLow, rec->gHigh);
			else fprintf(stderr,"GData::Write Recs (%d,%d) into gap\n",
					startId, startId+numRecs-1);
			Exit::DoExit(2);
		}

		/*
		printf("GDataWriteRecs: T:(%d,%d), G:(%d,%d)\n",
			rec->tLow,rec->tHigh,rec->gLow,rec->gHigh);
		*/

		/* Write this record */
		RecId gId = rec->gLow + (nextId - rec->tLow);
		int numToWrite = rec->tHigh - nextId +1;
		if (numToWrite > numRecs)
			numToWrite = numRecs;
		_recFile->WriteRec(gId, numToWrite, curBuf);

		numRecs -= numToWrite;
		curBuf += (numToWrite*_recSize);

		rec = _rangeMap->NextRangeRec(rec);
	}
}


/* Update info about which TData records have been converted into GData */
void GData::UpdateConversionInfo(RecId tLow, RecId tHigh,
	void *firstRec, void *lastRec){
	int numRecs = tHigh-tLow+1;
	if (_recsLeft > 0 && numRecs > _recsLeft){
		fprintf(stderr,"GData::UpdateconversionInfo: too many Recods\n");
		Exit::DoExit(2);
	}
	_rangeMap->InsertRange(tLow,tHigh,_totalRecs,_totalRecs+numRecs-1,
		firstRec, lastRec);
	_totalRecs += numRecs;
	if (_recsLeft > 0) _recsLeft -= numRecs;

	/* update info about conversion to all callbacks */
	int index;
	for (index=_callbacks.InitIterator(); _callbacks.More(index); ){
		GDataCallback *c = _callbacks.Next(index);
		c->Converted(tLow, tHigh);
	}
	_callbacks.DoneIterator(index);

/*
printf("GData: %s update %d,%d, %d left\n",GetName(), tLow, tHigh, _recsLeft);
*/
}

void GData::GetRecPointers(RecId startId, int numRecs, void *buf, 
								void **recPtrs){
	fprintf(stderr,"GData::GetRecPointers: should not be called\n");
	Exit::DoExit(2);
}

void GData::InitConvertedIterator(){
	_rangeMap->InitListIterator();
}
Boolean GData::NextRange(RecId &lowId, RecId &highId){
	GDataRangeMapRec *rec = _rangeMap->NextRec();
	if (rec == NULL)
		return false;
	else {
		lowId = rec->tLow;
		highId = rec->tHigh;
		return true;
	}
}

void GData::DoneConvertedIterator(){ }

int GData::UserAttr(int attrNum){
	if(attrNum == GDataAttrNum)
		return 1;
	else TData::UserAttr(attrNum);
}

/* Get next unconverted range <= id.
Return true if open ended (no high limit)*/
Boolean GData::NextUnConverted(RecId id, RecId &low, RecId &high){
	return _rangeMap->NextUnprocessed(id,low,high);
}

void GData::PrintConverted(){
	printf("GData converted: ");
	_rangeMap->Print();
}

void GData::RegisterCallback(GDataCallback *c){
	_callbacks.Append(c);
}

/*
void GData::Cleanup(){
	printf("GData::Cleanup %s: 0x%x \n", GetName(),this);
	if (_recFile != NULL){
		delete _recFile;
		_recFile = NULL;
	}
	if (_rangeMap != NULL){
		delete _rangeMap;
		_rangeMap = NULL;
	}
}
*/

int GData::GetModTime() { return _recFile->GetModTime(); }

AttrList *GData::GetAttrList() { return NULL; }

