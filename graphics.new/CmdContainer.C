/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2005
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
  Revision 1.82  2003/01/13 19:25:21  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.81  2002/06/17 19:41:06  wenger
  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.

  Revision 1.80  2002/05/01 21:30:11  wenger
  Merged V1_7b0_br thru V1_7b0_br_1 to trunk.

  Revision 1.79.4.16  2005/09/06 22:04:55  wenger
  Added proper const-ness to HashTable.

  Revision 1.79.4.15  2005/09/06 21:20:16  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.79.4.14  2004/04/23 21:57:15  wenger
  Added new 'select next view in pile' feature.

  Revision 1.79.4.13  2003/12/22 22:47:29  wenger
  JavaScreen support for print color modes is now in place.

  Revision 1.79.4.12  2003/11/19 19:40:20  wenger
  Display modes now work for symbol colors; also added some missing
  commands to the (horrible) Tcl code for copying views; minor
  improvement to error reporting.

  Revision 1.79.4.11  2003/11/05 17:01:52  wenger
  First part of display modes for printing is implemented (view foreground
  and background colors work, haven't done anything for symbol colors yet).

  Revision 1.79.4.10  2003/09/23 21:55:20  wenger
  "Option" dialog now displays JSPoP and DEVise version, and JSPoP ID.

  Revision 1.79.4.9  2003/06/06 20:48:42  wenger
  Implemented provision for automatic testing of DEVise, including
  running Tcl test scripts within DEVise itself.

  Revision 1.79.4.8  2003/02/04 19:41:15  wenger
  Added union capability for multiple GData attribute links (will help
  with restraint visualizations for BMRB).

  Revision 1.79.4.7  2003/01/09 22:21:58  wenger
  Added "link multiplication factor" feature; changed version to 1.7.14.

  Revision 1.79.4.6  2002/11/15 22:44:34  wenger
  Views with no TData records don't contribute to filter values on 'home'
  (this helps to fix some problems with the Condor user visualizations);
  added cursorHome command; changed version to 1.7.13.

  Revision 1.79.4.5  2002/09/21 23:24:37  wenger
  Fixed a few more special-case memory leaks.

  Revision 1.79.4.4  2002/09/02 21:29:33  wenger
  Did a bunch of Purifying -- the biggest change is storing the command
  objects in a HashTable instead of an Htable -- the Htable does a bunch
  of ugly memory copying.

  Revision 1.79.4.3  2002/06/11 17:27:37  wenger
  Added an option for a view to not "contribute" to home on its visual
  links; this allows a simplification of the NRG sessions, which fixes
  bug 753.

  Revision 1.79.4.2  2002/05/20 21:21:47  wenger
  Fixed bug 779 (client switching problem with multiple DEViseds).

  Revision 1.79.4.1  2002/04/18 17:25:42  wenger
  Merged js_tmpdir_fix_br_2 to V1_7b0_br (this fixes the problems with
  temporary session files when the JSPoP and DEViseds are on different
  machines).  Note: JS protocol version is now 11.0.

  Revision 1.79.8.4  2002/04/18 15:41:22  wenger
  Further cleanup of JavaScreen temporary session file code (added
  JAVAC_DeleteTmpSession command) (includes fixing bug 774).

  Revision 1.79.8.3  2002/04/17 20:14:47  wenger
  Implemented new JAVAC_OpenTmpSession command to go along with
  JAVAC_SaveTmpSession (so the JSPoP doesn't need to have any info about
  the path of the temporary session directory relative to the base
  session directory).

  Revision 1.79.8.2  2002/04/17 19:14:10  wenger
  Changed JAVAC_SaveSession command to JAVAC_SaveTmpSession (path is
  now relative to temp session directory, not main session directory).

  Revision 1.79.8.1  2002/04/17 17:46:26  wenger
  DEVised, not JSPoP, now does the actual work of creating or clearing
  the temporary session directory (new command from client to DEVised
  means that communication protocol version is now 11.0).  (Client
  switching is not working yet with this code because I need to change
  how temporary sessions are saved and loaded.)

  Revision 1.79  2001/11/28 21:56:41  wenger
  Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.

  Revision 1.78.6.1  2001/11/19 21:03:53  wenger
  Added JAVAC_RefreshData command and jsdevisec.refreshAllData method for
  Squid to be able to force DEVise to re-read all data and update the
  visualization; did some cleanup of JavaScreenCmd.C.

  Revision 1.78  2001/10/08 19:21:02  wenger
  Fixed bug 702 (JavaScreen locks up on unrecognized command in DEVised).

  Revision 1.77  2001/10/03 19:09:56  wenger
  Various improvements to error logging; fixed problem with return value
  from JavaScreenCmd::Run().

  Revision 1.76  2001/09/24 15:29:11  wenger
  Added warning if you close or quit with unsaved session changes (note
  that visual filter changes are not considered "changes").

  Revision 1.75  2001/08/28 18:30:19  wenger
  Added 'robustOpen' option (default is true) -- this allows a session
  open to succeed even if there are unrecognized commands (useful for
  opening a newer session file with an older version of DEVise); if a
  session open fails, we now clean things up.

  Revision 1.74  2001/06/12 15:29:45  wenger
  Implemented a choice of modulus (default) or truncate color modes.

  Revision 1.73  2001/05/03 19:39:10  wenger
  Changed negative axis flag to multiplicative factor to be more flexible;
  pass multiplicative factor to JS to correct mouse location display (mods
  to JAVAC_ViewDataArea command); corrected mouse location display in DEVise
  Tcl GUI.

  Revision 1.72  2001/04/23 18:58:40  wenger
  Added negative axis label option (no GUI yet) to allow us to display
  chemical shifts the way the BMRB people want.

  Revision 1.71  2001/04/02 16:09:56  wenger
  Devised now saves configuration for 3D JavaScreen views to sessions,
  and passes it to the JavaScreen when necessary (note: JS protocol
  version is now 6.0).

  Revision 1.70  2001/02/20 20:02:52  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.69.2.2  2001/02/16 21:37:58  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.69.2.1  2001/01/31 22:18:23  wenger
  Added 'stop' command to DEVise; tcl GUI now sends 'stop' instead
  of sending 'abortQuery' to each view.

  Revision 1.69  2001/01/08 20:32:52  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

  Revision 1.66.2.2  2000/12/27 19:39:18  wenger
  Merged changes from js_restart_improvements thru zero_js_cache_check from
  the trunk onto the js_cgi_br branch.

  Revision 1.68  2000/11/17 22:59:06  wenger
  Fixed problems with command logging of cursor movements and pile/stack
  flips.

  Revision 1.66.2.1  2000/10/18 20:32:08  wenger
  Merged changes from fixed_bug_616 through link_gui_improvements onto
  the branch.

  Revision 1.67  2000/09/14 19:39:06  wenger
  Added GUI to view and change cursor type (X, Y, or XY).

  Revision 1.66  2000/08/30 20:09:04  wenger
  Added the option of forcing a cursor to be entirely within its destination
  view; added control for whether a cursor must be at least partially within
  its destination view; generally improved implementation of cursor
  constraints.

  Revision 1.65  2000/07/12 20:49:26  wenger
  Added first version of metavisualization session description; changed
  DEVise version to 1.7.1.

  Revision 1.64  2000/06/20 22:17:04  wenger
  Added floating-point format for axes and mouse location display.

  Revision 1.63  2000/06/20 16:57:43  wenger
  Added commands and GUI to enable/disable the display of mouse location
  in various views, and globally.

  Revision 1.62  2000/03/30 16:27:11  wenger
  Added printInstances command; destroy command now reports an error
  if the instance is not found.

  Revision 1.61  2000/02/23 21:31:07  wenger
  Re-implemented session description capability.

  Revision 1.60  2000/02/08 22:11:58  wenger
  Added JAVAC_GetViewHelp and JAVAC_ShowViewHelp commands, added color
  edge grid, and type to JAVAC_DrawCursor command, JavaScreen protocol
  version now 4.0; added GUI to edit view help, and commands to save it
  to session files.

  Revision 1.59  2000/01/14 18:23:14  wenger
  Added resetAllFilters and JAVAC_ResetFilters commands to reset all visual
  filters back to the values defined in the session file, without actually
  re-opening the session.

  Revision 1.58  1999/12/14 17:57:35  wenger
  Added enableDrawing command (totally enables or disables drawing) to
  allow Omer to avoid "flashing" when he inserts views into windows.

  Revision 1.57  1999/12/06 18:41:02  wenger
  Simplified and improved command logging (includes fixing bug 537, command
  logs are now human-readable); added standard header to debug logs.

  Revision 1.56  1999/11/30 22:28:19  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.55  1999/11/29 21:08:47  wenger
  Fixed bug 535 and partially fixed bug 532 (problems with view order in
  piles); removed (unused) replaceView command and related ViewWin methods

  Revision 1.54  1999/11/24 15:44:21  wenger
  Removed (unnecessary) CommandObj class; commands are now logged for the
  monolithic form, not just the client/server form; other command-related
  cleanups; added GUI for playing back command logs.

  Revision 1.53  1999/11/19 18:19:02  wenger
  Removed (not really used) DeviseCommandOption class and related code.

  Revision 1.52  1999/11/19 17:17:41  wenger
  Added View::SetVisualFilterCommand() method to clean up command-related
  code for filter setting.

  Revision 1.51  1999/11/19 15:23:17  wenger
  Added dispatcherRun1 command (for debugging).

  Revision 1.50  1999/10/05 17:55:46  wenger
  Added debug log level.

  Revision 1.49  1999/09/27 20:41:18  wenger
  Unrecognized commands are now logged.

  Revision 1.48  1999/09/07 19:00:53  wenger
  dteInsertCatalogEntry command changed to tolerate an attempt to insert
  duplicate entries without causing a problem (to allow us to get rid of
  Tcl in session files); changed Condor session scripts to take out Tcl
  control statements in data source definitions; added viewGetImplicitHome
  and related code in Session class so this gets saved in session files
  (still no GUI for setting this, though); removed SEQ-related code.

  Revision 1.47  1999/08/19 20:46:34  wenger
  Added JAVAC_ProtocolVersion command.

  Revision 1.46  1999/08/19 13:54:23  wenger
  Changes for JavaScreen support: all 15 shape attributes now sent in
  GData; added zoom in/out argument to JAVAC_MouseAction_RubberBand;
  JAVAC_MouseAction_DoubleClick changed to JAVAC_ShowRecords.

  Revision 1.45  1999/08/13 17:22:45  wenger
  Custom view layouts are now saved to session files; removed now unused
  TileLayout code.

  Revision 1.44  1999/07/21 18:51:09  wenger
  Moved alignment and data font information from view into mapping.

  Revision 1.43  1999/07/14 18:42:51  wenger
  Added the capability to have axes without ticks and tick labels.

  Revision 1.42  1999/06/16 18:28:33  wenger
  Fixed bug 499 (problem with 'Next in Pile' not working for piled view
  symbols).

  Revision 1.41  1999/06/16 17:08:48  wenger
  Added raiseAllWindows command and corresponding GUI.

  Revision 1.40  1999/06/11 14:47:03  wenger
  Added the capability (mostly for the JavaScreen) to disable rubberband
  lines, cursor movement, drill down, and key actions in views (the code
  to send this info to the JS is still conditionaled out until the JS is
  ready for it).

  Revision 1.39  1999/05/17 18:37:57  wenger
  Views now have GData sending configuration that is only employed when
  connecting to the JavaScreen (eliminates the need for the current kludgey
  setup to send GData to the JS).

  Revision 1.38  1999/05/14 16:46:48  wenger
  View vertical scroll can now be configured by the user.

  Revision 1.37  1999/04/29 17:36:41  wenger
  Implemented 'fixed cursor size' option (for the sake of the JavaScreen).

  Revision 1.36  1999/04/22 19:29:50  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.35  1999/04/14 15:30:16  wenger
  Improved 'switch TData': moved the code from Tcl to C++, functionality
  is more flexible -- schemas don't have to match exactly as long as the
  appropriate TData attributes are present; TData can now be specified for
  view symbols in parent view mapping; updated shape help.

  Revision 1.34  1999/03/24 17:26:10  wenger
  Non-DTE data source code prevents adding duplicate data source names;
  added "nice axis" feature (sets axis limits to multiples of powers of
  10 if enabled); improved the propagation of DEVise errors back to the
  GUI; fixed bug 474 (problem with view home).

  Revision 1.33  1999/03/12 18:46:01  wenger
  Implemented duplicate symbol elimination.

  Revision 1.32  1999/03/10 19:11:04  wenger
  Implemented DataReader schema GUI; made other improvements to schema
  editing GUI, such as sorting the schema lists.

  Revision 1.31  1999/03/04 15:11:10  wenger
  Implemented 'automatic filter update' features: views can be designated
  to have their visual filters automatically updated with the 'Update
  Filters' menu selection; alternatively, a session can be opened with
  the 'Open, Update, and Save' selection, which updates the designated
  filters and saves the updated session.

  Revision 1.30  1999/03/01 23:09:09  wenger
  Fixed a number of memory leaks and removed unused code.

  Revision 1.29  1999/03/01 17:47:43  wenger
  Implemented grouping/ungrouping of views to allow custom view geometries.

  Revision 1.28  1999/02/11 19:54:54  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.27  1999/01/04 15:33:27  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.26.2.2  1999/02/11 18:24:18  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.26.2.1  1998/12/29 17:25:10  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.26  1998/12/22 19:39:26  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.25  1998/12/18 22:21:00  wenger
  Removed axis label code, which doesn't seem to have been fully implemented,
  and is not used; added sanity check on visual filter at view creation.

  Revision 1.24  1998/12/15 14:55:09  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.23  1998/11/11 14:30:55  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.22  1998/11/06 17:59:44  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.21  1998/11/02 19:22:41  wenger
  Added "range/MQL" session description capability.

  Revision 1.20  1998/10/28 19:22:25  wenger
  Added code to check all data sources (runs through the catalog and tries
  to open all of them); this includes better error handling in a number of
  data source-related areas of the code; also fixed errors in the propagation
  of command results.

  Revision 1.19  1998/10/21 17:16:36  wenger
  Fixed bug 101 (problems with the '5' (home) key); added "Set X, Y to
  Show All" (go home) button to Query dialog; fixed bug 421 (crash when
  closing set link sessions); fixed bug 423 (saving session file over
  directory).

  Revision 1.18  1998/10/20 19:46:12  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.17  1998/09/08 20:26:13  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

  Revision 1.16  1998/08/17 17:12:01  wenger
  Devised now responds to KeyAction commands from JavaScreen.

  Revision 1.15  1998/08/03 18:38:36  wenger
  Implemented JAVAC_ServerExit and JAVAC_SaveSession commands; partly
  implemented several other new commands for the JavaScreen.

  Revision 1.14  1998/07/29 14:20:26  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.13  1998/07/03 23:42:18  wenger
  Fixed some memory leaks; added provision to print data segment size
  at certain places in the code.

  Revision 1.12  1998/06/12 19:55:27  wenger
  Attribute of TAttr/set links can now be changed; GUI has menu of available
  attributes; attribute is set when master view is set instead of at link
  creation; misc. debug code added.

  Revision 1.11  1998/05/29 19:34:36  wenger
  Added JAVAC_SetDisplaySize to allow the JavaScreen to set the display
  size.

  Revision 1.10  1998/05/21 18:18:46  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.9  1998/05/14 18:21:41  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.8  1998/05/02 09:00:38  taodb
  Added support for JAVA Screen and command logging

  Revision 1.7  1998/04/07 14:30:05  wenger
  Reduced unnecessary include dependencies.

  Revision 1.6  1998/03/27 15:08:52  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

  Revision 1.5  1998/03/19 18:36:26  taodb
  Fixed core dump for illegal devise commands

  Revision 1.4  1998/03/08 00:01:03  wenger
  Fixed bugs 115 (I think -- can't test), 128, and 311 (multiple-link
  update problems) -- major changes to visual links.

  Revision 1.3  1998/03/03 05:21:03  taodb
  Added extendable stack to command object

  Revision 1.2  1998/03/02 22:03:38  taodb
  Add control parameter to Run() invocations

  Revision 1.1  1998/02/26 20:48:32  taodb
  Replaced ParseAPI() with Command Object Interface

