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
  Revision 1.1  1996/04/25 19:25:19  wenger
  Attribute projection code can now parse a schema, and create the
  corresponding TData object.

*/

#include <string.h>

#include "ApTDataBinaryInterp.h"
#include "AttrList.h"
#include "CompositeParser.h"
#include "Parse.h"
#include "Util.h"

#if 0
TDataBinaryInterpClassInfo::TDataBinaryInterpClassInfo(char *className,
						       AttrList *attrList,
						       int recSize)
{
  _className = className;
  _attrList = attrList;
  _recSize = recSize;
  _tdata = NULL;
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
  
  sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
  sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *TDataBinaryInterpClassInfo::CreateWithParams(int argc, char **argv)
{
  DOASSERT(argc == 2, "Invalid parameters");

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);
  TDataBinaryInterp *tdata = new TDataBinaryInterp(name,_recSize, _attrList);
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
#endif

TDataBinaryInterp::TDataBinaryInterp(char *name, int recSize,
				     AttrList *attrs) :
     TDataBinary(name, recSize)
{
  _name = name;
  _recSize = recSize;
  _attrList = attrs;
  _numAttrs = _attrList->NumAttrs();
  
  hasComposite = false;

  for(int i = 0; i < _attrList->NumAttrs(); i++) {
    AttrInfo *info = _attrList->Get(i);
    if (info->isComposite) {
      hasComposite = true;
      _numAttrs--;
    }
  }
  
  Initialize();
}

TDataBinaryInterp::~TDataBinaryInterp()
{
}

Boolean TDataBinaryInterp::Decode(RecId id, void *recordBuf, char *line)
{
  memcpy(recordBuf, line, _recSize);

  for(int i = 0; i < _numAttrs; i++) {
    AttrInfo *info = _attrList->Get(i);
    if (!info->hasMatchVal)
      continue;

    char *ptr = (char *)recordBuf + info->offset;
    int length = info->length;
    if (memcmp(ptr, &info->matchVal, length))
      return false;
  }
  
  /* decode composite attributes */
  if (hasComposite)
    CompositeParser::Decode(_attrList->GetName(), 0);

  return true;
}
