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
  Implementation of Session class (for opening as saving sessions).
 */

/*
  $Id$

  $Log$
  Revision 1.57  1999/08/13 17:22:31  wenger
  Custom view layouts are now saved to session files; removed now unused
  TileLayout code.

  Revision 1.56  1999/07/21 18:51:00  wenger
  Moved alignment and data font information from view into mapping.

  Revision 1.55  1999/07/14 18:42:38  wenger
  Added the capability to have axes without ticks and tick labels.

  Revision 1.54  1999/06/11 14:46:52  wenger
  Added the capability (mostly for the JavaScreen) to disable rubberband
  lines, cursor movement, drill down, and key actions in views (the code
  to send this info to the JS is still conditionaled out until the JS is
  ready for it).

  Revision 1.53  1999/05/17 18:37:35  wenger
  Views now have GData sending configuration that is only employed when
  connecting to the JavaScreen (eliminates the need for the current kludgey
  setup to send GData to the JS).

  Revision 1.52  1999/05/14 16:46:33  wenger
  View vertical scroll can now be configured by the user.

  Revision 1.51  1999/05/14 13:59:52  wenger
  User can now control data font family, weight, and slant, on a per-view
  basis.

  Revision 1.50  1999/04/29 17:36:31  wenger
  Implemented 'fixed cursor size' option (for the sake of the JavaScreen).

  Revision 1.49  1999/04/26 20:25:02  wenger
  Fixed bug 486.

  Revision 1.48  1999/04/22 19:29:09  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.47  1999/03/24 17:26:04  wenger
  Non-DTE data source code prevents adding duplicate data source names;
  added "nice axis" feature (sets axis limits to multiples of powers of
  10 if enabled); improved the propagation of DEVise errors back to the
  GUI; fixed bug 474 (problem with view home).

  Revision 1.46  1999/03/12 18:45:39  wenger
  Implemented duplicate symbol elimination.

  Revision 1.45  1999/03/04 15:10:47  wenger
  Implemented 'automatic filter update' features: views can be designated
  to have their visual filters automatically updated with the 'Update
  Filters' menu selection; alternatively, a session can be opened with
  the 'Open, Update, and Save' selection, which updates the designated
  filters and saves the updated session.

  Revision 1.44  1999/03/01 17:47:32  wenger
  Implemented grouping/ungrouping of views to allow custom view geometries.

  Revision 1.43  1999/02/11 19:54:34  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.42.2.1  1999/02/11 18:24:02  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.42  1998/12/22 19:39:08  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.41  1998/12/18 19:46:59  wenger
  Oops!  Fixed bug in saving sessions resulting from the elimination of
  the getAxis and setAxis commands.

  Revision 1.40  1998/12/11 18:07:13  wenger
  Tempararily disabled saving of color palette because of bug 441.

  Revision 1.39  1998/12/08 20:01:47  wenger
  Color palette is now saved in session files.

  Revision 1.38  1998/12/01 20:04:01  wenger
  More reductions of memory usage in DEVise -- basically eliminated the
  histogram capability (this really saves a lot, since there are big
  structures in every ViewGraph for this); made creation of TDatas more
  efficient by bypassing command code.

  Revision 1.37  1998/11/19 21:13:26  wenger
  Implemented non-DTE version of DEVise (new code handles data source catalog
  functions; Tables, SQLViews, etc., are not implemented); changed version to
  1.6.0.

  Revision 1.36  1998/11/16 18:58:59  wenger
  Added options to compile without DTE code (NO_DTE), and to warn whenever
  the DTE is called (DTE_WARN).

  Revision 1.35  1998/11/11 14:30:42  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.34  1998/11/06 17:59:35  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.33  1998/10/28 19:22:15  wenger
  Added code to check all data sources (runs through the catalog and tries
  to open all of them); this includes better error handling in a number of
  data source-related areas of the code; also fixed errors in the propagation
  of command results.

  Revision 1.32  1998/10/20 19:46:00  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.31  1998/09/30 17:44:32  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.30  1998/09/22 17:23:42  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.29  1998/09/08 20:25:58  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

  Revision 1.28  1998/08/10 19:08:22  wenger
  Moved command result buffer from DeviseCommand class to ControlPanel
  class -- saves 7 MB of memory!

  Revision 1.27  1998/06/10 18:04:03  wenger
  Improved new cursor drawing (bug in Mesa implementation sometimes makes
  it look ugly); cursor color now saved in sessions.

  Revision 1.26  1998/06/04 22:07:17  wenger
  View symbol alignment is now saved in session files.

  Revision 1.25  1998/05/14 18:21:08  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.24  1998/05/05 15:14:47  zhenhai
  Implemented 3D Cursor as a rectangular block in the destination view
  showing left, right, top, bottom, front and back cutting planes of the
  source view.

  Revision 1.23  1998/05/02 09:02:29  taodb
  Added support for command logging
  Added support for registering events with delay

  Revision 1.22  1998/04/19 22:30:12  wenger
  Modified CreateTData() so that it works on "regular" and "internal"
  data sources.

  Revision 1.21  1998/04/16 20:07:09  wenger
  Changed Session::CreateTData() back so UNIXFILEs work.

  Revision 1.20  1998/04/16 16:53:18  wenger
  Temporarily bypassed catalog checking stuff so we can create mappings
  with TAttrLink slave TDatas.

  Revision 1.19  1998/04/10 18:29:11  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

  Revision 1.18  1998/03/12 20:44:57  wenger
  Partial fix for bug 320.

  Revision 1.17  1998/03/02 22:04:06  taodb
  Added control parameter to Run invocations

  Revision 1.16  1998/02/26 20:40:53  taodb
  Replaced ParseAPI calls with Command Container calls

  Revision 1.15  1998/02/20 06:16:46  beyer
  resurected histograms

  Revision 1.14  1998/02/02 18:25:59  wenger
  Strings file can now be loaded manually; name of strings file is now
  stored in session file; added 'serverExit' command and kill_devised
  script to cleanly kill devised; fixed bug 249; more info is now
  printed for unrecognized commands.

  Revision 1.13  1998/01/07 19:28:44  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.12  1997/12/19 21:44:51  wenger
  Removed (now obsolete) view override color reference so saving
  sessions works.

  Revision 1.11  1997/11/24 16:22:20  wenger
  Added GUI for saving GData; turning on GData to socket now forces
  redraw of view; GData to socket params now saved in session files;
  improvement to waitForQueries command.

  Revision 1.10.2.1  1998/01/07 15:59:31  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.10  1997/10/28 15:46:19  wenger
  Fixed bug 236.

  Revision 1.9  1997/10/16 16:12:00  donjerko
  Fixed the problem of not recognizing DTE sources correctly.

  Revision 1.8  1997/10/03 16:01:28  wenger
  Enabled session opening and saving from back end; incremented version; a
  few more minor fixes to session-related code.

  Revision 1.7  1997/10/03 14:36:57  wenger
  Various fixes to get session opening/saving to work with client/server
  version; reading old-style (Tcl) session files now works in back end;
  got back-end session file stuff working for multi.

  Revision 1.6  1997/10/02 18:46:31  wenger
  Opening and saving batch-style sessions in back end now fully working;
  added tk2ds.tcl script for conversion.

  Revision 1.5  1997/09/23 19:57:25  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.4  1997/09/19 21:29:54  wenger
  Did various cleanups that didn't affect functionality.

  Revision 1.3  1997/09/19 20:04:10  wenger
  Now saving complete session info; works for tables as well as unixfiles;
  derived data not yet tested.

  Revision 1.2  1997/09/18 15:15:17  wenger
  Now writes a useable session file (at least in some cases).

  Revision 1.1  1997/09/17 18:43:59  wenger
  Opening sessions works except for Tcl interpreter handling result; saving
  is most of the way there.

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>

#include <tcl.h>

#include "Session.h"
#include "DevError.h"
#include "Control.h"
#include "ParseAPI.h"
#include "DevFileHeader.h"
#include "Util.h"
#if !defined(NO_DTE)
  #include "../../DTE/DeviseSpecific/CatalogComm.h"
#endif
#include "ParseCat.h"
#include "StringStorage.h"
#include "CmdContainer.h"
#include "DeviseCommand.h"
#include "DeviseServer.h"
#include "CmdLog.h"
#include "View.h"
#include "DataCatalog.h"
#include "DataSeg.h"
#include "Color.h"
#include "ViewGeom.h"
#include "ViewGraph.h"
#include "Layout.h"
#include "ElapsedTime.h"


//#define DEBUG
#define SESSION_TIMER

// Note: we're defining this class so we have a ControlPanel * to pass
// to ParseAPI so it can return its results.
// What we really need here is an _object_ that inherits all of the methods
// except ReturnVal() from the ControlPanel::Instance() object and also has
// its own interpreter...
// Anyhow, the important thing is that all methods of the ControlPanel class
// that are called from ParseAPI() are implemented here (unless they're
// static).
// RKW Oct. 2, 1997.

class ControlPanelSimple : public ControlPanel {
public:
  // These member functions are unique to this class.
  ControlPanelSimple(DevStatus &status) {
    _interp = Tcl_CreateInterp();
    if (_interp == NULL) {
      reportErrNosys("Unable to create Tcl interpreter");
      status = StatusFailed;
    }
  }
  virtual ~ControlPanelSimple() {
    Tcl_DeleteInterp(_interp);
  }

  virtual int ReturnVal(u_short flag, char *result) {
    _valueReturned = true;
    Tcl_SetResult(_interp, result, TCL_VOLATILE);
    return 1;
  }
  virtual int ReturnVal(int argc, char **argv) {
    _valueReturned = true;
    Tcl_ResetResult(_interp);
    for(int i = 0; i < argc; i++) 
      Tcl_AppendElement(_interp, argv[i]);
    return 1;
  }

  // These member functions are called in ParseAPI(), and therefore need
  // to call the appropriate function in the "real" ControlPanel object.
  virtual Mode GetMode() { return ControlPanel::Instance()->GetMode(); }
  virtual void SetMode(Mode mode) { ControlPanel::Instance()->SetMode(mode); }

  virtual void SetBusy() { ControlPanel::Instance()->SetBusy(); }
  virtual void SetIdle() { ControlPanel::Instance()->SetIdle(); }

  virtual void DestroySessionData() {
      ControlPanel::Instance()->DestroySessionData(); }
  virtual void RestartSession() { ControlPanel::Instance()->RestartSession(); }

  virtual void SetBatchMode(Boolean mode) {
      ControlPanel::Instance()-> SetBatchMode(mode); }

  virtual void SetSyncNotify() { ControlPanel::Instance()->SetSyncNotify(); }

  virtual GroupDir *GetGroupDir() {
      return ControlPanel::Instance()-> GetGroupDir(); }

  virtual MapInterpClassInfo *GetInterpProto() {
      return ControlPanel::Instance()->GetInterpProto(); }

  virtual void OpenDataChannel(int port) {
      ControlPanel::Instance()->OpenDataChannel(port); }
  virtual int getFd() { return ControlPanel::Instance()->getFd(); }

  // The following member functions are needed just because they are pure
  // virtual in the base class.
  virtual void SelectView(View *view) {}
  virtual Boolean IsBusy() { return false; }
  virtual void SubclassInsertDisplay(DeviseDisplay *disp, Coord x, Coord y,
      Coord w, Coord h) {}
  virtual void SubclassDoInit() {}

  // This is the interpreter used for opening a session.
  Tcl_Interp *_interp;
};

static char *classNameList;
static unsigned int classNameListLen;

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

Boolean Session::_isJsSession = false;
Boolean Session::_openingSession = false;

/*------------------------------------------------------------------------------
 * function: Session::Open
 * Open specified session file.
 */
