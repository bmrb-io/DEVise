/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-2000
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
  Revision 1.5  2000/01/13 23:07:09  wenger
  Got DEVise to compile with new (much fussier) compiler (g++ 2.95.2).

  Revision 1.4  1998/11/11 14:31:01  wenger
  Implemented "highlight views" for record links and set links; improved
  ClassDir::DestroyAllInstances() by having it destroy all links before
  it destroys anything else -- this cuts down on propagation problems as
  views are destroyed; added test code for timing a view's query and draw.

  Revision 1.3  1998/07/30 15:31:21  wenger
  Fixed bug 381 (problem with setting master and slave of a link to the same
  view); generally cleaned up some of the master-slave link related code.

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
  MasterSlaveLink(const char *name, VisualFlag linkFlag);
  virtual ~MasterSlaveLink();

  virtual void SetMasterView(ViewGraph *view);
  virtual ViewGraph *GetMasterView() { return _masterView; }

  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  virtual const char *GetFileName() = 0;
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
  virtual void ClearHighlightViews();

  ViewGraph *_masterView; // master view of link
  static Boolean _disableUpdates;	// disable all master/slave links
};

#endif // _MasterSlaveLink_h_

/*============================================================================*/
