/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
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
#include "CmdLog.h"
#include "DeviseCommand.h"
#include "Util.h"

//#define DEBUG

static char* cmdLogBase ="/tmp/cmdLog.";

// Note: the object allocated by each of these macros is leaked, but you
// can't destroy them easily because a bitwise copy of the object's contents
// is done, so you'll end up destroying other things the object points to.
#define REGISTER_COMMAND(objType)\
{\
	DeviseCommandOption	cmdOption;\
	cmdOption.setCmdName(#objType);\
	DeviseCommand_##objType *obj= new (DeviseCommand_##objType)(cmdOption);\
	insertCmd(#objType,(DeviseCommand*)obj,sizeof(DeviseCommand_##objType)); \
}

#define REGISTER_COMMAND_WITH_OPTION(objType,cmdOption)\
{\
	cmdOption.setCmdName(#objType);\
	DeviseCommand_##objType *obj= new (DeviseCommand_##objType)(cmdOption);\
	insertCmd(#objType,(DeviseCommand*)obj,sizeof(DeviseCommand_##objType)); \
}

CmdContainer*	cmdContainerp;
static const int CMD_HASHSIZE = 4;
int f(const Datum&,int);
static Htable 	cmdDb(CMD_HASHSIZE, &f);

int GetDisplayImageAndSize(ControlPanel *control, int port, char *imageType);
int GetWindowImageAndSize(ControlPanel *control, int port, char *imageType,
    char *windowName);
int		ParseAPIDTE(int argc, char** argv, ControlPanel* control);
int		ParseAPIColorCommands(int argc, char** argv, ControlPanel* control);

CmdContainer::CmdContainer(ControlPanel* defaultControl,CmdContainer::Make make,
	DeviseServer* server)
{
#if defined(DEBUG)
    printf("CmdContainer::CmdContainer()\n");
#endif

	DeviseCommand::setDefaultControl(defaultControl);
	this->make = make;
	cmdContainerp = this;
	_server = server;

	char	tempFileName[128];
	sprintf(tempFileName, "%s%ld", cmdLogBase, (long)getpid());
	unlink(tempFileName);
	cmdLog = new CmdLogRecord(tempFileName);

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
	REGISTER_COMMAND(replaceView)
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
}

CmdContainer::~CmdContainer()
{
	// clean logfile upon completion
	unlink(cmdLogFname);
	free(cmdLogFname);
	delete cmdLog;
	//TEMP -- clean out Htable here?
}

long
CmdContainer::logCommand(int argc, char** argv, CmdDescriptor& cmdDes)
{
	char**	nargv= new (char*)[argc +1];
	int		i;
	long	logId;

	for (i=0 ; i< argc; ++i)
		nargv[i] = argv[i];
	nargv[argc]= (char*)cmdDes.Serialize().c_str();

	//append one command to log tail
	logId = cmdLog->logCommand(argc+1, nargv);
	delete []nargv;
	return logId;
}

/* 
// play the log within a certain range
obosolete, command container is not supposed to do this
bool
CmdContainer::playCommand(long logId1, long logId2)
{
	long	logId;
	long	savelogId;

	logId =	cmdLog->seekLog(logId1);

	if (logId != logId1)
	{
		cerr << "Specified log record:"<<logId1<<" not found"<<endl;
		return false;
	}
	else
	{
		do
		{
			// fetch a command
			vector <string> vec;
			cmdLog->getCommand(vec);
			int args = vec.size();

			// deserialize the same command descriptor
			CmdDescriptor	cmdDes(vec[args-1]);

			// add fromLog flag
			CmdSource*	cmdSrcp;
			cmdSrcp = cmdDes.getCmdsource();
			cmdSrcp->setFromLog();

			// play one command from the log
			char** argv = new (char*)[args-1];
			Run(args-1, argv, DeviseCommand::getDefaultControl(), cmdDes);
			delete []argv;

			savelogId = logId;
			logId = cmdLog->seekLog(CmdLogRecord::LOG_NEXT);
		}while ((savelogId != logId)&&
				(logId <= logId2));
	}
	return true;
}
*/


int
CmdContainer::Run(int argc, char** argv, ControlPanel* control, 
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
	if (cmdDes.getCmdsource()->isFromLog())
		srcType = CmdSource::LOGFILE;

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
				((Server*)_server)->SendControl(API_CTL, argc, argv, true);
			}
			else
			if (cmdDes.getDest() == CmdDescriptor::FOR_SERVER)
			{
				retval = RunOneCommand(argc, argv, control);
			}

			//we do not log network commands at this stage
			//logCommand(argc, argv, cmdDes);
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
			if (cid != CLIENT_INVALID)
				cInfo = _server->getClientInfo(cid);
			else
				cInfo = _server->getClientInfo(0);

			inGroup = (cInfo->gname != NULL);
			activeGroup = inGroup?(cInfo->active):false;

			if (!inGroup)
			{
				// always allowed
				retval = RunOneCommand(argc, argv, control);

				// we do not log this command
				if (strcmp(argv[0],"playLog"))
				{
					if (retval > 0)
						logCommand(argc, argv, cmdDes);
					else
						cerr <<"Commands failed, not logged"<<endl;
				}
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
				retval = _server->GroupCast(cInfo->gname, cInfo->cname,
						SS_CSS_Cmd,
						argc, argv, errmsg) ? 1 : -1;

				// run it locally
				if (retval <= 0)
					cerr <<"Error in groupcast:"<<errmsg<<endl;
				retval = RunOneCommand(argc, argv, control);
				if (retval > 0)
					logCommand(argc, argv, cmdDes);
			}
			break;
		case CmdSource::INTERNAL:
			retval = RunOneCommand(argc, argv, control);
			if (retval > 0)
				logCommand(argc, argv, cmdDes);
			else
					cerr <<"Commands failed, not logged"<<endl;
			break;
		case CmdSource::SESSION_PLAY:
		case CmdSource::SESSION_SAVE:
			retval = RunOneCommand(argc, argv, control);
			break;
		default:
			errmsg = "Illegal command source type";
			break;
	}

	// set return value
	cmdDes.setRetval(retval>0 ? true : false, errmsg);
	if (retval <= 0)
		cerr << cmdDes.getErrmsg();
	return retval;
}

int
CmdContainer::RunOneCommand(int argc, char** argv, ControlPanel* control) 
{
	int	retval;
	DeviseCommand* cmd;

	cmd = lookupCmd(argv[0]);
	if (cmd == NULL)
	{
		fprintf(stderr, "Unrecognized command (%s)\n", argv[0]);
		fprintf(stderr, "Command is: ");
        PrintArgs(stderr, argc, argv, true);
		control->ReturnVal(API_NAK, "Unrecognized command");
		retval = -1;
	}
	else
	{
		retval = cmd->Run(argc, argv, control);
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
CmdContainer::lookupCmd(char* cmdName)
{
	DeviseCommand* 	cmdp = NULL;
	Datum	key(cmdName,strlen(cmdName)+1);
	Datum	*datap;

	datap = cmdDb.get(key);
	if (datap != NULL)
	{
		DeviseCommandOption option;
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
