/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2004
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of command classes for all DEVise commands.
 */

/*
  $Id$

  $Log$
  Revision 1.73  2003/01/13 19:25:22  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.72  2002/06/17 19:41:06  wenger
  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.

  Revision 1.71  2002/05/01 21:30:12  wenger
  Merged V1_7b0_br thru V1_7b0_br_1 to trunk.

  Revision 1.70.4.13  2005/09/28 17:14:50  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.70.4.12  2004/04/23 21:57:15  wenger
  Added new 'select next view in pile' feature.

  Revision 1.70.4.11  2003/12/22 22:47:30  wenger
  JavaScreen support for print color modes is now in place.

  Revision 1.70.4.10  2003/11/19 19:40:20  wenger
  Display modes now work for symbol colors; also added some missing
  commands to the (horrible) Tcl code for copying views; minor
  improvement to error reporting.

  Revision 1.70.4.9  2003/11/05 17:01:52  wenger
  First part of display modes for printing is implemented (view foreground
  and background colors work, haven't done anything for symbol colors yet).

  Revision 1.70.4.8  2003/09/23 21:55:23  wenger
  "Option" dialog now displays JSPoP and DEVise version, and JSPoP ID.

  Revision 1.70.4.7  2003/06/06 20:48:43  wenger
  Implemented provision for automatic testing of DEVise, including
  running Tcl test scripts within DEVise itself.

  Revision 1.70.4.6  2003/02/04 19:41:16  wenger
  Added union capability for multiple GData attribute links (will help
  with restraint visualizations for BMRB).

  Revision 1.70.4.5  2003/01/09 22:21:59  wenger
  Added "link multiplication factor" feature; changed version to 1.7.14.

  Revision 1.70.4.4  2002/11/15 22:44:35  wenger
  Views with no TData records don't contribute to filter values on 'home'
  (this helps to fix some problems with the Condor user visualizations);
  added cursorHome command; changed version to 1.7.13.

  Revision 1.70.4.3  2002/06/11 17:27:38  wenger
  Added an option for a view to not "contribute" to home on its visual
  links; this allows a simplification of the NRG sessions, which fixes
  bug 753.

  Revision 1.70.4.2  2002/05/20 21:21:48  wenger
  Fixed bug 779 (client switching problem with multiple DEViseds).

  Revision 1.70.4.1  2002/04/18 17:25:44  wenger
  Merged js_tmpdir_fix_br_2 to V1_7b0_br (this fixes the problems with
  temporary session files when the JSPoP and DEViseds are on different
  machines).  Note: JS protocol version is now 11.0.

  Revision 1.70.8.4  2002/04/18 15:41:24  wenger
  Further cleanup of JavaScreen temporary session file code (added
  JAVAC_DeleteTmpSession command) (includes fixing bug 774).

  Revision 1.70.8.3  2002/04/17 20:14:48  wenger
  Implemented new JAVAC_OpenTmpSession command to go along with
  JAVAC_SaveTmpSession (so the JSPoP doesn't need to have any info about
  the path of the temporary session directory relative to the base
  session directory).

  Revision 1.70.8.2  2002/04/17 19:14:11  wenger
  Changed JAVAC_SaveSession command to JAVAC_SaveTmpSession (path is
  now relative to temp session directory, not main session directory).

  Revision 1.70.8.1  2002/04/17 17:46:27  wenger
  DEVised, not JSPoP, now does the actual work of creating or clearing
  the temporary session directory (new command from client to DEVised
  means that communication protocol version is now 11.0).  (Client
  switching is not working yet with this code because I need to change
  how temporary sessions are saved and loaded.)

  Revision 1.70  2001/11/28 21:56:42  wenger
  Merged collab_cleanup_br_2 through collab_cleanup_br_6 to the trunk.

  Revision 1.69.6.1  2001/11/19 21:03:55  wenger
  Added JAVAC_RefreshData command and jsdevisec.refreshAllData method for
  Squid to be able to force DEVise to re-read all data and update the
  visualization; did some cleanup of JavaScreenCmd.C.

  Revision 1.69  2001/09/24 15:29:11  wenger
  Added warning if you close or quit with unsaved session changes (note
  that visual filter changes are not considered "changes").

  Revision 1.68  2001/06/12 15:29:46  wenger
  Implemented a choice of modulus (default) or truncate color modes.

  Revision 1.67  2001/05/03 19:39:11  wenger
  Changed negative axis flag to multiplicative factor to be more flexible;
  pass multiplicative factor to JS to correct mouse location display (mods
  to JAVAC_ViewDataArea command); corrected mouse location display in DEVise
  Tcl GUI.

  Revision 1.66  2001/04/23 18:58:41  wenger
  Added negative axis label option (no GUI yet) to allow us to display
  chemical shifts the way the BMRB people want.

  Revision 1.65  2001/04/02 16:09:57  wenger
  Devised now saves configuration for 3D JavaScreen views to sessions,
  and passes it to the JavaScreen when necessary (note: JS protocol
  version is now 6.0).

  Revision 1.64  2001/02/20 20:02:53  wenger
  Merged changes from no_collab_br_0 thru no_collab_br_2 from the branch
  to the trunk.

  Revision 1.63.2.2  2001/02/16 21:37:59  wenger
  Updated DEVise version to 1.7.2; implemented 'forward' and 'back' (like
  a web browser) on 'sets' of visual filters.

  Revision 1.63.2.1  2001/01/31 22:18:24  wenger
  Added 'stop' command to DEVise; tcl GUI now sends 'stop' instead
  of sending 'abortQuery' to each view.

  Revision 1.63  2001/01/08 20:32:53  wenger
  Merged all changes thru mgd_thru_dup_gds_fix on the js_cgi_br branch
  back onto the trunk.

  Revision 1.61.2.1  2000/10/18 20:32:09  wenger
  Merged changes from fixed_bug_616 through link_gui_improvements onto
  the branch.

  Revision 1.62  2000/09/14 19:39:16  wenger
  Added GUI to view and change cursor type (X, Y, or XY).

  Revision 1.61  2000/08/30 20:09:05  wenger
  Added the option of forcing a cursor to be entirely within its destination
  view; added control for whether a cursor must be at least partially within
  its destination view; generally improved implementation of cursor
  constraints.

  Revision 1.60  2000/08/16 14:14:44  wenger
  Removed _args and _numArgs (used automatic variables instead).

  Revision 1.59  2000/07/12 20:49:27  wenger
  Added first version of metavisualization session description; changed
  DEVise version to 1.7.1.

  Revision 1.58  2000/06/20 22:17:05  wenger
  Added floating-point format for axes and mouse location display.

  Revision 1.57  2000/06/20 16:57:45  wenger
  Added commands and GUI to enable/disable the display of mouse location
  in various views, and globally.

  Revision 1.56  2000/03/30 16:27:12  wenger
  Added printInstances command; destroy command now reports an error
  if the instance is not found.

  Revision 1.55  2000/02/23 21:31:11  wenger
  Re-implemented session description capability.

  Revision 1.54  2000/02/16 18:51:38  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.53  2000/02/08 22:11:59  wenger
  Added JAVAC_GetViewHelp and JAVAC_ShowViewHelp commands, added color
  edge grid, and type to JAVAC_DrawCursor command, JavaScreen protocol
  version now 4.0; added GUI to edit view help, and commands to save it
  to session files.

  Revision 1.52  2000/01/14 18:23:16  wenger
  Added resetAllFilters and JAVAC_ResetFilters commands to reset all visual
  filters back to the values defined in the session file, without actually
  re-opening the session.

  Revision 1.51  2000/01/13 23:07:06  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.50  1999/12/14 17:57:36  wenger
  Added enableDrawing command (totally enables or disables drawing) to
  allow Omer to avoid "flashing" when he inserts views into windows.

  Revision 1.49  1999/11/30 22:28:21  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.48  1999/11/29 21:08:49  wenger
  Fixed bug 535 and partially fixed bug 532 (problems with view order in
  piles); removed (unused) replaceView command and related ViewWin methods

  Revision 1.47  1999/11/22 18:13:18  wenger
  Fixed 'command buffer conflict' errors, other command-related cleanup.

  Revision 1.46  1999/11/19 18:19:02  wenger
  Removed (not really used) DeviseCommandOption class and related code.

  Revision 1.45  1999/09/07 19:00:56  wenger
  dteInsertCatalogEntry command changed to tolerate an attempt to insert
  duplicate entries without causing a problem (to allow us to get rid of
  Tcl in session files); changed Condor session scripts to take out Tcl
  control statements in data source definitions; added viewGetImplicitHome
  and related code in Session class so this gets saved in session files
  (still no GUI for setting this, though); removed SEQ-related code.

  Revision 1.44  1999/08/19 20:46:36  wenger
  Added JAVAC_ProtocolVersion command.

  Revision 1.43  1999/08/19 13:54:25  wenger
  Changes for JavaScreen support: all 15 shape attributes now sent in
  GData; added zoom in/out argument to JAVAC_MouseAction_RubberBand;
  JAVAC_MouseAction_DoubleClick changed to JAVAC_ShowRecords.

  Revision 1.42  1999/08/13 17:22:47  wenger
  Custom view layouts are now saved to session files; removed now unused
  TileLayout code.

  Revision 1.41  1999/07/21 18:51:11  wenger
  Moved alignment and data font information from view into mapping.

  Revision 1.40  1999/07/14 18:42:53  wenger
  Added the capability to have axes without ticks and tick labels.

  Revision 1.39  1999/06/16 18:28:33  wenger
  Fixed bug 499 (problem with 'Next in Pile' not working for piled view
  symbols).

  Revision 1.38  1999/06/16 17:08:47  wenger
  Added raiseAllWindows command and corresponding GUI.

  Revision 1.37  1999/06/11 14:47:05  wenger
  Added the capability (mostly for the JavaScreen) to disable rubberband
  lines, cursor movement, drill down, and key actions in views (the code
  to send this info to the JS is still conditionaled out until the JS is
  ready for it).

  Revision 1.36  1999/05/17 18:37:58  wenger
  Views now have GData sending configuration that is only employed when
  connecting to the JavaScreen (eliminates the need for the current kludgey
  setup to send GData to the JS).

  Revision 1.35  1999/05/14 16:46:50  wenger
  View vertical scroll can now be configured by the user.

  Revision 1.34  1999/04/29 17:36:41  wenger
  Implemented 'fixed cursor size' option (for the sake of the JavaScreen).

  Revision 1.33  1999/04/22 19:29:52  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.32  1999/04/16 20:59:23  wenger
  Fixed various bugs related to view symbols, including memory problem
  with MappingInterp dimensionInfo; updated create_condor_session script
  to take advantage of view symbol TData switching capability.

  Revision 1.31  1999/04/14 15:30:18  wenger
  Improved 'switch TData': moved the code from Tcl to C++, functionality
  is more flexible -- schemas don't have to match exactly as long as the
  appropriate TData attributes are present; TData can now be specified for
  view symbols in parent view mapping; updated shape help.

  Revision 1.30  1999/03/24 17:26:12  wenger
  Non-DTE data source code prevents adding duplicate data source names;
  added "nice axis" feature (sets axis limits to multiples of powers of
  10 if enabled); improved the propagation of DEVise errors back to the
  GUI; fixed bug 474 (problem with view home).

  Revision 1.29  1999/03/12 18:46:03  wenger
  Implemented duplicate symbol elimination.

  Revision 1.28  1999/03/10 19:11:07  wenger
  Implemented DataReader schema GUI; made other improvements to schema
  editing GUI, such as sorting the schema lists.

  Revision 1.27  1999/03/04 15:11:11  wenger
  Implemented 'automatic filter update' features: views can be designated
  to have their visual filters automatically updated with the 'Update
  Filters' menu selection; alternatively, a session can be opened with
  the 'Open, Update, and Save' selection, which updates the designated
  filters and saves the updated session.

  Revision 1.26  1999/03/01 17:47:47  wenger
  Implemented grouping/ungrouping of views to allow custom view geometries.

  Revision 1.25  1999/02/11 19:54:56  wenger
  Merged newpile_br through newpile_br_1 (new PileStack class controls
  pile and stacks, allows non-linked piles; various other improvements
  to pile-related code).

  Revision 1.24  1999/01/04 15:33:30  wenger
  Improved View symbol code; removed NEW_LAYOUT and VIEW_SHAPE conditional
  compiles; added code (GUI is currently disabled) to manually set view
  geometry (not yet saved to sessions).

  Revision 1.23.2.2  1999/02/11 18:24:21  wenger
  PileStack objects are now fully working (allowing non-linked piles) except
  for a couple of minor bugs; new PileStack state is saved to session files;
  piles and stacks in old session files are dealt with reasonably well;
  incremented version number; added some debug code.

  Revision 1.23.2.1  1998/12/29 17:25:13  wenger
  First version of new PileStack objects implemented -- allows piles without
  pile links.  Can't be saved or restored in session files yet.

  Revision 1.23  1998/12/22 19:39:29  wenger
  User can now change date format for axis labels; fixed bug 041 (axis
  type not being changed between float and date when attribute is changed);
  got dates to work semi-decently as Y axis labels; view highlight is now
  always drawn by outlining the view; fixed some bugs in drawing the highight.

  Revision 1.22  1998/12/18 22:21:02  wenger
  Removed axis label code, which doesn't seem to have been fully implemented,
  and is not used; added sanity check on visual filter at view creation.

  Revision 1.21  1998/12/15 14:55:13  wenger
  Reduced DEVise memory usage in initialization by about 6 MB: eliminated
  Temp.c (had huge global arrays); eliminated Object3D class and greatly
  simplified Map3D; removed ViewLens class (unused); got rid of large static
  buffers in a number of other source files.

  Revision 1.20  1998/11/11 14:30:57  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.19  1998/11/06 17:59:47  wenger
  Multiple string tables fully working -- allows separate tables for the
  axes in a given view.

  Revision 1.18  1998/11/02 19:22:44  wenger
  Added "range/MQL" session description capability.

  Revision 1.17  1998/10/21 17:16:40  wenger
  Fixed bug 101 (problems with the '5' (home) key); added "Set X, Y to
  Show All" (go home) button to Query dialog; fixed bug 421 (crash when
  closing set link sessions); fixed bug 423 (saving session file over
  directory).

  Revision 1.16  1998/10/20 19:46:16  wenger
  Mapping dialog now displays the view's TData name; "Next in Pile" button
  in mapping dialog allows user to edit the mappings of all views in a pile
  without actually flipping them; user has the option to show all view names;
  new GUI to display info about all links and cursors; added API and GUI for
  count mappings.

  Revision 1.15  1998/09/22 17:23:55  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.14  1998/09/21 16:47:43  wenger
  Fixed bug 395 (Condorview GIF dumping problem) (caused by waitForQueries
  not returning a value); made Dispatcher::WaitForQueries a little safer;
  added code in DeviseCommand class to print an error message and return
  a default value if a command does not return a value.

  Revision 1.13  1998/09/08 20:26:17  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

  Revision 1.12  1998/08/10 19:08:06  wenger
  Moved command result buffer from DeviseCommand class to ControlPanel
  class -- saves 7 MB of memory!

  Revision 1.11  1998/08/03 18:38:39  wenger
  Implemented JAVAC_ServerExit and JAVAC_SaveSession commands; partly
  implemented several other new commands for the JavaScreen.

  Revision 1.10  1998/07/07 17:59:04  wenger
  Moved #define of PURIFY from DeviseCommand.h to DeviseCommand.C so the
  "outside world" doesn't see it.

  Revision 1.9  1998/06/12 19:55:29  wenger
  Attribute of TAttr/set links can now be changed; GUI has menu of available
  attributes; attribute is set when master view is set instead of at link
  creation; misc. debug code added.

  Revision 1.8  1998/05/29 19:34:39  wenger
  Added JAVAC_SetDisplaySize to allow the JavaScreen to set the display
  size.

  Revision 1.7  1998/05/21 18:18:48  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.6  1998/05/13 13:36:38  wenger
  Fixed some dynamic memory errors in the csgroup code; cleaned up
  DeviseCommand class somewhat -- simplified the calling of Run()
  functions in subclasses.

  Revision 1.5  1998/05/02 09:00:42  taodb
  Added support for JAVA Screen and command logging

  Revision 1.4  1998/03/27 15:08:56  wenger
  Added dumping of logical session description, added GUI for dumping
  logical or physical description; cleaned up some of the command code
  a little.

 */

