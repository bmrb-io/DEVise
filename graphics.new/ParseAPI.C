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
  Revision 1.5  1996/05/13 21:57:53  jussi
  Added setBatchMode command.

  Revision 1.4  1996/05/13 18:14:13  jussi
  Changed type of "flag" parameter. Changed code to reflect new
  flag values: API_CMD, API_ACK, API_NAK, API_CTL.

  Revision 1.3  1996/05/11 20:43:52  jussi
  Moved output statement from "exit" command to ServerAPI.c.

  Revision 1.2  1996/05/11 19:08:53  jussi
  Added replica management.

  Revision 1.1  1996/05/11 17:26:26  jussi
  Initial revision. Moved all API parsing to this file so that
  ServerAPI.c and TkControl.c would not have to duplicate it.
*/

#include <stdio.h>
#include <ctype.h>

#include "ParseAPI.h"
#include "ClassDir.h"
#include "Control.h"
#include "ViewKGraph.h"
#include "Util.h"
#include "ParseCat.h"
#include "TData.h"
#include "TDataMap.h"
#include "Parse.h"
#include "GroupDir.h"
#include "ViewLayout.h"
#include "VisualLink.h"
#include "FilterQueue.h"

//#define DEBUG

static char result[10 * 1024];
static ViewKGraph *vkg = 0;

