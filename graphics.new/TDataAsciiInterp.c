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
  Revision 1.5  1995/11/22 17:51:01  jussi
  Added missing DoubleAttr case in Decode().

  Revision 1.4  1995/11/22 17:04:14  jussi
  Added IsValid() method and cleaned up the code.

  Revision 1.3  1995/11/21 23:20:49  jussi
  Added copyright notice and cleaned up code.

  Revision 1.2  1995/09/05 22:15:51  jussi
  Added CVS header.
*/

#include <string.h>
#include <assert.h>

#include "TDataAsciiInterp.h"
#include "AttrList.h"
#include "RecInterp.h"
#include "CompositeParser.h"
#include "Parse.h"
#include "Control.h"
#include "Util.h"

TDataAsciiInterpClassInfo::TDataAsciiInterpClassInfo(
	char *className, AttrList *attrList, int recSize,
	char *separators, int numSeparators,
	Boolean isSeparator, char *commentString)
{
  _className = className;
  _attrList = attrList;
  _separators = separators;
  _numSeparators = numSeparators;
  _commentString = commentString;
  _recSize = recSize;
  _isSeparator = isSeparator;
  _tdata = NULL;
}

TDataAsciiInterpClassInfo::TDataAsciiInterpClassInfo(
	char *className, char *name, char *alias, TData *tdata)
{
  _className = className;
  _name = name;
  _alias = alias;
  _tdata = tdata;
}

TDataAsciiInterpClassInfo::~TDataAsciiInterpClassInfo()
{
  /*
     printf("TDataAsciiInterpClassInfo: destructor\n");
  */
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
  if (argc != 2) {
    fprintf(stderr,"TDataAsciiInterp::CreateWithParams: wrong args\n");
    for (int i=0; i < argc; i++)
      printf("%s\n", argv[i]);
    return (ClassInfo *)NULL;
  }
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

TDataAsciiInterp::TDataAsciiInterp(char *name, int recSize, AttrList *attrs,
				   char *separators, int numSeparators,
				   Boolean isSeparator, char *commentString) :
	TDataAscii(name, recSize)
{
  /*
     printf("TDataAsciiInterp %s recSize %d\n", name, recSize);
  */
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

  for(int i = 0; i < _numAttrs; i++) {
    AttrInfo *info = _attrList->Get(i);
    if (info->isComposite)
      hasComposite = true;
    if (info->hasMatchVal){
      if (_numMatchingAttrs >= MAX_MATCHING_ATTRS){
	fprintf(stderr,"TDataASciiInterp: too many matching attrs\n");
	Exit::DoExit(2);
      }
      _matchingAttrs[_numMatchingAttrs++] = info;
    }
  }
  
  Initialize();
}

TDataAsciiInterp::~TDataAsciiInterp()
{
  /*
     printf("TDataAsciiInterp: destructor\n");
  */
}

Boolean TDataAsciiInterp::IsValid(char *line)
{
  int numArgs;
  char **args;

  Parse(line, numArgs, args, _separators, _numSeparators, _isSeparator);

  if (numArgs == 0 || 
      (_commentString != NULL &&
       strncmp(args[0], _commentString, _commentStringLength) == 0)) {
    return false;
  }
	
  return true;
}

Boolean TDataAsciiInterp::Decode(RecId id, void *recordBuf, char *line)
{
  /* set buffer for interpreted record */
  _recInterp->SetBuf(recordBuf);
  
  int numArgs;
  char **args;

  Parse(line, numArgs, args, _separators, _numSeparators, _isSeparator);
  
  assert(numArgs > 0);

  int maxArgs = MinMax::min(numArgs, _numAttrs);

  int i;
  for(i = 0; i < _numMatchingAttrs; i++)
    if (_matchingAttrs[i]->attrNum >= maxArgs)
      return false;

  for(i = 0; i < maxArgs; i++) {

    AttrInfo *info = _attrList->Get(i);
    char *ptr = (char *)recordBuf + info->offset;
    int intVal;
    float floatVal;
    double doubleVal;
    time_t tm;

    switch(info->type) {
    case IntAttr:
      intVal = atoi(args[i]);
      if (info->hasMatchVal && intVal != info->matchVal.intVal)
	return false;
      *(int *)ptr = intVal;
      // printf("int %d\n", intVal);
      break;

    case FloatAttr:
      floatVal = UtilAtof(args[i]);
      if (info->hasMatchVal && floatVal != info->matchVal.floatVal)
	return false;
      *(float *)ptr = floatVal;
      // printf("float %f\n", floatVal);
      break;

    case DoubleAttr:
      doubleVal = UtilAtof(args[i]);
      if (info->hasMatchVal && doubleVal != info->matchVal.doubleVal)
	return false;
      *(double *)ptr = doubleVal;
      // printf("double %f\n", doubleVal);
      break;

    case StringAttr:
      if (info->hasMatchVal && strcmp(args[i], info->matchVal.strVal))
	return false;
      strncpy(ptr, args[i], info->length);
      ptr[info->length - 1] = '\0';
      // printf("string '%s'\n", ptr);
      break;

    case DateAttr:
      if (info->hasMatchVal) {
	fprintf(stderr,"TDataAsciiInterp: date attr match not implemented\n");
	Exit::DoExit(2);
      }
      tm = atoi(args[i]);
      *(time_t *)ptr = tm;
      break;

    default:
      fprintf(stderr, "TDataAsciiInterp: unknown attribute type: %d\n",
	      info->type);
      Exit::DoExit(2);
    }
  }
  
  /* decode composite attributes */
  if (hasComposite)
    CompositeParser::Decode(_attrList->GetName(), _recInterp);

  return true;
}
