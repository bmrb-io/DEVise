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
  Revision 1.3.22.1  2013/06/14 14:48:58  wenger
  More cleanups from CentOS6 compile...

  Revision 1.3  2005/12/06 20:01:11  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.2.26.1  2005/09/28 22:29:21  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.2  1998/02/12 17:14:41  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.1.2.1  1998/01/28 22:42:23  taodb
  Added support for group communicatoin

 */

#ifndef _TclClient_h_
#define _TclClient_h_

#include <tcl/tcl.h>

#include "Client.h"

class TclClient : public Client {
public:
	TclClient(const char *name, const char *hostname, int port,
	  const char* initStr);
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
