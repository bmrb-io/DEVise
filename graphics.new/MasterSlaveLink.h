/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of MasterSlaveLink class (any kind of link where one or more
  slave views depend on a master view).
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/04/30 14:24:20  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.1  1998/04/10 18:29:26  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

 */

#ifndef _MasterSlaveLink_h_
#define _MasterSlaveLink_h_


#include <sys/types.h>

#include "DeviseLink.h"
#include "RecId.h"
#include "DevStatus.h"

class TData;

class MasterSlaveLink : public DeviseLink {
public:
  MasterSlaveLink(char *name, VisualFlag linkFlag);
  virtual ~MasterSlaveLink();

  virtual void SetMasterView(ViewGraph *view);
  virtual ViewGraph *GetMasterView() { return _masterView; }

  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  virtual char *GetFileName() = 0;
  virtual void Initialize() = 0;
  virtual void InsertRecs(RecId recid, int num) = 0;
  virtual int  FetchRecs(RecId recid, RecId &rec, int &num) = 0;
  virtual void Done();
  virtual void Abort() = 0;
  virtual void Print();
  virtual RecordLinkType GetLinkType () = 0;
  virtual void SetLinkType(RecordLinkType type) = 0;

  static void EnableUpdates() { _disableUpdates = false; }
  static void DisableUpdates() { _disableUpdates = true; }

protected:
  ViewGraph *_masterView; // master view of link
  static Boolean _disableUpdates;	// disable all master/slave links
};

#endif // _MasterSlaveLink_h_

/*============================================================================*/