#ifndef _DeviseCommand_h_
#define _DeviseCommand_h_
#include <iostream.h>
#include "ClassDir.h"
#include "Control.h"
#include "ExtStack.h"
#include "DeviseHistory.h"

#define DECLARE_CLASS_DeviseCommand_(command_name)\
class DeviseCommand_##command_name: public DeviseCommand\
{\
    public:\
    DeviseCommand_##command_name():\
        DeviseCommand(){}\
    ~DeviseCommand_##command_name(){}\
	private: /* only base class can Run(), we need to initialize control */\
\
	/* 1 = OK, -1 = error */ \
    int Run(int argc, char** argv);
#define DECLARE_CLASS_END };

class DeviseCommand
{
	friend class CmdContainer;
	friend ostream& operator <<(ostream&, DeviseCommand&);
	public:
		static ControlPanel* getDefaultControl();
		static int GetCmdDepth() { return _cmdDepth; }

	private:
		static	ControlPanel* _defaultControl;
		static void setDefaultControl(ControlPanel* defaultControl);
		ExtStack	*_controlStack;
		void pushControl(ControlPanel* control)
		{
			_controlStack->push((void*)control);
		}
		void popControl()
		{
			 _controlStack->pop();
			 _control =(ControlPanel*) _controlStack->top();
		}
		static int _cmdDepth;

