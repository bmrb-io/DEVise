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
  Declaration of DeviseBatchClient class.
 */

/*
  $Id$

  $Log$
  Revision 1.2.28.1  2005/09/28 22:29:34  wenger
  Various const-ifying to make things compile better on basslet.

  Revision 1.2  1998/01/07 19:28:12  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.1.2.1  1997/12/09 19:03:35  wenger
  deviseb now uses client/server library.

 */

#ifndef _DeviseBatchClient_h_
#define _DeviseBatchClient_h_

#include "DeviseTypes.h"
#include "Client.h"

class DeviseBatchClient : public Client {
public:
  DeviseBatchClient(char *name, char *hostname, int port);
  virtual ~DeviseBatchClient();

  virtual void MainLoop() {}

  virtual int ServerCmd(int argc, const char * const *argv) {
    return Client::ServerCmd(argc, argv); }
  virtual void ReadServer() { Client::ReadServer(); }

  	// process server command
  virtual void ControlCmd(int argc, const char * const *argv);

  virtual void SetSyncDone(Boolean syncDone) { _syncDone = syncDone; }
  virtual Boolean SyncDone() { return _syncDone; }

protected:
  Boolean _syncDone;

};

#endif /* _DeviseBatchClient_h_ */

/*============================================================================*/
