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
  Revision 1.1  1996/10/17 20:42:17  jussi
  Initial revision.
*/

#ifndef ClientServer_h
#define ClientServer_h

#include <tcl.h>
#include <tk.h>

#include "ClientAPI.h"
#include "Display.h"
#include "WindowRep.h"

class CompDate {
public:
  static char *Get();
};

class Client {
 public:
  Client(char *name, char *hostname, int port);
  virtual ~Client();

  virtual void MainLoop() = 0;          // main loop of client

 protected:
  virtual void DoAbort(char *reason);   // print error message and abort
  virtual int ServerCmd(int argc,
			char **argv);   // send command to server
  virtual void ReadServer();            // receive command from server
  virtual void ControlCmd(int argc,
			  char **argv); // process server command
  
  char *_name;                          // name of client
  char *_hostname;                      // hostname of server
  int _port;                            // port number of server
  int _serverFd;                        // fd of server socket
};

class TclClient : public Client {
 public:
  TclClient(char *name, char *hostname, int port, char *script);
  virtual ~TclClient();

  virtual void MainLoop();              // main loop of client
  virtual int ServerCmd(int argc,
			char **argv);   // send Tcl command to server
  virtual void ReadServer();            // receive command from server
  virtual void ControlCmd(int argc,
			  char **argv); // process server command
  
 protected:
  char *_script;                        // name of Tcl script
  Tcl_Interp *_interp;                  // Tcl interpreter
  Tk_Window _mainWindow;                // Tcl main window
};

class Server {
 public:
  Server(char *name, int port);
  virtual ~Server();

  virtual void MainLoop();              // main loop of server

 protected:
  virtual void DoAbort(char *reason);   // print error message and abort
  virtual void WaitForConnection();     // wait for client connection
  virtual void BeginConnection() {}     // executed when connection starts
  virtual void EndConnection() {}       // executed when connection ends
  virtual void ReadCmd();               // read client command
  virtual void ProcessCmd(int argc,
			  char **argv); // process client command
  virtual void CloseClient();           // close client connection

  char *_name;                          // name of server
  int _port;                            // port number of server
  int _listenFd;                        // fd of listen socket
  int _clientFd;                        // fd of client socket
};

class WinServer : public Server, public WindowRepCallback {
 public:
  WinServer(char *name, int port);
  virtual ~WinServer();

  virtual void MainLoop();              // main loop of server

 protected:
  /* Draw in the exposed area */
  virtual void HandleExpose(WindowRep *w, int x, int y,
			    unsigned width, unsigned height) {}

#ifdef RAWMOUSEEVENTS
  /* Handle button event */
  virtual void HandleButton(WindowRep *w, int x, int y,
			    int button, int state, int type) {}
#else
  /* Handle button press event */
  virtual void HandlePress(WindowRep *w, int xlow,
			   int ylow, int xhigh, int yhigh,
			   int button) {}
#endif

  /* Handle resize */
  virtual void HandleResize(WindowRep *w, int xlow,
			    int ylow, unsigned width,
			    unsigned height) {}

  /* Handle keypress */
  virtual void HandleKey(WindowRep *w, char key, int x, int y) {}

#ifndef RAWMOUSEEVENTS
  /* Handle pop-up */
  virtual Boolean HandlePopUp(WindowRep *w, int x, int y, int button,
			      char **&msgs, int &numMsgs) {
    return false;
  }
#endif

  /* Handle map/unmap info.
     mapped : means window has been mapped.
     unmapped: means window has been unmapped. (This can also mean
     that window has been iconified) */
  virtual void HandleWindowMappedInfo(WindowRep *w, Boolean mapped) {}

  DeviseDisplay *_display;              // display
};

#endif