int ParseAPI(int argc, char **argv, ControlPanel *control)
{
  ClassDir *classDir = control->GetClassDir();
  int numArgs;
  char **args;
  
  result[0] = '\0';

#ifdef DEBUG
  for(int i = 0; i < argc; i++)
    printf("\"%s\" ", argv[i]);
  printf("\n");
#endif

  // The first few commands have a variable number of arguments

  if (!strcmp(argv[0], "changeParam")) {
    classDir->ChangeParams(argv[1], argc - 2, &argv[2]);
    control->ReturnVal(API_ACK, "done");
    return 1;
  }

  if (!strcmp(argv[0], "createInterp")) {
    /* This command is supported for backward compatibility only */
    MapInterpClassInfo *interp = control->GetInterpProto();
    ClassInfo *classInfo = interp->CreateWithParams(argc-1, &argv[1]);
    if (!classInfo) {
      control->ReturnVal(API_NAK, "Cannot create mapping");
      return -1;
    }
    ControlPanel::RegisterClass(classInfo, true);
    control->ReturnVal(API_ACK, classInfo->ClassName());
    return 1;
  }

  if (!strcmp(argv[0], "create")) {
    control->SetBusy();
    // HACK to provide backward compatibility
    if (!strcmp(argv[2], "WinVertical") ||
	!strcmp(argv[2], "WinHorizontal"))
      argv[2] = "TileLayout";
    char *name = classDir->CreateWithParams(argv[1], argv[2],
					    argc - 3, &argv[3]);
    control->SetIdle();
    if (!name)
      control->ReturnVal(API_ACK, "");
    else
      control->ReturnVal(API_ACK, name);
    return 1;
  }

  if (!strcmp(argv[0], "showkgraph")) {
    if ((atoi(argv[1]) == 1) || (!vkg))  {
      /* Create a new ViewKGraph object */
      /*
	 Don't delete the previous vkg since we still want it to
	 continuously display the previously defined KGraph
      */
      vkg = new ViewKGraph();
    }
    /* Number of views */
    int nview = argc - 4;
    ViewGraph **vlist = (ViewGraph **)malloc(nview*sizeof(ViewGraph *));
    for(int i = 0; i < nview; i++) {
      vlist[i] = (ViewGraph *)classDir->FindInstance(argv[5 + i]);
      if (!vlist[i]) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
    }
    vkg->Init();
    /* Add these to the ViewKGraph class and display */
    if (vkg->AddViews(vlist, nview, argv[3]) == false) {
      control->ReturnVal(API_NAK, "Could not add views to ViewKGraph");
      return -1;
    }
    
    if (vkg->Display(atoi(argv[2])) == false) {
      control->ReturnVal(API_NAK, "Could not display the KGraph");
      return -1;
    }
    free(vlist);
    control->ReturnVal(API_ACK, "done");
    return 1;
  }

  if (!strcmp(argv[0], "createMappingClass")) {
    MapInterpClassInfo *interp = control->GetInterpProto();
    ClassInfo *classInfo = interp->CreateWithParams(argc-1,&argv[1]);
    if (!classInfo) {
      control->ReturnVal(API_NAK, "Cannot create mapping class");
      return -1;
    }
    ControlPanel::RegisterClass(classInfo, true);
    control->ReturnVal(API_ACK,classInfo->ClassName());
    return 1;
  }

  if (argc == 1) {
    if (!strcmp(argv[0], "date")) {
      time_t tm = time((time_t *)0);
      control->ReturnVal(API_ACK,DateString(tm));
      return 1;
    }
    if (!strcmp(argv[0], "printDispatcher")) {
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "catFiles")) {
      CatFiles(numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "exit")) {
      control->ReturnVal(API_ACK, "done");
      control->RestartSession();
      return 1;
    }
    if (!strcmp(argv[0], "clearAll")) {
      control->DestroySessionData();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }

  if (argc == 2) {
    if (!strcmp(argv[0], "setBatchMode")) {
      Boolean batch = (atoi(argv[1]) ? true : false);
      control->SetBatchMode(batch);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "invalidatePixmap")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      vg->InvalidatePixmaps();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "createMappingClass")) {
      MapInterpClassInfo *interp = control->GetInterpProto();
      ClassInfo *classInfo = interp->CreateWithParams(argc-1,&argv[1]);
      if (!classInfo) {
	control->ReturnVal(API_NAK, "Cannot create mapping class");
	return -1;
      }
      ControlPanel::RegisterClass(classInfo, true);
      control->ReturnVal(API_ACK,classInfo->ClassName());
      return 1;
    }
    if (!strcmp(argv[0], "readLine")) {
      FILE *file = (FILE *)atol(argv[1]);
      (void)fgets(result, 256, file);
      int len = strlen(result);
      if (len > 0 && result[len - 1] == '\n')
	result[len - 1] = '\0';
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "close")) {
      FILE *file = (FILE *)atol(argv[1]);
      fclose(file);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "isMapped")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      sprintf(result, "%d", (vg->Mapped() ? 1 : 0 ));
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getLabel")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Boolean occupyTop;
      int extent;
      char *name;
      vg->GetLabelParam(occupyTop, extent, name);
      sprintf(result, "%d %d {%s}", (occupyTop ? 1 : 0), extent,
	      (name ? name : ""));
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "tdataFileName")) {
      TData *tdata = (TData *)classDir->FindInstance(argv[1]);
      if (!tdata) {
	control->ReturnVal(API_NAK, "Cannot find tdata");
	return -1;
      }
      control->ReturnVal(API_ACK,tdata->GetName());
      return 1;
    }
    if (!strcmp(argv[0], "getViewWin")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      ViewWin *viewWin = view->GetParent();
      if (!viewWin)
	control->ReturnVal(API_ACK, "");
      else
	control->ReturnVal(API_ACK,viewWin->GetName());
      return 1;
    }
    if (!strcmp(argv[0], "clearViewHistory")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find cursor");
	return -1;
      }
      FilterQueue *queue = view->GetHistory();
      queue->Clear();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getCursorViews")) {
      DeviseCursor *cursor = (DeviseCursor *)classDir->FindInstance(argv[1]);
      if (!cursor) {
	control->ReturnVal(API_NAK, "Cannot find cursor");
	return -1;
      }
      View *src = cursor->GetSource();
      View *dst = cursor->GetDst();
      char *name[2];
      if (src)
	name[0] = src->GetName();
      else
	name[0] = "";
      if (dst)
	name[1] = dst->GetName();
      else
	name[1] = "";
      control->ReturnVal(2, name);
      return 1;
    } 
    if (!strcmp(argv[0], "getMappingTData")) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      TData *tdata = map->GetTData();
      control->ReturnVal(API_ACK,classDir->FindInstanceName(tdata));
      return 1;
    }
    if (!strcmp(argv[0], "destroy")) {
      classDir->DestroyInstance(argv[1]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "parseDateFloat")) {
      double val;
      (void)ParseFloatDate(argv[1], val);
      sprintf(result, "%f", val);
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "isInterpretedGData")) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      if (map->IsInterpreted())
	strcpy(result, "1");
      else
	strcpy(result, "0");
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "isInterpreted")) {
      int *isInterp = (int *)classDir->UserInfo("mapping", argv[1]);
      strcpy(result, "0");
      if (isInterp && *isInterp)
	strcpy(result, "1");
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getPixelWidth")) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      sprintf(result, "%d", map->GetPixelWidth());
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getTopGroups")) {
      control->GetGroupDir()->top_level_groups(result, argv[1]);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getWindowLayout")) {
      ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
      if (!layout) {
	control->ReturnVal(API_NAK, "Cannot find window");
	return -1;
      }
      int v, h;
      Boolean stacked;
      layout->GetPreferredLayout(v, h, stacked);
      sprintf(result, "%d %d %d", v, h, (stacked ? 1 : 0));
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getSchema")) {
      TData *tdata = (TData *)classDir->FindInstance(argv[1]);
      if (!tdata) {
	control->ReturnVal(API_NAK, "Cannot find tdata");
	return -1;
      }
      AttrList *attrList = tdata->GetAttrList();
      if (!attrList) {
	control->ReturnVal(API_NAK, "Null attribute list");
	return -1;
      }
      
#ifdef DEBUG
      printf("getSchema: \n");
      attrList->Print();
#endif
      int numAttrs = attrList->NumAttrs();
      args = new (char *) [numAttrs + 1];
      args[0] = new char[25];
      strcpy(args[0], "recId int 1 0 0 0 0");
      
      for(int i = 0; i < numAttrs; i++) {
	char attrBuf[160];
	AttrInfo *info = attrList->Get(i);
#ifdef DEBUG
	printf("inserting %s\n", info->name);
#endif
	switch(info->type) {
	case FloatAttr:
	  sprintf(attrBuf, "%s float %d %d %g %d %g",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (info->hasHiVal ? info->hiVal.floatVal : 100),
		  info->hasLoVal,
		  (info->hasLoVal ? info->loVal.floatVal : 0));
	  break;
	case DoubleAttr:
	  sprintf(attrBuf, "%s double %d %d %g %d %g",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (info->hasHiVal ? info->hiVal.doubleVal : 100),
		  info->hasLoVal,
		  (info->hasLoVal ? info->loVal.doubleVal : 0));
	  break;
	case StringAttr:
	  sprintf(attrBuf, "%s string %d 0 0 0 0", info->name,
		  info->isSorted);
	  break;
	case IntAttr:
	  sprintf(attrBuf, "%s int %d %d %ld %d %ld",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (long)(info->hasHiVal ? info->hiVal.intVal : 100),
		  info->hasLoVal,
		  (long)(info->hasLoVal ? info->loVal.intVal : 0));
	  break;
	case DateAttr:
	  sprintf(attrBuf, "%s date %d %d %ld %d %ld",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (long)(info->hasHiVal ? info->hiVal.dateVal : 100),
		  info->hasLoVal,
		  (long)(info->hasLoVal ? info->loVal.dateVal : 0));
	  break;
	default:
	  printf("Unknown attribute type\n");
	  Exit::DoExit(1);
	}
	args[i + 1] = new char [strlen(attrBuf) + 1];
	strcpy(args[i + 1], attrBuf);
      }
      control->ReturnVal(numAttrs + 1, args) ;
      for(int i = 0; i < numAttrs + 1; i++)
	delete args[i];
      delete args;
      return 1;
    }
    if (!strcmp(argv[0], "getAction")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Action *action = view->GetAction();
      if (!action || !strcmp(action->GetName(), "") ||
	  !strcmp(action->GetName(), "default"))
	strcpy(result , "");
      else
	strcpy(result,action->GetName());
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getLinkFlag")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      sprintf(result, "%d",link->GetFlag());
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "importFileType")) {
      char *name = ParseCat(argv[1]);
      if (!name) {
	strcpy(result , "");
	control->ReturnVal(API_NAK, result);
	return -1;
      }
      control->ReturnVal(API_ACK, name);
      return 1;
    }
    if (!strcmp(argv[0], "changeableParam")) {
      Boolean changeable = classDir->Changeable(argv[1]);
      if (changeable)
	strcpy(result, "1");
      else
	strcpy(result, "0");
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getInstParam")) {
      classDir->GetParams(argv[1], numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "tcheckpoint")) {
      TData *tdata;
      control->SetBusy();
      tdata = (TData *)classDir->FindInstance(argv[1]);
      if (tdata)
	tdata->Checkpoint();
      control->SetIdle();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "get")) {
      classDir->ClassNames(argv[1],numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "changeMode")) {
      if (!strcmp(argv[1], "0")) {
	if(control->GetMode() != ControlPanel::DisplayMode) {
	  /* Set display mode  and make all views refresh*/
	  control->SetMode(ControlPanel::DisplayMode);
	  ControlPanel::Instance()->ReportModeChange(control->GetMode());
	}
      } else if (control->GetMode() != ControlPanel::LayoutMode) {
	/* set layout mode */
	control->SetMode(ControlPanel::LayoutMode);
	ControlPanel::Instance()->ReportModeChange(control->GetMode());
      }
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "exists")) {
      if (!classDir->FindInstance(argv[1]))
	strcpy(result, "0");
      else
	strcpy(result, "1");
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "removeView")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      if (!view->Mapped()) {
	control->ReturnVal(API_NAK, "View not in any window");
	return -1;
      }
      view->DeleteFromParent();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getViewMappings")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      int index = view->InitMappingIterator();
      while(view->MoreMapping(index)) {
	TDataMap *map = view->NextMapping(index)->map;
	strcat(result, "{");
	strcat(result, map->GetGDataName());
	strcat(result, "} ");
      }
      view->DoneMappingIterator(index);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "refreshView")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      view->Refresh();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getWinViews")) {
      ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
      if (!win) {
	control->ReturnVal(API_NAK, "Cannot find window");
	return -1;
      }
      int index;
      for(index = win->InitIterator(); win->More(index); ) {
	ViewWin *view = (ViewWin *)win->Next(index);
	strcat(result, "{");
	strcat(result, view->GetName());
	strcat(result, "} ");
      }
      win->DoneIterator(index);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getLinkViews")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      int index;
      for(index = link->InitIterator(); link->More(index); ) {
	ViewWin *view = (ViewWin *)link->Next(index);
	strcat(result, "{");
	strcat(result, view->GetName());
	strcat(result, "} ");
      }
      link->DoneIterator(index);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getCurVisualFilter")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      VisualFilter *filter = view->GetVisualFilter();
      sprintf(result, "%.2f %.2f %.2f %.2f", filter->xLow, filter->yLow,
	      filter->xHigh, filter->yHigh);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getVisualFilters")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      FilterQueue *queue ;
      queue = view->GetHistory();
      int i;
      for(i = 0; i < queue->Size(); i++) {
	VisualFilter filter;
	queue->Get(i,filter);
	char buf[256];
	char xLowBuf[40], xHighBuf[40], yLowBuf[40], yHighBuf[40];
	if (view->GetXAxisAttrType() == DateAttr) {
	  sprintf(xLowBuf, "%s",DateString(filter.xLow));
	  sprintf(xHighBuf, "%s",DateString(filter.xHigh));
	} else {
	  sprintf(xLowBuf, "%.2f",filter.xLow);
	  sprintf(xHighBuf, "%.2f",filter.xHigh);
	}
	
	if (view->GetYAxisAttrType() == DateAttr) {
	  sprintf(yLowBuf, "%s",DateString(filter.yLow));
	  sprintf(yHighBuf, "%s",DateString(filter.yHigh));
	} else {
	  sprintf(yLowBuf, "%.2f",filter.yLow);
	  sprintf(yHighBuf, "%.2f",filter.yHigh);
	}
	sprintf(buf, "{{%s} {%s} {%s} {%s} %d} ",xLowBuf, yLowBuf,
		xHighBuf, yHighBuf, filter.marked);
	strcat(result, buf);
      }
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewStatistics")) {
      ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return status of statistics display */
      control->ReturnVal(API_ACK,vg->GetDisplayStats());
      return 1;
    }
    if (!strcmp(argv[0], "getViewDimensions")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return status of statistics display */
      sprintf(result, "%d", vg->GetNumDimensions());
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewOverrideColor")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Boolean active;
      Color color = view->GetOverrideColor(active);
      sprintf(result, "%d %d", (active ? 1 : 0),
	      (int)color);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "raiseView")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      view->Raise();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "lowerView")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      view->Lower();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }

  if (argc == 3) {
    if (!strcmp(argv[0], "addReplicaServer")) {
      if (control->AddReplica(argv[1], atoi(argv[2])) < 0)
	fprintf(stderr, "Could not add %s:%d as a replica.\n", argv[1],
		atoi(argv[2]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "removeReplicaServer")) {
      if (control->RemoveReplica(argv[1], atoi(argv[2])) < 0)
	fprintf(stderr, "Could not remove replica %s:%d.\n", argv[1],
		atoi(argv[2]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "writeLine")) {
      FILE *file = (FILE *)atol(argv[2]);
      fputs(argv[1],file);
      fputs("\n",file);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "open")) {
      FILE *file = fopen(argv[1],argv[2]);
      if (!file) {
	control->ReturnVal(API_NAK, "Cannot open file");
	return -1;
      }
      sprintf(result, "%ld", (long)file);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "setViewStatistics")) {
      ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Turn on/off display of statistics */
      vg->SetDisplayStats(argv[2]);
      control->ReturnVal(API_ACK, "API_ACK");
      return 1;
    }
    if (!strcmp(argv[0], "setViewDimensions")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Turn on/off display of statistics */
      vg->SetNumDimensions(atoi(argv[2]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "savePixmap")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      FILE *file = (FILE *)atol(argv[2]);
      vg->SavePixmaps(file);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "loadPixmap")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      FILE *file = (FILE *)atol(argv[2]);
      vg->LoadPixmaps(file);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getAxisDisplay")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Boolean stat, xAxis, yAxis;
      vg->AxisDisplay(xAxis, yAxis);
      if (!strcmp(argv[2], "X"))
	stat = xAxis;
      else
	stat = yAxis;
      if (stat)
	strcpy(result , "1");
      else
	strcpy(result , "0");
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "replaceView")) {
      View *view1 = (View *)classDir->FindInstance(argv[1]);
      View *view2 = (View *)classDir->FindInstance(argv[2]);
      if (!view1 || !view2) {
	control->ReturnVal(API_NAK, "TkControl:Cannot find view for replaceView");
	return -1;
      }
      ViewWin *win = view1->GetParent();
      if (!win) {
	control->ReturnVal(API_NAK, "TkControl:Cannot find window for replaceView");
	return -1;
      }
      win->Replace(view1, view2);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setCursorSrc")) {
      DeviseCursor *cursor = (DeviseCursor *)
	classDir->FindInstance(argv[1]);
      if (!cursor) {
	control->ReturnVal(API_NAK, "Cannot find cursor");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      cursor->SetSource(view);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setCursorDst")) {
      DeviseCursor *cursor = (DeviseCursor *)
	classDir->FindInstance(argv[1]);
      if (!cursor) {
	control->ReturnVal(API_NAK, "Cannot find cursor");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      cursor->SetDst(view);
      control->ReturnVal(API_ACK, "done");
      return 1;
    } 
    if (!strcmp(argv[0], "setPixelWidth")) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      int width = atoi(argv[2]);
      if (width > 0 )
	map->SetPixelWidth(width);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getAxis")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      AxisLabel *label;
      if (!strcmp(argv[2], "x"))
	label = view->GetXAxisLabel();
      else
	label = view->GetYAxisLabel();
      
      if (!label)
	control->ReturnVal(API_ACK, "");
      else
	control->ReturnVal(API_ACK,label->GetName());
      return 1;
    }
    if (!strcmp(argv[0], "setAction")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Action *action = (Action*)classDir->FindInstance(argv[2]);
      if (!action) {
	control->ReturnVal(API_NAK, "Cannot find action");
	return -1;
      }
      view->SetAction(action);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setLinkFlag")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      VisualFlag flag = atoi(argv[2]);
      link->ChangeFlag(flag);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "highlightView")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot highlight view");
	return -1;
      }
      view->Highlight(atoi(argv[2]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "get")) {
      classDir->InstanceNames(argv[1],argv[2], numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "getparam")) {
      classDir->GetParams(argv[1],argv[2],numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "insertMapping")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      view->InsertMapping(map);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getMappingLegend")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      control->ReturnVal(API_ACK,view->GetMappingLegend(map));
      return 1;
    }
    if (!strcmp(argv[0], "insertLink")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      link->InsertView(view);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "viewInLink")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      if (link->ViewInLink(view))
	strcpy(result, "1");
      else
	strcpy(result , "0");
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "unlinkView")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      link->DeleteView(view);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "insertWindow")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      ViewWin *win = (ViewWin *)classDir->FindInstance(argv[2]);
      if (!win) {
	control->ReturnVal(API_NAK, "Cannot find window");
	return -1;
      }
      view->DeleteFromParent();
      view->AppendToParent(win);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "removeMapping")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      view->RemoveMapping(map);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }

  if (argc == 4) {
    if (!strcmp(argv[0], "markViewFilter")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      int index = atoi(argv[2]);
      Boolean mark = atoi(argv[3]);
      view->Mark(index, mark);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setAxis")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      AxisLabel *label = (AxisLabel *)classDir->FindInstance(argv[2]);
      if (!label) {
	control->ReturnVal(API_NAK, "Cannot find label");
	return -1;
      }
      if (!strcmp(argv[3], "x"))
	view->SetXAxisLabel(label);
      else
	view->SetYAxisLabel(label);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setDefault")) {
      classDir->SetDefault(argv[1],argv[2],argc-3,&argv[3]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "swapView")) {
      if (argc != 4) {
	control->ReturnVal(API_NAK, "Invalid number of parameters");
	return -1;
      }
      ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[1]);
      View *view1 = (View *)classDir->FindInstance(argv[2]);
      View *view2 = (View *)classDir->FindInstance(argv[3]);
      if (!viewWin || !view1 || !view2) {
	control->ReturnVal(API_NAK, "Cannot find view or window");
	return -1;
      }
      if (view1->GetParent() != viewWin || view2->GetParent() != viewWin) {
	control->ReturnVal(API_NAK, "Views not in same window");
	return -1;
      }
      viewWin->SwapChildren(view1, view2);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setAxisDisplay")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Boolean stat = atoi(argv[3]);
      if (!strcmp(argv[2], "X"))
	vg->XAxisDisplayOnOff(stat);
      else
	vg->YAxisDisplayOnOff(stat);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "getCreateParam")) {
      classDir->CreateParams(argv[1],argv[2],argv[3],numArgs,args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "getItems")) {
      control->GetGroupDir()->get_items(result, argv[1], argv[2], argv[3]);
      control->ReturnVal(API_ACK,result);
      return 1;
    }
    if (!strcmp(argv[0], "setWindowLayout")) {
      ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
      if (!layout) {
	control->ReturnVal(API_NAK, "Cannot find window");
	return -1;
      }
      layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "saveWindowImage")) {
      DisplayExportFormat format = POSTSCRIPT;
      if (!strcmp(argv[1], "eps"))
	format = EPS;
      else if (!strcmp(argv[1], "gif"))
	format = GIF;
      ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
      if (!viewWin) {
	control->ReturnVal(API_NAK, "Cannot find window");
	return -1;
      }
      viewWin->GetWindowRep()->ExportImage(format, argv[3]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setViewOverrideColor")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Boolean active = (atoi(argv[2]) == 1);
      view->SetOverrideColor(atoi(argv[3]), active);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "insertMapping")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      view->InsertMapping(map, argv[3]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
	if (!strcmp(argv[0], "parseSchema"))
	{
	  char *name = ParseSchema(argv[1], argv[2], argv[3]);
	  if (name == NULL)
	  {
		control->ReturnVal(API_NAK, "Cannot parse schema(s)");
		return -1;
	  }
      control->ReturnVal(API_ACK, name);
      return 1;
	}

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }

  if (argc == 5) {
    if (!strcmp(argv[0], "setLabel")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      view->SetLabelParam(atoi(argv[2]), atoi(argv[3]), argv[4]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setWindowLayout")) {
      ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
      if (!layout) {
	control->ReturnVal(API_NAK, "Cannot find window");
	return -1;
      }
      layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]),
				 (atoi(argv[4]) ? true : false));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }

  if (argc == 6) {
    if (!strcmp(argv[0], "setFilter")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      VisualFilter filter;
      view->GetVisualFilter(filter);
      if (!ParseFloatDate(argv[2],filter.xLow) ||
	  !ParseFloatDate(argv[3], filter.yLow) ||
	  !ParseFloatDate(argv[4], filter.xHigh) ||
	  !ParseFloatDate(argv[5], filter.yHigh)) {
	control->ReturnVal(API_NAK, "invalid date or float");
	return -1;
      }
      view->SetVisualFilter(filter);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }
  
  if (argc == 7) {
    if (!strcmp(argv[0], "insertViewHistory")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      VisualFilter filter;
      (void)ParseFloatDate(argv[2],filter.xLow); 
      (void)ParseFloatDate(argv[3],filter.yLow);
      (void)ParseFloatDate(argv[4],filter.xHigh); 
      (void)ParseFloatDate(argv[5],filter.yHigh);
      filter.marked = atoi(argv[6]);
      view->InsertHistory(filter);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

    control->ReturnVal(API_NAK, "wrong args");
    return -1;
  }

  control->ReturnVal(API_NAK, "wrong args");
  return -1;
}

