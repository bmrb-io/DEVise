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
			char **argv);   // send Tcl command to server
  virtual void ReadServer();            // receive command from server
  virtual void ControlCmd(int argc,
			  char **argv); // process server command
  
 protected:
  Tcl_Interp *_interp;                  // Tcl interpreter
};

#endif
