/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of JavaScreenCmd and related classes (handle commands from
  JavaScreen client).
 */

/*
  $Id$

  $Log$
  Revision 1.42  1998/12/10 21:53:26  wenger
  Devised now sends GIFs to JavaScreen on a per-view rather than per-window
  basis; GIF "dirty" flags are now also referenced by view rather than by
  window.

  Revision 1.41  1998/11/24 19:31:09  wenger
  Fixed problem with soil science sessions sometimes locking up the
  JavaScreen by disallowing input from file descriptors while waiting for
  queries to finish; JavaScreen support code now omits sending windows for
  which the print exclusion flag is set (allows "control windows") to not
  be displayed in the JavaScreen.

  Revision 1.40  1998/10/02 17:21:50  wenger
  Fixed problem with GData being sent in the the wrong order for multiple
  views.

  Revision 1.39  1998/10/01 17:54:16  wenger
  Implemented the sending of GData to the JavaScreen.

  Revision 1.38  1998/09/30 18:01:55  wenger
  Oops!  Forgot one change for previous fix.

  Revision 1.37  1998/09/30 17:44:45  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.36  1998/09/25 20:53:04  wenger
  Fixed bug 390 (cursor drawn in slightly wrong location) (partly caused
  by problems in JavaScreenCmd code and partly by problems in XWindowRep).

  Revision 1.35  1998/09/22 19:13:41  wenger
  Fixed bug 393 (consumption of file descriptors in JavaScreen support
  code).

  Revision 1.34  1998/09/22 17:23:56  wenger
  Devised now returns no image data if there are any problems (as per
  request from Hongyu); added a bunch of debug and test code to try to
  diagnose bug 396 (haven't figured it out yet); made some improvements
  to the Dispatcher to make the main loop more reentrant; added some error
  reporting to the xv window grabbing code; improved command-result
  checking code.

  Revision 1.33  1998/09/15 22:29:17  wenger
  Fixed bug 392 (MolBio.tk crash in JavaScreen -- caused by cursor not
  connected to any views); workaround to bug 391 (problems with Xvfb).

  Revision 1.32  1998/09/15 17:34:06  wenger
  Changed JavaScreen communication protocol so that image data is sent
  after all associated commands (fixes bug 387); made opening a session
  and updating a session more similar to each other; fixed some other
  JavaScreen support bugs.

  Revision 1.31  1998/09/14 14:57:46  wenger
  Reorganized code somewhat for clarity (no change in functionality except
  for some better error checking).

  Revision 1.30  1998/09/10 23:21:29  wenger
  Fixed bug 388 (missing window in JavaScreen) (caused by '/' in window
  name, which was then used as part of temp file name); default for
  JavaScreen is to save selected view when saving a session.

  Revision 1.29  1998/09/08 20:52:18  wenger
  Oops!  Forgot to change SaveSession usage message.

  Revision 1.28  1998/09/08 20:26:17  wenger
  Added option to save which view is selected when saving a session -- for
  JavaScreen client switching support.

  Revision 1.27  1998/09/08 16:55:13  wenger
  Improved how JavaScreenCmd handles closing sessions -- fixes some problems
  with client switching.

  Revision 1.26  1998/09/04 17:26:49  wenger
  Added code for timing JavaScreenCmd::OpenSession() and other debug code.

  Revision 1.25  1998/09/01 21:48:27  wenger
  Another minor fix to cursor coords.

  Revision 1.24  1998/09/01 20:13:18  wenger
  Fixed problems with sometimes sending incorrect cursor coordinates to
  JavaScreen and sending DrawCursor commands before windows are created;
  removed extra braces on error messages to JavaScreen; changed from
  sending x0, y0, x1, y1 to x0, y0, width, height in JAVAC_CreateWindow
  commands.

  Revision 1.23  1998/08/28 22:01:17  wenger
  Made Dispatcher::WaitForQueries() function -- improved over earlier
  versions because it also checks the callback list (this fixes bug 367);
  fixed other piled-related JavaScreen support problems; JAVAC_OpenSession
  closes any existing session before opening the new one.

  Revision 1.22  1998/08/25 20:56:31  wenger
  Implemented support for JavaScreen cursors (not yet fully tested).

  Revision 1.21  1998/08/24 15:01:03  wenger
  Implemented support for JavaScreen drill-down.

  Revision 1.20  1998/08/17 17:12:02  wenger
  Devised now responds to KeyAction commands from JavaScreen.

  Revision 1.19  1998/08/13 18:14:45  wenger
  More updates to JavaScreen support.

  Revision 1.18  1998/08/11 18:06:36  wenger
  Switched over to simplified JavaScreen startup protocol; added isDir
  and priority args to session list.

  Revision 1.17  1998/08/11 13:43:23  wenger
  Server responds to KeyAction commands from JavaScreen (still needs event
  coordinates); did some cleanup of the ActionDefault class.

  Revision 1.16  1998/08/03 18:38:39  wenger
  Implemented JAVAC_ServerExit and JAVAC_SaveSession commands; partly
  implemented several other new commands for the JavaScreen.

  Revision 1.15  1998/07/29 14:20:26  wenger
  Mods to compile DEVise on Alpha/OSF again (partially successful); mods to
  allow static linking on Linux.

  Revision 1.14  1998/06/17 18:00:41  wenger
  MAXNAMELEN should have been MAXPATHLEN.

  Revision 1.13  1998/06/17 17:20:42  wenger
  Devised now sends "real" session file list to JavaScreen.

  Revision 1.12  1998/06/03 17:09:40  wenger
  Rubberband line in JavaScreen now sends updates of all changed windows
  using the "dirty GIF" flag; updated DEVise version to 1.5.3.

  Revision 1.11  1998/05/29 20:50:33  wenger
  Fixed sending of window locations.

  Revision 1.10  1998/05/29 19:34:39  wenger
  Added JAVAC_SetDisplaySize to allow the JavaScreen to set the display
  size.

  Revision 1.9  1998/05/29 18:12:34  wenger
  Fixed the problem with the JavaScreen only getting one window of multi-
  window sessions (JAVAC_Done isn't sent until all JAVAC_CreateWindow
  commands have been sent).

  Revision 1.8  1998/05/29 15:18:56  wenger
  Rubberband lines now work in JavaScreen, at least for single-window
  sessions.

  Revision 1.7  1998/05/26 14:09:19  wenger
  Now puts curly brackets on arguments to allow spaces in arguments;
  committed with debug code on for Hongyu.

  Revision 1.6  1998/05/21 18:18:49  wenger
  Most code for keeping track of 'dirty' GIFs in place; added 'test'
  command to be used for generic test code that needs to be controlled
  by GUI; added debug code in NetworkSend().

  Revision 1.5  1998/05/14 18:21:43  wenger
  New protocol for JavaScreen opening sessions works (sending "real" GIF)
  except for the problem of spaces in view and window names.

  Revision 1.4  1998/05/08 17:16:23  taodb
  Added stripping functions for "{}"

  Revision 1.3  1998/05/05 17:07:24  wenger
  Minor improvements to JavaScreenCmd.[Ch].

  Revision 1.2  1998/05/02 09:00:44  taodb
  Added support for JAVA Screen and command logging

  Revision 1.1  1998/04/25 05:45:23  taodb
  Initial Revision

 */
#include <sys/types.h>
#include <sys/stat.h>
#include "devise_varargs.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <dirent.h>
#include <string>
#include <vector>

