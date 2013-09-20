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
  Declaration of TkClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.2.70.1  2013/06/14 14:49:06  wenger
  More cleanups from CentOS6 compile...

  Revision 1.2  1998/01/07 19:27:54  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:26  wenger
  deviseb now uses client/server library.

 */

#ifndef _TkClient_h_
#define _TkClient_h_

#include <tk.h>

#include "TclClient.h"

class TkClient : public TclClient {
 public:
  TkClient(const char *name, const char *hostname, int port,
    const char *script);
  virtual ~TkClient();

  virtual void MainLoop();              // main loop of client

 protected:
  Tk_Window _mainWindow;                // Tcl main window
  char *_script;                        // name of Tcl script
};

#endif
