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
  Revision 1.48.14.2  2005/09/28 17:14:50  wenger
  Fixed a bunch of possible buffer overflows (sprintfs and
  strcats) in DeviseCommand.C and Dispatcher.c; changed a bunch
  of fprintfs() to reportErr*() so the messages go into the
  debug log; various const-ifying of function arguments.

  Revision 1.48.14.1  2003/12/19 18:07:51  wenger
  Merged redhat9_br_0 thru redhat9_br_1 to V1_7b0_br.

  Revision 1.48.32.1  2003/12/17 00:18:06  wenger
  Merged gcc3_br_1 thru gcc3_br_2 to redhat9_br (just fixed conflicts,
  didn't actually get it to work).

  Revision 1.48.30.1  2003/12/16 16:08:22  wenger
  Got DEVise to compile with gcc 3.2.3 (with lots of deprecated-header
  warnings).  It runs on RedHat 7.2, but not on Solaris 2.8 (some kind
  of dynamic library problem).

  Revision 1.48  2000/08/10 16:10:53  wenger
  Phase 1 of getting rid of shared-memory-related code.

  Revision 1.47  2000/03/23 19:58:41  wenger
  Updated dependencies, got everything to compile on pumori (Linux 2.2.12,
  g++ 2.95.2).

  Revision 1.46  2000/02/16 18:51:42  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.45  2000/01/13 23:07:10  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.44  1999/11/30 22:28:25  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.43  1999/10/18 15:36:42  wenger
  Window destroy events are handled better (DEVise doesn't crash); messages
  such as window destroy notifications are now passed to the client in
  client/server form.  (Parsing a string into arguments was moved from the
  Session class to the ArgList class.)

  Revision 1.42  1999/10/05 17:55:50  wenger
  Added debug log level.

  Revision 1.41  1999/10/04 19:37:09  wenger
  Mouse location is displayed in "regular" DEVise.

  Revision 1.40  1999/06/25 15:58:24  wenger
  Improved debug logging, especially for JavaScreen support: JavaScreenCmd.C
  now uses DebugLog facility instead of printf; dispatcher logging is turned
  on by default, and commands and queries are logged; added -debugLog command
  line flag to turn logging on and off.

  Revision 1.39  1998/11/04 20:33:59  wenger
  Multiple string tables partly working -- loading and saving works, one
  table per mapping works; need multiple tables per mapping, API and GUI,
  saving to session, sorting.

  Revision 1.38  1998/09/30 17:44:46  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.37  1998/09/28 20:05:53  wenger
  Fixed bug 383 (unnecessary creation of QueryProc); moved all
  DestroySessionData() code from subclasses of ControlPanel into base class,
  because it was all the same; found and fixed bug 398 (caused by a change
  in the propagation of view selections).

  Revision 1.36  1998/05/02 09:00:50  taodb
  Added support for JAVA Screen and command logging

  Revision 1.35  1998/02/12 17:17:07  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.34  1998/01/30 02:17:05  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.33.2.1  1998/01/28 22:44:00  taodb
  Added support for group communicatoin

  Revision 1.33  1998/01/07 19:29:54  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.32  1997/12/11 04:25:44  beyer
  Shared memory and semaphores are now released properly when devise
  terminates normally.

  Revision 1.31  1997/12/01 21:21:36  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br3 tag
  into the trunk.

  Revision 1.30  1997/11/19 17:02:11  wenger
  Fixed error in OpenDataChannel().

  Revision 1.29.12.3  1998/01/16 23:41:44  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.29.12.2  1998/01/07 16:00:18  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.29.12.1  1997/11/14 17:31:19  wenger
  More error messages for sending images to socket.

  Revision 1.29  1997/04/03 16:36:47  wenger
  Reduced memory and CPU usage in statistics; fixed a memory leak in the
  statistics code; switched devised back to listening on port 6100
  (changed accidentally?); turned off a bunch of debug output.

  Revision 1.28  1997/03/19 19:41:51  andyt
  Embedded Tcl/Tk windows are now sized in data units, not screen pixel
  units. The old list of ETk window handles (WindowRep class) has been
  replaced by a list of ETkInfo structs, each with fields for the window
  handle, x-y coordinates, name of the Tcl script, and an "in_use"
  flag. Added an ETk_Cleanup() procedure that gets called inside
  View::ReportQueryDone and destroys all ETk windows that are not marked
  as in_use.

  Revision 1.27  1996/12/20 16:26:28  jussi
  Removed call to SemaphoreV::create().

  Revision 1.26  1996/12/13 21:33:43  jussi
  Updated to use SemaphoreV::maxNumSemaphores().

  Revision 1.25  1996/12/12 22:02:50  jussi
  Added checking of user interrupts when waiting for client connection.

  Revision 1.24  1996/12/04 00:12:34  jussi
  Removed inclusion of netdb.h because it got included from machdep.h
  also, and netdb.h isn't protected by a pair of ifdef-define's.

  Revision 1.23  1996/12/03 20:35:17  jussi
  Added initialization of semaphore structures.

  Revision 1.22  1996/11/01 19:28:22  kmurli
  Added DQL sources to include access to TDataDQL. This is equivalent to
  TDataAscii/TDataBinary. The DQL type in the Tcl/Tk corresponds to this
  class.

  Revision 1.21  1996/08/29 22:19:01  guangshu
  Added function OpendataChannel.

  Revision 1.20  1996/08/04 22:58:39  jussi
  Server unregisters a socket connection only when an existing
  connection is broken.

  Revision 1.19  1996/07/18 01:17:32  jussi
  Added call to DestroySessionData() of base class.

  Revision 1.18  1996/07/15 17:02:03  jussi
  Added support for string attributes in GData.

  Revision 1.17  1996/07/09 16:00:19  wenger
  Added master version number and compile date to C++ code (also displayed
  in the user interface); added -usage and -version command line arguments;
  updated usage message.

  Revision 1.16  1996/06/27 16:39:30  jussi
  With the new dispatching scheme in place, the server uses blocking-mode
  I/O with interrupts generated from the select() call in the dispatcher.
  The switching between server-listen and server-connected mode was
  streamlined.

  Revision 1.15  1996/06/12 14:56:27  wenger
  Added GUI and some code for saving data to templates; added preliminary
  graphical display of TDatas; you now have the option of closing a session
  in template mode without merging the template into the main data catalog;
  removed some unnecessary interdependencies among include files; updated
  the dependencies for Sun, Solaris, and HP; removed never-accessed code in
  ParseAPI.C.

  Revision 1.14  1996/05/22 21:04:29  jussi
  ControlPanel::_controlPanel is now set by main program.

  Revision 1.13  1996/05/20 18:42:28  jussi
  Merged with ClientServer library code.

  Revision 1.12  1996/05/15 19:00:15  jussi
  Return value of recv() on a socket in non-blocking mode is
  EWOULDBLOCK when there is no data, although on Intel/Solaris
  testing the return value against EAGAIN worked just fine.
  Anyway, I fixed this so that the server works on other platforms
  as well.

  Revision 1.11  1996/05/15 16:41:08  jussi
  Moved all networking code from ServerAPI.c to ClientAPI.c.
  Improved support for bracketed or non-bracketed arguments
  passed in commands.

  Revision 1.10  1996/05/14 19:18:41  jussi
  Changed memory allocation in ReadSocket(). Rather than
  malloc a chunk of memory for each element every time
  a new command arrives, new memory is allocated only
  when the number of elements increases. Also ReadSocket()
  is called successively until no more commands exist
  rather than just a single time and then returning to
  the dispatcher.

  Revision 1.9  1996/05/13 21:55:43  jussi
  Moved initialization of _mode to Control.c.

  Revision 1.8  1996/05/13 20:25:03  jussi
  Improved support for replicas. Commands are echoed to replicas
  before being executed locally. See comments in ReadSocket()
  for an explanation.

  Revision 1.7  1996/05/13 18:07:11  jussi
  Changed interpretation of the "flag" argument to Send().
  It is no longer simply an error flag but instead indicates
  the type of a message: API_CMD, API_ACK, API_NAK, or API_CTL.
  Removed control channel from client-server communication;
  all communication goes through a single socket pair.

  Revision 1.6  1996/05/11 20:44:19  jussi
  Moved output statement for "exit" command from ParseAPI.C to
  ServerAPI.c.

  Revision 1.5  1996/05/11 19:08:24  jussi
  Added replica management.

  Revision 1.4  1996/05/11 17:25:39  jussi
  Moved all command parsing to ParseAPI.C so that TkControl.c
  would not have to duplicate the code.

  Revision 1.3  1996/05/11 02:01:01  jussi
  Fixed typo.

  Revision 1.2  1996/05/11 01:53:56  jussi
  Condensed the code by removing some unnecessary Tcl/Tk parts
  which are not used by the server. Changed the client-server
  protocol somewhat.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ServerAPI.h"
#include "View.h"
#include "Util.h"
#include "Version.h"
#include "Session.h"
#include "DebugLog.h"
#include "ArgList.h"

#ifdef SUN
#include "missing.h"
#endif

//#define SERV_ANYPORT
//#define DEBUG

MapInterpClassInfo *ServerAPI::_interpProto = 0;

ControlPanel *GetNewControl()
{
  return new ServerAPI();
}

// Note: this complication is so that the Version class doesn't directly
// refer to the DebugLog class, which goofs up the devisec link.  RKW
// 1999-06-24.
static void
LogFn(char *msg)
{
  DebugLog::DefaultLog()->Message(DebugLog::LevelInfo0, msg);
}

ServerAPI::ServerAPI()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::ServerAPI()\n", this);
#endif

  Version::PrintInfo(LogFn);

  ControlPanel::_controlPanel = (ControlPanel *)this;
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);
	
  _busy = false;
  _dataFd = -1;

  _server = new DeviseServer("DEVise",
#ifdef SERV_ANYPORT
	0,
    0,
	0,
#else
	Init::ImagePort(),
	Init::SwitchPort(),
    Init::Port(),
#endif
	Init::SwitchName(),
	Init::MaxClients(),
    this);
}

