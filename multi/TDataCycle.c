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
  Revision 1.6  1996/06/13 17:35:46  jussi
  Added calls to DataSeg::Set().

  Revision 1.5  1996/05/16 19:27:02  jussi
  Removed references to ControlPanel::File_Alias().

  Revision 1.4  1996/05/07 21:13:37  jussi
  Added record position parameter to Decode(). Added alias parameter
  to constructor.

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
#include "DataSeg.h"

CycleClassInfo::CycleClassInfo()
{
  _name = _type = _param = 0;
  _tdata = 0;
}

CycleClassInfo::CycleClassInfo(char *name, char *type,
                               char *param, TDataCycle *tdata)
{
  _name = name;
  _type = type;
  _param = param;
  _tdata = tdata;
}

static char buf[3][256];
static char *args[3];

void CycleClassInfo::ParamNames(int &argc, char **&argv)
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

ClassInfo *CycleClassInfo::CreateWithParams(int argc, char **argv)
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

  TDataCycle *tdata = new TDataCycle(name, type, param);
  return new CycleClassInfo(name, type, param, tdata);
}

char *CycleClassInfo::InstanceName()
{
  return _name;
}

void *CycleClassInfo::GetInstance()
{
  return _tdata;
}

/* Get parameters that can be used to re-create this instance */

void CycleClassInfo::CreateParams(int &argc, char **&argv)
{
  argc = 3;
  argv = args;
  args[0] = _name;
  args[1] = _type;
  args[2] = _param;
}

TDataCycle::TDataCycle(char *name, char *type, char *param) :
     TDataAscii(name, type, param, sizeof(CycleRec))
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
