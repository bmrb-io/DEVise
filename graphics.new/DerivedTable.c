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
  Implementation of DerivedTable class.
 */

/*
  $Id$

  $Log$
 */

#include <stdio.h>

#include "DerivedTable.h"

#include "TData.h"
#include "AttrList.h"
#include "DevError.h"
#include "Util.h"

#include "types.h"
#include "Inserter.h"
#include "RelationManager.h"

//#define DEBUG

/*------------------------------------------------------------------------------
 * function: DerivedTable::DerivedTable
 * Constructor.
 */
DerivedTable::DerivedTable(char *name, TData *tdata, char *masterAttrName,
    DevStatus &result)
{
#if defined(DEBUG)
  printf("DerivedTable(0x%p)::DerivedTable(%s, %s, %s)\n", this, name,
      tdata->GetName(), masterAttrName);
#endif

  _objectValid = false;

  result = StatusOk;

  _name = CopyString(name);
  _tdataName = CopyString(tdata->GetName());
  _masterAttrName = CopyString(masterAttrName);

  _tableFile = NULL;
  _schema = NULL;
  _stdInt = NULL;
  _relId = NULL;
  _inserter = NULL;

  //
  // Find (hopefully) the master attribute in this tdata.
  //
  AttrList *tdAttrs = tdata->GetAttrList();
  AttrInfo *attrInfo = tdAttrs->Find(_masterAttrName);
  if (attrInfo == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't find attribute <%s>", _masterAttrName);
    reportErrNosys(errBuf);
    result = StatusFailed;
    return;
  }

  //
  // Find the TData record size and make sure it's something we can
  // deal with.
  //
  int recordSize = tdata->RecSize();
  if (recordSize < 0) {
    reportErrNosys("Can't deal with variable-size TData records");
    result = StatusFailed;
    return;
  } else if (recordSize == 0) {
    reportErrNosys("TData record size is zero");
    result = StatusFailed;
    return;
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
    result = StatusFailed;
    return;
    break;
  }
  attrs[0] = _masterAttrName;

  _schema = new ISchema(1, types, attrs);

  _tableFile = tempnam(Init::TmpDir(), "tdaln");
  if (_tableFile == NULL) {
    reportErrSys("Out of memory");
    result = StatusFailed;
    return;
  }

  _stdInt = new StandardInterface(*_schema, _tableFile);
  _relId = new RelationId;
  *_relId = RELATION_MNGR.registerNewRelation(*_stdInt);
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
    result = StatusFailed;
    return;
  }
#if defined(DEBUG)
  printf("  Created master table file %s\n", _tableFile);
#endif

  _objectValid = true;
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::~DerivedTable
 * Destructor.
 */
DerivedTable::~DerivedTable()
{
#if defined(DEBUG)
  printf("DerivedTable(%s)::~DerivedTable()\n", _name);
#endif

  _objectValid = false;

  delete [] _name;
  _name = NULL;
  delete [] _tdataName;
  _tdataName = NULL;
  delete [] _masterAttrName;
  _masterAttrName = NULL;

  RELATION_MNGR.deleteRelation(*_relId);

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
    delete _inserter;
    _inserter = NULL;
  }

  if (_tableFile != NULL) {
    unlink(_tableFile);
    delete [] _tableFile;
    _tableFile = NULL;
  }
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::Initialize
 * Initialize the table.
 */
DevStatus
DerivedTable::Initialize()
{
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("DerivedTable(%s)::Initialize()\n", _name);
#endif

  DevStatus result = StatusOk;

  _inserter = new UniqueInserter(*_schema, _tableFile, ios::out);
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
    result = StatusFailed;
  }

  _recordCount = 0;

  return result;
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::InsertValues
 * Insert values into the table.
 */
DevStatus
DerivedTable::InsertValues(TData *tdata, int recCount, void **tdataRecs)
{
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("DerivedTable(%s)::InsertValues()\n", _name);
#endif

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
 * function: DerivedTable::Done
 * Clean up table when insertions are done.
 */
DevStatus
DerivedTable::Done()
{
  DOASSERT(_objectValid, "operation on invalid object");
#if defined(DEBUG)
  printf("DerivedTable(%s)::Done()\n", _name);
#endif

  DevStatus result = StatusOk;

#if defined(DEBUG)
  printf("  %d records (including duplicates) were inserted into table %s\n",
      _recordCount, _name);
#endif

  //
  // Close and delete inserter.
  //
  _inserter->close();
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
    result = StatusFailed;
  }

  delete _inserter;
  _inserter = NULL;

  return result;
}

/*============================================================================*/
