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
  Declaration of command classes for all DEVise commands.
 */

/*
  $Id$

  $Log$
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
#include "DeviseCommandOption.h"
#include "ClassDir.h"
#include "Control.h"
#include "ExtStack.h"

#define DECLARE_CLASS_DeviseCommand_(command_name)\
class DeviseCommand_##command_name: public DeviseCommand\
{\
    public:\
    DeviseCommand_##command_name(DeviseCommandOption& cmdOption):\
        DeviseCommand(cmdOption){}\
    ~DeviseCommand_##command_name(){}\
	private: /* only base class can Run(), we need to initialize control */\
    int Run(int argc, char** argv);
#define DECLARE_CLASS_END };

class DeviseCommand
{
	friend class CmdContainer;
	friend ostream& operator <<(ostream&, DeviseCommand&);
	public:
		static ControlPanel* getDefaultControl();
	private:
		DeviseCommandOption	_cmdOption;
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
	protected:
		// only friend class can construct this class
		ControlPanel* _control;
		DeviseCommand(DeviseCommandOption& cmdOption)
		{
			_cmdOption = cmdOption;
			_controlStack = new ExtStack(5, NULL);
			_result = NULL;
		}
		virtual ~DeviseCommand()
		{
			delete _controlStack;
		}
		virtual int Run(int argc, char** argv) = 0;
		virtual int ReturnVal(u_short flag, char *result);
		virtual int ReturnVal(int argc, char **argv);
	protected:
        char		*_result;
		ClassDir	*_classDir;

		// Note: _numArgs and _args seem to be used as local variables within
		// methods, and don't seem to maintain any state over the life of
		// the DeviseCommand object.  RKW 1999-04-15.
		int			_numArgs;
		char**		_args;
	public:
		virtual int Run(int argc, char** argv, ControlPanel* cntl);
		DeviseCommandOption& 
		getOption() 
		{
			return (DeviseCommandOption&)_cmdOption;
		}
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
//Class definition: DeviseCommand_JAVAC_JAVAC_MouseAction_Click
//
DECLARE_CLASS_DeviseCommand_(JAVAC_MouseAction_Click)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_JAVAC_MouseAction_DoubleClick
//
DECLARE_CLASS_DeviseCommand_(JAVAC_MouseAction_DoubleClick)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_JAVAC_MouseAction_RubberBand
//
DECLARE_CLASS_DeviseCommand_(JAVAC_MouseAction_RubberBand)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_JAVAC_SetDisplaySize
//
DECLARE_CLASS_DeviseCommand_(JAVAC_SetDisplaySize)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_KeyAction
//
DECLARE_CLASS_DeviseCommand_(JAVAC_KeyAction)
DECLARE_CLASS_END

//
//Class definition: DeviseCommand_JAVAC_SaveSession
//
DECLARE_CLASS_DeviseCommand_(JAVAC_SaveSession)
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

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(viewGetAlign) 
DECLARE_CLASS_END

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
DECLARE_CLASS_DeviseCommand_(replaceView) 
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



#endif // _DeviseCommand_h_
