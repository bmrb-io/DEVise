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
  Revision 1.35  1999/01/21 16:43:06  beyer
  removed debugging statements

  Revision 1.34  1999/01/20 22:46:38  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

  Revision 1.33  1998/06/29 17:18:16  wenger
  Fixed bug 372 (crashed in DataReader caused by a pointer alignment problem).

  Revision 1.32  1998/04/16 17:34:58  donjerko
  *** empty log message ***

  Revision 1.31  1998/04/14 17:04:02  donjerko
  Removed command "schema", using typecheck "select * from ..." instead.

  Revision 1.30  1998/04/10 16:25:53  donjerko
  *** empty log message ***

  Revision 1.29  1998/03/12 18:23:46  donjerko
  *** empty log message ***

  Revision 1.28  1997/12/23 23:34:19  liping
  Changed internal structure of BufMgrFull and classes it called
  The buffer manager is now able to accept queries on any attribute from the
          Query Processor
  The buffer manager is also able to issue queries on various attributes to DTE
  Instead of keeping an in memory list for each T/GData, the buffer manager keeps
          a list for each (T/GData, AttrName, Granularity) combination
  The class Range was replaced by Interval

  Revision 1.27  1997/12/04 04:05:33  donjerko
  *** empty log message ***

  Revision 1.26  1997/11/12 23:17:47  donjerko
  Improved error checking.

  Revision 1.25  1997/11/05 00:20:28  donjerko
  Added some error checking calls to the DTE.

  Revision 1.24  1997/10/10 21:07:27  liping
  The interface provided by TData::InitGetRecs and TData::GetRecs was changed
  The new interface carries the information of 1. LowId 2. HighId 3. AttrName
  	4. Granularity in the structure "Range"

  Revision 1.23  1997/09/17 02:36:07  donjerko
  *** empty log message ***

  Revision 1.22  1997/09/05 22:56:17  donjerko
  *** empty log message ***

  Revision 1.21  1997/09/05 22:20:31  donjerko
  Made changes for port to NT.

  Revision 1.20  1997/08/21 21:04:52  donjerko
  Implemented view materialization

  Revision 1.19  1997/08/09 00:55:03  donjerko
  Added indexing of select-project unmaterialized views.

  Revision 1.18  1997/07/22 15:01:31  donjerko
  Moved querying in InitGetRec

  Revision 1.17  1997/07/03 01:51:44  liping
  changed query interface to TData from RecId to double

  Revision 1.16  1997/06/27 23:17:29  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.15  1997/06/16 16:05:11  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.13  1997/06/10 19:21:25  wenger
  Removed (some) debug output.

*/

//#define DEBUG

#include "TDataDQL.h"
#include "Timer.h"

#include "CatalogComm.h"
#include "Utility.h"
#include "Engine.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteDateAdt.h"

AttrType getDeviseType(const DteAdt& adt)
{
  switch(adt.getTypeID()) {
  case DteIntAdt::typeID:
    return IntAttr;
  case DteDoubleAdt::typeID:
    return DoubleAttr;
  case DteStringAdt::typeID:
    return StringAttr;
  case DteDateAdt::typeID:      // but we convert it to a time_t
    return DateAttr;
  }
  cout << "Don't know DEVise type for: " << adt.getTypeSpec() << endl;
  assert(0);
#if 0
  else if(type == "time_t"){
    return DateAttr;
  }
  else if(type == "float") {
    return FloatAttr;
  }
#endif
}


#if 0
TDataDQL::TDataDQL(
  AttrList attrs,char *name, char *type, 
  int numFlds, string* types, int recSize, long totalRecs,
  int* sizes) : 
  TData(name, type, strdup("query"), 0), // query <- name
  _attrs(attrs),
  _numFlds(numFlds),
  _types(types),
  _sizes(sizes),
  _attributeNames(NULL),
  engine(NULL),
  tuple(0)
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
#endif


