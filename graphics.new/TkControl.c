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
  Revision 1.52  1996/05/13 21:56:16  jussi
  Moved initialization of _mode to Control.c. Changed code to
  reflect new interface with batch files and XDisplay.

  Revision 1.51  1996/05/13 18:11:20  jussi
  Emptied ViewCreated and ViewDestroyed and moved them to
  the header file.

  Revision 1.50  1996/05/11 20:51:49  jussi
  Removed some unnecessary code.

  Revision 1.49  1996/05/11 17:28:07  jussi
  Moved all API command parsing to ParseAPI.C so that TkControl.c
  and ServerAPI.c would not have to duplicate the effort.
  Reorganized the code somewhat to match the ParseAPI interface.

  Revision 1.48  1996/05/11 03:13:34  jussi
  Added clearAll command verb. Removed openSession and openTemplate
  command verbs. Removed control panel variables like fileName
  and fileAlias.

  Revision 1.47  1996/05/07 16:38:16  jussi
  Made changes to reflect the new interface with Action/ActionDefault.
  Action is now a member variable of ViewGraph, not View.

  Revision 1.46  1996/04/22 21:38:07  jussi
  Fixed problem with simultaneous view refresh and record query
  activities. Previously, there was a single iterator over the
  mappings of a view, which caused the system to crash when a record
  was queried while the data was still being displayed. Each activity
  now gets its own iterator.

  Revision 1.45  1996/04/19 17:22:22  wenger
  Put the GenClassInfo code back in -- this is needed for tape data;
  started adding the tape-related code back in (it was previously
  deleted for some reason; I'm not yet done adding it back); added
  the 'DEVise parseSchema' command and the first parts of the code
  related to it.

  Revision 1.44  1996/04/18 18:14:20  jussi
  The Tcl/Tk file interpreted in batch mode is now batch.tcl.

  Revision 1.43  1996/04/16 19:46:23  jussi
  Added DoAbort() method.

  Revision 1.42  1996/04/15 15:08:11  jussi
  Interface to ViewGraph's mapping iterator has changed. Added another
  version of insertMapping command that takes the label as a parameter.

  Revision 1.41  1996/04/14 00:17:15  jussi
  Removed interpMapClassInfo and clearInterp commands. Added
  createMappingClass which will eventually replace createInterp
  complete. createInterp is retained for backward compatibility.

  Revision 1.40  1996/04/11 17:53:45  jussi
  Added command verbs raiseView and lowerView.

  Revision 1.39  1996/04/10 15:30:07  jussi
  Added removeMapping command verb.

  Revision 1.38  1996/04/09 22:52:25  jussi
  Added getViewOverrideColor and setViewOverrideColor.

  Revision 1.37  1996/03/26 19:09:17  jussi
  Disabled debugging.

  Revision 1.36  1996/03/26 17:20:03  jussi
  Streamlined DEVise getSchema command.

  Revision 1.35  1996/03/25 22:59:46  jussi
  Reverted back to the state where maps are inserted to views but
  views are not inserted to maps. Currently TDataMap only stores
  one view which is not enough.

  Revision 1.34  1996/03/22 18:25:47  jussi
  Fixed problem with insertWindow.

  Revision 1.33  1996/03/07 16:54:39  jussi
  Added association of TDataMap and ViewGraph.

  Revision 1.32  1996/01/27 00:21:22  jussi
  Added support for postscript execution; scripts which are
  executed after query processor has evaluated all queries
  and system is idle.

  Revision 1.31  1996/01/19 18:59:02  jussi
  Added ClearTopGroups.

  Revision 1.30  1996/01/17 20:53:51  jussi
  Added support for additional image export formats.

  Revision 1.29  1996/01/13 20:51:20  jussi
  Added references to www_extract.

  Revision 1.28  1996/01/12 15:25:05  jussi
  Replaced libc.h with stdlib.h.

  Revision 1.27  1996/01/10 18:47:36  jussi
  Attribute hi/lo values are now conditional; a boolean value
  returned from getSchema indicates which values are default
  and which are not.

  Revision 1.26  1996/01/10 00:37:40  jussi
  Added support for hi/lo values defined in schema.

  Revision 1.25  1996/01/09 16:36:26  jussi
  Added references to Seq.c and seq_extract.

  Revision 1.24  1995/12/28 20:01:59  jussi
  Small fix to remove compiler warning.

  Revision 1.23  1995/12/14 18:07:29  jussi
  Small fixes to get rid of g++ -Wall warnings.

  Revision 1.22  1995/12/14 15:37:20  jussi
  Minor fixes.

  Revision 1.21  1995/12/14 15:27:44  jussi
  Changed getViewStatistics to reflect the new scheme of turning
  multiple stats on/off in one setViewStatistics call.

  Revision 1.20  1995/12/08 23:47:12  ravim
  Window name is a parameter to KGraph creation. Previous KGraph not deleted
  when the new one is created.

  Revision 1.19  1995/12/07 02:18:29  ravim
  The set of stats to be displayed is specified as a parameter to
  setViewStatistics.

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
#include <stdlib.h>

#include "TkControl.h"
#include "ParseCat.h"

//#define DEBUG

#ifdef TK_WINDOW
Tcl_Interp *ControlPanelTclInterp = 0;
Tk_Window ControlPanelMainWindow = 0;
#endif

extern int extractStocksCmd(ClientData clientData, Tcl_Interp *interp,
			    int argc, char *argv[]);
extern int comp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);
extern int crsp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);
extern int seq_extract(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);
extern int www_extract(ClientData clientData, Tcl_Interp *interp,
		       int argc, char *argv[]);

