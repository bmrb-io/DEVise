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
  Revision 1.7  1996/07/04 15:25:35  jussi
  Updated to reflect new interface to TDataAscii. Added type
  parameter (UNIXFILE).

  Revision 1.6  1996/06/13 17:35:48  jussi
  Added calls to DataSeg::Set().

  Revision 1.5  1996/05/16 19:27:03  jussi
  Removed references to ControlPanel::File_Alias().

  Revision 1.4  1996/05/07 21:13:39  jussi
  Added record position parameter to Decode(). Added alias parameter
  to constructor.

  Revision 1.3  1996/04/12 23:44:04  jussi
  Removed IsValid() and removed RecId parameter from Decode().

  Revision 1.2  1995/11/25 19:48:28  jussi
  Added copyright notice and cleaned up the code a bit. Added IsValid
  method.
*/

#include <stdio.h>
#include <stdlib.h>

#include "TDataMulti.h"
#include "MultiRec.h"
#include "Parse.h"
#include "Control.h"
#include "Exit.h"
#include "Util.h"
#include "DataSeg.h"

MultiClassInfo::MultiClassInfo()
{
  _name = _type = _param = 0;
  _tdata = 0;
}

MultiClassInfo::MultiClassInfo(char *name, char *type,
                               char *param, TDataMulti *tdata)
{
  _name = name;
  _type = type;
  _param = param;
  _tdata = tdata;
}

MultiClassInfo::~MultiClassInfo()
{
  delete _tdata;
}

static char buf[3][256];
static char *args[3];

void MultiClassInfo::ParamNames(int &argc, char **&argv)
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

ClassInfo *MultiClassInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 2 && argc != 3)
    return 0;

  char *name, *type, *param;

  if (argc == 2) {
    name = CopyString(argv[1]);
    type = CopyString("UNIXFILE");
    param = CopyString(argv[0]);
  } else {
    name = CopyString(argv[0]);
    type = CopyString(argv[1]);
    param = CopyString(argv[2]);
  }

  DataSeg::Set(name, param, 0, 0);

  TDataMulti *tdata = new TDataMulti(name, type, param);
  return new MultiClassInfo(name, type, param, tdata);
}

char *MultiClassInfo::InstanceName()
{
  return _name;
}

void *MultiClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */

void MultiClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 3;
  argv = args;
  args[0] = _name;
  args[1] = _type;
  args[2] = _param;
}

TDataMulti::TDataMulti(char *name, char *type, char *param) :
     TDataAscii(name, type, param, sizeof(MultiRec))
{
  _hasFirst = false;
  Initialize();
}

/* Decode a record and put data into buffer */

Boolean TDataMulti::Decode(void *recordBuf, int recPos, char *line)
{
  MultiRec *rec = (MultiRec *)recordBuf;
  int numArgs; char **args;

  Parse(line, numArgs, args);

  if (numArgs != 5) {
    fprintf(stderr, "Ignoring invalid line: %s\n", line);
    return false;
  }

  if (!_hasFirst) {
    _hasFirst = true;
    _first = atoi(args[1]);
  }
  
  rec->node = atoi(args[0]);
#if 0
  rec->begin = atoi(args[1])- _first;
  rec->end = atoi(args[2]) - _first;
#endif
  rec->begin = atoi(args[1]);
  rec->end = atoi(args[2]);
  rec->job = atoi(args[3]);
  rec->cycles = atoi(args[4]);
  
  if (rec->node < 0 || rec->job < -32 || rec->job > 32) {
    fprintf(stderr, "ignored line %d %d %d %d\n",
	    rec->node, rec->begin, rec->end, rec->job);
    return false;
  }
  
  return true;
}
