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

  Current shortcomings:
  - We need to make sure that GData for the master view is never cached,
    so that the query processor always has access to the TData values.
  - Having a view be the slave of multiple masters probably won't work.
  - If you have two slaves with the same TData and link attributes, they
    will each create their own new TData.
  - Donko says that the slave table's query should explicitly name the
    attributes rather than doing 'select *'.
 */

/*
  $Id$

  $Log$
  Revision 1.5  1998/04/27 17:30:26  wenger
  Improvements to TAttrLinks and related code.

  Revision 1.4  1998/04/16 16:52:36  wenger
  Further improvements to set/tdata attribute links, but not quite working
  yet...

  Revision 1.3  1998/04/15 17:14:22  wenger
  Committed version of TAttrLink.c that exposes possible DTE wild
  pointer problem; added DEVISE_ID_FILE and DEVISE_DEF_FILE to
  various startup scripts; added temp_catalog and def_file to
  sample schema directory.

  Revision 1.2  1998/04/14 21:03:14  wenger
  TData attribute links (aka set links) are working except for actually
  creating the join table, and some cleanup when unlinking, etc.

  Revision 1.1  1998/04/10 18:29:31  wenger
  TData attribute links (aka set links) mostly implemented through table
  insertion; a crude GUI for creating them is implemented; fixed some
  bugs in link GUI; changed order in session file for TData attribute links.

 */

#include <stdio.h>

#include "TAttrLink.h"
#include "Util.h"
#include "DevError.h"
#include "ViewGraph.h"
#include "TData.h"
#include "TDataMap.h"
#include "Init.h"
#include "Control.h"
#include "AttrList.h"
#include "Control.h"
#include "Session.h"

#include "RelationManager.h"
#include "types.h"
#include "Inserter.h"
#include "CatalogComm.h"

//#define DEBUG

static TData *
CreateTData(char *name)
{
  //TEMP -- maybe we DON'T want to go thru this so that the
  // TData isn't put into the class directory
  if (!Session::CreateTData(name).IsComplete()) {
    return NULL;
  }
  return (TData *)ControlPanel::Instance()->GetClassDir()->FindInstance(name);
}

static DevStatus
DestroyTData(TData *tdata)
{
  DevStatus result = StatusOk;

  ControlPanel::Instance()->GetClassDir()->DestroyInstance(tdata->GetName());

  return result;
}

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
  printf("TAttrLink(%s)::~TAttrLink()\n", _name);
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
  printf("TAttrLink(%s)::SetFlag(%d)\n", _name, flag);
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
  printf("TAttrLink(%s)::SetMasterView(%s)\n", _name,
      view != NULL ? view->GetName() : "NULL");
