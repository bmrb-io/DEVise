/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.5  1996/05/11 20:52:57  jussi
  Removed tcl.h and tk.h header file inclusion.

  Revision 1.4  1996/05/11 19:09:44  jussi
  Added replica management.

  Revision 1.3  1996/05/11 17:26:57  jussi
  Reorganized code in order to interface with ParseAPI properly.

  Revision 1.2  1996/05/11 01:53:34  jussi
  Condensed the code by removing some unnecessary Tcl/Tk parts
  which are not used by the server. Changed the client-server
  protocol somewhat.

  Revision 1.1  1996/05/09 18:14:39  kmurli
  Modified Group.C and GroupDir.C to include an oiverloaded functions for
  get_items, subitems to take in a char * instead of Tcp_interp *. This
  is for use in the ServerAPI.c
*/

#ifndef ServerAPI_h
#define ServerAPI_h

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "Dispatcher.h"
#include "Control.h"
#include "ViewCallback.h"
#include "GroupDir.h"
#include "ParseAPI.h"

class View;
class MapInterpClassInfo;

extern GroupDir *gdir;

class ServerAPI: public ControlPanel, public DispatcherCallback,
                 private ViewCallback
{
public:
  ServerAPI();

  virtual void SelectView(View *view);

  /* Get/set busy status. */
  virtual void SetBusy();
  virtual void SetIdle();
  virtual Boolean IsBusy();

  /* Start/restart session */
  virtual void StartSession() {}
  virtual void DestroySessionData();
  virtual void RestartSession();

  /* Execute script */
  virtual void ExecuteScript(char *script);

  /* Abort program */
  virtual void DoAbort(char *reason);

  /* Get GroupDir info */
  virtual GroupDir *GetGroupDir() { return gdir; }

  /* Get MapInterpClassInfo info */
  virtual MapInterpClassInfo *GetInterpProto() { return _interpProto; }

  /* Add replica server */
  virtual int AddReplica(char *hostName, int port);

  /* Remove replica server */
  virtual int RemoveReplica(char *hostName, int port);

protected:
  /* replicate API command to replica servers */
  virtual void Replicate(int argc, char **argv);

  virtual void SubclassInsertDisplay(DeviseDisplay *disp,
				     Coord x, Coord y,
				     Coord w, Coord h) {}
  virtual void SubclassRegisterView(View *view) {}
  virtual void SubclassUnregisterView(View *view) {}
  virtual void SubclassDoInit() {}
  virtual void SubclassDoViewPerspectiveChanged(View *view,
						Coord x, Coord y,
						Coord w, Coord h) {}

private:
  virtual void FilterAboutToChange(View *view) {}
  virtual void FilterChanged(View *view, VisualFilter &filter, int flushed);
  virtual void ViewCreated(View *view) {}
  virtual void ViewDestroyed(View *view) {}

  char *DispatchedName() { return "ServerAPI"; }
  virtual void Run();

  int _busy;
  static MapInterpClassInfo *_interpProto;

  int GotoConnectedMode();
  int ReadSocket();
  int NonBlockingMode(int fd);
  int BlockingMode(int fd);

  virtual int Send(int fd, u_short flag, int bracket,
		   int argc, char **argv);
  virtual int ReturnVal(u_short flag, char *result) {
    return Send(_socketFd, flag, 0, 1, &result);
  }
  virtual int ReturnVal(int argc, char **argv) {
    return Send(_socketFd, API_ACK, 1, argc, argv);
  }
  virtual int SendControl(u_short flag, char *result) {
    return Send(_socketFd, flag, 0, 1, &result);
  }
  virtual int SendControl(int argc, char **argv) {
    return Send(_socketFd, API_ACK, 1, argc, argv);
  }

  int _listenFd;                        // socket for listening for clients

  int _socketFd;                        // socket for client connection
  struct sockaddr_in _client_addr;      // address of client

  int _replicate;                       // number of servers to replicate to
  const int _maxReplicas = 10;
  struct ReplicaServer {
    char *host;
    int port;
    int fd;
  } _replicas[_maxReplicas];
};

const int DefaultDevisePort = 6100;

#endif
