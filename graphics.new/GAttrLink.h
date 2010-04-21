/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 2002-2010
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
  Revision 1.3.14.3  2010/04/07 19:06:26  wenger
  Finished implementation of "all" option in GAttrLinks, except for GUI
  to control it.

  Revision 1.3.14.2  2010/04/07 18:10:54  wenger
  The "All" option in GAttrLinks is now working except for commands to
  get and set it.

  Revision 1.3.14.1  2010/04/06 23:54:56  wenger
  Partially implemented the "All" option in GAttrLink -- a leader value
  of "All" will match all follower records (still needs a bunch of cleanup,
  and a command to turn it on and off).

  Revision 1.3  2005/12/06 20:04:00  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.2  2003/01/13 19:25:22  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.1.2.3  2005/09/06 22:04:55  wenger
  Added proper const-ness to HashTable.

  Revision 1.1.2.2  2005/09/06 21:20:17  wenger
  Got DEVise to compile with gcc 4.0.1.

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
class StringStorage;

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

  virtual const char *GetFileName() { return "none"; }
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

  bool GetAllEnabled() { return _enableAll; }
  void SetAllEnabled(bool enabled) { _enableAll = enabled; }

  void InsertRecord(TDataMap *tdMap, const char *gDataRec);
  bool TestRecord(TDataMap *tdMap, const char *gDataRec);

protected:
  char *_leaderAttrName;
  char *_followerAttrName;

private:
  // Returns true iff there are no leader/follower attribute incompatibilities.
  bool CheckAttrs();

  // Returns true iff value is valid.  (Optionally get the value as a
  // string, only if the actual attribute is a string -- this doesn't
  // convert if the attribute is not a string.)
  static bool GetAttrValue(TDataMap *tdMap, const char *gDataRec,
    const char *attrName, Coord &value, char *strBuf = NULL,
    int bufLen = 0);

  static int CoordHash(const Coord &value, int numBuckets);

  HashTable<Coord, int> *_selectionValues;

  static StringStorage *GetStringTable(TDataMap *tdMap,
    const char *attrName);

  // Whether the "all link" option is enabled.
  bool _enableAll;

  // Whether the leader actually had an "all" value.
  bool _hasAll;
 
  ObjectValid _objectValid;
};

#endif // _GAttrLink_h_

/*============================================================================*/
