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
  Implementation of SlaveTable class.

  Note: in a "logical" sense this code is kind of part of the ViewGraph
  class.  However, I wanted to physically separate it so that the ViewGraph
  code is not "contaminated" with details about TData queries, DTE interface,
  etc.  RKW 4/30/1998.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/06/15 19:55:20  wenger
  Fixed bugs 338 and 363 (problems with special cases of set links).

  Revision 1.1  1998/05/06 22:04:55  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

 */

#include <stdio.h>

#include "SlaveTable.h"
#include "ViewGraph.h"
#include "TDataMap.h"
#include "TData.h"
#include "TAttrLink.h"
#include "Session.h"
#include "DerivedTable.h"
#include "DataSeg.h"

#if !defined(NO_DTE)
  #include "RelationManager.h"
  #include "types.h"
  #include "CatalogComm.h"
  #include "TDataDQLInterp.h"
#endif

//#define DEBUG

/*------------------------------------------------------------------------------
 * function: SlaveTable::SlaveTable()
 * Constructor.
 */
SlaveTable::SlaveTable(ViewGraph *view)
{
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::SlaveTable(%s)\n", this, view->GetName());
#endif

  _view = view;
  _tdata = NULL;
  _relId = NULL;

  _objectValid.Set();
}

/*------------------------------------------------------------------------------
 * function: SlaveTable::~SlaveTable()
 * Destructor.
 */
SlaveTable::~SlaveTable()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::~SlaveTable()\n", this);
#endif

  DestroyTData();
  DestroyRelation();
  _view = NULL;
}

/*------------------------------------------------------------------------------
 * function: SlaveTable::UpdateTData()
 * Update a slave view's TData according to the links that it's a slave of.
 */
DevStatus
SlaveTable::UpdateTData()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::UpdateTData()\n", this);
  printf("  updating physical TData for view %s\n", _view->GetName());
