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
  Revision 1.9  1996/07/05 15:22:54  jussi
  This executable can now handle SortedX views.

  Revision 1.8  1996/06/13 18:17:22  jussi
  Minor improvements.

  Revision 1.7  1996/05/22 21:07:06  jussi
  ControlPanel::_controlPanel is now set by main program.

  Revision 1.6  1996/05/07 21:11:29  jussi
  Cleaned up the code a little bit.

  Revision 1.5  1995/12/14 19:16:25  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.4  1995/12/14 15:44:57  jussi
  Added copyright and CVS headers. Replaced WinVertical and
  WinHorizontal with TileLayout.
*/

#include <math.h>
#include <time.h>
#include <sys/time.h>

#include "DeviseTypes.h"
#include "Display.h"
#include "Dispatcher.h"
#include "RectShape.h"
#include "ColorMgr.h"
#include "Control.h"
#include "Init.h"
#include "VisualLinkClassInfo.h"
#include "TDataMulti.h"
#include "TDataHost.h"
#include "TDataCycle.h"
#include "TDataViewX.h"
#include "Snapshot.h"
#include "ViewClassInfo.h"
#include "MappingClassInfo.h"
#include "MapInterpClassInfo.h"
#include "HostMappingClassInfo.h"
#include "CycleOccMappingClassInfo.h"
#include "CycleEffMappingClassInfo.h"
#include "CycleWorkingMappingClassInfo.h"
#include "CycleSuspendedMappingClassInfo.h"
#include "CycleCpuTimeMappingClassInfo.h"
#include "ViewClassInfo.h"
#include "WinClassInfo.h"
#include "ActionDefault.h"
#include "Message.h"
#include "TData.h"
#include "TDataMap.h"
#include "ActionClassInfo.h"
#include "AxisLabelClassInfo.h"
#include "CursorClassInfo.h"

const int MAX_COLORS = 32;

int debug = 0;

class DateAxisLabel: public AxisLabel {
public:
  DateAxisLabel(char *name):AxisLabel(name) {}
  virtual char *GenerateLabel(Coord val) {
    /* Convert into # of hours */
    int clock = (int)val;
    int day = clock / (3600 * 24);
    clock -= day * (3600 * 24);
    int hour = clock / 3600;
    clock -= hour * 3600;
    int min = clock / 60;
    int sec = clock % 60;
    sprintf(buf, "%d:%02d:%02d:%02d", day, hour, min, sec);
    return buf;
  }

private:
  char buf[80];
};

class GenDateAxisLabel: public GenAxisLabel {
public:
  AxisLabel *MakeAxisLabel(char *name) {
    return new DateAxisLabel(name);
  }
};

class MultiAction: public ActionDefault  {
public:
  MultiAction(char *name, Coord leftEdge = 0.0, Boolean useLeftFlag = false,
	      Coord rightEdge = 0.0, Boolean useRightFlag = false) :
  ActionDefault(name,leftEdge, useLeftFlag, rightEdge, useRightFlag) {}

  virtual Boolean PopUp(ViewGraph *view, Coord x, Coord y, Coord xHigh,
			Coord yHigh, int button, char **&msgs,
			int &numMsgs) {
    /* only do 1st tdata */
    int index = view->InitMappingIterator();
    if (view->MoreMapping(index)) {
      TDataMap *map = view->NextMapping(index)->map;
      int time = (int)trunc(x);
      int node = (int)trunc(y);
      GenMessage(map->GetTData(), time, node, numMsgs, msgs);
    }
    view->DoneMappingIterator(index);
    return true;
  }

private:
  TData *tdata;
};

class GenMultiAction: public GenAction {
public:
  Action *MakeAction(char *name) {
    return new MultiAction(name);
  }
};

