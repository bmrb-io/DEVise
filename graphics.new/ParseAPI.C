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
  Revision 1.45  1996/11/26 16:51:35  ssl
  Added support for piled viws

  Revision 1.44  1996/11/20 20:35:20  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.43  1996/11/20 16:50:33  jussi
  Added abortQuery command.

  Revision 1.42  1996/11/19 02:42:20  kmurli
  Changed to include original importFileType parameters..This means that
  the those using the importFileType with a single parameter cannot use
  Query interface.

  Revision 1.41  1996/11/15 10:06:17  kmurli
  Changed importFile parameters and ParseCat parameters to take in the file type
  and data file name so that a whole query can be formed if necessary for calling
  DQL type. (In case of a query schema)

  Revision 1.40  1996/11/13 16:57:07  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.39  1996/11/04 22:05:04  wenger
  Turned off debug output in ParseAPI.C; fixed compile problem in
  TDataDQLInterp.c.

  Revision 1.38  1996/11/03 18:20:46  kmurli
  Changes for incorporating DQL type..

  Revision 1.37  1996/11/03 02:41:37  kmurli
  Modified to include the query schema level. Also modified to include DQL
  processing

  Revision 1.36  1996/11/01 19:28:20  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.35  1996/10/08 23:34:38  guangshu
  Fixed bug found by cs737 class.

  Revision 1.34  1996/10/04 17:24:15  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

  Revision 1.33  1996/09/27 15:51:51  wenger
  Added "Memory leaks" and "Memory in use" menu selections and associated
  Tcl "DEVise" commands (conditionaled out for commit).

  Revision 1.32  1996/09/18 20:18:50  guangshu
  Added API command saveDisplayView to save each view in the window.

  Revision 1.31  1996/09/13 23:06:06  guangshu
  Added saveDisplayImageAndMap for saving map files.

  Revision 1.30  1996/09/10 20:07:24  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.29  1996/09/05 22:19:03  jussi
  Added setScreenSize command. Renamed connectData as getWindowImage.
  Added getDisplayImage command.

  Revision 1.28  1996/08/29 22:17:22  guangshu
  Added connectData for DEVise server.

  Revision 1.27  1996/08/08 21:02:48  beyer
  made to compile with gcc 2.6.3

  Revision 1.26  1996/08/07 15:26:21  guangshu
  Added DEVise commands getStatBuff and getAllStats to get the color statistics
  gstat histigram and global stats buffer.

  Revision 1.25  1996/08/03 15:38:15  jussi
  Flag _solid3D now has three values.

  Revision 1.24  1996/07/23 20:12:59  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.23  1996/07/23 15:40:40  jussi
  Added set/getViewDisplayDataValues and set/getViewPileMode commands.

  Revision 1.22  1996/07/21 02:20:12  jussi
  Added getViewXYZoom and setViewXYZoom commands.

  Revision 1.21  1996/07/20 18:49:09  jussi
  Added getViewSolid3D and setViewSolid3D commands.

  Revision 1.20  1996/07/18 01:16:53  jussi
  Added saveDisplayImage command.

  Revision 1.19  1996/07/14 04:05:34  jussi
  Added (kludgy) ResetVkg() function which allows ViewKGraph
  to inform ParseAPI that the current KGraph has been
  destroyed.

  Revision 1.18  1996/07/11 17:25:47  wenger
  Devise now writes headers to some of the files it writes;
  DataSourceSegment class allows non-fixed data length with non-zero
  offset; GUI for editing schema files can deal with comment lines;
  added targets to top-level makefiles to allow more flexibility.

  Revision 1.17  1996/07/10 19:01:14  jussi
  Get3DLocation now reports fix_focus and perspective flags.

  Revision 1.16  1996/07/09 16:00:16  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.15  1996/06/27 00:01:39  jussi
  Changed number of parameters of set3DLocation.

  Revision 1.14  1996/06/20 21:47:30  jussi
  Added additional parameters to the return string of command
  get3DLocation. Command set3DLocation now switches to rectangular
  coordinate system temporarily when updating camera location.

  Revision 1.13  1996/06/15 14:49:29  jussi
  Added set3DLocation command.

  Revision 1.12  1996/06/15 13:50:59  jussi
  Added get3DLocation and setMappingLegend commands.

  Revision 1.11  1996/06/12 14:56:23  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.10  1996/06/04 14:21:38  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

  Revision 1.9  1996/05/31 17:09:38  jussi
  Fixed off-by-one error in "showkgraph" command parsing.

  Revision 1.8  1996/05/31 15:38:26  jussi
  Added setLinkMaster, resetLinkMaster, and getLinkMaster commands.

  Revision 1.7  1996/05/15 16:36:20  jussi
  Added sync command.

  Revision 1.6  1996/05/15 16:02:37  wenger
  Restored 'parseSchema' command (got lost in one of the recent changes);
  added 'Bugs' file to keep track of bugs in a central area.

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
#include <assert.h>