	protected:
		// only friend class can construct this class
		ControlPanel* _control;
		DeviseCommand()
		{
			_controlStack = new ExtStack(5, NULL);
			_result = NULL;
		}
		virtual ~DeviseCommand()
		{
			delete _controlStack;
		}

	    // 1 = OK, -1 = error
		virtual int Run(int argc, char** argv) = 0;

		virtual int ReturnVal(u_short flag, const char *result);
		//TEMP -- should be const char **
		virtual int ReturnVal(int argc, char **argv);
	protected:
        char		*_result;
		int			_resultCapacity;
		ClassDir	*_classDir;

	public:
	    // 1 = OK, -1 = error
		virtual int Run(int argc, char** argv, ControlPanel* cntl);
};
ostream& operator <<(ostream& os, DeviseCommand& cmd);

//-------------------------------------------------------------------------
//				Commands Dedicated for JAVA Screen - Begin
//-------------------------------------------------------------------------
//
//Class definition: DeviseCommand_JAVAC_GetSessionList
//
DECLARE_CLASS_DeviseCommand_(JAVAC_GetSessionList)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_Exit
//
DECLARE_CLASS_DeviseCommand_(JAVAC_Exit)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_CloseCurrentSession
//
DECLARE_CLASS_DeviseCommand_(JAVAC_CloseCurrentSession)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_OpenSession
//
DECLARE_CLASS_DeviseCommand_(JAVAC_OpenSession)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_MouseAction_Click
//
DECLARE_CLASS_DeviseCommand_(JAVAC_MouseAction_Click)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_ShowRecords
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ShowRecords)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_MouseAction_RubberBand
//
DECLARE_CLASS_DeviseCommand_(JAVAC_MouseAction_RubberBand)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_SetDisplaySize
//
DECLARE_CLASS_DeviseCommand_(JAVAC_SetDisplaySize)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_KeyAction
//
DECLARE_CLASS_DeviseCommand_(JAVAC_KeyAction)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_SaveTmpSession
//
DECLARE_CLASS_DeviseCommand_(JAVAC_SaveTmpSession)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_ServerExit
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ServerExit)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_ServerCloseSocket
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ServerCloseSocket)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_ImageChannel
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ImageChannel)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_CursorChanged
//
DECLARE_CLASS_DeviseCommand_(JAVAC_CursorChanged)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_ProtocolVersion
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ProtocolVersion)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_ShowRecords3D
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ShowRecords3D)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_ResetFilters)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_GetViewHelp)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_Set3DConfig)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_RefreshData)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_CreateTmpSessionDir)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_OpenTmpSession)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_DeleteTmpSession)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_SetTmpSessionDir)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_GetDeviseVersion)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(JAVAC_SetDisplayMode)
DECLARE_CLASS_END