const int MAX_JOBS = 32;
main(int argc, char **argv)
{
  Init::DoInit(argc,argv);
  ControlPanel::RegisterClass(new AxisLabelClassInfo("date",
						     new GenDateAxisLabel()));
  
  ControlPanel::RegisterClass(new TileLayoutInfo);
  ControlPanel::RegisterClass(new ActionClassInfo("multiAction",
						  new GenMultiAction()));
  ControlPanel::RegisterClass(new MultiClassInfo);
  ControlPanel::RegisterClass(new MultiMappingInfo);
  ControlPanel::RegisterClass(new HostMappingInfo);
  ControlPanel::RegisterClass(new CycleOccMappingInfo);
  ControlPanel::RegisterClass(new CycleEffMappingInfo);
  ControlPanel::RegisterClass(new VisualLinkClassInfo);
  ControlPanel::RegisterClass(new CursorClassInfo());
  ControlPanel::RegisterClass(new ViewXInfo);
  ControlPanel::RegisterClass(new ViewScatterInfo);
  ControlPanel::RegisterClass(new HostClassInfo);
  ControlPanel::RegisterClass(new CycleClassInfo);
  ControlPanel::RegisterClass(new CycleSuspendedMappingInfo);
  ControlPanel::RegisterClass(new CycleWorkingMappingInfo);
  ControlPanel::RegisterClass(new CycleCpuTimeMappingInfo);
  
  mapInfo = new MapInfo;

  /* job ordering based on length. job_order[i] == order of job i */
  int job_ordering[MAX_JOBS];
  mapInfo->job_ordering = job_ordering;

  job_ordering[30] = 0;
  job_ordering[31] = 1;
  job_ordering[29] = 2;
  job_ordering[28] = 3;
  job_ordering[27] = 4;
  job_ordering[26] = 5;
  job_ordering[1] = 6;
  job_ordering[25] = 7;
  job_ordering[2] = 8;
  job_ordering[24] = 9;
  job_ordering[3] = 10;
  job_ordering[4] = 11;
  job_ordering[23] = 12;
  job_ordering[21] = 13;
  job_ordering[22] = 14;
  job_ordering[8] = 15;
  job_ordering[9] = 16;
  job_ordering[10] = 17;
  job_ordering[ 5] = 18;
  job_ordering[15] = 19;
  job_ordering[6 ] = 20;
  job_ordering[16] = 21;
  job_ordering[17] = 22;
  job_ordering[ 7] = 23;
  job_ordering[ 20] = 24;
  job_ordering[ 12] = 25;
  job_ordering[ 11] = 26;
  job_ordering[ 13] = 27;
  job_ordering[ 19] = 28;
  job_ordering[ 18] = 29;
  job_ordering[ 14] = 30;
  
  /* init colors */
  Color colorArray0[MAX_COLORS];
  Color colorArray1[MAX_COLORS];
  Color colorArray2[MAX_COLORS];
  Color colorArray3[MAX_COLORS];
  Color colorArray4[MAX_COLORS];
  Color colorArray5[MAX_COLORS];
  Color *colorArrays[6];
  colorArrays[0] = colorArray0;
  colorArrays[1] = colorArray1;
  colorArrays[2] = colorArray2;
  colorArrays[3] = colorArray3;
  colorArrays[4] = colorArray4;
  colorArrays[5] = colorArray5;
  mapInfo->colorArrays = colorArrays;
  
  /* create control panel */
  ControlPanel::_controlPanel = GetNewControl();
  ControlPanel *ctrl = ControlPanel::Instance();
  
  int depth = DeviseDisplay::DefaultDisplay()->NumPlanes();
  
  if (depth >= 8) {
    printf("using colors\n");
    double colorVal = 0.0;
    double colorIncr = .8 / (MAX_COLORS);
    suspendColor = ColorMgr::AllocColor("red");
    abortColor = ColorMgr::AllocColor("black");
    all_busyColor = ColorMgr::AllocColor("green");
    some_idleColor = ColorMgr::AllocColor("red");
    
    for(int i = 0; i < MAX_COLORS; i++) {
      colorArray0[i] = ColorMgr::AllocColor(1.0, colorVal, colorVal);
      colorArray1[i] = ColorMgr::AllocColor(colorVal, 1.0, colorVal);
      colorArray2[i] = ColorMgr::AllocColor(colorVal,  colorVal, 1.0);
      colorArray3[i] = ColorMgr::AllocColor(1.0, colorVal, colorVal);
      colorArray4[i] = ColorMgr::AllocColor(colorVal, 1.0, colorVal);
      colorArray5[i] = ColorMgr::AllocColor(colorVal,  colorVal, 1.0);
      colorVal += colorIncr;
    }
  } else {
    /* use gray scale */
    printf("using grays\n");
    abortColor = ColorMgr::AllocColor("black");
    suspendColor = abortColor;
    all_busyColor = ColorMgr::AllocColor(.7,.7,.7);
    some_idleColor = ColorMgr::AllocColor(.4,.4,.4);

    Color color0  = ColorMgr::AllocColor(.97,.97,.97);
    Color color1  = ColorMgr::AllocColor(.7,.7,.7);
    Color color2  = ColorMgr::AllocColor(.5,.5,.5);
    Color color3  = ColorMgr::AllocColor(.4,.4,.4);
    Color color4  = ColorMgr::AllocColor(.3,.3,.3);
    Color color5  = ColorMgr::AllocColor(.2,.2,.2);

    for(int i = 0; i < MAX_COLORS; i++) {
      colorArray0[i] = color0;
      colorArray1[i] = color1;
      colorArray2[i] = color2;
      colorArray3[i] = color3;
      colorArray4[i] = color4;
      colorArray5[i] = color5;
    }
  }

  ctrl->StartSession();
  
  Dispatcher::RunNoReturn();
}
