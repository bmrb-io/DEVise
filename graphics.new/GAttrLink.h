/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2002
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of GAttrLink class (links doing joins on GData attributes).

  Follower view(s) can only display records whose specified GData attribute
  has a value that matches the value of that attribute for one of the
  records displayed in the leader view.
 */

/*
  $Id$

  $Log$
  Revision 1.1.2.1  2002/09/05 19:14:03  wenger
  Implemented GData attribute value links (but not GUI for creating
  them).

 */

#ifndef _GAttrLink_h_
#define _GAttrLink_h_

#include <sys/types.h>

#include "MasterSlaveLink.h"
#include "DevStatus.h"
#include "ObjectValid.h"
#include "HashTable.h"

class TDataMap;

class GAttrLink : public MasterSlaveLink {
public:
  // Constructor/destructor.
  GAttrLink(const char *name, const char *leaderAttrName,
      const char *followerAttrName);
  virtual ~GAttrLink();

  // From DeviseLink.
  virtual void SetFlag(VisualFlag flag);

  // From MasterSlaveLink.
  virtual void SetMasterView(ViewGraph *view);

  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  virtual char *GetFileName() { return "none"; }
  virtual void Initialize();
  virtual void InsertRecs(RecId recid, int num) {}
  virtual int  FetchRecs(RecId recid, RecId &rec, int &num) { return 0; }
  virtual void Done();
  virtual void Abort();
  virtual RecordLinkType GetLinkType () { return NotRecordLink; }
  virtual void SetLinkType(RecordLinkType type);

  // Unique to GAttrLink.
  const char *GetLeaderAttrName() { return _leaderAttrName; }
  const char *GetFollowerAttrName() { return _followerAttrName; }
  void SetLeaderAttr(char *leaderAttrName);
  void SetFollowerAttr(char *followerAttrName);

  void InsertRecord(TDataMap *tdMap, const char *gDataRec);
  bool TestRecord(TDataMap *tdMap, const char *gDataRec);

protected:
  char *_leaderAttrName;
  char *_followerAttrName;

private:
  // Returns true iff there are no leader/follower attribute incompatibilities.
  bool CheckAttrs();

  // Returns true iff value is valid.
  static bool GetAttrValue(TDataMap *tdMap, const char *gDataRec,
    const char *attrName, Coord &value);

  static int CoordHash(Coord &value, int numBuckets);

  HashTable<Coord, int> *_selectionValues;
 
  ObjectValid _objectValid;
};

#endif // _GAttrLink_h_

/*============================================================================*/
