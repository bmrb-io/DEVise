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
  Implementation of TAttrLink class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>

#include "TAttrLink.h"
#include "Util.h"
#include "DevError.h"
#include "ViewGraph.h"
#include "TData.h"
#include "TDataMap.h"
#include "Init.h"

#include "RelationManager.h"
#include "types.h"
#include "Inserter.h"

//#define DEBUG

/*------------------------------------------------------------------------------
 * function: TAttrLink::TAttrLink
 * Constructor.
 */
TAttrLink::TAttrLink(char *name, char *masterAttrName, char *slaveAttrName) :
    MasterSlaveLink(name, VISUAL_TATTR)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::TAttrLink(%s, %s, %s)\n", this, name,
      masterAttrName, slaveAttrName);
#endif

  _masterAttrName = CopyString(masterAttrName);
  _slaveAttrName = CopyString(slaveAttrName);
  _tdataName = NULL;
  _tableExists = false;
  _tableFile = NULL;
  _schema = NULL;
  _stdInt = NULL;
  _relId = NULL;
  _inserter = NULL;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::~TAttrLink
 * Destructor.
 */
TAttrLink::~TAttrLink()
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::~TAttrLink()\n", this);
#endif

  delete [] _masterAttrName;
  delete [] _slaveAttrName;

  (void) DestroyTable();
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetFlag
 * Set flag (link type).
 */
void
TAttrLink::SetFlag(VisualFlag flag)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::SetFlag(%d)\n", this, flag);
#endif

  reportErrNosys("Cannot change link type of TData attribute link\n");
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetMasterView
 * Set the master view for this link (create the table for the attribute
 * values).
 */
void
TAttrLink::SetMasterView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::SetMasterView(%s)\n", this, view->GetName());
#endif

  MasterSlaveLink::SetMasterView(view);

//TEMPTEMP -- check whether view is already master?

  (void) DestroyTable();
  (void) CreateTable(view);
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::InsertView
 * Insert a new slave view for this link.
 */
void
TAttrLink::InsertView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::InsertView(%s)\n", this, view->GetName());
#endif

  MasterSlaveLink::InsertView(view);

  //TEMPTEMP -- change tdata of slave view (must be table)
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::Initialize
 * Initialize this link (clear the table of attribute values).
 */
void
TAttrLink::Initialize()
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::Initialize()\n", this);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
    printf("  TData attribute link updates disabled -- returning\n");