ServerAPI::~ServerAPI()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::~ServerAPI()\n", this);
#endif

  Session::Close();
  delete _server;
}

void ServerAPI::DoAbort(const char *reason)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::DoAbort()\n", this);
#endif

  char errBuf[1024];
  int formatted = snprintf(errBuf, sizeof(errBuf)/sizeof(char),
    "An internal error has occurred. Reason:\n  %s\n", reason);
  checkAndTermBuf2(errBuf, formatted);
  reportErrNosys(errBuf);

  char *args[] = { "AbortProgram", (char *)reason };
  SendControl(2, args, false);
  fprintf(stderr, "Server aborts.\n");
  delete _server;
  reportErrNosys("Fatal error");//TEMP -- replace with better message
  exit(0);
}

void ServerAPI::RestartSession()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::RestartSession()\n", this);
#endif

  _server->CloseClient();
}

void ServerAPI::OpenDataChannel(int port)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::OpenDataChannel(%d)\n", this, port);
#endif

  _dataFd = socket(AF_INET, SOCK_STREAM, 0);
  if (_dataFd < 0) {
      reportErrSys("Cannot create socket in OpenDataChannel");
      return;
  }

  sockaddr_in clientAddr;
  clientAddr.sin_family = clientAddr.sin_port = clientAddr.sin_addr.s_addr = 0;
  
  if (port == 0) {
    clientAddr.sin_port = htons(DefaultDataPort); 
  } else {
    clientAddr.sin_port = htons(port);
  }
  clientAddr.sin_family = AF_INET;

  int ind;

  // Get address of the current client.
  struct sockaddr_in tmpaddr;