#include "ParseAPI.h"
#include "TDataDQLInterp.h"
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
#include "DataSeg.h"
#include "Version.h"
#include "CompDate.h"
#include "DevFileHeader.h"
#include "Display.h"
#include "TDataAscii.h"
#include "DevError.h"
#include "ViewLens.h"
#include "WinClassInfo.h"
#define PURIFY 0

#ifdef PURIFY
extern "C" int purify_is_running();
extern "C" int purify_new_leaks();
extern "C" int purify_new_inuse();
#endif


//#define DEBUG
#define LINESIZE 1024

static char result[10 * 1024];
static ViewKGraph *vkg = 0;

void ResetVkg()
{
  vkg = 0;
}

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
    ClassInfo *classInfo = interp->CreateWithParams(argc - 1, &argv[1]);
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
#ifdef DEBUG
    printf("Create - return value = %s\n", name);
#endif
    control->SetIdle();
    if (!name)
      control->ReturnVal(API_ACK, "");
    else
      control->ReturnVal(API_ACK, name);
    return 1;
  }

  if (!strcmp(argv[0], "showkgraph")) {
    if (atoi(argv[1]) == 1 || !vkg) {
      /* Create a new ViewKGraph object */
      /*
	 Don't delete the previous vkg since we still want it to
	 continuously display the previously defined KGraph
      */
      vkg = new ViewKGraph;
      DOASSERT(vkg, "Out of memory");
    }
    /* Number of views */
    int nview = argc - 4;
    ViewGraph **vlist = new (ViewGraph *) [nview];
    DOASSERT(vlist, "Out of memory");
    for(int i = 0; i < nview; i++) {
      vlist[i] = (ViewGraph *)classDir->FindInstance(argv[4 + i]);
      if (!vlist[i]) {
	control->ReturnVal(API_NAK, "Cannot find view");
	delete vlist;
	return -1;
      }
    }
    vkg->Init();
    /* Add these to the ViewKGraph class and display */
    if (vkg->AddViews(vlist, nview, argv[3]) == false) {
      control->ReturnVal(API_NAK, "Could not add views to Kiviat graph");
      delete vlist;
      return -1;
    }
    delete vlist;
    if (!vkg->Display(atoi(argv[2]))) {
      control->ReturnVal(API_NAK, "Could not display Kiviat graph");
      return -1;
    }
    control->ReturnVal(API_ACK, "done");
    return 1;
  }

  if (!strcmp(argv[0], "createMappingClass")) {
    MapInterpClassInfo *interp = control->GetInterpProto();
    ClassInfo *classInfo = interp->CreateWithParams(argc - 1, &argv[1]);
    if (!classInfo) {
      control->ReturnVal(API_NAK, "Cannot create mapping class");
      return -1;
    }
    ControlPanel::RegisterClass(classInfo, true);
    control->ReturnVal(API_ACK, classInfo->ClassName());
    return 1;
  }

  if (!strcmp(argv[0], "setDefault")) {
    classDir->SetDefault(argv[1], argv[2], argc - 3, &argv[3]);
    control->ReturnVal(API_ACK, "done");
    return 1;
  }

  if (argc == 1) {
    if (!strcmp(argv[0], "date")) {
      time_t tm = time((time_t *)0);
      control->ReturnVal(API_ACK, DateString(tm));
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
    if (!strcmp(argv[0], "sync")) {
      control->SetSyncNotify();
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "version")) {
      control->ReturnVal(API_ACK, (char *) Version::Get());
      return 1;
    }
    if (!strcmp(argv[0], "copyright")) {
      control->ReturnVal(API_ACK, (char *) Version::GetCopyright());
      return 1;
    }
    if (!strcmp(argv[0], "compDate")) {
      control->ReturnVal(API_ACK, (char *) CompDate::Get());
      return 1;
    }
#if PURIFY
    if (!strcmp(argv[0], "new_leaks")) {
      if (purify_is_running()) {
	purify_new_leaks();
      }
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "new_inuse")) {
      if (purify_is_running()) {
	purify_new_inuse();
      }
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
#endif

    if (!strcmp(argv[0], "getWinCount")) {
      char buf[100];
      sprintf(buf, "%d", DevWindow::GetCount());
      control->ReturnVal(API_ACK, buf);
      return 1;
    }
  }

  if (argc == 2) {
	if (!strcmp(argv[0],"createIndex")){
		dqlCreateIndex(argv[1]);
		return 1;
	}	
    if (!strcmp(argv[0], "abortQuery")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      view->AbortQuery();
      control->ReturnVal(API_ACK, "done");
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
    if (!strcmp(argv[0], "resetLinkMaster")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      link->SetMasterView(0);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "get3DLocation")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Camera c = view->GetCamera();
      sprintf(result, "%d %g %g %g %g %g %g %g %g %g %d %d",
	      c.spherical_coord,
	      c.x_, c.y_, c.z_, c.fx, c.fy, c.fz,
	      c._theta, c._phi, c._rho,
              c.fix_focus, c._perspective);
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getLinkMaster")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      ViewGraph *view = link->GetMasterView();
      control->ReturnVal(API_ACK, (view ? view->GetName() : ""));
      return 1;
    }
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
    if (!strcmp(argv[0], "readLine")) {
      FILE *file = (FILE *)atol(argv[1]);
      (void)fgets(result, 256, file);
      int len = strlen(result);
      if (len > 0 && result[len - 1] == '\n')
	result[len - 1] = '\0';
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "tdataFileName")) {
      TData *tdata = (TData *)classDir->FindInstance(argv[1]);
      if (!tdata) {
	control->ReturnVal(API_NAK, "Cannot find tdata");
	return -1;
      }
      control->ReturnVal(API_ACK, tdata->GetName());
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
	control->ReturnVal(API_ACK, viewWin->GetName());
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
      control->ReturnVal(API_ACK, classDir->FindInstanceName(tdata));
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
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "isInterpreted")) {
      int *isInterp = (int *)classDir->UserInfo("mapping", argv[1]);
      strcpy(result, "0");
      if (isInterp && *isInterp)
	strcpy(result, "1");
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getPixelWidth")) {
      TDataMap *map= (TDataMap *)classDir->FindInstance(argv[1]);
      if (!map) {
	control->ReturnVal(API_NAK, "Cannot find mapping");
	return -1;
      }
      sprintf(result, "%d", map->GetPixelWidth());
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getTopGroups")) {
      control->GetGroupDir()->top_level_groups(result, argv[1]);
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
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
      assert(args && args[0]);
      strcpy(args[0], "recId int 1 0 0 0 0");
      
      int i;
      for(i = 0; i < numAttrs; i++) {
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
	assert(args[i + 1]);
	strcpy(args[i + 1], attrBuf);
      }
      control->ReturnVal(numAttrs + 1, args) ;
      for(i = 0; i < numAttrs + 1; i++)
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
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getLinkFlag")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      sprintf(result, "%d", link->GetFlag());
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "changeableParam")) {
      Boolean changeable = classDir->Changeable(argv[1]);
      if (changeable)
	strcpy(result, "1");
      else
	strcpy(result, "0");
      control->ReturnVal(API_ACK, result);
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
      classDir->ClassNames(argv[1], numArgs, args);
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
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
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
	ViewGraph *view = (ViewGraph *)link->Next(index);
	strcat(result, "{");
	strcat(result, view->GetName());
	strcat(result, "} ");
      }
      link->DoneIterator(index);
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getBgColor")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK,"Cannot find view");
	return -1;
      }
      GlobalColor color = view->GetBgColor();
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewStatistics")) {
      ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return status of statistics display */
      control->ReturnVal(API_ACK, vg->GetDisplayStats());
      return 1;
    }
    if (!strcmp(argv[0], "getAllStats")) {
      ViewGraph *vg = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!vg) {
        control->ReturnVal(API_NAK, "Cannot find view");
        return -1;
      }
      /* Return allStats */
      char *buff[4];
      int i;
      for(i = 0; i < 4; i++) {
	  buff[i] = '\0';
      }
      for (i = 0; i < 4; i++) {
          buff[i] = new char[64];  /*64 bytes is enough for double type */
      }
      BasicStats *allStats = vg->GetStatObj();
      sprintf(buff[0], "%g", allStats->GetStatVal(STAT_MAX));
      sprintf(buff[1], "%g", allStats->GetStatVal(STAT_MEAN));
      sprintf(buff[2], "%g", allStats->GetStatVal(STAT_MIN));
      sprintf(buff[3], "%d", (int)allStats->GetStatVal(STAT_COUNT));
