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
  Revision 1.19  1995/12/07 02:18:29  ravim
  The set of stats to be displayed is specified as a parameter to setViewStatistics.

  Revision 1.18  1995/12/06 05:42:57  ravim
  Added function to display KGraph.

  Revision 1.17  1995/12/05 17:07:00  jussi
  View statistics are now part of ViewGraph and not View, its subclass.

  Revision 1.16  1995/12/04 18:07:35  jussi
  Added getLabel and getViewStatistics. Replaced ToggleStatistics with
  SetViewStatistics.

  Revision 1.15  1995/12/02 21:06:30  jussi
  Added support for TK_WINDOW, added Set Label command, and fixed
  error checking in insertWindow.

  Revision 1.14  1995/11/28 17:01:50  jussi
  Added copyright notice and renamed printWindows to saveWindow which
  now saves a single window image to a given file.

  Revision 1.13  1995/11/28 05:34:18  ravim
  Support for statistics.

  Revision 1.12  1995/11/28 00:02:13  jussi
  Added printWindows command.

  Revision 1.11  1995/11/18 01:52:15  ravim
  defaultGroup removed. Groups associated with schema.

  Revision 1.10  1995/11/15 18:19:34  jussi
  Added definition of crsp_extract command.

  Revision 1.9  1995/09/28 00:00:12  ravim
  Fixed some bugs. Added some new functions for handling groups.

  Revision 1.8  1995/09/26 23:06:22  jussi
  Did some reformatting.

  Revision 1.7  1995/09/19 16:08:32  jussi
  Changed comp_extract from C linkage to C++ linkage.

  Revision 1.6  1995/09/19 15:30:22  jussi
  Added creation of cstat_extract command for Compustat data.

  Revision 1.5  1995/09/13 14:43:39  jussi
  Found one more instance vhere name of view was used without protecting
  braces or quotes.

  Revision 1.4  1995/09/12 16:06:15  jussi
  Modified SelectView() and other view-related TCL calls which
  did not work if view name had spaces in it.

  Revision 1.3  1995/09/06 15:30:29  jussi
  Added creation of extractStocksCmd command for ISSM data

  Revision 1.2  1995/09/05 22:16:09  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <libc.h>
#include "TkControl.h"
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
#include "ViewKGraph.h"

#ifdef TK_WINDOW
Tcl_Interp *ControlPanelTclInterp = 0;
Tk_Window ControlPanelMainWindow = 0;
#endif

extern GroupDir *gdir;
ViewKGraph *vkg = NULL;

extern int extractStocksCmd(ClientData clientData, Tcl_Interp *interp,
			    int argc, char *argv[]);
extern int comp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);
extern int crsp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);

ControlPanel::Mode TkControlPanel::_mode = ControlPanel::DisplayMode;
MapInterpClassInfo *TkControlPanel::_interpProto = NULL;

ControlPanel *GetTkControl()
{
  return new TkControlPanel();
}

/* bitmap for control panel */
/*
#define idle_width 16
#define idle_height 16
static char idle_bits[] = {
   0xff, 0x01, 0xff, 0x01, 0x1f, 0x00, 0x3f, 0x00, 0x7f, 0x00, 0xfb, 0x00,
   0xf3, 0x01, 0xe3, 0x03, 0xc3, 0x07, 0x80, 0x0f, 0x00, 0x1f, 0x00, 0x3e,
   0x00, 0x7c, 0x00, 0xf8, 0x00, 0xf0, 0x00, 0xe0};
*/

TkControlPanel::TkControlPanel()
{
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);

  _busy = false;
  _restoring = false;
  _template = false;

  _fileName = (char *)malloc(1);
  _fileName[0] = '\0';
  _fileAlias = (char *)malloc(1);
  _fileAlias[0] = '\0';

  _winName = (char *)malloc(1);
  _winName[0] = '\0';
  _gdataName = (char *)malloc(1);
  _gdataName[0] = '\0';
  _viewName = (char *)malloc(1);
  _viewName[0] = '\0';
  _sessionName = CopyString(Init::SessionName());
  _argv0 = CopyString(Init::ProgName());

  /* register with dispatcher to be always informed of when to run */
  Dispatcher::Current()->Register(this, AllState, true);

  /* Create a new interpreter */
  _interp = Tcl_CreateInterp();
  _mainWindow = Tk_CreateMainWindow(_interp, NULL, "DEVise", "DEVise");
  if (_mainWindow == NULL) {
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
  Tk_MoveWindow(_mainWindow, 0,0);
  Tk_GeometryRequest(_mainWindow, 100, 200);

#ifdef TK_WINDOW
  ControlPanelTclInterp = _interp;
  ControlPanelMainWindow = _mainWindow;
#endif

  /* Init tcl and tk */
  if (Tcl_Init(_interp) == TCL_ERROR) {
    fprintf(stderr,"can't init tcl in TkControl.c\n");
    Exit::DoExit(1);
  }
  if (Tk_Init(_interp) == TCL_ERROR) {
    fprintf(stderr,"can't init tk in TkControl.c\n");
    Exit::DoExit(1);
  }

  /*
     if (Tk_DefineBitmap(_interp,
     Tk_GetUid("idle"), idle_bits, idle_width,idle_height)
     == TCL_ERROR){
     fprintf(stderr,"TkControlPanel::can't init bitmap\n");
     Exit::DoExit(1);
     }
     */
}