*/

#include <string>
#include <vector>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>

#include "ParseAPI.h"

//#define INLINE_TRACE
#include "debug.h"
#include "CmdContainer.h"
#include "DeviseServer.h"
#include "Csprotocols.h"
#include "DeviseCommand.h"
#include "Util.h"
#include "DevError.h"
#include "ControlPanelSimple.h"
#include "Session.h"
#include "Init.h"
#include "JavaScreenCmd.h"

template class HashTable<char *, DeviseCommand *>;

//#define DEBUG
#define DEBUG_LOG

// Note: the object allocated by each of these macros is leaked, but you
// can't destroy them easily because a bitwise copy of the object's contents
// is done, so you'll end up destroying other things the object points to.
#define REGISTER_COMMAND(objType)\
{\
	DeviseCommand_##objType *obj= new (DeviseCommand_##objType)();\
	insertCmd(#objType,(DeviseCommand*)obj,sizeof(DeviseCommand_##objType)); \
}

static CmdContainer*	cmdContainerP = NULL;

int GetDisplayImageAndSize(ControlPanel *control, int port, char *imageType);
int GetWindowImageAndSize(ControlPanel *control, int port, char *imageType,
    char *windowName);
int		ParseAPIDTE(int argc, char** argv, ControlPanel* control);
int		ParseAPIColorCommands(int argc, char** argv, ControlPanel* control);

CmdContainer *
CmdContainer::GetCmdContainer()
{
    return cmdContainerP;
}

void
CmdContainer::GenerateCommand(int argc, const char* const *argv)
{
    CmdSource cmdSrc(CmdSource::USER, CLIENT_INVALID);
    CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::FOR_SERVER);
    ControlPanelSimple control;
    CmdContainer::GetCmdContainer()->Run(argc, argv, &control, cmdDes);
}