#endif
    return;
  }

  char *curTDName = NULL;
  TData *tdata = GetTData(_masterView);
  if (tdata != NULL) {
    curTDName = tdata->GetName();
  }

  if (!_tableExists) {
    (void) CreateTable(_masterView);
  }
  else if (_tdataName != NULL && curTDName != NULL &&
      strcmp(_tdataName, curTDName)) {
#if defined(DEBUG)
    printf("TAttrLink %s: master view (%s) has new tdata\n", GetName(),
        _masterView->GetName());
#endif
    (void) DestroyTable();
    (void) CreateTable(_masterView);
  }

  if (_tableExists) {
    _inserter = new UniqueInserter(*_schema, _tableFile, ios::out);
    if (currExcept) {
      cerr << currExcept->toString() << endl;
    }
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::InsertValues
 * Insert values into table.
 */
DevStatus
TAttrLink::InsertValues(TData *tdata, int recCount, void **tdataRecs)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::InsertValues()\n", this);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
    printf("  TData attribute link updates disabled -- returning\n");
#endif
    return StatusCancel;
  }

  DevStatus result = StatusOk;

  //
  // Find offset and type of master attribute.
  //
  AttrList *tdAttrs = tdata->GetAttrList();
  AttrInfo *attrInfo = tdAttrs->Find(_masterAttrName);
  if (attrInfo == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't find attribute <%s>", _masterAttrName);
    reportErrNosys(errBuf);
    result = StatusFailed;
  }

  //
  // Insert master attribute values into the table.
  //
  if (result.IsComplete()) {
    Tuple tuple[1];

    for (int recNum = 0; recNum < recCount; recNum++) {
      void *recordP = tdataRecs[recNum];
      void *attrP = recordP + attrInfo->offset;

      switch (attrInfo->type) {
        case IntAttr: {
	  int tmpInt;
	  memcpy((void *)&tmpInt, attrP, sizeof(tmpInt));
#if defined(DEBUG)
	  printf("  Inserting %d\n", tmpInt);
#endif
	  tuple[0] = IInt::getTypePtr(&tmpInt);
          break;
        }

        case FloatAttr: {
	  float tmpFloat;
	  memcpy((void *)&tmpFloat, attrP, sizeof(tmpFloat));
	  double tmpDouble = (double) tmpFloat;
#if defined(DEBUG)
	  printf("  Inserting %g\n", tmpDouble);
#endif
	  tuple[0] = IDouble::getTypePtr(&tmpDouble);
          break;
	}

        case DoubleAttr: {
	  double tmpDouble;
	  memcpy((void *)&tmpDouble, attrP, sizeof(tmpDouble));
#if defined(DEBUG)
	  printf("  Inserting %g\n", tmpDouble);
#endif
	  tuple[0] = IDouble::getTypePtr(&tmpDouble);
          break;
	}

        case StringAttr: {
	  char *tmpStr = (char *)attrP;
	  tuple[0] = IString::getTypePtr(tmpStr);
#if defined(DEBUG)
	  printf("  Inserting %s\n", tmpStr);
#endif
          break;
	}

        case DateAttr: {
          reportErrNosys("TAttrLinks not yet implemented for dates");//TEMP
          result = StatusFailed;//TEMP
          break;
	}

        default: {
          reportErrNosys("Illegal attribute type");
          result = StatusFailed;
          break;
	}
      }

      _inserter->insert(tuple);
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::Done
 * Propagate updates when insertions to link are done.
 */
void
TAttrLink::Done()
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::Done()\n", this);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
    printf("  TData attribute link updates disabled -- returning\n");
#endif
    return;
  }

  _inserter->close();
  if (currExcept) {
    cerr << currExcept->toString() << endl;
  }

  delete _inserter;
  _inserter = NULL;

  MasterSlaveLink::Done();
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::Abort
 * Aborts link.
 */
void
TAttrLink::Abort()
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::Abort()\n", this);
#endif
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetLinkType
 * Set (record) link type -- illegal on this class.
 */
void
TAttrLink::SetLinkType(RecordLinkType type)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::SetLinkType(%d)\n", this, type);
#endif

  reportErrNosys("Cannot change record link type of TData attribute link\n");
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::CreateTable
 * Creates the table for storing the master attribute values.
 */
DevStatus
TAttrLink::CreateTable(ViewGraph *masterView)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::CreateTable(%s)\n", this, masterView->GetName());
#endif

  //
  // Get the TData for the master view.
  //
  TData *tdata = GetTData(masterView);
  if (tdata == NULL) return StatusFailed;
  _tdataName = CopyString(tdata->GetName());

#if defined(DEBUG)
  printf("  View %s has TData %s\n", masterView->GetName(), _tdataName);
#endif

  //
  // Find (hopefully) the master attribute in this tdata.
  //
  AttrList *tdAttrs = tdata->GetAttrList();
  AttrInfo *attrInfo = tdAttrs->Find(_masterAttrName);
  if (attrInfo == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't find attribute <%s>", _masterAttrName);
    reportErrNosys(errBuf);
    return StatusFailed;
  }

  //
  // Find the TData record size and make sure it's something we can
  // deal with.
  //
  int recordSize = tdata->RecSize();
  if (recordSize < 0) {
    reportErrNosys("Can't deal with variable-size TData records");
    return StatusFailed;
  } else if (recordSize == 0) {
    reportErrNosys("TData record size is zero");
    return StatusFailed;
  }

  //
  // Create the table.
  //
  TypeID types[1];
  string attrs[1];
  switch (attrInfo->type) {
  case IntAttr:
    types[0] = INT_TP;
    break;

  case FloatAttr:
    types[0] = DOUBLE_TP;
    break;

  case DoubleAttr:
    types[0] = DOUBLE_TP;
    break;

  case StringAttr:
    types[0] = STRING_TP;
    break;

  case DateAttr:
    types[0] = DATE_TP;
    break;

  default:
    reportErrNosys("Illegal attribute type");
    return StatusFailed;
    break;
  }
  attrs[0] = _masterAttrName;

  _schema = new ISchema(1, types, attrs);

  _tableFile = tempnam(Init::TmpDir(), "tdaln");
  if (_tableFile == NULL) {
    reportErrSys("Out of memory");
    return StatusFailed;
  }

  _stdInt = new StandardInterface(*_schema, _tableFile);
  _relId = new RelationId;
  *_relId = RELATION_MNGR.registerNewRelation(*_stdInt);
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    return StatusFailed;
  }
#if defined(DEBUG)
  printf("  Created table file %s\n", _tableFile);
#endif

  _tableExists = true;

  return StatusOk;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::DestroyTable
 * Destroys the table for storing the master attribute values.
 */
DevStatus
TAttrLink::DestroyTable()
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::DestroyTable()\n", this);
#endif

  _tableExists = false;

  RELATION_MNGR.deleteRelation(*_relId);

  delete _schema;
  delete _stdInt;
  delete _relId;

  if (_inserter != NULL) {
    _inserter->close();
    if (currExcept) {
      cerr << currExcept->toString() << endl;
    }
  }
  delete _inserter;
  _inserter = NULL;

  unlink(_tableFile);
  delete [] _tableFile;
  _tableFile = NULL;

  return StatusOk;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::GetTData
 * Get the TData for the given view.
 */
TData *
TAttrLink::GetTData(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(0x%p)::GetTData(%s)\n", this, view->GetName());
#endif

  TData *tdata;
  TDataMap *tdMap = view->GetFirstMap();
  if (tdMap == NULL) {
    tdata = NULL;
  } else {
    tdata = tdMap->GetTData();
  }

  if (tdata == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't get TData for view %s", view->GetName());
    reportErrNosys(errBuf);
  }

  return tdata;
}

/*============================================================================*/
