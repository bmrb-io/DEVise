/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 20:19:19  jussi
  Added VCS header
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
#include "Temp.h"
#include "ViewWinVer.h"
#include "Snapshot.h"
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
	sprintf(buf1,"File_Alias %s", ControlPanel::Instance()->FileAlias());
	arg[1] = buf2;
	sprintf(buf2,"GData_Name %s",ControlPanel::Instance()->GDataName());
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
