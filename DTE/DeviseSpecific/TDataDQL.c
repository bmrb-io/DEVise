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

// #define DEBUG
#include <assert.h>
#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "AttrList.h"

#include "Parse.h"
#include "TDataDQL.h"
#include "Exit.h"
#include "Util.h"
#include "DataSourceFileStream.h"
#include "DataSourceSegment.h"
#include "DataSourceBuf.h"
#include "DevError.h"
#include "DataSeg.h"
#include "QueryProc.h"

#include "Engine.h"
#include "exception.h"
#include "types.h"
#include "TuplePtr.XPlex.h"
#include "queue.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#   include "DataSourceWeb.h"
#endif

# define  _STREAM_COMPAT

TDataDQL::TDataDQL(
	AttrList attrs,char *name, char *type, 
	int numFlds, String* types, int recSize, TuplePtrXPlex& result,
	int* sizes) : 
	TData(name, type, strdup("query"), 0), // query <- name
	_attrs(attrs),
	_numFlds(numFlds),
	_types(types),
	_result(result),
	_sizes(sizes)
{
  
#ifdef DEBUG
    printf("TDataDQL::TDataDQL(name = %s, type = %s, recSize = %d) called\n", 
	name, type, recSize);
    cout << "_attrs = ";
    _attrs.Print();
#endif

	_totalRecs = _result.length();

	TData::_recSize = recSize;
}

void TDataDQL::runQuery(){

	_result.clear();
     Engine engine(_query);
     TRY(engine.optimize(), );
     _numFlds = engine.getNumFlds();
     _types = engine.getTypeIDs();
     Tuple* tup;
	Tuple* highTup = new Tuple[_numFlds];
	Tuple* lowTup = new Tuple[_numFlds];

	engine.initialize();
	Tuple* firstTup = engine.getNext();
	if(firstTup){
		for(int i = 0; i < _numFlds; i++){
			lowTup[i] = firstTup[i];
			highTup[i] = firstTup[i];
		}
          _result.add_high(firstTup);
	}
	OperatorPtr* lessPtrs = new OperatorPtr[_numFlds];
	OperatorPtr* greaterPtrs = new OperatorPtr[_numFlds];
     for(int i = 0; i < _numFlds; i++){
          TypeID retVal;
		GeneralPtr* tmp;
          TRY(tmp = getOperatorPtr("<",_types[i],_types[i],retVal), );
		assert(tmp);
		lessPtrs[i] = tmp->opPtr;
          TRY(tmp = getOperatorPtr(">",_types[i],_types[i],retVal), );
		assert(tmp);
		greaterPtrs[i] = tmp->opPtr;
     }

     while((tup = engine.getNext())){
		updateHighLow(_numFlds, lessPtrs, greaterPtrs, tup, highTup, lowTup);
          _result.add_high(tup);
     }

#ifdef DEBUG
     for(int j = _result.low(); j < _result.fence(); j++){
          for(int i = 0; i < _numFlds; i++){
               displayAs(cout, _result[j][i], _types[i]);
               cout << '\t';
          }
          cout << endl;
     }
#endif

	int offset = 0;
	_sizes = new int[_numFlds]; 
	attrList->rewind();
	_attrs.Clear();
	for(int i = 0; i < _numFlds; i++){
		assert(!attrList->atEnd());
		char* atname = attrList->get();
		attrList->step();
		TRY(int deviseSize = packSize(_types[i]), );
		_sizes[i] = deviseSize;
		AttrType deviseType = getDeviseType(_types[i]);
		AttrVal* hiVal = (AttrVal*) highTup[i];
		AttrVal* loVal = (AttrVal*) lowTup[i];

		// Devise will not take high and low values for strings

		bool hasHighLow = true;
		if(_types[i].through(5).contains("string")){
			hasHighLow = false;
		}
		_attrs.InsertAttr(i, strdup(atname), offset, deviseSize, 
			deviseType, false, 0, false, false, hasHighLow, hiVal, 
			hasHighLow, loVal); 
		offset += deviseSize;
	}

	_recSize = offset;

	_totalRecs = _result.length();

//	DataSeg::Set(_tableName, _query, 0, 0);
}

TDataDQL::TDataDQL(char* tableName, List<char*>* attrList, char* query) : _attrs(tableName), TData(strdup(tableName), strdup("DQL"), strdup("query"), 0) {
	_tableName = strdup(tableName);
	_query = strdup(query);
	this->attrList = attrList;
	runQuery();
	CATCH(
		cout << "DTE error coused by query: \n";
		cout << "   " << _query << endl;
		currExcept->display(); 
		currExcept = NULL; 
		cout << endl;
		exit(0);
	)
}