MapInterpClassInfo *TkControlPanel::_interpProto = 0;

ControlPanel *GetTkControl()
{
  return new TkControlPanel();
}

TkControlPanel::TkControlPanel()
{
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);

  _busy = false;
  _argv0 = CopyString(Init::ProgName());

  _interp = Tcl_CreateInterp();
  _mainWindow = Tk_CreateMainWindow(_interp, 0, "DEVise", "DEVise");
  if (!_mainWindow) {
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
  Tk_MoveWindow(_mainWindow, 0, 0);
  Tk_GeometryRequest(_mainWindow, 100, 200);

#ifdef TK_WINDOW
  ControlPanelTclInterp = _interp;
  ControlPanelMainWindow = _mainWindow;
#endif

  if (Tcl_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tcl.\n");
    Exit::DoExit(1);
  }
  if (Tk_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tk.\n");
    Exit::DoExit(1);
  }

  /* register with dispatcher to be always informed of when to run */
  Dispatcher::Current()->Register(this, AllState, true);
}

void TkControlPanel::StartSession()
{
  printf("DEVise Data Visualization Software\n");
  printf("(c) Copyright 1992-1996\n");
  printf("By the DEVise Development Group\n");
  printf("All Rights Reserved.\n");
  printf("\n");

  Tcl_LinkVar(_interp, "argv0", (char *)&_argv0, TCL_LINK_STRING);

  /* Create a new tcl command for control panel */
  Tcl_CreateCommand(_interp, "DEVise", DEViseCmd, this, 0);

  /* Create a new tcl command for ISSM stock data */
  Tcl_CreateCommand(_interp, "issm_extractStocks", extractStocksCmd, 0, 0);

  /* Create a new tcl command for Compustat data */
  Tcl_CreateCommand(_interp, "cstat_extract", comp_extract, 0, 0);

  /* Create a new tcl command for CRSP data */
  Tcl_CreateCommand(_interp, "crsp_extract", crsp_extract, 0, 0);

  /* Create a new tcl command for SEQ data */
  Tcl_CreateCommand(_interp, "seq_extract", seq_extract, 0, 0);

  /* Create a new tcl command for WWW data */
  Tcl_CreateCommand(_interp, "www_extract", www_extract, 0, 0);

  char *controlFile = "control.tk";
  if (Init::BatchFile()) {
    controlFile = "batch.tcl";
    SetBatchMode(true);
  }

  char *envPath = getenv("DEVISE_LIB");
  char *control;
  char buf[256];
  if (envPath) {
    sprintf(buf, "%s/%s", envPath, controlFile);
    control = buf;
  } else
    control = controlFile;

  printf("Control panel file is: %s\n", control);

  int code = Tcl_EvalFile(_interp, control);
  if (code != TCL_OK) {
    fprintf(stderr,"%s\n", _interp->result);
    Exit::DoExit(1);
  }

  if (Init::Restore()) {
    /* restore session */
    Tcl_SetVar(_interp, "restoring", "1", 0);
    char *sessionName = Init::SessionName();
    int code = Tcl_EvalFile(_interp, sessionName);
    Tcl_SetVar(_interp, "restoring", "0", 0);
    if (code != TCL_OK) {
      fprintf(stderr, "Can't restore session file %s\n", sessionName);
      fprintf(stderr, "%s\n", _interp->result);
      Exit::DoExit(1);
    }
  }

  if (Init::BatchFile())
    SetSyncNotify();
}

void TkControlPanel::DestroySessionData()
{
#ifdef DEBUG
  printf("Destroying session data\n");
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

void TkControlPanel::DoAbort(char *reason)
{
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  (void)Tcl_Eval(_interp, cmd);
}

int TkControlPanel::DEViseCmd(ClientData clientData, Tcl_Interp *interp,
			       int argc, char *argv[])
{
  // don't pass DEVise command verb (argv[0])
  if (ParseAPI(argc - 1, &argv[1], (ControlPanel *)clientData) < 0)
    return TCL_ERROR;

  return TCL_OK;
}

void TkControlPanel::Run()
{
  while(Tk_DoOneEvent(TK_X_EVENTS | TK_FILE_EVENTS
		      | TK_IDLE_EVENTS | TK_DONT_WAIT) != 0);
}

void TkControlPanel::SetBusy()
{
  if (++_busy == 1)
    (void)Tcl_Eval(_interp, "ChangeStatus 1");
}

void TkControlPanel::SetIdle()
{
  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0)
    (void)Tcl_Eval(_interp, "ChangeStatus 0");
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
  (void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  (void)Tcl_Eval(_interp, cmd);
}

void TkControlPanel::SyncNotify()
{
  char *batchFile = Init::BatchFile();

  if (!batchFile)
    return;

  char cmd[256];
  int code = Tcl_EvalFile(_interp, batchFile);
  if (code != TCL_OK) {
    fprintf(stderr, "Cannot execute batch file %s\n", batchFile);
    fprintf(stderr,"%s\n", _interp->result);
    Exit::DoExit(1);
  }

  ClearSyncNotify();
}
