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
#include <strstream.h>
#include "machdep.h"
#include <ctype.h>
#include <unistd.h>
#include "DataSeg.h"
#include <string>
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

#include "types.h"
#include "exception.h"
#include "queue.h"

// #define DEBUG

extern InsertCatFile(char* tableName);  // defined in graphics.new/ParseCat.c

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(
	char* tableName, const char* query) : _attrs(tableName)
{

// this constructor does nothing

	_tdata = NULL;
	_name = NULL;
	_tableName = strdup(tableName);
	_type = strdup("DQL");
	_className = strdup(tableName); 
	_counter = 0;
}

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(char * className, AttrList attrs,char * name,char * type,char *query, 
TData *tdata, char* tableName): _attrs(attrs)
{

  _className = strdup(className);
  _name = strdup(name);
  _type = strdup(type);
  _query = strdup(query);
  _tdata = tdata;
  _tableName = strdup(tableName);
  _counter = 0;
}

TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo()
{
#if defined(DEBUG)
	cout << "IN TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo" << endl;
#endif
  if (_tdata)
    delete _tdata;
  if (_className) free (_className);
  if (_query) free (_query);
  if (_name)  free(_name);
#if defined(DEBUG)
	cout << "OUT TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo" << endl;
#endif
}

char *TDataDQLInterpClassInfo::ClassName()
{
  return _className;
}

static char buf[2][256];
static char *args[2];

void TDataDQLInterpClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = buf[0];
  args[1] = buf[1];
  
  strcpy(buf[0], "Name {foobar}");
  strcpy(buf[1], "Type {foobar}");
}

ClassInfo *TDataDQLInterpClassInfo::CreateWithParams(int argc, char **argv)
{

#if defined(DEBUG) || 0
	cout << "In TDataDQLInterpClassInfo::CreateWithParams(";
	cout << argc << ", {";
	for(int i = 0; i < argc; i++){
		cout << argv[i] << ", ";
	}
	cout << "})" << endl;
#endif

	if(argc < 2){
		cout << "Problem in TDataDQLInterp.c, at leas 2 fields expected" 
			<< endl;
		for(int i = 0; i < argc; i++){
			cout << argv[i] << ", ";
		}
		cout << endl;
		exit(0);
	}
	_tableName = strdup(argv[0]);
	char* attrs = strdup(argv[1]);
	_type = strdup(argv[1]);

	// List<char*>* attrList = new List<char*>;
	List<char*>* attrList = NULL;

	/*
	for(char* currAtt = strtok(attrs, " "); currAtt;
					currAtt = strtok(NULL, " ")){
		attrList->append(strdup(currAtt));
	}
	*/

	string query = "select *";
	/*
	attrList->rewind();
	while(!attrList->atEnd()){
		query += string("t.") + attrList->get(); 
		attrList->step();
		if(attrList->atEnd()){
			break;
		}
		query += ", ";
	}
	*/
	query += string(" from ") + _tableName + " as t";
	_query = strdup(query.c_str());

  #ifdef DEBUG
	  printf(" Query = %s \n",_query);
  #endif


	ostrstream name;
//	if(_counter){
		name << argv[0];
//	}
//	else{
//		name << argv[0] << ends;
//	}
	_counter++;	// this is used to create unique names;
	name << ends;
	_name = name.str();

  DataSeg::Set(_name, _query, 0, 0);

  TDataDQLInterp *tdata = new TDataDQLInterp(_name, attrList, _query);

//  bool tdataFailed = tdata->fail();

  _tdata = NULL;	// cannot put _tdata = tdata (core dump?)

  return new TDataDQLInterpClassInfo(
	_className,_attrs, _name, _type, _query, tdata, _tableName);
}

ClassInfo *TDataDQLInterpClassInfo::CreateWithParamsNew(char **argv, char* queryS)
{

#if defined(DEBUG) || 0
	cout << "In TDataDQLInterpClassInfo::CreateWithParamsNew(";
	cout << "{";
	for(int i = 0; i < 2; i++){
		cout << argv[i] << ", ";
	}
	cout << queryS << "})" << endl;
#endif

	_tableName = strdup(argv[0]);
	char* attrs = argv[1];
	_type = strdup(argv[1]);
	List<char*>* attrList = new List<char*>;
	for(char* currAtt = strtok(attrs, " "); currAtt;
					currAtt = strtok(NULL, " ")){
		attrList->append(currAtt);
	}
	_query = strdup(queryS);

  #ifdef DEBUG
	  printf(" Query = %s \n",_query);
  #endif


//	ostrstream name;
//	if(_counter){
//		name << argv[0];
//	}
//	else{
//		name << argv[0] << ends;
//	}
//	_counter++;	// this is used to create unique names;
//	name << _counter;
//	_counter++;
//	name << ends;
//	_name = name.str();

	_name = strdup(argv[0]);

  DataSeg::Set(_name, _query, 0, 0);

  TDataDQLInterp *tdata = new TDataDQLInterp(_name,  attrList, _query);

  _tdata = NULL;	// cannot put _tdata = tdata (core dump?)

  return new TDataDQLInterpClassInfo(
	_className,_attrs, _name, _type, _query, tdata, _tableName);
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
  argc = 2;
  argv = args;
#if defined(DEBUG)
  cout << "To recreate this table args are: " << _tableName << " " << _type << endl;
#endif
  args[0] = _tableName;
  args[1] = _type;
}

TDataDQLInterp::TDataDQLInterp(
	AttrList attrs,char *name, char *type, 
	int numFlds, string* types, int recSize, long totalRecs, 
	int* sizes) : 
	TDataDQL(attrs, name, type, numFlds, types, recSize, totalRecs, sizes)
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