//-------------------------------------------------------------------------
//				Commands Dedicated for JAVA Screen - End
//-------------------------------------------------------------------------


//
//Class definition: DeviseCommand_dteImportFileType
//
DECLARE_CLASS_DeviseCommand_(dteImportFileType) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteListAllIndexes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteDeleteCatalogEntry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteMaterializeCatalogEntry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteReadSQLFilter) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteShowCatalogEntry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteListCatalog) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteListQueryAttributes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteListAttributes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteDeleteIndex) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteShowIndexDesc) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteShowAttrNames) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteInsertCatalogEntry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteCheckSQLViewEntry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dteCreateIndex) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(color) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getAllViews) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(changeParam) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(createInterp) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(create) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getTDataName) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(showkgraph) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(createMappingClass) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setDefault) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setHistogram) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getHistogram) 
DECLARE_CLASS_END

//
//Class definition
//
#if defined(KSB_MAYBE_DELETE_THIS_OLD_STATS_STUFF)
DECLARE_CLASS_DeviseCommand_(setBuckRefresh) 
DECLARE_CLASS_END
#endif

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setHistViewname) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getHistViewname) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(checkGstat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getSourceName) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(isXDateType) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(isYDateType) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(mapG2TAttr) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(mapT2GAttr) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(startLayoutManager) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(date) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(printDispatcher) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(catFiles) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(exit) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(clearAll) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(sync) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(version) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(copyright) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(compDate) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(new_leaks) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(new_inuse) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getWinCount) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getStringCount) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(waitForQueries) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(serverExit) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(abortQuery) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(importFileType) 
	public:
    	int Run_2(int argc, char** argv);
    	int Run_4(int argc, char** argv);
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(resetLinkMaster) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(get3DLocation) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkMaster) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkType) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setBatchMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(invalidateTData) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(invalidatePixmap) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(readLine) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(close) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(isMapped) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLabel) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(tdataFileName) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewWin) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(clearViewHistory) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCursorViews) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getMappingTData) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(destroy) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(parseDateFloat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(isInterpretedGData) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(isInterpreted) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getPixelWidth) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getTopGroups) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getWindowLayout) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getSchema) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getAction) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkFlag) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(changeableParam) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getInstParam) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(tcheckpoint) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(get) 
	public:
		int	Run_2(int argc, char** argv);
		int	Run_3(int argc, char** argv);
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(changeMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(exists) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(removeView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewMappings) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(refreshView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getWinGeometry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getWinViews) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkViews) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCurVisualFilter) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getVisualFilters) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewStatistics) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getAllStats) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getStatBuffer) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewDimensions) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewSolid3D) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewXYZoom) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewDisplayDataValues) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewPileMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(raiseView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(lowerView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getFileHeader) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(winGetPrint) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetHome) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetHorPan) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCursorGrid) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(writeDesc) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveStringSpace) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(loadStringSpace) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dumpLinkCursor) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(openSession) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(createTData) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewGDS) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(testDataSock) 
DECLARE_CLASS_END

