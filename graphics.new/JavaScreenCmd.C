/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-1999
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

  Revision 1.76  1999/09/30 15:02:05  wenger
  Changed things around so that session file paths sent by the JS always
  start from the "base" session directory -- the devised doesn't "remember"
  which subdirectory it's in.

  Revision 1.75.2.1  1999/10/01 22:17:06  wenger
  Devised now creates child views of all top-level views, not just all GIF
  views -- fixes problem when we have piled parent views containing piled
  view symbols.

  Revision 1.75  1999/09/29 00:56:01  wenger
  Improved handing of session files in JavaScreen support: better error
  checking, devised won't go up from 'base' session directory;
  more flexible debug logging method now available.

  Revision 1.74  1999/09/24 21:02:07  wenger
  Devised changes to correspond with the latest JavaScreen code:
  JAVAC_CreateView command sends more info; JS protocol version is now
  3.0; devised doesn't draw titles in JS views (JS draws the titles).

  Revision 1.73  1999/09/01 19:27:09  wenger
  Debug logging improved -- directory of log file can now be specified
  with the DEVISE_LOG_DIR environment variable (changed most startup scripts
  to put it in the DEVise tmp directory); added logging of a bunch of elapsed
  times to help figure out JavaScreen performance bottlenecks.

  Revision 1.72  1999/09/01 16:11:00  wenger
  New version of JAVAC_CreateView command (with more title info) is in place
  but currently disabled.

  Revision 1.71  1999/08/25 14:56:16  wenger
  More improvements to JavaScreen argument handling; assertion failures are
  written to debug log.

  Revision 1.70  1999/08/24 22:01:18  wenger
  JavaScreen support code deals with argument lists better (partly in
  preparation for JS-side axis drawing).

  Revision 1.69  1999/08/19 20:46:37  wenger
  Added JAVAC_ProtocolVersion command.

  Revision 1.68  1999/08/19 13:54:25  wenger
  Changes for JavaScreen support: all 15 shape attributes now sent in
  GData; added zoom in/out argument to JAVAC_MouseAction_RubberBand;
  JAVAC_MouseAction_DoubleClick changed to JAVAC_ShowRecords.

  Revision 1.67  1999/08/12 16:03:55  wenger
  Implemented "inverse" zoom -- alt-drag zooms out instead of in.

  Revision 1.66  1999/08/09 20:08:21  wenger
  Better error message if failure to open session directory; re-tries with
  "base" directory.

  Revision 1.65  1999/08/05 21:42:50  wenger
  Cursor improvements: cursors can now be dragged in "regular" DEVise;
  cursors are now drawn with a contrasting border for better visibility;
  fixed bug 468 (cursor color not working).

  Revision 1.64  1999/07/13 21:26:46  wenger
  Fixed minor problem in debug logging.

  Revision 1.63  1999/06/25 15:58:23  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

  Revision 1.62  1999/06/22 19:46:58  wenger
  Devised support for JavaScreen improvements: cursors are now drawn in any
  view of a pile; mouse actions can be disabled in views; cursor grid info
  is sent; JAVAC_CursorChanged command sends cursor name instead of view
  name; added dummy mouse grid info (not yet implemented).  Fixed some
  problems with how the devised deals with piles for the JavaScreen.

  Revision 1.61  1999/06/11 14:47:05  wenger
  Added the capability (mostly for the JavaScreen) to disable rubberband
  lines, cursor movement, drill down, and key actions in views (the code
  to send this info to the JS is still conditionaled out until the JS is
  ready for it).

  Revision 1.60  1999/06/10 19:59:21  wenger
  Devised sends axis type info to JS even if axes aren't drawn (so JS can
  display cursor position properly); added code to send cursor grid info
  and action disabling info (conditionaled out until the JS is ready for it);
  fixed bug with cursor Y grid value in cursor creation.

  Revision 1.59  1999/06/04 16:32:30  wenger
  Fixed bug 495 (problem with cursors in piled views) and bug 496 (problem
  with key presses in piled views in the JavaScreen); made other pile-
  related improvements (basically, I removed a bunch of pile-related code
  from the XWindowRep class, and implemented that functionality in the
  PileStack class).

  Revision 1.58  1999/05/17 20:55:20  wenger
  Partially-kludged fix for bug 488 (problems with cursors in piled views
  in the JavaScreen).

  Revision 1.57  1999/05/17 18:37:59  wenger
  Views now have GData sending configuration that is only employed when
  connecting to the JavaScreen (eliminates the need for the current kludgey
  setup to send GData to the JS).

  Revision 1.56  1999/05/14 20:10:03  wenger
  Fixed bugs 462 and 478 (both related to setting visual filters).

  Revision 1.55  1999/05/06 21:57:39  wenger
  New 'fixed-size-cursor' argument added to JS communication protocol.

  Revision 1.54  1999/05/04 17:17:07  wenger
  Merged js_viewsyms_br thru js_viewsyms_br_1 (code for new JavaScreen
  protocol that deals better with view symbols).

  Revision 1.53  1999/04/22 19:29:52  wenger
  Separated the configuration of explicit (user-requested) and implicit
  home actions (no GUI for configuring the implicit home); changed the
  Condor user script accordingly; modified JavaScreen support so that this
  all works for the JS.

  Revision 1.52.2.8  1999/04/13 14:34:23  wenger
  Fixed problem with view names in JavaScreen support; JAVAC_EraseCursor
  command now sends view name in addition to cursor name.

  Revision 1.52.2.7  1999/04/02 21:31:29  wenger
  Implemented changes to cursor drawing and erasing as per emails with
  Hongyu -- cursors name is now used.

  Revision 1.52.2.6  1999/03/29 17:05:16  wenger
  Fixed missing view title in JAVAC_CreateView command; added assertions to
  check argument counts in JavaScreen support.

  Revision 1.52.2.5  1999/03/17 15:16:30  wenger
  Added view Z coordinate to JAVAC_CreateView command, so the JavaScreen
  knows which views are on top of piles, etc.

  Revision 1.52.2.4  1999/03/15 22:13:31  wenger
  Fixed problems with view origin and data area origin for JavaScreen
  sessions.

  Revision 1.52.2.3  1999/02/24 21:12:00  wenger
  Set things up again for normal JavaScreen support as opposed to monolithic
  JavaScreen testing.

  Revision 1.52.2.2  1999/02/24 21:02:47  wenger
  Cursor drawing for JavaScreen is now working again (testing without the
  JavaScreen).

  Revision 1.52.2.1  1999/02/24 17:48:38  wenger
  Changes for view symbols in the JavaScreen are largely working (running
  without the JavaScreen), except for cursor drawing (code is currently
  configured to test JavaScreen support commands with monolithic executable).

  Revision 1.52  1999/02/19 18:21:37  wenger
  Fixed bug that caused windows with negative positions to goof up the
  screen-filling algorithm.

  Revision 1.51  1999/01/29 21:09:50  wenger
  Fixed bug 451 (dragging cursor in JS bypasses cursor grid).

  Revision 1.50  1999/01/29 16:41:37  wenger
  Fixed problem with source view not being redrawn correctly when cursor
  is dragged in the JavaScreen.

  Revision 1.49  1999/01/29 15:18:16  wenger
  Fixed egcs 1.1.1 fixes -- taking out array constructor arguments caused
  problems in some places.

  Revision 1.48  1999/01/18 18:25:13  beyer
  fixed compile errors for egcs v 1.1.1

  Revision 1.47  1999/01/05 15:06:56  wenger
  Got "static" (the views displayed don't change once the session is open)
  view symbols to work in the JavaScreen.

  Revision 1.46  1998/12/21 17:07:30  wenger
  Devise views now extend to the borders of the JavaScreen window in both
  X and Y directions, even if this means changing window aspect ratios.

  Revision 1.45  1998/12/21 16:33:39  wenger
  Added more support for cursors, and view axes and titles, for the JavaScreen.

  Revision 1.44  1998/12/15 21:20:59  wenger
  Devised now resizes windows to use as much of the JavaScreen real estate
  as possible.

  Revision 1.43  1998/12/11 17:21:36  wenger
  Fixed problem with size and location of Java buttons (GData offset was
  not fixed for per-view GIFs).

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
#include "PileStack.h"
#include "DebugLog.h"
#include "ElapsedTime.h"

//#define DEBUG
#define DEBUG_LOG
#define JS_TIMER 1
#define MONOLITHIC_TEST 0
#define NO_BATCH 0 // Non-zero for test only.

#define ROUND_TO_INT(value) ((int)(value + 0.5))

Boolean JavaScreenCmd::_postponeCursorCmds = false;

// Buffer for debug logging.
static char logBuf[MAXPATHLEN * 2];

// Views that are not subviews (view symbols).
static ViewWinList _topLevelViews;

// Views for which GIFs may be sent (this is per-session).
static ViewWinList _gifViews;

// Views for which GData may be sent (this is per-session).
static ViewWinList _gdataViews;

static DeviseCursorList _erasedCursors;
static DeviseCursorList _drawnCursors;

// Assume no more than 1000 views in a pile...
static const float viewZInc = 0.001;

static const int protocolMajorVersion = 3;
static const int protocolMinorVersion = 0;

// be very careful that this order agree with the ControlCmdType definition
char* JavaScreenCmd::_controlCmdName[JavaScreenCmd::CONTROLCMD_NUM]=
{
	"JAVAC_UpdateSessionList",
	"JAVAC_UpdateRecordValue",
	"JAVAC_UpdateGData",
	"JAVAC_DrawCursor",
	"JAVAC_EraseCursor",
	"JAVAC_CreateView",
	"JAVAC_DeleteView",
	"JAVAC_DeleteChildViews",
	"JAVAC_ViewDataArea",
	"JAVAC_UpdateViewImage",

	"JAVAC_Done",
	"JAVAC_Error",
	"JAVAC_Fail"
};

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
JSArgs::JSArgs(int maxArgs)
{
  _maxArgs = maxArgs;
  _argv = new (const char *)[_maxArgs];
  _dynamic = new Boolean[_maxArgs];

  for (int index = 0; index < _maxArgs; index++) {
    _dynamic[index] = false;
  }

  _argc = 0;
}

//====================================================================
JSArgs::~JSArgs()
{
  for (int index = 0; index < _argc; index++) {
    if (_dynamic[index]) {
	  delete [] _argv[index];
	}
  }
  delete [] _argv;
  delete [] _dynamic;
}

//====================================================================
void
JSArgs::FillString(const char *value)
{
  DOASSERT(_argc < _maxArgs, "Too many arguments");
  _argv[_argc++] = value;
}

//====================================================================
void
JSArgs::FillInt(int value)
{
  DOASSERT(_argc < _maxArgs, "Too many arguments");
  char buf[128];
  sprintf(buf, "%d", value);
  _argv[_argc] = CopyString(buf);
  _dynamic[_argc] = true;
  _argc++;
}

//====================================================================
void
JSArgs::FillDouble(double value)
{
  DOASSERT(_argc < _maxArgs, "Too many arguments");
  char buf[128];
  sprintf(buf, "%.10g", value);
  _argv[_argc] = CopyString(buf);
  _dynamic[_argc] = true;
  _argc++;
}

//====================================================================
// Set special modes, etc., needed for sessions to work in the JavaScreen.
static void
JSSessionInit()
{
	// Set batch mode so the server makes pixmaps instead of windows.
#if !NO_BATCH
	ControlPanel::Instance()->SetBatchMode(true);
#endif

	// Tell view class not to draw cursors, but to request JavaScreen to
	// draw them.
#if !MONOLITHIC_TEST
	View::SetDrawCursors(false);
#endif
	View::SetJSCursors(true);

	// Turn off collaboration; otherwise the collaboration stuff
	// interferes with some commands from the JavaScreen.
	CmdContainer::GetCmdContainer()->setMake(CmdContainer::MONOLITHIC);
}

//====================================================================
// Adjust the size and location of all DEVise windows to fill the virtual
// screen as much as possible while maintaining window aspect ratios.
static void
FillScreen()
{
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, "FillScreen()\n");
#endif

	//
	// Figure out how much of the virtual display is used up by the
	// DEVise windows.
	//
	int left = -1;
	int right = -1;
	int top = -1;
	int bottom = -1;

	int winIndex = DevWindow::InitIterator();
	while (DevWindow::More(winIndex)) {
	  ClassInfo *info = DevWindow::Next(winIndex);
	  ViewWin *window = (ViewWin *)info->GetInstance();
	  if (window != NULL && !window->GetPrintExclude()) {
	    int winX, winY;
	    unsigned winW, winH;
	    window->RealGeometry(winX, winY, winW, winH);
#if defined(DEBUG_LOG)
        sprintf(logBuf, "window <%s> RealGeometry = %d, %d, %d, %d\n",
		  window->GetName(), winX, winY, winW, winH);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	    window->AbsoluteOrigin(winX, winY);
#if defined(DEBUG_LOG)
        sprintf(logBuf, "window <%s> AbsoluteOrigin = %d, %d\n",
		  window->GetName(), winX, winY);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

		// Windows can have negative positions (off of virtual desktop).
		winX = ABS(winX);
		winY = ABS(winY);

        if (left < 0 || winX < left) {
		  left = winX;
		}

		if (right < 0 || winX + (int)winW > right) {
		  right = winX + winW;
		}

		if (top < 0 || winY < top) {
		  top = winY;
		}

		if (bottom < 0 || winY + (int)winH > bottom) {
		  bottom = winY + winH;
		}
	  }
	}
	DevWindow::DoneIterator(winIndex);

#if defined(DEBUG_LOG)
	sprintf(logBuf, "top = %d\nbottom = %d\nleft = %d\nright = %d\n",
	  top, bottom, left, right);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	Coord displayWidth, displayHeight;
	DeviseDisplay::DefaultDisplay()->Dimensions(displayWidth, displayHeight);

	//
	// Now figure out how to resize the windows to make maxiumum use of
	// the display area.
	//
	Coord xMult = displayWidth / (right - left + 1);
	Coord yMult = displayHeight / (bottom - top + 1);

	Coord xOffset = -left * xMult;
	Coord yOffset = -top * yMult;

	//
	// Resize each window.
	//
	winIndex = DevWindow::InitIterator();
	while (DevWindow::More(winIndex)) {
	  ClassInfo *info = DevWindow::Next(winIndex);
	  ViewWin *window = (ViewWin *)info->GetInstance();
	  if (window != NULL && !window->GetPrintExclude()) {
	    int winX, winY;
	    unsigned winW, winH;
	    window->RealGeometry(winX, winY, winW, winH);
	    window->AbsoluteOrigin(winX, winY);

		winX = (int)(winX * xMult + xOffset);
		winY = (int)(winY * yMult + yOffset);
		winW = (int)(winW * xMult);
		winH = (int)(winH * yMult);

#if defined(DEBUG_LOG)
        sprintf(logBuf, "window %s geometry changed to = %d, %d, %d, %d\n",
		  window->GetName(), winX, winY, winW, winH);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

        window->MoveResize(winX, winY, winW, winH);

	  }
	}
	DevWindow::DoneIterator(winIndex);
}