/* start session */
void TkControlPanel::StartSession()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1995\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

  Tcl_LinkVar(_interp,"fileName", (char *)&_fileName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"fileAlias", (char *)&_fileAlias, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"gdataName", (char *)&_gdataName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"windowName",(char *) &_winName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"viewName",(char *) &_viewName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"sessionName",(char *)&_sessionName, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"argv0",(char *)&_argv0, TCL_LINK_STRING);
  Tcl_LinkVar(_interp,"restoring",(char *)&_restoring, TCL_LINK_INT);
  Tcl_LinkVar(_interp,"template",(char *)&_template, TCL_LINK_INT);

  /* Create a new tcl command for control panel */
  Tcl_CreateCommand(_interp, "DEVise", ControlCmd, this, NULL);

  /* Create a new tcl command for ISSM stock data */
  Tcl_CreateCommand(_interp, "issm_extractStocks", extractStocksCmd, 0, 0);

  /* Create a new tcl command for Compustat data */
  Tcl_CreateCommand(_interp, "cstat_extract", comp_extract, 0, 0);

  /* Create a new tcl command for CRSP data */
  Tcl_CreateCommand(_interp, "crsp_extract", crsp_extract, 0, 0);

  char *envPath = getenv("DEVISE_LIB");
  char *control;
  char buf[256];
  if (envPath != NULL) {
    sprintf(buf,"%s/control.tk",envPath);
    control = buf;
  } else
    control = "control.tk";

  printf("Control panel file is: %s\n", control);

  int code = Tcl_EvalFile(_interp,control);
  if (code != TCL_OK) {
    fprintf(stderr,"%s\n", _interp->result);
    Exit::DoExit(1);
  }

  if (Init::Restore()) {
    /* restore session */
    _restoring = true;
    int code = Tcl_EvalFile(_interp,_sessionName);
    _restoring = false;
    if (code != TCL_OK){
      fprintf(stderr,"Can't restore session file %s\n",_sessionName);
      fprintf(stderr,"%s\n", _interp->result);
      Exit::DoExit(1);
    }
  }
}

char * TkControlPanel::SessionName()
{
  return _sessionName;
}
void TkControlPanel::SetSessionName(char *name)
{
  /*XXX: This is a memory leak */
  _sessionName = CopyString(name);
}

inline void MakeReturnVals(Tcl_Interp *interp, int numArgs, char **args)
{
  for (int i=0; i < numArgs; i++){
    Tcl_AppendElement(interp,args[i]);
  }
}

ControlPanel::Mode TkControlPanel::GetMode()
{
  return _mode;
}

Boolean TkControlPanel::Restoring()
{
  return _restoring;
}