TDataDQL::TDataDQL(const string& tableName)
  : TData(strdup(tableName.c_str()), strdup("DQL"), strdup("query"), 0),
    _tableName(tableName), _attrs((char*)tableName.c_str()),
    engine(NULL), tuple(NULL)
{
  EXIT(char* attNames = dteListAttributes(_tableName.c_str()));
  assert(attNames);
  char* attName = strtok(attNames, " ");
  minMaxQuery = "select ";
  queryHeader = "select ";
  int i = 0;
  _recIdAttrPosition = -1;
  while(attName) {
    _attributeNames.push_back(attName);
    if(_attributeNames[i] == RID_STRING) {
      _recIdAttrPosition = i;
    }
    string delimiAttNm = addSQLQuotes(attName, '"');
    minMaxQuery += string("min(t.") + delimiAttNm
      + "), max(t." + delimiAttNm + ")";
    queryHeader += string("t.") + delimiAttNm;
    attName = strtok(NULL, " "); 	
    minMaxQuery += ", ";
    if(attName) {
      queryHeader += ", ";
    }
    i++;
  }
  //assert(_recIdAttrPosition >= 0 || !"recId not found in schema");
  minMaxQuery += "count(*)";
  _numFlds = i;
  assert(_numFlds > 0);
  minMaxQuery += string(" from ") + _tableName + " as t";
  queryHeader += string(" from ") + _tableName + " as t";
  runMinMaxQuery();
  CATCH(
    cout << "DTE error caused by query: \n";
    cout << "   " << minMaxQuery << endl;
    cout << currExcept->toString(); 
    currExcept = NULL;
    cout << endl;
    assert(0);
    );
}


void TDataDQL::runMinMaxQuery(){

#if defined(DEBUG)
  cout << "Running query: " << _query << endl;
  cerr << ".";
#endif

  Timer::StopTimer();

  _nextToFetch = _totalRecs;

  delete engine;
  tuple = 0;
  engine = new Engine(minMaxQuery);
  TRY(engine->optimize(), );
  if(2 * _numFlds + 1 != engine->getNumFields()){
    cerr << "_numFlds = " << _numFlds << endl;
    cerr << "engine->getNumFields() = " << engine->getNumFields() << endl;
  }
  assert(2 * _numFlds + 1 == engine->getNumFields());

  TRY(const Tuple* firstTup = engine->getFirst(), );
  if(!firstTup){
    cout << "Empty result set" << endl;
  }

  int offset = 0;
  _sizes.resize(_numFlds+1);	// store rec len in last pos
  const DteTupleAdt& tupAdt = engine->getAdt();
  
  for(int i = 0; i < _numFlds; i++) {

    //kb: fix dates
    //if(_types[i] == DATE_TP){
    //deviseSize = sizeof(time_t);	
    //}
    const DteAdt& adt = tupAdt.getAdt(2*i);

    int align;
    int size;
    if( adt.getTypeID() == DteDateAdt::typeID ) {
      align = INT4_ALIGN;       // assumes time_t == int4
      size = sizeof(time_t);
    } else {
      align = adt.getAlignment();
      size = adt.getMaxSize();
      assert(size > 0);
    }

    offset = Align(offset, align);
    _sizes[i] = offset;		// sizes[] stores offsets right now

    AttrType deviseType = getDeviseType(adt);
    bool hasHighLow = false;
    AttrVal hiVal;
    AttrVal loVal;
    if( deviseType == StringAttr ) {
      // Devise will not take high and low values for strings
      hasHighLow = false;
    }
    else if(firstTup && firstTup[2*i] && firstTup[2*i+1]) {
      assert((unsigned)size <= sizeof(AttrVal));
      hasHighLow = true;
      int rc;
      if( adt.getTypeID() == DteDateAdt::typeID ) {
        loVal.dateVal = DteDateAdt::toUnixTime(firstTup[2 * i]);
        hiVal.dateVal = DteDateAdt::toUnixTime(firstTup[2 * i + 1]);
      } else {
        rc = adt.copyNonNull(firstTup[2 * i],
                             (char*)&loVal, sizeof(AttrVal));
        assert(rc >= 0);
        rc = adt.copyNonNull(firstTup[2 * i + 1],
                             (char*)&hiVal, sizeof(AttrVal));
        assert(rc >= 0);
      }
    }
    else {
      hasHighLow = false;
    }

    _attrs.InsertAttr(i, strdup(_attributeNames[i].c_str()), 
                      offset, size, deviseType, false, 0, false, false,
		      hasHighLow, &hiVal, hasHighLow, &loVal);
    offset += size;
  }
  offset = Align(offset, MAX_ALIGN);
  _recSize = offset;
  _sizes[_numFlds] = offset;

  for(int i = 0 ; i < _numFlds ; i++) {
    _sizes[i] = _sizes[i+1] - _sizes[i];
  }

  // get count(*) = max(recId) = total num records
  _totalRecs = 0;
  if(firstTup && firstTup[2*_numFlds]) {
    _totalRecs = DteIntAdt::cast(firstTup[2*_numFlds]);
    if( _recIdAttrPosition >= 0 ) { // if record as recId field
      const Type* x = firstTup[2 * _recIdAttrPosition + 1];
      if( x ) { // max(recId) not null
	assert(_totalRecs == DteIntAdt::cast(x) + 1);
      }
    }
  }

#if defined(DEBUG)
  cout << "_totalRecs = " << _totalRecs << endl;
  //	cout << "Attr list is:\n";
  _attrs.Print();
#endif

  // DataSeg::Set(_tableName, _query, 0, 0);

  Timer::StartTimer();
}


