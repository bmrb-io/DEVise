/* HostMappingClassInfo.c */
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
#include "TDataHost.h"
#include "Temp.h"
#include "ViewWinVer.h"
#include "Snapshot.h"
#include "ViewScatter.h"
#include "HostMappingClassInfo.h"
#include "HostRec.h"
#include "Util.h"

Color all_busyColor;
Color some_idleColor;

#include "map_host.h"

HostMapInfo *HostmapInfo;

HostMappingInfo::HostMappingInfo() {}

HostMappingInfo::HostMappingInfo(char *tdataAlias, char *name, TDataMap *mapping){
	_tdataAlias = tdataAlias;
	_name = name;
	_map = mapping;
}

static char buf1[256], buf2[256];
static char *arg[5];
/* Get name of parameters */
void HostMappingInfo::ParamNames(int &argc, char **&argv){
    argc = 2;
	argv = arg;
	arg[0] = buf1;
	sprintf(buf1,"File_Alias %s", ControlPanel::Instance()->FileAlias());
	arg[1] = buf2;
	sprintf(buf2,"GData_Name %s",ControlPanel::Instance()->GDataName());
}

/* Create instance using the supplied parameters. Return
the instance info if successful, otherwise return NULL. */
ClassInfo *HostMappingInfo::CreateWithParams(int argc, char **argv){
    if (argc != 2){
		fprintf(stderr,"HostMapInfo::CreateWithParams\n");
		return NULL;
	}
	TData *tdata;
	char *tdataAlias = CopyString(argv[0]);
	if ((tdata=(TData *)ControlPanel::FindInstance(tdataAlias)) == NULL){
		fprintf(stderr,"HostHostMappingInfo::CreateWithParams: can't find tdata\n");
		return NULL;
	}
	char *gdataName = CopyString(argv[1]);
	TDataMap *map = new HostMapping(tdata,gdataName,NULL, 0,
		Init::MaxGDataPages(), HostmapInfo);
	return new HostMappingInfo(tdataAlias, gdataName, map);
}

/**************************************************
	Instance Info.
***************************************************/
char *HostMappingInfo::InstanceName() { return _name; }
void *HostMappingInfo::GetInstance() { return _map; }

/* Get parameters that can be used to re-create this instance */
void HostMappingInfo::CreateParams(int &argc, char **&argv){
	argc =2;
	argv = arg;
	arg[0] = _tdataAlias;
	arg[1] = _name;
}
