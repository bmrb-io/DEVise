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
  _name = _alias = 0;
  _tdata = 0;
}

MultiClassInfo::MultiClassInfo(char *name, char *alias, TDataMulti *tdata)
{
  _name = name;
  _alias = alias;
  _tdata = tdata;
}

static char buf1[512], buf2[256];
static char *args[5];

void MultiClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = buf1;
  args[1] = buf2;
  
  strcpy(buf1, "File {foobar}");
  strcpy(buf2, "Alias {foobar}");
}

ClassInfo *MultiClassInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr, "MultiClassInfo::CreateWithParams: wrong args\n");
    for(int i = 0; i < argc; i++)
      printf("%s\n", argv[i]);
    return 0;
  }

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);

  DataSeg::Set(alias, name, 0, 0);

  TDataMulti *tdata = new TDataMulti(name, alias);
  return new MultiClassInfo(name, alias, tdata);
}

char *MultiClassInfo::InstanceName()
{
  return _alias;
}

void *MultiClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */

void MultiClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = _name;
  args[1] = _alias;
}

TDataMulti::TDataMulti(char *name, char *alias) :
     TDataAscii(name, alias, sizeof(MultiRec))
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