CmdContainer::CmdContainer(ControlPanel* defaultControl,CmdContainer::Make make,
	DeviseServer* server)
{
#if defined(DEBUG)
    printf("CmdContainer::CmdContainer()\n");
#endif

	_commands = new HashTable<char *, DeviseCommand *>(128, CmdHash, CmdComp);

	DeviseCommand::setDefaultControl(defaultControl);
	this->make = make;

    DOASSERT(!cmdContainerP, "Can only create one CmdContainer object!");
	cmdContainerP = this;

	_server = server;

	_collabEnabled = true;

	// JAVA Screen commands
	REGISTER_COMMAND(JAVAC_GetSessionList)
	REGISTER_COMMAND(JAVAC_OpenSession)
	REGISTER_COMMAND(JAVAC_CloseCurrentSession)
	REGISTER_COMMAND(JAVAC_Exit)
	REGISTER_COMMAND(JAVAC_MouseAction_Click)
	REGISTER_COMMAND(JAVAC_ShowRecords)
	REGISTER_COMMAND(JAVAC_MouseAction_RubberBand)
	REGISTER_COMMAND(JAVAC_SetDisplaySize)
	REGISTER_COMMAND(JAVAC_KeyAction)
	REGISTER_COMMAND(JAVAC_SaveTmpSession)
	REGISTER_COMMAND(JAVAC_ServerExit)
	REGISTER_COMMAND(JAVAC_ServerCloseSocket)
	REGISTER_COMMAND(JAVAC_ImageChannel)
	REGISTER_COMMAND(JAVAC_CursorChanged)
	REGISTER_COMMAND(JAVAC_ProtocolVersion)
	REGISTER_COMMAND(JAVAC_ShowRecords3D)
	REGISTER_COMMAND(JAVAC_ResetFilters)
	REGISTER_COMMAND(JAVAC_GetViewHelp)
	REGISTER_COMMAND(JAVAC_Set3DConfig)
	REGISTER_COMMAND(JAVAC_RefreshData)
	REGISTER_COMMAND(JAVAC_CreateTmpSessionDir)
	REGISTER_COMMAND(JAVAC_OpenTmpSession)
	REGISTER_COMMAND(JAVAC_DeleteTmpSession)
	REGISTER_COMMAND(JAVAC_SetTmpSessionDir)
	REGISTER_COMMAND(JAVAC_GetDeviseVersion)
	REGISTER_COMMAND(JAVAC_SetDisplayMode)

	REGISTER_COMMAND(dteImportFileType)
	REGISTER_COMMAND(dteListAllIndexes)
	REGISTER_COMMAND(dteDeleteCatalogEntry)
	REGISTER_COMMAND(dteMaterializeCatalogEntry)
	REGISTER_COMMAND(dteReadSQLFilter)
	REGISTER_COMMAND(dteShowCatalogEntry)
	REGISTER_COMMAND(dteListCatalog)
	REGISTER_COMMAND(dteListQueryAttributes)
	REGISTER_COMMAND(dteListAttributes)
	REGISTER_COMMAND(dteDeleteIndex)
	REGISTER_COMMAND(dteShowIndexDesc)
	REGISTER_COMMAND(dteShowAttrNames)
	REGISTER_COMMAND(dteInsertCatalogEntry)
	REGISTER_COMMAND(dteCheckSQLViewEntry)
	REGISTER_COMMAND(dteCreateIndex)
	REGISTER_COMMAND(color)
	REGISTER_COMMAND(getAllViews)
	REGISTER_COMMAND(changeParam)
	REGISTER_COMMAND(createInterp)
	REGISTER_COMMAND(create)
	REGISTER_COMMAND(getTDataName)
	REGISTER_COMMAND(showkgraph)
	REGISTER_COMMAND(createMappingClass)
	REGISTER_COMMAND(setDefault)
	REGISTER_COMMAND(setHistogram)
	REGISTER_COMMAND(getHistogram)

#if defined(KSB_MAYBE_DELETE_THIS_OLD_STATS_STUFF)
	REGISTER_COMMAND(setBuckRefresh)
#endif
	REGISTER_COMMAND(setHistViewname)
	REGISTER_COMMAND(getHistViewname)
	REGISTER_COMMAND(checkGstat)
	REGISTER_COMMAND(getSourceName)
	REGISTER_COMMAND(isXDateType)
	REGISTER_COMMAND(isYDateType)
	REGISTER_COMMAND(mapG2TAttr)
	REGISTER_COMMAND(mapT2GAttr)
	REGISTER_COMMAND(startLayoutManager)
	REGISTER_COMMAND(date)
	REGISTER_COMMAND(printDispatcher)
	REGISTER_COMMAND(catFiles)
	REGISTER_COMMAND(exit)
	REGISTER_COMMAND(clearAll)
	REGISTER_COMMAND(sync)
	REGISTER_COMMAND(version)
	REGISTER_COMMAND(copyright)
	REGISTER_COMMAND(compDate)
	REGISTER_COMMAND(new_leaks)
	REGISTER_COMMAND(new_inuse)
	REGISTER_COMMAND(getWinCount)
	REGISTER_COMMAND(getStringCount)
	REGISTER_COMMAND(waitForQueries)
	REGISTER_COMMAND(serverExit)
	REGISTER_COMMAND(abortQuery)
	REGISTER_COMMAND(importFileType)
	REGISTER_COMMAND(resetLinkMaster)
	REGISTER_COMMAND(get3DLocation)
	REGISTER_COMMAND(getLinkMaster)
	REGISTER_COMMAND(getLinkType)
	REGISTER_COMMAND(setBatchMode)
	REGISTER_COMMAND(invalidateTData)
	REGISTER_COMMAND(invalidatePixmap)
	REGISTER_COMMAND(readLine)
	REGISTER_COMMAND(close)
	REGISTER_COMMAND(isMapped)
	REGISTER_COMMAND(getLabel)
	REGISTER_COMMAND(tdataFileName)
	REGISTER_COMMAND(getViewWin)
	REGISTER_COMMAND(clearViewHistory)
	REGISTER_COMMAND(getCursorViews)
	REGISTER_COMMAND(getMappingTData)
	REGISTER_COMMAND(destroy)
	REGISTER_COMMAND(parseDateFloat)
	REGISTER_COMMAND(isInterpretedGData)
	REGISTER_COMMAND(isInterpreted)
	REGISTER_COMMAND(getPixelWidth)
	REGISTER_COMMAND(getTopGroups)
	REGISTER_COMMAND(getWindowLayout)
	REGISTER_COMMAND(getSchema)
	REGISTER_COMMAND(getAction)
	REGISTER_COMMAND(getLinkFlag)
	REGISTER_COMMAND(changeableParam)
	REGISTER_COMMAND(getInstParam)
	REGISTER_COMMAND(tcheckpoint)
	REGISTER_COMMAND(get)
	REGISTER_COMMAND(changeMode)
	REGISTER_COMMAND(exists)
	REGISTER_COMMAND(removeView)
	REGISTER_COMMAND(getViewMappings)
	REGISTER_COMMAND(refreshView)
	REGISTER_COMMAND(getWinGeometry)
	REGISTER_COMMAND(getWinViews)
	REGISTER_COMMAND(getLinkViews)
	REGISTER_COMMAND(getCurVisualFilter)
	REGISTER_COMMAND(getVisualFilters)
	REGISTER_COMMAND(getViewStatistics)
	REGISTER_COMMAND(getAllStats)
	REGISTER_COMMAND(getStatBuffer)
	REGISTER_COMMAND(getViewDimensions)
	REGISTER_COMMAND(getViewSolid3D)
	REGISTER_COMMAND(getViewXYZoom)
	REGISTER_COMMAND(getViewDisplayDataValues)
	REGISTER_COMMAND(getViewPileMode)
	REGISTER_COMMAND(raiseView)
	REGISTER_COMMAND(lowerView)
	REGISTER_COMMAND(getFileHeader)
	REGISTER_COMMAND(winGetPrint)
	REGISTER_COMMAND(viewGetHome)
	REGISTER_COMMAND(viewGetHorPan)
	REGISTER_COMMAND(getCursorGrid)
	REGISTER_COMMAND(writeDesc)
	REGISTER_COMMAND(saveStringSpace)
	REGISTER_COMMAND(loadStringSpace)
	REGISTER_COMMAND(dumpLinkCursor)
	REGISTER_COMMAND(openSession)
	REGISTER_COMMAND(createTData)
	REGISTER_COMMAND(getViewGDS)
	REGISTER_COMMAND(testDataSock)
	//REGISTER_COMMAND(viewGetAlign)
	REGISTER_COMMAND(setLinkMaster)
	REGISTER_COMMAND(setLinkType)
	REGISTER_COMMAND(setScreenSize)
	REGISTER_COMMAND(writeLine)
	REGISTER_COMMAND(open)
	REGISTER_COMMAND(setViewStatistics)
	REGISTER_COMMAND(setViewDimensions)
	REGISTER_COMMAND(setViewSolid3D)
	REGISTER_COMMAND(setViewXYZoom)
	REGISTER_COMMAND(setViewDisplayDataValues)
	REGISTER_COMMAND(setViewPileMode)
	REGISTER_COMMAND(savePixmap)
	REGISTER_COMMAND(loadPixmap)
	REGISTER_COMMAND(getAxisDisplay)
	REGISTER_COMMAND(setCursorSrc)
	REGISTER_COMMAND(setCursorDst)
	REGISTER_COMMAND(setPixelWidth)
	REGISTER_COMMAND(setAction)
	REGISTER_COMMAND(setLinkFlag)
	REGISTER_COMMAND(highlightView)
	REGISTER_COMMAND(getparam)
	REGISTER_COMMAND(insertMapping)
	REGISTER_COMMAND(getMappingLegend)
	REGISTER_COMMAND(insertLink)
	REGISTER_COMMAND(viewInLink)
	REGISTER_COMMAND(unlinkView)
	REGISTER_COMMAND(insertWindow)
	REGISTER_COMMAND(removeMapping)
	REGISTER_COMMAND(saveDisplayImage)
	REGISTER_COMMAND(saveDisplayView)
	REGISTER_COMMAND(saveTdata)
	REGISTER_COMMAND(getDisplayImage)
	REGISTER_COMMAND(getDisplayImageAndSize)
	REGISTER_COMMAND(getFont)
	REGISTER_COMMAND(viewSetAlign)
	REGISTER_COMMAND(checkTDataForRecLink)
	REGISTER_COMMAND(setMappingLegend)
	REGISTER_COMMAND(markViewFilter)
	REGISTER_COMMAND(getWindowImage)
	REGISTER_COMMAND(getWindowImageAndSize)
	REGISTER_COMMAND(swapView)
	REGISTER_COMMAND(setAxisDisplay)
	REGISTER_COMMAND(getCreateParam)
	REGISTER_COMMAND(getItems)
	REGISTER_COMMAND(setWindowLayout)
	REGISTER_COMMAND(saveWindowImage)
	REGISTER_COMMAND(setViewOverrideColor)
	REGISTER_COMMAND(parseSchema)
	REGISTER_COMMAND(winSetPrint)
	REGISTER_COMMAND(setLabel)
	REGISTER_COMMAND(dataSegment)
	REGISTER_COMMAND(viewSetHorPan)
	REGISTER_COMMAND(setCursorGrid)
	REGISTER_COMMAND(saveSession)
	REGISTER_COMMAND(setWinGeometry)
	REGISTER_COMMAND(setFilter)
	REGISTER_COMMAND(saveDisplayImageAndMap)
	REGISTER_COMMAND(insertViewHistory)
	REGISTER_COMMAND(setFont)
	REGISTER_COMMAND(set3DLocation)
	REGISTER_COMMAND(setViewGDS)
	REGISTER_COMMAND(viewSetHome)
	REGISTER_COMMAND(viewSetImplicitHome)
	REGISTER_COMMAND(viewGetImplicitHome)
	REGISTER_COMMAND(playLog)
	REGISTER_COMMAND(test)
	REGISTER_COMMAND(getLinkMasterAttr)
	REGISTER_COMMAND(getLinkSlaveAttr)
	REGISTER_COMMAND(setLinkMasterAttr)
	REGISTER_COMMAND(setLinkSlaveAttr)
	REGISTER_COMMAND(selectView)
	REGISTER_COMMAND(setShowViewNames)
	REGISTER_COMMAND(getShowViewNames)
	REGISTER_COMMAND(getCountMapping)
	REGISTER_COMMAND(setCountMapping)
	REGISTER_COMMAND(getCursorType)
	REGISTER_COMMAND(viewGoHome)
	REGISTER_COMMAND(writeRangeDesc)
	REGISTER_COMMAND(viewSetStringTable)
	REGISTER_COMMAND(viewGetStringTable)
	REGISTER_COMMAND(viewSetIsHighlight)
	REGISTER_COMMAND(viewGetIsHighlight)
	REGISTER_COMMAND(getXAxisDateFormat)
	REGISTER_COMMAND(getYAxisDateFormat)
	REGISTER_COMMAND(setXAxisDateFormat)
	REGISTER_COMMAND(setYAxisDateFormat)
	REGISTER_COMMAND(updateAxisTypes)
	REGISTER_COMMAND(getViewGeometry)
	REGISTER_COMMAND(setViewGeometry)
	REGISTER_COMMAND(setPileStackState)
	REGISTER_COMMAND(getPileStackState)
	REGISTER_COMMAND(flipPileStack)
	REGISTER_COMMAND(groupUngroupViews)
	REGISTER_COMMAND(updateSession)
	REGISTER_COMMAND(getViewAutoFilter)
	REGISTER_COMMAND(setViewAutoFilter)
	REGISTER_COMMAND(updateFilters)
	REGISTER_COMMAND(parseDRSchema)
	REGISTER_COMMAND(getDupElim)
	REGISTER_COMMAND(setDupElim)
	REGISTER_COMMAND(niceifyAxes)
	REGISTER_COMMAND(getNiceAxes)
	REGISTER_COMMAND(setNiceAxes)
	REGISTER_COMMAND(switchTData)
	REGISTER_COMMAND(getCursorFixedSize)
	REGISTER_COMMAND(setCursorFixedSize)
	REGISTER_COMMAND(viewGetVerPan)
	REGISTER_COMMAND(viewSetVerPan)
	REGISTER_COMMAND(viewGetJSSendP)
	REGISTER_COMMAND(viewSetJSSendP)
	REGISTER_COMMAND(viewGetDisabledActions)
	REGISTER_COMMAND(viewSetDisabledActions)
	REGISTER_COMMAND(raiseAllWindows)
	REGISTER_COMMAND(nextViewInPile)
	REGISTER_COMMAND(setAxisTicks)
	REGISTER_COMMAND(getAxisTicks)
	REGISTER_COMMAND(dispatcherRun1)
	REGISTER_COMMAND(removeViewFromPile)
	REGISTER_COMMAND(setOpeningSession)
	REGISTER_COMMAND(enableDrawing)
	REGISTER_COMMAND(resetAllFilters)
	REGISTER_COMMAND(getViewHelp)
	REGISTER_COMMAND(setViewHelp)
	REGISTER_COMMAND(setSessionDesc)
	REGISTER_COMMAND(getSessionDesc)
	REGISTER_COMMAND(printInstances)
	REGISTER_COMMAND(setShowMouseLocation)
	REGISTER_COMMAND(getShowMouseLocation)
	REGISTER_COMMAND(getXAxisFloatFormat)
	REGISTER_COMMAND(getYAxisFloatFormat)
	REGISTER_COMMAND(setXAxisFloatFormat)
	REGISTER_COMMAND(setYAxisFloatFormat)
	REGISTER_COMMAND(writeMetaVisDesc)
	REGISTER_COMMAND(getCursorConstraints)
	REGISTER_COMMAND(setCursorConstraints)
	REGISTER_COMMAND(getCursorFlag)
	REGISTER_COMMAND(setCursorFlag)
	REGISTER_COMMAND(stop)
	REGISTER_COMMAND(back)
	REGISTER_COMMAND(forward)
	REGISTER_COMMAND(getJS3dValid)
	REGISTER_COMMAND(getJS3dConfig)
	REGISTER_COMMAND(setJS3dConfig)
	REGISTER_COMMAND(setAxisNegLabel)
	REGISTER_COMMAND(setAxisMultFact)
	REGISTER_COMMAND(getAxisMultFact)
	REGISTER_COMMAND(setColorMode)
	REGISTER_COMMAND(getColorMode)
	REGISTER_COMMAND(sessionIsDirty)
	REGISTER_COMMAND(setDoHomeOnVisLink)
	REGISTER_COMMAND(getDoHomeOnVisLink)
	REGISTER_COMMAND(cursorHome)
	REGISTER_COMMAND(setLinkMultFact)
	REGISTER_COMMAND(getLinkMultFact)
	REGISTER_COMMAND(setGAttrLinkMode)
	REGISTER_COMMAND(getGAttrLinkMode)
	REGISTER_COMMAND(testExit)
	REGISTER_COMMAND(keyToView)
	REGISTER_COMMAND(setDisplayMode)
	REGISTER_COMMAND(getDisplayMode)
	REGISTER_COMMAND(setMappingColors)
	REGISTER_COMMAND(getMappingColors)
	REGISTER_COMMAND(selectNextInPile)
}

