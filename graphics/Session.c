/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2000
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
  Revision 1.87  2000/08/07 16:23:14  wenger
  Added comment about why a command is not written to session files if
  the string length of the corresponding info is zero.

  Revision 1.86  2000/06/20 22:16:54  wenger
  Added floating-point format for axes and mouse location display.

  Revision 1.85  2000/06/20 16:57:18  wenger
  Added commands and GUI to enable/disable the display of mouse location
  in various views, and globally.

  Revision 1.84  2000/04/19 16:16:28  wenger
  Found and fixed bug 579 (crash if opening session fails); found bug
  580; better handling of session opening errors on the Tcl side;
  improved WindowVisible and WindowExists procedures.

  Revision 1.83  2000/04/07 17:36:00  wenger
  String file path in session file is specified with $DEVISE_SESSION.

  Revision 1.82  2000/03/14 17:05:09  wenger
  Fixed bug 569 (group/ungroup causes crash); added more memory checking,
  including new FreeString() function.

  Revision 1.81  2000/02/23 21:30:57  wenger
  Re-implemented session description capability.

  Revision 1.80  2000/02/17 17:50:41  wenger
  Fixed bug that caused DEVise to crash when saving a session if session
  file could not be opened.

  Revision 1.79  2000/02/16 18:51:21  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.78  2000/02/08 22:11:47  wenger
  Added JAVAC_GetViewHelp and JAVAC_ShowViewHelp commands, added color
  edge grid, and type to JAVAC_DrawCursor command, JavaScreen protocol
  version now 4.0; added GUI to edit view help, and commands to save it
  to session files.

  Revision 1.77  2000/01/14 18:23:07  wenger
  Added resetAllFilters and JAVAC_ResetFilters commands to reset all visual
  filters back to the values defined in the session file, without actually
  re-opening the session.

  Revision 1.76  2000/01/13 23:06:51  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.75  1999/12/06 20:03:20  wenger
  Windows are forced to be on-screen when opening or saving a session.

  Revision 1.74  1999/12/06 18:40:48  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

  Revision 1.73  1999/12/02 15:06:49  wenger
  Fixed bug 538 (slow session saving).

  Revision 1.72  1999/11/24 15:43:55  wenger
  Removed (unnecessary) CommandObj class; commands are now logged for the
  monolithic form, not just the client/server form; other command-related
  cleanups; added GUI for playing back command logs.

  Revision 1.71  1999/11/22 18:11:58  wenger
  Fixed 'command buffer conflict' errors, other command-related cleanup.

  Revision 1.70  1999/11/19 17:17:26  wenger
  Added View::SetVisualFilterCommand() method to clean up command-related
  code for filter setting.

  Revision 1.69  1999/11/16 17:01:44  wenger
  Removed all DTE-related conditional compiles; changed version number to
  1.7.0 because of removing DTE; removed DTE-related schema editing and
  data source creation GUI.

  Revision 1.68  1999/11/01 17:55:13  wenger
  Fixed compile warning.

  Revision 1.67  1999/10/26 16:29:33  wenger
  Fixed bug 519 (problems with opening various sequences of soil science
  sessions, caused by stupid composite parsers not getting reset when a
  session is closed).

  Revision 1.66  1999/10/18 15:36:30  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.65  1999/10/04 22:36:07  wenger
  Fixed bug 508 (windows move slightly when repeatedly opening and saving
  a session) -- replaced kludgey (incorrect) way of dealing with window
  manager borders with correct way; user is warned if any windows extend
  off-screen when opening or saving a session.

  Revision 1.64  1999/10/04 19:36:56  wenger
  Mouse location is displayed in "regular" DEVise.

  Revision 1.63  1999/09/24 22:02:19  wenger
  C++ code no longer allows a session to be opened while one is already
  open.

  Revision 1.62  1999/09/23 15:46:23  wenger
  Added per-session data source capability:  data sources defined in a
  session file are added to a separate catalog which is delete when the
  session is closed; the "regular" and the per-session catalog are treated
  as a single catalog while the session is open.

  Revision 1.61  1999/09/20 21:33:38  wenger
  Trailing semicolons are now removed from session file lines.

  Revision 1.60  1999/09/08 20:56:21  wenger
  Removed all Tcl dependencies from the devised (main changes are in the
  Session class); changed version to 1.6.5.

  Revision 1.59  1999/09/07 19:00:31  wenger
  dteInsertCatalogEntry command changed to tolerate an attempt to insert
  duplicate entries without causing a problem (to allow us to get rid of
  Tcl in session files); changed Condor session scripts to take out Tcl
  control statements in data source definitions; added viewGetImplicitHome
  and related code in Session class so this gets saved in session files
  (still no GUI for setting this, though); removed SEQ-related code.

  Revision 1.58  1999/09/01 19:27:01  wenger
  Debug logging improved -- directory of log file can now be specified
  with the DEVISE_LOG_DIR environment variable (changed most startup scripts
  to put it in the DEVise tmp directory); added logging of a bunch of elapsed
  times to help figure out JavaScreen performance bottlenecks.

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
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>

