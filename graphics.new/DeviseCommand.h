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
  Declaration of command classes for all DEVise commands.
 */

/*
  $Id$

  $Log$
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
#define PURIFY 0

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
		static	ControlPanel* defaultControl;
		static void setDefaultControl(ControlPanel* defaultControl);
		ExtStack	*_controlStack;
		void pushControl(ControlPanel* control)
		{
			_controlStack->push((void*)control);
		}
		void popControl()
		{
			 _controlStack->pop();
			 control =(ControlPanel*) _controlStack->top();
		}
	protected:
		// only friend class can construct this class
		ControlPanel* control;
		DeviseCommand(DeviseCommandOption& cmdOption)
		{
			_cmdOption = cmdOption;
			_controlStack = new ExtStack(5, NULL);
		}
		virtual ~DeviseCommand()
		{
			delete _controlStack;
		}
		virtual int Run(int argc, char** argv) = 0;
	protected:
		char		result[10*1024];
		ClassDir	*classDir;
		int			numArgs;
		char**		args;
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
DECLARE_CLASS_DeviseCommand_(setViewLensParams) 
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
DECLARE_CLASS_DeviseCommand_(getViewLensParams) 
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
DECLARE_CLASS_DeviseCommand_(getAxis) 
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
DECLARE_CLASS_DeviseCommand_(insertViewInLens) 
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
DECLARE_CLASS_DeviseCommand_(setAxis) 
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
DECLARE_CLASS_DeviseCommand_(playLog) 
DECLARE_CLASS_END

//
//Class definition
//
DECLARE_CLASS_DeviseCommand_(test) 
DECLARE_CLASS_END

#endif // _DeviseCommand_h_
