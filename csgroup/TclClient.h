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
  Revision 1.1.2.1  1998/01/28 22:42:23  taodb
  Added support for group communicatoin

 */

#ifndef _TclClient_h_
#define _TclClient_h_

#include <tcl.h>

#include "Client.h"

class TclClient : public Client {
public:
	TclClient(char *name, char *hostname, int port, char* initStr);
	virtual ~TclClient();

	virtual int ServerCmd(int argc,
			char **argv);   	// send Tcl command to server
	virtual void ReadServer();	// receive command from server
	virtual void ControlCmd(int argc,
			  char **argv);		// process server command

	// group control commands
	virtual int	GeneralGroupCmd(int argc, char**argv){return 0;};
								// general group-related commands
protected:
	Tcl_Interp *_interp;			// Tcl interpreter
};

#endif
