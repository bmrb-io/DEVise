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
#include "RecordLink.h"
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
#include "VisualLinkClassInfo.h"
#include "CursorClassInfo.h"
#include "MappingInterp.h"
#include "QueryProc.h"

#include "LMControl.h"		// LayoutManager

#include "CatalogComm.h"
#include "SessionDesc.h"
#include "StringStorage.h"
#include "DepMgr.h"
#include "Session.h"
#include "GDataSock.h"
#include "Timer.h"

#include "Color.h"
//#define INLINE_TRACE
#include "debug.h"
#include "htable.h"
#include "datum.h"
#include "CmdContainer.h"

CmdContainer*	cmdContainerp;
static const int CMD_HASHSIZE = 4;
int f(const Datum&,int);
Htable 	cmdDb(CMD_HASHSIZE, &f);

int GetDisplayImageAndSize(ControlPanel *control, int port, char *imageType);
int GetWindowImageAndSize(ControlPanel *control, int port, char *imageType,
    char *windowName);
int		ParseAPIDTE(int argc, char** argv, ControlPanel* control);
int		ParseAPIColorCommands(int argc, char** argv, ControlPanel* control);

CmdContainer::CmdContainer(ControlPanel* control, CmdContainer::Make make)
{
	DeviseCommand::setControl(control);
	this->control = control;
	this->make = make;
	cmdContainerp = this;

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
	REGISTER_COMMAND(setViewLensParams)
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
	REGISTER_COMMAND(getViewLensParams)
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
	REGISTER_COMMAND(viewGetAlign)
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
	REGISTER_COMMAND(getAxis)
	REGISTER_COMMAND(setAction)
	REGISTER_COMMAND(setLinkFlag)
	REGISTER_COMMAND(highlightView)
	REGISTER_COMMAND(getparam)
	REGISTER_COMMAND(insertViewInLens)
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
	REGISTER_COMMAND(setAxis)
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
}

CmdContainer::~CmdContainer()
{
}

int
CmdContainer::Run(int argc, char** argv)
{
	DeviseCommand*	cmd;
	int	retval;

	cmd = lookupCmd(argv[0]);
	if (cmd == NULL)
	{
		fprintf(stderr, "Unrecognized command or wrong number of args.\n");
		fprintf(stderr, "Command is: ");
		int index;
		char *prefix = "";
		for (index = 0; index < argc; index++) {
			  fprintf(stderr, "%s{%s}", prefix, argv[index]);
			  prefix = " ";
		}
		fprintf(stderr, "\n");
		control->ReturnVal(API_NAK, "Unrecognized command");
		retval = -1;
	}
	else
		retval = cmd->Run(argc, argv);
	return retval;
}

void
CmdContainer::insertCmd(char* cmdName, DeviseCommand* cmdp, int cmdsize)
{
	Datum	*key = new (Datum);
	Datum	*data = new (Datum);
	key->set(cmdName, strlen(cmdName) +1);
	data->set((char*)cmdp, cmdsize);
	cmdDb.insert(key,data);
	Datum	*datap;
	datap = cmdDb.get(*key);
	return;
}
DeviseCommand*
CmdContainer::lookupCmd(char* cmdName)
{
	DeviseCommand* 	cmdp;
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
