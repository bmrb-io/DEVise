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
	List<char*>* attrList = new List<char*>;
	for(char* currAtt = strtok(attrs, " "); currAtt;
					currAtt = strtok(NULL, " ")){
		attrList->append(currAtt);
	}
	String query = "select ";
	attrList->rewind();
	while(!attrList->atEnd()){
		query += String("t.") + attrList->get(); 
		attrList->step();
		if(attrList->atEnd()){
			break;
		}
		query += ", ";
	}
	query += String(" from ") + tableName + " as t";
	_query = strdup(query.chars());

  #ifdef DEBUG
	  printf(" Query = %s \n",_query);
  #endif


  char *name = NULL, *type = NULL;

	type = strdup("DQL");
	_type = strdup(type);
	name = strdup(argv[0]);
	_name = strdup(name);

  DataSeg::Set(name, _query, 0, 0);

  TDataDQLInterp *tdata = new TDataDQLInterp(name, attrList, _query);

//  TDataDQLInterp *tdata = new TDataDQLInterp(
//	_attrs, name, type, _numFlds, _types, _recSize, _result, _sizes);
          
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
	TDataDQL(attrs, name, type, numFlds, types, recSize, result, sizes)
{
  
 // Need to form a attribute list and pass it back..

  Initialize();
}

TDataDQLInterp::TDataDQLInterp(char* tableName, List<char*>* attrList, char* query) : 
	TDataDQL(tableName, attrList, query) {
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
