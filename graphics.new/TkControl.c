/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1999
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
  Revision 1.90  1999/09/07 19:00:57  wenger
  dteInsertCatalogEntry command changed to tolerate an attempt to insert
  duplicate entries without causing a problem (to allow us to get rid of
  Tcl in session files); changed Condor session scripts to take out Tcl
  control statements in data source definitions; added viewGetImplicitHome
  and related code in Session class so this gets saved in session files
  (still no GUI for setting this, though); removed SEQ-related code.

  Revision 1.89  1999/09/02 17:26:06  wenger
  Took out the ifdefs around the MARGINS code, since DEVise won't compile
  without them; removed all of the TK_WINDOW code, and removed various
  unnecessary includes of tcl.h, etc.

  Revision 1.88  1999/07/12 19:02:11  wenger
  Got DEVise to compile and run again on Linux (including Tcl/Tk 8.0).

  Revision 1.87  1999/06/25 15:58:24  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

  Revision 1.86  1998/11/04 20:34:02  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.85  1998/09/28 20:05:55  wenger
  Fixed bug 383 (unnecessary creation of QueryProc); moved all
  DestroySessionData() code from subclasses of ControlPanel into base class,
  because it was all the same; found and fixed bug 398 (caused by a change
  in the propagation of view selections).

  Revision 1.84  1998/05/02 09:00:52  taodb
  Added support for JAVA Screen and command logging

  Revision 1.83  1998/03/02 22:03:40  taodb
  Add control parameter to Run() invocations

  Revision 1.82  1998/02/26 20:49:05  taodb
  Replaced ParseAPI() with Command Object Interface

  Revision 1.81  1998/02/12 17:17:09  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.80  1998/01/30 02:17:07  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.79.2.2  1998/02/12 05:04:10  taodb
  *** empty log message ***

  Revision 1.79.2.1  1998/01/28 22:44:03  taodb
  Added support for group communicatoin

  Revision 1.79  1998/01/07 19:29:58  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.78  1997/12/11 04:25:46  beyer
  Shared memory and semaphores are now released properly when devise
  terminates normally.

  Revision 1.77.12.2  1998/01/16 23:41:46  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.77.12.1  1998/01/07 16:00:22  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.77  1997/02/03 19:45:36  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.76  1997/01/27 20:15:14  wenger
  Workaround to bug 137: disables Stack Control dialog buttons while drawing.

  Revision 1.75  1997/01/23 17:02:23  wenger
  Changed startup code in TkControl so that '-sharedMem' argument works
  in batch mode.

  Revision 1.74  1996/12/20 16:26:26  jussi
  Removed call to SemaphoreV::create().

  Revision 1.73  1996/12/15 20:21:53  wenger
  Added '-noshm' command line flag to allow user to disable shared memory;
  temporarily disabled RTree stuff.

  Revision 1.72  1996/12/15 06:48:55  donjerko
  Added support for RTrees and moved DQL sources to DTE/DeviseSpecific dir

  Revision 1.71  1996/12/13 21:33:42  jussi
  Updated to use SemaphoreV::maxNumSemaphores().

  Revision 1.70  1996/12/12 22:03:32  jussi
  Removed calls to semaphore and shared memory DestroyAll().

  Revision 1.69  1996/12/03 20:34:35  jussi
  Added initialization of semaphore structures.

  Revision 1.68  1996/11/18 18:10:57  donjerko
  New files and changes to make DTE work with Devise

  Revision 1.67  1996/11/07 00:10:27  jussi
  Tk main window is not created when monolithic Devise is run
  in batch mode.

  Revision 1.66  1996/08/29 22:22:27  guangshu
  Adde quiet varible to make client quiet.

  Revision 1.65  1996/08/05 17:26:13  beyer
  changed filter reporting format to %g (from %.2f)

  Revision 1.64  1996/08/04 21:29:52  beyer
  Added Raise() to raise the control panel to the top of the stacking order.

  Revision 1.63  1996/07/18 01:17:47  jussi
  Added call to DestroySessionData() of base class.

  Revision 1.62  1996/07/15 17:02:07  jussi
  Added support for string attributes in GData.

  Revision 1.61  1996/07/15 14:23:39  jussi
  Made one #ifdef statement more flexible, allowing for future
  versions of Tcl/Tk.

  Revision 1.60  1996/07/15 01:03:47  jussi
  Code now compatible with tk4.1. Compatibility with tk4.0 maintained
  for a short while.

  Revision 1.59  1996/07/11 19:38:35  jussi
  The Tcl variable 'file' is set to the name of the session file
  before the session file is executed. Exported session files
  with data use the 'file' variable.'

  Revision 1.58  1996/07/09 16:00:22  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.57  1996/07/01 19:24:42  jussi
  The Web data transfer routines are now caller from DataSourceWeb.c,
  and not TkControl.c. The Web interface is now in the server, not
  in the client.

  Revision 1.56  1996/06/24 19:47:05  jussi
  TkControl now passes the fd of the open X connection to the
  Dispatcher. Added a call to Run() to a few places which are
  output-only because Tcl/Tk doesn't otherwise have a chance
  to flush the output to the screen.

  Revision 1.55  1996/06/12 14:56:37  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.54  1996/05/22 21:04:31  jussi
  ControlPanel::_controlPanel is now set by main program.

  Revision 1.53  1996/05/15 16:41:42  jussi
  TkControl now uses the new server synchronization mechanism.

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
#include "View.h"
#include "Util.h"
#include "Init.h"
#include "Version.h"
#include "DCE.h"
#include "CmdContainer.h"
#include "DebugLog.h"

