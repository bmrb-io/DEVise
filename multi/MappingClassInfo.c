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
  Revision 1.3  1996/05/07 21:13:10  jussi
  Renamed MappingInfo to MultiMappingInfo.

  Revision 1.2  1995/12/14 19:24:13  jussi
  Added copyright notice and CVS header. Removed reference to
  ViewWinVer.h.
*/

#include <sys/time.h>
#include <time.h>
#include <math.h>

#include "tcl.h"
#include "DeviseTypes.h"
#include "Display.h"
#include "Dispatcher.h"
#include "RectShape.h"
#include "ColorMgr.h"
#include "Control.h"
#include "TDataCMap.h"
#include "Connector.h"
#include "ActionDefault.h"
#include "Selection.h"
#include "VisualArg.h"
#include "Init.h"
#include "TDataMapDispatch.h"
#include "DispQueryProc.h"
#include "BufMgr.h"
#include "TDataMulti.h"
#include "Temp.h"
#include "Snapshot.h"
#include "ViewScatter.h"
#include "MappingClassInfo.h"
#include "MultiRec.h"
#include "Util.h"

Color suspendColor;
Color abortColor;
struct MapInfo *mapInfo;

#include "mapfile.h"

MultiMappingInfo::MultiMappingInfo()
{
}

MultiMappingInfo::MultiMappingInfo(char *tdataAlias, char *name,
				   TDataMap *mapping)
{
  _tdataAlias = tdataAlias;
  _name = name;
  _map = mapping;
}

static char buf1[256], buf2[256];
static char *arg[5];

/* Get name of parameters */
void MultiMappingInfo::ParamNames(int &argc, char **&argv)
{
  argc = 2;
  argv = arg;
  arg[0] = buf1;
  strcpy(buf1, "File_Alias {foobar}");
  arg[1] = buf2;
  strcpy(buf2, "GData_Name {foobar}");
}

/* Create instance using the supplied parameters. Return
   the instance info if successful, otherwise return NULL. */
ClassInfo *MultiMappingInfo::CreateWithParams(int argc, char **argv)
{
  if (argc != 2) {
    fprintf(stderr,"MultiMapInfo::CreateWithParams\n");
    return NULL;
  }

  TData *tdata;
  char *tdataAlias = CopyString(argv[0]);
  if ((tdata = (TData *)ControlPanel::FindInstance(tdataAlias)) == NULL) {
    fprintf(stderr,
	    "MultiMultiMappingInfo::CreateWithParams: can't find tdata\n");
    return NULL;
  }
  char *gdataName = CopyString(argv[1]);
  TDataMap *map = new MultiMapping(tdata, gdataName, NULL, 0,
				   Init::MaxGDataPages(), mapInfo);
  return new MultiMappingInfo(tdataAlias, gdataName, map);
}

char *MultiMappingInfo::InstanceName()
{
  return _name;
}

void *MultiMappingInfo::GetInstance()
{
  return _map;
}

/* Get parameters that can be used to re-create this instance */
void MultiMappingInfo::CreateParams(int &argc, char **&argv)
{
  argc = 2;
  argv = arg;
  arg[0] = _tdataAlias;
  arg[1] = _name;
}