#include "JavaScreenCmd.h"
#include "DeviseServer.h"
#include "CmdContainer.h"
#include "ClientAPI.h"
#include "Control.h"
#include "Util.h"
#include "Session.h"
#include "WinClassInfo.h"
#include "ViewWin.h"
#include "ViewGraph.h"
#include "Init.h"
#include "Timer.h"
#include "Display.h"
#include "ArgList.h"
#include "Cursor.h"
#include "CursorClassInfo.h"

//#define DEBUG
#define JS_TIMER 0

#define ROUND_TO_INT(value) ((int)(value + 0.5))

Boolean JavaScreenCmd::_postponeCursorCmds = false;

// Views for which GIFs may be sent (this is per-session).
static ViewWinList _gifViews;

// Views for which GData may be sent (this is per-session).
static ViewWinList _gdataViews;

// be very careful that this order agree with the ControlCmdType definition
char* JavaScreenCmd::_controlCmdName[JavaScreenCmd::CONTROLCMD_NUM]=
{
	"JAVAC_UpdateSessionList",
	"JAVAC_CreateWindow",
	"JAVAC_UpdateRecordValue",
	"JAVAC_UpdateGData",
	"JAVAC_UpdateWindow",
	"JAVAC_DrawCursor",
	"JAVAC_EraseCursor",
	"JAVAC_Done",
	"JAVAC_Error",
	"JAVAC_Fail",
	"JAVAC_UpdateImage"
};

static char *_sessionDir = NULL;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//====================================================================
// Determine whether the given point is within the given rectangle.
static Boolean
PointInRect(int px, int py, int rx1, int ry1, int rx2, int ry2)
{
    if (rx1 > rx2) {
		int tmp = rx1;
		rx1 = rx2;
		rx2 = tmp;
	}

	if (ry1 > ry2) {
		int tmp = ry1;
		ry1 = ry2;
		ry2 = tmp;
	}

	if (px < rx1) {
		return false;
	} else if (px > rx2) {
		return false;
	} else if (py < ry1) {
		return false;
	} else if (py > ry2) {
		return false;
	} else {
		return true;
	}
}

//====================================================================
static Boolean
IsSessionFile(const char *filename)
{
	Boolean isSession = false;

	// We assume that a file is a session file if it ends in ".ds" or ".tk".
	int length = strlen(filename);
	if (!strcmp(&filename[length-3], ".ds") ||
	  !strcmp(&filename[length-3], ".tk")) {
		isSession = true;
	}

	return isSession;
}

//====================================================================
static JavaWindowInfo *
CreateWinInfo(char *name, ViewWin *window, Boolean isView = false)
{
#if defined(DEBUG)
	printf("CreateWinInfo()\n");
    printf("  name: %s\n", name);
    printf("  window/view name: %s\n", window->GetName());
	printf("  is view: %d\n", isView);
	printf("  %d children\n", window->NumChildren());
#endif

	int viewCount;
	if (isView) {
	  viewCount = 1;
	} else {
	  viewCount = window->NumChildren();
	}
    JavaViewInfo **views = new JavaViewInfo*[viewCount];

	int winX, winY;
	unsigned winW, winH;
	window->RealGeometry(winX, winY, winW, winH);
#if defined(DEBUG)
    printf("window RealGeometry = %d, %d, %d, %d\n", winX, winY, winW,
		winH);
#endif

	if (isView) {
	  window->GetParent()->AbsoluteOrigin(winX, winY);
	  int tmpWinX, tmpWinY;
	  window->AbsoluteOrigin(tmpWinX, tmpWinY);
	  winX += tmpWinX;
	  winY += tmpWinY;
	} else {
	  window->AbsoluteOrigin(winX, winY);
	}
#if defined(DEBUG)
    printf("window AbsoluteOrigin = %d, %d\n", winX, winY);
#endif

	JavaRectangle winRect(winX, winY, winW, winH);
#if defined(DEBUG)
    printf("window JavaRectangle: (%g, %g), (%g, %g)\n", winRect._x0,
	    winRect._y0, winRect._width, winRect._height);
#endif

    string winName(name);
    string imageName(Init::TmpDir());
	// Remove slashes from window name, so when it's used as part of
	// a file path it won't goof things up.
	string winName2(winName);
	for (int index = 0; index < (int)winName2.length(); index++) {
		if (winName2[index] == '/') {
			winName2[index] = '_';
		}
	}
    imageName += "/" + winName2 + ".gif";
#if defined(DEBUG)
    cout << "imageName = " << imageName << endl;
#endif

	if (isView) {
	    int viewX, viewY;
	    unsigned viewW, viewH;
	    window->RealGeometry(viewX, viewY, viewW, viewH);
#if defined(DEBUG)
        printf("  view RealGeometry = %d, %d, %d, %d\n", viewX, viewY,
		    viewW, viewH);
#endif

	    JavaRectangle viewRect(viewX, viewY, viewW, viewH);
#if defined(DEBUG)
        printf("  view JavaRectangle: (%g, %g), (%g, %g)\n", viewRect._x0,
		    viewRect._y0, viewRect._width, viewRect._height);
#endif

	  string viewName(window->GetName());
#if defined(DEBUG)
		cout << "  view name: <" << viewName << ">\n";
#endif
	  views[0] = new JavaViewInfo(viewRect, viewName);
	} else {
      int viewNum = 0;
	  int viewIndex = window->InitIterator();
	  while (window->More(viewIndex))
	  {
	    ViewWin *view = window->Next(viewIndex);
#if defined(DEBUG)
        printf("  view name: <%s>\n", view->GetName());
#endif

	    int viewX, viewY;
	    unsigned viewW, viewH;
	    view->RealGeometry(viewX, viewY, viewW, viewH);
#if defined(DEBUG)
        printf("  view RealGeometry = %d, %d, %d, %d\n", viewX, viewY,
		    viewW, viewH);
#endif

	    view->GetWindowRep()->Origin(viewX, viewY);
#if defined(DEBUG)
        printf("  view WindowRep Origin = %d, %d\n", viewX, viewY);
#endif

	    JavaRectangle viewRect(viewX, viewY, viewW, viewH);
#if defined(DEBUG)
        printf("  view JavaRectangle: (%g, %g), (%g, %g)\n", viewRect._x0,
		    viewRect._y0, viewRect._width, viewRect._height);
#endif

	    string viewName(view->GetName());

	    views[viewNum] = new JavaViewInfo(viewRect, viewName);

	    viewNum++;
	  }
	  window->DoneIterator(viewIndex);
	}

    JavaWindowInfo *winInfo = new JavaWindowInfo(winRect, winName, imageName,
		viewCount, views);

	for (int index = 0; index < window->NumChildren(); index++) {
		// TEMP: Deleting this causes devised to crash; not deleting is a leak.
		// delete views[index];
	}
	delete [] views;

    return winInfo;
}

//====================================================================
static off_t
getFileSize(const char* filename)
{
	off_t filesize;
	int	fd = open(filename, O_RDONLY);
	filesize = lseek(fd, 0, SEEK_END);
	if (filesize <0)
	{
		perror("Error in getting file size:");
	}
	close(fd);
	return filesize;
}

//====================================================================
static void
FillInt(char** argv, int& pos, const int x)
{
	char buf[128];
	sprintf(buf, "%d", x);
	argv[pos++] = CopyString(buf);
}

//====================================================================
static void
FillDouble(char** argv, int& pos, const double x)
{
	char buf[128];
	sprintf(buf, "%.10g", x);
	argv[pos++] = CopyString(buf);
}

