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
  Revision 1.2  1996/05/11 01:53:56  jussi
  Condensed the code by removing some unnecessary Tcl/Tk parts
  which are not used by the server. Changed the client-server
  protocol somewhat.
*/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "ServerAPI.h"
#include "ClassDir.h"
#include "ViewGraph.h"
#include "View.h"
#include "TData.h"
#include "TDataMap.h"
#include "Init.h"
#include "Util.h"
#include "VisualArg.h"
#include "VisualLink.h"
#include "FilterQueue.h"
#include "ParseCat.h"
#include "Action.h"
#include "AttrList.h"
#include "MapInterpClassInfo.h"
#include "Parse.h"
#include "QueryProc.h"
#include "Cursor.h"
#include "Group.h"
#include "GroupDir.h"
#include "ViewLayout.h"
#include "ViewKGraph.h"

//#define SERV_ANYPORT
//#define DEBUG
//#define DEBUG9

extern GroupDir *gdir;
ViewKGraph *vkg = NULL;

ControlPanel::Mode ServerAPI::_mode = ControlPanel::DisplayMode;
MapInterpClassInfo *ServerAPI::_interpProto = NULL;

ControlPanel *GetTkControl()
{
  return new ServerAPI();
}

ServerAPI::ServerAPI()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");

  ControlPanel::_controlPanel = (ControlPanel *)this;
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);
	
  _busy = false;
  _listenFd = _socketFd = _controlFd = -1;

  GotoDisconnectedMode();
}

void ServerAPI::StartSession()
{
}

ControlPanel::Mode ServerAPI::GetMode()
{
  return _mode;
}

void ServerAPI::DoAbort(char *reason)
{
  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  SendControl(OK, reason);
  fprintf(stderr, "Server aborts.\n");
  exit(0);
}

void ServerAPI::DestroyClientData()
{
#ifdef DEBUG
  printf("Destroying client data\n");
#endif

  ClassDir *classDir = GetClassDir();

  // destroy tdata, gdata, cursor, view link, win, axis, action
  classDir->DestroyAllInstances();

  // clearQP
  classDir->DestroyTransientClasses();
  QueryProc::Instance()->ClearQueries();
  ClearCats();

  // clearTopGroups
  delete gdir;
  gdir = new GroupDir();
}

