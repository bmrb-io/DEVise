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
#include "QueryProc.h"
#include "ParseCat.h"
#include "View.h"
#include "Util.h"
#include "Version.h"
#include "StringStorage.h"
#include "DCE.h"

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

ServerAPI::ServerAPI()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::ServerAPI()\n", this);
#endif

  Version::PrintInfo();

  ControlPanel::_controlPanel = (ControlPanel *)this;
  _interpProto = new MapInterpClassInfo();
  
  View::InsertViewCallback(this);
	
  _busy = false;
  _dataFd = -1;

  if (!Init::UseSharedMem()) {
    fprintf(stderr, "Proceeding without shared memory and semaphores.\n");
  }

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

  DestroySessionData();
  delete _server;
}

void ServerAPI::DoAbort(char *reason)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::DoAbort()\n", this);
#endif

  fprintf(stderr, "An internal error has occurred. Reason:\n  %s\n", reason);
  char *args[] = { "AbortProgram", reason };
  SendControl(2, args, false);
  fprintf(stderr, "Server aborts.\n");
  delete _server;
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
  int addrlen = sizeof(tmpaddr);
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

void ServerAPI::FilterChanged(View *view, VisualFilter &filter,
				   int flushed)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::FilterChanged(%s)\n", this, view->GetName());
#endif

  char xLowBuf[80], yLowBuf[80], xHighBuf[80], yHighBuf[80];
  if (view->GetXAxisAttrType() == DateAttr) {
    sprintf(xLowBuf, "%s", DateString(filter.xLow));
    sprintf(xHighBuf, "%s", DateString(filter.xHigh));
  } else {
    sprintf(xLowBuf, "%.2f", filter.xLow);
    sprintf(xHighBuf, "%.2f", filter.xHigh);
  }
				
  if (view->GetYAxisAttrType() == DateAttr) {
    sprintf(yLowBuf, "%s", DateString(filter.yLow));
    sprintf(yHighBuf, "%s", DateString(filter.yHigh));
  } else {
    sprintf(yLowBuf, "%.2f", filter.yLow);
    sprintf(yHighBuf, "%.2f", filter.yHigh);
  }
  
  char *args[] = { "ProcessViewFilterChange", view->GetName(),
		   (flushed ? "1" : "0"), xLowBuf, yLowBuf,
		   xHighBuf, yHighBuf, "0" };
  SendControl(8, args, true);

  char *rep[] = { "setFilter", view->GetName(), xLowBuf,
		  yLowBuf, xHighBuf, yHighBuf };
}

void ServerAPI::SelectView(View *view)
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::SelectView(%s)\n", this, view->GetName());
#endif

  char *args[] = { "ProcessViewSelected", view->GetName() };
  SendControl(2, args, true);
}

void ServerAPI::SyncNotify()
{
#if defined(DEBUG)
  printf("ServerAPI(0x%p)::SyncNotify()\n", this);
#endif

  SendControl(API_CTL, "SyncDone", false);
  ClearSyncNotify();
}
