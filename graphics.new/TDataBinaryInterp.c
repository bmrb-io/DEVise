/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.5  1996/05/07 16:46:20  jussi
  This class now makes a copy of the attribute list so that attribute
  hi/lo values can be maintained per data stream, not per schema.
  Hi/lo values are now computed after composite parser is executed.

  Revision 1.4  1996/05/05 03:08:23  jussi
  Added support for composite attributes. Also added tape drive
  support.

  Revision 1.3  1996/04/16 20:38:52  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.2  1996/02/01 18:28:55  jussi
  Improved handling of case where data file has more attributes
  than schema defined.

  Revision 1.1  1996/01/23 20:54:51  jussi
  Initial revision.
*/

#include <string.h>
#include <unistd.h>

#include "TDataBinaryInterp.h"
#include "AttrList.h"
#include "RecInterp.h"
#include "CompositeParser.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"

TDataBinaryInterpClassInfo::TDataBinaryInterpClassInfo(char *className,
						       AttrList *attrList,
						       int recSize)
{
  _className = className;
  _attrList = attrList;
  _recSize = recSize;
  _tdata = NULL;

  // compute size of physical record (excluding attributes)

  _physRecSize = 0;
  _attrList->InitIterator();
  while(_attrList->More()) {
    AttrInfo *info = _attrList->Next();
    if (!info->isComposite)
      _physRecSize += info->length;
  }
  _attrList->DoneIterator();

  DOASSERT(_physRecSize > 0 && _physRecSize <= _recSize,
	   "Invalid physical record size");
}

TDataBinaryInterpClassInfo::TDataBinaryInterpClassInfo(char *className,
						       char *name,
						       char *alias,
						       TData *tdata)
{
  _className = className;
  _name = name;
  _alias = alias;
  _tdata = tdata;
}

TDataBinaryInterpClassInfo::~TDataBinaryInterpClassInfo()
{
  if (_tdata)
    delete _tdata;
}

char *TDataBinaryInterpClassInfo::ClassName()
{
  return _className;
}

static char buf1[256], buf2[256];
static char *args[2];

void TDataBinaryInterpClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = buf1;
  args[1] = buf2;
  
  strcpy(buf1, "File {foobar}");
  strcpy(buf2, "Alias {foobar}");
}

ClassInfo *TDataBinaryInterpClassInfo::CreateWithParams(int argc, char **argv)
{
  DOASSERT(argc == 2, "Invalid parameters");

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);
  TDataBinaryInterp *tdata = new TDataBinaryInterp(name, alias, _recSize,
						   _physRecSize, _attrList);
  return new TDataBinaryInterpClassInfo(_className, name, alias, tdata);
}

char *TDataBinaryInterpClassInfo::InstanceName()
{
  return _alias;
}

void *TDataBinaryInterpClassInfo::GetInstance()
{
  return _tdata;
}

void TDataBinaryInterpClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = _name;
  args[1] = _alias;
}

TDataBinaryInterp::TDataBinaryInterp(char *name, char *alias, int recSize,
				     int physRecSize, AttrList *attrs) :
     TDataBinary(name, alias, recSize, physRecSize),
     _attrList(*attrs)
{
#ifdef DEBUG
  printf("TDataBinaryInterp %s, recSize %d, physRecSize %d\n",
	 name, recSize, physRecSize);
#endif

  _recInterp = new RecInterp();
  _recInterp->SetAttrs(attrs);
  
  _name = name;
  _recSize = recSize;
  _physRecSize = physRecSize;
  _numAttrs = _numPhysAttrs = _attrList.NumAttrs();
  
  hasComposite = false;

  _attrList.InitIterator();
  while(_attrList.More()) {
    AttrInfo *info = _attrList.Next();
    if (info->isComposite) {
      hasComposite = true;
      _numPhysAttrs--;
    }
  }
  _attrList.DoneIterator();

  Initialize();
}

TDataBinaryInterp::~TDataBinaryInterp()
{
}

Boolean TDataBinaryInterp::WriteCache(int fd)
{
  int numAttrs = _attrList.NumAttrs();
  if (write(fd, &numAttrs, sizeof numAttrs) != sizeof numAttrs) {
    perror("write");
    return false;
  }

  for(int i = 0; i < _attrList.NumAttrs(); i++) {
    AttrInfo *info = _attrList.Get(i);
    if (info->type == StringAttr)
      continue;
    if (write(fd, &info->hasHiVal, sizeof info->hasHiVal)
	!= sizeof info->hasHiVal) {
      perror("write");
      return false;
    }
    if (write(fd, &info->hiVal, sizeof info->hiVal) != sizeof info->hiVal) {
      perror("write");
      return false;
    }
    if (write(fd, &info->hasLoVal, sizeof info->hasLoVal)
	!= sizeof info->hasLoVal) {
      perror("write");
      return false;
    }
    if (write(fd, &info->loVal, sizeof info->loVal) != sizeof info->loVal) {
      perror("write");
      return false;
    }
  }

  return true;
}

