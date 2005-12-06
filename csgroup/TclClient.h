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
  Revision 1.2.26.1  2005/09/28 22:29:21  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.2  1998/02/12 17:14:41  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

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
			const char * const *argv); // send Tcl command to server
	virtual void ReadServer();	// receive command from server
	virtual void ControlCmd(int argc,	// process server command
			  const char * const *argv);

	// group control commands
	virtual int	GeneralGroupCmd(int argc, const char* const *argv)
	  {return 0;};
								// general group-related commands
protected:
	Tcl_Interp *_interp;			// Tcl interpreter
};

#endif