//#define DEBUG

extern int extractStocksCmd(ClientData clientData, Tcl_Interp *interp,
			    int argc, char *argv[]);
extern int comp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);
extern int crsp_extract(ClientData clientData, Tcl_Interp *interp,
			int argc, char *argv[]);

MapInterpClassInfo *TkControlPanel::_interpProto = 0;

int _quiet;

ControlPanel *GetNewControl()
{
  return new TkControlPanel;
}

TkControlPanel::TkControlPanel()
{
	
  _interpProto = new MapInterpClassInfo();
  cmdContainerp = new CmdContainer(this,CmdContainer::MONOLITHIC, NULL);
  
  View::InsertViewCallback(this);

  _busy = 0;
  _argv0 = CopyString(Init::ProgName());

  _interp = Tcl_CreateInterp();
  if (Tcl_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tcl. Is TCL_LIBRARY pointing to\n");
    fprintf(stderr, "the directory with the Tcl initialization files?\n");
    Exit::DoExit(1);
  }

  if (!Init::UseSharedMem()) {
    fprintf(stderr, "Proceeding without shared memory and semaphores.\n");
  }

  if (Init::BatchFile()) {
    // In batch mode, we don't need a Tk main window
    _mainWindow = 0;
    return;
  }

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
  _mainWindow = Tk_CreateMainWindow(_interp, 0, "DEVise", "DEVise");
  if (!_mainWindow) {
    fprintf(stderr, "%s\n", _interp->result);
    exit(1);
  }
  Tk_MoveWindow(_mainWindow, 0, 0);
  Tk_GeometryRequest(_mainWindow, 100, 200);
#endif

  if (Tk_Init(_interp) == TCL_ERROR) {
    fprintf(stderr, "Cannot initialize Tk. Is TK_LIBRARY pointing to\n");
    fprintf(stderr, "the directory with the Tk initialization files?\n");
    Exit::DoExit(1);
  }

#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION > 0
  _mainWindow = Tk_MainWindow(_interp);
#endif

#if TK_MAJOR_VERSION != 4
  _mainWindow = Tk_MainWindow(_interp);
#endif

  int fd = ConnectionNumber(Tk_Display(_mainWindow));
  Dispatcher::Current()->Register(this, 10, GoState, true, fd);
}

TkControlPanel::~TkControlPanel()
{
  Dispatcher::Current()->Unregister(this);
}

// Note: this complication is so that the Version class doesn't directly
// refer to the DebugLog class, which goofs up the devisec link.  RKW
// 1999-06-24.
static void
LogFn(char *msg)
{
  DebugLog::DefaultLog()->Message(msg);
}

void TkControlPanel::StartSession()
{
  Version::PrintInfo(LogFn);
  printf("\n");

  Tcl_LinkVar(_interp, "argv0", (char *)&_argv0, TCL_LINK_STRING);
  Tcl_LinkVar(_interp, "quiet", (char *)&_quiet, TCL_LINK_INT);

  /* Create a new tcl command for control panel */
  Tcl_CreateCommand(_interp, "DEVise", DEViseCmd, this, 0);

  /* Create a new tcl command for ISSM stock data */
  Tcl_CreateCommand(_interp, "issm_extractStocks", extractStocksCmd, 0, 0);

  /* Create a new tcl command for Compustat data */
  Tcl_CreateCommand(_interp, "cstat_extract", comp_extract, 0, 0);

  /* Create a new tcl command for CRSP data */
  Tcl_CreateCommand(_interp, "crsp_extract", crsp_extract, 0, 0);

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
    char *sessionName = Init::SessionName();
    char buf[MAXPATHLEN + 256];
    sprintf(buf, "DEVise openSession %s", sessionName);
    int code = Tcl_Eval(_interp, buf);
    if (code != TCL_OK) {
      fprintf(stderr, "Can't restore session file %s\n", sessionName);
      fprintf(stderr, "%s\n", _interp->result);
      Exit::DoExit(1);
    }
  }

  Run();

  if (Init::BatchFile())
    SetSyncNotify();
}