#if defined(LINUX) || defined(SOLARIS)
  socklen_t
#else
  int
#endif
      addrlen = sizeof(tmpaddr);
  if (getpeername(_server->CurrentClientFd(), (struct sockaddr *) &tmpaddr,
      &addrlen) < 0) {
    reportErrSys("Error in getpeername()");
  } else {
    clientAddr.sin_addr.s_addr = tmpaddr.sin_addr.s_addr;
  }

  if(clientAddr.sin_addr.s_addr > 0) {
      ind = connect(_dataFd, (struct sockaddr *)&clientAddr,
			 sizeof(struct sockaddr));
      if (ind < 0) {
	  reportErrSys("Cannot open the data channel");
	  close(_dataFd);
	  _dataFd = -1;
	  return;
      }
#ifdef DEBUG
      printf("Data channel established\n");
#endif
  } else {
      printf("Must establish the initial connection first.\n");
  }
}

void ServerAPI::SetBusy()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::SetBusy()\n", this);
#endif

  if (++_busy == 1) {
    char *args[] = { "ChangeStatus", "1" };
    SendControl(2, args, false);
  }
}

void ServerAPI::SetIdle()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::SetIdle()\n", this);
#endif

  DOASSERT(_busy > 0, "Control panel unexpectedly busy");

  if (--_busy == 0) {
    char *args[] = { "ChangeStatus", "0" };
    SendControl(2, args, false);
  }
}

