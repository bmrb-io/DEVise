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
  Implementation of DeviseServer class.
 */

/*
  $Id$

  $Log$
  Revision 1.11  1998/09/30 17:44:43  wenger
  Fixed bug 399 (problems with parsing of UNIXFILE data sources); fixed
  bug 401 (improper saving of window positions).

  Revision 1.10  1998/09/10 23:25:10  wenger
  Added more error reporting.

  Revision 1.9  1998/08/13 18:14:45  wenger
  More updates to JavaScreen support.

  Revision 1.8  1998/05/02 09:00:43  taodb
  Added support for JAVA Screen and command logging

  Revision 1.7  1998/03/02 22:03:41  taodb
  Add control parameter to Run() invocations

  Revision 1.6  1998/02/26 20:48:39  taodb
  Replaced ParseAPI() with Command Object Interface

  Revision 1.5  1998/02/12 17:16:12  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.4  1998/02/03 23:46:32  wenger
  Fixed a problem Hongyu had with getting GData on socket; fixed bugs
  283 and 285 (resulted from problems in color manager merge);
  conditionaled out some debug output.

  Revision 1.3  1998/01/30 02:16:59  wenger
  Merged cleanup_1_4_7_br_7 thru cleanup_1_4_7_br_8.

  Revision 1.2.2.1  1998/01/28 22:43:44  taodb
  Added support for group communicatoin

  Revision 1.2  1998/01/07 19:28:56  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.2  1998/01/16 23:41:38  wenger
  Fixed some problems that Tony found with the client/server communication
  and GIF generation; fixed problem that session files specified on the
  command line were still opened by the Tcl code.

  Revision 1.1.2.1  1998/01/07 15:59:42  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/uio.h>
#include <errno.h>

#include "DeviseServer.h"
#include "DevError.h"
#include "Scheduler.h"
#include "CmdContainer.h"
#include "CmdDescriptor.h"
#include "Csprotocols.h"
#include "Session.h"
#include "DebugLog.h"

//#define DEBUG
#define DEBUG_LOG

/*------------------------------------------------------------------------------
 * function: DeviseServer::DeviseServer
 * Constructor.
 */
DeviseServer::DeviseServer(char *name,int image_port,int swt_port, 
	int clnt_port, 
	char* switchname, int maxclients, ControlPanel *control) :
  	Server(name,image_port, swt_port, clnt_port,switchname, maxclients)
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::DeviseServer(%s,%d, %d, %d)\n", this, name, 
	image_port, swt_port, clnt_port);
#endif

  cmdContainerp = new CmdContainer(control,CmdContainer::CSGROUP, this);
  _currentClient = CLIENT_INVALID;
  _previousClient = CLIENT_INVALID;
  _control = control;

  _clientDispIDs = new DispatcherID[_maxClients];
  int client;
  for (client = 0; client < _maxClients; client++) {
    _clientDispIDs[client] = NULL;
  }

  // Note: we have to do WaitForConnection(), or _listenFd isn't valid.
  WaitForConnection();
  (void) Dispatcher::Current()->Register(this, 10, AllState, true, _listenFd);
  (void) Dispatcher::Current()->Register(this, 10, AllState, true, 
	_listenImageFd);
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::~DeviseServer
 * Destructor.
 */
DeviseServer::~DeviseServer()
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::~DeviseServer()\n", this);
#endif

  int client;
  for (client = 0; client < _maxClients; client++) {
    if (_clientDispIDs[client] != NULL) {
      Dispatcher::Current()->Unregister(_clientDispIDs[client]);
    }
  }

  delete [] _clientDispIDs;

  Dispatcher::Current()->Unregister(this);
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::Run
 * Called by Dispatcher when there is something for this object to do
 * (a command from a client).
 */
void
DeviseServer::Run()
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::Run()\n", this);
#endif

  if (_numClients < 1) {
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message("Before WaitForConnection()\n");
#endif
    WaitForConnection();
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message("After WaitForConnection()\n");
#endif
    if (_numClients < 1) {
      reportErrNosys("No client connected");
    }
  } else {
    // Note: SingleStep() receives a command and/or sets up a new connection.
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message("Before SingleStep()\n");
#endif
    SingleStep();
#if defined(DEBUG_LOG)
    DebugLog::DefaultLog()->Message("After SingleStep()\n");
#endif
  }
}

void 
DeviseServer::RunCmd(int argc, char** argv, CmdDescriptor& cmdDes)
{
	cmdContainerp->Run(argc, argv, _control, cmdDes);
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::WaitForConnection
 * Waits for a client connection.
 */
void
DeviseServer::WaitForConnection()
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::WaitForConnection()\n", this);
#endif

  Server::WaitForConnection();
}

