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
  Revision 1.4  1997/05/21 21:05:10  andyt
  Support for multiple clients in client-server library. Single-client mode
  still supported by compiling with -DSINGLE_CLIENT. Client-server library
  can now display EmbeddedTk windows and Tasvir images. Added a sample
  Embedded-Tk script and GIF image to the repository (ETkSample.tcl,
  earth.gif). Modified tarcslib script to include all new files.

  Revision 1.3  1996/11/23 00:24:01  wenger
  Incorporated all of the PostScript-related stuff into the client/server
  library; added printing to PostScript to the example client and server;
  made some fixes to PSDisplay, PSWindowRep, and XWindowRep classes as
  a result of testing with client/server stuff.

  Revision 1.2  1996/10/18 15:19:44  jussi
  Added CompDate class.

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

//-----------------------------------------------------------------------
// BEGIN Server class
//-----------------------------------------------------------------------

#if !defined(SINGLE_CLIENT)
//
// Each Server can serve multiple clients. Client state is stored
// in a ClientInfo object. A Server instance will have an array of
// ClientInfo objects, indexed by ClientID values.
//
class ClientInfo
{
  public:
    ClientInfo()
    : fd(-1), valid(false)
    {}
    int fd;
    bool valid;
};
typedef int ClientID;
#endif

class Server {

  public:

#if defined(SINGLE_CLIENT)
    Server(char *name, int port);
#else
    Server(char *name, int port, int maxClients = 10);
#endif
    virtual ~Server();
    virtual void MainLoop();              // main loop of server
    virtual void SingleStep();            // once throught the body of MainLoop

  protected:
    virtual void DoAbort(char *reason);   // print error message and abort
    virtual void WaitForConnection();     // wait for client connection
    virtual void ReadCmd();               // read client command
    char *_name;                          // name of server
    int _port;                            // port number of server
    int _listenFd;                        // fd of listen socket
  private:
    void InitializeListenFd();             // initialize the fd for listening
    
#if defined(SINGLE_CLIENT)
  protected:
    virtual void BeginConnection() {}     // executed when connection starts
    virtual void EndConnection() {}       // executed when connection ends
    virtual void ProcessCmd(int argc,
			    char **argv); // process client command
    virtual void CloseClient();           // close client connection
    int _clientFd;                        // fd of client socket
#else
  protected:
    virtual void BeginConnection(ClientID)
    {}                                     // executed when a connection starts
    virtual void EndConnection(ClientID)
    {}                                     // executed when a connection ends
    virtual void ExecClientCmds(fd_set *); // run incoming client commands
    virtual void ProcessCmd(ClientID,
			    int argc,
			    char **argv);  // process a single client command
    virtual void CloseClient(ClientID);    // close client connection
    ClientInfo *_clients;                  // array of ClientInfo structs
    int _numClients;                       // current number of clients
    int _maxClients;                       // maximum number of clients
  private:
    int FindIdleClientSlot();              // search for slot in client array
#endif
};

class WinServer : public Server, public WindowRepCallback {

  public:

#if defined(SINGLE_CLIENT)
    WinServer(char *name, int port);
#else
    WinServer(char *name, int port, int maxClients = 10);
#endif
    virtual ~WinServer();
    virtual void MainLoop();              // main loop of server
    virtual void SingleStep();            // once throught the body of MainLoop

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
    
    DeviseDisplay *_screenDisp;            // display to draw to screen
    DeviseDisplay *_fileDisp;              // display to draw to file

};

#endif
