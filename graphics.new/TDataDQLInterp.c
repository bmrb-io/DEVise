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
#include <ctype.h>
#include <unistd.h>
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
#ifndef ATTRPROJ
#  include "StringStorage.h"
#endif

//#define DEBUG

#ifndef ATTRPROJ

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(
	char * className, char *schemaFile, char *fileType,char *dataFile,
	char *query): _attrs(className)
{

/* Note that this only saves a pointer to the attrList; it doesn't copy it. */

  _className = strdup(className); 
  _schemaFile = strdup(schemaFile);

  if (strcmp(fileType,"DQL")){
	
	// Extract the attributes from the schema..
	String list = "";

	if (ParseCatDQL(schemaFile,list) == false)
		return;

	String tmp1 = "select "+ list + " from \"DeviseTable ";
	tmp1 = tmp1 + schemaFile + " " + dataFile + "\" where ";
	tmp1 = tmp1 + query + ";";

	_query = strdup(tmp1.chars());

  }
  else
	_query = strdup(query);
 
  printf(" DQL::Query = %s \n",_query);

  _tdata = NULL ;
  _type = NULL;
  _name = NULL;
 _attrs.InsertAttr(0,"ID1",0,sizeof(int),IntAttr,false,0,false,true,
 false,0,false,0);
 _attrs.InsertAttr(1,"ID2",4,sizeof(int),IntAttr,false,0,false,false,
 false,0,false,0);
}

TDataDQLInterpClassInfo::TDataDQLInterpClassInfo(char * className, char *schemaFile, AttrList attrs,char * name,char * type,char *query, 
TData *tdata): _attrs(attrs)
{
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

#if 1
  char *name, *type, *query;

  if (argc == 2) {
    name = CopyString(argv[1]);
    type = CopyString("DQL");
    query = CopyString(argv[0]);
  } else {
    name = CopyString(argv[0]);
    type = CopyString("DQL");
	// 2 will give the actual type... e.g. UNIXFILE..
  }
#endif
  TDataDQLInterp *tdata = new TDataDQLInterp(_attrs,name,type,_query);
                                                 
  return new TDataDQLInterpClassInfo(_className,_schemaFile,_attrs,name,type,_query,tdata);
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
#endif

TDataDQLInterp::TDataDQLInterp(AttrList attrs,char *name, char *type,char *query): TDataDQL(name, type, query ),_attrList(attrs)
{
#ifdef DEBUG
  printf("TDataDQLInterp %s, recSize %d\n", name, recSize);
#endif
  
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