#endif

  DevStatus result = StatusOk;

  //
  // See whether the view is the slave of a TAttrLink.
  //
  Boolean hasTAttrLink = false;
  int index = _view->_slaveLink.InitIterator();
  while (_view->_slaveLink.More(index)) {
    MasterSlaveLink *link = _view->_slaveLink.Next(index);
    if (link->GetFlag() == VISUAL_TATTR) {
      hasTAttrLink = true;
      break;
    }
  }
  _view->_slaveLink.DoneIterator(index);

  //
  // Figure out whether we have to do anything to the physical TData (if
  // physical TData is older than TAttrLink timestamp).
  //
  TDataMap *map = _view->GetFirstMap();
  if (map != NULL &&
      _view->_slaveTAttrTimestamp > map->GetPhysTdTimestamp()) {

    //
    // If the existing physical TData is not the same as the logical
    // TData, destroy it and set the physical TData to the logical
    // TData -- if there are no TAttrLinks, it will stay that way.
    //
    if (result.IsComplete()) {
      if (map->GetLogTData() != map->GetPhysTData() &&
        map->GetPhysTData() == _tdata) {
        map->SetPhysTData(map->GetLogTData());
        result += DestroyTData();
      }
    }

    //
    // Destroy the existing relation and physical TData.
    //
    if (result.IsComplete()) {
      result += DestroyRelation();
    }

    //
    // Now create a new relation and physical TData if necessary.
    //
    if (result.IsComplete() && hasTAttrLink) {
      result += CreateRelation();
    }
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: SlaveTable::CreateRelation()
 * Create a new relation for the physical TData of the slave view.
 */
DevStatus
SlaveTable::CreateRelation()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::CreateRelation()\n", this);
#endif

  DevStatus result = StatusOk;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  //
  // Make sure the slave view has a TData already defined, and that the
  // TData is a Table (as opposed to a UNIXFILE, or whatever).
  //
  TData *oldTdata;
  if (result.IsComplete()) {
    oldTdata = TAttrLink::GetTData(_view, TDataLog);
    if (oldTdata == NULL) result = StatusFailed;
  }
  if (result.IsComplete()) {
    if (strcmp(oldTdata->DispatchedName(), "TDataDQL")) {
      reportErrNosys("Slave view of TAttrLink must have Table data source");
      result = StatusFailed;
    }
  }

  const int subQSize = 1024;
  char selectBuf[subQSize]; // holds 'select' part of query
  char fromBuf[subQSize]; // holds 'from' part of query
  char whereBuf[subQSize]; // holds 'where' part of query

  int numFlds;
  string *attributeNames;

  //
  // Generate the 'select' part of the query  and the list of attributes
  // for the new relation (these don't depend on the master link(s).
  //
  {
    memset((void *)selectBuf, 0, subQSize);
    ostrstream ostSelect(selectBuf, subQSize);

    ostSelect << "select ";

    AttrList *tdAttrs = oldTdata->GetAttrList();
    numFlds = tdAttrs->NumAttrs() - 1;
    attributeNames = new string[numFlds];

    int count = 0;
    tdAttrs->InitIterator();
    while (tdAttrs->More()) {
      AttrInfo *attr = tdAttrs->Next();

      // Don't put recId into query.
      if (strcmp(attr->name, "recId")) {
        if (count > 0) ostSelect << ", ";
        ostSelect << "t1." << attr->name;
        attributeNames[count] = attr->name;
        count++;
      }
    }
    tdAttrs->DoneIterator();

  } // force ostSelect out of scope
#if defined(DEBUG)
  printf("  selectBuf = '%s'\n", selectBuf);
#endif


  //
  // Find all TAttrLinks that are masters of the given view, and
  // generate the 'from' and 'where' parts of the query.
  //
  if (result.IsComplete()) {
    memset((void *)fromBuf, 0, subQSize);
    ostrstream ostFrom(fromBuf, subQSize);
    ostFrom << " from " << oldTdata->GetName() << " as t1";

    memset((void *)whereBuf, 0, subQSize);
    ostrstream ostWhere(whereBuf, subQSize);
    ostWhere << " where ";

    int masterCount = 0;
    int index = _view->_slaveLink.InitIterator();
    while (_view->_slaveLink.More(index)) {
      MasterSlaveLink *link = _view->_slaveLink.Next(index);
#if defined(DEBUG)
      printf("  link %s\n", link->GetName());
#endif
      if (link->GetFlag() == VISUAL_TATTR) {
#if defined(DEBUG)
        printf("  TAttrLink %s\n", link->GetName());
#endif
	masterCount++;
	// Bad cast here -- should change MasterSlaveLink class to avoid
	// this.  RKW May 5, 1998.
        TAttrLink *taLink = (TAttrLink *)link;

        //
        // Make sure the link has a table of the master attribute values.
        //
        DerivedTable *masterTable = taLink->GetMasterTable();
        if (masterTable == NULL) {
          char errBuf[256];
          sprintf(errBuf, "No master table for link %s", taLink->GetName());
          reportErrNosys(errBuf);
          result = StatusFailed;
        } else {
	  ostFrom << ", " << *masterTable->GetRelationId() << " as t" <<
	      masterCount + 1;

	  // Multiple links give intersection...
	  if (masterCount > 1) ostWhere << " and ";
          ostWhere << "t1." << taLink->GetSlaveAttrName() << " = t" <<
	      masterCount + 1 << "." << taLink->GetPhysMasterAttrName();
	}
      }
    }
    _view->_slaveLink.DoneIterator(index);
  }

  //
  // Create a relation that has the query we just generated.
  //
  const int nameSize = 128;
  char slaveTableName[nameSize];

  if (result.IsComplete()) {
#if defined(DEBUG)
  printf("  fromBuf = '%s'\n", fromBuf);
  printf("  whereBuf = '%s'\n", whereBuf);
#endif

    char queryBuf[3*subQSize];
    sprintf(queryBuf, "%s%s%s", selectBuf, fromBuf, whereBuf);
#if defined(DEBUG)
    printf("  slave table query = '%s'\n", queryBuf);
#endif

    ViewInterface vi(numFlds, attributeNames, queryBuf);

    // Create the table.
    _relId = new RelationId;
    *_relId = RELATION_MNGR.registerNewRelation(vi);
    if (currExcept) {
      cerr << currExcept->toString() << endl;
      currExcept = NULL;
      result += StatusFailed;
    } else {
      memset((void *)slaveTableName, 0, nameSize);
      ostrstream ostName(slaveTableName, nameSize);
      ostName << *_relId;
    } // force ostName out of scope

#if defined(DEBUG)
    printf("  _relId: %s\n", slaveTableName);
#endif
  }

  //
  // Change the view's mapping's physical TData to the new TData.
  //
  if (result.IsComplete()) {
    TDataMap *map;
    if (result.IsComplete()) {
      map = _view->GetFirstMap();
      map->SetPhysTData(CreateTData(slaveTableName));
    }
  }
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: SlaveTable::DestroyRelation()
 * Destroy the relation (remove it from the DTE's tables).
 */
DevStatus
SlaveTable::DestroyRelation()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::DestroyRelation()\n", this);
#endif

  DevStatus result = StatusOk;

#if defined(DEBUG)
    printf("  _relId = 0x%p\n", _relId);
#endif

  // We can't have the TData without the relation.
  result += DestroyTData();

  if (_relId != NULL) {
#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)

  #if defined(DEBUG)
    cout << "  deleting relation " << *_relId << "\n";
  #endif
    RELATION_MNGR.deleteRelation(*_relId);
    delete _relId;
    _relId = NULL;
#endif
  }

  return result;
}

/*------------------------------------------------------------------------------
 * function: SlaveTable::CreateTData()
 * Create an actual TData object for the relation we've defined.
 */
TData *
SlaveTable::CreateTData(char *name)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::CreateTData(%s)\n", this, name);
#endif

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  (void) DestroyTData();

  DataSeg::Set(name, NULL, 0, 0);
  _tdata = new TDataDQLInterp(name, NULL, NULL);
  if (_tdata == NULL) {
    char errBuf[1024];
    sprintf(errBuf, "Unable to create slave TData %s", name);
    reportErrNosys(errBuf);
  } else {
    // So slave's visual filter doesn't get changed each time master updates.
    _tdata->SetGoHomeOnInvalidate(false);

    // Make sure everything gets updated correctly.
    _tdata->InvalidateTData();
  }

  return _tdata;
#else
  return NULL;
#endif
}

/*------------------------------------------------------------------------------
 * function: SlaveTable::DestroyTData()
 * Destroy the TData object.
 */
DevStatus
SlaveTable::DestroyTData()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("SlaveTable(0x%p)::DestroyTData()\n", this);
#endif

  DevStatus result = StatusOk;

#if defined(DEBUG)
    printf("  _tdata = 0x%p\n", _tdata);
#endif

  if (_tdata != NULL) {
#if defined(DEBUG)
    printf("  destroying TData %s (0x%p)\n", _tdata->GetName(), _tdata);
#endif
    delete _tdata;
    _tdata = NULL;
  }

  return result;
}

/*============================================================================*/