#if 0
//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetAlign) 
DECLARE_CLASS_END
#endif

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLinkMaster) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLinkType) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setScreenSize) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(writeLine) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(open) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewStatistics) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewDimensions) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewSolid3D) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewXYZoom) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewDisplayDataValues) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewPileMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(savePixmap) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(loadPixmap) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getAxisDisplay) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCursorSrc) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCursorDst) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setPixelWidth) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setAction) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLinkFlag) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(highlightView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getparam) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(insertMapping) 
	public:
    	int Run_3(int argc, char** argv);
    	int Run_4(int argc, char** argv);
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getMappingLegend) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(insertLink) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewInLink) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(unlinkView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(insertWindow) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(removeMapping) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveDisplayImage) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveDisplayView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveTdata) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getDisplayImage) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getDisplayImageAndSize) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getFont) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetAlign) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(checkTDataForRecLink) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setMappingLegend) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(markViewFilter) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getWindowImage) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getWindowImageAndSize) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(swapView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setAxisDisplay) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCreateParam) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getItems) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setWindowLayout) 
	public:
    	int Run_4(int argc, char** argv);
    	int Run_5(int argc, char** argv);
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveWindowImage) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewOverrideColor) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(parseSchema) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(winSetPrint) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLabel) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dataSegment) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetHorPan) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCursorGrid) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveSession) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setWinGeometry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setFilter) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(saveDisplayImageAndMap) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(insertViewHistory) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setFont) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(set3DLocation) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewGDS) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetHome) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetImplicitHome) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetImplicitHome) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(playLog) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(test) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkMasterAttr) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkSlaveAttr) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLinkMasterAttr) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLinkSlaveAttr) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(selectView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setShowViewNames) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getShowViewNames) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCountMapping) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCountMapping) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCursorType) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGoHome) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(writeRangeDesc) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetStringTable) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetStringTable) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetIsHighlight) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetIsHighlight) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getXAxisDateFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getYAxisDateFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setXAxisDateFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setYAxisDateFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(updateAxisTypes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewGeometry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewGeometry) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setPileStackState) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getPileStackState) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(flipPileStack) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(groupUngroupViews) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(updateSession) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewAutoFilter) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewAutoFilter) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(updateFilters) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(parseDRSchema) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getDupElim) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setDupElim) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(niceifyAxes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getNiceAxes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setNiceAxes) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(switchTData) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCursorFixedSize) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCursorFixedSize) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetVerPan) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetVerPan) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetJSSendP) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetJSSendP) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetDisabledActions) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewSetDisabledActions) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(raiseAllWindows) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(nextViewInPile) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setAxisTicks) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getAxisTicks) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(dispatcherRun1)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(removeViewFromPile)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setOpeningSession)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(enableDrawing)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(resetAllFilters)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getViewHelp)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setViewHelp)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setSessionDesc)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getSessionDesc)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(printInstances)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setShowMouseLocation)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getShowMouseLocation)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getXAxisFloatFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getYAxisFloatFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setXAxisFloatFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setYAxisFloatFormat) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(writeMetaVisDesc) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCursorConstraints) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCursorConstraints) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getCursorFlag) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setCursorFlag) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(stop) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(back) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(forward) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getJS3dValid) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getJS3dConfig) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setJS3dConfig) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setAxisNegLabel) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setAxisMultFact) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getAxisMultFact) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setColorMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getColorMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(sessionIsDirty) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setDoHomeOnVisLink) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getDoHomeOnVisLink) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(cursorHome)
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setLinkMultFact) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getLinkMultFact) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setGAttrLinkMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getGAttrLinkMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(testExit) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(keyToView) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setDisplayMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getDisplayMode) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(setMappingColors) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(getMappingColors) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(selectNextInPile) 
DECLARE_CLASS_END

#endif // _DeviseCommand_h_
