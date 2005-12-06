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
  Declaration of TclClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.2.28.1  2005/09/28 22:29:28  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.2  1998/01/07 19:27:52  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:25  wenger
  deviseb now uses client/server library.

 */

#ifndef _TclClient_h_
#define _TclClient_h_

#include <tcl.h>

#include "Client.h"

class TclClient : public Client {
 public:
  TclClient(char *name, char *hostname, int port);
  virtual ~TclClient();

  virtual int ServerCmd(int argc,
			const char * const *argv);   // send Tcl command to server
  virtual void ReadServer();            // receive command from server
  virtual void ControlCmd(int argc,		// process server command
			  const char * const *argv);
  
 protected:
  Tcl_Interp *_interp;                  // Tcl interpreter
};

#endif