void TkControlPanel::DoAbort(char *reason)
{
  char cmd[256];
  sprintf(cmd, "AbortProgram {%s}", reason);
  (void)Tcl_Eval(_interp, cmd);
  Run();
}

int TkControlPanel::DEViseCmd(ClientData clientData, Tcl_Interp *interp,
			       int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Receiving command from client: ");
  int argnum;
  for (argnum = 0; argnum < argc; argnum++) {
    printf("{%s} ", argv[argnum]);
  }
  printf("\n");
#endif

  // don't pass DEVise command verb (argv[0])
  if (cmdContainerp->RunOneCommand(argc - 1, 
		&argv[1],(ControlPanel *)clientData) < 0)
    return TCL_ERROR;

  return TCL_OK;
}

void TkControlPanel::Run()
{
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
  while(Tk_DoOneEvent(TK_X_EVENTS | TK_FILE_EVENTS
		      | TK_IDLE_EVENTS | TK_DONT_WAIT) != 0);
#else
  while(Tcl_DoOneEvent(TCL_WINDOW_EVENTS | TCL_FILE_EVENTS
                       | TCL_IDLE_EVENTS | TCL_DONT_WAIT) != 0);
#endif
}

void TkControlPanel::SetBusy()
{
  if (++_busy == 1) {
    (void)Tcl_Eval(_interp, "ChangeStatus 1");
    Run();
  }
}

void TkControlPanel::SetIdle()
{
  DOASSERT(_busy >= 0, "Control panel unexpectedly busy");
  if (_busy == 0) {
    return;
  }
  if (--_busy == 0) {
    (void)Tcl_Eval(_interp, "ChangeStatus 0");
    Run();
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
  if (view->GetXAxisAttrType() == DateAttr) {
    sprintf(xLowBuf, "%s", DateString(filter.xLow));
    sprintf(xHighBuf, "%s", DateString(filter.xHigh));
  } else {
    sprintf(xLowBuf, "%g", filter.xLow);
    sprintf(xHighBuf, "%g", filter.xHigh);
  }
				
  if (view->GetYAxisAttrType() == DateAttr) {
    sprintf(yLowBuf, "%s", DateString(filter.yLow));
    sprintf(yHighBuf, "%s", DateString(filter.yHigh));
  } else {
    sprintf(yLowBuf, "%g", filter.yLow);
    sprintf(yHighBuf, "%g", filter.yHigh);
  }
  
  sprintf(cmd, "ProcessViewFilterChange {%s} %d {%s} {%s} {%s} {%s} 0",
	  view->GetName(), flushed, xLowBuf, yLowBuf, xHighBuf, yHighBuf);
  (void)Tcl_Eval(_interp, cmd);
  Run();
}

void TkControlPanel::SelectView(View *view)
{
  char cmd[256];
  sprintf(cmd, "ProcessViewSelected {%s}", view->GetName());
  (void)Tcl_Eval(_interp, cmd);
  Run();
}

void
TkControlPanel::ShowMouseLocation(char *dataX, char *dataY)
{
#if defined(DEBUG)
  printf("TkControlPanel::ShowMouseLocation(%s, %s)\n", dataX, dataY);
#endif

  char cmd[256];
  sprintf(cmd, "ShowMouseLocation {%s} {%s}", dataX, dataY);
  (void)Tcl_Eval(_interp, cmd);
  Run();
}

void TkControlPanel::SyncNotify()
{
  char *batchFile = Init::BatchFile();

  if (!batchFile)
    return;

  printf("Executing batch file %s\n", batchFile);
  char cmd[256];
  int code = Tcl_EvalFile(_interp, batchFile);
  if (code != TCL_OK) {
    fprintf(stderr, "Cannot execute batch file %s\n", batchFile);
    fprintf(stderr,"%s\n", _interp->result);
    Exit::DoExit(1);
  }

  ClearSyncNotify();
}


void TkControlPanel::Raise()
{
  (void)Tcl_Eval(_interp, "raise .");
  Run();
}

void TkControlPanel::NotifyFrontEnd(char *script) 
{
  if (Tcl_Eval(_interp, script) != TCL_OK) {
    fprintf(stderr, "Cannot execute %s: %s\n",
            script, _interp->result);
  }
}