int ServerAPI::ControlCmd(int argc, char *argv[])
{
  ServerAPI *control = this;
  ClassDir *classDir = GetClassDir();
  int code;
  int numArgs; char **args;
  char *name;
  char result[10 * 1024];
  
  result[0] = '\0';

#ifdef DEBUG
  printf("%s (%d)",argv[0],argc);
  for(int i = 0; i < argc; i++)
    printf("<%s>",argv[i]);
  printf("\n");
#endif

  // The first few commands have variable number of arguments

  if (strcmp(argv[0],"changeParam") == 0) {
    classDir->ChangeParams(argv[1],argc-2,&argv[2]);
    SendClient(OK,"done");
    return 1;
  }

  if (strcmp(argv[0],"createInterp") == 0) {
    /* This command is supported for backward compatibility only */
    ClassInfo *classInfo = _interpProto->CreateWithParams(argc-1,&argv[1]);
    if (!classInfo) {
      SendClient(ERROR,"Cannot create mapping");
      return -1;
    }
    RegisterClass(classInfo,true);
    SendClient(OK,classInfo->ClassName());
    return 1;
  }

  if (strcmp(argv[0],"create") == 0) {
    control->SetBusy();
    // HACK to provide backward compatibility
    if (!strcmp(argv[2], "WinVertical") ||
	!strcmp(argv[2], "WinHorizontal"))
      argv[2] = "TileLayout";
    char *name = classDir->CreateWithParams(argv[1],argv[2],
					    argc-3, &argv[3]);
    control->SetIdle();
    if (!name)
      SendClient(OK,"");
    else
      SendClient(OK,name);
    return 1;
  }

  if (strcmp(argv[0], "showkgraph") == 0) {
    if ((atoi(argv[1]) == 1) || (!vkg))  {
      /* Create a new ViewKGraph object */
      /* Dont delete the previous vkg since we still want it to
	 continuously display the previously defined KGraph */
      /* Who eventually deletes it ?? */
      /* delete vkg; */
      vkg = new ViewKGraph();
    }
    int i = 0;
    /* Number of views */
    int nview = argc - 4;
    ViewGraph **vlist = (ViewGraph **)malloc(nview*sizeof(ViewGraph *));
    for (i = 0; i < nview; i++) {
      vlist[i] = (ViewGraph *)classDir->FindInstance(argv[5+i]);
      if (vlist[i] == NULL) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
    }
    vkg->Init();
    /* Add these to the ViewKGraph class and display */
    if (vkg->AddViews(vlist, nview, argv[3]) == false) {
      SendClient(ERROR,"Could not add views to ViewKGraph");
      return -1;
    }
    
    if (vkg->Display(atoi(argv[2])) == false) {
      SendClient(ERROR,"Could not display the KGraph");
      return -1;
    }
    free(vlist);
    SendClient(OK,"done");
    return 1;
  }

  if (strcmp(argv[0],"createMappingClass") == 0) {
    ClassInfo *classInfo = _interpProto->CreateWithParams(argc-1,&argv[1]);
    if (!classInfo) {
      SendClient(ERROR,"Cannot create mapping class");
      return -1;
    }
    RegisterClass(classInfo, true);
    SendClient(OK,classInfo->ClassName());
    return 1;
  }

  if (argc == 1) {
    if (strcmp(argv[0],"date") == 0) {
      time_t tm = time((time_t *)0);
      SendClient(OK,DateString(tm));
      return 1;
    }
    if (strcmp(argv[0],"printDispatcher") == 0) {
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"catFiles") == 0) {
      CatFiles(numArgs, args);
      SendClient(numArgs, args);
      return 1;
    }
    if (strcmp(argv[0],"exit")== 0) {
      printf("Client exits. Closing client connection.\n");
      DestroyClientData();
      SendClient(OK, "done");
      GotoDisconnectedMode();
      return 1;
    }
    if (strcmp(argv[0],"clearAll")== 0) {
      DestroyClientData();
      SendClient(OK, "done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }

  if (argc == 2) {
    if (strcmp(argv[0],"invalidatePixmap") == 0 ) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (vg == NULL) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      vg->InvalidatePixmaps();
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"createMappingClass") == 0) {
      ClassInfo *classInfo = _interpProto->CreateWithParams(argc-1,&argv[1]);
      if (!classInfo) {
	SendClient(ERROR,"Cannot create mapping class");
	return -1;
      }
      RegisterClass(classInfo, true);
      SendClient(OK,classInfo->ClassName());
      return 1;
    }
    if (strcmp(argv[0],"readLine") == 0 ) {
      FILE *file = (FILE *)atol(argv[1]);
      (void)fgets(result, 256, file);
      int len = strlen(result);
      if (len > 0 && result[len-1] == '\n')
	result[len-1] = '\0';
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"close") == 0) {
      FILE *file = (FILE *)atol(argv[1]);
      fclose(file);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"isMapped") == 0) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      sprintf(result,"%d",(vg->Mapped()? 1 : 0 ));
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getLabel") == 0) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Boolean occupyTop;
      int extent;
      char *name;
      vg->GetLabelParam(occupyTop, extent, name);
      sprintf(result, "%d %d {%s}", (occupyTop ? 1 : 0), extent,
	      (name ? name : ""));
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"tdataFileName") == 0) {
      TData *tdata = (TData *)classDir->FindInstance(argv[1]);
      if (!tdata) {
	SendClient(ERROR,"Cannot find tdata");
	return -1;
      }
      SendClient(OK,tdata->GetName());
      return 1;
    }
    if (strcmp(argv[0],"getViewWin") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      ViewWin *viewWin = view->GetParent();
      if (!viewWin)
	SendClient(OK,"");
      else
	SendClient(OK,viewWin->GetName());
      return 1;
    }
    if (strcmp(argv[0],"clearViewHistory") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find cursor");
	return -1;
      }
      FilterQueue *queue = view->GetHistory();
      queue->Clear();
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getCursorViews") == 0) {
      DeviseCursor *cursor = (DeviseCursor *)
	classDir->FindInstance(argv[1]);
      if (cursor == NULL) {
	SendClient(ERROR,"Cannot find cursor");
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
      SendClient(2,name);
      return 1;
    } 
    if (strcmp(argv[0],"getMappingTData") == 0) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      TData *tdata = map->GetTData();
      SendClient(OK,classDir->FindInstanceName(tdata));
      return 1;
    }
    if (strcmp(argv[0],"destroy") == 0 ) {
      classDir->DestroyInstance(argv[1]);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"parseDateFloat") == 0 ) {
      char buf[80];
      double val;
      (void)ParseFloatDate(argv[1], val);
      sprintf(buf,"%f",val);
      SendClient(OK,buf);
      return 1;
    }
    if (strcmp(argv[0],"isInterpretedGData") == 0 ) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      if (map->IsInterpreted())
	strcpy(result, "1");
      else
	strcpy(result, "0");
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"isInterpreted") == 0 ) {
      int *isInterp = (int *)classDir->UserInfo("mapping", argv[1]);
      strcpy(result, "0");
      if (isInterp && *isInterp)
	strcpy(result, "1");
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getPixelWidth") == 0 ) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      sprintf(result,"%d", map->GetPixelWidth());
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0], "getTopGroups") == 0) {
      gdir->top_level_groups(result, argv[1]);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0], "getWindowLayout") == 0) {
      ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
      if (!layout) {
	SendClient(ERROR,"Cannot find window");
	return -1;
      }
      int v, h;
      Boolean stacked;
      layout->GetPreferredLayout(v, h, stacked);
      sprintf(result, "%d %d %d", v, h, (stacked ? 1 : 0));
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getSchema") == 0) {
      char **args;
      TData *tdata = (TData *)classDir->FindInstance(argv[1]);
      if (!tdata) {
	SendClient(ERROR,"Cannot find tdata");
	return -1;
      }
      AttrList *attrList = tdata->GetAttrList();
      if (!attrList) {
	SendClient(ERROR,"Null attribute list");
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
	  sprintf(attrBuf,"%s float %d %d %g %d %g",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (info->hasHiVal ? info->hiVal.floatVal : 100),
		  info->hasLoVal,
		  (info->hasLoVal ? info->loVal.floatVal : 0));
	  break;
	case DoubleAttr:
	  sprintf(attrBuf,"%s double %d %d %g %d %g",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (info->hasHiVal ? info->hiVal.doubleVal : 100),
		  info->hasLoVal,
		  (info->hasLoVal ? info->loVal.doubleVal : 0));
	  break;
	case StringAttr:
	  sprintf(attrBuf,"%s string %d 0 0 0 0", info->name,
		  info->isSorted);
	  break;
	case IntAttr:
	  sprintf(attrBuf,"%s int %d %d %ld %d %ld",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (long)(info->hasHiVal ? info->hiVal.intVal : 100),
		  info->hasLoVal,
		  (long)(info->hasLoVal ? info->loVal.intVal : 0));
	  break;
	case DateAttr:
	  sprintf(attrBuf,"%s date %d %d %ld %d %ld",
		  info->name, info->isSorted,
		  info->hasHiVal,
		  (long)(info->hasHiVal ? info->hiVal.dateVal : 100),
		  info->hasLoVal,
		  (long)(info->hasLoVal ? info->loVal.dateVal : 0));
	  break;
	default:
	  printf("TkControl:unknown attr\n");
	  Exit::DoExit(1);
	}
	args[i + 1] = new char [strlen(attrBuf) + 1];
	strcpy(args[i + 1], attrBuf);
      }
      SendClient(numAttrs + 1, args) ;
      for(int i = 0; i < numAttrs + 1; i++)
	delete args[i];
      delete args;
      return 1;
    }
    if (strcmp(argv[0],"getAction") == 0 ) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Action *action = view->GetAction();
      if (!action || strcmp(action->GetName(),"") == 0 ||
	  strcmp(action->GetName(),"default") == 0)
	strcpy(result , "");
      else
	strcpy(result,action->GetName());
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getLinkFlag") == 0 ) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	SendClient(ERROR,"Cannot find link");
	return -1;
      }
      sprintf(result,"%d",link->GetFlag());
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"importFileType") == 0) {
      if (!(name = ParseCat(argv[1]))) {
	strcpy(result , "");
	SendClient(ERROR,result);
	return -1;
      }
      SendClient(OK,name);
      return 1;
    }
    if (strcmp(argv[0],"changeableParam") == 0) {
      Boolean changeable = classDir->Changeable(argv[1]);
      if (changeable)
	strcpy(result, "1");
      else
	strcpy(result, "0");
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getInstParam") == 0) {
      classDir->GetParams(argv[1], numArgs, args);
      SendClient(numArgs, args);
      return 1;
    }
    if (strcmp(argv[0],"tcheckpoint") == 0) {
      TData *tdata;
      control->SetBusy();
      tdata = (TData *)classDir->FindInstance(argv[1]);
      if (tdata)
	tdata->Checkpoint();
      control->SetIdle();
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"get") == 0) {
      classDir->ClassNames(argv[1],numArgs, args);
      SendClient(numArgs, args);
      return 1;
    }
    if (strcmp(argv[0],"changeMode") == 0) {
      if (!strcmp(argv[1],"0")) {
	if( _mode != ControlPanel::DisplayMode) {
	  /* Set display mode  and make all views refresh*/
	  _mode = ControlPanel::DisplayMode;
	  ControlPanel::Instance()->ReportModeChange(_mode);
	}
      } else if(_mode != ControlPanel::LayoutMode) {
	/* set layout mode */
	_mode = ControlPanel::LayoutMode;
	ControlPanel::Instance()->ReportModeChange(_mode);
      }
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"exists") == 0) {
      if (!classDir->FindInstance(argv[1]))
	strcpy(result, "0");
      else
	strcpy(result, "1");
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"removeView") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      if (!view->Mapped()) {
	SendClient(ERROR,"View not in any window");
	return -1;
      }
      view->DeleteFromParent();
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getViewMappings") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      int index = view->InitMappingIterator();
      strcat(result, "{");
      while(view->MoreMapping(index)) {
	TDataMap *map = view->NextMapping(index)->map;
	strcat(result, map->GetGDataName());
	strcat(result, "} ");
      }
      view->DoneMappingIterator(index);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"refreshView") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      view->Refresh();
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getWinViews") == 0) {
      ViewWin *win = (ViewWin*)classDir->FindInstance(argv[1]);
      if (!win) {
	SendClient(ERROR,"Cannot find window");
	return -1;
      }
      int index;
      strcat(result, "{");
      for(index = win->InitIterator(); win->More(index); ) {
	ViewWin *view = (ViewWin *)win->Next(index);
	strcat(result, view->GetName());
	strcat(result, "} ");
      }
      win->DoneIterator(index);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getLinkViews") == 0) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	SendClient(ERROR,"Cannot find link");
	return -1;
      }
      int index;
      strcat(result, "{");
      for(index = link->InitIterator(); link->More(index); ) {
	ViewWin *view = (ViewWin *)link->Next(index);
	strcat(result, view->GetName());
	strcat(result, "} ");
      }
      link->DoneIterator(index);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getCurVisualFilter") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (view == NULL) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      VisualFilter *filter = view->GetVisualFilter();
      sprintf(result,"%.2f %.2f %.2f %.2f", filter->xLow, filter->yLow,
	      filter->xHigh, filter->yHigh);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getVisualFilters") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      FilterQueue *queue ;
      queue = view->GetHistory();
      int i;
      for(i = 0; i < queue->Size(); i++) {
	VisualFilter filter;
	queue->Get(i,filter);
	char buf[256];
	char xLowBuf[40],xHighBuf[40],yLowBuf[40],yHighBuf[40];
	if (view->GetXAxisAttrType() == DateAttr) {
	  sprintf(xLowBuf,"%s",DateString(filter.xLow));
	  sprintf(xHighBuf,"%s",DateString(filter.xHigh));
	} else {
	  sprintf(xLowBuf,"%.2f",filter.xLow);
	  sprintf(xHighBuf,"%.2f",filter.xHigh);
	}
	
	if (view->GetYAxisAttrType() == DateAttr) {
	  sprintf(yLowBuf,"%s",DateString(filter.yLow));
	  sprintf(yHighBuf,"%s",DateString(filter.yHigh));
	} else {
	  sprintf(yLowBuf,"%.2f",filter.yLow);
	  sprintf(yHighBuf,"%.2f",filter.yHigh);
	}
	sprintf(buf,"{{%s} {%s} {%s} {%s} %d} ",xLowBuf, yLowBuf,
		xHighBuf, yHighBuf, filter.marked);
	strcat(result, buf);
      }
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0], "getViewStatistics") == 0) {
      ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      /* Return status of statistics display */
      SendClient(OK,vg->GetDisplayStats());
      return 1;
    }
    if (strcmp(argv[0], "getViewDimensions") == 0) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      /* Return status of statistics display */
      sprintf(result, "%d", vg->GetNumDimensions());
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"getViewOverrideColor") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Boolean active;
      Color color = view->GetOverrideColor(active);
      sprintf(result, "%d %d", (active ? 1 : 0),
	      (int)color);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"raiseView") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (view == NULL) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      view->Raise();
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"lowerView") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      view->Lower();
      SendClient(OK,"done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }

  if (argc == 3) {
    if (strcmp(argv[0],"writeLine") == 0) {
      FILE *file = (FILE *)atol(argv[2]);
      fputs(argv[1],file);
      fputs("\n",file);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"open") == 0) {
      FILE *file = fopen(argv[1],argv[2]);
      if (!file) {
	SendClient(ERROR,"Cannot open file");
	return -1;
      }
      sprintf(result, "%ld", (long)file);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0], "setViewStatistics") == 0) {
      ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      /* Turn on/off display of statistics */
      vg->SetDisplayStats(argv[2]);
      SendClient(OK,"OK");
      return 1;
    }
    if (strcmp(argv[0], "setViewDimensions") == 0) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      /* Turn on/off display of statistics */
      vg->SetNumDimensions(atoi(argv[2]));
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"savePixmap") == 0) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      FILE *file = (FILE *)atol(argv[2]);
      vg->SavePixmaps(file);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"loadPixmap") == 0) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      FILE *file = (FILE *)atol(argv[2]);
      vg->LoadPixmaps(file);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getAxisDisplay") == 0 ) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Boolean stat, xAxis, yAxis;
      vg->AxisDisplay(xAxis, yAxis);
      if (!strcmp(argv[2],"X"))
	stat = xAxis;
      else
	stat = yAxis;
      if (stat)
	strcpy(result , "1");
      else
	strcpy(result , "0");
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"replaceView") == 0) {
      View *view1 = (View *)classDir->FindInstance(argv[1]);
      View *view2 = (View *)classDir->FindInstance(argv[2]);
      if (!view1 || !view2) {
	SendClient(ERROR,"TkControl:Cannot find view for replaceView");
	return -1;
      }
      ViewWin *win = view1->GetParent();
      if (!win) {
	SendClient(ERROR,"TkControl:Cannot find window for replaceView");
	return -1;
      }
      win->Replace(view1, view2);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"setCursorSrc") == 0 ) {
      DeviseCursor *cursor = (DeviseCursor *)
	classDir->FindInstance(argv[1]);
      if (!cursor) {
	SendClient(ERROR,"Cannot find cursor");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      cursor->SetSource(view);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"setCursorDst") == 0) {
      DeviseCursor *cursor = (DeviseCursor *)
	classDir->FindInstance(argv[1]);
      if (!cursor) {
	SendClient(ERROR,"Cannot find cursor");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      cursor->SetDst(view);
      SendClient(OK,"done");
      return 1;
    } 
    if (strcmp(argv[0],"setPixelWidth") == 0 ) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      int width = atoi(argv[2]);
      if (width > 0 )
	map->SetPixelWidth(width);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getAxis") == 0 ) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      AxisLabel *label;
      if (!strcmp(argv[2],"x"))
	label = view->GetXAxisLabel();
      else
	label = view->GetYAxisLabel();
      
      if (!label)
	SendClient(OK,"");
      else
	SendClient(OK,label->GetName());
      return 1;
    }
    if (strcmp(argv[0],"setAction") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Action *action = (Action*)classDir->FindInstance(argv[2]);
      if (!action) {
	SendClient(ERROR,"Cannot find action");
	return -1;
      }
      view->SetAction(action);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"setLinkFlag") == 0 ) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	SendClient(ERROR, "Cannot find link");
	return -1;
      }
      VisualFlag flag = atoi(argv[2]);
      link->ChangeFlag(flag);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0], "highlightView") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot highlight view");
	return -1;
      }
      view->Highlight(atoi(argv[2]));
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"get") == 0) {
      classDir->InstanceNames(argv[1],argv[2], numArgs, args);
      SendClient(numArgs, args);
      return 1;
    }
    if (strcmp(argv[0],"getparam") == 0) {
      classDir->GetParams(argv[1],argv[2],numArgs, args);
      SendClient(numArgs, args);
      return 1;
    }
    if (strcmp(argv[0],"insertMapping") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      view->InsertMapping(map);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getMappingLegend") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      SendClient(OK,view->GetMappingLegend(map));
      return 1;
    }
    if (strcmp(argv[0],"insertLink") == 0) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	SendClient(ERROR,"Cannot find link");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      link->InsertView(view);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"viewInLink") == 0) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	SendClient(ERROR,"Cannot find link");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      if (link->ViewInLink(view))
	strcpy(result, "1");
      else
	strcpy(result , "0");
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0],"unlinkView") == 0) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	SendClient(ERROR,"Cannot find link");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      link->DeleteView(view);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"insertWindow") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      ViewWin *win = (ViewWin *)classDir->FindInstance(argv[2]);
      if (!win) {
	SendClient(ERROR,"Cannot find window");
	return -1;
      }
      view->DeleteFromParent();
      view->AppendToParent(win);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"removeMapping") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      view->RemoveMapping(map);
      SendClient(OK,"done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }

  if (argc == 4) {
    if (strcmp(argv[0],"markViewFilter") == 0 ) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      int index = atoi(argv[2]);
      Boolean mark = atoi(argv[3]);
      view->Mark(index, mark);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"setAxis") == 0 ) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      AxisLabel *label = (AxisLabel *)classDir->FindInstance(argv[2]);
      if (!label) {
	SendClient(ERROR,"Cannot find label");
	return -1;
      }
      if (!strcmp(argv[3],"x"))
	view->SetXAxisLabel(label);
      else
	view->SetYAxisLabel(label);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"setDefault") == 0) {
      classDir->SetDefault(argv[1],argv[2],argc-3,&argv[3]);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"swapView") == 0 ) {
      if (argc != 4) {
	SendClient(ERROR,"Invalid number of parameters");
	return -1;
      }
      ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[1]);
      View *view1 = (View *)classDir->FindInstance(argv[2]);
      View *view2 = (View *)classDir->FindInstance(argv[3]);
      if (!viewWin || !view1 || !view2) {
	SendClient(ERROR,"Cannot find view or window");
	return -1;
      }
      if (view1->GetParent() != viewWin || view2->GetParent() != viewWin) {
	SendClient(ERROR,"Views not in same window");
	return -1;
      }
      viewWin->SwapChildren(view1, view2);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"setAxisDisplay") == 0 ) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Boolean stat = atoi(argv[3]);
      if (!strcmp(argv[2],"X"))
	vg->XAxisDisplayOnOff(stat);
      else
	vg->YAxisDisplayOnOff(stat);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"getCreateParam") == 0) {
      classDir->CreateParams(argv[1],argv[2],argv[3],numArgs,args);
      SendClient(numArgs, args);
      return 1;
    }
    if (strcmp(argv[0], "getItems") == 0) {
      gdir->get_items(result, argv[1], argv[2], argv[3]);
      SendClient(OK,result);
      return 1;
    }
    if (strcmp(argv[0], "setWindowLayout") == 0) {
      ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
      if (!layout) {
	SendClient(ERROR,"Cannot find window");
	return -1;
      }
      layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]));
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0], "saveWindowImage") == 0) {
      DisplayExportFormat format = POSTSCRIPT;
      if (!strcmp(argv[1], "eps"))
	format = EPS;
      else if (!strcmp(argv[1], "gif"))
	format = GIF;
      ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
      if (!viewWin) {
	SendClient(ERROR,"Cannot find window");
	return -1;
      }
      viewWin->GetWindowRep()->ExportImage(format, argv[3]);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0], "setViewOverrideColor") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      Boolean active = (atoi(argv[2]) == 1);
      view->SetOverrideColor(atoi(argv[3]), active);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0],"insertMapping") == 0) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      TDataMap *map = (TDataMap *)classDir->FindInstance(argv[2]);
      if (!map) {
	SendClient(ERROR,"Cannot find mapping");
	return -1;
      }
      view->InsertMapping(map, argv[3]);
      SendClient(OK,"done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }

  if (argc == 5) {
    if (strcmp(argv[0], "setLabel") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      view->SetLabelParam(atoi(argv[2]), atoi(argv[3]), argv[4]);
      SendClient(OK,"done");
      return 1;
    }
    if (strcmp(argv[0], "setWindowLayout") == 0) {
      ViewLayout *layout = (ViewLayout *)classDir->FindInstance(argv[1]);
      if (!layout) {
	SendClient(ERROR,"Cannot find window");
	return -1;
      }
      layout->SetPreferredLayout(atoi(argv[2]), atoi(argv[3]),
				 (atoi(argv[4]) ? true : false));
      SendClient(OK,"done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }

  if (argc == 6) {
    if (strcmp(argv[0],"setFilter") == 0) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      VisualFilter filter;
      view->GetVisualFilter(filter);
      if (!ParseFloatDate(argv[2],filter.xLow) ||
	  !ParseFloatDate(argv[3], filter.yLow) ||
	  !ParseFloatDate(argv[4], filter.xHigh) ||
	  !ParseFloatDate(argv[5], filter.yHigh)) {
	SendClient(ERROR,"invalid date or float");
	return -1;
      }
      view->SetVisualFilter(filter);
      SendClient(OK,"done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }
  
  if (argc == 7) {
    if (strcmp(argv[0],"insertViewHistory") == 0 ) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	SendClient(ERROR,"Cannot find view");
	return -1;
      }
      VisualFilter filter;
      (void)ParseFloatDate(argv[2],filter.xLow); 
      (void)ParseFloatDate(argv[3],filter.yLow);
      (void)ParseFloatDate(argv[4],filter.xHigh); 
      (void)ParseFloatDate(argv[5],filter.yHigh);
      filter.marked = atoi(argv[6]);
      view->InsertHistory(filter);
      SendClient(OK,"done");
      return 1;
    }

    SendClient(ERROR,"wrong args");
    return -1;
  }

  SendClient(ERROR,"wrong args");
  return -1;
}

void ServerAPI::Run()
{
#if DEBUG9
  printf("In ServerAPI::Run\n");
#endif

  DOASSERT((_socketFd < 0 && _controlFd < 0 && _listenFd >= 0) ||
	   (_socketFd >= 0 && _listenFd < 0), "Invalid sockets");

  if (_socketFd >= 0) {
    // We are connected so just keep reading from the socket
    if (ReadSocket() < 0) {
      fprintf(stderr, "Cannot communicate with client. Closing connection.\n");
      DestroyClientData();
      GotoDisconnectedMode();
    }
    return;
  }

  printf("\n");
  printf("Server waiting for client connection.\n");

  int len = sizeof(_client_addr);
  _socketFd = accept(_listenFd, (struct sockaddr *)&_client_addr, &len);
  if (_socketFd < 0) {
    perror("accept");
    if (errno == EINTR) {
      fprintf(stderr, "Server exits.\n");
      exit(1);
    }
    DOASSERT(0, "Error in network interface");
  }
  
  close(_listenFd);
  _listenFd = -1;

  if (GotoConnectedMode() < 0) {
    fprintf(stderr, "Bad client connection. Closing connection.\n");
    GotoDisconnectedMode();
    return;
  }

  Dispatcher::Current()->Unregister(this);
  Dispatcher::Current()->Register(this, 10, AllState, true, _socketFd);
}

int ServerAPI::ReadSocket()
{
  DOASSERT(_socketFd >= 0, "Invalid socket");

  static u_short maxSize = 0;
  static char **buff = 0;

#ifdef DEBUG9
  printf("Getting numElements\n");
#endif

  u_short numElements;
  int result = recv(_socketFd, (char *)&numElements, sizeof numElements, 0);
  if (result < (int)sizeof numElements) {
#ifdef DEBUG9
    perror("recv");
#endif
    if (errno == EAGAIN)
      return 1;
    return -1;
  }

  if (!result) {
#ifdef DEBUG
    printf("End of client data.\n");
#endif
    return -1;
  }

  numElements = ntohs(numElements);

  // set socket to blocking mode

  result = fcntl(_socketFd, F_SETFL, 0);
  if (result < 0) {
    perror("fcntl");
    return -1;
  }
    
#ifdef DEBUG
  printf("\nGot numElements = %d\n", numElements);
#endif

  if (numElements > maxSize) {
    if (buff) 
      delete buff;
    buff = new char * [numElements];
    DOASSERT(buff, "Out of memory");
    maxSize = numElements;
  }

  for(int i = 0; i < numElements;i++) {
#ifdef DEBUG
    printf("Getting size of element %d\n", i);
#endif
    u_short size;
    result = recv(_socketFd, (char *)&size, sizeof size, 0);
    if (result < (int)sizeof size) {
      perror("recv");
      goto error;
    }
    size = ntohs(size);
#ifdef DEBUG
    printf("Getting element %d (%d bytes)\n", i, size);
#endif
    buff[i] = new char [size];
    DOASSERT(buff[i], "Out of memory");
    buff[i][0] = 0;
    if (size > 0) {
      result = recv(_socketFd, buff[i], size, 0);
      if (result < size) {
	perror("recv");
	goto error;
      }
#ifdef DEBUG
      printf("Got element \"%s\"\n", buff[i]);
#endif
    }
  }

#ifdef DEBUG
  printf("Executing command\n");
#endif

  if (ControlCmd(numElements, buff) < 0)
    fprintf(stderr, "Devise API command error\n");

#ifdef DEBUG
  printf("Done executing command\n");
#endif

  for(int i = 0; i < numElements; i++)
    delete buff[i];

  // When client executes an exit command, _socketFd will be
  // closed in ControlCmd()

  if (_socketFd >= 0) {
    // set socket back to non-blocking mode

#ifdef SUN
    result = fcntl(_socketFd, F_SETFL, FNDELAY);
#else
    result = fcntl(_socketFd, F_SETFL, O_NDELAY);
#endif
    if (result < 0) {
      perror("fcntl");
      return -1;
    }
  }

  return 1;

 error:
  // set socket back to non-blocking mode

#ifdef SUN
  result = fcntl(_socketFd, F_SETFL, FNDELAY);
#else
  result = fcntl(_socketFd, F_SETFL, O_NDELAY);
#endif
  if (result < 0)
    perror("fcntl");

  return -1;
}

int ServerAPI::GotoConnectedMode()
{
  DOASSERT(_socketFd >= 0 && _controlFd < 0, "Invalid sockets");

  printf("Setting up client connection.\n");
    
  // we will receive the port number of the clients back channel

  u_short port;
  int result = recv(_socketFd, (char *)&port, sizeof port, 0);
  if (result < (int)sizeof port) {
    perror("recv");
    return -1;
  }
  port = ntohs(port);

  // next we receive the name of the display at the client (DISPLAY)

  u_short size;
  result = recv(_socketFd, (char *)&size, sizeof size, 0);
  if (result < (int)sizeof size) {
    perror("recv");
    return -1;
  }
  size = ntohs(size);

  char displayName[size];
  result = recv(_socketFd, displayName, size, 0);
  if (result < size) {
    perror("recv");
    return -1;
  }
  
#ifdef DEBUG
  printf("Client sent display name: %s\n", displayName);
#endif

  if (port > 0) {
    _controlFd = socket(AF_INET, SOCK_STREAM, 0);
    if (_controlFd < 0) {
      perror("socket");
      return -1;
    }

    struct sockaddr_in backChannel;
    memcpy(&backChannel, &_client_addr, sizeof backChannel);
    backChannel.sin_port = htons(port);
    
#ifdef DEBUG
    printf("Connecting to client port %u\n", port);
#endif

    result = connect(_controlFd, (struct sockaddr *)&backChannel,
		     sizeof(struct sockaddr));
    if (result < 0) {
      perror("connect");
      return -1;
    }
  } else {
#ifdef DEBUG
    printf("No control channel.\n");
#endif
  }

  // now set socket to non-blocking mode

#ifdef SUN
  result = fcntl(_socketFd, F_SETFL, FNDELAY);
#else
  result = fcntl(_socketFd, F_SETFL, O_NDELAY);
#endif
  if (result < 0) {
    perror("fcntl");
    return -1;
  }
    
  printf("Client connection established.\n");
    
  return 1;
}

void ServerAPI::GotoDisconnectedMode()
{
  Dispatcher::Current()->Unregister(this);

  if (_socketFd >= 0) {
    close(_socketFd);
    _socketFd = -1;
  }

  if (_controlFd >= 0) {
    close(_controlFd);
    _controlFd = -1;
  }

  DOASSERT(_listenFd < 0, "Invalid socket");
  _listenFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_listenFd < 0)
    perror("socket");
  DOASSERT(_listenFd >= 0, "Cannot create socket");

  // allow listening port number to be reused
  int enable = 1;
  int result = setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR,
		      (char *)&enable, sizeof enable);
  if (result < 0)
    perror("setsockopt");
  DOASSERT(result >= 0, "Cannot set socket options");

  // Now bind these to the address..

  struct sockaddr_in servAddr;
  memset(&servAddr, 0, sizeof servAddr);
  servAddr.sin_family = AF_INET;
#ifdef SERV_ANYPORT
  servAddr.sin_port = htons(0);
#else
  servAddr.sin_port = htons(SERV_PORT_NUM);
#ifdef DEBUG
  printf("Server is listening at port %u\n", SERV_PORT_NUM);
#endif
#endif
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

  result = bind(_listenFd, (struct sockaddr *)&servAddr,
		sizeof(struct sockaddr));
  if (result < 0)
    perror("bind");
  DOASSERT(result >= 0, "Cannot bind to address");
     
#ifdef SERV_ANYPORT	
  // Write the server data....
  struct sockaddr_in tempAddr;
  memset(&tempAddr, 0, sizeof(struct sockaddr));
  int len = sizeof(struct sockaddr);

  result = getsockname(_listenFd, (sockaddr *)&tempAddr, &len);
  if (result < 0)
    perror("getsockname");
  DOASSERT(result >= 0, "Cannot get sockname");
  FILE *fp = fopen("server.dat", "w");
  DOASSERT(fp, "Cannot create server.dat");
  char hostname[30];
  result = gethostname(host, sizeof host);
  if (result < 0)
    perror("gethostname");
  DOASSERT(result >= 0, "Cannot get hostname");
  int temp = ntohs(tempAddr.sin_port);
  result = fprintf(fp, "%s %d\n", hostname, (int)temp);
  if (result < 0)
    perror("fprintf");
  DOASSERT(result >= 0, "Cannot write to server.dat");
  fclose(fp);
  printf("Server uses port number %d.\n", temp);
#endif	

  result = listen(_listenFd, 5);
  if (result < 0)
    perror("listen");
  DOASSERT(result >= 0, "Cannot listen");

  Dispatcher::Current()->Register(this, 10, AllState, true, _listenFd);
}

int ServerAPI::Send(int fd, enum FLAG flag, int bracket,
			 int numArgs, char **argv)
{
  u_short errflag = (flag == OK ? 0 : 1);

#ifdef DEBUG
  printf("Sending error flag %d\n", errflag);
#endif

  errflag = htons(errflag);
  int result = send(fd, (char *)&errflag, sizeof errflag, 0);
  if (result < (int)sizeof errflag) {
    perror("send");
    return -1;
  }
 
#ifdef DEBUG
  printf("Sending bracket flag %d\n", bracket);
#endif

  u_short nbracket = htons(bracket);
  result = send(fd, (char *)&nbracket, sizeof nbracket, 0);
  if (result < (int)sizeof nbracket) {
    perror("send");
    return -1;
  }

#ifdef DEBUG
  printf("Sending number of elements %d\n", numArgs);
#endif

  u_short num = htons((u_short)numArgs);
  result = send(fd, (char *)&num, sizeof num, 0);
  if (result < (int)sizeof num) {
    perror("send");
    return -1;
  }

  for(int i = 0; i < numArgs; i++) {
    u_short size;
    if (!argv[i])
      size = 0;
    else
      size = (u_short)(strlen(argv[i]) + 1);
#ifdef DEBUG
    printf("Sending size of element %d: %d\n", i, size);
#endif
    size = htons(size);
    result = send(fd, (char *)&size, sizeof size, 0);
    if (result < (int)sizeof size) {
      perror("send");
      return -1;
    }
    size = ntohs(size);
    if (size > 0) {
#ifdef DEBUG
      printf("Sending element %d: \"%s\"\n", i, argv[i]);
#endif
      result = send(fd, argv[i], size, 0);
      if (result < 0) {
	perror("send");
	return -1;
      }
    }
  }

#ifdef DEBUG
  printf("Complete message sent\n");
#endif

  return 1;
}

void ServerAPI::SubclassInsertDisplay(DeviseDisplay *disp,
					   Coord x, Coord y,
					   Coord w, Coord h)
{
}

void ServerAPI::SubclassRegisterView(View *view)
{
}

void ServerAPI::SubclassUnregisterView(View *view)
{
}

void ServerAPI::SubclassDoInit()
{
}

void ServerAPI::SubclassDoViewPerspectiveChanged(View *view,
						      Coord x, Coord y,
						      Coord w, Coord h)
{
}

void ServerAPI::SetBusy()
{
  if (++_busy == 1)
    SendControl(OK, "ChangeStatus 1");
}

void ServerAPI::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0)
    SendControl(OK, "ChangeStatus 0");
}

