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
#include "DataSeg.h"//TEMPTEMP

#include "RelationManager.h"
#include "types.h"
#include "Inserter.h"
#include "CatalogComm.h"
#include "TDataDQLInterp.h"//TEMPTEMP?

//#define DEBUG

class SlaveViewInfo {
public:
  View *view;
  TDataMap *oldMap;
  //TEMPTEMP -- need to save relation ID of new slave table
};

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
#if defined(DEBUG) || 1 //TEMPTEMP
  printf("TAttrLink(%s)::SetMasterView(%s)\n", _name,
      view != NULL ? view->GetName() : "NULL");
#endif

  if (view != NULL && _masterView == view) return;

  MasterSlaveLink::SetMasterView(view);
  //TEMP -- make sure master view doesn't cache GData

  (void) DestroyTable();

  if (view != NULL) {
    //
    // Create the table to hold the master attribute values; set up any
    // slave views that have already been added to this link.
    //
    (void) CreateTable(view);
    int index = _viewList->InitIterator();
    while (_viewList->More(index)) {
      ViewGraph *slaveView = _viewList->Next(index);
      (void) SetSlaveTable(slaveView);
    }
    _viewList->DoneIterator(index);
  } else {
    //
    // Remove the mappings from all slave views.
    //
    int index = _viewList->InitIterator();
    while (_viewList->More(index)) {
      ViewGraph *slaveView = _viewList->Next(index);
      TDataMap *map = slaveView->GetFirstMap();
      if (map != NULL) {
	slaveView->RemoveMapping(map);
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
    (void) SetSlaveTable(view);
  }
}

/*------------------------------------------------------------------------------
 * function: TAttrLink::DeleteView
 * Delete a view from this link.
 */
bool
TAttrLink::DeleteView(ViewGraph *view)
{
#if defined(DEBUG) || 1 //TEMPTEMP
  printf("TAttrLink(%s)::DeleteView(%s)\n", _name, view->GetName());
#endif

  Boolean isMaster = (view == _masterView);

  bool result = MasterSlaveLink::DeleteView(view);

  if (result) {
    if (isMaster) {
      SetMasterView(NULL);
    } else {
      //
      // Set the mapping and TData of this view back to what they were
      // before it was linked; destroy the slave mapping.
      //
      int index = _slaveViewInfo.InitIterator();
      while (_slaveViewInfo.More(index)) {
        SlaveViewInfo *slaveInfo = _slaveViewInfo.Next(index);
        if (slaveInfo->view == view) {
          TDataMap *map = view->GetFirstMap();
	  if (map != NULL) {
            view->RemoveMapping(map);
            ClassDir *classDir = ControlPanel::Instance()->GetClassDir();
	    classDir->DestroyInstance(map->GetName());
	  }

//TEMPTEMP -- should we delete the slave mapping and TData?

	  //TEMPTEMP -- maybe make a new mapping with the old TData so
	  // we keep any changes made in the mapping
          view->InsertMapping(slaveInfo->oldMap, "");
	  _slaveViewInfo.DeleteCurrent(index);
	  break;
        }
      }
      _slaveViewInfo.DoneIterator(index);
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
      currExcept = NULL;
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

  //
  // Close and delete inserter.
  //
  _inserter->close();
  if (currExcept) {
    cerr << currExcept->toString() << endl;
  }

  delete _inserter;
  _inserter = NULL;

  //
  // Invalidate the TDatas of all slave views.
  //
  int index = InitIterator();
  while (More(index)) {
    ViewGraph *view = Next(index);
    TData *tdata = GetTData(view);
    //TEMPTEMP -- for some reason this also changes the slave's visual filter!
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
    currExcept = NULL;
    return StatusFailed;
  }
#if defined(DEBUG)
  printf("  Created table file %s\n", _tableFile);
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
#if defined(DEBUG) || 1 //TEMPTEMP
  printf("TAttrLink(%s)::SetSlaveTable(%s)\n", _name, view->GetName());
#endif

//TEMPTEMP -- setting master several times causes original TData for
//slaves to be lost
// maybe we should always unlink slave view before calling this -- or
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
    tdata = GetTData(view);
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
  memset((void *)slaveTableName, 0, nameSize);//TEMPTEMP?
  if (result.IsComplete()) {
    const int querySize = 1024;
    char query[querySize];
    memset((void *)query, 0, querySize);//TEMPTEMP?
    ostrstream ost(query, querySize);

#if 1 //TEMPTEMP
//TEMPTEMP -- do we want recId in here?
    ost << "select ";

    AttrList *tdAttrs = tdata->GetAttrList();
    tdAttrs->InitIterator();
    while (tdAttrs->More()) {
      AttrInfo *attr = tdAttrs->Next();
      ost << "t1." << attr->name << ", ";
    }
    tdAttrs->DoneIterator();
    ost << " from " << tdata->GetName() << " as t1, " << *_relId <<
        " as t2 where t1." << _slaveAttrName << " = t2." << _masterAttrName;
#endif




#if 0
//TEMPTEMP -- Donko says not to use select *
#if 0 //TEMPTEMP
    ost << "select * from " << tdata->GetName() << " as t1, " << *_relId <<
	" as t2 where t1." << _slaveAttrName << " = t2." << _masterAttrName;
#else
    ost << "select t1.\"recId\", t1.\"X\", t1.\"Y\", t1.\"Color\", t1.\"Name\" from "
	<< tdata->GetName() << " as t1, " << *_relId <<
	" as t2 where t1." << _slaveAttrName << " = t2." << _masterAttrName;
#endif
#endif
    printf("  query = %s\n", query);//TEMPTEMP

#if 0 //TEMPTEMP
    int numFlds = 5;//TEMPTEMP
    string *attributeNames = new string[5];
    attributeNames[0] = "recId";
    attributeNames[1] = "X";
    attributeNames[2] = "Y";
    attributeNames[3] = "Color";
    attributeNames[4] = "Name";
#endif
#if 1 //TEMPTEMP
    int numFlds = tdAttrs->NumAttrs();
    string *attributeNames = new string[numFlds];

//TEMPTEMP -- do we want recId in here?
    int count = 0;
    tdAttrs->InitIterator();
    while (tdAttrs->More()) {
      AttrInfo *attr = tdAttrs->Next();
      attributeNames[count] = attr->name;
      count++;
    }
    tdAttrs->DoneIterator();
#endif
    ViewInterface vi(numFlds, attributeNames, query);

#if 1 //TEMPTEMP
    RelationId newRelId = RELATION_MNGR.registerNewRelation(vi);
    if (currExcept) {
      cerr << currExcept->toString() << endl;
      currExcept = NULL;
      return StatusFailed;
    }

    ostrstream nost(slaveTableName, nameSize);
    nost << newRelId;
#endif

#if 0 //TEMPTEMP
    sprintf(slaveTableName, "slave_table");//TEMPTEMP
    char buf[1024];
    sprintf(buf, "\"%s\" SQLView 4 X Y Color Name \"%s\"", slaveTableName, query);
    dteInsertCatalogEntry(".", buf);
#endif
/*TEMPTEMP*/printf("  newRelId: <%s>\n", slaveTableName);
#if 0 //TEMPTEMP
    DataSeg::Set(slaveTableName, "", 0, 0);
    char query2[1024];
    sprintf(query2, "select * from %s as t", slaveTableName);
    TData *newTData = new TDataDQLInterp(slaveTableName, NULL, query2);
    printf("newTData = 0x%p\n", newTData);
#endif
  }

  //
  // Remove the slave view's original mapping and TData and substitute the
  // new mapping and TData.
  //
  TDataMap *map;
  ClassDir *classDir;
  ClassInfo *classInfo;
  if (result.IsComplete()) {
    map = view->GetFirstMap();
    view->RemoveMapping(map);

    classDir = ControlPanel::Instance()->GetClassDir();
    classInfo = classDir->FindClassInfo(map->GetName());
    if (classInfo == NULL) {
      reportErrNosys("Can't find class info for existing mapping");
      result = StatusFailed;
    }
  }
  if (result.IsComplete()) {
    int argc;
    char ** argv;
    classDir->CreateParams(classInfo->CategoryName(), classInfo->ClassName(),
	map->GetName(), argc, argv);

    // argv[0] is TData name, argv[1] is mapping name.
#if 0 //TEMPTEMP
    argv[0] = ".testcolors2_table";//TEMPTEMP
#else
    argv[0] = slaveTableName;
#endif
    char mapNameBuf[1024];
    sprintf(mapNameBuf, "%s_slave", argv[1]);
    argv[1] = mapNameBuf;

/*TEMPTEMP*/printf("%s: %d\n", __FILE__, __LINE__);
    char *newMapName = classDir->CreateWithParams(
	classInfo->CategoryName(), classInfo->ClassName(), argc, argv);
/*TEMPTEMP*/printf("%s: %d\n", __FILE__, __LINE__);
    if (newMapName == NULL) {
      reportErrNosys("Can't create new mapping");
      result = StatusFailed;
    } else {
      TDataMap *newMap = (TDataMap *)classDir->FindInstance(newMapName);
      if (newMap == NULL) {
        reportErrNosys("Can't find new mapping instance");
        result = StatusFailed;
      } else {
        view->InsertMapping(newMap, "");
	view->Refresh();
      }
    }
  }

  //
  // Save the info we need to unlink the slave view.
  //
  if (result.IsComplete()) {
    SlaveViewInfo *slaveInfo = new SlaveViewInfo;
    if (slaveInfo == NULL) {
      reportErrSys("Out of memory");
      result = StatusFailed;
    } else {
      slaveInfo->view = view;
      slaveInfo->oldMap = map;
      _slaveViewInfo.Insert(slaveInfo);
    }
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
TAttrLink::GetTData(ViewGraph *view)
{
#if defined(DEBUG)
  printf("TAttrLink(%s)::GetTData(%s)\n", _name, view->GetName());
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