DevStatus
Session::Open(char *filename)
{
#if defined(DEBUG)
  printf("Session::Open(%s)\n", filename);
#endif

  DevStatus status = StatusOk;

  _openingSession = true;

  ControlPanelSimple control(status);

  if (status.IsComplete()) {
    Tcl_CreateCommand(control._interp, "DEVise", DEViseCmd, &control, 0);
    Tcl_CreateCommand(control._interp, "OpenDataSource", OpenDataSourceCmd,
	&control, 0);
    Tcl_CreateCommand(control._interp, "scanDerivedSources",
	scanDerivedSourcesCmd, &control, 0);
    Tcl_CreateCommand(control._interp, "SetDescription", SetDescriptionCmd,
	&control, 0);

    Tcl_SetVar(control._interp, "template", "0", TCL_GLOBAL_ONLY);

	// disable logging while evaluating the file
	CmdLogRecord* cmdLog = cmdContainerp->getCmdLog();
	int oldStatus = cmdLog->getLogStatus();
	cmdLog->setLogStatus(false);
    if (Tcl_EvalFile(control._interp, filename) != TCL_OK) {
      char errBuf[256];
      sprintf(errBuf, "Tcl error: %s", control._interp->result);
      reportErrNosys(errBuf);
      status = StatusFailed;
    }
	// resume original logging status
	cmdLog->setLogStatus(oldStatus);
  }

  _openingSession = false;

#if defined(DEBUG)
  printf("  finished Session::Open(%s)\n", filename);
  status.Print();
#endif

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::Close
 * Close the current session.
 */
DevStatus
Session::Close()
{
#if defined(DEBUG)
  printf("Session::Close()\n");
#endif

  DevStatus status = StatusOk;
  ViewGeom *vg = ViewGeom::GetViewGeom();
  if (!vg->IsGrouped()) {
    status += vg->Group();
  }
  ControlPanel::Instance()->DestroySessionData();
  _isJsSession = false;
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::Save
 * Save session to specified file.
 */
DevStatus
Session::Save(char *filename, Boolean asTemplate, Boolean asExport,
    Boolean withData, Boolean selectedView)
{
#if defined(DEBUG)
  printf("Session::Save(%s, %d, %d, %d, %d)\n", filename, asTemplate, asExport,
      withData, selectedView);
#endif

  DevStatus status = StatusOk;

  if (asTemplate) {
    reportErrNosys("Save as template not currently implemented");
    status = StatusFailed;
  }

  if (asExport) {
    reportErrNosys("Save as exported template not currently implemented");
    status = StatusFailed;
  }

  if (withData) {
    reportErrNosys("Save with data not currently implemented");
    status = StatusFailed;
  }

  ControlPanelSimple control(status);
  SaveData saveData;
  saveData.control = &control;
  saveData.fp = NULL;

  if (status.IsComplete()) {
    saveData.fp = fopen(filename, "w");
  }

  if (status.IsComplete()) {
    char *header = DevFileHeader::Get(FILE_TYPE_SESSION);
    fprintf(saveData.fp, "%s", header);

    const char *stringFile = StringStorage::GetFile();
    if (stringFile != NULL) {
      fprintf(saveData.fp, "\n# Load strings table\n");
      fprintf(saveData.fp, "DEVise loadStringSpace %s\n", stringFile);
    }

#if 0 // Temporarily disabling because of bug 441.
    fprintf(saveData.fp, "\n# Load color palette\n");
    PaletteID pid = PM_GetCurrentPalette();
    if (pid != nullPaletteID) {
      Palette *palette = PM_GetPalette(pid);
      if (palette == NULL) {
	reportErrNosys("Can't get palette");
        status += StatusFailed;
      } else {
        string colors = palette->ToString();
	fprintf(saveData.fp, "DEVise color CreateAndSetPalette \"%s\"\n",
	  colors.c_str());
      }
    }
#endif

    fprintf(saveData.fp, "\n# Create views\n");
    status += ForEachInstance("view", SaveView, &saveData);

    fprintf(saveData.fp, "\n# Create interpreted mapping classes\n");
    char nameBuf[1024];
    nameBuf[0] = '\0';
    classNameList = nameBuf;
    classNameListLen = 1024;
    status += ForEachInstance("mapping", SaveInterpMapping, &saveData);
    classNameList = NULL;
    classNameListLen = 0;

    fprintf(saveData.fp, "\n# Create mapping instances (GData)\n");
    status += ForEachInstance("mapping", SaveGData, &saveData);

    fprintf(saveData.fp, "\n# Create windows\n");
    SaveCategory(&saveData, "window");

    fprintf(saveData.fp, "\n# Set up window layouts\n");
    status += ForEachInstance("window", SaveWindowLayout, &saveData);

    fprintf(saveData.fp, "\n# Create links\n");
    status += SaveCategory(&saveData, "link");

    fprintf(saveData.fp, "\n# Create cursors\n");
    status += SaveCategory(&saveData, "cursor");

    fprintf(saveData.fp, "\n# Create axislabel\n");
    status += SaveCategory(&saveData, "axisLabel");

    fprintf(saveData.fp, "\n# Create actions\n");
    status += SaveCategory(&saveData, "action");

    fprintf(saveData.fp, "\n# Put labels into views\n");
    status += ForEachInstance("view", SaveViewAxisLabels, &saveData);

    fprintf(saveData.fp, "\n# Put action into view\n");
    status += ForEachInstance("view", SaveViewActions, &saveData);

    fprintf(saveData.fp, "\n# Put views in cursors\n");
    status += ForEachInstance("cursor", SaveCursor, &saveData);

    fprintf(saveData.fp, "\n# Put axis label into views\n");
    // Note: the Tcl code didn't do anything here.

    fprintf(saveData.fp, "\n# Insert mappings into views\n");
    status += ForEachInstance("view", SaveViewMappings, &saveData);

    fprintf(saveData.fp, "\n# Link views\n");
    status += ForEachInstance("link", SaveViewLinks, &saveData);

    fprintf(saveData.fp, "\n# Insert views into windows\n");
    status += ForEachInstance("window", SaveWindowViews, &saveData);

    fprintf(saveData.fp, "\n# Set window pile/stack state\n");
    status += ForEachInstance("window", SavePileStack, &saveData);

    fprintf(saveData.fp, "\n# Init history of view\n");
    status += ForEachInstance("view", SaveViewHistory, &saveData);

    fprintf(saveData.fp, "\n# Set camera location for each view\n");
    status += ForEachInstance("view", SaveCamera, &saveData);

    // This is done here because it must be done *after* mappings are
    // inserted into views.
    fprintf(saveData.fp, "\n# Set string tables for each view\n");
    status += ForEachInstance("view", SaveStringTables, &saveData);

    if (selectedView) {
      fprintf(saveData.fp, "\n# Select view\n");
      View *view = View::FindSelectedView();
      if (view != NULL) {
	fprintf(saveData.fp, "DEVise selectView {%s}\n", view->GetName());
      }
    }
  }

  if (saveData.fp != NULL) {
    if (fclose(saveData.fp) != 0) {
      reportErrSys("Error closing session file");
      status = StatusWarn;
    }
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::Update
 * Update specified session file (open it, do 'home' on specified views,
 * save it).
 */
DevStatus
Session::Update(char *filename)
{
#if defined(DEBUG)
  printf("Session::Update(%s)\n", filename);
#endif

  DevStatus status = StatusOk;

  status += Session::Open(filename);
  if (status.IsComplete()) {
    status += Session::UpdateFilters();
  }
  if (status.IsComplete()) {
    status += Session::Save(filename, false, false, false, false);
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::UpdateFilters
 * Update specified session file (open it, do 'home' on specified views,
 * save it).
 */
DevStatus
Session::UpdateFilters()
{
#if defined(DEBUG)
  printf("Session::UpdateFilters()\n");
#endif

  DevStatus status = StatusOk;

  Dispatcher::Current()->WaitForQueries();

  int index = View::InitViewIterator();
  while (View::MoreView(index)) {
    View *view = View::NextView(index);
    
    if (view->AutoUpdateFilter()) {
#if defined(DEBUG)
      printf("  Updating filter for view <%s>\n", view->GetName());
#endif
      ((ViewGraph *)view)->GoHome(false);
    }
  }
  View::DoneViewIterator(index);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::CreateTData
 * Create the given TData.  The purpose of this function is to create the
 * TData given only its name, so extra (redundant) information doesn't have
 * to be stored in session files.
 */
DevStatus
Session::CreateTData(char *name)
{
#if defined(DEBUG)
  printf("Session::CreateTData(%s)\n", name);
#endif

#if defined(SESSION_TIMER)
  ElapsedTime et;
  et.Start();
#endif

  CmdSource cmdSrc(CmdSource::INTERNAL, CLIENT_INVALID);
  CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::UNDEFINED);

  DevStatus status = StatusOk;

  // check for derived sources
  if( strncmp(name, "Hist: ", 6) == 0 ) {
    char* schemaName = "HISTOGRAM";
    char* schemaText = 
      "type HISTOGRAM ascii\n"
      "separator ' '\n"
      "sorted attr Bucket double\n"
      "attr Value int\n";
    ParseSchema(schemaName, schemaText, NULL);

    DataSeg::Set(name, "", 0, 0);

    char *argvIn[3];
    argvIn[0] = name;
    argvIn[1] = schemaName;
    argvIn[2] = "";
    ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
    classDir->CreateWithParams("tdata", schemaName, 3, argvIn);
    return status;
  }

  char *catEntry;
  int dum1, dum2;
  char namebuf[1024];
  if (sscanf(name, "%d.%d", &dum1, &dum2) == 2) {
    // Data source is (hopefully) an internal DTE table, probably generated
    // for a TAttrLink.
    catEntry = CopyString("");
  } else {
    if (name[0] != '.') {
      // DTE goes nuts if first char of TData name isn't '.'.
      sprintf(namebuf, ".%s", name);
      name = namebuf;
    }
    // Get the DTE catalog entry for this data source.
	// TEMP -- memory may be leaked in here
#if defined(DTE_WARN)
    fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
    catEntry = dteShowCatalogEntry(name);
#else
    catEntry = DataCatalog::Instance()->ShowEntry(name);
#endif
    if ((catEntry == NULL) || (strlen(catEntry) == 0)) {
      char errBuf[256];
      sprintf(errBuf, "No catalog entry for data source {%s}", name);
      reportErrNosys(errBuf);
      status = StatusFailed;
    }
  }
#if defined(DEBUG)
  printf("  catEntry = <%s>\n", catEntry != NULL ? catEntry : "NULL");
#endif

  // Get rid of braces surrounding the whole catalog entry and semicolon at
  // the end so it can get parsed.
  if (status.IsComplete()) {
    if (catEntry[0] == '{') catEntry[0] = ' ';
    int length = strlen(catEntry);
    if (catEntry[length - 1] == '}') {
      catEntry[length - 1] = '\0';
    } else if (catEntry[length - 1] == ' ' && catEntry[length - 2] == '}') {
      catEntry[length - 2] = '\0';
    }
    char *semicolon = strrchr(catEntry, ';');
    if (semicolon != NULL) *semicolon = ' ';
  }

  // Parse the catalog entry and assemble the proper arguments for creating
  // the TData object.
  char schema[MAXPATHLEN];
  char schemaFile[MAXPATHLEN];
  char sourceType[MAXPATHLEN];
  char param[MAXPATHLEN];
  Boolean isDteSource;
  if (status.IsComplete()) {
    Tcl_Interp* interp = Tcl_CreateInterp();
    if (interp == NULL) {
      reportErrNosys("Unable to create Tcl interpreter");
      status = StatusFailed;
    } else {

      // This is a kludgey way of trying to figure out whether we have a
      // DTE data source or a UNIXFILE -- it will fail if someone has a
      // table with an attribute called UNIXFILE.  However, because of changes
      // Donko has made to SQLViews, Tcl_SplitList() will barf on many
      // SQLView data sources, so we have to decide whether it belongs to
      // the DTE before with do Tcl_SplitList() on it.  RKW Mar. 12, 1998.
      if (strstr(catEntry, "UNIXFILE") != NULL) {
        isDteSource = false;
      } else {
        isDteSource = true;
      }

      if (!isDteSource) {
        int argcOut;
        char **argvOut;

        if (Tcl_SplitList(interp, catEntry, &argcOut, &argvOut) != TCL_OK) {
          reportErrNosys(interp->result);
          status = StatusFailed;
        } else {
          strcpy(schema, argvOut[3]);
          strcpy(schemaFile, argvOut[4]);
          strcpy(sourceType, argvOut[1]);
          sprintf(param, "%s/%s", argvOut[8], argvOut[2]);
          free((char *) argvOut);
        }
      }

      Tcl_DeleteInterp(interp);
    }
  }

  // Parse the schema file for the given data source, and re-set the
  // schema type if the type in the file is different from the type
  // given in the catalog.
  if (status.IsComplete()) {
    char *result ;
    if (isDteSource) {
	  // TEMP -- memory may be leaked in here
#if defined(DTE_WARN)
      fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
      result = dteImportFileType(name);
#else
      result = NULL;
#endif
      if (result == NULL) {
	char buf[2048];
	sprintf(buf, "Error parsing schema %s", name);
	reportErrNosys(buf);
	status = StatusFailed;
      }
    } else {
      result = ParseCat(schemaFile);
      if (result == NULL) {
	status = StatusFailed;
      } else {
        if (strcmp(schema, result)) {
          printf("File %s appears to contain schema %s, not %s\n", schemaFile,
	      result, schema);
          strcpy(schema, result);
        }
      }
    }
  }

  // Execute dataSegment command.
  if (status.IsComplete()) {
	char *filename;
    if (isDteSource) {
      filename = "";
    } else {
      filename = param;
    }
    DataSeg::Set(name, filename, 0, 0);
  }

  // Create the TData object.
  if (status.IsComplete()) {
	char *arg2;
    char *argvIn[3];
    if (isDteSource) {
      arg2 = name;
      argvIn[0] = name;
      argvIn[1] = "";
      argvIn[2] = "";
    } else {
      arg2 = schema;
      argvIn[0] = name;
      argvIn[1] = sourceType;
      argvIn[2] = param;
    }
    ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
    classDir->CreateWithParams("tdata", arg2, 3, argvIn);
  }

  if (catEntry != NULL) free(catEntry);

#if defined(SESSION_TIMER)
  char timeBuf[256];
  sprintf(timeBuf, "Creating TData <%s>", name);
  et.ReportTime(timeBuf);
#endif

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::DEViseCmd
 * DEVise command procedure for session file interpreter.
 */
int
Session::DEViseCmd(ClientData clientData, Tcl_Interp *interp,
    int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::DEViseCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  CmdSource cmdSrc(CmdSource::SESSION_PLAY, CLIENT_INVALID);
  CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::UNDEFINED);

  int status = TCL_OK;

  // Don't do anything for "DEVise create tdata...", "DEVise importFileType",
  // "DEVise dteImportFileType", and "DEVise dataSegment" commands.
  if (!strcmp(argv[1], "create") && !strcmp(argv[2], "tdata")) {
    // No op.
    Tcl_SetResult(interp, "", TCL_VOLATILE);
  } else if (!strcmp(argv[1], "importFileType")) {
    // No op.
    Tcl_SetResult(interp, "", TCL_VOLATILE);
  } else if (!strcmp(argv[1], "dteImportFileType")) {
    // No op.
    Tcl_SetResult(interp, "", TCL_VOLATILE);
  } else if (!strcmp(argv[1], "dataSegment")) {
    // No op.
    Tcl_SetResult(interp, "", TCL_VOLATILE);
  } else {
    // don't pass DEVise command verb (argv[0])
    if (cmdContainerp->Run(argc-1, &argv[1],  (ControlPanel *) clientData, cmdDes) < 0) {
      status = TCL_ERROR;
      fprintf(stderr, "Error in command: ");
      PrintArgs(stderr, argc, argv);
    }
  }

#if defined(DEBUG)
  printf("  finished command %s; status = %s\n", argv[1], status == TCL_OK ?
      "TCL_OK" : "TCL_ERROR");
  printf("    result = {%s}\n", interp->result);
#endif

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::OpenDataSourceCmd
 * OpenDataSource command procedure for session file interpreter.
 */
int
Session::OpenDataSourceCmd(ClientData clientData, Tcl_Interp *interp,
    int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::OpenDataSourceCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  int status = TCL_OK;

  // Turn old-style (pre-DTE) name into new-style (DTE) name.
  char *result;
  char newName[1024];
  if (argv[1][0] != '.') {
    sprintf(newName, ".%s", argv[1]);
    char *current = &newName[1];
    while (*current != '\0') {
      if (*current == '.' || *current == ' ' || *current == ',') {
	*current = '_';
      }
      current++;
    }
    result = newName;
  } else {
    result = argv[1];
  }

  Tcl_SetResult(interp, result, TCL_VOLATILE);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::scanDerivedSourcesCmd
 * scanDerivedSources command procedure for session file interpreter.
 */
int
Session::scanDerivedSourcesCmd(ClientData clientData, Tcl_Interp *interp,
    int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::scanDerivedSourcesCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  int status = TCL_OK;

  // This command is a no-op -- we just need to have the command exist
  // because it's in session files -- derived sources are goofed up
  // right now anyhow.

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SetDescriptionCmd
 * SetDescription command procedure for session file interpreter.
 */
int
Session::SetDescriptionCmd(ClientData clientData, Tcl_Interp *interp,
    int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::SetDescriptionCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  int status = TCL_OK;

  // This command is a no-op -- we just need to have the command exist
  // because it's in session files.

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveView
 * Save information about the given view.
 */
DevStatus
Session::SaveView(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveView({%s} {%s} {%s})\n", category, devClass, instance);
#endif

  DevStatus status = StatusOk;

  FILE *fp = saveData->fp;

  status += SaveParams(saveData, "getCreateParam", "create", "view",
      devClass, instance);

  status += SaveParams(saveData, "getLabel", "setLabel", instance);

  status += SaveParams(saveData, "getViewStatistics", "setViewStatistics",
      instance);

  status += SaveParams(saveData, "getViewDimensions", "setViewDimensions",
      instance);

  status += SaveParams(saveData, "getViewSolid3D", "setViewSolid3D",
      instance);

  status += SaveParams(saveData, "getViewXYZoom", "setViewXYZoom", instance);

  status += SaveParams(saveData, "getViewDisplayDataValues",
      "setViewDisplayDataValues", instance);

  status += SaveParams(saveData, "getFont", "setFont", instance, "title");
  status += SaveParams(saveData, "getFont", "setFont", instance, "x axis");
  status += SaveParams(saveData, "getFont", "setFont", instance, "y axis");

  status += SaveParams(saveData, "viewGetHome", "viewSetHome", instance);

  status += SaveParams(saveData, "viewGetHorPan", "viewSetHorPan", instance);
  status += SaveParams(saveData, "viewGetVerPan", "viewSetVerPan", instance);

  status += SaveParams(saveData, "getViewGDS", "setViewGDS", instance);
  status += SaveParams(saveData, "viewGetJSSendP", "viewSetJSSendP",
      instance, NULL, NULL, false);

  status += SaveParams(saveData, "getHistogram", "setHistogram", instance);

  status += SaveParams(saveData, "getCountMapping", "setCountMapping",
      instance);

  status += SaveParams(saveData, "viewGetIsHighlight", "viewSetIsHighlight",
      instance);

  status += SaveParams(saveData, "getXAxisDateFormat", "setXAxisDateFormat",
      instance, NULL, NULL, true);
  status += SaveParams(saveData, "getYAxisDateFormat", "setYAxisDateFormat",
      instance, NULL, NULL, true);

  status += SaveParams(saveData, "getViewAutoFilter", "setViewAutoFilter",
      instance, NULL, NULL, true);

  status += SaveParams(saveData, "getDupElim", "setDupElim",
      instance, NULL, NULL, true);

  status += SaveParams(saveData, "getNiceAxes", "setNiceAxes",
      instance, NULL, NULL, false);

  status += SaveParams(saveData, "viewGetDisabledActions",
      "viewSetDisabledActions", instance, NULL, NULL, false);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveInterpMapping
 * Save information about the given mapping class (if it is interpreted).
 */
DevStatus
Session::SaveInterpMapping(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveInterpMapping({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  // Save only _unique_ mapping classes (we may have multiple mapping
  // instances of the same class, so this function may get called multiple
  // times with identical class names).
  char buf[256];
  sprintf(buf, "{%s}", devClass);
  if (strstr(classNameList, buf) == NULL) {
    if (strlen(classNameList) + strlen(buf) < classNameListLen) {
      strcat(classNameList, buf);
    } else {
      reportErrNosys("Ran out of room in class name list; saved session file"
	" may have errors");
    }

    char *result;
    int argcOut;
    char **argvOut;
    status += CallParseAPI(saveData->control, result, false, argcOut, argvOut,
        "isInterpretedGData", instance);
    if (status.IsComplete()) {
      int isInterpreted = atoi(result);
      if (isInterpreted) {
        fprintf(saveData->fp, "DEVise createMappingClass {%s}\n", devClass);
      }
    }
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveGData
 * Save information about the given GData.
 */
DevStatus
Session::SaveGData(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveGData({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  status += SaveParams(saveData, "getCreateParam", "create", "mapping",
      devClass, instance);
  status += SaveParams(saveData, "getPixelWidth", "setPixelWidth", instance);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveWindowLayout
 * Save layout information about the given window.
 */
DevStatus
Session::SaveWindowLayout(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveWindowLayout({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = SaveParams(saveData, "getWindowLayout",
      "setWindowLayout", instance);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewAxisLabels
 * Save axis label information for the given view.
 */
DevStatus
Session::SaveViewAxisLabels(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewAxisLabels({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  status += SaveParams(saveData, "getAxisDisplay", "setAxisDisplay",
      instance, "X");
  status += SaveParams(saveData, "getAxisDisplay", "setAxisDisplay",
      instance, "Y");

  status += SaveParams(saveData, "getAxisTicks", "setAxisTicks",
      instance, "X");
  status += SaveParams(saveData, "getAxisTicks", "setAxisTicks",
      instance, "Y");

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewActions
 * Save actions for the given view.
 */
DevStatus
Session::SaveViewActions(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewActions({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = SaveParams(saveData, "getAction", "setAction", instance);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewLinks
 * Save connections between views and links.
 */
DevStatus
Session::SaveViewLinks(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewLinks({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "getLinkViews", instance);
  if (status.IsComplete()) {
    int index;
    for (index = 0; index < argcOut; index++) {
      fprintf(saveData->fp, "DEVise insertLink {%s} {%s}\n", instance,
	  argvOut[index]);
    }
    free((char *) argvOut);
  }

  status += SaveParams(saveData, "getLinkMaster", "setLinkMaster", instance,
      NULL, NULL, true);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveCursor
 * Save the given cursor.
 */
DevStatus
Session::SaveCursor(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveCursor({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "getCursorViews", instance);
  if (status.IsComplete()) {
    char *source = argvOut[0];
    char *dest = argvOut[1];
    if (strlen(source) > 0) {
      fprintf(saveData->fp, "DEVise setCursorSrc {%s} {%s}\n", instance,
          source);
    }
    if (strlen(dest) > 0) {
      fprintf(saveData->fp, "DEVise setCursorDst {%s} {%s}\n", instance,
          dest);
    }
    free((char *) argvOut);
  }

  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "color", "GetCursorColor", instance);
  if (status.IsComplete()) {
    fprintf(saveData->fp, "DEVise color SetCursorColor {%s} %s\n", instance,
	argvOut[0]);

    free((char *) argvOut);
  }

  status += SaveParams(saveData, "getCursorFixedSize", "setCursorFixedSize",
      instance);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewMappings
 * Save mappings for the given view.
 */
DevStatus
Session::SaveViewMappings(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewMappings({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "getViewMappings", instance);
  if (status.IsComplete()) {
    int index;
    for (index = 0; index < argcOut; index++) {
      fprintf(saveData->fp, "DEVise insertMapping {%s} {%s}\n", instance,
          argvOut[index]);
      status += SaveParams(saveData, "getMappingLegend", "setMappingLegend",
          instance, argvOut[index]);
    }
    free((char *) argvOut);
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveWindowViews
 * Save views for the given window.
 */
DevStatus
Session::SaveWindowViews(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveWindowViews({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "getWinViews", instance);
  if (status.IsComplete()) {
    //
    // Insert views into windows.
    //
    int index;
    for (index = 0; index < argcOut; index++) {
      fprintf(saveData->fp, "DEVise insertWindow {%s} {%s}\n",
	  argvOut[index], instance);
    }

    //
    // Save view geometries if window layout is custom.
    //
    ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
    Layout *window = (Layout *)classDir->FindInstance(instance);
    LayoutMode mode;
    window->GetLayoutMode(mode);
    if (mode == CUSTOM) {
      for (index = 0; index < argcOut; index++) {
        status += SaveParams(saveData, "getViewGeometry", "setViewGeometry",
            argvOut[index], NULL, NULL, false);
      }
    }
    
    free((char *) argvOut);
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SavePileStack
 * Save views for the given window.
 */
DevStatus
Session::SavePileStack(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SavePileStack({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = SaveParams(saveData, "getPileStackState",
      "setPileStackState", instance);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveViewHistory
 * Save the history for the given view.
 */
DevStatus
Session::SaveViewHistory(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveViewHistory({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  fprintf(saveData->fp, "DEVise clearViewHistory {%s}\n", instance);

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "getVisualFilters", instance);
  if (status.IsComplete()) {
    int index;
    for (index = 0; index < argcOut; index++) {
      fprintf(saveData->fp, "DEVise insertViewHistory {%s} %s\n", instance,
          argvOut[index]);
    }
    free((char *) argvOut);
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveCamera
 * Save the camera location for the given view.
 */
DevStatus
Session::SaveCamera(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveCamera({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, true, argcOut, argvOut,
      "get3DLocation", instance);
  if (status.IsComplete()) {
    fprintf(saveData->fp, "DEVise set3DLocation {%s} ", instance);
    PrintArgs(saveData->fp, 9, argvOut);
    free((char *) argvOut);
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveStringTables
 * Save the camera location for the given view.
 */
DevStatus
Session::SaveStringTables(char *category, char *devClass, char *instance,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::SaveStringTables({%s} {%s} {%s})\n", category, devClass,
      instance);
#endif

  DevStatus status = StatusOk;

  status += SaveParams(saveData, "viewGetStringTable", "viewSetStringTable",
      instance, "x");
  status += SaveParams(saveData, "viewGetStringTable", "viewSetStringTable",
      instance, "y");
  status += SaveParams(saveData, "viewGetStringTable", "viewSetStringTable",
      instance, "z");
  status += SaveParams(saveData, "viewGetStringTable", "viewSetStringTable",
      instance, "gen");

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveParams
 * Get parameters from the given 'get' function, write out the given 'set'
 * function with those parameters.
 */
DevStatus
Session::SaveParams(SaveData *saveData, char *getCommand, char *setCommand,
    char *arg0, char *arg1, char *arg2, Boolean addBraces)
{
#if defined(DEBUG)
  printf("Session::SaveParams(%s)\n", arg0);
#endif

  DevStatus status = StatusOk;

  char *leftBrace;
  char *rightBrace;
  if (addBraces) {
    leftBrace = "{";
    rightBrace = "}";
  } else {
    leftBrace = "";
    rightBrace = "";
  }

  char *result;
  int argcOut;
  char **argvOut;
  status += CallParseAPI(saveData->control, result, false, argcOut, argvOut,
      getCommand, arg0, arg1, arg2);
  if (status.IsComplete()) {
    if (strlen(result) > 0) {
      fprintf(saveData->fp, "DEVise %s {%s} ", setCommand, arg0);
      if (arg1 != NULL) {
        // Note: arg2 is not passed to 'set' command.
	fprintf(saveData->fp, "{%s} ", arg1);
      }
      fprintf(saveData->fp, "%s%s%s\n", leftBrace, result, rightBrace);
    }
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::CallParseAPI
 * Get parameters from the given 'get' function, write out the given 'set'
 * function with those parameters.
 */
DevStatus
Session::CallParseAPI(ControlPanelSimple *control, char *&result,
    Boolean splitResult, int &argcOut, char **&argvOut, char *arg0,
    char *arg1 = NULL, char *arg2 = NULL, char *arg3 = NULL)
{
#if defined(DEBUG)
  printf("Session::CallParseAPI(%s)\n", arg0);
#endif

  CmdSource cmdSrc(CmdSource::SESSION_SAVE,CLIENT_INVALID);
  CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::UNDEFINED);

  DevStatus status = StatusOk;

  int argcIn;
  if (arg1 != NULL) {
    if (arg2 != NULL) {
      if (arg3 != NULL) {
        argcIn = 4;
      } else {
        argcIn = 3;
      }
    } else {
      argcIn = 2;
    }
  } else {
    argcIn = 1;
  }
  char *argvIn[4];
  argvIn[0] = arg0;
  argvIn[1] = arg1;
  argvIn[2] = arg2;
  argvIn[3] = arg3;
  if (cmdContainerp->Run(argcIn, argvIn, control, cmdDes) <= 0) {
    reportErrNosys(control->_interp->result);
    status = StatusFailed;
  } else {
    result = control->_interp->result;
    if (splitResult) {
      if (Tcl_SplitList(control->_interp, control->_interp->result, &argcOut,
	  &argvOut) != TCL_OK) {
        reportErrNosys(control->_interp->result);
        status = StatusFailed;
      }
    } else {
      argcOut = 0;
      argvOut = NULL;
    }
  }

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveCategory
 * Save all instances of the given category.
 */
DevStatus
Session::SaveCategory(SaveData *saveData, char *category)
{
#if defined(DEBUG)
  printf("Session::SaveCategory()\n");
#endif

  DevStatus status = StatusOk;
  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  int classArgc;
  char **tmpArgv;
  classDir->ClassNames(category, classArgc, tmpArgv);
  char **classArgv;
  CopyArgs(classArgc, tmpArgv, classArgv);

  int index;
  for (index = 0; index < classArgc; index++) {
    SaveClass(saveData, category, classArgv[index]);
  }

  FreeArgs(classArgc, classArgv);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveClass
 * Save all instances of the given class.
 */
DevStatus
Session::SaveClass(SaveData *saveData, char *category, char *devClass)
{
#if defined(DEBUG)
  printf("Session::SaveClass()\n");
#endif

  DevStatus status = StatusOk;
  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  int instArgc;
  char **tmpArgv;
  classDir->InstanceNames(category, devClass, instArgc, tmpArgv);
  char **instArgv;
  CopyArgs(instArgc, tmpArgv, instArgv);

  int index;
  for (index = 0; index < instArgc; index++) {
    SaveInstance(saveData, category, devClass, instArgv[index]);
  }

  FreeArgs(instArgc, instArgv);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SaveInstance
 * Save the given instance.
 */
DevStatus
Session::SaveInstance(SaveData *saveData, char *category, char *devClass,
    char *instance)
{
#if defined(DEBUG)
  printf("Session::SaveInstance(%s, %s, %s)\n", category, devClass, instance);
#endif

  DevStatus status = SaveParams(saveData, "getCreateParam", "create", category,
      devClass, instance);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::ForEachInstance
 * Call the given function for each instance in the given category.
 */
DevStatus
Session::ForEachInstance(char *category, InstanceFuncP function,
    SaveData *saveData)
{
#if defined(DEBUG)
  printf("Session::ForEachInstance(%s)\n", category);
#endif

  DevStatus status = StatusOk;

  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();

  char **tmpArgv;

  // Get class names
  int classArgc;
  classDir->ClassNames(category, classArgc, tmpArgv);
  char **classArgv;
  CopyArgs(classArgc, tmpArgv, classArgv);

  // For each class, get each instance name.
  int classIndex;
  for (classIndex = 0; classIndex < classArgc; classIndex++) {
    int instArgc;
    classDir->InstanceNames(category, classArgv[classIndex], instArgc, tmpArgv);
    char **instArgv;
    CopyArgs(instArgc, tmpArgv, instArgv);

    int instIndex;
    for (instIndex = 0; instIndex < instArgc; instIndex++) {
#if defined(DEBUG)
      printf("  Instance: {%s} {%s} {%s}\n", category, classArgv[classIndex],
	  instArgv[instIndex]);
#endif
      status += (*function)(category, classArgv[classIndex],
	  instArgv[instIndex], saveData);
    }

    FreeArgs(instArgc, instArgv);
  }

  FreeArgs(classArgc, classArgv);

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*============================================================================*/