#include <string>

#include "Session.h"
#include "DevError.h"
#include "Control.h"
#include "ParseAPI.h"
#include "DevFileHeader.h"
#include "Util.h"
#include "ParseCat.h"
#include "StringStorage.h"
#include "CmdContainer.h"
#include "DeviseCommand.h"
#include "DeviseServer.h"
#include "View.h"
#include "DataCatalog.h"
#include "DataSeg.h"
#include "Color.h"
#include "ViewGeom.h"
#include "ViewGraph.h"
#include "Layout.h"
#include "ElapsedTime.h"
#include "WinClassInfo.h"
#include "ArgList.h"
#include "CompositeParser.h"
#include "ControlPanelSimple.h"

//#define DEBUG
#define SESSION_TIMER

static char *classNameList;
static unsigned int classNameListLen;

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

Boolean Session::_isJsSession = false;
Boolean Session::_openingSession = false;

DataCatalog *Session::_dataCat = NULL;
char *Session::_catFile = NULL;
char *Session::_sessionFile = NULL;
char *Session::_description = NULL;

/*------------------------------------------------------------------------------
 * function: Session::Open
 * Open specified session file.
 */
DevStatus
Session::Open(const char *filename)
{
#if defined(DEBUG)
  printf("Session::Open(%s)\n", filename);
#endif

  DevStatus status = StatusOk;

  //
  // Make sure we don't already have a session open.
  //
  ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
  if (classDir->InstanceCount() != 0) {
	reportErrNosys("Can't open a session with a session already open");
    status += StatusFailed;
  }

  if (status.IsComplete()) {
    if (_sessionFile) {
      FreeString(_sessionFile);
    }
    _sessionFile = CopyString(filename);

    if (_description) {
      FreeString(_description);
    }
    _description = NULL;

    _openingSession = true;

    ControlPanelSimple control;

    status += ReadSession(&control, filename, RunCommand);

    _openingSession = false;
  }

  status += CheckWindowLocations();


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

  if (_sessionFile) {
    FreeString(_sessionFile);
  }
  _sessionFile = NULL;

  if (_description) {
    FreeString(_description);
  }
  _description = NULL;

  ViewGeom *vg = ViewGeom::GetViewGeom();
  if (!vg->IsGrouped()) {
    status += vg->Group();
  }
  ControlPanel::Instance()->DestroySessionData();
  DeleteDataSources();
  CompositeParser::ResetAll();
  _isJsSession = false;
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::Save
 * Save session to specified file.
 */
DevStatus
Session::Save(const char *filename, Boolean asTemplate, Boolean asExport,
    Boolean withData, Boolean selectedView)
{
#if defined(DEBUG)
  printf("Session::Save(%s, %d, %d, %d, %d)\n", filename, asTemplate, asExport,
      withData, selectedView);
#endif

  DevStatus status = StatusOk;

  status += CheckWindowLocations();

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

  ControlPanelSimple control;
  SaveData saveData;
  saveData.control = &control;
  saveData.fp = NULL;

  if (status.IsComplete()) {
    saveData.fp = fopen(filename, "w");
    if (!saveData.fp) {
	  char errBuf[MAXPATHLEN + 100];
	  sprintf(errBuf, "Can't open session file (%s) for save", filename);
      reportErrSys(errBuf);
      status += StatusFailed;
    }
  }

  if (status.IsComplete()) {
    char *header = DevFileHeader::Get(FILE_TYPE_SESSION);
    fprintf(saveData.fp, "%s", header);

    fprintf(saveData.fp, "\n# Session description\n");
    fprintf(saveData.fp, "DEVise setSessionDesc {%s}\n", GetDescription());

    const char *stringFile = StringStorage::GetFile();
    if (stringFile != NULL) {
      fprintf(saveData.fp, "\n# Load strings table\n");
      char *stringWithEnv = AddEnvToPath("DEVISE_SESSION", stringFile);
      fprintf(saveData.fp, "DEVise loadStringSpace %s\n", stringWithEnv);
      FreeString(stringWithEnv);
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

    status += SaveDataSources(saveData.fp);

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

  if (status.IsComplete()) {
    if (_sessionFile) {
      FreeString(_sessionFile);
    }
    _sessionFile = CopyString(filename);
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
Session::Update(const char *filename)
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
 * function: Session::ResetFilters
 * Reset all visual filters to the values specified in the session file.
 */
DevStatus
Session::ResetFilters()
{
#if defined(DEBUG)
  printf("Session::ResetFilters()\n");
#endif

  DevStatus status = StatusOk;

  if (_sessionFile) {
    ControlPanelSimple control;
    status += ReadSession(&control, _sessionFile, FilterCommand);
  } else {
    reportErrNosys("No session file available");
    status = StatusCancel;
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::UpdateFilters
 * Do 'home' on specified views.
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
Session::CreateTData(const char *name)
{
#if defined(DEBUG)
  printf("Session::CreateTData(%s)\n", name);
#endif

#if defined(SESSION_TIMER)
  ElapsedTime et;
  et.Start();
#endif

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

    const char *argvIn[3];
    argvIn[0] = name;
    argvIn[1] = schemaName;
    argvIn[2] = "";
    ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
    classDir->CreateWithParams("tdata", schemaName, 3, (char **)argvIn);
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
    catEntry = ShowDataSource(name);
    if ((catEntry == NULL) || (strlen(catEntry) == 0)) {
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

    // This is a kludgey way of trying to figure out whether we have a
    // DTE data source or a UNIXFILE -- it will fail if someone has a
    // table with an attribute called UNIXFILE.  However, because of changes
    // Donko has made to SQLViews, Tcl_SplitList() will barf on many
    // SQLView data sources, so we have to decide whether it belongs to
    // the DTE before with do Tcl_SplitList() on it.  RKW Mar. 12, 1998.
	//
	// Tcl_SplitList() has been replaced by ParseString().  RKW 1999-09-08.
    if (strstr(catEntry, "UNIXFILE") != NULL) {
      isDteSource = false;
    } else {
      isDteSource = true;
    }

    if (!isDteSource) {
	  ArgList args;

	  DevStatus tmpStatus = args.ParseString(catEntry);
	  status += tmpStatus;
	  if (tmpStatus.IsComplete()) {
        strcpy(schema, args.GetArgs()[3]);
        strcpy(schemaFile, args.GetArgs()[4]);
        strcpy(sourceType, args.GetArgs()[1]);
        sprintf(param, "%s/%s", args.GetArgs()[8], args.GetArgs()[2]);
      }
    }
  }

  // Parse the schema file for the given data source, and re-set the
  // schema type if the type in the file is different from the type
  // given in the catalog.
  if (status.IsComplete()) {
    char *result ;
    if (isDteSource) {
	  // TEMP -- memory may be leaked in here
      result = NULL;
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
	const char *arg2;
    const char *argvIn[3];
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
    classDir->CreateWithParams("tdata", (char *)arg2, 3, (char **)argvIn);
  }

  if (catEntry != NULL) FreeString(catEntry);

#if defined(SESSION_TIMER)
  char timeBuf[256];
  sprintf(timeBuf, "Creating TData <%s>", name);
  et.ReportTime(timeBuf);
#endif

  if (status.IsError()) reportErrNosys("Error or warning");
  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::AddDataSource
 * Add a per-session data sources.
 */
DevStatus
Session::AddDataSource(const char *catName, const char *entry)
{
#if defined(DEBUG)
  printf("Session::AddDataSource(%s, %s)\n", catName, entry);
#endif

  DevStatus status = StatusOk;

  if (strcmp(catName, ".")) {
	reportErrNosys("Catalog name for per-session data sources must be '.'\n");
    status += StatusFailed;
  } else {
    if (GetDataCatalog()->AddEntry(catName, entry) != 0) {
      status += StatusFailed;
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::ShowDataSource
 * Show the data source catalog entry (if any) for the given source.
 */
char *
Session::ShowDataSource(const char *sourceName)
{
#if defined(DEBUG)
  printf("Session::ShowDataSource(%s)\n", sourceName);
#endif

  char *catEntry;

  catEntry = DataCatalog::Instance()->ShowEntry(sourceName);
  if (catEntry == NULL || strlen(catEntry) == 0) {
    catEntry = GetDataCatalog()->ShowEntry(sourceName);
  }

#if defined(DEBUG)
  printf("  catEntry = <%s>\n", catEntry ? catEntry : "NULL");
#endif

  return catEntry;
}

/*------------------------------------------------------------------------------
 * function: Session::ListDataCatalog
 * Lists the data catalog, including per-session data sources.
 */
char *
Session::ListDataCatalog(const char *catName)
{
#if defined(DEBUG)
  printf("Session::ListDataCatalog(%s)\n", catName);
#endif

  char *catListMain;
  catListMain = DataCatalog::Instance()->ListCatalog(catName);

#if defined(DEBUG)
  printf("  catListMain = <%s>\n", catListMain);
#endif

  char *catListSess = NULL;
  if (!strcmp(catName, ".")) {
    catListSess = GetDataCatalog()->ListCatalog(".");
#if defined(DEBUG)
  printf("  catListSess = <%s>\n", catListSess);
#endif
  }

  char *catListTotal;
  if (catListSess && strlen(catListSess) > 0) {
	// +2 is for space and terminator.
    catListTotal = new char[strlen(catListMain) + strlen(catListSess) + 2];
	sprintf(catListTotal, "%s %s", catListMain, catListSess);
	FreeString(catListMain);
	FreeString(catListSess);
  } else {
    catListTotal = catListMain;
  }

#if defined(DEBUG)
  printf("  catListTotal = <%s>\n", catListTotal);
#endif

  return catListTotal;
}

/*------------------------------------------------------------------------------
 * function: Session::SetDescription
 * Set session description.
 */
void
Session::SetDescription(const char *description)
{
#if defined(DEBUG)
  printf("Session::SetDescription(%s)\n", description);
#endif

  if (_description) FreeString(_description);
  _description = CopyString(description);
}

/*------------------------------------------------------------------------------
 * function: Session::GetDescription
 * Get session description.
 */
const char *
Session::GetDescription()
{
#if defined(DEBUG)
  printf("Session::GetDescription()\n");
#endif

  return _description ? _description : "";
}


/*------------------------------------------------------------------------------
 * function: Session::GetDataCatalog
 * Get the per-session data source catalog.
 */
DataCatalog *
Session::GetDataCatalog()
{
#if defined(DEBUG)
  printf("Session::GetDataCatalog()\n");
#endif

  if (!_dataCat) {
    _catFile = tempnam("/tmp", "dscat");

    // Create the file.
    FILE *fp = fopen(_catFile, "w");
    if (!fp) {
      reportErrSys("Can't create catalog file");
    } else {
      fclose(fp);
    }

    _dataCat = new DataCatalog(_catFile);
  }

  return _dataCat;
}

/*------------------------------------------------------------------------------
 * function: Session::ReadSession
 * Read and execute a DEVise session file.
 */
DevStatus
Session::ReadSession(ControlPanelSimple *control, const char *filename, CommandProc cp)
{
#if defined(DEBUG)
  printf("Session::ReadSession(%s)\n", filename);
#endif

  DevStatus result = StatusOk;

  FILE *fp = fopen(filename, "r");
  if (!fp) {
	char errBuf[MAXPATHLEN * 2];
	sprintf(errBuf, "Unable to open session file <%s>", filename);
    reportErrSys(errBuf);
	result += StatusFailed;
  } else {
    const int bufSize = 1024;
	char lineBuf[bufSize];

	while (ReadCommand(fp, lineBuf, bufSize) && result.IsComplete()) {
#if defined(DEBUG)
      printf("  read line: %s", lineBuf);
#endif

      RemoveTrailingSemicolons(lineBuf);

#if defined(DEBUG)
      printf("  semicolons removed: %s", lineBuf);
#endif

	  if (!IsBlankOrComment(lineBuf)) {
		ArgList args;
	    if (args.ParseString(lineBuf).IsComplete()) {
#if defined(DEBUG)
	      printf("Arguments: ");
          PrintArgs(stdout, args.GetCount(), args.GetArgs(), true);
#endif
		  result += cp(control, args.GetCount(),
		      (char **)args.GetArgs());
	    }
	  }
	}

    if (fclose(fp) != 0) {
	  char errBuf[MAXPATHLEN * 2];
	  sprintf("Error closing session file <%s>", filename);
      reportErrSys(errBuf);
	}
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: Session::ReadCommand
 * Read a single command from the session file.  (Note that a command may
 * extend over several lines, or several commands may be on one line,
 * separated by semicolons.)
 * Returns true if a command was successfully read.
 */
Boolean
Session::ReadCommand(FILE *fp, char buf[], int bufSize)
{
  Boolean result = true;

  Boolean done = false;
  char *ptr = buf;
  char *last = buf + bufSize - 1;
  int leftBraces = 0;
  int rightBraces = 0;

  while (!done) {
    int tmpC = getc(fp);

	if (tmpC == EOF) {
	  done = true;
	  result = false;
	} else if (tmpC == ';' || tmpC == '\n') {
	  if (leftBraces == rightBraces) {
		// Normal end of command.
	    done = true;
		result = true;
	  }
	}

	if (!done) {
	  *ptr++ = tmpC;

	  if (tmpC == '{') {
	    leftBraces++;
	  } else if (tmpC == '}') {
	    rightBraces++;
	  }

	  if (ptr >= last) {
	    done = true;
		reportErrNosys("Command buffer too short");
		result = false;
	  }
	}
  }

  *ptr = '\0';

  return result;
}

/*------------------------------------------------------------------------------
 * function: Session::IsBlankOrComment
 * Determine whether is string is a blank or comment string, or not.
 * Comments have '#' as the first non-whitespace character.
 */
Boolean
Session::IsBlankOrComment(const char *str)
{
#if defined(DEBUG)
  printf("Session::IsBlankOrComment(%s)\n", str);
#endif

  Boolean result = true;

  while (*str) {
	if (*str == '#') {
	  break;
	} else if (!isspace(*str)) {
	  result = false;
	  break;
	}
    str++;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: Session::RemoveTrailingSemicolons
 * Removes trailing semicolons from the given string.
 */
void
Session::RemoveTrailingSemicolons(char *str)
{
#if defined(DEBUG)
  printf("Session::RemoveTrailingSemicolons(%s)\n", str);
#endif

  // Note: Tcl uses semicolons as command separators; we're not currently
  // implementing this.  However, we need to get rid of any trailing
  // semicolons so they're not parsed as arguments.  RKW 1999-09-20.
  char *tmpC = &str[strlen(str) - 1];
  while (tmpC >= str) {
    if (*tmpC == ';') {
      *tmpC = ' ';
    } else if (!isspace(*tmpC)) {
      break;
    }
    tmpC--;
  }
}

/*------------------------------------------------------------------------------
 * function: Session::RunCommand
 * Run the appropriate command.
 * Note: this function needs a ControlPanelSimple object because it uses
 * the GetResult() method not found in ControlPanel.
 */
DevStatus
Session::RunCommand(ControlPanelSimple *control, int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::RunCommand(");
  PrintArgs(stdout, argc, argv, false);
  printf(")\n");
#endif

  DevStatus result = StatusOk;
  char errBuf[1024];

  if (!strcmp(argv[0], "DEVise")) {
    result += DEViseCmd(control, argc, argv);
  } else if (!strcmp(argv[0], "OpenDataSource")) {
	result += OpenDataSourceCmd(control, argc, argv);
  } else if (!strcmp(argv[0], "scanDerivedSources")) {
	result += ScanDerivedSourcesCmd(control, argc, argv);
  } else if (!strcmp(argv[0], "SetDescription")) {
	result += SetDescriptionCmd(control, argc, argv);
  } else {
    sprintf(errBuf, "Unrecognized command: <%s>", argv[0]);
	reportErrNosys(errBuf);
    result = StatusFailed;
  }

#if defined(DEBUG)
  printf("  finished command %s; result = ", argv[1]);
  result.Print();
  printf("    result = {%s}\n", control->GetResult());
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: Session::FilterCommand
 * Run any command that set's a view's visual filter (this is used to reset
 * all visual filters to their original values as defined in the session file).
 * Note: this function needs a ControlPanelSimple object because it uses
 * the GetResult() method not found in ControlPanel.
 */
DevStatus
Session::FilterCommand(ControlPanelSimple *control, int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::FilterCommand(");
  PrintArgs(stdout, argc, argv, false);
  printf(")\n");
#endif

  DevStatus result = StatusOk;

  if (!strcmp(argv[0], "DEVise") && !strcmp(argv[1], "create") &&
      !strcmp(argv[2], "view")) {
#if defined(DEBUG)
    PrintArgs(stdout, argc, argv, true);
#endif
    ArgList args(7);
    args.AddArg(argv[0]);
    args.AddArg("setFilter");
    args.AddArg(argv[4]); // view name
    args.AddArg(argv[5]); // x low
    args.AddArg(argv[7]); // y low
    args.AddArg(argv[6]); // x high
    args.AddArg(argv[8]); // y high

    // Don't check result here, otherwise we stop if a view is missing, for
    // example.
    DEViseCmd(control, args.GetCount(), args.GetArgs());
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: Session::DEViseCmd
 * DEVise command procedure for session file interpreter.
 */
DevStatus
Session::DEViseCmd(ControlPanel *control, int argc, const char * const *argv)
{
#if defined(DEBUG)
  printf("Session::DEViseCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  DevStatus status = StatusOk;

  // Don't do anything for "DEVise create tdata...", "DEVise importFileType",
  // "DEVise dteImportFileType", and "DEVise dataSegment" commands.
  if (!strcmp(argv[1], "create") && !strcmp(argv[2], "tdata")) {
    // No op.
	control->ReturnVal(0, "");
  } else if (!strcmp(argv[1], "importFileType")) {
    // No op.
	control->ReturnVal(0, "");
  } else if (!strcmp(argv[1], "dteImportFileType")) {
    // No op.
	control->ReturnVal(0, "");
  } else if (!strcmp(argv[1], "dataSegment")) {
    // No op.
	control->ReturnVal(0, "");
  } else {
    // don't pass DEVise command verb (argv[0])
    if (CmdContainer::GetCmdContainer()->RunOneCommand(argc-1, &argv[1],
	    control) <= 0) {
      status = StatusFailed;
      fprintf(stderr, "Error in command: ");
      PrintArgs(stderr, argc, argv);
    }
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::OpenDataSourceCmd
 * OpenDataSource command procedure for session file interpreter.
 */
DevStatus
Session::OpenDataSourceCmd(ControlPanel *control, int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::OpenDataSourceCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  DevStatus status = StatusOk;

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

  control->ReturnVal(0, result);

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::ScanDerivedSourcesCmd
 * ScanDerivedSources command procedure for session file interpreter.
 */
DevStatus
Session::ScanDerivedSourcesCmd(ControlPanel *control, int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::ScanDerivedSourcesCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  DevStatus status = StatusOk;

  // This command is a no-op -- we just need to have the command exist
  // because it's in session files -- derived sources are goofed up
  // right now anyhow.

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::SetDescriptionCmd
 * SetDescription command procedure for session file interpreter.
 */
DevStatus
Session::SetDescriptionCmd(ControlPanel *control, int argc, char *argv[])
{
#if defined(DEBUG)
  printf("Session::SetDescriptionCmd() ");
  PrintArgs(stdout, argc, argv);
#endif

  DevStatus status = StatusOk;

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
  status += SaveParams(saveData, "viewGetImplicitHome",
      "viewSetImplicitHome", instance);

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

  status += SaveParams(saveData, "getXAxisFloatFormat", "setXAxisFloatFormat",
      instance, NULL, NULL, true);
  status += SaveParams(saveData, "getYAxisFloatFormat", "setYAxisFloatFormat",
      instance, NULL, NULL, true);

  status += SaveParams(saveData, "getViewAutoFilter", "setViewAutoFilter",
      instance, NULL, NULL, true);

  status += SaveParams(saveData, "getDupElim", "setDupElim",
      instance, NULL, NULL, true);

  status += SaveParams(saveData, "getNiceAxes", "setNiceAxes",
      instance, NULL, NULL, false);

  status += SaveParams(saveData, "viewGetDisabledActions",
      "viewSetDisabledActions", instance, NULL, NULL, false);

  status += SaveParams(saveData, "getViewHelp", "setViewHelp", instance,
      NULL, NULL, true);

  status += SaveParams(saveData, "getShowMouseLocation",
      "setShowMouseLocation", instance, NULL, NULL, true);

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

    const char *result;
	ArgList args;
    status += CallParseAPI(saveData->control, result, false, args,
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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, true, args,
      "getLinkViews", instance);
  if (status.IsComplete()) {
    int index;
    for (index = 0; index < args.GetCount(); index++) {
      fprintf(saveData->fp, "DEVise insertLink {%s} {%s}\n", instance,
	      args.GetArgs()[index]);
    }
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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, true, args,
      "getCursorViews", instance);
  if (status.IsComplete()) {
    const char *source = args.GetArgs()[0];
    const char *dest = args.GetArgs()[1];
    if (strlen(source) > 0) {
      fprintf(saveData->fp, "DEVise setCursorSrc {%s} {%s}\n", instance,
          source);
    }
    if (strlen(dest) > 0) {
      fprintf(saveData->fp, "DEVise setCursorDst {%s} {%s}\n", instance,
          dest);
    }
  }

  args.Cleanup();
  status += CallParseAPI(saveData->control, result, true, args,
      "color", "GetCursorColor", instance);
  if (status.IsComplete()) {
    fprintf(saveData->fp, "DEVise color SetCursorColor {%s} %s\n", instance,
	    args.GetArgs()[0]);
  }

  status += SaveParams(saveData, "getCursorConstraints",
      "setCursorConstraints", instance);

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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, true, args,
      "getViewMappings", instance);
  if (status.IsComplete()) {
    int index;
    for (index = 0; index < args.GetCount(); index++) {
      fprintf(saveData->fp, "DEVise insertMapping {%s} {%s}\n", instance,
          args.GetArgs()[index]);
      status += SaveParams(saveData, "getMappingLegend", "setMappingLegend",
          instance, args.GetArgs()[index]);
    }
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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, true, args,
      "getWinViews", instance);
  if (status.IsComplete()) {
    //
    // Insert views into windows.
    //
    int index;
    for (index = 0; index < args.GetCount(); index++) {
      fprintf(saveData->fp, "DEVise insertWindow {%s} {%s}\n",
	      args.GetArgs()[index], instance);
    }

    //
    // Save view geometries if window layout is custom.
    //
    ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
    Layout *window = (Layout *)classDir->FindInstance(instance);
    LayoutMode mode;
    window->GetLayoutMode(mode);
    if (mode == CUSTOM) {
      for (index = 0; index < args.GetCount(); index++) {
        status += SaveParams(saveData, "getViewGeometry", "setViewGeometry",
            args.GetArgs()[index], NULL, NULL, false);
      }
    }
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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, true, args,
      "getVisualFilters", instance);
  if (status.IsComplete()) {
    int index;
    for (index = 0; index < args.GetCount(); index++) {
      fprintf(saveData->fp, "DEVise insertViewHistory {%s} %s\n", instance,
          args.GetArgs()[index]);
    }
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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, true, args,
      "get3DLocation", instance);
  if (status.IsComplete()) {
    fprintf(saveData->fp, "DEVise set3DLocation {%s} ", instance);
    PrintArgs(saveData->fp, 9, args.GetArgs());
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
    const char *arg0, const char *arg1, const char *arg2, Boolean addBraces)
{
#if defined(DEBUG)
  printf("Session::SaveParams(%s, %s, %s, %s, %s)\n", getCommand, setCommand,
      arg0, arg1 ? arg1 : "null", arg2 ? arg2 : "null");
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

  const char *result;
  ArgList args;
  status += CallParseAPI(saveData->control, result, false, args,
      getCommand, arg0, arg1, arg2);
  if (status.IsComplete()) {
    if (strlen(result) > 0) { // Note: session files won't work without this
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
 * Run the given command; parse the result into individual arguments if
 * requested.
 * Note: this function needs a ControlPanelSimple object because it uses
 * the GetResult() method not found in ControlPanel.
 */
DevStatus
Session::CallParseAPI(ControlPanelSimple *control, const char *&result,
    Boolean splitResult, ArgList &args, const char *arg0,
	const char *arg1 = NULL, const char *arg2 = NULL, const char *arg3 = NULL)
{
#if defined(DEBUG)
  printf("Session::CallParseAPI(%s)\n", arg0);
#endif

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
  const char *argvIn[4];
  argvIn[0] = arg0;
  argvIn[1] = arg1;
  argvIn[2] = arg2;
  argvIn[3] = arg3;
  if (CmdContainer::GetCmdContainer()->RunOneCommand(argcIn, (char **)argvIn,
      control) <= 0) {
    reportErrNosys(control->GetResult());
    status = StatusFailed;
  } else {
    result = control->GetResult();
#if defined(DEBUG)
    printf("  result = <%s>\n", result);
#endif
    if (splitResult) {
      DevStatus tmpStatus = args.ParseString(result);
	  if (!tmpStatus.IsComplete()) {
        reportErrNosys(result);
	  }
      status += tmpStatus;
    } else {
	  args.Cleanup();
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
  const char **tmpArgv;
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
  const char **tmpArgv;
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

  const char **tmpArgv;

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

/*------------------------------------------------------------------------------
 * function: Session::SaveDataSources
 * Save all per-session data sources.
 */
DevStatus
Session::SaveDataSources(FILE *fp)
{
#if defined(DEBUG)
  printf("Session::SaveDataSources()\n");
#endif

  DevStatus status = StatusOk;

  char *sourceList = GetDataCatalog()->ListCatalog(".");
#if defined(DEBUG)
  printf("  sourceList = <%s>\n", sourceList);
#endif

  if (sourceList) {
	ArgList args;
    status += args.ParseString(sourceList);

	if (status.IsComplete()) {
	  if (args.GetCount() > 0) {
	    fprintf(fp, "\n# Per-session data source definitions\n");
	  }

	  for (int sourceNum = 0; sourceNum < args.GetCount(); sourceNum++) {
#if defined(DEBUG)
        printf("  source[%d] = <%s>\n", sourceNum, args.GetArgs()[sourceNum]);
#endif
		ArgList args2;
        DevStatus tmpStatus = args2.ParseString(args.GetArgs()[sourceNum]);
		status += tmpStatus;
		if (tmpStatus.IsComplete()) {
		  if (args2.GetCount() != 2) {
			reportErrNosys("Incorrect catalog listing format");
			status += StatusFailed;
		  } else {
#if defined(DEBUG)
            printf("  sourceName = <%s>\n", args2.GetArgs()[0]);
#endif
		    char *catEntry = GetDataCatalog()->ShowEntry(args2.GetArgs()[0]);
			if (!catEntry || strlen(catEntry) == 0) {
			  reportErrNosys("Can't find catalog entry");
			  status += StatusFailed;
			} else {
#if defined(DEBUG)
              printf("  catEntry = <%s>\n", catEntry);
#endif

              // Get rid of trailing semicolon and spaces so we don't gain
			  // another semicolon each time we save.
	          char *tmpC = &catEntry[strlen(catEntry) - 1];
	          while (tmpC >= catEntry) {
	            if (*tmpC == ';' || isspace(*tmpC)) {
	              *tmpC = '\0';
	            } else {
	              break;
	            }
	            tmpC--;
	          }

			  fprintf(fp, "DEVise dteInsertCatalogEntry . {%s}\n", catEntry);
			}
		  }
		}
	  }
	}

    FreeString(sourceList);
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::DeleteDataSources
 * Delete all per-session data sources.
 */
DevStatus
Session::DeleteDataSources()
{
#if defined(DEBUG)
  printf("Session::DeleteDataSources()\n");
#endif

  DevStatus status = StatusOk;

  if (_dataCat) {
    delete _dataCat;
    _dataCat = NULL;
  }

  if (_catFile) {
    if (unlink(_catFile) != 0) {
      reportErrSys("Can't delete session data catalog file");
    }
    FreeString(_catFile);
    _catFile = NULL;
  }

  return status;
}

/*------------------------------------------------------------------------------
 * function: Session::CheckWindowLocations
 * Warn the user if any windows extent off the screen.
 */
DevStatus
Session::CheckWindowLocations()
{
#if defined(DEBUG)
  printf("Session::CheckWindowLocations()\n");
#endif

  DevStatus status = StatusOk;

  int winIndex = DevWindow::InitIterator();
  while (DevWindow::More(winIndex)) {
    ClassInfo *info = DevWindow::Next(winIndex);
    ViewWin *window = (ViewWin *)info->GetInstance();
    if (window) {
      int argc;
      const char **argv;
      info->CreateParams(argc, argv);
      Coord relX = atof(argv[1]);
      Coord relY = atof(argv[2]);
      Coord relWidth = atof(argv[3]);
      Coord relHeight = atof(argv[4]);
#if defined(DEBUG)
      printf("Relative window geometry: %g, %g, %g, %g\n", relX, relY,
          relWidth, relHeight);
#endif
      if (relX < 0.0 || relX + relWidth > 1.0 || relY < 0.0 ||
          relY + relHeight > 1.0) {
	    char errBuf[256];
	    sprintf(errBuf, "Warning: window <%s> extends outside of screen",
	    window->GetName());
        reportErrNosys(errBuf);
		status += StatusWarn;
      }
    }
  }
  DevWindow::DoneIterator(winIndex);

  return status;
}

/*============================================================================*/
