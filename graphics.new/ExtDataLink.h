/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2001
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of the ExtDataLink class.  This link is used similarly to
  a record link, in the case where the leader view runs an external
  process to generate data to be displayed in the follower view.
 */

/*
  $Id$

  $Log$
 */

#ifndef _ExtDataLink_h_
#define _ExtDataLink_h_


#include <sys/types.h>

#include <MasterSlaveLink.h>

class ExtDataLink : public MasterSlaveLink {
public:
  ExtDataLink(const char *name);
  virtual ~ExtDataLink();

  virtual const char *GetFileName() { return "none"; }
  virtual void Initialize();
  virtual void InsertRecs(RecId recid, int num) {}
  virtual int FetchRecs(RecId recid, RecId &rec, int &num) { return 0; }
  virtual void Done();
  virtual void Abort();
  virtual RecordLinkType GetLinkType() { return NotRecordLink; }
  virtual void SetLinkType(RecordLinkType type) {}
};

#endif // _ExtDataLink_h_

/*============================================================================*/
