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
  for(int i = 0; i < _attrList->NumAttrs(); i++) {
    AttrInfo *info = _attrList->Get(i);
    if (!info->isComposite)
      _physRecSize += info->length;
  }

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
  
  sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
  sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *TDataBinaryInterpClassInfo::CreateWithParams(int argc, char **argv)
{
  DOASSERT(argc == 2, "Invalid parameters");

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);
  TDataBinaryInterp *tdata = new TDataBinaryInterp(name, _recSize,
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

TDataBinaryInterp::TDataBinaryInterp(char *name, int recSize, int physRecSize,
				     AttrList *attrs) :
     TDataBinary(name, recSize, physRecSize)
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

Boolean TDataBinaryInterp::WriteCache(int fd)
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

Boolean TDataBinaryInterp::ReadCache(int fd)
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

Boolean TDataBinaryInterp::Decode(RecId id, void *recordBuf, char *line)
{
  /* set buffer for interpreted record */
  _recInterp->SetBuf(recordBuf);
  
  if (recordBuf != line)
    memcpy(recordBuf, line, _physRecSize);

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
    CompositeParser::Decode(_attrList->GetName(), _recInterp);

  return true;
}