AttrList* TDataDQL::GetAttrList(){
  return &_attrs;
}

TDataDQL::~TDataDQL()
{
#ifdef DEBUG
  printf("TDataDQL destructor\n");
#endif
  delete engine;
}

//  Boolean TDataDQL::CheckFileStatus()
//  {
//    return true;
//  }

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

TData::TDHandle TDataDQL::InitGetRecs(Interval *interval, int &bytesleft,
                                      Boolean asyncAllowed,
                                      ReleaseMemoryCallback *callback
                                      )
{
	
#if defined(DEBUG)
  cerr << "TDataDQL::InitGetRecs(" << interval->Low << ", " << interval->High << ")\n";
#endif

  if( RID_STRING != interval->AttrName ) {
    cout << "TDataDQL: Only recId is implemented right now.\n";
    assert(0);
  }

  RecId lowId = (RecId)(interval->Low);
  RecId highId = (RecId)(interval->High);

  DOASSERT((long)lowId < _totalRecs && (long)highId < _totalRecs
	   && highId >= lowId, "Invalid record parameters");

  // check to see if current query can be used to answer request.
  if( _recIdAttrPosition < 0 && _nextToFetch <= lowId ) {

    //cerr << "not running a new query: " << _nextToFetch << ' ' << lowId << endl;
    // skip the starting of a new query

  } else {

    //cerr << "running a new query: " << _nextToFetch << ' ' << lowId << endl;

    //  Issue a query to the engine;
    string query(queryHeader);
    if( _recIdAttrPosition >= 0 ) { // query has recId field
      char whereClause[100];
      sprintf(whereClause, " where t.recId>=%ld and t.recId<=%ld",
              lowId, highId);
      query += whereClause;
      _nextToFetch = lowId;
    } else {
      _nextToFetch = 0;
    }
    
#if defined(DEBUG)
    cout << "Running: " << query << endl;
    static int entryCount;
    static double cumRecs;
    if(entryCount++ % 100 == 0){
      int percent = int(cumRecs / _totalRecs * 100);
      cerr << cumRecs << " records retreived (" << percent << "%) ";
      cerr << "dte called " << entryCount << " times" << endl;
    }
    cumRecs += interval->High - interval->Low + 1;
#endif

    delete engine;
    engine = new Engine(query);
    engine->optimize();
    CATCH(
          cout << "DTE error caused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          assert(0)
          );

    tuple = engine->getFirst();
    CATCH(
          cout << "DTE error caused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          assert(0);
          );
  }

  TDataRequest* req = new TDataRequest;
  req->nextVal = lowId;
  req->endVal = highId;
  req->relcb = callback;
  req->AttrName = interval->AttrName;
  req->granularity = interval->Granularity;

  // Do things similar to LimitRecords
  if (interval->NumRecs < highId - lowId +1) {
    req->endVal = lowId + (interval->NumRecs) - 1;
  }

  bytesleft = (int)(RecSize() * ( req->endVal - req->nextVal + 1));

  return req;
}

