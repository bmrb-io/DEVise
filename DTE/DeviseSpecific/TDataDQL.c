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
  Revision 1.15  1997/06/16 16:05:11  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.13  1997/06/10 19:21:25  wenger
  Removed (some) debug output.

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
#include <string.h>
#include <String.h>
#include "AttrList.h"
#include "Timer.h"

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
#include "CatalogComm.h"

#ifdef ATTRPROJ
#   include "ApInit.h"
#else
#   include "Init.h"
#   include "DataSourceWeb.h"
#endif

# define  _STREAM_COMPAT

AttrType getDeviseType(String type){
	if(type == "int"){
		return IntAttr;
	}
	else if(type == "float"){
		return FloatAttr;
	}
	else if(type == "double"){
		return DoubleAttr;
	}
	else if(type.through(5).contains("string")){
		return StringAttr;
	}
	else if(type == "date"){
		return DateAttr;
	}
	else{
		cout << "Don't know DEVise type for: " << type << endl;
		assert(0);
	}
}

TDataDQL::TDataDQL(
	AttrList attrs,char *name, char *type, 
	int numFlds, String* types, int recSize, long totalRecs,
	int* sizes) : 
	TData(name, type, strdup("query"), 0), // query <- name
	_attrs(attrs),
	_numFlds(numFlds),
	_types(types),
	_sizes(sizes),
	_attributeNames(NULL),
	_marshalPtrs(NULL)
{
  
#ifdef DEBUG
    printf("TDataDQL::TDataDQL(name = %s, type = %s, recSize = %d) called\n", 
	name, type, recSize);
    cout << "_attrs = ";
    _attrs.Print();
#endif

	_totalRecs = totalRecs;

	TData::_recSize = recSize;
}

void TDataDQL::runQuery(){

#if defined(DEBUG) || 1
	cout << "Running: " << _query << endl;
#endif

	Timer::StopTimer();
	Engine engine(_query);
     TRY(engine.optimize(), );
     assert(_numFlds == engine.getNumFlds() / 2);
	_types = new TypeID[_numFlds];
	const TypeID* tmpTypes = engine.getTypeIDs();
	for(int i = 0; i < _numFlds; i++){
		_types[i] = tmpTypes[2 * i];
	}
	_marshalPtrs = new MarshalPtr[_numFlds];
	for(int i = 0; i < _numFlds; i++){
		if(_types[i] == DATE_TP){
			_marshalPtrs[i] = dateToUnixTime;
		}
		else{
			_marshalPtrs[i] = getMarshalPtr(_types[i]);
		}
	}
     Tuple* tup;
	Tuple* highTup = new Tuple[_numFlds];
	memset(highTup, 0, _numFlds * sizeof(Tuple));
	Tuple* lowTup = new Tuple[_numFlds];
	memset(lowTup, 0, _numFlds * sizeof(Tuple));

	engine.initialize();
	const Tuple* firstTup = engine.getNext();
	assert(firstTup);

	int offset = 0;
	_sizes = new int[_numFlds]; 
	_attrs.Clear();
	for(int i = 0; i < _numFlds; i++){

		int deviseSize;
		if(_types[i] == DATE_TP){
			deviseSize = sizeof(time_t);	
		}
		else{
			TRY(deviseSize = packSize(_types[i]), );
		}
#if defined(DEBUG)
		cout << "deviseSize = " << deviseSize << endl;
#endif

		_sizes[i] = deviseSize;
		AttrType deviseType = getDeviseType(_types[i]);
		bool hasHighLow = false;
		AttrVal* hiVal = new AttrVal;
		AttrVal* loVal = new AttrVal;
		if(_types[i].through(5).contains("string")){

			// Devise will not take high and low values for strings

			hasHighLow = false;
		}
		else {
			assert((unsigned) _sizes[i] <= sizeof(AttrVal));
			hasHighLow = true;
			_marshalPtrs[i](firstTup[2 * i + 1], (char*) hiVal);
			_marshalPtrs[i](firstTup[2 * i], (char*) loVal);
		}

		_attrs.InsertAttr(i, strdup(_attributeNames[i].chars()), 
			offset, deviseSize, 
			deviseType, false, 0, false, false, hasHighLow, hiVal, 
			hasHighLow, loVal); 
		offset += deviseSize;
	}

	_recSize = offset;

	_totalRecs = ((int) firstTup[1]) + 1;	// max(recId)

//	DataSeg::Set(_tableName, _query, 0, 0);

#if defined(DEBUG)
	cout << "_totalRecs = " << _totalRecs << endl;
//	cout << "Attr list is:\n";
	_attrs.Print();
#endif

	Timer::StartTimer();
}

TDataDQL::TDataDQL(char* tableName, List<char*>* attrList, char* query) : 
	_attrs(tableName), 
	_attributeNames(NULL),
	TData(strdup(tableName), strdup("DQL"), strdup("query"), 0) {
	_tableName = strdup(tableName);
	_marshalPtrs = NULL;
	char* attNames = dteListAttributes(_tableName);
	char* attName = strtok(attNames, " ");
	String minmaxQ("select ");
	const int MAX_NUM_ATTRS = 100;
	_attributeNames = new String[MAX_NUM_ATTRS];
	int i = 0;
	while(attName){
		assert(i < MAX_NUM_ATTRS);	
		_attributeNames[i] = String(attName);
		minmaxQ += String("min(t.") + attName + "), max(t." + attName + ")";
		attName = strtok(NULL, " "); 	
		if(attName){
			minmaxQ += ", ";
		}
		i++;
	}
	_numFlds = i;
	minmaxQ += String(" from ") + _tableName + " as t";
	_query = strdup(minmaxQ.chars());
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
	delete [] _marshalPtrs;
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
#if defined(DEBUG)
    cout << "Invalidating TDataDQL" << endl;
#endif
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

//  Issue a query to the engine;
  String SQLquery;
  SQLquery="select ";
  SQLquery+="* ";
  SQLquery+="from ";
  SQLquery+=_tableName;
  SQLquery+=" as t where ";
  char whereClause[29+sizeof(unsigned long)*2];
  sprintf(whereClause, "t.recId>=%ld and t.recId<=%ld", 
		id, (unsigned long)(id + numRecs - 1));
  SQLquery+=whereClause;
  String query(SQLquery);

#if defined(DEBUG) || 1
	cout << "Running: " << query << endl;
#endif

  Engine engine(query);
  engine.optimize();
CATCH(
     cout << "DTE error coused by query: \n";
     cout << "   " << query << endl;
     currExcept->display();
     currExcept = NULL;
     cout << endl;
     exit(0);
)

  engine.initialize();
  int counter=0; // how many tuples have been written to buf;

  const Tuple* tuple;

  while((tuple = engine.getNext())){

		counter++;
		assert(counter <= numRecs);
		for(int j = 0; j < _numFlds; j++){
			_marshalPtrs[j](tuple[j], (char*) ptr);
			ptr += _sizes[j];
		}
	}

	assert(counter == numRecs);

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

