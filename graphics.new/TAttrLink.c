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
  Revision 1.9  1998/05/06 22:04:56  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.8  1998/04/30 14:24:20  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

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
  _objectValid.Set();
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

  (void) DestroyMasterTable();
  delete [] _masterAttrName;
  _masterAttrName = NULL;
  delete [] _slaveAttrName;
  _slaveAttrName = NULL;
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetFlag
 * Set flag (link type).
 */
void
TAttrLink::SetFlag(VisualFlag flag)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetMasterView(%s)\n", _name,
      view != NULL ? view->GetName() : "NULL");
#endif

  if (view != NULL && _masterView == view) return;

  (void) DestroyMasterTable();

  MasterSlaveLink::SetMasterView(view);
  //TEMP -- make sure master view doesn't cache GData

  if (view != NULL) {
    //
    // Create the table to hold the master attribute values.
    //
    (void) CreateMasterTable();
  } else {
    //
    // Update all slave views.
    //
    ReCreateSlaveTDatas();
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::InsertView
 * Insert a new slave view for this link.
 */
void
TAttrLink::InsertView(ViewGraph *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::InsertView(%s)\n", _name, view->GetName());
#endif

  MasterSlaveLink::InsertView(view);
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::DeleteView
 * Delete a view from this link.
 */
bool
TAttrLink::DeleteView(ViewGraph *view)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::DeleteView(%s)\n", _name, view->GetName());
#endif

  Boolean isMaster = (view == _masterView);

  bool result = MasterSlaveLink::DeleteView(view);

  if (result) {
    if (isMaster) {
      SetMasterView(NULL);
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
    (void) CreateMasterTable();
  } else {
    //
    // Make sure the master table has the same TData as the master view
    // (in case the vie
    //
    DerivedTable *masterTable = _masterView->GetDerivedTable(_masterTableName);
    DOASSERT(masterTable != NULL, "No master table!");
  
    if (masterTable->GetTDataName() != NULL && curTDName != NULL &&
        strcmp(masterTable->GetTDataName(), curTDName)) {
#if defined(DEBUG)
      printf("TAttrLink %s: master view (%s) has new tdata\n", GetName(),
          _masterView->GetName());
#endif
      (void) DestroyMasterTable();
      (void) CreateMasterTable();
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
  // Invalidate the TDatas of all slave views.
  //
  int index = InitIterator();
  while (More(index)) {
    ViewGraph *view = Next(index);
    TData *tdata = GetTData(view, TDataPhys);
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
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
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetLinkType(%d)\n", _name, type);
#endif

  reportErrNosys("Cannot change record link type of TData attribute link\n");
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::CreateMasterTable
 * Creates the table for storing the master attribute values.
 */
DevStatus
TAttrLink::CreateMasterTable()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::CreateMasterTable()\n", _name);
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
 * function: TAttrLink::DestroyMasterTable
 * Destroys the table for storing the master attribute values.
 */
DevStatus
TAttrLink::DestroyMasterTable()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("TAttrLink(%s)::DestroyMasterTable()\n", _name);
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

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetMasterAttr
 * Set the master attribute for this link.
 */
void
TAttrLink::SetMasterAttr(char *masterAttrName)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetMasterAttr(%s)\n", _name, masterAttrName);
#endif

  if (strcmp(masterAttrName, _masterAttrName)) {
    delete [] _masterAttrName;
    _masterAttrName = CopyString(masterAttrName);

    //
    // Force the master attribute table to be re-created for the new
    // master attribute.
    //
    (void) DestroyMasterTable();
    (void) CreateMasterTable();

    ReCreateSlaveTDatas();

    _masterView->Refresh();
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::SetSlaveAttr
 * Set the slave attribute for this link.
 */
void
TAttrLink::SetSlaveAttr(char *slaveAttrName)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::SetSlaveAttr(%s)\n", _name, slaveAttrName);
#endif

  if (strcmp(slaveAttrName, _slaveAttrName)) {
    delete [] _slaveAttrName;
    _slaveAttrName = CopyString(slaveAttrName);

    //
    // Force the physical TDatas of all slave views to be re-created with
    // the new slave attribute.
    //
    ReCreateSlaveTDatas();
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::ReCreateSlaveTDatas
 * Force all slave view TDatas to be re-created.
 */
void
TAttrLink::ReCreateSlaveTDatas()
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::ReCreateSlaveTDatas(%s)\n", _name);
#endif

  int index = InitIterator();
  while (More(index)) {
    ViewGraph *view = Next(index);
    view->TAttrLinkChanged();
  }
  DoneIterator(index);
}

/*============================================================================*/
