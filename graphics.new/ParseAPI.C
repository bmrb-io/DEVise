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
   IMPORTANT!!  All member functions of the ControlPanel class that are
   called here must be implemented in the ControlPanelSimple class in 
   Session.c.  RKW Oct. 2, 1997.
 */

/*
  $Id$

  $Log$
  Revision 1.90  1998/02/05 23:46:04  wenger
  Added view-level specification of symbol alignment, API commands, simple
  GUI for Sanjay.

  Revision 1.89  1998/02/03 23:46:36  wenger
  Fixed a problem Hongyu had with getting GData on socket; fixed bugs
  283 and 285 (resulted from problems in color manager merge);
  conditionaled out some debug output.

  Revision 1.88  1998/02/02 18:26:14  wenger
  Strings file can now be loaded manually; name of strings file is now
  stored in session file; added 'serverExit' command and kill_devised
  script to cleanly kill devised; fixed bug 249; more info is now
  printed for unrecognized commands.

  Revision 1.87  1998/01/30 02:17:03  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.86  1998/01/07 19:29:52  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.85  1997/12/23 21:00:11  wenger
  Got the latest version of DEVise to compile and link on SPARC/Solaris;
  removed (old) layout manager code from DEVise link.

  Revision 1.84  1997/12/12 05:50:42  weaver
  *** empty log message ***

  Revision 1.83  1997/12/05 18:42:23  wenger
  Timers are now stopped whenever we're doing anything with the data
  socket.

  Revision 1.82  1997/12/01 21:21:32  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br3 tag
  into the trunk.

  Revision 1.81  1997/11/24 23:15:11  weaver
  Changes for the new ColorManager.

  Revision 1.80  1997/11/24 16:22:28  wenger
  Added GUI for saving GData; turning on GData to socket now forces
  redraw of view; GData to socket params now saved in session files;
  improvement to waitForQueries command.

  Revision 1.79  1997/11/18 23:26:57  wenger
  First version of GData to socket capability; removed some extra include
  dependencies; committed test version of TkControl::OpenDataChannel().

  Revision 1.78  1997/11/12 15:46:38  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
  into the trunk.

  Revision 1.77.2.6  1998/01/16 23:41:42  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.77.2.5  1998/01/07 16:00:16  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.77.2.4  1998/01/06 17:04:14  wenger
  Added 'testDataSock' command (used by TestClient).

  Revision 1.77.2.3  1997/12/23 21:43:09  wenger
  Disabled layout manager GUI and removed (old) layout manager code
  from DEVise link.

  Revision 1.77.2.2  1997/11/14 17:31:17  wenger
  More error messages for sending images to socket.

  Revision 1.77.2.1  1997/11/11 19:13:58  wenger
  Added getWindowImageAndSize and waitForQueries commands; fixed bug in
  WindowRep::ExportGIF() inheritance.

  Revision 1.77  1997/10/03 14:37:13  wenger
  Various fixes to get session opening/saving to work with client/server
  version; reading old-style (Tcl) session files now works in back end;
  got back-end session file stuff working for multi.

  Revision 1.76  1997/10/02 18:46:22  wenger
  Opening and saving batch-style sessions in back end now fully working;
  added tk2ds.tcl script for conversion.

  Revision 1.75  1997/09/23 19:57:38  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.74  1997/09/05 22:36:21  wenger
  Dispatcher callback requests only generate one callback; added Scheduler;
  added DepMgr (dependency manager); various minor code cleanups.

  Revision 1.73  1997/08/27 14:44:17  wenger
  Made improvements to getDisplayImageAndSize command (not yet fully tested).

  Revision 1.72  1997/08/20 19:15:18  wenger
  Added new getDisplayImageAndSize command for Hongyu (not yet fully
  tested).

  Revision 1.71  1997/07/22 15:36:37  wenger
  Added capability to dump human-readable information about all links
  and cursors.

  Revision 1.70  1997/07/17 18:43:59  wenger
  Added menu selections to report number of strings and save string space.

  Revision 1.69  1997/06/25 21:25:29  wenger
  Added writeDesc (write session description) command needed by Hongyu's
  Java client.

  Revision 1.68  1997/06/09 14:46:52  wenger
  Added cursor grid; fixed bug 187; a few minor cleanups.

  Revision 1.67  1997/05/30 21:18:54  wenger
  Oops!  Minor error in new print config. stuff.

  Revision 1.66  1997/05/30 20:45:31  wenger
  Added GUI to allow user to specify windows to exclude from display
  print and/or print from pixmaps (for EmbeddedTk).  Exclusion is
  implemented but pixmap printing is not.

  Revision 1.65  1997/05/30 15:41:19  wenger
  Most of the way to user-configurable '4', '5', and '6' keys -- committing
  this stuff now so it doesn't get mixed up with special stuff for printing
  Mitre demo.

  Revision 1.64  1997/05/28 15:39:25  wenger
  Merged Shilpa's layout manager code through the layout_mgr_branch_2 tag.

  Revision 1.63.4.1  1997/05/20 16:11:11  ssl
  Added layout manager to DEVise

  Revision 1.63  1997/04/21 22:55:36  guangshu
  Added several CPI commands.

  Revision 1.63.6.1  1997/05/21 20:40:41  weaver
  Changes for new ColorManager

  Revision 1.62  1997/03/25 17:59:25  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.61  1997/03/20 22:12:32  guangshu
  Added DEVise command SetBuckRefresh, checkGstat, isXDateType.

  Revision 1.60  1997/03/19 19:41:50  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.59  1997/03/19 18:44:41  wenger
  Disabled RTree indices in DTE, got that to compile; fixed some compile
  warnings; undid accidental(?) changes to devise run script; devise.etk
  uses 'localhost' instead of '$HOSTNAME'; added Solaris-only release
  script.

  Revision 1.58  1997/03/18 20:42:35  donjerko
  Changes for GUI.

  Revision 1.57  1997/03/14 20:33:55  donjerko
  *** empty log message ***

  Revision 1.56  1997/03/14 18:36:59  donjerko
  Separated DTE calls from ParseAPI.C into ParseAPIDTE.c

  Revision 1.55  1997/03/06 23:52:02  ssl
  Added functionality for issuing a flush on the TData from GUI

  Revision 1.54  1997/03/06 02:37:20  donjerko
  *** empty log message ***

  Revision 1.53  1997/03/02 00:42:40  donjerko
  Attribute recId as not added to the schema in this file, it is done
  by DTE now. This might screw up the UNIXFILE because they are not
  handled by DTE, but UNIXFILE will be gone soon anyway.

  Revision 1.52  1997/02/26 16:31:39  wenger
  Merged rel_1_3_1 through rel_1_3_3c changes; compiled on Intel/Solaris.

  Revision 1.51  1997/02/18 18:07:14  donjerko
  *** empty log message ***

  Revision 1.50  1997/02/03 19:45:29  ssl
  1) RecordLink.[Ch],QueryProcFull.[ch]  : added negative record links
  2) ViewLens.[Ch] : new implementation of piled views
  3) ParseAPI.C : new API for ViewLens, negative record links and layout
     manager

  Revision 1.49  1997/02/03 04:12:16  donjerko
  Catalog management moved to DTE

  Revision 1.48.4.2  1997/03/15 00:31:30  wenger
  PostScript printing of entire DEVise display now works; PostScript output
  is now centered on page; other cleanups of the PostScript printing along
  the way.

  Revision 1.48.4.1  1997/02/13 18:11:45  ssl
  Added a check to the user interface asking when user links two different
  data sets with a record link

  Revision 1.48  1997/01/13 18:08:06  wenger
  Fixed bugs 043, 083, 084, 091, 114.

  Revision 1.47  1996/12/20 16:50:25  wenger
  Fonts for view label, x axis, and y axis can now be changed.

  Revision 1.46  1996/12/15 06:48:52  donjerko
  Added support for RTrees and moved DQL sources to DTE/DeviseSpecific dir

  Revision 1.45  1996/11/26 16:51:35  ssl
  Added support for piled viws

  Revision 1.44  1996/11/20 20:35:20  wenger
  Fixed bugs 062, 073, 074, and 075; added workaround for bug 063; make
  some Makefile improvements so compile works first time; fixed up files
  to correspond to new query catalog name.

  Revision 1.43  1996/11/20 16:50:33  jussi
  Added abortQuery command.

  Revision 1.42  1996/11/19 02:42:20  kmurli
  Changed to include original importFileType parameters..This means that
  the those using the importFileType with a single parameter cannot use
  Query interface.

  Revision 1.41  1996/11/15 10:06:17  kmurli
  Changed importFile parameters and ParseCat parameters to take in the file type
  and data file name so that a whole query can be formed if necessary for calling
  DQL type. (In case of a query schema)

  Revision 1.40  1996/11/13 16:57:07  wenger
  Color working in direct PostScript output (which is now enabled);
  improved ColorMgr so that it doesn't allocate duplicates of colors
  it already has, also keeps RGB values of the colors it has allocated;
  changed Color to GlobalColor, LocalColor to make the distinction
  explicit between local and global colors (_not_ interchangeable);
  fixed global vs. local color conflict in View class; changed 'dali'
  references in command-line arguments to 'tasvir' (internally, the
  code still mostly refers to Dali).

  Revision 1.39  1996/11/04 22:05:04  wenger
  Turned off debug output in ParseAPI.C; fixed compile problem in
  TDataDQLInterp.c.

  Revision 1.38  1996/11/03 18:20:46  kmurli
  Changes for incorporating DQL type..

  Revision 1.37  1996/11/03 02:41:37  kmurli
  Modified to include the query schema level. Also modified to include DQL
  processing

  Revision 1.36  1996/11/01 19:28:20  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.35  1996/10/08 23:34:38  guangshu
  Fixed bug found by cs737 class.

  Revision 1.34  1996/10/04 17:24:15  wenger
  Moved handling of indices from TDataAscii and TDataBinary to new
  FileIndex class.

  Revision 1.33  1996/09/27 15:51:51  wenger
  Added "Memory leaks" and "Memory in use" menu selections and associated
  Tcl "DEVise" commands (conditionaled out for commit).

  Revision 1.32  1996/09/18 20:18:50  guangshu
  Added API command saveDisplayView to save each view in the window.

  Revision 1.31  1996/09/13 23:06:06  guangshu
  Added saveDisplayImageAndMap for saving map files.

  Revision 1.30  1996/09/10 20:07:24  wenger
  High-level parts of new PostScript output code are in place (conditionaled
  out for now so that the old code is used until the new code is fully
  working); changed (c) (tm) in windows so images are not copyrighted
  by DEVise; minor bug fixes; added more debug code in the course of working
  on the PostScript stuff.

  Revision 1.29  1996/09/05 22:19:03  jussi
  Added setScreenSize command. Renamed connectData as getWindowImage.
  Added getDisplayImage command.

  Revision 1.28  1996/08/29 22:17:22  guangshu
  Added connectData for DEVise server.

  Revision 1.27  1996/08/08 21:02:48  beyer
  made to compile with gcc 2.6.3

  Revision 1.26  1996/08/07 15:26:21  guangshu
  Added DEVise commands getStatBuff and getAllStats to get the color statistics
  gstat histigram and global stats buffer.

  Revision 1.25  1996/08/03 15:38:15  jussi
  Flag _solid3D now has three values.

  Revision 1.24  1996/07/23 20:12:59  wenger
  Preliminary version of code to save TData (schema(s) and data) to a file.

  Revision 1.23  1996/07/23 15:40:40  jussi
  Added set/getViewDisplayDataValues and set/getViewPileMode commands.

  Revision 1.22  1996/07/21 02:20:12  jussi
  Added getViewXYZoom and setViewXYZoom commands.

  Revision 1.21  1996/07/20 18:49:09  jussi
  Added getViewSolid3D and setViewSolid3D commands.

  Revision 1.20  1996/07/18 01:16:53  jussi
  Added saveDisplayImage command.

  Revision 1.19  1996/07/14 04:05:34  jussi
  Added (kludgy) ResetVkg() function which allows ViewKGraph
  to inform ParseAPI that the current KGraph has been
  destroyed.

  Revision 1.18  1996/07/11 17:25:47  wenger
  Devise now writes headers to some of the files it writes;
  DataSourceSegment class allows non-fixed data length with non-zero
  offset; GUI for editing schema files can deal with comment lines;
  added targets to top-level makefiles to allow more flexibility.

  Revision 1.17  1996/07/10 19:01:14  jussi
  Get3DLocation now reports fix_focus and perspective flags.

  Revision 1.16  1996/07/09 16:00:16  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.15  1996/06/27 00:01:39  jussi
  Changed number of parameters of set3DLocation.

  Revision 1.14  1996/06/20 21:47:30  jussi
  Added additional parameters to the return string of command
  get3DLocation. Command set3DLocation now switches to rectangular
  coordinate system temporarily when updating camera location.

  Revision 1.13  1996/06/15 14:49:29  jussi
  Added set3DLocation command.

  Revision 1.12  1996/06/15 13:50:59  jussi
  Added get3DLocation and setMappingLegend commands.

  Revision 1.11  1996/06/12 14:56:23  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.10  1996/06/04 14:21:38  wenger
  Ascii data can now be read from session files (or other files
  where the data is only part of the file); added some assertions
  to check for pointer alignment in functions that rely on this;
  Makefile changes to make compiling with debugging easier.

  Revision 1.9  1996/05/31 17:09:38  jussi
  Fixed off-by-one error in "showkgraph" command parsing.

  Revision 1.8  1996/05/31 15:38:26  jussi
  Added setLinkMaster, resetLinkMaster, and getLinkMaster commands.

  Revision 1.7  1996/05/15 16:36:20  jussi
  Added sync command.

  Revision 1.6  1996/05/15 16:02:37  wenger
  Restored 'parseSchema' command (got lost in one of the recent changes);
  added 'Bugs' file to keep track of bugs in a central area.

  Revision 1.5  1996/05/13 21:57:53  jussi
  Added setBatchMode command.

  Revision 1.4  1996/05/13 18:14:13  jussi
  Changed type of "flag" parameter. Changed code to reflect new
  flag values: API_CMD, API_ACK, API_NAK, API_CTL.

  Revision 1.3  1996/05/11 20:43:52  jussi
  Moved output statement from "exit" command to ServerAPI.c.

  Revision 1.2  1996/05/11 19:08:53  jussi
  Added replica management.

  Revision 1.1  1996/05/11 17:26:26  jussi
  Initial revision. Moved all API parsing to this file so that
  ServerAPI.c and TkControl.c would not have to duplicate it.
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

//#define DEBUG
#define LINESIZE 1024
int WriteFileToDataSock(ControlPanel *control, int port, char *tmpFile);

int
GetDisplayImageAndSize(ControlPanel *control, int port, char *imageType)
{
#if defined(DEBUG)
  printf("GetDisplayImageAndSize()\n");
#endif

  if (strcmp(imageType, "gif")) {
    control->ReturnVal(API_NAK, "Can only support GIF now.");
    return -1;
  }

  // Write the GIF to a temporary file.
  char tmpFile[L_tmpnam];
  (void) tmpnam(tmpFile);
  FILE *tmpfp = fopen(tmpFile, "w");
  if (tmpfp == NULL) {
    char errBuf[2096];
    sprintf("Can't open temp file (%s)", tmpFile);
    reportErrSys(errBuf);
    control->ReturnVal(API_NAK, errBuf);
    return -1;
  }
  Timer::StopTimer();
  DeviseDisplay::DefaultDisplay()->ExportGIF(tmpfp, false);
  Timer::StartTimer();
  if (fclose(tmpfp) != 0) {
    reportErrSys("fclose error");
  }

  // Write the temporary file to the data socket (prefixing it with its
  // size).
  if (WriteFileToDataSock(control, port, tmpFile) != 1) {
    (void) unlink(tmpFile);
    return -1;
  }

  if (unlink(tmpFile) != 0) {
    reportErrSys("unlink error");
  }

  control->ReturnVal(API_ACK, "done");
  return 1;
}

int
GetWindowImageAndSize(ControlPanel *control, int port, char *imageType,
    char *windowName)
{
#if defined(DEBUG)
  printf("GetWindowImageAndSize(%p, %d, %s, %s)\n", control, port, imageType,
      windowName);
#endif

  if (strcmp(imageType, "gif")) {
    control->ReturnVal(API_NAK, "Can only support GIF now.");
    return -1;
  }

  ClassDir *classDir = control->GetClassDir();
  ViewWin *viewWin = (ViewWin *)classDir->FindInstance(windowName);
  if (!viewWin) {
    control->ReturnVal(API_NAK, "Cannot find window");
    return -1;
  }

  // Write the GIF to a temporary file.
  char tmpFile[L_tmpnam];
  (void) tmpnam(tmpFile);
  FILE *tmpfp = fopen(tmpFile, "w");
  if (tmpfp == NULL) {
    reportErrSys("Can't open temp file");
    control->ReturnVal(API_NAK, "Can't open temp file");
    return -1;
  }
  Timer::StopTimer();
  viewWin->GetWindowRep()->ExportGIF(tmpfp);
  Timer::StartTimer();
  if (fclose(tmpfp) != 0) {
    reportErrSys("fclose error");
  }

  // Write the temporary file to the data socket (prefixing it with its
  // size).
  if (WriteFileToDataSock(control, port, tmpFile) != 1) {
    (void) unlink(tmpFile);
    return -1;
  }

  if (unlink(tmpFile) != 0) {
    reportErrSys("unlink error");
  }

  control->ReturnVal(API_ACK, "done");

  return 1;
}

int
WriteFileToDataSock(ControlPanel *control, int port, char *tmpFile)
{
#if defined(DEBUG)
  printf("WriteFileToDataSock()\n");
#endif

  int tmpfd = open(tmpFile, O_RDONLY, 0644);

  // Find out out big the temp file is.
  struct stat filestat;
  if (fstat(tmpfd, &filestat) != 0) {
    reportErrSys("Can't get temp file size");
    control->ReturnVal(API_NAK, "Can't get temp file size");
    (void) close(control->getFd());
    (void) close(tmpfd);
    return -1;
  }

  control->OpenDataChannel(port);
  if (control->getFd() < 0) {
    reportErrSys("Cannot open data channel");
    control->ReturnVal(API_NAK, "Invalid socket to write");
    return -1;
  }

  // Write the size and the GIF to the socket.
  const int bufSize = 1024;
  char buf[bufSize];
  sprintf(buf, "%d", (int) filestat.st_size);
  int numBytes = strlen(buf) + 1;
  Timer::StopTimer();
  if (write(control->getFd(), buf, numBytes) != numBytes) {
    reportErrSys("write error");
    control->ReturnVal(API_NAK, "write error");
    (void) close(control->getFd());
    (void) close(tmpfd);
    Timer::StartTimer();
    return -1;
  }
  Timer::StartTimer();

  int bytesLeft = (int) filestat.st_size;
  while (bytesLeft > 0) {
    Timer::StopTimer();
    int bytesToRead = MIN(bytesLeft, bufSize);
    int bytesRead = read(tmpfd, buf, bytesToRead);
    if (bytesRead != bytesToRead) {
      reportErrSys("read error");
      control->ReturnVal(API_NAK, "read error");
      (void) close(control->getFd());
      (void) close(tmpfd);
      Timer::StartTimer();
      return -1;
    }
    if (write(control->getFd(), buf, bytesRead) != bytesRead) {
      reportErrSys("write error");
      control->ReturnVal(API_NAK, "write error");
      (void) close(control->getFd());
      (void) close(tmpfd);
      Timer::StartTimer();
      return -1;
    }
    bytesLeft -= bytesRead;
    Timer::StartTimer();
  }
  if (close(control->getFd()) != 0) {
    reportErrSys("close error");
  }
  if (close(tmpfd) != 0) {
    reportErrSys("close error");
  }

  return 1;
}

//******************************************************************************

int		ParseAPIColorCommands(int argc, char** argv, ControlPanel* control)
{
	Trace("ParseAPIColorCommands()");

	char		result[10 * 1024];
	ClassDir*	classDir = control->GetClassDir();

	if (!strcmp(argv[1], "GetColor"))
	{
		Trace("    Command: color GetColor");

		PColorID	pcid = ((PColorID)atoi(argv[2]));
		RGB			rgb;

		if (!PM_GetRGB(pcid, rgb) && !PM_GetRGB((PColorID)0, rgb))
		{
			control->ReturnVal(API_ACK, "Couldn't find Color");
			return -1;
		}
			
		strcpy(result, rgb.ToString().c_str());
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "GetColorID"))
	{
		Trace("    Command: color GetColorID");

		PColorID	pcid;
		RGB			rgb;

		rgb.FromString(argv[2]);

		if (!PM_GetPColorID(rgb, pcid))
		{
			control->ReturnVal(API_ACK, "Couldn't find RGB");
			return -1;
		}

		sprintf(result, "%ld", pcid);
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "GetForeground"))
	{
		Trace("    Command: color GetForeground");

		ViewWin*	view = (ViewWin*)classDir->FindInstance(argv[2]);

		if (!view)
		{
			control->ReturnVal(API_NAK, "Cannot find view");
			return -1;
		}

		PColorID	pcid = view->GetForeground();
		RGB			rgb;

		if (!PM_GetRGB(pcid, rgb) && !PM_GetRGB((PColorID)0, rgb))
		{
			control->ReturnVal(API_NAK, "Couldn't get color");
			return -1;
		}

		string		s = rgb.ToString();
		
		strcpy(result, s.c_str());
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "GetBackground"))
	{
		Trace("    Command: color GetBackground");

		ViewWin*	view = (ViewWin*)classDir->FindInstance(argv[2]);

		if (!view)
		{
			control->ReturnVal(API_NAK,"Cannot find view");
			return -1;
		}

		PColorID	pcid = view->GetBackground();
		RGB			rgb;

		if (!PM_GetRGB(pcid, rgb) && !PM_GetRGB((PColorID)0, rgb))
		{
			control->ReturnVal(API_NAK, "Couldn't get color");
			return -1;
		}

		string		s = rgb.ToString();
		
		strcpy(result, s.c_str());
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "SetForeground"))
	{
		Trace("    Command: color SetForeground");

		View*	view = (View*)classDir->FindInstance(argv[2]);

		if (!view)
		{
			control->ReturnVal(API_NAK, "Cannot find view");
			return -1;
		}

		PColorID	pcid;
		RGB			rgb;

		rgb.FromString(argv[3]);

		if (!PM_GetPColorID(rgb, pcid))
		{
			control->ReturnVal(API_NAK, "Couldn't set color");
			return -1;
		}

		view->SetForeground(pcid);
		control->ReturnVal(API_ACK, "done");
		return 1;
	}

	if (!strcmp(argv[1], "SetBackground"))
	{
		Trace("    Command: color SetBackground");

		View*	view = (View*)classDir->FindInstance(argv[2]);

		if (!view)
		{
			control->ReturnVal(API_NAK, "Cannot find view");
			return -1;
		}

		PColorID	pcid;
		RGB			rgb;

		rgb.FromString(argv[3]);

		if (!PM_GetPColorID(rgb, pcid))
		{
			control->ReturnVal(API_NAK, "Couldn't set color");
			return -1;
		}

		view->SetBackground(pcid);
		control->ReturnVal(API_ACK, "done");
		return 1;
	}

//	// This command does nothing right now (override color doesn't exist)
//	if (!strcmp(argv[1], "GetOverrideColor"))
//	{
//		Trace("    Command: color GetOverrideColor");

//		View*	view = (View*)classDir->FindInstance(argv[2]);

//		if (!view)
//		{
//			control->ReturnVal(API_NAK, "Cannot find view");
//			return -1;
//		}

//		Boolean		active = false;
////		ColorID		color = view->GetOverrideColor(active);
//		ColorID		color = 0;

//		sprintf(result, "%d %d", (active ? 1 : 0), (int)color);
//		control->ReturnVal(API_ACK, result);
//		return 1;
//    }

//	// This command does nothing right now (override color doesn't exist)
//    if (!strcmp(argv[1], "SetOverrideColor"))
//	{
//		Trace("    Command: color SetOverrideColor");

//		View*	view = (View*)classDir->FindInstance(argv[2]);

//		if (!view)
//		{
//			control->ReturnVal(API_NAK, "Cannot find view");
//			return -1;
//		}

//		Boolean		active = (atoi(argv[3]) == 1);

////		view->SetOverrideColor((ColorID)atoi(argv[4]), active);
//		control->ReturnVal(API_ACK, "done");
//		return 1;
//	}

	if (!strcmp(argv[1], "GetAllColors"))
	{
		Trace("    Command: color GetAllColors");

		RGBList		rgbList = CM_GetRGBList();
		string		s;

		for (unsigned int i=0; i<rgbList.size(); i++)
		{
			s += rgbList[i].ToString();

			if (i < rgbList.size() - 1)
				s += ' ';
		}

		strcpy(result, s.c_str());
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	// Command to request automatic color selection from the ColorManager.
	// VERY DUMB for now. Finds most distant color. Push into the real CM later.
	// The commented code inside is obsolete.
	if (!strcmp(argv[1], "AutoColor"))
	{
		Trace("    Command: color AutoColor");

		ColorID		bg, fg = 0;
//		float		r = 0 , g = 0, b = 0, d, dmax = 0;
//		RGB			back, fore;

		bg = (ColorID)atoi(argv[2]);			// Background ColorID arg
//		ColorMgr::GetColorRgb(bg, r, g, b);		// Get rgb values from ColorMgr
//		back.Set(r, g, b);
		
//		for (int i=1; i<43; i++)
//		{
//			ColorMgr::GetColorRgb((GlobalColor)i, r, g, b);
//			fore.Set(r, g, b);
//			d = back.Distance(fore);			// Calculate color distance

////			printf("  Trying (%f,%f,%f), d = %f\n", r, g, b, d);
			
//			if (d > dmax)
//			{
//				dmax = d;
//				fg = (GlobalColor)i;			// Remember new maximal distance
//			}
//		}

		fg = 0;	// Always black

		sprintf(result, "%d", (int)fg);
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "NewPalette"))
	{
		Trace("    Command: color NewPalette");

		string		s(argv[2]);
		PaletteID	pid = PM_NewPalette(s);

		if (pid == nullPaletteID)
		{
			control->ReturnVal(API_ACK, "Couldn't create palette");
			return -1;
		}

		sprintf(result, "%d", (int)pid);
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "GetCurrentPalette"))
	{
		Trace("    Command: color GetCurrentPalette");

		PaletteID	pid = PM_GetCurrentPalette();

		if (pid == nullPaletteID)
		{
			control->ReturnVal(API_ACK, "Couldn't get palette");
			return -1;
		}

		sprintf(result, "%d", (int)pid);
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	// This command always "succeeds", but failure to change the palette
	// is indicated by returning the current pid after the operation.
	if (!strcmp(argv[1], "SetCurrentPalette"))
	{
		Trace("    Command: color SetCurrentPalette");

		PaletteID	pid = (PaletteID)atoi(argv[2]);
		PaletteID	oldPid = PM_GetCurrentPalette();

		PM_SetCurrentPalette(pid);

		if (pid != oldPid) View::RefreshAll();

		sprintf(result, "%d", (int)PM_GetCurrentPalette());
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "GetPaletteColors"))
	{
		Trace("    Command: color GetPaletteColors");

		PaletteID	pid = (PaletteID)atoi(argv[2]);
		Palette*	palette = PM_GetPalette(pid);

		if (palette == NULL)
		{
			control->ReturnVal(API_ACK, "Couldn't get palette colors");
			return -1;
		}

		string		s = palette->ToString();

		strcpy(result, s.c_str());
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	if (!strcmp(argv[1], "CheckColoring"))
	{
		Trace("    Command: color CheckColoring");

//		View*		view = (View*)classDir->FindInstance(argv[2]);
		ViewGraph*	view = (ViewGraph*)classDir->FindInstance(argv[2]);

		if (!view)
		{
			control->ReturnVal(API_NAK, "Cannot find view");
			return -1;
		}

		double	distance = view->RMSDistance();
//		double	distance = view->CalcDataColorEntropy();

		sprintf(result, "%f", distance);
		control->ReturnVal(API_ACK, result);
		return 1;
	}

	return -1;
}

//******************************************************************************