//====================================================================
// Create lists of the views for which GIFs and GData will be sent.
static void
CreateViewLists()
{
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "CreateViewLists()\n");
#endif

	_topLevelViews.DeleteAll();
    _gifViews.DeleteAll();
    _gdataViews.DeleteAll();

	float viewZ;

	int winIndex = DevWindow::InitIterator();
	while (DevWindow::More(winIndex))
	{
	  ClassInfo *info = DevWindow::Next(winIndex);
	  ViewWin *window = (ViewWin *)info->GetInstance();
	  if (window != NULL && !window->GetPrintExclude())
	  {

	    // For now, we don't include any info about which windows are on top
	    // of others.  RKW 1999-03-17.
		if (window->GetMyPileStack()->IsPiled()) {
	      viewZ = (window->NumChildren() - 1) * viewZInc;
		} else {
		  viewZ = 0.0;
		}
		// Guard against roundoff errors causing a negative Z value.
		viewZ += viewZInc / 2.0;

		Boolean isPiled = false;
		int viewIndex = window->InitIterator();
		while (window->More(viewIndex)) {
		  // A top-level view.
		  ViewGraph *view = (ViewGraph *)window->Next(viewIndex);

		  // Note that which view is on "top" of a pile is not consistent
		  // overall within DEVise.  The way we are doing this, the view
		  // that's on top in terms of the Z value is the one whose WindowRep
		  // gets drawn into, and whose image is sent.  However, in terms of
		  // how the data is drawn, that view is actually the view on the
		  // *bottom* of the pile...  RKW 1999-03-17.
		  view->SetZ(viewZ);
		  if (view->IsInPileMode()) viewZ -= viewZInc;

		  _topLevelViews.Append(view);

          //TEMP maybe check whether first view of window is first view of pile
		  // Only send GIF for the first view of a pile.
		  if (!isPiled) {
		    _gifViews.Append(view);
			if (view->IsInPileMode()) isPiled = true;
		  }
	    }
		window->DoneIterator(viewIndex);
	  }
	}
	DevWindow::DoneIterator(winIndex);

	//
	// We must consider *all* views as potential GData views, even if
	// they're not currently in a window (this fixes bug 525).
	//
	{
        int viewIndex = View::InitViewIterator();
		while (View::MoreView(viewIndex)) {
		    ViewGraph *view = (ViewGraph *)View::NextView(viewIndex);
            if (view->GetSendToSocket()) {
		      _gdataViews.Append(view);
		    }
		}
	    View::DoneViewIterator(viewIndex);
	}