//====================================================================
static void
FillArgv(char** argv, int& pos, const JavaRectangle& jr)
{
	char	buf[128];
	sprintf(buf,"%.0f", jr._x0);
	argv[pos ++] = CopyString(buf);
	sprintf(buf,"%.0f", jr._y0);
	argv[pos ++] = CopyString(buf);
	sprintf(buf,"%.0f", jr._width);
	argv[pos ++] = CopyString(buf);
	sprintf(buf,"%.0f", jr._height);
	argv[pos ++] = CopyString(buf);
}

//====================================================================
// Set special modes, etc., needed for sessions to work in the JavaScreen.
static void
JSSessionInit()
{
	// Set batch mode so the server makes pixmaps instead of windows.
	ControlPanel::Instance()->SetBatchMode(true);

	// Tell view class not to draw cursors, but to request JavaScreen to
	// draw them.
	View::SetDrawCursors(false);
	View::SetJSCursors(true);

	// Turn off collaboration; otherwise the collaboration stuff
	// interferes with some commands from the JavaScreen.
	cmdContainerp->setMake(CmdContainer::MONOLITHIC);
}

//====================================================================
// Create lists of the views for which GIFs and GData will be sent.
static void
CreateViewLists()
{
#if defined(DEBUG)
    printf("CreateViewLists()\n");
#endif

    _gifViews.DeleteAll();
    _gdataViews.DeleteAll();

	int winIndex = DevWindow::InitIterator();
	while (DevWindow::More(winIndex))
	{
	  ClassInfo *info = DevWindow::Next(winIndex);
	  ViewWin *window = (ViewWin *)info->GetInstance();
	  if (window != NULL && !window->GetPrintExclude())
	  {
		Boolean isPiled = false;
		int viewIndex = window->InitIterator();
		while (window->More(viewIndex)) {
		  ViewGraph *view = (ViewGraph *)window->Next(viewIndex);
		  // Only send GIF for the first view of a pile.
		  if (!isPiled) {
		    _gifViews.Append(view);
			if (view->IsInPileMode()) isPiled = true;
		  }

		  // If a view sends GData, we need to send it whether or not
		  // the view is in a pile.
          if (view->GetSendToSocket()) {
		    _gdataViews.Append(view);
		  }
	    }
		window->DoneIterator(viewIndex);
	  }
	}
	DevWindow::DoneIterator(winIndex);

#if defined (DEBUG)
    printf("GIF views:\n");
	int viewIndex = _gifViews.InitIterator();
	while (_gifViews.More(viewIndex)) {
	  printf("  <%s>\n", _gifViews.Next(viewIndex)->GetName());
	}
	_gifViews.DoneIterator(viewIndex);

    printf("GData views:\n");
	viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
	  printf("  <%s>\n", _gdataViews.Next(viewIndex)->GetName());
	}
	_gdataViews.DoneIterator(viewIndex);
#endif
}

//====================================================================
// Change the files that GData is dumped to so that they're unique to
// this process, in case any other process has the same session open
// at the same time.
static void
SetGDataFiles()
{
#if defined(DEBUG)
    printf("SetGDataFiles()\n");
#endif

	int viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);

		GDataSock::Params params;
		view->GetSendParams(params);

		delete [] params.file;
		params.file = tempnam(Init::TmpDir(), NULL);
#if defined(DEBUG)
		printf("Setting GData file for view %s to %s\n",
		  view->GetName(), params.file);
#endif
		view->SetSendParams(params);
		delete [] params.file;
	}
	_gdataViews.DoneIterator(viewIndex);
}

//====================================================================
// Delete all files that GData is dumped to.
static void
DeleteGDataFiles()
{
#if defined(DEBUG)
    printf("DeleteGDataFiles()\n");
#endif

	int viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);

		GDataSock::Params params;
		view->GetSendParams(params);
		if (params.file != NULL) {
#if defined(DEBUG)
			printf("Deleting GData file: %s\n", params.file);
#endif
			(void) unlink(params.file);
		}
	}
	_gdataViews.DoneIterator(viewIndex);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//====================================================================
JavaWindowInfo::JavaWindowInfo(JavaRectangle& winRec, string& winName,
	string& imageName, int viewCount, JavaViewInfo **views)
{
	_winRec = winRec;
	_winName = winName;
	_imageName = imageName;
	_viewCount 	= viewCount;
	_viewList = new JavaViewInfo[viewCount];

	int i;
	for (i=0; i< viewCount; ++i)
	{
		_viewList[i] = *views[i];
	}
}