#endif

  if (view != NULL && _masterView == view) return;

  MasterSlaveLink::SetMasterView(view);
  //TEMP -- make sure master view doesn't cache GData

  (void) DestroyTable();

  if (view != NULL) {
    //
    // Create the table to hold the master attribute values.
    //
    (void) CreateTable(view);
  } else {
    //
    // Remove the mappings from all slave views.
    //
    int index = _viewList->InitIterator();
    while (_viewList->More(index)) {
      ViewGraph *slaveView = _viewList->Next(index);
      TDataMap *map = slaveView->GetFirstMap();
      if (map != NULL) {
	//TEMP: this doesn't do the right thing, but it keeps us from
	// crashing
        map->SetPhysTData(map->GetLogTData());
	slaveView->Refresh();
      }
    }
    _viewList->DoneIterator(index);
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::InsertView
 * Insert a new slave view for this link.
 */
void
TAttrLink::InsertView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  MasterSlaveLink::InsertView(view);

  if (_tableExists) {
    if (!SetSlaveTable(view).IsComplete()) {
      MasterSlaveLink::DeleteView(view);
    }
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::DeleteView
 * Delete a view from this link.
 */
bool
TAttrLink::DeleteView(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::DeleteView(%s)\n", _name, view->GetName());
#endif

  Boolean isMaster = (view == _masterView);

  bool result = MasterSlaveLink::DeleteView(view);

  if (result) {
    if (isMaster) {
      SetMasterView(NULL);
    } else {
      //
      // Set the physical TData of the view back to the original (logical)
      // TData.
      //
      TDataMap *map = view->GetFirstMap();
      TData *oldTdata = map->GetPhysTData();
      map->SetPhysTData(map->GetLogTData());
      if (!DestroyTData(oldTdata).IsComplete()) result = false;
      //TEMP -- remove old TData from temp catalog
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::Initialize
 * Initialize this link (clear the table of attribute values).
 */
void
TAttrLink::Initialize()
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::Initialize()\n", _name);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
    printf("  TData attribute link updates disabled -- returning\n");
#endif
    return;
  }

  char *curTDName = NULL;
  TData *tdata = GetTData(_masterView, TDataPhys);
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
      currExcept = NULL;
    }
  }

  _recordCount = 0;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::InsertValues
 * Insert values into table.
 */
DevStatus
TAttrLink::InsertValues(TData *tdata, int recCount, void **tdataRecs)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::InsertValues()\n", _name);
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

  _recordCount += recCount;

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
  printf("TAttrLink(%s)::Done()\n", _name);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
    printf("  TData attribute link updates disabled -- returning\n");
#endif
    return;
  }

#if defined(DEBUG)
  printf("  %d records (including duplicates) were inserted into table "
      "for link %s\n", _recordCount, _name);
#endif

  //
  // Close and delete inserter.
  //
  _inserter->close();
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
  }

  delete _inserter;
  _inserter = NULL;

  //
  // Create TDatas for all slave views that don't have them.
  //
    int index = _viewList->InitIterator();
    while (_viewList->More(index)) {
      ViewGraph *slaveView = _viewList->Next(index);
      TDataMap *map = slaveView->GetFirstMap();
      // TEMP: this is a crude test for whether the slave view already has
      // the appropriate TData
      if (map->GetLogTData() == map->GetPhysTData()) {
        if (!SetSlaveTable(slaveView).IsComplete()) {
	  // Can't do this while iterator is open.
	  //TEMP MasterSlaveLink::DeleteView(slaveView);
	}
      }
    }
    _viewList->DoneIterator(index);

  //
  // Invalidate the TDatas of all slave views.
  //
  index = InitIterator();
  while (More(index)) {
    ViewGraph *view = Next(index);
    TData *tdata = GetTData(view, TDataPhys);
    //TEMP -- this changes visual filter
    if (tdata != NULL) tdata->InvalidateTData();
  }
  DoneIterator(index);

  //
  // Tell the slave views to redraw.
  //
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
  printf("TAttrLink(%s)::Abort()\n", _name);
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
  printf("TAttrLink(%s)::SetLinkType(%d)\n", _name, type);
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
  printf("TAttrLink(%s)::CreateTable(%s)\n", _name, masterView->GetName());
#endif

  //
  // Get the TData for the master view.
  //
  TData *tdata = GetTData(masterView, TDataPhys);
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
    currExcept = NULL;
    return StatusFailed;
  }
#if defined(DEBUG)
  printf("  Created master table file %s\n", _tableFile);
#endif

  _tableExists = true;

  return StatusOk;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetSlaveTable
 * Sets the table for a slave view (creates a new table that's the join
 * of the original table and the table of master attribute values).
 */