Boolean TDataDQL::GetRecs(TDHandle req, void *buf, int bufSize, 
			  Interval *interval, int &dataSize)
{
  DOASSERT(req, "Invalid request handle");
  DOASSERT(buf == Align(buf, MAX_ALIGN), "buffer not aligned");

  if (RID_STRING != req->AttrName) {
    cout << "TDataDQL: GetRecs deals with recId only right now.\n";
    assert(0);
  }

  interval->NumRecs = bufSize / _recSize;
  DOASSERT(interval->NumRecs, "Not enough record buffer space");

  if (req->nextVal > req->endVal)
    return false;
  
  int num = (int)(req->endVal) - (int)(req->nextVal) + 1;
  if (num < interval->NumRecs)
    interval->NumRecs = num;
  
  ReadRec((RecId)(req->nextVal), interval->NumRecs, buf);
  
  interval->Low = req->nextVal;
  dataSize = interval->NumRecs * _recSize;
  req->nextVal += interval->NumRecs;
  
  //_bytesFetched += dataSize;

  interval->High = interval->Low + interval->NumRecs - 1;
  interval->AttrName = req->AttrName;
  interval->Granularity = req->granularity;

  RecId HIGHId, LOWId;
  DOASSERT(HeadID(LOWId), "can not find HeadID");
  DOASSERT(LastID(HIGHId), "can not find LastID");
  if (LOWId < req->nextVal)
    {
      interval->has_left = true;
      interval->left_adjacent = interval->Low - 1;
    }
  else
    interval->has_left = false;

  if (HIGHId > interval->High)
    {
      interval->has_right = true;
      interval->right_adjacent = interval->High + 1;
    }
  else
    interval->has_right = false;


#if defined(DEBUG)
  static int entryCount;
  if(entryCount++ % 1000 == 0){
    cerr << " get recs called " << entryCount << "times\n";
  }
#endif

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
  return 1;
}

// char *TDataDQL::MakeIndexFileName(char *name, char *type)
// {
//   return 0;
// }

// void TDataDQL::Initialize()
// {
//  // Probably do all the things here..

// }

void TDataDQL::Checkpoint()
{
}

#if 0
void TDataDQL::InvalidateTData()
{
#if defined(DEBUG)
  cout << "Invalidating TDataDQL" << endl;
#endif
  runQuery();
  CATCH(
    cout << "DTE error caused by query: \n";
    cout << "   " << _query << endl;
    cout << currExcept->toString(); 
    currExcept = NULL; 
    cout << endl;
    assert(0);
    )
    TData::InvalidateTData();
}
#endif


/* Build index for the file. This code should work when file size
   is extended dynamically. Before calling this function, position
   should be at the last place where file was scanned. */

//  void TDataDQL::BuildIndex()
//  {
//    // Find the last record, total Recs,...
//    // _totalRecs = 2000;
//  }

TD_Status TDataDQL::ReadRec(RecId firstId, int numRecs, void *buf)
{
  int rid;
  for(rid = _nextToFetch ; rid < firstId ; rid++) {
    tuple = engine->getNext();
    assert(tuple && "premature end of query!");
  }
  char *ptr = (char *)buf;
  const DteTupleAdt& tupAdt = engine->getAdt();
  for(int i = 0 ; i < numRecs ; i++) {
    assert(tuple && "premature end of query!");
    for(int j = 0; j < _numFlds; j++) {
      const DteAdt& adt = tupAdt.getAdt(j);
      int size = _sizes[j];
      if( adt.getTypeID() == DteDateAdt::typeID ) {
        *(time_t*)ptr = DteDateAdt::toUnixTime(tuple[j]);
      } else {
        int rc = adt.copyNonNull(tuple[j], ptr, size);
        assert(rc >= 0);
      }
      ptr += size;
    }
    tuple = engine->getNext();
  }
  _nextToFetch = firstId + numRecs;
  return TD_OK;
}

char* TDataDQL::DispatchedName()
{
  return "TDataDQL";
}