//====================================================================
JavaWindowInfo::~JavaWindowInfo()
{
	delete []_viewList;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//====================================================================
JavaScreenCmd::JavaScreenCmd(ControlPanel* control,
	ServiceCmdType ctype, int argc, char** argv)
{
#if defined(DEBUG)
    printf("\nJavaScreenCmd(0x%p)::JavaScreenCmd(%d)\n", this, (int)ctype);
#endif

	int	i;
	static	char	leftBrace ='{';
	static  char	rightBrace ='}';

	_control  = control;
	_ctype = ctype;
	_argc = argc;
	_argv = new (char*)[argc](NULL);
	errmsg = NULL;

    for (i=0; i< _argc; ++i)
    {
        int j = 0;
        int arglen = strlen(argv[i]);
        int startPos, endPos;
 
        startPos = -1;
        endPos = arglen;
        _argv[i] = new (char)[arglen+1](0);
        while (argv[i][j]&&(
            (argv[i][j]==' ')||
            (argv[i][j]=='\t')&&
            (argv[i][j]!= leftBrace)))
            ++j;
        if (argv[i][j]==leftBrace)
        {
            startPos = j;
            j = arglen -1;
            while ((j>0)&&(
                (argv[i][j]==' ')||
                (argv[i][j]=='\t')&&
                (argv[i][j]!= rightBrace))
            )
                --j;
            if (j > startPos)
            {
                endPos = j;
            }
            else
            {
                fprintf(stderr, " { expected\n");
                startPos = -1;
            }
        }
        strncpy(_argv[i], argv[i]+startPos +1, endPos - startPos -1);
    }
}

//====================================================================
JavaScreenCmd::~JavaScreenCmd()
{
#if defined(DEBUG)
    printf("\nJavaScreenCmd(0x%p)::~JavaScreenCmd(%d)\n", this, (int)_ctype);
#endif

	int	i;
	for (i=0; i< _argc; ++i)
		delete []_argv[i];
	delete []_argv;
}

//====================================================================
int
JavaScreenCmd::Run()
{
#if defined(DEBUG)
    printf("\nJavaScreenCmd(0x%p)::Run(%d)\n", this, _ctype);
#endif

	_status = DONE;
	if (_ctype == JAVAEXIT)
	{
		CloseJavaConnection();
		return _status;
	}
	switch (_ctype)
	{
		case CLOSECURRENTSESSION:
			CloseCurrentSession();
			break;
		case GETSESSIONLIST:
			GetSessionList();
			break;
		case OPENSESSION:
			OpenSession();
			break;
		case MOUSEACTION_CLICK:
			MouseAction_Click();
			break;
		case MOUSEACTION_DOUBLECLICK:
			MouseAction_DoubleClick();
			break;
		case MOUSEACTION_RUBBERBAND:
			MouseAction_RubberBand();
			break;
		case SETDISPLAYSIZE:
			SetDisplaySize();
			break;
		case KEYACTION:
			KeyAction();
			break;
		case SAVESESSION:
			SaveSession();
			break;
		case SERVEREXIT:
			ServerExit();
			break;
		case SERVERCLOSESOCKET:
			ServerCloseSocket();
			break;
		case IMAGECHANNEL:
			ImageChannel();
			break;
		default:
			fprintf(stderr, "Undefined JAVA Screen Command:%d\n", _ctype);
	}
	return ControlCmd(_status);
}

//====================================================================
void
JavaScreenCmd::GetSessionList()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::GetSessionList(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	if (_argc == 0) {
		// No directory name specified.
	    UpdateSessionList(NULL);
	} else if (_argc == 1) {
		// Directory name specified.
		UpdateSessionList(_argv[0]);
	} else {
		// Wrong number of arguments.
		errmsg = "Usage: GetSessionList [directory name]";
		_status = ERROR;
	}
	return;
}

//====================================================================
void
JavaScreenCmd::OpenSession()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::OpenSession(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	if (_argc != 1)
	{
		errmsg = "Usage: OpenSession <session name>";
		_status = ERROR;
		return;
	}

	if (_sessionDir == NULL) {
		_sessionDir = CopyString(getenv("DEVISE_SESSION"));
	}

	// If the given filename is a directory, send back a list of the
	// contents of that directory; otherwise, assume the file is a
	// session file and go ahead and open it.
	char fullpath[MAXPATHLEN];
	sprintf(fullpath, "%s/%s", _sessionDir, _argv[0]);

	struct stat buf;
	stat(fullpath, &buf);
	if (S_ISDIR(buf.st_mode)) {
		UpdateSessionList(_argv[0]);
	} else {
		// Reset the session directory -- this matches the behavior of the
		// "regular" DEVise, but it would probably make more sense to reset it
		// when the JavaScreen disconnects.  RKW Jun 17, 1998.
		delete [] _sessionDir;
		_sessionDir = NULL;

		printf("Session:%s requested!\n", fullpath);

		DoOpenSession(fullpath);
	}
}

//====================================================================
void
JavaScreenCmd::DoOpenSession(char *fullpath)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::DoOpenSession(%s)\n", fullpath);
#endif

#if JS_TIMER
    struct timeval startTime;
	int timeResult = gettimeofday(&startTime, NULL);
	if (timeResult < 0) {
		perror("gettimeofday");
	}
#endif

	// Close the current session, if any, to prevent possible name conflicts
	// and other problems.
	DoCloseSession();

	_postponeCursorCmds = true;

	// Set modes, etc., for session to work correctly in JavaScreen.
	JSSessionInit();

	// Open the session.
    DevStatus result = Session::Open(fullpath);
	if (!result.IsComplete())
	{
		errmsg = "Error opening session";
		_status = ERROR;
		return;
	}

	//
	// Figure out which views need to generate GIFs and which views need
	// to send GData (not disjoint).
	//
	CreateViewLists();

	//
	// Make sure that GData views are set up appropriately for the
	// JavaScreen.
	//
	SetGDataFiles();

	// Wait for all queries to finish before continuing.
#if defined(DEBUG)
	printf("Before WaitForQueries()\n");
#endif

	Dispatcher::Current()->WaitForQueries();

#if defined(DEBUG)
	printf("After WaitForQueries()\n");
#endif

	//
	// Make a JavaScreen window for each view, not each DEVise window.
	// The exception is piled views -- then there is a single JavaScreen
	// window (named the same as the first view in the pile) for the entire
	// pile).  However, all of the views in the pile must be created as
	// views so that GData can be sent if necessary.  For each JavaScreen
	// window, export a GIF from the appropriate view, in preparation for
	// sending it, and send a command requesting the creation of the window.
	//
	int winCount = View::_viewList->Size();
	char **imageNames = new char *[winCount];
	int winNum = 0;

	int winIndex = DevWindow::InitIterator();
	while (DevWindow::More(winIndex) && (_status == DONE))
	{
	  ClassInfo *info = DevWindow::Next(winIndex);
	  ViewWin *window = (ViewWin *)info->GetInstance();
	  if (window != NULL && !window->GetPrintExclude())
	  {
		int viewIndex = window->InitIterator();
		while (window->More(viewIndex)) {
			ViewGraph *view = (ViewGraph *)window->Next(viewIndex);
			JavaWindowInfo *winInfo;
			if (view->IsInPileMode()) {
				// Views in this window are in piled mode -- create one
				// JavaScreen window for the entire DEVise window.
				winInfo = CreateWinInfo(view->GetName(), window, false);
			} else {
				// This view is not piled -- create one JavaScreen window
				// corresponding to this view.
				winInfo = CreateWinInfo(view->GetName(), view, true);
			}

        	if (!view->ExportImage(GIF,
			  winInfo->_imageName.c_str()).IsComplete()) {
	        	errmsg = "Error exporting window image";
				_status = ERROR;
	    	} else {
				int filesize = getFileSize(winInfo->_imageName.c_str());
	        	if (filesize > 0) {
					_status = RequestCreateWindow(*winInfo, filesize);
				}
			}

			if (_status == DONE) {
		    	imageNames[winNum] = CopyString(winInfo->_imageName.c_str());
            	winNum++;
			}

			delete winInfo;

			// If we're in piled mode, we've already taken care of all of
			// the views in this window.
			if (view->IsInPileMode()) break;
		}
		window->DoneIterator(viewIndex);
	  }
	}
	DevWindow::DoneIterator(winIndex);
    winCount = winNum;


	//
	// Send "update GData" commands for all of the "GData views".
	//
	int viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);
		if (RequestUpdateGData(view) != DONE) {
			_status = ERROR;
		}
	}
	_gdataViews.DoneIterator(viewIndex);


	//
	// Okay, now that the JavaScreen knows about all of the windows,
	// we can go ahead and ask it to draw the cursors.
	//
	_postponeCursorCmds = false;
	if (_status == DONE) {
		DrawAllCursors();
	}

	// Send DONE here so jspop and js start reading from the image socket.
    if (_status == DONE) {
		char *argv[1];
		argv[0] = _controlCmdName[DONE];
		ReturnVal(1, argv);
	}

	// Send the window images.
    for (winNum = 0; winNum < winCount; winNum++) {
		if (_status == DONE) {
			(void) SendWindowData(imageNames[winNum]);
		}
		(void) unlink(imageNames[winNum]);
		delete [] imageNames[winNum];
	}
	delete [] imageNames;

	// Send GData, if any.
	viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);
		(void) SendViewGData(view);
	}
	_gdataViews.DoneIterator(viewIndex);

	// avoid unnecessary JAVAC_Done command, after sending back images
	if (_status == DONE) {
		_status = NULL_COMMAND;
    } else {
		// Clean up if things didn't work.
#if defined(DEBUG)
    	printf("Cleaning up partially-opened session\n");
#endif
		DoCloseSession();
	}

	Session::SetIsJsSession(true);

#if JS_TIMER
    struct timeval stopTime;
	timeResult = gettimeofday(&stopTime, NULL);
	if (timeResult < 0) {
		perror("gettimeofday");
	}
	double time1 = startTime.tv_sec + startTime.tv_usec * 1.0e-6;
	double time2 = stopTime.tv_sec + stopTime.tv_usec * 1.0e-6;
	printf("  OpenSession took %g sec.\n", time2 - time1);
#endif

#if defined(DEBUG)
    printf("End of OpenSession; _status = %d\n", _status);
#endif
}

