/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of WinServer class.
 */

/*
  $Id$

  $Log$
  Revision 1.3  1998/01/07 19:27:54  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.2  1997/12/08 18:17:58  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_4 tag
  into the trunk (split of libcs into libdevcs and libdevwin).

  Revision 1.1.2.5  1998/01/09 16:33:38  wenger
  Updated copyright date and version number; minor mods to compile for
  hp and sun; fixed problem with _batchMode flag getting improperly
  reset in the ControlPanel class (prevented using pixmaps instead of
  X windows).

  Revision 1.1.2.4  1998/01/07 15:59:00  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

  Revision 1.1.2.3  1997/12/30 16:47:56  wenger
  Removed single-client compile option to simplify the code.

  Revision 1.1.2.2  1997/12/17 17:30:11  wenger
  Got cslib to compile for Linux.

  Revision 1.1.2.1  1997/12/06 17:43:00  wenger
  Split libcs.a into libdevcs.a and libdevwin.a in preparation for
  incorporating client/server part into DEVise.

 */

#define _WinServer_C_

#include <sys/param.h>

#include "WinServer.h"
#include "machdep.h"

#if defined(LINUX)
#include <sys/time.h>
#endif

//-----------------------------------------------------------------------
// BEGIN WinServer class
//-----------------------------------------------------------------------

WinServer::WinServer(char *name, int port, int maxClients)
: Server(name, port, maxClients)
{
  _screenDisp = DeviseDisplay::DefaultDisplay();
  _fileDisp = DeviseDisplay::GetPSDisplay();
  DOASSERT(_screenDisp, "Out of memory");
  DOASSERT(_fileDisp, "Out of memory");
}

WinServer::~WinServer()
{
  delete _screenDisp;
  delete _fileDisp;
}

void WinServer::SingleStep()
{
    int wfd;
    fd_set fdset;
    int maxFdCheck;
    int numFds;

    //
    // Initialize the fd set
    //
    memset(&fdset, 0, sizeof fdset);
    wfd = _screenDisp->fd();
    FD_SET(wfd, &fdset);
    FD_SET(_listenFd, &fdset);
    maxFdCheck = (wfd > _listenFd ? wfd :_listenFd);
    for (int i = 0; i < _maxClients; i++)
    {
	if (_clients[i].valid)
	{
	    FD_SET(_clients[i].fd, &fdset);
	    if (_clients[i].fd > maxFdCheck)
	    {
		maxFdCheck = _clients[i].fd;
	    }
	}
    }
    //
    // select()
    //
    numFds = select(maxFdCheck + 1,
#if defined(HPUX)
        (int *) &fdset, (int *) 0, (int *) 0,
#else
        &fdset, 0, 0,
#endif
        0);
    if (numFds < 0)
    {
        char errBuf[MAXPATHLEN + 256];
        sprintf(errBuf, "select() failed at %s: %d", __FILE__, __LINE__);
        perror(errBuf);
    }
    DOASSERT(numFds > 0, "Internal error");
    //
    // Handle a new connection request
    //
    if (FD_ISSET(_listenFd, &fdset))
    {
	WaitForConnection();
    }
    //
    // Process commands on all client fds
    //
    ExecClientCmds(&fdset);
    //
    // Process window events
    //
    if (FD_ISSET(wfd, &fdset))
    {
        _screenDisp->InternalProcessing();
    }
}

/*============================================================================*/
