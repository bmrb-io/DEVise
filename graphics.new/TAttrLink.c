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
  Revision 1.7  1998/04/29 17:53:55  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

  Revision 1.6  1998/04/28 18:03:12  wenger
  Added provision for "logical" and "physical" TDatas to mappings,
  instead of creating new mappings for slave views; other TAttrLink-
  related improvements.

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
#include "Session.h"
#include "DerivedTable.h"

#include "RelationManager.h"
#include "types.h"
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

  //TEMP ControlPanel::Instance()->GetClassDir()->DestroyInstance(tdata->GetName());

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

  _masterTableName = NULL;
  _masterAttrName = CopyString(masterAttrName);
  _slaveAttrName = CopyString(slaveAttrName);
  _objectValid = true;
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

  (void) DestroyTable();
  delete [] _masterAttrName;
  _masterAttrName = NULL;
  delete [] _slaveAttrName;
  _slaveAttrName = NULL;
  _objectValid = false;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetFlag
 * Set flag (link type).
 */
void
TAttrLink::SetFlag(VisualFlag flag)
{
  DOASSERT(_objectValid, "operation on invalid object");
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
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetMasterView(%s)\n", _name,
      view != NULL ? view->GetName() : "NULL");
#endif

  if (view != NULL && _masterView == view) return;

  (void) DestroyTable();

  MasterSlaveLink::SetMasterView(view);
  //TEMP -- make sure master view doesn't cache GData

  if (view != NULL) {
    //
    // Create the table to hold the master attribute values.
    //
    (void) CreateTable();
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
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  MasterSlaveLink::InsertView(view);

  if (_masterTableName != NULL) {
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
  DOASSERT(_objectValid, "operation on invalid object");
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
  DOASSERT(_objectValid, "operation on invalid object");
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

  if (_masterTableName == NULL) {
    (void) CreateTable();
  } else {
    //
    // Make sure the master table has the same TData as the master view
    // (in case the view's TData got changed).
    //
    DerivedTable *masterTable = _masterView->GetDerivedTable(_masterTableName);
    DOASSERT(masterTable != NULL, "No master table!");
  
    if (masterTable->GetTDataName() != NULL && curTDName != NULL &&
        strcmp(masterTable->GetTDataName(), curTDName)) {
#if defined(DEBUG)
      printf("TAttrLink %s: master view (%s) has new tdata\n", GetName(),
          _masterView->GetName());
#endif
      (void) DestroyTable();
      (void) CreateTable();
    }
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::Done
 * Propagate updates when insertions to link are done.
 */
void
TAttrLink::Done()
{
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::Done()\n", _name);
#endif

  if (_disableUpdates) {
#if defined(DEBUG)
    printf("  TData attribute link updates disabled -- returning\n");
#endif
    return;
  }

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
  DOASSERT(_objectValid, "operation on invalid object");
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
  DOASSERT(_objectValid, "operation on invalid object");
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
TAttrLink::CreateTable()
{
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::CreateTable()\n", _name);
#endif

  DevStatus result = StatusOk;

  _masterTableName = _masterView->CreateDerivedTable(_name, _masterAttrName);
  if (_masterTableName == NULL) {
    char errBuf[1024];
    sprintf(errBuf, "Can't create master table for link %s\n", _name);
    result = StatusFailed;
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetSlaveTable
 * Sets the table for a slave view (creates a new table that's the join
 * of the original table and the table of master attribute values).
 */
DevStatus
TAttrLink::SetSlaveTable(ViewGraph *view)
{
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetSlaveTable(%s)\n", _name, view->GetName());
#endif

// TEMP: maybe we should always unlink slave view before calling this -- or
// delete the slave table and mapping for it

  DevStatus result = StatusOk;

  //
  // Make sure we have a table of the master attribute values.
  //
  if (_masterTableName == NULL) {
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
    DerivedTable *masterTable = _masterView->GetDerivedTable(_masterTableName);
    DOASSERT(masterTable != NULL, "No master table!");
    ost << " from " << tdata->GetName() << " as t1, " <<
	*masterTable->GetRelationId() <<
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
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::DestroyTable()\n", _name);
#endif

  if (_masterTableName != NULL) _masterView->DestroyDerivedTable(
      _masterTableName);
  delete [] _masterTableName;
  _masterTableName = NULL;

  return StatusOk;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::GetTData
 * Get the TData for the given view.
 */
TData *
TAttrLink::GetTData(ViewGraph *view, TDType tdType)
{
  DOASSERT(_objectValid, "operation on invalid object");
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