//====================================================================
void
JavaScreenCmd::MouseAction_Click()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::MouseAction_Click(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	if (_argc != 3)
	{
		// Note: x and y are relative to GIF origin.  This used to be the
		// DEVise window, but now it's the view.
		errmsg = "Usage: MouseAction_Click <view name> <x> <y>";
		_status = ERROR;
		return;
	}

	ViewGraph *view = (ViewGraph *) ControlPanel::FindInstance(_argv[0]);
	if (view == NULL) {
		errmsg = "Can't find specified view";
		_status = ERROR;
		return;
	}

	int xLoc = atoi(_argv[1]);
	int yLoc = atoi(_argv[2]);

    view->HandlePress(view->GetWindowRep(), xLoc, yLoc, xLoc, yLoc, 1);

	// Make sure everything has actually been re-drawn before we
	// continue.
	Dispatcher::Current()->WaitForQueries();

	// Send the updated window image(s).
	_status = SendChangedWindows();
}

//====================================================================
void
JavaScreenCmd::MouseAction_DoubleClick()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::MouseAction_DoubleClick(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	// Note: x and y are relative to GIF origin.  This used to be the
	// DEVise window, but now it's the view.
	if (_argc != 3)
	{
		errmsg = "Usage: MouseAction_DoubleClick <view name> <x> <y>";
		_status = ERROR;
		return;
	}

	ViewGraph *view = (ViewGraph *) ControlPanel::FindInstance(_argv[0]);
	if (view == NULL) {
		errmsg = "Can't find specified view";
		_status = ERROR;
		return;
	}

    int xPix = atoi(_argv[1]);
    int yPix = atoi(_argv[2]);

	char **msgs;
	int msgCount;

    view->HandlePopUp(NULL, xPix, yPix, 2, msgs, msgCount);

	_status = RequestUpdateRecordValue(msgCount, msgs);
}

//====================================================================
void
JavaScreenCmd::MouseAction_RubberBand()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::MouseAction_RubberBand(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	if (_argc != 5)
	{
		// Note: 'view name' is really the JavaScreen's window name.
		errmsg = "Usage: MouseAction_RubberBand <view name>"
				 " <x1> <y1> <x2> <y2>";
		// Note: (x1, y1) is where mouse started; Y is down from the top of
		// the view/GIF.
		_status = ERROR;
		return;
	}

	int startX = atoi(_argv[1]);
	int startY = atoi(_argv[2]);
	int endX = atoi(_argv[3]);
	int endY = atoi(_argv[4]);

    ViewGraph *view = (ViewGraph *)ControlPanel::FindInstance(_argv[0]);

#if defined(DEBUG)
    printf("Rubberband from (%d, %d) to (%d, %d) in view %s\n",
	  startX, startY, endX, endY, view->GetName());
#endif

	// Update the visual filter of the view that the
	// rubberband line started in.
	VisualFilter filter;
	view->GetVisualFilter(filter);
	view->FindWorld(startX, startY, endX,
	  endY, filter.xLow, filter.yLow, filter.xHigh, filter.yHigh);
	view->SetVisualFilter(filter);

	// Make sure everything has actually been re-drawn before we
	// continue.
	Dispatcher::Current()->WaitForQueries();

	// Send the updated window image(s).
	_status = SendChangedWindows();
}

//====================================================================
void
JavaScreenCmd::CloseCurrentSession()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::CloseCurrentSession(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	if (_argc != 0) {
		errmsg = "Usage: CloseCurrentSession";
		_status = ERROR;
		return;
	}

	DeleteGDataFiles();
	DoCloseSession();

	_status = DONE;
	return;
}

//====================================================================
void
JavaScreenCmd::SetDisplaySize()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::SetDisplaySize(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	if (_argc != 2) {
		errmsg = "Usage: SetDisplaySize <width> <height>";
		_status = ERROR;
		return;
	}

    DeviseDisplay::DefaultDisplay()->DesiredScreenWidth() = atoi(_argv[0]);
    DeviseDisplay::DefaultDisplay()->DesiredScreenHeight() = atoi(_argv[1]);

	_status = DONE;
	return;
}

//====================================================================
void
JavaScreenCmd::KeyAction()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::KeyAction(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	//TEMP -- this must be changed to give key press location
	if (_argc != 2) {
		errmsg = "Usage: KeyAction <viewName> <key>";
		_status = ERROR;
		return;
	}

	ViewGraph *view = (ViewGraph *)ControlPanel::FindInstance(_argv[0]);
	if (view == NULL) {
		errmsg = "Cannot find given view";
		_status = ERROR;
	} else {
	    int key = atoi(_argv[1]);

		// TEMP -- pixels or data units?
        Coord xLoc = 0; //TEMP
        Coord yLoc = 0; //TEMP

	    view->GetAction()->KeySelected(view, key, xLoc, yLoc);

		// Make sure everything has actually been re-drawn before we
		// continue.
		Dispatcher::Current()->WaitForQueries();

		// Send the updated window image(s).
		_status = SendChangedWindows();
	}
	return;
}

//====================================================================
void
JavaScreenCmd::SaveSession()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::SaveSession(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	Boolean saveSelView;
	if (_argc == 1) {
		saveSelView = true;
	} else if (_argc == 2) {
		saveSelView = atoi(_argv[1]);
	} else {
		errmsg = "Usage: SaveSession <file name> [save selected view]";
		_status = ERROR;
		return;
	}

	if (_sessionDir == NULL) {
		_sessionDir = CopyString(getenv("DEVISE_SESSION"));
	}

	char fullpath[MAXPATHLEN];
	sprintf(fullpath, "%s/%s", _sessionDir, _argv[0]);

	if (!Session::Save(fullpath, false, false, false,
	  saveSelView).IsComplete()) {
		errmsg = "Error saving session";
		_status = ERROR;
	} else {
		_status = DONE;
	}

	return;
}

//====================================================================
void
JavaScreenCmd::ServerExit()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::ServerExit(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

	printf("Server exiting on command from JavaScreen\n");
	DoCloseSession();
	ControlPanel::Instance()->DoQuit();

	_status = DONE;
	return;
}

//====================================================================
void
JavaScreenCmd::ServerCloseSocket()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::ServerCloseSocket(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

//TEMP -- add body of function

	_status = DONE;
	return;
}

//====================================================================
void
JavaScreenCmd::ImageChannel()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::ImageChannel(");
    PrintArgs(stdout, _argc, _argv, false);
    printf(")\n");
#endif

//TEMP -- add body of function

	_status = DONE;
	return;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateRecordValue(int argc, char **argv)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::RequestUpdateRecordValue()\n");
#endif

    char **args = new char *[argc+1];
	args[0] = _controlCmdName[UPDATERECORDVALUE];
	for (int index = 0; index < argc; index++) {
		args[index+1] = argv[index];
	}
	ReturnVal(argc+1, args);

	delete [] args;

	return DONE;
}

//====================================================================
char* 
JavaScreenCmd::JavaScreenCmdName(JavaScreenCmd::ControlCmdType ctype)
{
	return JavaScreenCmd::_controlCmdName[(int)ctype];
}