CmdContainer::~CmdContainer()
{
#if defined(DEBUG)
    printf("CmdContainer::~CmdContainer()\n");
#endif

	// Delete the command objects (note that the command name strings
	// are static).
	int buckIdx;
	void *next;
	_commands->InitRetrieveEntries(buckIdx, next);
	char *name;
	DeviseCommand *cmd;
	while (_commands->RetrieveEntries(buckIdx, next, name, cmd) == 0) {
		delete cmd;
	}

	// Delete the actual command HashTable.
	delete _commands;

	// clean logfile upon completion
}

int
CmdContainer::RunInternal(int argc, const char* const *argv,
    ControlPanel* control)
{
    CmdSource cmdSrc(CmdSource::INTERNAL, CLIENT_INVALID);
    CmdDescriptor cmdDes(cmdSrc, CmdDescriptor::UNDEFINED);
    return Run(argc, argv, control, cmdDes);
}

int
CmdContainer::Run(int argc, const char* const *argv, ControlPanel* control, 
	CmdDescriptor& cmdDes)
{
	int		retval = 1;
	char*	errmsg = NULL;
	int		cid;
	bool	inGroup = false;
	bool	activeGroup = false;

#if defined(DEBUG)
    printf("CmdContainer::Run(");
	PrintArgs(stdout, argc, argv, false);
    printf(")\n");
#endif
	
	// no command logging & broadcasting for log file replay and session replay
	// control command routing 
	CmdSource::SrcType srcType = cmdDes.getCmdsource()->getSrctype();
	if (cmdDes.getCmdsource()->isFromLog()) {
		srcType = CmdSource::LOGFILE;
	} else if (getMake() == MONOLITHIC) {
	    srcType = CmdSource::INTERNAL;
	}

	switch (srcType)
	{
		case CmdSource::LOGFILE:
				retval = RunOneCommand(argc, argv, control);
				break;

		case CmdSource::NETWORK:
			if (cmdDes.getDest() == CmdDescriptor::FOR_CLIENT)
			{
				if (_collabEnabled) {
				    // send control commands to its client
				    // which CLIENT?
				    //TEMP -- remove typecast on argv
				    ((Server*)_server)->SendControl(API_CTL, argc,
					  (char **)argv, true);
				}
			}
			else
			if (cmdDes.getDest() == CmdDescriptor::FOR_SERVER)
			{
				retval = RunOneCommand(argc, argv, control);
			}
			break;

		case CmdSource::JAVACLIENT:
			// run JAVA commands with logging turned off
			JavaScreenCmd::CmdInit();
			retval = RunOneCommand(argc, argv, control);
			JavaScreenCmd::CmdTerminate();
			break;

		case CmdSource::USER:
		case CmdSource::CLIENT:
			cid = cmdDes.getCmdsource()->getClientid();
			retval = -1;

			// access-control, this is a temporary hack
			// we need better solution!
			// by default, cid =0, is always the group communication client
			// we use default for USER commands, since at this time
			// we do not know SERVER is sending commands for which
			// group control client.

			ClientInfo* cInfo;
			if (cid != CLIENT_INVALID) {
				cInfo = _server->getClientInfo(cid);
			} else {
				cInfo = _server->getClientInfo(0);
			}

			inGroup = (cInfo->gname != NULL);
			activeGroup = inGroup?(cInfo->active):false;

			if (!inGroup)
			{
				// always allowed
				retval = RunOneCommand(argc, argv, control);
			}
			else 
			if (!activeGroup)
			{
				// commands are disallowd
				retval = -1;
				errmsg = "Non-leader should not operate!";
				_server->ReturnVal(cid, (u_short)API_NAK, errmsg);
			}
			else
			{
				// group cast commands first
				//TEMP -- remove typecast on argv
				retval = _server->GroupCast(cInfo->gname, cInfo->cname,
						SS_CSS_Cmd,
						argc, (char **)argv, errmsg) ? 1 : -1;

				// run it locally
				if (retval <= 0)
					cerr <<"Error in groupcast:"<<errmsg<<endl;
				retval = RunOneCommand(argc, argv, control);
			}
			break;

		case CmdSource::INTERNAL:
			retval = RunOneCommand(argc, argv, control);
			break;

		default:
			errmsg = "Illegal command source type\n";
			break;
	}

	// set return value
	cmdDes.setRetval(retval>0 ? true : false, errmsg);
	if (retval <= 0)
		cerr << cmdDes.getErrmsg();
	return retval;
}

