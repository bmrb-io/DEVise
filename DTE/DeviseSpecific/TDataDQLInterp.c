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
  Revision 1.2  1996/12/16 07:21:26  donjerko
  Fixed some compile problems

  Revision 1.1  1996/12/15 06:32:01  donjerko
  Added the DeviseSpecific directory to DTE

  Revision 1.10  1996/12/02 16:54:31  wenger
  Fixed compile warning; added standard headers to some files;
  conditionaled out debug code.

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

#define DEBUG

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(
	char * className, char *schemaFile, char *fileType,char *dataFile,
	char *query): _attrs(className)
{

/* Note that this only saves a pointer to the attrList; it doesn't copy it. */

  _className = strdup(className); 

  String phySchemaFile;

  if (strcmp(fileType,"DQL")){
	
	// Extract the attributes from the schema..
	String list = "";

	if (ParseCatDQL(schemaFile,phySchemaFile,list) == false)
		return;

	String tmp1 = "select "+ list + " from \"DeviseTable ";
	tmp1 = tmp1 + phySchemaFile + " " + dataFile + "\" where ";
	tmp1 = tmp1 + query + ";";

	_query = strdup(tmp1.chars());
	_schemaFile = (char *)phySchemaFile.chars();
  }
  else{
	_query = strdup(query);
  	_schemaFile = strdup(schemaFile);
  }
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
	String* attrs = engine.getAttributeNames();
     _types = engine.getTypeIDs();
     Tuple* tup;

	engine.initialize();
     while((tup = engine.getNext())){
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
	for(int i = 0; i < _numFlds; i++){
		String res[20];
		int numSplit = split(attrs[i],res,20,String(","));
				
		char* atname = strdup(res[0].chars());
		int deviseSize = packSize(_result[0][i], _types[i]);
		_sizes[i] = deviseSize;
		if(numSplit > 1){
			String temp = atname;
			delete atname;
			temp += ")";
			atname = strdup(temp.chars());
		}

		AttrType deviseType = getDeviseType(_types[i]);
		_attrs.InsertAttr(i, atname, offset, deviseSize, 
			deviseType, false, 0, false, false, false, 0, false, 0); 
		offset += deviseSize;
	}

	_recSize = offset;

  _tdata = NULL;
  _type = NULL;
  _name = NULL;
}

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(char * className, char *schemaFile, AttrList attrs,char * name,char * type,char *query, 
TData *tdata): _attrs(attrs)
{

#ifdef DEBUG
  printf(" Calling TData class -- ClassName = %s,schemaFile = %s,Name = %s,
	type = %s,query = %s \n\n",className,schemaFile,name,type,query);
#endif

  _className = strdup(className);
  _schemaFile = strdup(schemaFile);
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
  if (argc != 2 && argc != 3)
    return (ClassInfo *)NULL;

  char *name = NULL, *type = NULL, *query = NULL;

  if (argc == 2) {
    name = CopyString(argv[1]);
    type = CopyString("DQL");
    query = CopyString(argv[0]);
  } else {
    name = CopyString(argv[0]);
    type = CopyString("DQL");
	// 2 will give the actual type... e.g. UNIXFILE..
  }
	
	type = strdup("DQL");
	name = strdup(argv[0]);

#ifdef DEBUG
	cout << "TDataDQInterpClassInfo::CreateWithParams: \n";
	cout << "  _query: " << _query << endl;
	cout << "  name: " << name << endl;
	cout << "  type: " << type << endl;
#endif
  DataSeg::Set(name, _query, 0, 0);

  TDataDQLInterp *tdata = new TDataDQLInterp(
	_attrs, name, type, _numFlds, _types, _recSize, _result, _sizes);
          

  return new TDataDQLInterpClassInfo(
	_className,_schemaFile,_attrs, name, type, _query, tdata);
}

char *TDataDQLInterpClassInfo::InstanceName()
{
  return _name;
}

void *TDataDQLInterpClassInfo::GetInstance()
{
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

void dqlCreateIndex(char *query)
{
	
	cout << "strlen = " << strlen(query);
	cout << "qurery = " << query << endl;
	String *queryStr = new String(query);
     Engine engine(*queryStr);
     engine.optimize();
	CATCH(
		cout << "DTE error coused by query: \n";
		cout << "   " << *queryStr << endl;
		currExcept->display(); 
		currExcept = NULL; 
		cout << endl;
		exit(0);
	)
}








