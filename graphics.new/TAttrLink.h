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
  Revision 1.1  1998/04/10 18:29:31  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

 */

#ifndef _TAttrLink_h_
#define _TAttrLink_h_

#include <sys/types.h>

#include "MasterSlaveLink.h"
#include "DList.h"
#include "DevStatus.h"

class TData;
class ISchema;
class StandardInterface;
class RelationId;
class UniqueInserter;
class SlaveViewInfo;

DefinePtrDList(SlaveViewInfoList, SlaveViewInfo *);

class TAttrLink : public MasterSlaveLink {
public:
  TAttrLink(char *name, char *masterAttrName, char *slaveAttrName);
  virtual ~TAttrLink();

  virtual void SetFlag(VisualFlag flag);

  virtual void SetMasterView(ViewGraph *view);

  virtual void InsertView(ViewGraph *view);
  virtual bool DeleteView(ViewGraph *view);

  virtual char *GetFileName() { return ( _tableFile ? _tableFile : "none"); }
  virtual void Initialize();
  virtual void InsertRecs(RecId recid, int num) {}
  virtual int  FetchRecs(RecId recid, RecId &rec, int &num) { return 0; }
  virtual void Done();
  virtual void Abort();
  virtual RecordLinkType GetLinkType () { return NotRecordLink; }
  virtual void SetLinkType(RecordLinkType type);

  const char *GetMasterAttrName() { return _masterAttrName; }
  const char *GetSlaveAttrName() { return _slaveAttrName; }

  virtual DevStatus InsertValues(TData *tdata, int recCount, void **tdataRecs);

protected:
  DevStatus CreateTable(ViewGraph *masterView);
  DevStatus SetSlaveTable(ViewGraph *view);
  DevStatus DestroyTable();
  TData *GetTData(ViewGraph *view);

  char *_masterAttrName;
  char *_slaveAttrName;

  char *_tdataName;

  Boolean _tableExists;
  char *_tableFile;

  ISchema *_schema;
  StandardInterface *_stdInt;
  RelationId *_relId;
  UniqueInserter *_inserter;

  SlaveViewInfoList _slaveViewInfo;
};

#endif // _TAttrLink_h_

/*============================================================================*/