void
DeviseServer::CloseImageConnection()
{
	if (_currentClient != CLIENT_INVALID)
	{
		Server::CloseImageConnection(_currentClient);
	}
	else
	{
    	reportErrNosys("No active client to close image connection");
	}
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::CloseClient
 * Closes a client connection (of the current client or the one that
 * most recently executed a command).
 */
void
DeviseServer::CloseClient()
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::CloseClient()\n", this);
#endif

  if (_currentClient != CLIENT_INVALID) {
    Server::CloseClient(_currentClient);
  } else if (_previousClient != CLIENT_INVALID) {
    Server::CloseClient(_previousClient);
  } else {
    reportErrNosys("No client to close");
  }
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::CurrentClientFd
 * Closes a client connection (of the current client or the one that
 * most recently executed a command).
 */
int
DeviseServer::CurrentClientFd()
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::CurrentClientFd()\n", this);
#endif

  int result;

  if (_currentClient != CLIENT_INVALID) {
    result = _clients[_currentClient].fd;
  } else if (_previousClient != CLIENT_INVALID) {
    result = _clients[_previousClient].fd;
  } else {
    reportErrNosys("No client to connect data socket to");
    result = -1;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::BeginConnection
 * Begin the connection for a given client.
 */
void
DeviseServer::BeginConnection(ClientID clientID)
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::BeginConnection(%d)\n", this, clientID);
#endif

  // Register with the dispatcher to get called when anything comes in on
  // the appropriate file descriptor.
  _clientDispIDs[clientID] = Dispatcher::Current()->Register(this, 10, AllState,
      true, _clients[clientID].fd);
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::EndConnection
 * End the connection for a given client.
 */
void
DeviseServer::EndConnection(ClientID clientID)
{
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::EndConnection(%d)\n", this, clientID);
  printf("  _numClients = %d\n", _numClients);
#endif

  // We need to unregister by DispatcherID rather than by the address of
  // this object, because we may have registered this object for more than
  // one file descriptor.
  Dispatcher::Current()->Unregister(_clientDispIDs[clientID]);
  _clientDispIDs[clientID] = NULL;

  // Note: Server class decrements _numClients *after* this function is
  // called.
  if (_numClients <= 1) {
    Session::Close();
  }

  if (_currentClient == clientID) _currentClient = CLIENT_INVALID;
  if (_previousClient == clientID) _previousClient = CLIENT_INVALID;
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::WriteImagePort
 * Write Image into current client's image port, only apply to JAVA_SCREEN
 */
int 
DeviseServer::WriteImagePort(const void* buf, int nsize)
{
#if defined(DEBUG)
    printf("DeviseServer::WriteImagePort(%d)\n", nsize);
	printf("  _currentClient = %d\n", _currentClient);
#endif

	if (_currentClient == CLIENT_INVALID)
	{
		fprintf(stderr, "No client was specified\n");
		return -1;
	}
	if (_clients[_currentClient].ctype != ClientInfo::JAVA_SCREEN)
	{
		fprintf(stderr, "Not JAVA_SCREEN client\n");
		return -1;
	}
	if (_clients[_currentClient].imagefd <0)
	{
		fprintf(stderr, "Invalid image port file descriptor\n");
		return -1;
	}

	int	 nbytes =-1;
	char tempbuf[IMG_OOB];	
	bool stopped = false; 
	do {
#if 0
		// prvoide client-controlled stopping functions
		do{
			nbytes = recv(_clients[_currentClient].imagefd, tempbuf,IMG_OOB, 0);
			if ((nbytes >0)&&(!stopped))
				stopped = true;
		}while (nbytes >0);
		if (stopped)
		{
			printf("Stop requested from the client\n");
			nbytes = -1;
			break;
		}
		if ((nbytes!=-1 )||(errno != EWOULDBLOCK))
		{
			perror("Image connection error:");
			nbytes = -1;
			break;
		}
#endif
		nbytes = write(_clients[_currentClient].imagefd, buf, nsize);
	} while ((nbytes == -1) && ((errno == EAGAIN)||(errno==EINTR)));
	if (nbytes != nsize) {
		reportErrSys("Writing to image port");
	}
	return nbytes;
}

/*------------------------------------------------------------------------------
 * function: DeviseServer::ProcessCmd
 * Process a command from a client.
 */
void
DeviseServer::ProcessCmd(ClientID clientID, int argc, char **argv)
{
	cerr << "******I should not come here "<<endl;
/*
#if defined(DEBUG)
  printf("DeviseServer(0x%p)::ProcessCmd(%d)\n", this, clientID);
  printf("  Command: ");
  for (int num = 0; num < argc; num++) {
    printf("<%s> ", argv[num]);
  }
  printf("\n");
#endif

#if defined(DEBUG)
  printf("  _currentClient = %d\n", _currentClient);
#endif
  

  if (_currentClient == CLIENT_INVALID) {
    // Not currently processing a command.
    _currentClient = clientID;
#if defined(DEBUG)
  printf("  Before cmdContainer call: _currentClient = %d\n", _currentClient);
#endif

    if (cmdContainerp->RunOneCommand(argc, argv,_control) < 0) {
      	char errBuf[1024];
      	sprintf(errBuf, "Devise API command error (command %s).", argv[0]);
      	reportErrNosys(errBuf);
    }
	_currentClient = CLIENT_INVALID;
  } else {
    // Currently processing another command.
    reportErrNosys("Command while server is busy");
    (void) ReturnVal(clientID, (u_short)API_NAK, "Server is busy");
  }

#if defined(DEBUG)
  printf("  After cmdContainer call: _currentClient = %d\n", _currentClient);
#endif
*/
}

/*============================================================================*/
