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
  Revision 1.1.2.1  1997/12/06 17:43:00  wenger
  Split libcs.a into libdevcs.a and libdevwin.a in preparation for
  incorporating client/server part into DEVise.

 */

#define _WinServer_C_

#include "WinServer.h"

//-----------------------------------------------------------------------
// BEGIN WinServer class
//-----------------------------------------------------------------------

#if defined(SINGLE_CLIENT)
WinServer::WinServer(char *name, int port) : Server(name, port)
#else
WinServer::WinServer(char *name, int port, int maxClients)
: Server(name, port, maxClients)
#endif
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
#if defined(SINGLE_CLIENT)
    FD_SET(_clientFd, &fdset);
    maxFdCheck = (wfd > _clientFd ? wfd : _clientFd);
#else
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
#endif
    //
    // select()
    //
    numFds = select(maxFdCheck + 1, &fdset, 0, 0, 0);
    if (numFds < 0)
    {
	perror("select");
    }
    DOASSERT(numFds > 0, "Internal error");
#if defined(SINGLE_CLIENT)
    //
    // Process the client command
    //
    if (FD_ISSET(_clientFd, &fdset))
    {
	ReadCmd();
    }
#else
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
#endif
    //
    // Process window events
    //
    if (FD_ISSET(wfd, &fdset))
    {
        _screenDisp->InternalProcessing();
    }
}

/*============================================================================*/