#if defined(DEBUG) || 0
      printf("buff=%s %s %s %s\n", buff[0],buff[1],buff[2],buff[3]);
#endif
      control->ReturnVal(4, buff);
      for (i = 0; i < 4; i++) {
	  delete buff[i];
      }
      return 1;
    }    	
    if (!strcmp(argv[0], "getStatBuffer")) {
       enum {
           STAT_TYPE,
           HIST_TYPE,
           GSTAT_TYPE 
	} type;
        char* viewName = NULL;
        if(!strncmp(argv[1], "Stat:", 5)) {
	  type = STAT_TYPE;
	  viewName = &argv[1][5];
        } else if(!strncmp(argv[1], "Hist:", 5)) {
	  type = HIST_TYPE;
	  viewName = &argv[1][5];
        } else if(!strncmp(argv[1], "Gstat:", 6)) {
	  type = GSTAT_TYPE;
	  viewName = &argv[1][6];
        } else {
	  control->ReturnVal(API_NAK, "Invalid stat type");
	  return -1;
        }
        while( *viewName == ' ' ) {
	   viewName++;
	}
        ViewGraph *vg = (ViewGraph *)classDir->FindInstance(viewName);
        if(!vg) {
	  control->ReturnVal(API_NAK, "Cannot find view");
	  return -1;
        }

        /* Return statistics buffer */
        DataSourceBuf *buffObj = NULL;
        switch( type ) {
          case STAT_TYPE:
            buffObj = vg->GetViewStatistics();
	    break;
          case HIST_TYPE:
            buffObj = vg->GetViewHistogram();
	    break;
          case GSTAT_TYPE:
            buffObj = vg->GetGdataStatistics();
	    break;
	}
       DOASSERT(buffObj != NULL, "didn't find stats buffer\n");
      
         if (buffObj ->Open("r") != StatusOk)
         {
	     reportError("Cannot open statBuffer object for read", devNoSyserr);
	     control->ReturnVal(API_NAK, "statBuffer object");
	     return -1;
          }
          int k;
          args = new (char *) [MAX_GSTAT+1];
          for(k=0; k < MAX_GSTAT+1; k++) {
	      args[k]=0;
          }
          char statBuff[LINESIZE];
          k = 0;
          while(buffObj->Fgets(statBuff, LINESIZE) != NULL) {
	      DOASSERT(k < MAX_GSTAT, "too many stats lines");
	      int len = strlen(statBuff);
	      args[k] = new char[len + 1];
	      strcpy(args[k], statBuff);
	      k++;
           }
           control->ReturnVal(k, args);
           for(int j = 0; j < k; j++)
	       delete args[j];
           delete [] args;
           return 1;
    }
    if (!strcmp(argv[0], "getViewDimensions")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return number of dimensions in view */
      sprintf(result, "%d", vg->GetNumDimensions());
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewSolid3D")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return setting of solid/wireframe 3D objects */
      sprintf(result, "%d", vg->GetSolid3D());
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewXYZoom")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return setting of XY or X/Y zoom */
      sprintf(result, "%d", (vg->IsXYZoom() ? 1 : 0));
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewDisplayDataValues")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return setting of data value display */
      sprintf(result, "%d", (vg->GetDisplayDataValues() ? 1 : 0));
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewPileMode")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Return pile mode flag */
      sprintf(result, "%d", (vg->IsInPileMode() ? 1 : 0));
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "getViewOverrideColor")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Boolean active;
      GlobalColor color = view->GetOverrideColor(active);
      sprintf(result, "%d %d", (active ? 1 : 0),
	      (int)color);
      control->ReturnVal(API_ACK, result);
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
    if (!strcmp(argv[0], "getFileHeader")) {
      control->ReturnVal(API_ACK, DevFileHeader::Get(argv[1]));
      return 1;
    }
  }

  if (argc == 3) {

    if (!strcmp(argv[0], "setLinkMaster")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      link->SetMasterView(view);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setScreenSize")) {
      DeviseDisplay::DefaultDisplay()->DesiredScreenWidth() = atoi(argv[1]);
      DeviseDisplay::DefaultDisplay()->DesiredScreenHeight() = atoi(argv[2]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
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
      control->ReturnVal(API_ACK, result);
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
      control->ReturnVal(API_ACK, "done");
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
    if (!strcmp(argv[0], "setViewSolid3D")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Set solid or wireframe 3D objects */
      vg->SetSolid3D(atoi(argv[2]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setViewXYZoom")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Set XY or X/Y zoom */
      vg->SetXYZoom(atoi(argv[2]) ? true : false);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setViewDisplayDataValues")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Set data value display on/off */
      vg->SetDisplayDataValues(atoi(argv[2]) ? true : false);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "setViewPileMode")) {
      View *vg = (View *)classDir->FindInstance(argv[1]);
      if (!vg) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      /* Set pile mode flag */
      vg->SetPileMode(atoi(argv[2]) ? true : false);
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
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "replaceView")) {
      View *view1 = (View *)classDir->FindInstance(argv[1]);
      View *view2 = (View *)classDir->FindInstance(argv[2]);
      if (!view1 || !view2) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      ViewWin *win = view1->GetParent();
      if (!win) {
	control->ReturnVal(API_NAK, "Cannot find window");
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
	control->ReturnVal(API_ACK, label->GetName());
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
      link->SetFlag(flag);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "highlightView")) {
      View *view = (View *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      view->Highlight(atoi(argv[2]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "get")) {
      classDir->InstanceNames(argv[1], argv[2], numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "getparam")) {
      classDir->GetParams(argv[1], argv[2], numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
// MODIFIED BY SSL NOV 3 -BEGIN
    if (!strcmp(argv[0], "insertViewInLens")) { 
      ViewLens *lens = (ViewLens *)classDir->FindInstance(argv[1]);
      if (!lens) {
	control->ReturnVal(API_NAK, "Cannot find lens");
	return -1;
      }
      View *view = (View *)classDir->FindInstance(argv[2]);
      if (!view){
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      lens->InsertView(view);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
// MODIFIED BY SSL NOV 3 - END
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
      control->ReturnVal(API_ACK, view->GetMappingLegend(map));
      return 1;
    }
    if (!strcmp(argv[0], "insertLink")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
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
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      if (link->ViewInLink(view))
	strcpy(result, "1");
      else
	strcpy(result , "0");
      control->ReturnVal(API_ACK, result);
      return 1;
    }
    if (!strcmp(argv[0], "unlinkView")) {
      VisualLink *link = (VisualLink *)classDir->FindInstance(argv[1]);
      if (!link) {
	control->ReturnVal(API_NAK, "Cannot find link");
	return -1;
      }
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[2]);
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
    if (!strcmp(argv[0], "saveDisplayImage")) {
      DisplayExportFormat format = POSTSCRIPT;
      if (!strcmp(argv[1], "eps"))
	format = EPS;
      else if (!strcmp(argv[1], "gif"))
	format = GIF;
      DeviseDisplay::DefaultDisplay()->ExportImage(format, argv[2]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "saveDisplayView")) {
      DisplayExportFormat format = POSTSCRIPT;
      if (!strcmp(argv[1], "eps"))
	 format = EPS;
      else if (!strcmp(argv[1], "gif"))
	 format = GIF;
      DeviseDisplay::DefaultDisplay()->ExportView(format, argv[2]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "saveTdata")) {
      TData *tdata = (TData *)classDir->FindInstance(argv[1]);
      if (!tdata) {
	control->ReturnVal(API_NAK, "Cannot find tdata");
	return -1;
      }
      if (tdata->Save(argv[2]) != StatusFailed) {
        control->ReturnVal(API_ACK, "done");
        return 1;
      } else {
	control->ReturnVal(API_NAK, "Cannot save tdata");
	return -1;
      }
    }
    if (!strcmp(argv[0], "getDisplayImage")) {
      int port = atoi(argv[1]);
      control->OpenDataChannel(port);
      if (strcmp(argv[2], "gif")) {
         control->ReturnVal(API_NAK, "Can only support GIF now.");
         return -1;
      }
      int fd = control->getFd();
      if (fd < 0) {
        control->ReturnVal(API_NAK, "Invalid socket to write");
        return -1;
      }
      FILE *fp = fdopen(control->getFd(), "wb");
      DeviseDisplay::DefaultDisplay()->ExportGIF(fp, 0);
      close(control->getFd());
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
  }

  if (argc == 4) {

    if (!strcmp(argv[0], "importFileType")) {
      char *name = ParseCat(argv[1],argv[2],argv[3]);
      if (!name) {
	strcpy(result , "");
	control->ReturnVal(API_NAK, result);
	return -1;
      }
      control->ReturnVal(API_ACK, name);
      return 1;
    }

    if (!strcmp(argv[0], "setMappingLegend")) {
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
      view->SetMappingLegend(map, argv[3]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
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
    if (!strcmp(argv[0], "getWindowImage")) {
      int port = atoi(argv[1]);
      control->OpenDataChannel(port);
      if (strcmp(argv[2], "gif")) {
         control->ReturnVal(API_NAK, "Can only support GIF now.");
         return -1;
      }
      ViewWin *viewWin = (ViewWin *)classDir->FindInstance(argv[3]);
      if (!viewWin) {
        control->ReturnVal(API_NAK, "Cannot find window");
        return -1;
      }
      int fd = control->getFd();
      if (fd < 0) {
        control->ReturnVal(API_NAK, "Invalid socket to write");
        return -1;
      }
      FILE *fp = fdopen(control->getFd(), "wb");
      viewWin->GetWindowRep()->ExportGIF(fp);
      close(control->getFd());
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
    if (!strcmp(argv[0], "swapView")) {
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
      classDir->CreateParams(argv[1], argv[2], argv[3], numArgs, args);
      control->ReturnVal(numArgs, args);
      return 1;
    }
    if (!strcmp(argv[0], "getItems")) {
      control->GetGroupDir()->get_items(result, argv[1], argv[2], argv[3]);
      control->ReturnVal(API_ACK, result);
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
      viewWin->ExportImage(format, argv[3]);
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
      view->SetOverrideColor((GlobalColor) atoi(argv[3]), active);
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
    if (!strcmp(argv[0], "parseSchema")) {
      char *name = ParseSchema(argv[1], argv[2], argv[3]);
      if (!name) {
	control->ReturnVal(API_NAK, "Cannot parse schema(s)");
	return -1;
      }
      control->ReturnVal(API_ACK, name);
      return 1;
    }
  }

  if (argc == 5) {

	if(!strcmp(argv[0],"importFileDQLType")){
	  char * name = ParseDQL(argv[1],argv[2],argv[3],"DQL",argv[2],argv[4]); 
	  if (!name){
		strcpy(result,"");
		control->ReturnVal(API_NAK, result);
		return -1;
	  }
      control->ReturnVal(API_ACK, name);
      return 1;
    }
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
    if (!strcmp(argv[0], "dataSegment")) {
      DataSeg::Set(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]));
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
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
      if (!ParseFloatDate(argv[2], filter.xLow) ||
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
    if(!strcmp(argv[0], "saveDisplayImageAndMap")) {
      DisplayExportFormat format = POSTSCRIPT;
      if (!strcmp(argv[1], "eps"))
        format = EPS;
      else if (!strcmp(argv[1], "gif"))
        format = GIF;
      DeviseDisplay::DefaultDisplay()->ExportImageAndMap(format, argv[2], 
					argv[3], argv[4], argv[5]);
      control->ReturnVal(API_ACK, "done");
      return 1;
    }

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
  }

  if (argc == 8) {
    if (!strcmp(argv[0], "set3DLocation")) {
      ViewGraph *view = (ViewGraph *)classDir->FindInstance(argv[1]);
      if (!view) {
	control->ReturnVal(API_NAK, "Cannot find view");
	return -1;
      }
      Camera c = view->GetCamera();
      Boolean isSphere = c.spherical_coord;
      c.spherical_coord = false;
      c.x_ = atof(argv[2]);
      c.y_ = atof(argv[3]);
      c.z_ = atof(argv[4]);
      c.fx = atof(argv[5]);
      c.fy = atof(argv[6]);
      c.fz = atof(argv[7]);
      view->SetCamera(c);
      if (isSphere) {
	c = view->GetCamera();
	c.spherical_coord = true;
	view->SetCamera(c);
      }
      control->ReturnVal(API_ACK, "done");
      return 1;
    }
  }

  fprintf(stderr, "Unrecognized command: %s\n", argv[0]);
  fprintf(stderr, "argc = %d\n", argc);
  control->ReturnVal(API_NAK, "Unrecognized command");
  return -1;
}

