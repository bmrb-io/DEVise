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
  Declaration of DeviseServer class.

  Note: DeviseServer class currently allows only one client, although
  the client/server library supports multiple clients.  The DeviseServer
  class could be changed to support multiple clients when and if the rest
  of DEVise is changed in such a way that this makes sense.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  1998/01/07 15:59:43  wenger
  Removed replica cababilities (since this will be replaced by collaboration
  library); integrated cslib into DEVise server; commented out references to
  Layout Manager in Tcl/Tk code; changed Dispatcher to allow the same object
  to be registered and unregistered for different file descriptors (needed
  for multiple clients); added command line argument to specify port that
  server listens on.

 */

#ifndef _DeviseServer_h_
#define _DeviseServer_h_

#include "Server.h"
#include "Control.h"
#include "Dispatcher.h"

#define CLIENT_INVALID (-1)

class DeviseServer : public DispatcherCallback, public Server {
public:
  DeviseServer(char *name, int port, ControlPanel *control);
  virtual ~DeviseServer();

  virtual char *DispatchedName() { return "DeviseServer"; }
  virtual void Run();

  virtual int ReturnVal(u_short flag, char *result) {
    return ReturnVal(flag, 1, &result, false); }
  virtual int ReturnVal(int argc, char **argv) {
    return ReturnVal(API_ACK, argc, argv, true); }

  virtual void WaitForConnection();

  virtual int CurrentClientFd() { return _currentClient != CLIENT_INVALID ?
      _clients[_currentClient].fd : CLIENT_INVALID; }

protected:
  virtual void BeginConnection(ClientID clientID);
  virtual void EndConnection(ClientID clientID);

  virtual void ProcessCmd(ClientID,	// process a single client command
      int argc, char **argv);
  virtual int ReturnVal(u_short flag, int argc, char **argv,
      Boolean addBraces = true); 

private:
  ClientID _currentClient;
  ControlPanel *_control;
  DispatcherID *_clientDispIDs;
};

#endif /* _DeviseServer_h_ */

/*============================================================================*/