#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, "Top-level views:");
	int viewIndex = _topLevelViews.InitIterator();
	while (_topLevelViews.More(viewIndex)) {
	  sprintf(logBuf, "  <%s>", _topLevelViews.Next(viewIndex)->GetName());
      DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
	}
	_topLevelViews.DoneIterator(viewIndex);

    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, "GIF views:");
	viewIndex = _gifViews.InitIterator();
	while (_gifViews.More(viewIndex)) {
	  sprintf(logBuf, "  <%s>", _gifViews.Next(viewIndex)->GetName());
      DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
	}
	_gifViews.DoneIterator(viewIndex);

    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, "GData views:");
	viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
	  sprintf(logBuf, "  <%s>", _gdataViews.Next(viewIndex)->GetName());
      DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, "SetGDataFiles()\n");
#endif

	int viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);

		GDataSock::Params params;
		view->GetSendParams(params);

		delete [] params.file;
		params.file = tempnam(Init::TmpDir(), NULL);
#if defined(DEBUG_LOG)
		sprintf(logBuf, "Setting GData file for view %s to %s\n",
		  view->GetName(), params.file);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "DeleteGDataFiles()\n");
#endif

	int viewIndex = _gdataViews.InitIterator();
	while (_gdataViews.More(viewIndex)) {
		ViewGraph *view = (ViewGraph *)_gdataViews.Next(viewIndex);

		GDataSock::Params params;
		view->GetSendParams(params);
		if (params.file != NULL) {
#if defined(DEBUG_LOG)
			sprintf(logBuf, "Deleting GData file: %s\n", params.file);
            DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
			(void) unlink(params.file);
		}
	}
	_gdataViews.DoneIterator(viewIndex);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

//====================================================================
JavaScreenCmd::JavaScreenCmd(ControlPanel* control,
	ServiceCmdType ctype, int argc, char** argv)
{
#if defined(DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd(0x%p)::JavaScreenCmd(%d)\n", this,
	  (int)ctype);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	int	i;
	static	char	leftBrace ='{';
	static  char	rightBrace ='}';

	_control  = control;
	_ctype = ctype;
	_argc = argc;
	_argv = new (char*)[argc];
	errmsg = NULL;

    for (i=0; i< _argc; ++i)
    {
        int j = 0;
        int arglen = strlen(argv[i]);
        int startPos, endPos;
 
        startPos = -1;
        endPos = arglen;
        _argv[i] = new (char)[arglen+1];
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
                fprintf(stderr, " { expected\n"); // } balance braces
                startPos = -1;
            }
        }
		int copyLen = endPos - startPos - 1;
        strncpy(_argv[i], argv[i]+startPos +1, copyLen);
		_argv[i][copyLen] = '\0';
    }
}

//====================================================================
JavaScreenCmd::~JavaScreenCmd()
{
#if defined(DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd(0x%p)::~JavaScreenCmd(%d)\n", this,
	  (int)_ctype);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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