DevStatus
TAttrLink::SetSlaveTable(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetSlaveTable(%s)\n", _name, view->GetName());
#endif

// TEMP: maybe we should always unlink slave view before calling this -- or
// delete the slave table and mapping for it

  DevStatus result = StatusOk;

  //
  // Make sure we have a table of the master attribute values.
  //
  if (!_tableExists) {
    char errBuf[256];
    sprintf(errBuf, "No master table for link %s", GetName());
    reportErrNosys(errBuf);
    result = StatusFailed;
  }

  //
  // Make sure the slave view has a TData already defined, and that the
  // TData is a Table (as opposed to a UNIXFILE, or whatever).
  //
  TData *tdata;
  if (result.IsComplete()) {
    tdata = GetTData(view, TDataLog);
    if (tdata == NULL) result = StatusFailed;
  }
  if (result.IsComplete()) {
    if (strcmp(tdata->DispatchedName(), "TDataDQL")) {
      reportErrNosys("Slave view of TAttrLink must have Table data source");
      result = StatusFailed;
    }
  }

  //
  // Define a new TData that's the join of the slave view's original
  // TData with the table of master attribute values.
  //
  const int nameSize = 128;
  char slaveTableName[nameSize];
  memset((void *)slaveTableName, 0, nameSize);
  if (result.IsComplete()) {

    // Generate query for slave table.
    const int querySize = 1024;
    char query[querySize];
    memset((void *)query, 0, querySize);
    ostrstream ost(query, querySize);

    ost << "select ";

    int count = 0;
    AttrList *tdAttrs = tdata->GetAttrList();
    tdAttrs->InitIterator();
    while (tdAttrs->More()) {
      AttrInfo *attr = tdAttrs->Next();

      // Don't put recId into query.
      if (strcmp(attr->name, "recId")) {
        if (count > 0) ost << ", ";
        ost << "t1." << attr->name;
        count++;
      }
    }
    tdAttrs->DoneIterator();
    ost << " from " << tdata->GetName() << " as t1, " << *_relId <<
        " as t2 where t1." << _slaveAttrName << " = t2." << _masterAttrName;

#if defined(DEBUG)
    printf("  slave table query = %s\n", query);
#endif

    // Generate attribute list for slave table.

    // -1 because we don't include recId.
    int numFlds = tdAttrs->NumAttrs() - 1;
    string *attributeNames = new string[numFlds];

    count = 0;
    tdAttrs->InitIterator();
    while (tdAttrs->More()) {
      AttrInfo *attr = tdAttrs->Next();

      // Don't put recId into attribute list.
      if (strcmp(attr->name, "recId")) {
        attributeNames[count] = attr->name;
        count++;
      }
    }
    tdAttrs->DoneIterator();
    ViewInterface vi(numFlds, attributeNames, query);

    // Create the table.
    RelationId newRelId = RELATION_MNGR.registerNewRelation(vi);
    if (currExcept) {
      cerr << currExcept->toString() << endl;
      currExcept = NULL;
      return StatusFailed;
    }

    ostrstream nost(slaveTableName, nameSize);
    nost << newRelId;

#if defined(DEBUG)
    printf("  newRelId: %s\n", slaveTableName);
#endif
  }

  //
  // Change the view's mapping's physical TData to the new TData.
  //
  TDataMap *map;
  if (result.IsComplete()) {
    map = view->GetFirstMap();
    map->SetPhysTData(CreateTData(slaveTableName));
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::DestroyTable
 * Destroys the table for storing the master attribute values.
 */
DevStatus
TAttrLink::DestroyTable()
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::DestroyTable()\n", _name);
#endif

  if (_tableExists) RELATION_MNGR.deleteRelation(*_relId);

  _tableExists = false;

  delete _schema;
  _schema = NULL;

  delete _stdInt;
  _stdInt = NULL;

  delete _relId;
  _relId = NULL;

  if (_inserter != NULL) {
    _inserter->close();
    if (currExcept) {
      cerr << currExcept->toString() << endl;
      currExcept = NULL;
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
TAttrLink::GetTData(ViewGraph *view, TDType tdType)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::GetTData(%s, %d)\n", _name, view->GetName(), tdType);
#endif

  TData *tdata;
  TDataMap *tdMap = view->GetFirstMap();
  if (tdMap == NULL) {
    tdata = NULL;
  } else {
    switch (tdType) {
    case TDataPhys:
      tdata = tdMap->GetPhysTData();
      break;

    case TDataLog:
      tdata = tdMap->GetLogTData();
      break;

    default:
      reportErrNosys("Illegal TData type");
      tdata = NULL;
      break;
    }
  }

  if (tdata == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't get TData for view %s", view->GetName());
    reportErrNosys(errBuf);
  }

  return tdata;
}

/*============================================================================*/
