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
  Revision 1.4  1999/01/20 22:47:14  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

  Revision 1.3  1998/11/16 18:58:48  wenger
  Added options to compile without DTE code (NO_DTE), and to warn whenever
  the DTE is called (DTE_WARN).

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

  return NULL;
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
