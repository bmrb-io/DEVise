/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.7  1996/12/03 20:44:05  jussi
  Removed reference to unneeded file Snapshot.h.

  Revision 1.6  1996/11/23 20:47:42  jussi
  Removed references to DispQueryProc.

  Revision 1.5  1996/11/18 23:40:44  wenger
  Got 'multi' to compile with latest Color stuff.

  Revision 1.4  1996/07/10 00:01:25  jussi
  TDataMapDispatch replaced with TDataMap.

  Revision 1.3  1996/05/16 19:27:01  jussi
  Removed references to ControlPanel::File_Alias().

  Revision 1.2  1995/12/14 19:24:17  jussi
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
#include "TDataMap.h"
#include "TDataHost.h"
#include "Temp.h"
#include "ViewScatter.h"
#include "HostMappingClassInfo.h"
#include "HostRec.h"
#include "Util.h"
#include "Session.h"

GlobalColor all_busyColor;
GlobalColor some_idleColor;

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
    strcpy(buf1, "File_Alias {foobar}");
    arg[1] = buf2;
    strcpy(buf2, "GData_Name {foobar}");
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
        tdata = (TData *) ControlPanel::FindInstance(tdataAlias);
        // If we don't already have this TData, try to create it.
        if (!tdata) {
          Session::CreateTData(tdataAlias);
          tdata = (TData *)ControlPanel::FindInstance(tdataAlias);
        }
        if (tdata == NULL) {
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
