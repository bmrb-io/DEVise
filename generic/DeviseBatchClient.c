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
  Implementation of DeviseBatchClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  1997/12/09 19:03:35  wenger
  deviseb now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>

#include "DeviseBatchClient.h"

//#define DEBUG

/*------------------------------------------------------------------------------
 * function: DeviseBatchClient::DeviseBatchClient
 * Constructor
 */
DeviseBatchClient::DeviseBatchClient(char *name, char *hostname, int port) :
  Client(name, hostname, port)
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
DeviseBatchClient::ControlCmd(int argc, char **argv)
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
