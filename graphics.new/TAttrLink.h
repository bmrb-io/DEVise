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
  Declaration of TAttrLink class (links doing joins on TData attributes).

  Slave view(s) can only display records whose specified TData attribute
  has a value that matches the value of that attribute for one of the
  records displayed in the master view.
 */

/*
  $Id$

  $Log$
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

class TData;

enum TDType { TDataPhys, TDataLog };

class TAttrLink : public MasterSlaveLink {
public:
  TAttrLink(char *name, char *masterAttrName, char *slaveAttrName);
  virtual ~TAttrLink();

  virtual void SetFlag(VisualFlag flag);

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

  const char *GetMasterAttrName() { return _masterAttrName; }
  const char *GetSlaveAttrName() { return _slaveAttrName; }

protected:
  DevStatus CreateTable();
  DevStatus SetSlaveTable(ViewGraph *view);
  DevStatus DestroyTable();
  TData *GetTData(ViewGraph *view, TDType tdType);

  char *_masterTableName;

  char *_masterAttrName;
  char *_slaveAttrName;

private:
  Boolean _objectValid;
};

#endif // _TAttrLink_h_

/*============================================================================*/
