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

HostClassInfo::HostClassInfo()
{
  _name = _alias = 0;
  _tdata = 0;
}

HostClassInfo::HostClassInfo(char *name, char *alias, TDataHost *tdata)
{
  _name = name;
  _alias = alias;
  _tdata = tdata;
}

static char buf1[512], buf2[256];
static char *args[5];

void HostClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = buf1;
  args[1] = buf2;
  
  strcpy(buf1, "File {foobar}");
  strcpy(buf2, "Alias {foobar}");
}

ClassInfo *HostClassInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr,"AfsioClassInfo::CreateWithParams: wrong args\n");
    for(int i = 0; i < argc; i++)
      printf("%s\n", argv[i]);
    return 0;
  }

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);
  TDataHost *tdata = new TDataHost(name, alias);
  return new HostClassInfo(name, alias, tdata);
}

char *HostClassInfo::InstanceName()
{
  return _alias;
}

void *HostClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */

void HostClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = _name;
  args[1] = _alias;
}

TDataHost::TDataHost(char *name, char *alias) :
     TDataAscii(name, alias, sizeof(HostRec))
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