/********************************************************************
Commands:
	catFiles: get all catalog files imported.
	exit: exit program
	get category: get all classes belonging to a category.
	importFileType file: import file type from file
	get category class: get all instances beloging to class
	getparam category class: get params for creating class.

	changeableParam instance: true if instance can return current parameters
	getInstParam instance: get current parameters for an instance
	changeParam instance [ params]: change params for instance

	exists instance: Return true if instance exists, else false.
	getCreateParam category class instance: get parameters that can
		be used to recreate an instance.
	getViewMappings view: Get all mappings connected to view
	refreshView view: Refresh a view
	changeMode mode: 0 == display, 1 == layout mode
	getWinViews win: get all views in this window.
	getViewWin view: get window for this view, or ""
	getLinkViews: get all views in this link

	create category class [params]: create a new instance of class and
			return its name, or "" if nothing created..
	insertMapping view mapping: connect mapping to view
	insertLink link view: connect view to link
	viewInLink link view: return true if view is in this visual link
	unlinkView link view: unlink view from link
	getLinkFlag link: return flag for link
	setLinkFlag link flag: set flag for link

	insertWindow view window: insert view into window
	saveWindowImage window file: save window image as Postscript to file
	removeView viewName: remove view from its window
	isMapped view: Return true if view is mapped.
	setDefault category class [params]: set defaults for a class
	tcheckpoint tinstances: do checkpoint on tdata
	getVisualFilters view:  Get all visual filters for a view, in
				string format
	getCurVisualFilter view: get current visual filter for a view,
			in float format
	setFilter view xlow ylow xhigh yhigh: set visual filter for view
	highlightView view flag: highlight/unhighlight view

	setAxis view axisLabel type: set axis label for view. type == "x" or "y"
	getAxis view type: get axis for view. type == "x" or "y"

	setAction view action : set action for view
	getAction view: get action for view

	getSchema tdata: get schema for tdata. Return:
		{ {attr type sorted} {attr type sorted} ... }

	getPixelWidth mapping: get current pixel width for mapping
	setPixelWidth mapping width: set mapping's pixel width

	createInterp tdata interpName x y color size pattern orientation
		shape shapeAttr 0 shapeAttr 1: create interpreted mapping with 
		these args. Return NULL if no mapping created. Otherwise,
		return name of the mapping.
	isInterpreted mappClass: return true if interpreted mapping class.
	isInterpretedGData gdata: return true if interpreted GData.
	interpMapClassInfo: return infomation that
		can be used to create interpreted mapping classes. Info
		is of the form: { {class 1 args } {class2 args2 } ... },
		where the args are { tdata mappingName sorted x y color ... }
	
	paseDateFloat "date"

	destroy instance: destroy this instance
	openSession file: open and restore the session file, assuming
			no pre-existing objects in memory.	
	openTemplate file: open and restore the session file, assuming
			no pre-existing objects in memory.	
	printDispatcher: print what's in the dispatcher.
	clearQP: clear query processor's queries.
	clearInterp: clear list of interpreted classes
	getMappingTData map: Get the tdata for mapping
	markViewFilter view index true_false: mark indexth visualfilter
		of view as either marked or unmarked.
	getCursorViews cursor: get src and dst views of cursor.
	setCursorSrc cursor view : set source of cursor
	setCursorDst cursor view: set destination of cursor

	For view initialization:
	clearViewHistory view: clear history of view
	insertViewHistory view xl yl xh yh marked: insert history into view
		without changing visual filter.
	tdataFileName tdata: return actual file name of tdata.
	getAxisDisplay view [X|Y]: return "1" if X/Y axis display is on. else, 0
	setAxisDisplay view [X|Y] boolean: set X/Y axis display on/off.
	swapView win view1 view2: swap the positions of the two views
		in the window.
	close fileId: close a file with the given id.
	open file flag: open a file with the specified flags. Return id to it.
	savePixmap View file: save view's pixmap to file. This is experimental.
	loadPixmap View file: load view's pixmap.
	date: return current file
	readLine file: read a line from file, return the line
	writeLine line file: write a line to file
	invalidatePixmap view: invalid pixmap cache for view
*********************************************************************/
int TkControlPanel::ControlCmd(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]) {
	TkControlPanel *control = (TkControlPanel *)clientData;

	ClassDir *classDir = control->GetClassDir();
	int code;
	int numArgs; char **args;
	char *name;
	Group *grp;
	
	if (argc == 1){
		interp->result = "wrong args";
		goto error;
	}
	else if (argc == 2){
		if (strcmp(argv[1],"date") == 0 ) {
			time_t tm = time((time_t *)0);
			sprintf(interp->result,"%s",DateString(tm));
		}
		else if (strcmp(argv[1],"clearQP") == 0){
			/*
			classDir->Print();
			*/

			classDir->DestroyTransientClasses();

			QueryProc::Instance()->ClearQueries();

			/* Clear all catalog files */
			ClearCats();
		}
		else if (strcmp(argv[1],"clearInterp") == 0){
			control->_interpProto->ClearMapClasses();
		}
		else if (strcmp(argv[1],"printDispatcher") == 0){
			Dispatcher::Current()->Print();
		}
		else if (strcmp(argv[1],"interpMapClassInfo") == 0){
			int num;
			MapInterpClassInfo **infos;
			_interpProto->MapClasses(num, infos);
			int i;
			/*
			Tcl_AppendResult(interp," { ", NULL);
			*/
			for (i=0; i < num; i++){
				MapInterpClassInfo *cInfo = infos[i];
				int numArgs; char **args;
				cInfo->CreateParams(numArgs, args);
				int j;
				Tcl_AppendResult(interp," {", NULL);
				for (j=0; j < numArgs; j++){
					if (args[j] == NULL)
						Tcl_AppendResult(interp, " {} ", NULL);
					else Tcl_AppendResult(interp, " {", args[j], "} ", NULL);
				}
				Tcl_AppendResult(interp," }", NULL);
			}
			/*
			Tcl_AppendResult(interp," } ", NULL);
			*/
		}
		else if (strcmp(argv[1],"catFiles") == 0){
			CatFiles(numArgs, args);
			MakeReturnVals(interp, numArgs, args);
		}
		else if (strcmp(argv[1],"exit")== 0){
			QueryProc::Instance()->PrintStat();
			control->DoQuit();
		}
		else {
			interp->result = "wrong args";
			goto error;
		}
	}
	else if (strcmp(argv[1], "showkgraph") == 0) {
	    if ((atoi(argv[2]) == 1) || (!vkg))  {
		/* Create a new ViewKGraph object */
		/* Dont delete the previous vkg since we still want it to
		   continuously display the previously defined KGraph */
		/* Who eventually deletes it ?? */
		/* delete vkg; */
		vkg = new ViewKGraph();
	      }
	    int i = 0;
	    /* Number of views */
	    int nview = argc - 5;
	    ViewGraph **vlist = (ViewGraph **)malloc(nview*sizeof(ViewGraph *));
	    for (i = 0; i < nview; i++) {
	      vlist[i] = (ViewGraph *)classDir->FindInstance(argv[5+i]);
	      if (vlist[i] == NULL) {
		  interp->result = "Cannot find view";
		  goto error;
		}
	    }
	    vkg->Init();
	    /* Add these to the ViewKGraph class and display */
	    if (vkg->AddViews(vlist, nview, argv[4]) == false) {
		interp->result = "Could not add views to ViewKGraph";
		goto error;
	      }
		  
	    if (vkg->Display(atoi(argv[3])) == false) {
		interp->result = "Could not display the KGraph";
		goto error;
	      }
	    free(vlist);
	  }
	else if (argc == 3){
		if (strcmp(argv[1],"invalidatePixmap") == 0 ) {
			View *vg = (View *)classDir->FindInstance(argv[2]);
			if (vg == NULL){
				interp->result = "Can't find view";
				goto error;
			}
			vg->InvalidatePixmaps();
		}
		else if (strcmp(argv[1],"readLine") == 0 ) {
			FILE *file = (FILE *)atol(argv[2]);
			(void)fgets(interp->result, 256, file);
			int len = strlen(interp->result);
			if (len > 0 && interp->result[len-1] == '\n')
					interp->result[len-1] = '\0';
		}
		else if (strcmp(argv[1],"close") == 0){
			FILE *file = (FILE *)atol(argv[2]);
			fclose(file);
		}
		else if (strcmp(argv[1],"isMapped") == 0) {
			View *vg = (View *)classDir->FindInstance(argv[2]);
			if (vg == NULL) {
				sprintf(interp->result,"Can't find view %s in isMapped",
					argv[2]);
				goto error;
			}
			sprintf(interp->result,"%d",(vg->Mapped()? 1: 0 ));
		}
		else if (strcmp(argv[1],"getLabel") == 0){
			View *vg = (View *)classDir->FindInstance(argv[2]);
			if (vg == NULL) {
			  sprintf(interp->result,"Can't find view %s in getLabelParam",
				  argv[2]);
			  goto error;
			}
			Boolean occupyTop;
			int extent;
			char *name;
			vg->GetLabelParam(occupyTop, extent, name);
			sprintf(interp->result, "%d %d {%s}",
				(occupyTop ? 1 : 0), extent,
				(name ? name : ""));
		}
		else if (strcmp(argv[1],"tdataFileName") == 0){
			TData *tdata = (TData *)classDir->FindInstance(argv[2]);
			if (tdata == NULL){
				interp->result = "Can't find tdata in tdataFileName";
				goto error;
			}
			sprintf(interp->result,"%s",tdata->GetName());
		}
		else if (strcmp(argv[1],"getViewWin") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "Can't find view in getViewWin";
				goto error;
			}
			ViewWin *viewWin = view->GetParent();
			if (viewWin == NULL)
				interp->result = "";
			else sprintf(interp->result,"%s",viewWin->GetName());
		}
		else if (strcmp(argv[1],"clearViewHistory") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "Can't find cursor in getCursorViews";
				goto error;
			}
			FilterQueue *queue = view->GetHistory();
			queue->Clear();
		}
		else if (strcmp(argv[1],"getCursorViews") == 0){
			DeviseCursor *cursor = (DeviseCursor *)
				classDir->FindInstance(argv[2]);
			if (cursor == NULL){
				interp->result = "Can't find cursor in getCursorViews";
				goto error;
			}
			View *src = cursor->GetSource();
			View *dst = cursor->GetDst();
			char *name1, *name2;
			if (src != NULL)
				name1 = src->GetName();
			else name1 = "";
			if (dst != NULL)
				name2 = dst->GetName();
			else name2 = "";

			sprintf(interp->result,"{%s} {%s}",name1,name2);
		} 
		else if (strcmp(argv[1],"getMappingTData") == 0){
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[2]);
			if (map== NULL){
				interp->result = "Can't find mapping in getPixelWidth";
				goto error;
			}
			TData *tdata = map->GetTData();
			sprintf(interp->result,"%s",classDir->FindInstanceName(tdata));
		}
		else if (strcmp(argv[1],"openSession") == 0 ){
			(void)Tcl_Eval(interp,"set template 0");
			control->_restoring = true;
			code = Tcl_EvalFile(interp,argv[2]);
			control->_restoring = false;
			if (code != TCL_OK){
				interp->result = "can't restore session file\n";
				goto error;
			}
			control->SetSessionName(argv[2]);
		}
		else if (strcmp(argv[1],"openTemplate") == 0 ){
			(void)Tcl_Eval(interp,"set template 1");
			control->_restoring = true;
			code = Tcl_EvalFile(interp,argv[2]);
			control->_restoring = false;
			if (code != TCL_OK){
				interp->result = "can't restore template file\n";
				goto error;
			}
			control->SetSessionName(argv[2]);
		}
		else if (strcmp(argv[1],"destroy") == 0 ){
			classDir->DestroyInstance(argv[2]);
		}
		else if (strcmp(argv[1],"parseDateFloat") == 0 ){
			char buf[80];
			double val;
			(void)ParseFloatDate(argv[2], val);
			sprintf(buf,"%f",val);
			interp->result= buf;
		}
		else if (strcmp(argv[1],"isInterpretedGData") == 0 ){
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[2]);
			if (map== NULL){
				interp->result = "Can't find mapping in getPixelWidth";
				goto error;
			}
			if (map->IsInterpreted())
				interp->result = "1";
			else interp->result = "0";
		}
		else if (strcmp(argv[1],"isInterpreted") == 0 ){
			int *isInterp = (int *)classDir->UserInfo("mapping", argv[2]);
			if (isInterp== NULL){
				/*
				interp->result = "Can't find mapping in isInterpreted";
				goto error;
				*/
				interp->result = "0";
			}
			else {
				if (*isInterp)
					interp->result = "1";
				else interp->result = "0";
			}
		}
		else if (strcmp(argv[1],"getPixelWidth") == 0 ){
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[2]);
			if (map== NULL){
				interp->result = "Can't find mapping in getPixelWidth";
				goto error;
			}
			char buf[80];
			sprintf(buf,"%d", map->GetPixelWidth());
			Tcl_AppendElement(interp,buf);
		}
		else if (strcmp(argv[1], "getTopGroups") == 0)
		{
		  gdir->top_level_groups(interp, argv[2]);
		}
		else if (strcmp(argv[1],"getSchema") == 0 ){

			TData *tdata = (TData *)classDir->FindInstance(argv[2]);
			if (tdata == NULL){
				interp->result = "Can't find tdata in getSchema";
				goto error;
			}
			AttrList *attrList = tdata->GetAttrList();
			if (attrList != NULL){
				/*
				printf("getSchema: \n");
				attrList->Print();
				*/
				char attrBuf[160];
				int numAttrs = attrList->NumAttrs();
				int i;
				Tcl_AppendElement(interp,"recId int 1");
				for (i=0; i < numAttrs; i++){
					AttrInfo *info = attrList->Get(i);
					/*
					printf("inserting %s\n",info->name);
					*/
					switch(info->type){
						case FloatAttr:
							sprintf(attrBuf,"%s float %d",info->name,
								info->isSorted);
							break;
						case DoubleAttr:
							sprintf(attrBuf,"%s double %d",info->name,
								info->isSorted);
							break;
						case StringAttr:
							sprintf(attrBuf,"%s string %d",info->name,
								info->isSorted);
							break;
						case IntAttr:
							sprintf(attrBuf,"%s int %d",info->name,
								info->isSorted);
							break;
						case DateAttr:
							sprintf(attrBuf,"%s date %d",info->name,
								info->isSorted);
							break;
						default:
							printf("TkControl:unknown attr\n");
							Exit::DoExit(1);
					}
					Tcl_AppendElement(interp,attrBuf);
				}
			}
			else {
				/*
				printf("NULL attr List\n");
				*/
			}
		}
		else if (strcmp(argv[1],"getAction") == 0 ) {
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "Can't find view";
				goto error;
			}
			Action *action = view->GetAction();
			if (action == NULL || strcmp(action->GetName(),"") == 0 ||
				strcmp(action->GetName(),"default") == 0)
				interp->result = "";
			else interp->result = action->GetName();
		}
		else if (strcmp(argv[1],"getLinkFlag") == 0 ) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[2]);
			if (link == NULL) {
				interp->result = "can't find link";
				goto error;
			}
			sprintf(interp->result,"%d",link->GetFlag());
		}
		else if (strcmp(argv[1],"importFileType") == 0) {
			if ((name=ParseCat(argv[2])) == NULL){
				interp->result= "";
			}
			else interp->result = name;
		}
		else if (strcmp(argv[1],"changeableParam") == 0){
			Boolean changeable = classDir->Changeable(argv[2]);
			if (changeable)
				interp->result = "1";
			else interp->result = "0";
		}
		else if (strcmp(argv[1],"getInstParam") == 0){
			/* get current parameters for instance */
			/*printf("getInstParam %s\n", argv[2]); */
			classDir->GetParams(argv[2],numArgs, args);
			/* printf("getInstParam %s: got %d args\n",argv[2], numArgs);*/
			MakeReturnVals(interp, numArgs, args);
		}
		else if (strcmp(argv[1],"tcheckpoint") == 0){
			/* checkpoint all tdata */
			TData *tdata;
			control->SetBusy();
			if ((tdata=(TData *)classDir->FindInstance(argv[2])) != NULL)
				tdata->Checkpoint();
			control->SetIdle();
		}
		else if (strcmp(argv[1],"get") == 0){
			classDir->ClassNames(argv[2],numArgs, args);
			/*
			printf("get %s, got %d args\n", argv[2], numArgs);
			int ind;
			for (ind=0; ind < numArgs; ind++){
				printf("%s\n",args[ind]);
			}
			*/
			MakeReturnVals(interp, numArgs, args);
		} else if (strcmp(argv[1],"changeMode") == 0){
			if (strcmp(argv[2],"0") == 0){
				if( _mode != ControlPanel::DisplayMode) {
					/* Set display mode  and make all views refresh*/
					_mode = ControlPanel::DisplayMode;
					ControlPanel::Instance()->ReportModeChange(_mode);
				}
			} else if(_mode != ControlPanel::LayoutMode){
				/* set layout mode */
				_mode = ControlPanel::LayoutMode;
				ControlPanel::Instance()->ReportModeChange(_mode);
			}
		}
		else if (strcmp(argv[1],"exists") == 0){
			void *ptr = classDir->FindInstance(argv[2]);
			if ( ptr == (void *)NULL )
				interp->result = "0";
			else interp->result = "1";
		} 
		else if (strcmp(argv[1],"removeView") == 0){
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "can't find view";
				goto error;
			}
			if (!view->Mapped()){
				interp->result = "view not in any window";
				goto error;
			}
			view->DeleteFromParent();
		}
		else if (strcmp(argv[1],"getViewMappings") == 0) {
			/*
			printf("finding view '%s'\n",argv[2]);
			*/
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "can't find view";
				goto error;
			}
			for (view->InitMappingIterator(); view->MoreMapping(); ){
				TDataMap *map = view->NextMapping();
				Tcl_AppendElement(interp,map->GetGDataName());
			}
			view->DoneMappingIterator();
		}
		else if (strcmp(argv[1],"refreshView") == 0) {
			/*
			printf("finding view '%s'\n",argv[2]);
			*/
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "can't find view";
				goto error;
			}
			view->Refresh();
		}
		else if (strcmp(argv[1],"getWinViews") == 0) {
			ViewWin *win = (ViewWin*)classDir->FindInstance(argv[2]);
			if (win == NULL) {
				interp->result = "can't find window";
				goto error;
			}
			for(int index = win->InitIterator(); win->More(index); ){
				ViewWin *view = (ViewWin *)win->Next(index);
				Tcl_AppendElement(interp, view->GetName());
			}
			win->DoneIterator(index);
		} else if (strcmp(argv[1],"getLinkViews") == 0) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[2]);
			if (link == NULL) {
				interp->result = "can't find link";
				goto error;
			}
			int index;
			for (index=link->InitIterator(); link->More(index); ){
				ViewWin *view = (ViewWin *)link->Next(index);
				Tcl_AppendElement(interp, view->GetName());
			}
			link->DoneIterator(index);
		}
		else if (strcmp(argv[1],"getCurVisualFilter") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "can't find view";
				goto error;
			}
			VisualFilter *filter = view->GetVisualFilter();
			char buf[256];
			sprintf(buf,"%.2f", filter->xLow);
			Tcl_AppendElement(interp, buf);

			sprintf(buf,"%.2f", filter->yLow);
			Tcl_AppendElement(interp, buf);

			sprintf(buf,"%.2f", filter->xHigh);
			Tcl_AppendElement(interp, buf);

			sprintf(buf,"%.2f", filter->yHigh);
			Tcl_AppendElement(interp, buf);
		}
		else if (strcmp(argv[1],"getVisualFilters") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "can't find view";
				goto error;
			}
			FilterQueue *queue = view->GetHistory();
			int i;
			for (i=0; i < queue->Size(); i++){
				VisualFilter filter;
				queue->Get(i,filter);
				char buf[256];
				char xLowBuf[40],xHighBuf[40],yLowBuf[40],yHighBuf[40];
				if (view->GetXAxisAttrType() == DateAttr){
					sprintf(xLowBuf,"%s",DateString(filter.xLow));
					sprintf(xHighBuf,"%s",DateString(filter.xHigh));
				}
				else {
					sprintf(xLowBuf,"%.2f",filter.xLow);
					sprintf(xHighBuf,"%.2f",filter.xHigh);
				}
				
				if (view->GetYAxisAttrType() == DateAttr){
					sprintf(yLowBuf,"%s",DateString(filter.yLow));
					sprintf(yHighBuf,"%s",DateString(filter.yHigh));
				}
				else {
					sprintf(yLowBuf,"%.2f",filter.yLow);
					sprintf(yHighBuf,"%.2f",filter.yHigh);
				}

				sprintf(buf,"{%s} {%s} {%s} {%s} %d",xLowBuf, yLowBuf,
					xHighBuf, yHighBuf, filter.marked);
				Tcl_AppendElement(interp, buf);
			}
		}
		else if (strcmp(argv[1], "getViewStatistics") == 0) {
		    ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[2]);
		    if (vg == NULL) {
			interp->result = "Can't find view in GetViewStatistics";
			goto error;
		    }
		    /* Return status of statistics display */
		    if (vg->GetDisplayStats())
		      interp->result = "1";
		    else
		      interp->result = "0";
		}
		else {
			interp->result = "wrong args";
			goto error;
		}
	}
	else if (argc == 4) {
		if (strcmp(argv[1],"writeLine") == 0){
			FILE *file = (FILE *)atol(argv[3]);
			/*
			printf("writeLIne %s file is %d\n", argv[2], argv[3]);
			*/
			fputs(argv[2],file);
			fputs("\n",file);
		}
		else if (strcmp(argv[1],"open") == 0){
			FILE *file = fopen(argv[2],argv[3]);
			if (file == NULL){
				fprintf(stderr,"can't open file %s\n", argv[2]);
				Exit::DoExit(1);
			}
			/*
			printf("open %d\n",file);
			*/
			sprintf(interp->result,"%ld",(long)file);
		}
		else if (strcmp(argv[1], "setViewStatistics") == 0) {
		    ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[2]);
		    if (vg == NULL) {
			interp->result = "Can't find view in SetViewStatistics";
			goto error;
		    }
		    /* Turn on/off display of statistics */
		    vg->SetDisplayStats(argv[3]);
		}
		else if (strcmp(argv[1],"savePixmap") == 0){
			/*
			printf("savePixmap %s %s %s\n",argv[1],argv[2],argv[3]);
			*/
			View *vg = (View *)classDir->FindInstance(argv[2]);
			if (vg == NULL){
				interp->result = "Can't find view in savePixmap";
				goto error;
			}
			FILE *file = (FILE *)atol(argv[3]);
			/*
			printf("file is %d\n", (int)file);
			*/
			vg->SavePixmaps(file);
		}
		else if (strcmp(argv[1],"loadPixmap") == 0){
			/*
			printf("loadPixmap %s %s %s\n",argv[1],argv[2],argv[3]);
			*/
			View *vg = (View *)classDir->FindInstance(argv[2]);
			if (vg == NULL){
				interp->result = "Can't find view in loadPixmap";
				goto error;
			}
			FILE *file = (FILE *)atol(argv[3]);
			/*
			printf("file is %d\n", (int)file);
			*/
			vg->LoadPixmaps(file);
		}
		else if (strcmp(argv[1],"getAxisDisplay") == 0 ) {
			View *vg = (View *)classDir->FindInstance(argv[2]);
			if (vg == NULL){
				interp->result = "Can't find view in getAxisDisplay";
				goto error;
			}
			Boolean stat, xAxis, yAxis;
			vg->AxisDisplay(xAxis, yAxis);
			if (strcmp(argv[3],"X") == 0) 
			  stat = xAxis;
			else
			  stat = yAxis;
			if (stat)
			  interp->result = "1";
			else
			  interp->result = "0";
		}
		else if (strcmp(argv[1],"replaceView") == 0) {
			View *view1 = (View *)classDir->FindInstance(argv[2]);
			View *view2 = (View *)classDir->FindInstance(argv[3]);
			if (view1 == NULL || view2 == NULL){
				fprintf(stderr,"TkControl: can't find view for replaceView\n");
				Exit::DoExit(1);
			}
			ViewWin *win = view1->GetParent();
			if (win == NULL){
				fprintf(stderr,"TkControl: can't find window for replaceView\n");
				Exit::DoExit(1);
			}
			win->Replace(view1, view2);
		} else if (strcmp(argv[1],"setCursorSrc") == 0 ) {
			DeviseCursor *cursor = (DeviseCursor *)
				classDir->FindInstance(argv[2]);
			if (cursor == NULL){
				interp->result = "Can't find cursor";
				goto error;
			}
			View *view = (View *)classDir->FindInstance(argv[3]);
			if (view == NULL){
				interp->result = "Can't find view";
				goto error;
			}
			cursor->SetSource(view);
		}
		else if (strcmp(argv[1],"setCursorDst") == 0) {
			DeviseCursor *cursor = (DeviseCursor *)
				classDir->FindInstance(argv[2]);
			if (cursor == NULL){
				interp->result = "Can't find cursor";
				goto error;
			}
			View *view = (View *)classDir->FindInstance(argv[3]);
			if (view == NULL){
				interp->result = "Can't find view";
				goto error;
			}
			cursor->SetDst(view);
		} 
		else if (strcmp(argv[1],"setPixelWidth") == 0 ){
			TDataMap *map= (TDataMap *)classDir->FindInstance(argv[2]);
			if (map== NULL){
				interp->result = "Can't find mapping in setPixelWidth";
				goto error;
			}
			int width = atoi(argv[3]);
			if (width > 0 )
				map->SetPixelWidth(width);
		}
		else if (strcmp(argv[1],"getAxis") == 0 ) {
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				interp->result = "Can't find view";
				goto error;
			}
			AxisLabel *label;
			if (strcmp(argv[3],"x") == 0) 
				label = view->GetXAxisLabel();
			else label = view->GetYAxisLabel();

			if (label == NULL || strcmp(label->GetName(),"") == 0)
				interp->result = "";
			else interp->result = label->GetName();
		}
		else if (strcmp(argv[1],"setAction") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL) {
				interp->result = "can't find view";
				goto error;
			}
			Action *action = (Action*)classDir->FindInstance(argv[3]);
			if (action == NULL) {
				interp->result = "can't find action";
				goto error;
			}
			view->SetAction(action);
		}
		else if (strcmp(argv[1],"setLinkFlag") == 0 ) {
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[2]);
			if (link == NULL) {
				interp->result = "can't find link";
				goto error;
			}
			VisualFlag flag = atoi(argv[3]);
			link->ChangeFlag(flag);
		}
		else if (strcmp(argv[1], "highlightView") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd highlightView can't find view %s\n", argv[2]);
				Exit::DoExit(2);
			}
			view->Highlight(atoi(argv[3]));
		}
		else if (strcmp(argv[1],"get") == 0){
			classDir->InstanceNames(argv[2],argv[3], numArgs, args);
			MakeReturnVals(interp, numArgs, args);
		}
		else if (strcmp(argv[1],"getparam") == 0){
			classDir->GetParams(argv[2],argv[3],numArgs, args);
			MakeReturnVals(interp, numArgs, args);
		}
		else if (strcmp(argv[1],"insertMapping") == 0){
			ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd insertMapping can't find view %s\n", argv[2]);
				Exit::DoExit(2);
			}
			TDataMap *map = (TDataMap *)classDir->FindInstance(argv[3]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd insertMapping can't find map %s\n", argv[3]);
				Exit::DoExit(2);
			}
			view->InsertMapping(map);
		}
		else if (strcmp(argv[1],"insertLink") == 0){
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[2]);
			if (link == NULL){
				fprintf(stderr,"TkControl:Cmd insertLink can't find link %s\n", argv[2]);
				Exit::DoExit(2);
			}
			View *view = (View *)classDir->FindInstance(argv[3]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd insertMapping can't find view %s\n", argv[3]);
				Exit::DoExit(2);
			}
			link->InsertView(view);
		}
		else if (strcmp(argv[1],"viewInLink") == 0){
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[2]);
			if (link == NULL){
				fprintf(stderr,"TkControl:Cmd insertLink can't find link %s\n", argv[2]);
				Exit::DoExit(2);
			}
			View *view = (View *)classDir->FindInstance(argv[3]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd insertMapping can't find view %s\n", argv[3]);
				Exit::DoExit(2);
			}
			if (link->ViewInLink(view)){
				interp->result = "1";
			}
			else {
				interp->result = "0";
			}
		}
		else if (strcmp(argv[1],"unlinkView") == 0){
			VisualLink *link = (VisualLink *)classDir->FindInstance(argv[2]);
			if (link == NULL){
				fprintf(stderr,"TkControl:Cmd insertLink can't find link %s\n", argv[2]);
				Exit::DoExit(2);
			}
			View *view = (View *)classDir->FindInstance(argv[3]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd insertMapping can't find view %s\n", argv[3]);
				Exit::DoExit(2);
			}
			link->DeleteView(view);
		}
		else if (strcmp(argv[1],"insertWindow") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (!view) {
				fprintf(stderr,
					"TkControl:Cmd insertWindow can't find view %s\n", argv[2]);
				Exit::DoExit(2);
			}
			ViewWin *win = (ViewWin *)classDir->FindInstance(argv[3]);
			if (!win) {
				fprintf(stderr,"TkControl:Cmd insertWindow can't find window %s\n", argv[3]);
				Exit::DoExit(2);
			}
			view->DeleteFromParent();
			view->AppendToParent(win);
		}
		else if (strcmp(argv[1], "saveWindowImage") == 0) {
		  ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
		  if (!viewWin) {
		    fprintf(stderr,
			    "TkControl:cmd saveWindowImage can't find window %s\n",
			    argv[2]);
		    Exit::DoExit(2);
		  }
		  viewWin->GetWindowRep()->WritePostscript(true, argv[3]);
		}
		else {
			interp->result = "wrong args";
			goto error;
		}
	}
	else if (strcmp(argv[1],"swapView") == 0 ) {
		ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[2]);
		View *view1 = (View *)classDir->FindInstance(argv[3]);
		View *view2 = (View *)classDir->FindInstance(argv[4]);
		if (viewWin == NULL || view1 == NULL || view2 == NULL){
			interp->result = "Can't find view or window in swapView ";
			goto error;
		}
		if (view1->GetParent() != viewWin || view2->GetParent() != viewWin){
			interp->result = "Can't view not in window in swapView ";
			goto error;
		}
		viewWin->SwapChildren(view1, view2);
	}
	else if (strcmp(argv[1],"setAxisDisplay") == 0 ) {
		View *vg = (View *)classDir->FindInstance(argv[2]);
		if (vg == NULL){
			interp->result = "Can't find view in setAxisDisplay";
			goto error;
		}
		Boolean stat = atoi(argv[4]);
		if (strcmp(argv[3],"X") == 0)
		  vg->XAxisDisplayOnOff(stat);
		else
		  vg->YAxisDisplayOnOff(stat);
	}
	else if (strcmp(argv[1],"insertViewHistory") == 0 ){
		View *view = (View *)classDir->FindInstance(argv[2]);
		if (view == NULL){
			fprintf(stderr,"TkControl:Cmd insertViewHistory find view %s\n", 
				argv[2]);
			Exit::DoExit(2);
		}
		VisualFilter filter;
		(void)ParseFloatDate(argv[3],filter.xLow); 
		(void)ParseFloatDate(argv[4],filter.yLow);
		(void)ParseFloatDate(argv[5],filter.xHigh); 
		(void)ParseFloatDate(argv[6],filter.yHigh);
		filter.marked = atoi(argv[7]);
		view->InsertHistory(filter);
	}
	else if (strcmp(argv[1],"markViewFilter") == 0 ){
		View *view = (View *)classDir->FindInstance(argv[2]);
		if (view == NULL){
			fprintf(stderr,"TkControl:Cmd markViewFiltercan't find view %s\n", 
				argv[2]);
			Exit::DoExit(2);
		}
		int index = atoi(argv[3]);
		Boolean mark = atoi(argv[4]);
		view->Mark(index, mark);
	}
	else if (strcmp(argv[1],"setAxis") == 0 ) {
		if (argc != 5){
			fprintf(stderr,"TkControl:setAxis needs 5 params\n");
			Exit::DoExit(2);
		}
		View *view = (View *)classDir->FindInstance(argv[2]);
		if (view == NULL){
			fprintf(stderr,"TkControl:Cmd setAxis can't find view %s\n", 
				argv[2]);
			Exit::DoExit(2);
		}
		AxisLabel *label = (AxisLabel *)classDir->FindInstance(argv[3]);
		if (label == NULL){
			fprintf(stderr,"TkControl:Cmd setAxis can't find label %s\n", 
				argv[3]);
			Exit::DoExit(2);
		}
		if (strcmp(argv[4],"x")== 0)
			view->SetXAxisLabel(label);
		else view->SetYAxisLabel(label);
	}
	else if (strcmp(argv[1],"changeParam") == 0){
		classDir->ChangeParams(argv[2],argc-3,&argv[3]);
	}
	else if (strcmp(argv[1],"createInterp") == 0){
		/*
		printf("CreateInterp %d args\n", argc-2);
		int i;
		for (i=0; i < argc; i++){
			printf("arg %d: %s\n", i, argv[i]);
		}
		*/
		ClassInfo *classInfo = 
			_interpProto->CreateWithParams(argc-2,&argv[2]);
		if (classInfo == NULL){
			interp->result = "Can't create mapping";
			goto error;
		}
		interp->result = classInfo->ClassName();
		RegisterClass(classInfo,true);
	}
	else if (strcmp(argv[1],"create") == 0) {
		control->SetBusy();
		char *name = classDir->CreateWithParams(argv[2],argv[3],
			argc-4, &argv[4]);
		control->SetIdle();
		if (name == NULL){
			interp->result = "";
		}
		else {
			Tcl_AppendResult(interp,name, (char *)NULL);
		}
	}
	else if (strcmp(argv[1],"setDefault") == 0) {
		if (argc < 5){
			interp->result = "wrong args";
			goto error;
		}
		classDir->SetDefault(argv[2],argv[3],argc-4,&argv[4]);
	}
	else if (argc == 5) {
		if (strcmp(argv[1],"getCreateParam") == 0){
			/*getCreateParam category class instance: 
			get parameters used to recreate an instance */
			classDir->CreateParams(argv[2],argv[3],argv[4],numArgs,args);
			MakeReturnVals(interp, numArgs, args);
		}
		else if (strcmp(argv[1], "getItems") == 0) {
		  gdir->get_items(interp, argv[2], argv[3], argv[4]);
		}
	}
	else if (argc == 6) {
		if (strcmp(argv[1], "setLabel") == 0) {
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL) {
				interp->result = "Can't find view in setLabel\n";
				goto error;
			}
			view->SetLabelParam(atoi(argv[3]), atoi(argv[4]),
					    argv[5]);
		}
		else {
			interp->result = "wrong args";
			goto error;
		}
	}
	else if (argc == 7 ) {
		if (strcmp(argv[1],"setFilter") == 0){
			View *view = (View *)classDir->FindInstance(argv[2]);
			if (view == NULL){
				fprintf(stderr,"TkControl:Cmd insertWindow can't find view %s\n", argv[2]);
				Exit::DoExit(2);
			}
			VisualFilter filter;
			view->GetVisualFilter(filter);
			if (!ParseFloatDate(argv[3],filter.xLow) ||
				!ParseFloatDate(argv[4], filter.yLow) ||
				!ParseFloatDate(argv[5], filter.xHigh) ||
				!ParseFloatDate(argv[6], filter.yHigh)){
				interp->result = "invalid date or float";
				goto error;
			}
			view->SetVisualFilter(filter);
		}
	}
	else {
		interp->result = "wrong args";
		goto error;
	}

	/*
	Instance()->SetIdle();
	*/
	return TCL_OK;