#if defined(DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd(0x%p)::Run(%d)\n", this, _ctype);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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
		case SHOW_RECORDS:
			ShowRecords();
			break;
		case SHOW_RECORDS3D:
			ShowRecords3D();
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
		case CURSORCHANGED:
			CursorChanged();
			break;
		case PROTOCOLVERSION:
			JSProtocolVersion();
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
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::GetSessionList(", _argc, _argv, ")\n");
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
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::OpenSession(", _argc, _argv, ")\n");
#endif

	if (_argc != 1)
	{
		errmsg = "Usage: OpenSession <session name>";
		_status = ERROR;
		return;
	}

	const char *basePath = getenv("DEVISE_SESSION");

	// If the given filename is a directory, send back a list of the
	// contents of that directory; otherwise, assume the file is a
	// session file and go ahead and open it.
	char fullpath[MAXPATHLEN];
	sprintf(fullpath, "%s/%s", basePath, _argv[0]);

	struct stat buf;
	if (stat(fullpath, &buf) != 0) {
		char errBuf[MAXPATHLEN + 128];
		errmsg = "Can't get status for requested file or directory";
		sprintf(errBuf, "%s <%s>", errmsg, fullpath);
	    reportErrSys(errBuf);
		_status = ERROR;
	} else {
	    if (S_ISDIR(buf.st_mode)) {
		    UpdateSessionList(_argv[0]);
	    } else {
		    printf("Session %s requested!\n", fullpath);
		    DoOpenSession(fullpath);
	    }
	}
}

//====================================================================
void
JavaScreenCmd::DoOpenSession(char *fullpath)
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::DoOpenSession(", fullpath, ")\n");
#endif

#if JS_TIMER
	ElapsedTime et;
	et.Start();
#endif

	// Close the current session, if any, to prevent possible name conflicts
	// and other problems.
	DoCloseSession();

	_postponeCursorCmds = true;

	// Set modes, etc., for session to work correctly in JavaScreen.
	JSSessionInit();

	// Open the session.
	Session::SetIsJsSession(true);
    DevStatus result = Session::Open(fullpath);
	if (!result.IsComplete())
	{
		errmsg = "Error opening session";
		_status = ERROR;
		return;
	}

	// Resize the windows to use as much of the JavaScreen real estate
	// as possible.
#if !MONOLITHIC_TEST
    FillScreen();
#endif

	// For some reason, the Condor user session doesn't work right without
	// this.  RKW 1999-04-22.
	View::RefreshAll();

	//
	// The following section of the code is a little kludgey for the
	// following reason: CreateViewLists() and SetGDataFiles() have to
	// be called *before* the queries run so that the GData files can
	// be properly set up for any views that will send GData.  However,
	// CreateViewLists() also has to be called *after* the queries have
	// been run so that view symbols can be dealt with correctly
	// Note that we'll almost certainly run into problems if we have
	// a window that has view symbols in combination with piles or
	// GData views...
	//

	// Build up the view lists -- we really just use the GData views
	// list at this point.
	CreateViewLists();
	SetGDataFiles();

	// Wait for all queries to finish before continuing.
#if JS_TIMER
	et.ReportTime("Before WaitForQueries()");
#endif

#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "Before WaitForQueries()\n");
#endif

	Dispatcher::Current()->WaitForQueries();
	_postponeCursorCmds = false;

#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "After WaitForQueries()\n");
#endif

#if JS_TIMER
	et.ReportTime("After WaitForQueries()");
#endif

	// Now re-create the view lists so that we get the correct view symbol
	// views.  Note that we'll run into problems if we have "dynamic" view
	// symbols (i.e., the user can change which views get displayed on the
	// fly).
	CreateViewLists();

	//
	// ...end of kludgey section.
	//

	// Send commands to create the top-level views.
	int viewIndex = _topLevelViews.InitIterator();
	while (_topLevelViews.More(viewIndex)) {
		View *view = (View *)_topLevelViews.Next(viewIndex);
		CreateView(view, NULL);
	}
	_topLevelViews.DoneIterator(viewIndex);

	SendChangedViews(false);

    // avoid unnecessary JAVAC_Done command, after sending back images
    if (_status == DONE) {
        _status = NULL_COMMAND;
    } else {
        // Clean up if things didn't work.
#if defined(DEBUG_LOG)
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
		  "Cleaning up partially-opened session\n");
#endif
        DoCloseSession();
    }

#if JS_TIMER
	et.ReportTime("OpenSession");
#endif

#if defined(DEBUG_LOG)
    sprintf(logBuf, "End of OpenSession; _status = %d\n", _status);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
}

//====================================================================
void
JavaScreenCmd::MouseAction_Click()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::MouseAction_Click(", _argc, _argv, ")\n");
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

	_postponeCursorCmds = true;

    view->HandlePress(view->GetWindowRep(), xLoc, yLoc, xLoc, yLoc, 1, 0);

	// Make sure everything has actually been re-drawn before we
	// continue.
	Dispatcher::Current()->WaitForQueries();
	_postponeCursorCmds = false;

	// Send the updated window image(s).
	_status = SendChangedViews(true);
}

//====================================================================
void
JavaScreenCmd::ShowRecords()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::ShowRecords(", _argc, _argv, ")\n");
#endif

	// Note: x and y are relative to GIF origin.  This used to be the
	// DEVise window, but now it's the view.
	if (_argc != 3)
	{
		errmsg = "Usage: ShowRecords <view name> <x> <y>";
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

    if (view->HandlePopUp(NULL, xPix, yPix, 2, msgs, msgCount)) {
		_status = RequestUpdateRecordValue(msgCount, msgs);
	}
}

//====================================================================
void
JavaScreenCmd::ShowRecords3D()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::ShowRecords3D(", _argc, _argv, ")\n");
#endif

	// Note: x, y, and z are the values from the relevant GData record.
	if (_argc != 3)
	{
		errmsg = "Usage: ShowRecords3D <view name> <x> <y> <z>";
		_status = ERROR;
		return;
	}
}

//====================================================================
void
JavaScreenCmd::MouseAction_RubberBand()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::MouseAction_RubberBand(", _argc, _argv, ")\n");
#endif

	if (_argc != 5 && _argc != 6)
	{
		errmsg = "Usage: MouseAction_RubberBand <view name>"
				 " <x1> <y1> <x2> <y2> [zoom out]";
		// Note: (x1, y1) is where mouse started; Y is down from the top of
		// the view/GIF.
		_status = ERROR;
		return;
	}

	int startX = atoi(_argv[1]);
	int startY = atoi(_argv[2]);
	int endX = atoi(_argv[3]);
	int endY = atoi(_argv[4]);

	int state; // controls zoom in/out
	if (_argc > 5) {
		if (atoi(_argv[5]) != 0) {
		    state = 8; // alt
		} else {
		    state = 0;
		}
	} else {
	    state = 0;
	}

    ViewGraph *view = (ViewGraph *)ControlPanel::FindInstance(_argv[0]);
	if (view == NULL) {
		errmsg = "Can't find specified view";
		_status = ERROR;
		return;
	}

#if defined(DEBUG_LOG)
    sprintf(logBuf, "Rubberband from (%d, %d) to (%d, %d) in view %s\n",
	  startX, startY, endX, endY, view->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	_postponeCursorCmds = true;

	// Update the visual filter of the view that the
	// rubberband line started in.
	int xLow = MIN(startX, endX);
	int yLow = MIN(startY, endY);
	int xHigh = MAX(startX, endX);
	int yHigh = MAX(startY, endY);
	// button = 3 for XY zoom
    view->HandlePress(NULL, xLow, yLow, xHigh, yHigh, 3, state);

	// Make sure everything has actually been re-drawn before we
	// continue.
	Dispatcher::Current()->WaitForQueries();
	_postponeCursorCmds = false;

	// Send the updated window image(s).
	_status = SendChangedViews(true);
}

