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
  $Id$

  $Log$
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
#include "htable.h"
#include "datum.h"
#include "CmdContainer.h"
#include "DeviseServer.h"
#include "Csprotocols.h"
#include "DeviseCommand.h"
#include "Util.h"
#include "DevError.h"
#include "DebugLog.h"

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
static const int CMD_HASHSIZE = 4;
int f(const Datum&,int);
static Htable 	cmdDb(CMD_HASHSIZE, &f);

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

CmdContainer::CmdContainer(ControlPanel* defaultControl,CmdContainer::Make make,
	DeviseServer* server)
{
#if defined(DEBUG)
    printf("CmdContainer::CmdContainer()\n");
#endif

	DeviseCommand::setDefaultControl(defaultControl);
	this->make = make;

    DOASSERT(!cmdContainerP, "Can only create one CmdContainer object!");
	cmdContainerP = this;

	_server = server;

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
	REGISTER_COMMAND(JAVAC_SaveSession)
	REGISTER_COMMAND(JAVAC_ServerExit)
	REGISTER_COMMAND(JAVAC_ServerCloseSocket)
	REGISTER_COMMAND(JAVAC_ImageChannel)
	REGISTER_COMMAND(JAVAC_CursorChanged)
	REGISTER_COMMAND(JAVAC_ProtocolVersion)
	REGISTER_COMMAND(JAVAC_ShowRecords3D)
	REGISTER_COMMAND(JAVAC_ResetFilters)
	REGISTER_COMMAND(JAVAC_GetViewHelp)

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
}

CmdContainer::~CmdContainer()
{
	// clean logfile upon completion
	//TEMP -- clean out Htable here?
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
	CmdSource::SrcType srcType =cmdDes.getCmdsource()->getSrctype();
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
				// send control commands to its client
				// which CLIENT?
				//TEMP -- remove typecast on argv
				((Server*)_server)->SendControl(API_CTL, argc, (char **)argv, true);
			}
			else
			if (cmdDes.getDest() == CmdDescriptor::FOR_SERVER)
			{
				retval = RunOneCommand(argc, argv, control);
			}
			break;

		case CmdSource::JAVACLIENT:
			// run JAVA commands with logging turned off
			retval = RunOneCommand(argc, argv, control);
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
        reportErrNosys("Unrecognized command");
		fprintf(stderr, "Command is: ");
        PrintArgs(stderr, argc, argv, true);
#if defined(DEBUG_LOG)
		DebugLog::DefaultLog()->Message(DebugLog::LevelError, "Command: ",
		  argc, argv);
#endif
		control->ReturnVal(API_NAK, "Unrecognized command");
		retval = -1;
	}
	else
	{
		//TEMP -- remove typecast on argv
		retval = cmd->Run(argc, (char **)argv, control);
	}
	return retval;
}

void
CmdContainer::insertCmd(char* cmdName, DeviseCommand* cmdp, int cmdsize)
{
	Datum	*key = new (Datum);
	Datum	*data = new (Datum);
	key->set(cmdName, strlen(cmdName) +1);
	data->set((char*)cmdp, cmdsize); // Note: this copies contents of *cmdp

	cmdDb.insert(key,data);
	Datum	*datap;
	datap = cmdDb.get(*key);
	return;
}

DeviseCommand*
CmdContainer::lookupCmd(const char* cmdName)
{
	DeviseCommand* 	cmdp = NULL;
	Datum	key(cmdName,strlen(cmdName)+1);
	Datum	*datap;

	datap = cmdDb.get(key);
	if (datap != NULL)
	{
		cmdp = (DeviseCommand*)datap->data();
	}
	return cmdp;
}

void
CmdContainer::deleteCmd(char* cmdName)
{
	DeviseCommand* 	cmdp;
	Datum	key = Datum(cmdName,strlen(cmdName)+1);
	cmdDb.del(key);
}

ostream&
operator <<(ostream&os, const CmdContainer& cc)
{
	return os;
}
