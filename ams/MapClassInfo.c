/*
  ========================================================================
  DEVise Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  University of Wisconsin at Madison
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.8  1996/11/23 20:45:36  jussi
  Removed references to DispQueryProc.

  Revision 1.7  1996/07/10 00:01:48  jussi
  TDataMapDispatch replaced with TDataMap.

  Revision 1.6  1996/05/16 19:29:34  jussi
  Removed references to ControlPanel::File_Alias().

  Revision 1.5  1995/12/14 18:45:26  jussi
  Removed #include "ViewWinVer.h".

  Revision 1.4  1995/09/22 16:01:07  jussi
  Added copyright message.

  Revision 1.3  1995/09/05 20:23:45  jussi
  Updated CVS header.

  Revision 1.2  1995/09/05 20:19:19  jussi
  Added CVS header
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
#include "TDataMap.h"
#include "Temp.h"
#include "ViewScatter.h"
#include "MapClassInfo.h"
#include "Util.h"
#include "AttrList.h"

#include "Soil.h"
#include "AmsMap.h"


MapInfo::MapInfo(MapType mapType) {
	_mapType = mapType;
}

MapInfo::MapInfo(MapType mapType,
	char *tdataAlias, char *name, TDataMap *mapping){
	_mapType = mapType;
	_tdataAlias = tdataAlias;
	_name = name;
	_map = mapping;
}

static char buf1[256], buf2[256];
static char *arg[5];
/* Get name of parameters */
void MapInfo::ParamNames(int &argc, char **&argv){
    argc = 2;
    argv = arg;
    arg[0] = buf1;
    strcpy(buf1,"File_Alias {foobar}");
    arg[1] = buf2;
    strcpy(buf2,"GData_Name {foobar}");
}

/* Create instance using the supplied parameters. Return
the instance info if successful, otherwise return NULL. */
ClassInfo *MapInfo::CreateWithParams(int argc, char **argv){
    if (argc != 2){
		fprintf(stderr,"MapInfo::CreateWithParams argc = %d != 2\n", argc);
		return NULL;
	}
	TData *tdata;
	char *tdataAlias = CopyString(argv[0]);
	if ((tdata=(TData *)ControlPanel::FindInstance(tdataAlias)) == NULL){
		fprintf(stderr,"MapInfo::CreateWithParams: can't find tdata\n");
		return NULL;
	}
	char *gdataName = CopyString(argv[1]);
	TDataMap *map;
	VisualFlag *dimInfo = new VisualFlag[1];
	dimInfo[0] = VISUAL_X;
	if (_mapType == SolDnType){
		map = new SolDnCompiled(tdata,gdataName,dimInfo, 1,
			Init::MaxGDataPages());
	} else if (_mapType == RainType) {
		map = new RainCompiled(tdata,gdataName,dimInfo, 1,
			Init::MaxGDataPages());
	} else {
		map = new TempCompiled(tdata,gdataName,dimInfo, 1,
			Init::MaxGDataPages());
	}
	return new MapInfo(_mapType, tdataAlias, gdataName, map);
}

/**************************************************
	Instance Info.
***************************************************/
char *MapInfo::InstanceName() { return _name; }
void *MapInfo::GetInstance() { return _map; }

/* Get parameters that can be used to re-create this instance */
void MapInfo::CreateParams(int &argc, char **&argv){
	argc =2;
	argv = arg;
	arg[0] = _tdataAlias;
	arg[1] = _name;
}