//====================================================================
int
JavaScreenCmd::ControlCmd(JavaScreenCmd::ControlCmdType  status)
{
	// return either DONE/ERROR/FAIL to current JAVA client
	if (status == DONE)
	{
		ReturnVal(1,&_controlCmdName[DONE]);
		return 1;
	}
	if (status == FAIL)
	{
		// abort the client 
		ReturnVal(1, &_controlCmdName[FAIL]);
		CloseJavaConnection();
		return -1;
	}
	if (status == ERROR)
	{
		const char *argv[2] = {
			JavaScreenCmd::_controlCmdName[ERROR],
			errmsg
		};	
		if (argv[1] == NULL)
		{
			argv[1]= "Error, but no error message available";
		}
		ReturnVal(2, (char**)&argv[0]);
		return -1;
	}
	if (status != NULL_COMMAND)
	{
		fprintf(stderr, "Return value is not DONE/FAIL/ERROR\n");
		return -1;
	}
	return DONE;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::SendWindowData(const char* fileName)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::SendWindowData(%s)\n", fileName);
#endif

	ControlCmdType	status = DONE;
#if 0 // Enable this for JavaScreenCmd debugging in monolithic executable.
	return status;
#endif

	int filesize = 0;

	// send the dumped image file to the JAVA_SCREEN client
	int	fd = open(fileName, O_RDONLY);
	if (fd <0)
	{
		perror("Image file:");
		status = ERROR;
		errmsg = "Cannot open image file";
	}
	else
	{
		char buf[2048];
		int	bytesRead;
		
		while ((bytesRead = read(fd, buf, sizeof(buf))) > 0)
		{
			DeviseServer*	server;
			server = cmdContainerp->getDeviseServer();
			int bytesWritten = server->WriteImagePort(buf, bytesRead);
			if (bytesWritten < 0) {
				reportErrSys("write image port");
				status = FAIL;
				break;
			} else if (bytesWritten != bytesRead) {
				// Note: we may want to re-try the write here.  RKW 1998-09-14.
				char errBuf[2048];
				sprintf(errBuf, "Expected to write %d bytes; wrote %d",
				  bytesRead, bytesWritten);
				reportErrSys(errBuf);
				break;
			}
			filesize += bytesRead;
#if defined(DEBUG)
        printf("    %d bytes sent\n", filesize);
#endif
		}
		if (bytesRead < 0) {
			reportErrSys("Reading image file");
		}
		close(fd);
#if defined(DEBUG)
        printf("  Image file size = %d\n", filesize);
#endif
	}

#if defined(DEBUG)
    printf("  done sending window image\n");
	printf("  status = %d\n", status);
#endif

	return status;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::SendChangedWindows()
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::SendChangedWindows()\n");
#endif

    JavaScreenCmd::ControlCmdType result = DONE;

	//
	// For each "GIF view", if it's dirty, dump it to the GIF file, and send
	// the command for the JavaScreen to update the appropriate window.
	//
	char **imageNames = new char *[View::_viewList->Size()];
	int dirtyGifCount = 0;
	ViewWinList dirtyGifList;

	int viewIndex = _gifViews.InitIterator();
	while (_gifViews.More(viewIndex)) {
		ViewWin *view = _gifViews.Next(viewIndex);
        if (view->GetGifDirty()) {
#if defined(DEBUG)
	        printf("GIF of view <%s> is \"dirty\".\n", view->GetName());
#endif
		    dirtyGifList.Append(view);

    		ControlCmdType tmpResult = DONE;
			int	filesize;

			char *fileName = tempnam(Init::TmpDir(), NULL);
			if (!view->ExportImage(GIF, fileName).IsComplete()) {
				errmsg = "Error exporting window image";
				tmpResult = ERROR;
				filesize = 0;
			} else {
				filesize = getFileSize(fileName);
			}
	
#if defined(DEBUG)
            printf("  Image file size = %d\n", filesize);
#endif
			if (filesize > 0)
			{
				if (tmpResult == DONE) {
				    tmpResult = RequestUpdateWindow(view->GetName(),
					  filesize);
				}
				if (tmpResult == DONE) {
					imageNames[dirtyGifCount] = fileName;
					dirtyGifCount++;
				} else {
					(void) unlink(fileName);
					delete [] fileName;
				}
			}
			if (tmpResult != DONE) result = tmpResult;
		}
	}
	_gifViews.DoneIterator(viewIndex);

	DOASSERT(dirtyGifCount == dirtyGifList.Size(),
	  "Error in dirty window list");


	//
	// For each "GData view", if it's dirty, send the command for the
	// JavaScreen to update the appropriate view.
	//
	ViewWinList dirtyGdataList;

	viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);

		GDataSock::Params gdParams;
		view->GetSendParams(gdParams);

		// We determine whether the GData is "dirty" by testing whether the
		// GData file exists -- if the file does exist, the GData is "dirty"
		// (because the file is deleted after being sent).
		if (gdParams.file != NULL && access(gdParams.file, F_OK) == 0) {
#if defined(DEBUG)
	        printf("GData of view <%s> is \"dirty\".\n", view->GetName());
#endif
   			ControlCmdType tmpResult = DONE;

			if (RequestUpdateGData(view) == DONE) {
				dirtyGdataList.Append(view);
			} else {
				tmpResult = ERROR;
			}

			if (tmpResult != DONE) result = tmpResult;
		}
	}
	_gdataViews.DoneIterator(viewIndex);

	//
	// Send DONE here so jspop and js start reading from the image socket.
	//
    if (_status == DONE) {
		char *argv[1];
		argv[0] = _controlCmdName[DONE];
		ReturnVal(1, argv);
	}

	//
	// Send the actual images for the dirty GIF views.
	//
    for (int winNum = 0; winNum < dirtyGifCount; winNum++) {
		if (result == DONE) {
			(void) SendWindowData(imageNames[winNum]);
		}
		(void) unlink(imageNames[winNum]);
		delete [] imageNames[winNum];
	}
	delete [] imageNames;

	//
	// Send the GData for the dirty GData views.
	//
	viewIndex = dirtyGdataList.InitIterator();
	while (dirtyGdataList.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)dirtyGdataList.Next(viewIndex);
		(void) SendViewGData(view);
	}
	dirtyGdataList.DoneIterator(viewIndex);

	// avoid unnecessary JAVAC_Done command, after sending
	// back images
	if (result == DONE) {
		result = NULL_COMMAND;
	}

#if defined(DEBUG)
    printf("End of SendChangedWindows; result = %d\n", result);
#endif
	return result;
}

//====================================================================
JavaScreenCmd::ControlCmdType 
JavaScreenCmd::RequestUpdateSessionList(int argc, char** argv)
{
	ReturnVal(argc, argv);
	return DONE;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestCreateWindow(JavaWindowInfo& winInfo, int imageSize)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::RequestCreateWindow(%s, %d)\n",
	  winInfo._winName.c_str(), imageSize);
#endif

	char*	fileName = CopyString(winInfo._imageName.c_str());

	ControlCmdType	status = DONE;
	//
	// Return one command to  the JAVA Screen
	// This is Okay, since JAVA Screen will not react until 
	// it sees DONE/ERROR/FAIL
	int		argc = 
			+1 						// CommandName
			+1 						// WinName
			+4 						// Win coordinates
			+1						// Image Size
			+1						// # Views
			+winInfo._viewCount * 5;	// viewname + coordinaes

	char**	argv = new (char*)[argc](NULL);
	int		i;
	int		pos = 0;

	argv[pos++] = CopyString(_controlCmdName[CREATEWINDOW]);
	argv[pos++] = CopyString(winInfo._winName.c_str());
	FillArgv(argv, pos, winInfo._winRec);
	FillInt(argv, pos, imageSize);
	FillInt(argv, pos, winInfo._viewCount);

	for (i =0; i< winInfo._viewCount; ++i)
	{
		JavaViewInfo	*jv;
		jv = &winInfo._viewList[i];
		argv[pos++] = CopyString(jv->_viewName.c_str());
		FillArgv(argv, pos, jv->_jr);
	}
	if (pos != argc)
	{
		fprintf(stderr, "Error in preparing create window command\n");
	}

	// Send a image retrieving command to JAVA_Screen
	ReturnVal(argc, argv);

	// free all ..
	for (i=0; i< argc; ++i)
	{
		delete [] argv[i];
	}
	delete [] argv;

	delete [] fileName;