error:
	/*
	Instance()->SetIdle();
	*/
	return TCL_ERROR;
}

void TkControlPanel::Run()
{
  while(Tk_DoOneEvent(TK_X_EVENTS | TK_FILE_EVENTS
		      | TK_IDLE_EVENTS | TK_DONT_WAIT) != 0);
}

/*************************************************************************/
void TkControlPanel::SubclassInsertDisplay(DeviseDisplay * /*disp*/,
					   Coord /*x*/, Coord /*y*/,
					   Coord /*w*/, Coord /*h*/)
{
  /* noop for now */
}

/************************************************************************/
void TkControlPanel::SubclassRegisterView(View *view){

}

/**********************************************************************/
void TkControlPanel::SubclassUnregisterView(View *view){
}

/************************************************************************/
void TkControlPanel::SubclassDoInit(){
}

/********************************************************************/
void TkControlPanel::SubclassDoViewPerspectiveChanged(View *view,
		Coord , Coord, Coord, Coord){
}

void TkControlPanel::SetBusy(){
	if (++_busy == 1 ){
		(void)Tcl_Eval(_interp,"ChangeStatus 1");
	}
}

void TkControlPanel::SetIdle() {
	if (--_busy == 0 )
		(void)Tcl_Eval(_interp,"ChangeStatus 0" );
	if (_busy < 0){
		fprintf(stderr,"Internal Error: TkContrlPanel::_busy < 0\n");
		Exit::DoExit(2);
	}
}

