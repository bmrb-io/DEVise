/* TDataRec.c */
#include <stdio.h>
#include "Config.h"
#include "Exit.h"
#include "TDataRec.h"
#include "RecFile.h"
#include "Init.h"
int  TDATAREC_CHUNK_SIZE = DEVISE_PAGESIZE;


TDataRec::TDataRec(char *name, int recSize):TData(){
	 TDATAREC_CHUNK_SIZE = Init::PageSize();

	_rFile = RecFile::OpenFile(name, recSize);
	if (_rFile == NULL){
		fprintf(stderr,"can't open file %s\n",name);
		Exit::DoExit(1);
	}
	_recSize = recSize;
	_totalRecs = _rFile->NumRecs();
}

TDataRec::~TDataRec(){
	delete _rFile;
}


int TDataRec::Dimensions(int *sizeDimension){
	sizeDimension[0] = _totalRecs;
	return 1;
}

Boolean TDataRec::HeadID(RecId &recId){
	recId = 0;
	return (_totalRecs > 0);
}

Boolean TDataRec::LastID(RecId &recId){
	recId = _totalRecs-1;
	return (_totalRecs > 0);
}

char *TDataRec::GetName(){
	return _rFile->GetName();
}

void TDataRec::InitGetRecs(RecId lowId, RecId highId,RecordOrder order){
	if (lowId < 0 || lowId >= _totalRecs ||
		highId < 0 || highId >= _totalRecs || highId < lowId ){
		fprintf(stderr,"TDataRec::GetRecs: invalid recId %d, %d total=%d\n",
			lowId, highId, _totalRecs);
		Exit::DoExit(1);
	}
	_lowId = lowId;
	_highId = highId;
	_recOrder = order;
	if (_recOrder == Randomize){
		if (_recSize > TDATAREC_CHUNK_SIZE)
			_chunkSize = 1;
		else _chunkSize = TDATAREC_CHUNK_SIZE/RecSize();

		_chunkSkip = 4; /* amount to skip each chunk */

		_chunkStart = _lowId/_chunkSize*_chunkSize; /* start of chunk 0 */
		int numRecs = highId-_chunkStart+1;
		if (numRecs % _chunkSize == 0)
			_numChunks = numRecs/_chunkSize;
		else _numChunks = numRecs/_chunkSize+1;

		_loopIndex = 0;	/* from 0 to _chunkSkip-1 */
		_nextChunk = 0; /* next chunk to return records */
		_nextId = lowId; /* next record to return within the chunk */
	}
	else {
		_nextId = lowId;
		_endId = highId;
	}
}

Boolean TDataRec::GetRecs(void *buf, int bufSize, 
	RecId &startRid,int &numRecs, int &dataSize, void **recPtrs){
	numRecs = bufSize/_recSize;
	if (numRecs == 0){
		fprintf(stderr,"TDataRec::GetRecs(): not enough buffer space \n");
		fprintf(stderr,"buffer size = %d, rec size= %d\n",
			bufSize, _recSize);
		Exit::DoExit(1);
	}

	if (_recOrder == Randomize){
		if (_loopIndex >= _chunkSkip)
			return false;

		/* Return records in randomized order */
		RecId chunkEnd = _chunkStart+ (_nextChunk+1)*_chunkSize -1;
		if (chunkEnd > _highId)
			chunkEnd = _highId;
		if (_nextId > chunkEnd){
			/* go to next chunk */
			_nextChunk += _chunkSkip;
			if (_nextChunk >= _numChunks){
				_loopIndex++;
				if (_loopIndex >= _chunkSkip){
					/* done */
					return false;
				}
				_nextChunk = _loopIndex;
			}
			_nextId = _chunkStart +_nextChunk*_chunkSize;
			chunkEnd = _chunkStart+ (_nextChunk+1)*_chunkSize-1;
			if (chunkEnd > _highId)
				chunkEnd = _highId;
		}

		int recsLeftInChunk = chunkEnd-_nextId+1;
		if (numRecs > recsLeftInChunk)
			numRecs = recsLeftInChunk;

		_rFile->ReadRec(_nextId, numRecs, buf);

		startRid = _nextId;
		dataSize = numRecs*_recSize;
		_nextId += numRecs;

		return true;

	}
	else {
		if (_nextId > _endId)
			return false;


		int num= _endId- _nextId+1;
		if (num < numRecs)
			numRecs = num;

		_rFile->ReadRec(_nextId, numRecs, buf);

		startRid = _nextId;
		dataSize = numRecs*_recSize;
		_nextId += numRecs;

		return true;
	}

}

void TDataRec::DoneGetRecs() {}

/* Given buffer space and RecId, set the array "recPtrs" to
the address of individual records. For varialbe size records. */
void TDataRec::GetRecPointers(RecId startId, int numRecs,
	void *buf, void **recPtrs){

	fprintf(stderr,"TDataRec::GetRecPointers: not implemented\n");
	Exit::DoExit(1);
}

/* convert RecId into index */
void TDataRec::GetIndex(RecId id, int *&indices){
	static int index[1];
	index[0] = id;
	indices = index;
}

/* Writing records */
void TDataRec::WriteRecs(RecId startId, int numRecs, void *buf){
	if (startId >= _totalRecs){
		fprintf(stderr,"WriteRecs: can not write beyond end of file\n");
		Exit::DoExit(2);
	}
	_rFile->WriteRec(startId,numRecs,buf);
	if (startId +numRecs >= _totalRecs)
		_totalRecs = startId+numRecs;
}

int TDataRec::GetModTime(){
	return _rFile->GetModTime();
}

