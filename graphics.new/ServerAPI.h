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
  Revision 1.1  1996/05/09 18:14:39  kmurli
  Modified Group.C and GroupDir.C to include an oiverloaded functions for
  get_items, subitems to take in a char * instead of Tcp_interp *. This
  is for use in the ServerAPI.c
*/

#ifndef TkControl_h
#define TkControl_h

#include <tk.h>
#include <tcl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "Dispatcher.h"
#include "Control.h"
#include "ViewCallback.h"

#ifdef SUN
#include "missing.h"
#endif

enum FLAG { ERROR, OK };

class View;
class MapInterpClassInfo;

class ServerAPI: public ControlPanel, public DispatcherCallback,
                 private ViewCallback
{
public:
  ServerAPI();

  virtual void SelectView(View *view);

  /* Get current mode */
  virtual Mode GetMode();

  /* Set busy status, should be called in pairs. */
  virtual void SetBusy();
  virtual void SetIdle();

  /* Get current busy status */
  virtual Boolean IsBusy();

  virtual void StartSession();

  /* Execute script */
  virtual void ExecuteScript(char *script);

  /* Abort program */
  virtual void DoAbort(char *reason);

protected:
  virtual void SubclassInsertDisplay(DeviseDisplay *disp,
				     Coord x, Coord y,
				     Coord w, Coord h);
  virtual void SubclassRegisterView(View *view);
  virtual void SubclassUnregisterView(View *view);
  virtual void SubclassDoInit();
  virtual void SubclassDoViewPerspectiveChanged(View *view,
						Coord x, Coord y,
						Coord w, Coord h);

  int ReadSocket();
  int GotoConnectedMode();
  void GotoDisconnectedMode();
  void DestroyClientData();

  int _listenFd;                        // socket for listening for clients

  int _socketFd;                        // socket for receiving commands
  int _controlFd;                       // socket for back channel
  struct sockaddr_in _client_addr;      // address of client

  int _busy;                  /* >0 if system is busy processing queries */

private:
  virtual void FilterAboutToChange(View *view) {}
  virtual void FilterChanged(View *view, VisualFilter &filter,
			     int flushed);
  virtual void ViewCreated(View *view);
  virtual void ViewDestroyed(View *view);

  virtual int ControlCmd(int argc, char *argv[]);

  char *DispatchedName() {
    return "ServerAPI";
  }
  virtual void Run();

  static ControlPanel::Mode _mode;
  static MapInterpClassInfo *_interpProto; /* proto interpreted mapping */

  virtual int Send(int fd, enum FLAG flag, int bracket,
			 int argc, char **argv);
  virtual int SendClient(enum FLAG flag, char *result) {
    return Send(_socketFd, flag, 0, 1, &result);
  }
  virtual int SendClient(int argc, char **argv) {
    return Send(_socketFd, OK, 1, argc, argv);
  }
  virtual int SendControl(enum FLAG flag, char *result) {
    if (_controlFd < 0)
      return 1;
    return Send(_controlFd, flag, 0, 1, &result);
  }
  virtual int SendControl(int argc, char **argv) {
    if (_controlFd < 0)
      return 1;
    return Send(_controlFd, OK, 1, argc, argv);
  }

  const int SERV_PORT_NUM = 6100;
};

#endif