Boolean TkControlPanel::IsBusy()
{
  return (_busy > 0);
}

void TkControlPanel::FilterChanged(View *view, VisualFilter &filter,
				   int flushed)
{
#ifdef DEBUG
  printf("TkControl: filter changed\n");
#endif

  char cmd[256];
  char xLowBuf[80], yLowBuf[80], xHighBuf[80], yHighBuf[80];
  if (view->GetXAxisAttrType() == DateAttr){
    sprintf(xLowBuf, "%s", DateString(filter.xLow));
    sprintf(xHighBuf, "%s", DateString(filter.xHigh));
  } else {
    sprintf(xLowBuf, "%.2f", filter.xLow);
    sprintf(xHighBuf, "%.2f", filter.xHigh);
  }
				
  if (view->GetYAxisAttrType() == DateAttr){
    sprintf(yLowBuf, "%s", DateString(filter.yLow));
    sprintf(yHighBuf, "%s", DateString(filter.yHigh));
  } else {
    sprintf(yLowBuf, "%.2f", filter.yLow);
    sprintf(yHighBuf, "%.2f", filter.yHigh);
  }
  
  sprintf(cmd, "ProcessViewFilterChange {%s} %d {%s} {%s} {%s} {%s} 0",
	  view->GetName(),  flushed,  xLowBuf,  yLowBuf,  xHighBuf,  yHighBuf);
  (void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::ViewCreated(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewCreated {%s}", view->GetName());
  (void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::ViewDestroyed(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewDestroyed {%s}", view->GetName());
  (void)Tcl_Eval(_interp, cmd);
}

/* Make view the current view int he control panel */

void TkControlPanel::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  (void)Tcl_Eval(_interp, cmd);
}