//====================================================================
void
JavaScreenCmd::CloseCurrentSession()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::CloseCurrentSession(", _argc, _argv, ")\n");
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
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::SetDisplaySize(", _argc, _argv, ")\n");
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
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::KeyAction(", _argc, _argv, ")\n");
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

		_postponeCursorCmds = true;

        view->HandleKey(NULL, key, xLoc, yLoc);

		// Make sure everything has actually been re-drawn before we
		// continue.
		Dispatcher::Current()->WaitForQueries();
		_postponeCursorCmds = false;

		// Send the updated window image(s).
		_status = SendChangedViews(true);
	}
	return;
}

//====================================================================
void
JavaScreenCmd::SaveSession()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::SaveSession(", _argc, _argv, ")\n");
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

	const char *basePath = getenv("DEVISE_SESSION");

	char fullpath[MAXPATHLEN];
	sprintf(fullpath, "%s/%s", basePath, _argv[0]);

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
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::ServerExit(", _argc, _argv, ")\n");
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
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::ServerCloseSocket(", _argc, _argv, ")\n");
#endif

//TEMP -- add body of function

	_status = DONE;
	return;
}

//====================================================================
void
JavaScreenCmd::ImageChannel()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::ImageChannel(", _argc, _argv, ")\n");
#endif

//TEMP -- add body of function

	_status = DONE;
	return;
}

//====================================================================
// Sent by JavaScreen when cursor location is changed.
void
JavaScreenCmd::CursorChanged()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::CursorChanged(", _argc, _argv, ")\n");
#endif

	// Note: x and y are the location of the upper left corner of the cursor
	// relative to GIF origin (upper left).  This used to be the
	// DEVise window, but now it's the view.
	if (_argc != 5)
	{
		errmsg = "Usage: CursorChanged <cursor name> <x> <y> <width> <height>";
		_status = ERROR;
		return;
	}

	// Find the cursor object and the view that it's drawn in.
    DeviseCursor *cursor = (DeviseCursor *)ControlPanel::FindInstance(_argv[0]);
	if (cursor == NULL) {
		errmsg = "Can't find specified cursor";
		_status = ERROR;
		return;
	}

    ViewGraph *view = (ViewGraph *)cursor->GetDst();
	if (view == NULL) {
		errmsg = "Cursor has no destination view";
		_status = ERROR;
		return;
	}

	int pixX = atoi(_argv[1]);
	int pixY = atoi(_argv[2]);
	int pixWidth = atoi(_argv[3]);
	int pixHeight = atoi(_argv[4]);
#if defined(DEBUG_LOG)
	sprintf(logBuf, "x, y = %d, %d; width, height = %d, %d\n", pixX, pixY,
	  pixWidth, pixHeight);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	if (view->GetCursorMoveDisabled()) {
		printf("  Cursor moving disabled in view <%s>\n", view->GetName());
		DrawCursor(view, cursor);
	    return;
	}

	_postponeCursorCmds = true;

	//
	// Convert new cursor size and location from pixels to data units,
	// and move the cursor.
	//
	Coord dataXLow, dataYLow, dataXHigh, dataYHigh;
	view->FindWorld(pixX, pixY, pixX + pixWidth - 1, pixY + pixHeight - 1,
	  dataXLow, dataYLow, dataXHigh, dataYHigh);

	Coord centerX = (dataXLow + dataXHigh) / 2.0;
	Coord centerY = (dataYLow + dataYHigh) / 2.0;;
	Coord width = ABS(dataXHigh - dataXLow);
	Coord height = ABS(dataYHigh - dataYLow);
	cursor->MoveSource(centerX, centerY, width, height);

	// Make sure everything has actually been re-drawn before we
	// continue.
	Dispatcher::Current()->WaitForQueries();
	_postponeCursorCmds = false;

	// Send the updated window image(s).
	_status = SendChangedViews(true);

	return;
}

//====================================================================
// Sent by JavaScreen when first trying to connect.
void
JavaScreenCmd::JSProtocolVersion()
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::JSProtocolVersion(", _argc, _argv, ")\n");
#endif

	if (_argc != 1) {
		errmsg = "Usage: ProtocolVersion <major.minor>";
		_status = ERROR;
	} else {
		int jsMajor, jsMinor;
	    sscanf(_argv[0], "%d.%d", &jsMajor, &jsMinor);
        if (jsMajor != protocolMajorVersion) {
			char errBuf[1024];
			sprintf(errBuf, "Expected protocol version %d.%d; JavaScreen "
			    "has version %d.%d", protocolMajorVersion,
				protocolMinorVersion, jsMajor, jsMinor);
			reportErrNosys(errBuf);

		    errmsg = "Protocol version mismatch";
			_status = ERROR;
		} else if (jsMinor != protocolMinorVersion) {
		    fprintf(stderr, "Warning: devised implements JavaScreen protocol "
			    "version %d.%d;\n  JavaScreen has version %d.%d\n",
				protocolMajorVersion, protocolMinorVersion, jsMajor, jsMinor);
		}
	}
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::RequestUpdateRecordValue(int argc, char **argv)
{
#if defined (DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::RequestUpdateRecordValue()\n");
#endif

	JSArgs args(argc+1);
	args.FillString(_controlCmdName[UPDATERECORDVALUE]);
	for (int index = 0; index < argc; index++) {
		args.FillString(argv[index]);
	}
	args.ReturnVal(this);

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
		const int argCount = 2;
		const char *argv[argCount] = {
			JavaScreenCmd::_controlCmdName[ERROR],
			errmsg
		};	
		if (argv[1] == NULL)
		{
			argv[1]= "Error, but no error message available";
		}
		ReturnVal(argCount, (char**)&argv[0]);
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
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::SendWindowData(%s)\n", fileName);
#endif

#if defined(JS_TIMER)
    ElapsedTime et;
	et.Start();
#endif

	ControlCmdType	status = DONE;
#if MONOLITHIC_TEST
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
			server = CmdContainer::GetCmdContainer()->getDeviseServer();
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
#if defined(DEBUG_LOG)
        sprintf(logBuf, "    %d bytes sent\n", filesize);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
		}
		if (bytesRead < 0) {
			reportErrSys("Reading image file");
		}
		close(fd);
#if defined(DEBUG_LOG)
        sprintf(logBuf, "  Image file size = %d\n", filesize);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
	}

#if defined(DEBUG_LOG)
    sprintf(logBuf, "  done sending window image or GData\n  status = %d\n",
	  status);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

#if defined(JS_TIMER)
	char timeBuf[MAXPATHLEN * 2];
	sprintf(timeBuf, "Sending file <%s>", fileName);
	et.ReportTime(timeBuf);
#endif

	return status;
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::SendChangedViews(Boolean update)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::SendChangedViews(%d)\n", update);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
#if JS_TIMER
	ElapsedTime et;
	et.Start();
