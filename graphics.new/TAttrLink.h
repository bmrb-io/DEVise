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
 */

#ifndef _TAttrLink_h_
#define _TAttrLink_h_

#include <sys/types.h>

#include "MasterSlaveLink.h"
#include "DevStatus.h"

class TData;
class ISchema;
class StandardInterface;
class RelationId;
class UniqueInserter;

class TAttrLink : public MasterSlaveLink {
public:
  TAttrLink(char *name, char *masterAttrName, char *slaveAttrName);
  virtual ~TAttrLink();

  virtual void SetFlag(VisualFlag flag);

  virtual void SetMasterView(ViewGraph *view);

  virtual void InsertView(ViewGraph *view);

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
  //? Print();

protected:
  DevStatus CreateTable(ViewGraph *masterView);
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
};

#endif // _TAttrLink_h_

/*============================================================================*/
