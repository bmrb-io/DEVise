#ifndef DEVISE_COMMAND_H
#define DEVISE_COMMAND_H
#include <iostream.h>
#include "DeviseCommandOption.h"
#include "ClassDir.h"
#include "Control.h"
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
	private:
		DeviseCommandOption	_cmdOption;
		static	ControlPanel* defaultControl;
		static void setDefaultControl(ControlPanel* defaultControl);

	protected:
		// only friend class can construct this class
		ControlPanel* control;
		DeviseCommand(DeviseCommandOption& cmdOption)
		{
			_cmdOption = cmdOption;
		}
		virtual ~DeviseCommand(){}
		virtual int Run(int argc, char** argv);
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

/*
class DeviseCommand_dteImportFileType: public DeviseCommand
{
    public:
    DeviseCommand_dteImportFileType(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteImportFileType(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteListAllIndexes: public DeviseCommand
{
    public:
    DeviseCommand_dteListAllIndexes(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteListAllIndexes(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteDeleteCatalogEntry: public DeviseCommand
{
    public:
    DeviseCommand_dteDeleteCatalogEntry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteDeleteCatalogEntry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteMaterializeCatalogEntry: public DeviseCommand
{
    public:
    DeviseCommand_dteMaterializeCatalogEntry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteMaterializeCatalogEntry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteReadSQLFilter: public DeviseCommand
{
    public:
    DeviseCommand_dteReadSQLFilter(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteReadSQLFilter(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteShowCatalogEntry: public DeviseCommand
{
    public:
    DeviseCommand_dteShowCatalogEntry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteShowCatalogEntry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteListCatalog: public DeviseCommand
{
    public:
    DeviseCommand_dteListCatalog(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteListCatalog(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteListQueryAttributes: public DeviseCommand
{
    public:
    DeviseCommand_dteListQueryAttributes(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteListQueryAttributes(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteListAttributes: public DeviseCommand
{
    public:
    DeviseCommand_dteListAttributes(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteListAttributes(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteDeleteIndex: public DeviseCommand
{
    public:
    DeviseCommand_dteDeleteIndex(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteDeleteIndex(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteShowIndexDesc: public DeviseCommand
{
    public:
    DeviseCommand_dteShowIndexDesc(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteShowIndexDesc(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteShowAttrNames: public DeviseCommand
{
    public:
    DeviseCommand_dteShowAttrNames(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteShowAttrNames(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteInsertCatalogEntry: public DeviseCommand
{
    public:
    DeviseCommand_dteInsertCatalogEntry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteInsertCatalogEntry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteCheckSQLViewEntry: public DeviseCommand
{
    public:
    DeviseCommand_dteCheckSQLViewEntry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteCheckSQLViewEntry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dteCreateIndex: public DeviseCommand
{
    public:
    DeviseCommand_dteCreateIndex(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dteCreateIndex(){}
    int Run(int argc, char** argv);
};


//
// CLIENT command objects
//
class DeviseCommand_color: public DeviseCommand
{
    public:
    DeviseCommand_color(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_color(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getAllViews: public DeviseCommand
{
    public:
    DeviseCommand_getAllViews(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getAllViews(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_changeParam: public DeviseCommand
{
    public:
    DeviseCommand_changeParam(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_changeParam(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_createInterp: public DeviseCommand
{
    public:
    DeviseCommand_createInterp(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_createInterp(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_create: public DeviseCommand
{
    public:
    DeviseCommand_create(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_create(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getTDataName: public DeviseCommand
{
    public:
    DeviseCommand_getTDataName(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getTDataName(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_showkgraph: public DeviseCommand
{
    public:
    DeviseCommand_showkgraph(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_showkgraph(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_createMappingClass: public DeviseCommand
{
    public:
    DeviseCommand_createMappingClass(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_createMappingClass(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setDefault: public DeviseCommand
{
    public:
    DeviseCommand_setDefault(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setDefault(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setBuckRefresh: public DeviseCommand
{
    public:
    DeviseCommand_setBuckRefresh(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setBuckRefresh(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setHistViewname: public DeviseCommand
{
    public:
    DeviseCommand_setHistViewname(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setHistViewname(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getHistViewname: public DeviseCommand
{
    public:
    DeviseCommand_getHistViewname(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getHistViewname(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_checkGstat: public DeviseCommand
{
    public:
    DeviseCommand_checkGstat(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_checkGstat(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getSourceName: public DeviseCommand
{
    public:
    DeviseCommand_getSourceName(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getSourceName(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_isXDateType: public DeviseCommand
{
    public:
    DeviseCommand_isXDateType(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_isXDateType(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_isYDateType: public DeviseCommand
{
    public:
    DeviseCommand_isYDateType(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_isYDateType(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_mapG2TAttr: public DeviseCommand
{
    public:
    DeviseCommand_mapG2TAttr(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_mapG2TAttr(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_mapT2GAttr: public DeviseCommand
{
    public:
    DeviseCommand_mapT2GAttr(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_mapT2GAttr(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewLensParams: public DeviseCommand
{
    public:
    DeviseCommand_setViewLensParams(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewLensParams(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_startLayoutManager: public DeviseCommand
{
    public:
    DeviseCommand_startLayoutManager(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_startLayoutManager(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_date: public DeviseCommand
{
    public:
    DeviseCommand_date(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_date(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_printDispatcher: public DeviseCommand
{
    public:
    DeviseCommand_printDispatcher(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_printDispatcher(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_catFiles: public DeviseCommand
{
    public:
    DeviseCommand_catFiles(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_catFiles(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_exit: public DeviseCommand
{
    public:
    DeviseCommand_exit(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_exit(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_clearAll: public DeviseCommand
{
    public:
    DeviseCommand_clearAll(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_clearAll(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_sync: public DeviseCommand
{
    public:
    DeviseCommand_sync(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_sync(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_version: public DeviseCommand
{
    public:
    DeviseCommand_version(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_version(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_copyright: public DeviseCommand
{
    public:
    DeviseCommand_copyright(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_copyright(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_compDate: public DeviseCommand
{
    public:
    DeviseCommand_compDate(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_compDate(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_new_leaks: public DeviseCommand
{
    public:
    DeviseCommand_new_leaks(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_new_leaks(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_new_inuse: public DeviseCommand
{
    public:
    DeviseCommand_new_inuse(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_new_inuse(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getWinCount: public DeviseCommand
{
    public:
    DeviseCommand_getWinCount(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getWinCount(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getStringCount: public DeviseCommand
{
    public:
    DeviseCommand_getStringCount(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getStringCount(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_waitForQueries: public DeviseCommand
{
    public:
    DeviseCommand_waitForQueries(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_waitForQueries(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_serverExit: public DeviseCommand
{
    public:
    DeviseCommand_serverExit(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_serverExit(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_abortQuery: public DeviseCommand
{
    public:
    DeviseCommand_abortQuery(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_abortQuery(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_importFileType: public DeviseCommand
{
    public:
    DeviseCommand_importFileType(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_importFileType(){}
    int Run(int argc, char** argv);
    int Run_2(int argc, char** argv);
    int Run_4(int argc, char** argv);
};

class DeviseCommand_resetLinkMaster: public DeviseCommand
{
    public:
    DeviseCommand_resetLinkMaster(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_resetLinkMaster(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_get3DLocation: public DeviseCommand
{
    public:
    DeviseCommand_get3DLocation(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_get3DLocation(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getLinkMaster: public DeviseCommand
{
    public:
    DeviseCommand_getLinkMaster(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getLinkMaster(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getLinkType: public DeviseCommand
{
    public:
    DeviseCommand_getLinkType(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getLinkType(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setBatchMode: public DeviseCommand
{
    public:
    DeviseCommand_setBatchMode(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setBatchMode(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_invalidateTData: public DeviseCommand
{
    public:
    DeviseCommand_invalidateTData(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_invalidateTData(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_invalidatePixmap: public DeviseCommand
{
    public:
    DeviseCommand_invalidatePixmap(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_invalidatePixmap(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_readLine: public DeviseCommand
{
    public:
    DeviseCommand_readLine(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_readLine(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_close: public DeviseCommand
{
    public:
    DeviseCommand_close(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_close(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_isMapped: public DeviseCommand
{
    public:
    DeviseCommand_isMapped(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_isMapped(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getLabel: public DeviseCommand
{
    public:
    DeviseCommand_getLabel(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getLabel(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_tdataFileName: public DeviseCommand
{
    public:
    DeviseCommand_tdataFileName(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_tdataFileName(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewWin: public DeviseCommand
{
    public:
    DeviseCommand_getViewWin(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewWin(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_clearViewHistory: public DeviseCommand
{
    public:
    DeviseCommand_clearViewHistory(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_clearViewHistory(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getCursorViews: public DeviseCommand
{
    public:
    DeviseCommand_getCursorViews(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getCursorViews(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getMappingTData: public DeviseCommand
{
    public:
    DeviseCommand_getMappingTData(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getMappingTData(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_destroy: public DeviseCommand
{
    public:
    DeviseCommand_destroy(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_destroy(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_parseDateFloat: public DeviseCommand
{
    public:
    DeviseCommand_parseDateFloat(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_parseDateFloat(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_isInterpretedGData: public DeviseCommand
{
    public:
    DeviseCommand_isInterpretedGData(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_isInterpretedGData(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_isInterpreted: public DeviseCommand
{
    public:
    DeviseCommand_isInterpreted(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_isInterpreted(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getPixelWidth: public DeviseCommand
{
    public:
    DeviseCommand_getPixelWidth(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getPixelWidth(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getTopGroups: public DeviseCommand
{
    public:
    DeviseCommand_getTopGroups(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getTopGroups(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getWindowLayout: public DeviseCommand
{
    public:
    DeviseCommand_getWindowLayout(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getWindowLayout(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getSchema: public DeviseCommand
{
    public:
    DeviseCommand_getSchema(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getSchema(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getAction: public DeviseCommand
{
    public:
    DeviseCommand_getAction(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getAction(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getLinkFlag: public DeviseCommand
{
    public:
    DeviseCommand_getLinkFlag(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getLinkFlag(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_changeableParam: public DeviseCommand
{
    public:
    DeviseCommand_changeableParam(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_changeableParam(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getInstParam: public DeviseCommand
{
    public:
    DeviseCommand_getInstParam(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getInstParam(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_tcheckpoint: public DeviseCommand
{
    public:
    DeviseCommand_tcheckpoint(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_tcheckpoint(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_get: public DeviseCommand
{
    public:
    DeviseCommand_get(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_get(){}
    int Run(int argc, char** argv);

	// run with two parameters
	int	Run_2(int argc, char** argv);

	// run with two parameters
	int	Run_3(int argc, char** argv);
};

class DeviseCommand_changeMode: public DeviseCommand
{
    public:
    DeviseCommand_changeMode(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_changeMode(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_exists: public DeviseCommand
{
    public:
    DeviseCommand_exists(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_exists(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_removeView: public DeviseCommand
{
    public:
    DeviseCommand_removeView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_removeView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewMappings: public DeviseCommand
{
    public:
    DeviseCommand_getViewMappings(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewMappings(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_refreshView: public DeviseCommand
{
    public:
    DeviseCommand_refreshView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_refreshView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getWinGeometry: public DeviseCommand
{
    public:
    DeviseCommand_getWinGeometry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getWinGeometry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getWinViews: public DeviseCommand
{
    public:
    DeviseCommand_getWinViews(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getWinViews(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getLinkViews: public DeviseCommand
{
    public:
    DeviseCommand_getLinkViews(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getLinkViews(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getCurVisualFilter: public DeviseCommand
{
    public:
    DeviseCommand_getCurVisualFilter(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getCurVisualFilter(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getVisualFilters: public DeviseCommand
{
    public:
    DeviseCommand_getVisualFilters(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getVisualFilters(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewStatistics: public DeviseCommand
{
    public:
    DeviseCommand_getViewStatistics(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewStatistics(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getAllStats: public DeviseCommand
{
    public:
    DeviseCommand_getAllStats(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getAllStats(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getStatBuffer: public DeviseCommand
{
    public:
    DeviseCommand_getStatBuffer(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getStatBuffer(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewDimensions: public DeviseCommand
{
    public:
    DeviseCommand_getViewDimensions(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewDimensions(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewSolid3D: public DeviseCommand
{
    public:
    DeviseCommand_getViewSolid3D(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewSolid3D(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewXYZoom: public DeviseCommand
{
    public:
    DeviseCommand_getViewXYZoom(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewXYZoom(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewDisplayDataValues: public DeviseCommand
{
    public:
    DeviseCommand_getViewDisplayDataValues(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewDisplayDataValues(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewPileMode: public DeviseCommand
{
    public:
    DeviseCommand_getViewPileMode(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewPileMode(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_raiseView: public DeviseCommand
{
    public:
    DeviseCommand_raiseView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_raiseView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_lowerView: public DeviseCommand
{
    public:
    DeviseCommand_lowerView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_lowerView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getFileHeader: public DeviseCommand
{
    public:
    DeviseCommand_getFileHeader(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getFileHeader(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewLensParams: public DeviseCommand
{
    public:
    DeviseCommand_getViewLensParams(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewLensParams(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_winGetPrint: public DeviseCommand
{
    public:
    DeviseCommand_winGetPrint(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_winGetPrint(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewGetHome: public DeviseCommand
{
    public:
    DeviseCommand_viewGetHome(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewGetHome(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewGetHorPan: public DeviseCommand
{
    public:
    DeviseCommand_viewGetHorPan(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewGetHorPan(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getCursorGrid: public DeviseCommand
{
    public:
    DeviseCommand_getCursorGrid(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getCursorGrid(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_writeDesc: public DeviseCommand
{
    public:
    DeviseCommand_writeDesc(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_writeDesc(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_saveStringSpace: public DeviseCommand
{
    public:
    DeviseCommand_saveStringSpace(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveStringSpace(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_loadStringSpace: public DeviseCommand
{
    public:
    DeviseCommand_loadStringSpace(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_loadStringSpace(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dumpLinkCursor: public DeviseCommand
{
    public:
    DeviseCommand_dumpLinkCursor(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dumpLinkCursor(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_openSession: public DeviseCommand
{
    public:
    DeviseCommand_openSession(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_openSession(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_createTData: public DeviseCommand
{
    public:
    DeviseCommand_createTData(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_createTData(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getViewGDS: public DeviseCommand
{
    public:
    DeviseCommand_getViewGDS(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getViewGDS(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_testDataSock: public DeviseCommand
{
    public:
    DeviseCommand_testDataSock(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_testDataSock(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewGetAlign: public DeviseCommand
{
    public:
    DeviseCommand_viewGetAlign(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewGetAlign(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setLinkMaster: public DeviseCommand
{
    public:
    DeviseCommand_setLinkMaster(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setLinkMaster(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setLinkType: public DeviseCommand
{
    public:
    DeviseCommand_setLinkType(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setLinkType(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setScreenSize: public DeviseCommand
{
    public:
    DeviseCommand_setScreenSize(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setScreenSize(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_writeLine: public DeviseCommand
{
    public:
    DeviseCommand_writeLine(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_writeLine(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_open: public DeviseCommand
{
    public:
    DeviseCommand_open(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_open(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewStatistics: public DeviseCommand
{
    public:
    DeviseCommand_setViewStatistics(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewStatistics(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewDimensions: public DeviseCommand
{
    public:
    DeviseCommand_setViewDimensions(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewDimensions(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewSolid3D: public DeviseCommand
{
    public:
    DeviseCommand_setViewSolid3D(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewSolid3D(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewXYZoom: public DeviseCommand
{
    public:
    DeviseCommand_setViewXYZoom(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewXYZoom(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewDisplayDataValues: public DeviseCommand
{
    public:
    DeviseCommand_setViewDisplayDataValues(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewDisplayDataValues(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewPileMode: public DeviseCommand
{
    public:
    DeviseCommand_setViewPileMode(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewPileMode(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_savePixmap: public DeviseCommand
{
    public:
    DeviseCommand_savePixmap(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_savePixmap(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_loadPixmap: public DeviseCommand
{
    public:
    DeviseCommand_loadPixmap(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_loadPixmap(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getAxisDisplay: public DeviseCommand
{
    public:
    DeviseCommand_getAxisDisplay(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getAxisDisplay(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_replaceView: public DeviseCommand
{
    public:
    DeviseCommand_replaceView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_replaceView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setCursorSrc: public DeviseCommand
{
    public:
    DeviseCommand_setCursorSrc(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setCursorSrc(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setCursorDst: public DeviseCommand
{
    public:
    DeviseCommand_setCursorDst(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setCursorDst(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setPixelWidth: public DeviseCommand
{
    public:
    DeviseCommand_setPixelWidth(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setPixelWidth(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getAxis: public DeviseCommand
{
    public:
    DeviseCommand_getAxis(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getAxis(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setAction: public DeviseCommand
{
    public:
    DeviseCommand_setAction(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setAction(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setLinkFlag: public DeviseCommand
{
    public:
    DeviseCommand_setLinkFlag(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setLinkFlag(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_highlightView: public DeviseCommand
{
    public:
    DeviseCommand_highlightView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_highlightView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getparam: public DeviseCommand
{
    public:
    DeviseCommand_getparam(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getparam(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_insertViewInLens: public DeviseCommand
{
    public:
    DeviseCommand_insertViewInLens(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_insertViewInLens(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_insertMapping: public DeviseCommand
{
    public:
    DeviseCommand_insertMapping(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_insertMapping(){}
    int Run(int argc, char** argv);
    int Run_3(int argc, char** argv);
    int Run_4(int argc, char** argv);
};

class DeviseCommand_getMappingLegend: public DeviseCommand
{
    public:
    DeviseCommand_getMappingLegend(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getMappingLegend(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_insertLink: public DeviseCommand
{
    public:
    DeviseCommand_insertLink(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_insertLink(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewInLink: public DeviseCommand
{
    public:
    DeviseCommand_viewInLink(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewInLink(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_unlinkView: public DeviseCommand
{
    public:
    DeviseCommand_unlinkView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_unlinkView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_insertWindow: public DeviseCommand
{
    public:
    DeviseCommand_insertWindow(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_insertWindow(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_removeMapping: public DeviseCommand
{
    public:
    DeviseCommand_removeMapping(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_removeMapping(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_saveDisplayImage: public DeviseCommand
{
    public:
    DeviseCommand_saveDisplayImage(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveDisplayImage(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_saveDisplayView: public DeviseCommand
{
    public:
    DeviseCommand_saveDisplayView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveDisplayView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_saveTdata: public DeviseCommand
{
    public:
    DeviseCommand_saveTdata(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveTdata(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getDisplayImage: public DeviseCommand
{
    public:
    DeviseCommand_getDisplayImage(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getDisplayImage(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getDisplayImageAndSize: public DeviseCommand
{
    public:
    DeviseCommand_getDisplayImageAndSize(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getDisplayImageAndSize(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getFont: public DeviseCommand
{
    public:
    DeviseCommand_getFont(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getFont(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewSetAlign: public DeviseCommand
{
    public:
    DeviseCommand_viewSetAlign(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewSetAlign(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_checkTDataForRecLink: public DeviseCommand
{
    public:
    DeviseCommand_checkTDataForRecLink(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_checkTDataForRecLink(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setMappingLegend: public DeviseCommand
{
    public:
    DeviseCommand_setMappingLegend(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setMappingLegend(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_markViewFilter: public DeviseCommand
{
    public:
    DeviseCommand_markViewFilter(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_markViewFilter(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setAxis: public DeviseCommand
{
    public:
    DeviseCommand_setAxis(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setAxis(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getWindowImage: public DeviseCommand
{
    public:
    DeviseCommand_getWindowImage(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getWindowImage(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getWindowImageAndSize: public DeviseCommand
{
    public:
    DeviseCommand_getWindowImageAndSize(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getWindowImageAndSize(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_swapView: public DeviseCommand
{
    public:
    DeviseCommand_swapView(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_swapView(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setAxisDisplay: public DeviseCommand
{
    public:
    DeviseCommand_setAxisDisplay(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setAxisDisplay(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getCreateParam: public DeviseCommand
{
    public:
    DeviseCommand_getCreateParam(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getCreateParam(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_getItems: public DeviseCommand
{
    public:
    DeviseCommand_getItems(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_getItems(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setWindowLayout: public DeviseCommand
{
    public:
    DeviseCommand_setWindowLayout(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setWindowLayout(){}
    int Run(int argc, char** argv);
    int Run_4(int argc, char** argv);
    int Run_5(int argc, char** argv);
};

class DeviseCommand_saveWindowImage: public DeviseCommand
{
    public:
    DeviseCommand_saveWindowImage(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveWindowImage(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewOverrideColor: public DeviseCommand
{
    public:
    DeviseCommand_setViewOverrideColor(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewOverrideColor(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_parseSchema: public DeviseCommand
{
    public:
    DeviseCommand_parseSchema(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_parseSchema(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_winSetPrint: public DeviseCommand
{
    public:
    DeviseCommand_winSetPrint(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_winSetPrint(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setLabel: public DeviseCommand
{
    public:
    DeviseCommand_setLabel(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setLabel(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_dataSegment: public DeviseCommand
{
    public:
    DeviseCommand_dataSegment(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_dataSegment(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewSetHorPan: public DeviseCommand
{
    public:
    DeviseCommand_viewSetHorPan(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewSetHorPan(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setCursorGrid: public DeviseCommand
{
    public:
    DeviseCommand_setCursorGrid(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setCursorGrid(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_saveSession: public DeviseCommand
{
    public:
    DeviseCommand_saveSession(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveSession(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setWinGeometry: public DeviseCommand
{
    public:
    DeviseCommand_setWinGeometry(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setWinGeometry(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setFilter: public DeviseCommand
{
    public:
    DeviseCommand_setFilter(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setFilter(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_saveDisplayImageAndMap: public DeviseCommand
{
    public:
    DeviseCommand_saveDisplayImageAndMap(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_saveDisplayImageAndMap(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_insertViewHistory: public DeviseCommand
{
    public:
    DeviseCommand_insertViewHistory(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_insertViewHistory(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setFont: public DeviseCommand
{
    public:
    DeviseCommand_setFont(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setFont(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_set3DLocation: public DeviseCommand
{
    public:
    DeviseCommand_set3DLocation(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_set3DLocation(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_setViewGDS: public DeviseCommand
{
    public:
    DeviseCommand_setViewGDS(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_setViewGDS(){}
    int Run(int argc, char** argv);
};

class DeviseCommand_viewSetHome: public DeviseCommand
{
    public:
    DeviseCommand_viewSetHome(DeviseCommandOption& cmdOption):
        DeviseCommand(cmdOption){}
    ~DeviseCommand_viewSetHome(){}
    int Run(int argc, char** argv);
};
*/
#endif