int
CmdContainer::RunOneCommand(int argc, const char* const *argv, ControlPanel* control) 
{
#if defined(DEBUG)
    printf("CmdContainer::RunOneCommand(");
	PrintArgs(stdout, argc, argv, false);
    printf(")\n");
#endif

	int	retval;
	DeviseCommand* cmd;

	cmd = lookupCmd(argv[0]);
	if (cmd == NULL)
	{
        reportErrArgs("Unrecognized command: ", argc, argv, devNoSyserr);
		control->ReturnVal(API_NAK, "Unrecognized command");
		retval = -1;
		if (Session::OpeningSession() && Init::RobustOpen()) {
			// Continue opening the session.
		    retval = 1;
		}
	}
	else
	{
		//TEMP -- remove typecast on argv
		retval = cmd->Run(argc, (char **)argv, control);
		if (retval != 1) {
		    reportErrArgs("Error in command: ", argc, argv, devNoSyserr);
		}
	}
	return retval;
}

void
CmdContainer::insertCmd(char* cmdName, DeviseCommand* cmdp, int cmdsize)
{
    _commands->insert(cmdName, cmdp);
}

DeviseCommand*
CmdContainer::lookupCmd(const char* cmdName)
{
	DeviseCommand* 	cmdp = NULL;
	char *tmpName = (char *)cmdName;
	if (_commands->lookup(tmpName, cmdp) != 0) {
	    cmdp = NULL;
	}

	return cmdp;
}

void
CmdContainer::deleteCmd(char* cmdName)
{
	DeviseCommand *cmdp = lookupCmd(cmdName);
	if (cmdp != NULL) {
	    delete cmdp;
	}
	_commands->remove(cmdName);
}

ostream&
operator <<(ostream&os, const CmdContainer& cc)
{
	return os;
}

// Note that this is a bad hash function if there are more than 256 buckets.
int
CmdContainer::CmdHash(char * const &index, int numBuckets)
{
	const char *cp = index;
    unsigned char hash = 0;
	while (*cp != '\0') {
		hash ^= *cp;
	    cp++;
	}

	return hash % numBuckets;
}

int
CmdContainer::CmdComp(char * const &index1, char * const &index2)
{
    return strcmp(index1, index2);
}