Boolean TDataBinaryInterp::ReadCache(int fd)
{
  int numAttrs;
  if (read(fd, &numAttrs, sizeof numAttrs) != sizeof numAttrs) {
    perror("read");
    return false;
  }
  if (numAttrs != _attrList.NumAttrs()) {
    printf("Cache has inconsistent schema; rebuilding\n");
    return false;
  }

  for(int i = 0; i < _attrList.NumAttrs(); i++) {
    AttrInfo *info = _attrList.Get(i);
    if (info->type == StringAttr)
      continue;
    if (read(fd, &info->hasHiVal, sizeof info->hasHiVal)
	!= sizeof info->hasHiVal) {
      perror("read");
      return false;
    }
    if (read(fd, &info->hiVal, sizeof info->hiVal) != sizeof info->hiVal) {
      perror("read");
      return false;
    }
    if (read(fd, &info->hasLoVal, sizeof info->hasLoVal)
	!= sizeof info->hasLoVal) {
      perror("read");
      return false;
    }
    if (read(fd, &info->loVal, sizeof info->loVal) != sizeof info->loVal) {
      perror("read");
      return false;
    }
  }

  return true;
}

Boolean TDataBinaryInterp::Decode(void *recordBuf, int recPos, char *line)
{
  /* set buffer for interpreted record */
  _recInterp->SetBuf(recordBuf);
  _recInterp->SetRecPos(recPos);

  if (recordBuf != line)
    memcpy(recordBuf, line, _physRecSize);

  /* decode composite attributes */
  if (hasComposite)
    CompositeParser::Decode(_attrList.GetName(), _recInterp);

  for(int i = 0; i < _numAttrs; i++) {
    AttrInfo *info = _attrList.Get(i);

    char *ptr = (char *)recordBuf + info->offset;
    int intVal;
    float floatVal;
    double doubleVal;
    time_t dateVal;

    switch(info->type) {
    case IntAttr:
      intVal = *(int *)ptr;
      if (info->hasMatchVal && intVal != info->matchVal.intVal)
	return false;
      if (!info->hasHiVal || intVal > info->hiVal.intVal) {
	info->hiVal.intVal = intVal;
	info->hasHiVal = true;
      }
      if (!info->hasLoVal || intVal < info->loVal.intVal) {
	info->loVal.intVal = intVal;
	info->hasLoVal = true;
      }
#ifdef DEBUG
      printf("int %d, hi %d, lo %d\n", intVal, info->hiVal.intVal,
	     info->loVal.intVal);
#endif
      break;

    case FloatAttr:
      floatVal = *(float *)ptr;
      if (info->hasMatchVal && floatVal != info->matchVal.floatVal)
	return false;
      if (!info->hasHiVal || floatVal > info->hiVal.floatVal) {
	info->hiVal.floatVal = floatVal;
	info->hasHiVal = true;
      }
      if (!info->hasLoVal || floatVal < info->loVal.floatVal) {
	info->loVal.floatVal = floatVal;
	info->hasLoVal = true;
      }
#ifdef DEBUG
      printf("float %.2f, hi %.2f, lo %.2f\n", floatVal,
	     info->hiVal.floatVal, info->loVal.floatVal);
#endif
      break;

    case DoubleAttr:
      doubleVal = *(double *)ptr;
      if (info->hasMatchVal && doubleVal != info->matchVal.doubleVal)
	return false;
      if (!info->hasHiVal || doubleVal > info->hiVal.doubleVal) {
	info->hiVal.doubleVal = doubleVal;
	info->hasHiVal = true;
      }
      if (!info->hasLoVal || doubleVal < info->loVal.doubleVal) {
	info->loVal.doubleVal = doubleVal;
	info->hasLoVal = true;
      }
#ifdef DEBUG
      printf("double %.2f, hi %.2f, lo %.2f\n", doubleVal,
	     info->hiVal.doubleVal, info->loVal.doubleVal);
#endif
      break;

    case StringAttr:
      if (info->hasMatchVal && strcmp(ptr, info->matchVal.strVal))
	return false;
      break;

    case DateAttr:
      dateVal = *(time_t *)ptr;
      if (info->hasMatchVal && dateVal != info->matchVal.dateVal)
	return false;
      if (!info->hasHiVal || dateVal > info->hiVal.dateVal) {
	info->hiVal.dateVal = dateVal;
	info->hasHiVal = true;
      }
      if (!info->hasLoVal || dateVal < info->loVal.dateVal) {
	info->loVal.dateVal = dateVal;
	info->hasLoVal = true;
      }
#ifdef DEBUG
      printf("date %ld, hi %ld, lo %ld\n", dateVal, info->hiVal.dateVal,
	     info->loVal.dateVal);
#endif
      break;

    default:
      DOASSERT(0, "Unknown attribute type");
    }
  }
  
  return true;
}