AttrList* TDataDQL::GetAttrList(){
  return &_attrs;
}

TDataDQL::~TDataDQL()
{
#ifdef DEBUG
  printf("TDataDQL destructor\n");
#endif

}

Boolean TDataDQL::CheckFileStatus()
{
  return true;
}

int TDataDQL::Dimensions(int *sizeDimension)
{
  sizeDimension[0] = _totalRecs;
  return 1;
}

Boolean TDataDQL::HeadID(RecId &recId)
{
  recId = 0;
  return (_totalRecs > 0);
}

Boolean TDataDQL::LastID(RecId &recId)
{
  // Find the last record 
  recId = _totalRecs - 1;
  return (_totalRecs > 0);
}

TData::TDHandle TDataDQL::InitGetRecs(RecId lowId, RecId highId,
                                      Boolean asyncAllowed,
                                      ReleaseMemoryCallback *callback)
{
  DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
	   && highId >= lowId, "Invalid record parameters");

  TDataRequest *req = new TDataRequest;
  DOASSERT(req, "Out of memory");

  req->nextId = lowId;
  req->endId = highId;
  req->relcb = callback;

#ifdef DEBUG
  cout << "TDataDQL::InitGetRecs(" << lowId << ", " << highId << ")\n";
#endif

  return req;
}

Boolean TDataDQL::GetRecs(TDHandle req, void *buf, int bufSize, 
                          RecId &startRid, int &numRecs, int &dataSize)
{
  DOASSERT(req, "Invalid request handle");

  numRecs = bufSize / _recSize;
  DOASSERT(numRecs, "Not enough record buffer space");

  if (req->nextId > req->endId)
    return false;
  
  int num = req->endId - req->nextId + 1;
  if (num < numRecs)
    numRecs = num;
  
  ReadRec(req->nextId, numRecs, buf);
  
  startRid = req->nextId;
  dataSize = numRecs * _recSize;
  req->nextId += numRecs;
  
  _bytesFetched += dataSize;

#ifdef DEBUG
  printf("TDataDQL::GetRecs buf = ");
  cout << " _recSize = " <<  _recSize << endl;
  for(int i = 0; i < _recSize; i++){
	printf("%x, ", *(((char*)buf) + i));
	// cout << hex << (short) *(((char*)buf) + i) << ", ";
  }
  cout << endl;
#endif

  
  return true;
}

void TDataDQL::DoneGetRecs(TDHandle req)
{
  DOASSERT(req, "Invalid request handle");

  delete req;
}

void TDataDQL::GetIndex(RecId id, int *&indices)
{
  static int index[1];
  index[0] = id;
  indices = index;
}

int TDataDQL::GetModTime()
{
  if (!CheckFileStatus())
    return -1;

  return _data->GetModTime();
}

char *TDataDQL::MakeIndexFileName(char *name, char *type)
{
  return 0;
}

void TDataDQL::Initialize()
{
 // Probably do all the things here..

}

void TDataDQL::Checkpoint()
{
}

void TDataDQL::InvalidateTData()
{
    cout << "Invalidating TDataDQL" << endl;
    runQuery();
	CATCH(
		cout << "DTE error coused by query: \n";
		cout << "   " << _query << endl;
		currExcept->display(); 
		currExcept = NULL; 
		cout << endl;
		exit(0);
	)
    TData::InvalidateTData();
}


/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

void TDataDQL::BuildIndex()
{

 // Find the last record, total Recs,...
 // _totalRecs = 2000;

}

TD_Status TDataDQL::ReadRec(RecId id, int numRecs, void *buf){
	
	char *ptr = (char *)buf;
	for(long unsigned int i = id  ; i < numRecs + id; i++){
		for(int j = 0; j < _numFlds; j++){
			marshal(_result[i][j], (char*) ptr, _types[j]);
			ptr += _sizes[j];
		}
	}

    return TD_OK;
}

void TDataDQL::WriteRecs(RecId startRid, int numRecs, void *buf)
{
 printf(" Failing Write Recs not supported \n");
 assert(0); 
}

void TDataDQL::WriteLine(void *line)
{
 printf(" Failing Write Recs not supported \n");
 assert(0); 
}

void TDataDQL::Run()
{
}

void TDataDQL::Cleanup()
{
}

void TDataDQL::PrintIndices()
{
}

