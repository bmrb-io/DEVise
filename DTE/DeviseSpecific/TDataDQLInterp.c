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


#include <string.h>
#include "machdep.h"
#include <ctype.h>
#include <unistd.h>
#include "DataSeg.h"
#include <String.h>
#include "TDataDQLInterp.h"
#include "AttrList.h"
#include "RecInterp.h"
#include "CompositeParser.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"
#include "DevError.h"
#include "ParseCat.h"
#include "DevError.h"
// #include "StringStorage.h"

#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "TuplePtr.XPlex.h"
#include "queue.h"

// #define DEBUG

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(
	char* tableName, const char* query) : _attrs(tableName)
{

/* Note that this only saves a pointer to the attrList; it doesn't copy it. */

	_tdata = NULL;
	_name = NULL;
	_type = strdup("DQL");
	_className = strdup(tableName); 
}

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(char * className, AttrList attrs,char * name,char * type,char *query, 
TData *tdata): _attrs(attrs)
{

  _className = strdup(className);
  _name = strdup(name);
  _type = strdup(type);
  _query = strdup(query);
  _tdata = tdata;
}

TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo()
{
  if (_tdata)
    delete _tdata;
  if (_className) free (_className);
  if (_query) free (_query);
  if (_name)  free(_name);
}

char *TDataDQLInterpClassInfo::ClassName()
{
  return _className;
}

static char buf[3][256];
static char *args[3];

void TDataDQLInterpClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 3;
  argv = args;
  args[0] = buf[0];
  args[1] = buf[1];
  args[2] = buf[2];
  
  strcpy(buf[0], "Name {foobar}");
  strcpy(buf[1], "Type {foobar}");
  strcpy(buf[2], "Param {foobar}");
}

ClassInfo *TDataDQLInterpClassInfo::CreateWithParams(int argc, char **argv)
{

#ifdef DEBUG
	cout << "In TDataDQLInterpClassInfo::CreateWithParams(";
	cout << argc << ", {";
	for(int i = 0; i < argc; i++){
		cout << argv[i] << ", ";
	}
	cout << "})" << endl;
#endif

	assert(argc == 2);
	char* tableName = argv[0];
	char* attrs = argv[1];
	List<char*> attrList;
	for(char* currAtt = strtok(attrs, " "); currAtt;
					currAtt = strtok(NULL, " ")){
		attrList.append(currAtt);
	}
	String query = "select ";
	attrList.rewind();
	while(!attrList.atEnd()){
		query += String("t.") + attrList.get(); 
		attrList.step();
		if(attrList.atEnd()){
			break;
		}
		query += ", ";
	}
	query += String(" from ") + tableName + " as t";
	_query = strdup(query.chars());

  #ifdef DEBUG
	  printf(" Query = %s \n",_query);
  #endif


     Engine engine(_query);
     engine.optimize();
	CATCH(
		cout << "DTE error coused by query: \n";
		cout << "   " << _query << endl;
		currExcept->display(); 
		currExcept = NULL; 
		cout << endl;
		exit(0);
	)
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
          TRY(tmp = getOperatorPtr("<",_types[i],_types[i],retVal), NULL);
		assert(tmp);
		lessPtrs[i] = tmp->opPtr;
          TRY(tmp = getOperatorPtr(">",_types[i],_types[i],retVal), NULL);
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
	attrList.rewind();
	for(int i = 0; i < _numFlds; i++){
		assert(!attrList.atEnd());
		char* atname = attrList.get();
		attrList.step();
		int deviseSize = packSize(_result[0][i], _types[i]);
		_sizes[i] = deviseSize;
		AttrType deviseType = getDeviseType(_types[i]);
		AttrVal* hiVal = (AttrVal*) highTup[i];
		AttrVal* loVal = (AttrVal*) lowTup[i];
		_attrs.InsertAttr(i, strdup(atname), offset, deviseSize, 
			deviseType, false, 0, false, false, true, hiVal, true, loVal); 
		offset += deviseSize;
	}

	_recSize = offset;

  char *name = NULL, *type = NULL;

	type = strdup("DQL");
	_type = strdup(type);
	name = strdup(argv[0]);
	_name = strdup(name);

  DataSeg::Set(name, _query, 0, 0);

  TDataDQLInterp *tdata = new TDataDQLInterp(
	_attrs, name, type, _numFlds, _types, _recSize, _result, _sizes);
          
  _tdata = NULL;	// cannot put _tdata = tdata (core dump?)

  return new TDataDQLInterpClassInfo(
	_className,_attrs, name, type, _query, tdata);
}

char *TDataDQLInterpClassInfo::InstanceName()
{
	assert(_name);
  return _name;
}

void *TDataDQLInterpClassInfo::GetInstance()
{
	assert(_tdata);
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */
void TDataDQLInterpClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 3;
  argv = args;
  args[0] = _name;
  args[1] = _type;
  args[2] = _query;
}

TDataDQLInterp::TDataDQLInterp(
	AttrList attrs,char *name, char *type, 
	int numFlds, String* types, int recSize, TuplePtrXPlex& result, 
	int* sizes) : 
	TDataDQL(attrs, name, type, numFlds, types, recSize, result, sizes),
	_attrList(attrs)
{
  
 // Need to form a attribute list and pass it back..

  Initialize();
}

TDataDQLInterp::~TDataDQLInterp()
{
}

void TDataDQLInterp::InvalidateIndex()
{
}

Boolean TDataDQLInterp::WriteIndex(int fd)
{
  return true;
}

Boolean TDataDQLInterp::ReadIndex(int fd)
{
  return true;
}

Boolean TDataDQLInterp::Decode(void *recordBuf, int recPos, char *line)
{
  /* set buffer for interpreted record */
  return true;
}
