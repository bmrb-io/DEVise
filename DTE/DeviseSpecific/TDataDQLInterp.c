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
#include "TDataDQL.h"

#include "types.h"
#include "exception.h"
//#include "queue.h"

// #define DEBUG

extern void InsertCatFile(char* tableName);  // defined in graphics.new/ParseCat.c

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(const char* tableName)
  : _tableName(tableName), _tdata(NULL)
{
}

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(const char* tableName,
                                                 TData* tdata)
  : _tableName(tableName), _tdata(tdata)
{
}

TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo()
{
#if defined(DEBUG)
  cout << "IN TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo" << endl;
#endif
  delete _tdata;
#if defined(DEBUG)
  cout << "OUT TDataDQLInterpClassInfo::~TDataDQLInterpClassInfo" << endl;
#endif
}

char *TDataDQLInterpClassInfo::ClassName()
{
  return (char *)_tableName.c_str();
}

void TDataDQLInterpClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 0;
  argv = NULL;
}

ClassInfo *TDataDQLInterpClassInfo::CreateWithParams(int argc, char **argv)
{

#if defined(DEBUG)
  cout << "In TDataDQLInterpClassInfo::CreateWithParams(";
  cout << argc << ", {";
  for(int i = 0; i < argc; i++){
    cout << argv[i] << ", ";
  }
  cout << "})" << endl;
#endif

  string query("select * from ");
  query += _tableName;
  query += " as t";

#ifdef DEBUG
  printf(" Query = %s \n",_query);
#endif

  //_name = argv[0];

  //DataSeg::Set(_name, _query, 0, 0); //kb: why?

  TDataDQL* tdata = new TDataDQL(_tableName);
  if (currExcept) {
    CATCH(cerr << currExcept->toString());
    delete tdata;
    return NULL;
  }

  return new TDataDQLInterpClassInfo(_tableName.c_str(), tdata);
}

ClassInfo *TDataDQLInterpClassInfo::CreateWithParamsNew(char **argv, char* queryS)
{
  //kb: is CreateWithParamsNew() needed?
  return CreateWithParams(0, NULL);
#if 0
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

	//DataSeg::Set(_name, _query, 0, 0);

  TDataDQL* tdata = new TDataDQL(_name,  attrList, _query);

  return new TDataDQLInterpClassInfo(_tableName, tdata);
#endif
}

char *TDataDQLInterpClassInfo::InstanceName()
{
  return (char *)_tableName.c_str();
}

void *TDataDQLInterpClassInfo::GetInstance()
{
  assert(_tdata);
  return _tdata;
}

AttrList* TDataDQLInterpClassInfo::GetAttrList()
{
  assert(_tdata);
  return _tdata->GetAttrList();
}	


/* Get parameters that can be used to re-create this instance */
void TDataDQLInterpClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 0;
  argv = NULL;
}

#if 0
// TDataDQLInterp::TDataDQLInterp(
// 	AttrList attrs,char *name, char *type, 
// 	int numFlds, string* types, int recSize, long totalRecs, 
// 	int* sizes) : 
// 	TDataDQL(attrs, name, type, numFlds, types, recSize, totalRecs, sizes)
// {
  
//  // Need to form a attribute list and pass it back..

//   Initialize();
// }

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
#endif