#if defined(DEBUG)
    printf("exiting JavaScreenCmd::RequestCreateWindow(); status = %d\n",
	  status);
#endif
	return status;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateWindow(char* winName, int imageSize)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::RequestUpdateWindow(%s, %d)\n", winName, imageSize);
#endif

	char* argv[3];
	int	pos = 0;
	argv[pos++] = _controlCmdName[UPDATEWINDOW];
	argv[pos++] = winName;
	FillInt(argv, pos, imageSize);

	ReturnVal(3, argv);
	delete [] argv[2];

	return DONE;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateGData(ViewGraph *view)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::RequestUpdateGData(%s)\n", view->GetName());
#endif

	//
	// Make sure the GData is dumped to a file, rather than to the data
	// socket, and get the size of the file.
	//
	GDataSock::Params gdParams;
	view->GetSendParams(gdParams);

	if (gdParams.file == NULL) {
		char errBuf[1024];
		sprintf(errBuf, "Can't update GData for view %s; must send GData "
		  "to file, not socket", view->GetName());
		reportErrNosys(errBuf);
		return ERROR;
	}

	int gdSize = getFileSize(gdParams.file);


	//
	// Figure out the multipliers and offsets that the JavaScreen will
	// use to translate data units in the GData to pixels.
	//
    double xMult, xOffset;
    double yMult, yOffset;
    
    WindowRep *winRep = view->GetWindowRep();
	Transform2D *transform = winRep->TopTransform();
	Coord a00, a01, a02, a10, a11, a12;
	transform->GetCoords(a00, a01, a02, a10, a11, a12);
	if (a01 != 0.0 || a10 != 0.0) {
		char errBuf[1024];
		sprintf(errBuf, "Transform for view %s has rotation\n",
		  view->GetName());
		reportErrNosys(errBuf);
		return ERROR;
	}

	xMult = a00;
	xOffset = a02;
	yMult = a11;
	yOffset = a12;

	// Compensate for the fact that the XWindowRep inverts the direction of
	// Y *outside* of the transform.
    unsigned int viewWidth, viewHeight;
	winRep->Dimensions(viewWidth, viewHeight);
	yMult *= -1.0;
	yOffset = viewHeight - yOffset - 1;

	//
	// Send the command...
	//
	char* argv[7];
	int	pos = 0;
	argv[pos++] = _controlCmdName[UPDATEGDATA];
	argv[pos++] = view->GetName();
	FillDouble(argv, pos, xMult);
	FillDouble(argv, pos, xOffset);
	FillDouble(argv, pos, yMult);
	FillDouble(argv, pos, yOffset);
	FillInt(argv, pos, gdSize);

	ReturnVal(7, argv);

	delete [] argv[2];
	delete [] argv[3];
	delete [] argv[4];
	delete [] argv[5];
	delete [] argv[6];

	return DONE;
}

//====================================================================
void
JavaScreenCmd::DrawCursor(View *view, DeviseCursor *cursor)
{
#if defined (DEBUG)
    printf("\nJavaScreenCmd::DrawCursor(%s, %s)\n", view->GetName(),
      cursor->GetName());
#endif

	if (_postponeCursorCmds) {
#if defined (DEBUG)
        printf("Cursor draw postponed until done opening session.\n");
#endif
		return;
	}

	//
	// Get the cursor's visual filter (the same as the source view's).
	//
	VisualFilter cursorFilter;
	{
        VisualFilter *filter;
	    cursor->GetVisualFilter(filter);
	    cursorFilter = *filter;
	}
#if defined(DEBUG)
    printf("Cursor vis. filter: (%g, %g), (%g, %g)\n", cursorFilter.xLow,
	  cursorFilter.yLow, cursorFilter.xHigh, cursorFilter.yHigh);
#endif

	//
	// Get the destination view's visual filter, and change the cursor filter
	// to fit the view filter according to whether the cursor is X, Y, or XY.
	//
	VisualFilter viewFilter;
	view->GetVisualFilter(viewFilter);

    if (!(cursorFilter.flag & VISUAL_X)) {
		cursorFilter.xLow = viewFilter.xLow;
		cursorFilter.xHigh = viewFilter.xHigh;
	} 
	if (!(cursorFilter.flag & VISUAL_Y)) {
		cursorFilter.yLow = viewFilter.yLow;
		cursorFilter.yHigh = viewFilter.yHigh;
	}

	//
	// Make sure the cursor's visual filter doesn't go outside destination
	// view's visual filter.
	//
	cursorFilter.xLow = MAX(cursorFilter.xLow, viewFilter.xLow);
	cursorFilter.yLow = MAX(cursorFilter.yLow, viewFilter.yLow);
	cursorFilter.xHigh = MIN(cursorFilter.xHigh, viewFilter.xHigh);
	cursorFilter.yHigh = MIN(cursorFilter.yHigh, viewFilter.yHigh);

#if defined(DEBUG)
    printf("Modified cursor vis. filter: (%g, %g), (%g, %g)\n",
	  cursorFilter.xLow, cursorFilter.yLow, cursorFilter.xHigh,
	  cursorFilter.yHigh);
#endif

	//
	// Transform data coordinates to pixels.
	//
	int xPixLow, yPixLow, xPixHigh, yPixHigh;
	{
		Coord xPix1, yPix1, xPix2, yPix2;
		WindowRep *winRep = view->GetWindowRep();
		winRep->Transform(cursorFilter.xLow, cursorFilter.yLow, xPix1, yPix1);
		winRep->Transform(cursorFilter.xHigh, cursorFilter.yHigh, xPix2,
	  	  yPix2);
	
		xPixLow = ROUND_TO_INT(MIN(xPix1, xPix2));
		yPixLow = ROUND_TO_INT(MIN(yPix1, yPix2));
		xPixHigh = ROUND_TO_INT(MAX(xPix1, xPix2));
		yPixHigh = ROUND_TO_INT(MAX(yPix1, yPix2));
	}

#if defined(DEBUG)
    printf("Pixels: (%d, %d), (%d, %d)\n", xPixLow, yPixLow, xPixHigh,
	  yPixHigh);
#endif

	//
	// Make sure everything is within the view.  This should be taken care
	// of by setting up the visual filters correctly, but there seem to be
	// some off-by-one problems in the view transform or maybe in the
	// truncation to integer.
	//
	xPixLow = MAX(xPixLow, 0);
	yPixLow = MAX(yPixLow, 0);
	{
	    int viewX, viewY;
	    unsigned viewW, viewH;
	    view->RealGeometry(viewX, viewY, viewW, viewH);
		xPixHigh = MIN(xPixHigh, (int)viewW-1);
		yPixHigh = MIN(yPixHigh, (int)viewH-1);
    }

#if defined(DEBUG)
    printf("Modified pixels: (%d, %d), (%d, %d)\n", xPixLow, yPixLow, xPixHigh,
	  yPixHigh);
#endif

	int xLoc = (int)MIN(xPixLow, xPixHigh);
	int yLoc = (int)MIN(yPixLow, yPixHigh);
	int width = (int)ABS(xPixHigh - xPixLow) + 1;
	int height = (int)ABS(yPixHigh - yPixLow) + 1;
#if defined(DEBUG)
    printf("x, y: (%d, %d), width, height: (%d, %d)\n", xLoc, yLoc, width,
	  height);
#endif

	//
    // Generate the command to send.
	//
	char *argv[6];
	int	pos = 0;
	argv[pos++] = _controlCmdName[DRAWCURSOR];
	argv[pos++] = view->GetName();
	FillInt(argv, pos, xLoc);
	FillInt(argv, pos, yLoc);
	FillInt(argv, pos, width);
	FillInt(argv, pos, height);

	//
    // This JavaScreenCmd object is created only to send the command.
	//
    JavaScreenCmd jsc(ControlPanel::Instance(), NULL_SVC_CMD, 0, NULL);
    jsc.ReturnVal(6, argv);

	delete [] argv[2];
	delete [] argv[3];
	delete [] argv[4];
	delete [] argv[5];
}