Boolean ServerAPI::IsBusy()
{
  return (_busy > 0);
}

void ServerAPI::ExecuteScript(char *script)
{
  char cmd[256];
  sprintf(cmd, "ExecuteScript %s", script);
  SendControl(OK, cmd);
}

void ServerAPI::FilterChanged(View *view, VisualFilter &filter,
				   int flushed)
{
#ifdef DEBUG
  printf("TkControl: filter changed\n");
#endif

  char cmd[256];
  char xLowBuf[80], yLowBuf[80], xHighBuf[80], yHighBuf[80];
  if (view->GetXAxisAttrType() == DateAttr) {
    sprintf(xLowBuf, "%s", DateString(filter.xLow));
    sprintf(xHighBuf, "%s", DateString(filter.xHigh));
  } else {
    sprintf(xLowBuf, "%.2f", filter.xLow);
    sprintf(xHighBuf, "%.2f", filter.xHigh);
  }
				
  if (view->GetYAxisAttrType() == DateAttr) {
    sprintf(yLowBuf, "%s", DateString(filter.yLow));
    sprintf(yHighBuf, "%s", DateString(filter.yHigh));
  } else {
    sprintf(yLowBuf, "%.2f", filter.yLow);
    sprintf(yHighBuf, "%.2f", filter.yHigh);
  }
  
  sprintf(cmd, "ProcessViewFilterChange {%s} %d {%s} {%s} {%s} {%s} 0",
	  view->GetName(), flushed, xLowBuf, yLowBuf, xHighBuf, yHighBuf);
  SendControl(OK, cmd);
}

void ServerAPI::ViewCreated(View *view)
{
#if 0
  char cmd[256];
  sprintf(cmd, "ProcessViewCreated {%s}", view->GetName());
  SendControl(OK, cmd);
#endif
}

void ServerAPI::ViewDestroyed(View *view)
{
#if 0
  char cmd[256];
  sprintf(cmd, "ProcessViewDestroyed {%s}", view->GetName());
  SendControl(OK, cmd);
#endif
}

/* Make view the current view in the control panel */

void ServerAPI::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  SendControl(OK, cmd);
}
