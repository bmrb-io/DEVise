/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998-2013
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of TAttrLink class (links doing joins on TData attributes).

  Slave view(s) can only display records whose specified TData attribute
  has a value that matches the value of that attribute for one of the
  records displayed in the master view.
 */

/*
  $Id$

  $Log$
  Revision 1.10.22.1  2013/06/13 21:03:01  wenger
  Changes to get DEVise to compile/link on CentOS6 (with comments for
  a bunch of unfixed warnings); minor mods to get this version to also
  compile on RHEL5...

  Revision 1.10  2005/12/06 20:04:14  wenger
  Merged V1_7b0_br_4 thru V1_7b0_br_5 to trunk.  (This should
  be the end of the V1_7b0_br branch.)

  Revision 1.9.14.1  2005/09/06 21:20:18  wenger
  Got DEVise to compile with gcc 4.0.1.

  Revision 1.9  2000/02/16 18:51:44  wenger
  Massive "const-ifying" of strings in ClassDir and its subclasses.

  Revision 1.8  1998/06/15 19:55:21  wenger
  Fixed bugs 338 and 363 (problems with special cases of set links).

  Revision 1.7  1998/06/12 19:55:31  wenger
  Attribute of TAttr/set links can now be changed; GUI has menu of available
  attributes; attribute is set when master view is set instead of at link
  creation; misc. debug code added.

  Revision 1.6  1998/05/06 22:04:57  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.5  1998/04/30 14:24:21  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.4  1998/04/29 17:53:56  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.3  1998/04/28 18:03:14  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

  Revision 1.2  1998/04/14 21:03:14  wenger
  TData attribute links (aka set links) are working except for actually
  creating the join table, and some cleanup when unlinking, etc.

  Revision 1.1  1998/04/10 18:29:31  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

 */

#ifndef _TAttrLink_h_
#define _TAttrLink_h_

#include <sys/types.h>

#include "MasterSlaveLink.h"
#include "DevStatus.h"
#include "ObjectValid.h"

class TData;
class DerivedTable;

enum TDType { TDataPhys, TDataLog };

class TAttrLink : public MasterSlaveLink {
public:
  // Constructor/destructor.
  TAttrLink(const char *name, const char *masterAttrName,
      const char *slaveAttrName);
  virtual ~TAttrLink();

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

  // Unique to TAttrLink.
  const char *GetMasterAttrName() { return _masterAttrName; }
  const char *GetSlaveAttrName() { return _slaveAttrName; }
  void SetMasterAttr(char *masterAttrName);
  void SetSlaveAttr(char *slaveAttrName);

  const char *GetPhysMasterAttrName();

  DerivedTable *GetMasterTable();

  static TData *GetTData(ViewGraph *view, TDType tdType);

protected:
  DevStatus CreateMasterTable();
  DevStatus DestroyMasterTable();

  void ReCreateSlaveTDatas();

  static int _objectCount;
  static DerivedTable *_dummyTable;
  static const char *_dummyAttrName;

  char *_masterTableName;

  char *_masterAttrName;
  char *_slaveAttrName;

private:
  ObjectValid _objectValid;
};

#endif // _TAttrLink_h_

/*============================================================================*/
