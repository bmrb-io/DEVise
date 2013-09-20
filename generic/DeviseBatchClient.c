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
  Implementation of DeviseBatchClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.5.22.1  2013/09/20 15:29:20  wenger
  More Centos 6 compile fixes.

  Revision 1.5  2005/12/06 20:02:49  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.4  2002/06/17 19:40:52  wenger
  Merged V1_7b0_br_1 thru V1_7b0_br_2 to trunk.

  Revision 1.3.26.2  2005/09/28 22:29:34  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.3.26.1  2002/05/27 18:15:47  wenger
  Got DEVise to compile with gcc 2.96 (so I can compile it at NRG).

  Revision 1.3  1998/02/16 18:13:14  wenger
  Added collaborator to appropriate release scripts; got deviseb to compile
  and link.

  Revision 1.2  1998/01/07 19:28:12  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:35  wenger
  deviseb now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DeviseBatchClient.h"

//#define DEBUG

/*------------------------------------------------------------------------------
 * function: DeviseBatchClient::DeviseBatchClient
 * Constructor
 */
DeviseBatchClient::DeviseBatchClient(const char *name, const char *hostname,
  int port) : Client(name, hostname, port, "NULL")
{
#if defined(DEBUG)
  printf("DeviseBatchClient(0x%p)::DeviseBatchClient(%s, %s, %d)\n", this,
      name, hostname, port);
#endif

  _syncDone = false;
}

/*------------------------------------------------------------------------------
 * function: DeviseBatchClient::~DeviseBatchClient
 * Destructor.
 */
DeviseBatchClient::~DeviseBatchClient()
{
#if defined(DEBUG)
  printf("DeviseBatchClient(0x%p)::~DeviseBatchClient()\n", this);
#endif
}

/*------------------------------------------------------------------------------
 * function: DeviseBatchClient::ControlCmd
 * Executes a command from the server.
 */
void
DeviseBatchClient::ControlCmd(int argc, const char * const *argv)
{
#if defined(DEBUG)
  printf("DeviseBatchClient(0x%p)::ControlCmd()\n", this);
#endif

  if (argc == 1 && !strcmp(argv[0], "SyncDone")) {
#ifdef DEBUG
    printf("Server synchronized.\n");
#endif
    _syncDone = 1;
    return;
  }

#ifdef DEBUG
  printf("Ignoring control command: \"%s\"\n", argv[0]);
#endif
}

/*============================================================================*/