//====================================================================
void
JavaScreenCmd::EraseCursor(View *view, DeviseCursor *cursor)
{
#if defined (DEBUG)
    printf("JavaScreenCmd::EraseCursor(%s, %s)\n", view->GetName(),
      cursor->GetName());
#endif

	if (_postponeCursorCmds) {
#if defined (DEBUG)
        printf("Cursor erase postponed until done opening session.\n");
#endif
		return;
	}

	char* argv[2];
	argv[0] = _controlCmdName[ERASECURSOR];
	argv[1] = view->GetName();

    // This JavaScreenCmd object is created only to send the command.
    JavaScreenCmd jsc(ControlPanel::Instance(), NULL_SVC_CMD, 0, NULL);
    jsc.ReturnVal(2, argv);
}

//====================================================================
void
JavaScreenCmd::ReturnVal(int argc, char** argv)
{
#if defined(DEBUG)
    printf("JavaScreenCmd(0x%p)::ReturnVal(", this);
	PrintArgs(stdout, argc, argv, false);
	printf(")\n");
#endif

#if 0 // I don't understand what the heck all of this junk is for, and taking
	  // it out doesn't seem to change how anything works.  RKW 1998-08-28.
	static	char* buf = NULL;
	static	int bufsize = 0;
	int		eleSize = 0;

	// Append end-of-command marker
	char** nargv = new (char*)[argc](NULL);
	int	i;
	for (i=0; i< argc-1; ++i)
		nargv[i] = argv[i];

	eleSize = strlen(argv[argc-1]);

	if ((buf == NULL)||(eleSize > bufsize))
	{
		if (!buf)
			delete buf;
		buf = new char[strlen(argv[argc-1])+3];
	}

	// We can also send back multiple commands by seperating them with "\n"
	sprintf(buf,"%s",argv[argc-1]);
	nargv[argc-1]= buf;
		nargv[argc-1] = argv[argc-1];

	// send the command out
	_control->ReturnVal(API_JAVACMD, argc, nargv, true);
	delete []nargv;
#else
	// send the command out
	_control->ReturnVal(API_JAVACMD, argc, argv, true);
#endif
}

//====================================================================
void
JavaScreenCmd::CloseJavaConnection()
{
	DeviseServer*	server;
	server = cmdContainerp->getDeviseServer();
	server->CloseClient();
}

//====================================================================
void JavaScreenCmd::UpdateSessionList(char *dirName)
{
	if (_sessionDir == NULL) {
		_sessionDir = CopyString(getenv("DEVISE_SESSION"));
	}

	if (dirName != NULL) {
	    char fullpath[MAXPATHLEN];
	    sprintf(fullpath, "%s/%s", _sessionDir, dirName);
		delete [] _sessionDir;
		_sessionDir = CopyString(fullpath);
	}

	ArgList files;

	DIR *directory = opendir(_sessionDir);
	if (directory == NULL) {
		reportErrSys("Can't open session directory");
		errmsg = "Can't open session directory";
		_status = ERROR;
		return;
	} else {
		while (true) {
		    struct dirent *entry = readdir(directory);
			if (entry == NULL) break;
			if (strcmp(entry->d_name, ".") &&
			  (!strcmp(entry->d_name, "..") || entry->d_name[0] != '.')) {
				files.AddArg(entry->d_name);
			}
		}
		files.Sort();
		if (closedir(directory) < 0) {
			reportErrSys("Error closing directory");
		}
    }

	ArgList args(files.GetCount() * 3 + 1);
	args.AddArg(_controlCmdName[UPDATESESSIONLIST]);

	for (int index = 0; index < files.GetCount(); index++) {
		const char *file = files.GetArgs()[index];
		char fullpath[MAXPATHLEN];
		sprintf(fullpath, "%s/%s", _sessionDir, file);

		struct stat buf;
		stat(fullpath, &buf);
		if (S_ISDIR(buf.st_mode)) {
		    args.AddArg(file);
			args.AddArg("1"); // is a directory
		    args.AddArg("0"); // priority
		} else if (IsSessionFile(file)) {
		    args.AddArg(file);
			args.AddArg("0"); // is not a directory
		    args.AddArg("0"); // priority
		}
	}

	_status = RequestUpdateSessionList(args.GetCount(),
	  (char **)args.GetArgs());	
}

//====================================================================
void
JavaScreenCmd::DrawAllCursors()
{
#if defined(DEBUG)
	printf("JavaScreenCmd::DrawAllCursors()\n");
#endif

	int index = DevCursor::InitIterator();
	while (DevCursor::More(index)) {
		CursorClassInfo *info = DevCursor::Next(index);
		if (info != NULL) {
			DeviseCursor *cursor = (DeviseCursor *)info->GetInstance();
			if (cursor != NULL) {
				View *view = cursor->GetDst();
				if (view != NULL) {
					DrawCursor(view, cursor);
				}
			}
		}
	}
	DevCursor::DoneIterator(index);

#if defined(DEBUG)
    printf("Done with JavaScreenCmd::DrawAllCursors()\n");
#endif
}

//====================================================================
void
JavaScreenCmd::DoCloseSession()
{
    _gifViews.DeleteAll();
    _gdataViews.DeleteAll();

	Boolean savePostpone = _postponeCursorCmds;
	_postponeCursorCmds = true;

    int width = DeviseDisplay::DefaultDisplay()->DesiredScreenWidth();
    int height = DeviseDisplay::DefaultDisplay()->DesiredScreenHeight();

	Session::Close();
	Dispatcher::Current()->WaitForQueries();

    DeviseDisplay::DefaultDisplay()->DesiredScreenWidth() = width;
    DeviseDisplay::DefaultDisplay()->DesiredScreenHeight() = height;

	_postponeCursorCmds = savePostpone;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::SendViewGData(ViewGraph *view)
{
#if defined(DEBUG)
    printf("JavaScreenCmd::SendViewGData(%s)\n", view->GetName());
#endif

	ControlCmdType status = DONE;

	GDataSock::Params gdParams;
	view->GetSendParams(gdParams);

	if (gdParams.file == NULL) {
		char errBuf[1024];
		sprintf(errBuf, "Can't send GData for view %s; must send GData "
		  "to file, not socket", view->GetName());
		reportErrNosys(errBuf);
		status = ERROR;
	} else {
		status = SendWindowData(gdParams.file);

		// Delete the GData file so that we can tell when the view gets
		// redrawn.
		if (unlink(gdParams.file) != 0) {
		  reportErrSys("unlink");
		}
	}

	return status;
}
