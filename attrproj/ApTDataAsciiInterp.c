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
  Revision 1.1  1996/04/25 19:25:15  wenger
  Attribute projection code can now parse a schema, and create the
  corresponding TData object.

*/

#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "ApTDataAsciiInterp.h"
#include "AttrList.h"
#include "RecInterp.h"
#include "CompositeParser.h"
#include "Parse.h"
#include "Util.h"

//#define DEBUG

#if 0
TDataAsciiInterpClassInfo::TDataAsciiInterpClassInfo(char *className,
						     AttrList *attrList,
						     int recSize,
						     char *separators,
						     int numSeparators,
						     Boolean isSeparator,
						     char *commentString)
{
/* Note that this only saves a pointer to the attrList; it doesn't copy it. */
  _className = className;
  _attrList = attrList;
  _separators = separators;
  _numSeparators = numSeparators;
  _commentString = commentString;
  _recSize = recSize;
  _isSeparator = isSeparator;
  _tdata = NULL;
}

TDataAsciiInterpClassInfo::TDataAsciiInterpClassInfo(char *className,
						     char *name,
						     char *alias,
						     TData *tdata)
{
  _className = className;
  _name = name;
  _alias = alias;
  _tdata = tdata;
}

TDataAsciiInterpClassInfo::~TDataAsciiInterpClassInfo()
{
  if (_tdata)
    delete _tdata;
}

char *TDataAsciiInterpClassInfo::ClassName()
{
  return _className;
}

static char buf1[256], buf2[256];
static char *args[2];

void TDataAsciiInterpClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = buf1;
  args[1] = buf2;
  
  sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
  sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *TDataAsciiInterpClassInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 2)
    return (ClassInfo *)NULL;

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);
  TDataAsciiInterp *tdata = new TDataAsciiInterp(name,_recSize, _attrList,
						 _separators, _numSeparators,
						 _isSeparator, _commentString);
  return new TDataAsciiInterpClassInfo(_className, name, alias, tdata);
}

char *TDataAsciiInterpClassInfo::InstanceName()
{
  return _alias;
}

void *TDataAsciiInterpClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */
void TDataAsciiInterpClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = _name;
  args[1] = _alias;
}
#endif

TDataAsciiInterp::TDataAsciiInterp(char *name, int recSize, AttrList *attrs,
				   char *separators, int numSeparators,
				   Boolean isSeparator, char *commentString) :
	TDataAscii(name, recSize)
{
#ifdef DEBUG
  printf("TDataAsciiInterp %s, recSize %d\n", name, recSize);
#endif

  _recInterp = new RecInterp();
  _recInterp->SetAttrs(attrs);
  
  _name = name;
  _recSize = recSize;
  _attrList = attrs;
  _separators = separators;
  _numSeparators = numSeparators;
  _isSeparator = isSeparator;

  _commentString = commentString;
  if (_commentString)
    _commentStringLength = strlen(_commentString);
  _numAttrs = _attrList->NumAttrs();
  
  _numMatchingAttrs = 0;
  hasComposite = false;

  for(int i = 0; i < _attrList->NumAttrs(); i++) {
    AttrInfo *info = _attrList->Get(i);
    if (info->isComposite) {
      hasComposite = true;
      _numAttrs--;
    }
    if (info->hasMatchVal) {
      DOASSERT(_numMatchingAttrs < MAX_MATCHING_ATTRS,
	       "Too many matching attributes");
      _matchingAttrs[_numMatchingAttrs++] = info;
    }
  }
  
  Initialize();
}

TDataAsciiInterp::~TDataAsciiInterp()
{
}

Boolean TDataAsciiInterp::WriteCache(int fd)
{
  int numAttrs = _attrList->NumAttrs();
  if (write(fd, &numAttrs, sizeof numAttrs) != sizeof numAttrs) {
    perror("write");
    return false;
  }

  for(int i = 0; i < _attrList->NumAttrs(); i++) {
    AttrInfo *info = _attrList->Get(i);
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

Boolean TDataAsciiInterp::ReadCache(int fd)
{
  int numAttrs;
  if (read(fd, &numAttrs, sizeof numAttrs) != sizeof numAttrs) {
    perror("read");
    return false;
  }
  if (numAttrs != _attrList->NumAttrs()) {
    printf("Cache has inconsistent schema; rebuilding\n");
    return false;
  }

  for(int i = 0; i < _attrList->NumAttrs(); i++) {
    AttrInfo *info = _attrList->Get(i);
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

Boolean TDataAsciiInterp::Decode(void *recordBuf, char *line)
{
  /* set buffer for interpreted record */
  _recInterp->SetBuf(recordBuf);
  
  int numArgs;
  char **args;

  Parse(line, numArgs, args, _separators, _numSeparators, _isSeparator);
  
  if (numArgs < _numAttrs || 
      (_commentString != NULL &&
       strncmp(args[0], _commentString, _commentStringLength) == 0)) {
#ifdef DEBUG
    printf("Too few arguments (%d < %d) or commented line\n",
	   numArgs, _numAttrs);
#endif
    return false;
  }
	
  int i;
  for(i = 0; i < _numAttrs; i++) {
    AttrInfo *info = _attrList->Get(i);
    if (info->type == IntAttr || info->type == DateAttr) {
      if (!isdigit(args[i][0]) && args[i][0] != '-' && args[i][0] != '+') {
#ifdef DEBUG
	printf("Invalid integer/date value: %s\n", args[i]);
#endif
	return false;
      }
    } else if (info->type == FloatAttr || info->type == DoubleAttr) {
      if (!isdigit(args[i][0]) && args[i][0] != '.'
	  && args[i][0] != '-' && args[i][0] != '+') {
#ifdef DEBUG
	printf("Invalid float/double value: %s\n", args[i]);
#endif
	return false;
      }
    }
  }

  DOASSERT(numArgs >= _numAttrs, "Invalid number of arguments");

  for(i = 0; i < _numAttrs; i++) {

    AttrInfo *info = _attrList->Get(i);
    char *ptr = (char *)recordBuf + info->offset;
    int intVal;
    float floatVal;
    double doubleVal;
    time_t dateVal;

    switch(info->type) {
    case IntAttr:
      intVal = atoi(args[i]);
      if (info->hasMatchVal && intVal != info->matchVal.intVal)
	return false;
      *(int *)ptr = intVal;
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
      floatVal = UtilAtof(args[i]);
      if (info->hasMatchVal && floatVal != info->matchVal.floatVal)
	return false;
      *(float *)ptr = floatVal;
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
      doubleVal = UtilAtof(args[i]);
      if (info->hasMatchVal && doubleVal != info->matchVal.doubleVal)
	return false;
      *(double *)ptr = doubleVal;
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
      if (info->hasMatchVal && strcmp(args[i], info->matchVal.strVal))
	return false;
      strncpy(ptr, args[i], info->length);
      ptr[info->length - 1] = '\0';
#ifdef DEBUG
      printf("string '%s'\n", ptr);
#endif
      break;

    case DateAttr:
      DOASSERT(!info->hasMatchVal, "Date attribute match not implemented");
      dateVal = atoi(args[i]);
      *(time_t *)ptr = dateVal;
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
  
  /* decode composite attributes */
  if (hasComposite)
    CompositeParser::Decode(_attrList->GetName(), _recInterp);

  return true;
}
