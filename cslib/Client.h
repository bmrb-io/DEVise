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
  Declaration of Client (and CompDate) classes.
 */

/*
  $Id$

  $Log$
  Revision 1.2.28.1  2005/09/28 22:29:28  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.2  1998/01/07 19:27:46  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:21  wenger
  deviseb now uses client/server library.

 */

#ifndef _Client_h_
#define _Client_h_

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
  virtual int ServerCmd(int argc,		// send command to server
			const char * const *argv);
  virtual void ReadServer();            // receive command from server
  virtual void ControlCmd(int argc,
			  const char * const *argv); // process server command
  
  char *_name;                          // name of client
  char *_hostname;                      // hostname of server
  int _port;                            // port number of server
  int _serverFd;                        // fd of server socket
  char *_result;			// result of command
  char *_cmd;				// command from server
};

#endif
