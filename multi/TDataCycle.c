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
  Revision 1.3  1996/04/12 23:43:51  jussi
  Removed IsValid() and removed RecId parameter from Decode().

  Revision 1.2  1995/11/25 19:51:58  jussi
  Added copyright notice and the IsValid() method.
*/

#include <stdio.h>
#include <stdlib.h>

#include "TDataCycle.h"
#include "CycleRec.h"
#include "Parse.h"
#include "Control.h"
#include "Exit.h"
#include "Util.h"

CycleClassInfo::CycleClassInfo()
{
  _name = _alias = 0;
  _tdata = 0;
}

CycleClassInfo::CycleClassInfo(char *name, char *alias, TDataCycle *tdata)
{
  _name = name;
  _alias = alias;
  _tdata = tdata;
}

static char buf1[512], buf2[256];
static char *args[5];

void CycleClassInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = buf1;
  args[1] = buf2;

  sprintf(buf1, "File %s", ControlPanel::Instance()->FileName());
  sprintf(buf2, "Alias %s", ControlPanel::Instance()->FileAlias());
}

ClassInfo *CycleClassInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr,"AfsioClassInfo::CreateWithParams: wrong args\n");
    for(int i = 0; i < argc; i++)
      printf("%s\n", argv[i]);
    return 0;
  }

  char *name = CopyString(argv[0]);
  char *alias = CopyString(argv[1]);
  TDataCycle *tdata = new TDataCycle(name, alias);
  return new CycleClassInfo(name, alias, tdata);
}

char *CycleClassInfo::InstanceName()
{
  return _alias;
}

void *CycleClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */

void CycleClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv = args;
  args[0] = _name;
  args[1] = _alias;
}

TDataCycle::TDataCycle(char *name, char *alias) :
     TDataAscii(name, alias, sizeof(CycleRec))
{
  _now = 0;
  Initialize();
}

/* Decode a record and put data into buffer */

Boolean TDataCycle::Decode(void *recordBuf, int recPos, char *line)
{
  CycleRec *rec = (CycleRec *)recordBuf;
  int numArgs; char **args;
  int this_time;

  Parse(line, numArgs, args);

  if (numArgs != 7) {
    fprintf(stderr, "Invalid line: %s\n", line);
    _now = 0;
    
    rec->last_time = 0;
    rec->time = 0;
    rec->length = 0;
    rec->occ = 0;
    rec->working = 0;
    rec->cpu_time = 0;
    rec->suspended = 0;
    rec->eff = 0.0;
    return true;
  }

  if (!strcmp(args[0], "-------") || !strcmp(args[0], "Totals")) {
    fprintf(stderr, "Header or trailer line: %s\n", line);
    return false;
  }
  
  this_time = atoi(args[1]);
  rec->last_time = _now;
  _now += this_time;
  
  rec->time = _now;
  rec->length = this_time;
  rec->occ = atoi(args[2]);
  rec->working = atoi(args[3]);
  rec->cpu_time = atoi(args[4]);
  rec->suspended = atoi(args[5]);
  rec->eff = (double) atof(args[6]);
  
  return true;
}

