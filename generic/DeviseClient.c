/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2002
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DeviseClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.4.22.1  2002/05/27 18:15:47  wenger
  Got DEVise to compile with gcc 2.96 (so I can compile it at NRG).

  Revision 1.4  1998/08/18 15:22:05  wenger
  Found and fixed bug 384 (devisec not quitting).

  Revision 1.3  1998/02/12 17:15:38  wenger
  Merged through collab_br_2; updated version number to 1.5.1.

  Revision 1.2.2.1  1998/01/28 22:43:16  taodb
  Added support for group communicatoin

  Revision 1.2  1998/01/07 19:28:13  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.2  1997/12/09 19:03:36  wenger
  deviseb now uses client/server library.

  Revision 1.1.2.1  1997/12/09 16:03:45  wenger
  Devise client now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DeviseClient.h"

//#define DEBUG

/*------------------------------------------------------------------------------
 * function: DeviseClient::DeviseClient
 * Constructor.
 */
DeviseClient::DeviseClient(char *name, char *hostname, int port,
    Boolean createWindow, char* initStr) :
  TclClient(name, hostname, port, initStr)
{
#if defined(DEBUG)
  printf("DeviseClient(0x%p)::DeviseClient(%s, %s, %d)\n", this, name,
      hostname, port);
#endif

  _syncDone = false;

  if (createWindow) {
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION == 0
    _mainWindow = Tk_CreateMainWindow(_interp, NULL, "DEVise", "DEVise");
    if (!_mainWindow) {
      fprintf(stderr, "%s\n", _interp->result);
      exit(1);
    }
    Tk_MoveWindow(_mainWindow, 0, 0);
    Tk_GeometryRequest(_mainWindow, 100, 200);
#endif
    if (Tk_Init(_interp) == TCL_ERROR) {
      fprintf(stderr, "Cannot initialize Tk.\n");
      exit(1);
    }
#if TK_MAJOR_VERSION == 4 && TK_MINOR_VERSION > 0
    _mainWindow = Tk_MainWindow(_interp);
#endif
  }
}

/*------------------------------------------------------------------------------
 * function: DeviseClient::~DeviseClient
 * Destructor.
 */
DeviseClient::~DeviseClient()
{
#if defined(DEBUG)
  printf("DeviseClient(0x%p)::~DeviseClient()\n", this);
#endif
}

/*------------------------------------------------------------------------------
 * function: DeviseClient::MainLoop
 * Destructor.
 */
void
DeviseClient::MainLoop()
{
  Tk_MainLoop();
#ifdef DEBUG
  printf("Returned from Tk loop\n");
#endif
}

/*------------------------------------------------------------------------------
 * function: DeviseClient::ControlCmd
 * Executes a command from the server.
 */
void
DeviseClient::ControlCmd(int argc, char **argv)
{
#if defined(DEBUG)
  printf("DeviseClient(0x%p)::ControlCmd(%s)\n", this, argv[0]);
#endif

  if (argc == 1 && !strcmp(argv[0], "SyncDone")) {
#if defined(DEBUG)
      printf("Server synchronized.\n");
#endif
      _syncDone = true;
      return;
  }

  TclClient::ControlCmd(argc, argv);
}

/*============================================================================*/