#endif

    JavaScreenCmd::ControlCmdType result = DONE;

	if (update) {
		EraseChangedCursors();
	}

	int viewIndex2 = _topLevelViews.InitIterator();
	while (_topLevelViews.More(viewIndex2)) {
		View *view = (View *)_topLevelViews.Next(viewIndex2);
#if defined(DEBUG_LOG)
        sprintf(logBuf, "Checking view <%s>; GetGifDirty() = %d\n",
		  view->GetName(), view->GetGifDirty());
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
        if (view->GetGifDirty()) {

			//TEMP -- is there some way to avoid destroying and re-creating
			// the same views here?  RKW 1999-10-27.
		    if (update) {
			    DeleteChildViews(view);
		    }

		    int subViewIndex = view->InitIterator();
		    while (view->More(subViewIndex)) {
			    View *subView = (View *)view->Next(subViewIndex);

			    // Force a redraw of all cursors in view symbols that have just
			    // been created.  This ensures that the cursors will be redrawn
			    // by the JavaScreen, even if the devised wouldn't otherwise
			    // have to redraw them.  RKW 1999-10-14.
	            Boolean savePostpone = _postponeCursorCmds;
	            _postponeCursorCmds = true;
			    subView->HideCursors();
			    subView->DrawCursors();
                _postponeCursorCmds = savePostpone;

			    subView->SetZ(view->GetZ() + 1.0);
			    CreateView(subView, view);
			    SendViewDataArea(subView);
		    }
		    view->DoneIterator(subViewIndex);
		}
	}
	_topLevelViews.DoneIterator(viewIndex2);

	//
	// For each "GIF view", if it's dirty, dump it to the GIF file, and send
	// the command for the JavaScreen to update the appropriate window.
	//
	char **imageNames = new char *[View::_viewList->Size()];
	int dirtyGifCount = 0;
	ViewWinList dirtyGifList;

	int viewIndex = _gifViews.InitIterator();
	while (_gifViews.More(viewIndex)) {
		View *view = (View *)_gifViews.Next(viewIndex);
        if (view->GetGifDirty()) {
#if defined(DEBUG_LOG)
	        sprintf(logBuf, "GIF of view <%s> is \"dirty\".\n",
			  view->GetName());
            DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
		    dirtyGifList.Append(view);

			SendViewDataArea(view);

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
	
#if defined(DEBUG_LOG)
            sprintf(logBuf, "  Image file size = %d\n", filesize);
            DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
			if (filesize > 0)
			{
				if (tmpResult == DONE) {
				    UpdateViewImage(view, filesize);
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
		if (gdParams.file != NULL && access(gdParams.file, F_OK) == 0 &&
		  view->Mapped()) {
#if defined(DEBUG_LOG)
	        sprintf(logBuf, "GData of view <%s> is \"dirty\".\n",
			  view->GetName());
            DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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

	DrawChangedCursors();

	//
	// Send DONE here so jspop and js start reading from the image socket.
	//
    if (_status == DONE) {
		JSArgs args(1);
		args.FillString(_controlCmdName[DONE]);
		args.ReturnVal(this);
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

	// Clear the GIF dirty flag on all top-level views (this is really just
	// needed for piled parent views with child GData views, where the
	// parent view is not the first view in the pile).  RKW 1999-10-28.
	viewIndex = _topLevelViews.InitIterator();
	while (_topLevelViews.More(viewIndex)) {
		View *view = (View *)_topLevelViews.Next(viewIndex);
		view->GetWindowRep()->SetGifDirty(false);
    }
	_topLevelViews.DoneIterator(viewIndex);

#if defined(DEBUG_LOG)
    sprintf(logBuf, "End of SendChangedViews; result = %d\n", result);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
#if JS_TIMER
	et.ReportTime("Sending changed views");
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
JavaScreenCmd::RequestUpdateGData(ViewGraph *view)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::RequestUpdateGData(%s)\n",
	  view->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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
	JSArgs args(7);
	args.FillString(_controlCmdName[UPDATEGDATA]);
	args.FillString(view->GetName());
	args.FillDouble(xMult);
	args.FillDouble(xOffset);
	args.FillDouble(yMult);
	args.FillDouble(yOffset);
	args.FillInt(gdSize);

	args.ReturnVal(this);

	return DONE;
}

//====================================================================
void
JavaScreenCmd::DrawCursor(View *view, DeviseCursor *cursor)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::DrawCursor(%s, %s)\n", view->GetName(),
      cursor->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	if (_postponeCursorCmds) {
#if defined (DEBUG_LOG)
        sprintf(logBuf, "Cursor draw temporarily postponed.\n");
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
		if (!_drawnCursors.Find(cursor)) {
			_drawnCursors.Append(cursor);
		}
		return;
	}

	int xPixLow, yPixLow, xPixHigh, yPixHigh;
	cursor->GetDestPixels(xPixLow, yPixLow, xPixHigh, yPixHigh);

#if defined(DEBUG_LOG)
    sprintf(logBuf, "Pixels: (%d, %d), (%d, %d)\n", xPixLow, yPixLow,
	  xPixHigh, yPixHigh);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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

#if defined(DEBUG_LOG)
    sprintf(logBuf, "Modified pixels: (%d, %d), (%d, %d)\n", xPixLow,
	  yPixLow, xPixHigh, yPixHigh);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	int xLoc = (int)MIN(xPixLow, xPixHigh);
	int yLoc = (int)MIN(yPixLow, yPixHigh);
	int width = (int)ABS(xPixHigh - xPixLow) + 1;
	int height = (int)ABS(yPixHigh - yPixLow) + 1;
#if defined(DEBUG_LOG)
    sprintf(logBuf, "x, y: (%d, %d), width, height: (%d, %d)\n", xLoc,
	  yLoc, width, height);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	//
	// Figure out which direction(s) the cursor is allowed to move in.
	//
	VisualFlag cursorFlag = cursor->GetFlag();
	char *movement;
	if ((cursorFlag & VISUAL_X) && (cursorFlag & VISUAL_Y)) {
	  movement = "XY";
	} else if (cursorFlag & VISUAL_X) {
	  movement = "X";
	} else if (cursorFlag & VISUAL_Y) {
	  movement = "Y";
	} else {
	  movement = "";
	}

	//
	// Figure out whether the cursor is fixed-size.
	//
	char *fixedSize;
	if (cursor->GetFixedSize()) {
	  fixedSize = "XY";
	} else {
	  fixedSize = "none";
	}

	//
	// Get the cursor grid information.
	//
	Boolean useGrid;
	Coord gridX, gridY;
	cursor->GetGrid(useGrid, gridX, gridY);
	if (!useGrid) {
	  gridX = gridY = 0.0;
	}

	//
    // Generate the command to send.
	//
	JSArgs args(11);
	args.FillString(_controlCmdName[DRAWCURSOR]);
	args.FillString(cursor->GetName());
	args.FillString(view->GetName());
	args.FillInt(xLoc);
	args.FillInt(yLoc);
	args.FillInt(width);
	args.FillInt(height);
	args.FillString(movement);
	args.FillString(fixedSize);
	args.FillDouble(gridX);
	args.FillDouble(gridY);

	//
    // This JavaScreenCmd object is created only to send the command.
	//
    JavaScreenCmd jsc(ControlPanel::Instance(), NULL_SVC_CMD, 0, NULL);
	args.ReturnVal(&jsc);
}

//====================================================================
void
JavaScreenCmd::EraseCursor(View *view, DeviseCursor *cursor)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::EraseCursor(%s, %s)\n", view->GetName(),
      cursor->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

// JavaScreen will now automatically erase cursor before re-drawing it,
// so (hopefully) we should never have to explicitly request it to erase
// a cursor.  RKW 1999-04-02.
#if 0
	if (_postponeCursorCmds) {
#if defined (DEBUG_LOG)
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
		  "Cursor erase temporarily postponed.\n");
#endif
		if (!_erasedCursors.Find(cursor)) {
			_erasedCursors.Append(cursor);
		}
		return;
	}

	JSArgs args(3);
	args.FillString(_controlCmdName[ERASECURSOR]);
	args.FillString(cursor->GetName());
	args.FillString(view->GetName());

    // This JavaScreenCmd object is created only to send the command.
    JavaScreenCmd jsc(ControlPanel::Instance(), NULL_SVC_CMD, 0, NULL);
	args.ReturnVal(&jsc);
#endif
}

//====================================================================
void
JavaScreenCmd::ReturnVal(int argc, char** argv)
{
#if defined(DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd(0x%p)::ReturnVal(", this);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf, argc, argv,
	  ")\n");
#endif
#if defined(DEBUG)
    printf("JavaScreenCmd::ReturnVal(");
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
	server = CmdContainer::GetCmdContainer()->getDeviseServer();
	server->CloseClient();
}

//====================================================================
void JavaScreenCmd::UpdateSessionList(char *dirName)
{
	const char *basePath = getenv("DEVISE_SESSION");

	//
	// Add the specified directory name onto the current directory.
	//
	char newPath[MAXPATHLEN];
	if (dirName != NULL) {
	    sprintf(newPath, "%s/%s", basePath, dirName);
	} else {
	    strcpy(newPath, basePath);
	}

    //
	// Make sure we're not going up from the base session directory.
	//
	char baseBuf[MAXPATHLEN];
	char newBuf[MAXPATHLEN];

	// resolvepath() doesn't terminate the strings(!).
	memset(baseBuf, 0, MAXPATHLEN);
	memset(newBuf, 0, MAXPATHLEN);

	const char *sessionDir = basePath;
    if (resolvepath(basePath, baseBuf, MAXPATHLEN) < 0) {
	    reportErrSys("Error in resolvepath()");
	} else if (resolvepath(newPath, newBuf, MAXPATHLEN) < 0) {
	    reportErrSys("Error in resolvepath()");
	} else {
#if defined(DEBUG)
        printf("baseBuf = <%s>\n", baseBuf);
        printf("newBuf = <%s>\n", newBuf);
#endif
	    int length = strlen(baseBuf);
		if (strncmp(baseBuf, newBuf, length)) {
            errmsg = "Illegal session directory (can't go up from base)";
			reportErrNosys(errmsg);
		} else {
		    sessionDir = newBuf;
		}
	}

	//
	// Now open the directory and assemble a list of its files.
	//
	ArgList files;

	DIR *directory = opendir(sessionDir);
	if (directory == NULL) {
	    char errBuf[MAXPATHLEN * 2];
		sprintf(errBuf, "Can't open session directory (%s)", sessionDir);
		reportErrSys(errBuf);

	    // Reset things to the "base" session directory and try again.
		sessionDir = getenv("DEVISE_SESSION");
	    directory = opendir(sessionDir);
	}
	if (directory == NULL) {
	    char errBuf[MAXPATHLEN * 2];
		sprintf(errBuf, "Can't open session directory (%s)", sessionDir);
		reportErrSys(errBuf);
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
		sprintf(fullpath, "%s/%s", sessionDir, file);

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
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::DrawAllCursors()\n");
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

#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "Done with JavaScreenCmd::DrawAllCursors()\n");
#endif
}

//====================================================================
void
JavaScreenCmd::EraseChangedCursors()
{
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::EraseChangedCursors()\n");
#endif

	int index = _erasedCursors.InitIterator();
	while (_erasedCursors.More(index)) {
		DeviseCursor *cursor = _erasedCursors.Next(index);
		View *view = cursor->GetDst();
		if (view != NULL) {
			EraseCursor(view, cursor);
		}
	}
	_erasedCursors.DoneIterator(index);

	_erasedCursors.DeleteAll();
}

//====================================================================
void
JavaScreenCmd::DrawChangedCursors()
{
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1,
	  "JavaScreenCmd::DrawChangedCursors()\n");
#endif

	int index = _drawnCursors.InitIterator();
	while (_drawnCursors.More(index)) {
		DeviseCursor *cursor = _drawnCursors.Next(index);
		View *view = cursor->GetDst();
		if (view != NULL) {
			DrawCursor(view, cursor);
		}
	}
	_drawnCursors.DoneIterator(index);

	_drawnCursors.DeleteAll();
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

	_erasedCursors.DeleteAll();
	_drawnCursors.DeleteAll();

	_topLevelViews.DeleteAll();
    _gifViews.DeleteAll();
    _gdataViews.DeleteAll();
}

//====================================================================
JavaScreenCmd::ControlCmdType
JavaScreenCmd::SendViewGData(ViewGraph *view)
{
#if defined(DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::SendViewGData(%s)\n", view->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
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

//====================================================================
void
JavaScreenCmd::CreateView(View *view, View* parent)
{
#if defined(DEBUG_LOG)
	sprintf(logBuf, "CreateView(%s, %s)\n", view->GetName(),
	  parent ? parent->GetName() : "none");
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	//
	// Get view geometry.
	//
	int viewX, viewY;
	unsigned viewWidth, viewHeight;
	view->RealGeometry(viewX, viewY, viewWidth, viewHeight);
#if defined(DEBUG_LOG)
    sprintf(logBuf, "window RealGeometry = %d, %d, %d, %d\n", viewX,
	  viewY, viewWidth, viewHeight);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	view->AbsoluteOrigin(viewX, viewY);
#if defined(DEBUG_LOG)
    sprintf(logBuf, "view AbsoluteOrigin = %d, %d\n", viewX, viewY);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

    //
	// In non-batch mode, a top-level view's AbsoluteOrigin() method
	// returns the origin relative to the *screen* origin; in batch mode,
	// it's relative to the *window* origin.
	//
	if (parent) {
#if 0 // Either AbsoluteOrigin() got changed, or I just goofed this up
      // in the first place.  RKW.
		// Make subview's location relative to parent.
		int tmpX, tmpY;
		view->GetParent()->AbsoluteOrigin(tmpX, tmpY);
#if defined(DEBUG_LOG)
    	sprintf(logBuf, "parent AbsoluteOrigin = %d, %d\n", tmpX, tmpY);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
		viewX -= tmpX;
		viewY -= tmpY;
#endif
    } else if (ControlPanel::Instance()->GetBatchMode()) {
		// Include window's location.
		int tmpX, tmpY;
		view->GetParent()->AbsoluteOrigin(tmpX, tmpY);
#if defined(DEBUG_LOG)
    	sprintf(logBuf, "parent AbsoluteOrigin = %d, %d\n", tmpX, tmpY);
        DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif
		viewX += tmpX;
		viewY += tmpY;
	}

	// Note: Z is positive out of the screen.
	double viewZ = view->GetZ();
	if (viewZ < 0.0) {
		char errBuf[1024];
		sprintf(errBuf, "Invalid Z value (%g) for view <%s>", viewZ,
		  view->GetName());
		reportErrNosys(errBuf);
	}

	//
	// Get geometry of data area within view.
	//
	int dataX, dataY, dataWidth, dataHeight;
	view->GetDataArea(dataX, dataY, dataWidth, dataHeight);
	// Correct for the fact that GetDataArea returns Y up from the bottom.
	dataY = viewHeight - (dataY + dataHeight);

	//
	// Get view foreground and background color.
	//
	string fgColorStr, bgColorStr;
	PColorID pColor = view->GetForeground();
	RGB rgb;
	if (PM_GetRGB(pColor, rgb)) {
		fgColorStr = rgb.ToString();
	}

	pColor = view->GetBackground();
	if (PM_GetRGB(pColor, rgb)) {
		bgColorStr = rgb.ToString();
	}

	//
	// Get axis types.
	//
	char *xAxisType = "none";
	char *yAxisType = "none";
	
	// Don't draw axes for 3D views.
	// Note that the type is send for 2D views even if the axes are not
	// drawn, so that the JS knows how to display the cursor position.
	if (view->GetNumDimensions() == 2) {
		if (view->GetXAxisAttrType() == DateAttr) {
			xAxisType = "date";
		} else {
			xAxisType = "real";
		}

		if (view->GetYAxisAttrType() == DateAttr) {
			yAxisType = "date";
		} else {
			yAxisType = "real";
		}
	}

	//
	// Get view title.
	//
	char *viewTitle = "";
	if (view->_label.occupyTop) {
		viewTitle = view->_label.name;
	}

	//
	// View mouse movement grid (not yet implemented on this end).
	//
	Coord gridX = 0.0;
	Coord gridY = 0.0;

    //
	// Get information about which (if any) actions are disabled in this
	// view.
	//
	Boolean rubberbandDisabled, cursorMoveDisabled, drillDownDisabled,
	  keysDisabled;
	view->GetDisabledActions(rubberbandDisabled, cursorMoveDisabled,
	  drillDownDisabled, keysDisabled);

	// Change values from disabled to enabled (Hongyu wanted this).
	Boolean rubberbandEnabled = !rubberbandDisabled;
	Boolean cursorMoveEnabled = !cursorMoveDisabled;
	Boolean drillDownEnabled = !drillDownDisabled;
	Boolean keysEnabled = !keysDisabled;

	{ // limit variable scopes
	  JSArgs args(31);
	  args.FillString(_controlCmdName[CREATEVIEW]);
	  args.FillString(view->GetName());
	  args.FillString(parent ? parent->GetName() : "");
	  args.FillString(view->IsInPileMode() ?
	    view->GetParentPileStack()->GetFirstView()->GetName() : "");
	  args.FillInt(viewX);
	  args.FillInt(viewY);
	  args.FillInt(viewWidth);
	  args.FillInt(viewHeight);
	  args.FillDouble(viewZ);
	  args.FillInt(view->GetNumDimensions());
	  args.FillInt(dataX);
	  args.FillInt(dataY);
	  args.FillInt(dataWidth);
	  args.FillInt(dataHeight);
	  args.FillString(fgColorStr.c_str());
	  args.FillString(bgColorStr.c_str());
	  args.FillString(xAxisType);
	  args.FillString(yAxisType);
	  args.FillDouble(gridX);
	  args.FillDouble(gridY);
	  args.FillInt(rubberbandEnabled);
	  args.FillInt(cursorMoveEnabled);
	  args.FillInt(drillDownEnabled);
	  args.FillInt(keysEnabled);

	  args.FillString(viewTitle);
	  if (strcmp(viewTitle, "")) {
		  int titleX, titleY, titleWidth, titleHeight;
		  // Note: this gives title Y relative to the *bottom* of the view.
		  view->GetLabelArea(titleX, titleY, titleWidth, titleHeight);

	      int viewX, viewY;
	      unsigned viewWidth, viewHeight;
	      view->RealGeometry(viewX, viewY, viewWidth, viewHeight);

		  args.FillInt(titleX + (titleWidth / 2));
		  titleY = viewHeight - titleY - 1;
		  args.FillInt(titleY - (titleHeight / 2));

		  int family;
		  float pointSize;
		  Boolean bold, italic;
		  view->GetFont("title", family, pointSize, bold, italic);
		  args.FillInt(family);
		  args.FillDouble(pointSize);
		  args.FillInt(bold);
		  args.FillInt(italic);
	  }

	  args.ReturnVal(this);
	}
}

//====================================================================
void
JavaScreenCmd::DeleteChildViews(View *view)
{
#if defined(DEBUG_LOG)
	sprintf(logBuf, "DeleteChildViews(%s)\n", view->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	JSArgs args(2);
	args.FillString(_controlCmdName[DELETECHILDVIEWS]);
	args.FillString(view->GetName());

	args.ReturnVal(this);
}

//====================================================================
void
JavaScreenCmd::SendViewDataArea(View *view)
{
#if defined(DEBUG_LOG)
	sprintf(logBuf, "SendViewDataArea(%s)\n", view->GetName());
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	// Don't draw axes for 3D views.
	if (view->GetNumDimensions() == 2) {
		VisualFilter filter;
		view->GetVisualFilter(filter);

		// X axis...

		//
   		// Generate the command to send.
		//
		{
		  JSArgs args(5);
		  args.FillString(_controlCmdName[VIEWDATAAREA]);
		  args.FillString(view->GetName());
		  args.FillString("X");
		  args.FillDouble(filter.xLow);
		  args.FillDouble(filter.xHigh);

		  args.ReturnVal(this);
		}


		// Y axis...

		//
   		// Generate the command to send.
		//

		{
		  JSArgs args(5);
		  args.FillString(_controlCmdName[VIEWDATAAREA]);
		  args.FillString(view->GetName());
		  args.FillString("Y");
		  args.FillDouble(filter.yLow);
		  args.FillDouble(filter.yHigh);

		  args.ReturnVal(this);
		}
	}
}

//====================================================================
void
JavaScreenCmd::UpdateViewImage(View *view, int imageSize)
{
#if defined (DEBUG_LOG)
    sprintf(logBuf, "JavaScreenCmd::UpdateViewImage(%s, %d)\n",
	  view->GetName(), imageSize);
    DebugLog::DefaultLog()->Message(DebugLog::LevelInfo1, logBuf);
#endif

	JSArgs args(3);
	args.FillString(_controlCmdName[UPDATEVIEWIMAGE]);
	args.FillString(view->GetName());
	args.FillInt(imageSize);

	args.ReturnVal(this);
}
