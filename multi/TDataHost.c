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
  Revision 1.6  1996/06/13 17:35:47  jussi
  Added calls to DataSeg::Set().

  Revision 1.5  1996/05/16 19:27:03  jussi
  Removed references to ControlPanel::File_Alias().

  Revision 1.4  1996/05/07 21:13:38  jussi
  Added record position parameter to Decode(). Added alias parameter
  to constructor.

  Revision 1.3  1996/04/12 23:43:57  jussi
  Removed IsValid() and removed RecId parameter from Decode().

  Revision 1.2  1995/11/25 19:50:24  jussi
  Added copyright notice and the IsValid() method.
*/

#include <stdio.h>
#include <stdlib.h>

#include "TDataHost.h"
#include "HostRec.h"
#include "Parse.h"
#include "Control.h"
#include "Exit.h"
#include "Util.h"
#include "DataSeg.h"

HostClassInfo::HostClassInfo()
{
  _name = _type = _param = 0;
  _tdata = 0;
}

HostClassInfo::HostClassInfo(char *name, char *type,
                             char *param, TDataHost *tdata)
{
  _name = name;
  _type = type;
  _param = param;
  _tdata = tdata;
}

static char buf[3][256];
static char *args[3];

void HostClassInfo::ParamNames(int &argc, char **&argv)
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

ClassInfo *HostClassInfo::CreateWithParams(int argc, char **argv)
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

  TDataHost *tdata = new TDataHost(name, type, param);
  return new HostClassInfo(name, type, param, tdata);
}

char *HostClassInfo::InstanceName()
{
  return _name;
}

void *HostClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */

void HostClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 3;
  argv = args;
  args[0] = _name;
  args[1] = _type;
  args[2] = _param;
}

TDataHost::TDataHost(char *name, char *type, char *param) :
     TDataAscii(name, type, param, sizeof(HostRec))
{
  _hasFirst = false;
  Initialize();
}

/* Decode a record and put data into buffer */

Boolean TDataHost::Decode(void *recordBuf, int recPos, char *line)
{
  HostRec *rec = (HostRec *)recordBuf;
  int numArgs; char **args;

  Parse(line, numArgs, args);

  if (numArgs != 4) {
    fprintf(stderr, "Ignoring invalid line: %s\n", line);
    return false;
  }

  if (!_hasFirst) {
    _hasFirst = true;
    _first = atoi(args[1]);
  }
  
  rec->time = atoi(args[0]);
  rec->hosts = atoi(args[1]);
  rec->working = atoi(args[2]);
  rec->suspended = atoi(args[3]);

#if 0
  if (rec->working + rec->suspended > rec->hosts) {
    fprintf(stderr, "Ignoring line %d %d %d %d\n",
	    rec->time, rec->hosts, rec->working, rec->suspended);
    return false;
  }
#endif

  return true;
}