Boolean ServerAPI::IsBusy()
{
  return (_busy > 0);
}

void ServerAPI::FilterChanged(View *view, const VisualFilter &filter,
				   int flushed)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::FilterChanged(%s)\n", this, view->GetName());
#endif

  DevStatus status(StatusOk);
  int formatted;

  const int bufSize = 80;
  char xLowBuf[bufSize], yLowBuf[bufSize], xHighBuf[bufSize],
      yHighBuf[bufSize];
  if (view->GetXAxisAttrType() == DateAttr) {
    formatted = snprintf(xLowBuf, bufSize, "%s", DateString(filter.xLow));
    status += checkAndTermBuf2(xLowBuf, formatted);
    formatted = snprintf(xHighBuf, bufSize, "%s", DateString(filter.xHigh));
    status += checkAndTermBuf2(xHighBuf, formatted);
  } else {
    formatted = snprintf(xLowBuf, bufSize, "%.2f", filter.xLow);
    status += checkAndTermBuf2(xLowBuf, formatted);
    formatted = snprintf(xHighBuf, bufSize, "%.2f", filter.xHigh);
    status += checkAndTermBuf2(xHighBuf, formatted);
  }
				
  if (view->GetYAxisAttrType() == DateAttr) {
    formatted = snprintf(yLowBuf, bufSize, "%s", DateString(filter.yLow));
    status += checkAndTermBuf2(yLowBuf, formatted);
    formatted = snprintf(yHighBuf, bufSize, "%s", DateString(filter.yHigh));
    status += checkAndTermBuf2(yHighBuf, formatted);
  } else {
    formatted = snprintf(yLowBuf, bufSize, "%.2f", filter.yLow);
    status += checkAndTermBuf2(yLowBuf, formatted);
    formatted = snprintf(yHighBuf, bufSize, "%.2f", filter.yHigh);
    status += checkAndTermBuf2(yHighBuf, formatted);
  }
  
  if (!status.IsComplete()) {
    reportErrNosys("Aborting ServerAPI::FilterChanged() because of "
        "insufficient buffer size!!!!!!");
    return;
  }

  const char *args[] = { "ProcessViewFilterChange", view->GetName(),
		   (flushed ? "1" : "0"), xLowBuf, yLowBuf,
		   xHighBuf, yHighBuf, "0" };
  //TEMP -- typecast here should be removed.  RKW 2000-01-13.
  SendControl(8, args, true);
}

void ServerAPI::SelectView(View *view)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::SelectView(%s)\n", this, view->GetName());
#endif

  char *args[] = { "ProcessViewSelected", view->GetName() };
  SendControl(2, args, true);
}

void
ServerAPI::ShowMouseLocation(const char *dataX, const char *dataY)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::ShowMouseLocation(%s, %s)\n", this, dataX, dataY);
#endif

  char *args[] = { "ShowMouseLocation", (char *)dataX, (char *)dataY };
  SendControl(3, args, true);
}

void ServerAPI::SyncNotify()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::SyncNotify()\n", this);
#endif

  SendControl(API_CTL, "SyncDone", false);
  ClearSyncNotify();
}

void ServerAPI::Raise()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::Raise()\n", this);
#endif

  SendControl(API_CTL, "raise .", false);
}

void
ServerAPI::NotifyFrontEnd(const char *script)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::NotifyFrontEnd(%s)\n", this, script);
#endif

  ArgList args;
  args.ParseString(script);

  SendControl(args.GetCount(), (char **)args.GetArgs(), false);
}
