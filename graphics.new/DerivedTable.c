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
  Revision 1.5  1998/11/16 18:58:42  wenger
  Added options to compile without DTE code (NO_DTE), and to warn whenever
  the DTE is called (DTE_WARN).

  Revision 1.4  1998/08/17 18:51:49  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.3  1998/06/15 19:55:07  wenger
  Fixed bugs 338 and 363 (problems with special cases of set links).

  Revision 1.2  1998/05/06 22:04:52  wenger
  Single-attribute set links are now working except where the slave of
  one is the master of another.

  Revision 1.1  1998/04/29 17:53:50  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

 */

#include <stdio.h>
#include <unistd.h>

#include "DerivedTable.h"

#include "TData.h"
#include "AttrList.h"
#include "DevError.h"
#include "Util.h"

#if !defined(NO_DTE)
#include "types.h"
#include "Inserter.h"
#include "RelationManager.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteDateAdt.h"
#endif

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

  result = StatusOk;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  ClearAll();

  _name = CopyString(name);
  _tdataName = CopyString(tdata->GetName());
  _masterAttrName = CopyString(masterAttrName);

  //
  // Find (hopefully) the master attribute in this tdata.
  //
  AttrList *tdAttrs = tdata->GetAttrList();
  AttrInfo *attrInfo = tdAttrs->Find(_masterAttrName);
  if (attrInfo == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't find attribute <%s>", _masterAttrName);
    reportErrNosys(errBuf);
    result += StatusFailed;
    return;
  }

  //
  // Find the TData record size and make sure it's something we can
  // deal with.
  //
  int recordSize = tdata->RecSize();
  if (recordSize < 0) {
    reportErrNosys("Can't deal with variable-size TData records");
    result += StatusFailed;
    return;
  } else if (recordSize == 0) {
    reportErrNosys("TData record size is zero");
    result += StatusFailed;
    return;
  }

  //
  // Create the table.
  //
  DteTupleAdt tupAdt;
  vector<string> attrs(1);
  switch (attrInfo->type) {
  case IntAttr:
    tupAdt.push_back(DteIntAdt());
    break;

  case FloatAttr:
  case DoubleAttr:
    tupAdt.push_back(DteDoubleAdt());
    break;

  case StringAttr:
    tupAdt.push_back(DteStringAdt(attrInfo->length));
    break;

  case DateAttr:
    tupAdt.push_back(DteDateAdt());
     break;

  default:
    reportErrNosys("Illegal attribute type");
    result += StatusFailed;
    return;
    break;
  }
  attrs[0] = _masterAttrName;

  _schema = new ISchema(tupAdt, attrs);

  //
  // Create the (empty) table file right away, so that it can be referenced
  // by other relations before values get filled in.
  //
  result += CreateTable();
  if (!result.IsComplete()) return;
#endif

  _objectValid.Set();
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::DerivedTable
 * Constructor (creates dummy table).
 */
DerivedTable::DerivedTable(char *name, char *masterAttrName, DevStatus &result)
{
#if defined(DEBUG)
  printf("DerivedTable(0x%p)::DerivedTable(%s)\n", this, name);
#endif

  result = StatusOk;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  ClearAll();

  _name = CopyString(name);
  _masterAttrName = CopyString(masterAttrName);

  //
  // Create a dummy schema (integer type picked arbitrarily).
  //
  _schema = new ISchema();
  _schema->push_back(DteIntAdt(), _masterAttrName);

  //
  // Create the (empty) table file right away, so that it can be referenced
  // by other relations before values get filled in.
  //
  result += CreateTable();
  if (!result.IsComplete()) return;
#endif

  _objectValid.Set();
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::ClearAll
 * Utility for use by constructors and destructor.
 */
void
DerivedTable::ClearAll()
{
#if defined(DEBUG)
  printf("DerivedTable(0x%p)::ClearAll()\n", this);
#endif

  _name = NULL;
  _tdataName = NULL;
  _masterAttrName = NULL;
  _tableFile = NULL;
  _recordCount = 0;
  _schema = NULL;
  _stdInt = NULL;
  _relId = NULL;
  _inserter = NULL;
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::CreateTable
 * Utility for use by constructors.
 */
DevStatus
DerivedTable::CreateTable()
{
#if defined(DEBUG)
  printf("DerivedTable(0x%p)::CreateTable()\n", this);
#endif

  DevStatus result = StatusOk;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  _tableFile = tempnam(Init::TmpDir(), "tdaln");
  if (_tableFile == NULL) {
    reportErrSys("Out of memory");
    result += StatusFailed;
    return result;
  }

  int fd = open(_tableFile, O_WRONLY | O_CREAT, 0644);
  if (fd == -1) {
    char errBuf[2*MAXPATHLEN];
    sprintf(errBuf, "Unable to create table file %s", _tableFile);
    reportErrSys(errBuf);
    result += StatusFailed;
  } else {
    if (close(fd) != 0) {
      reportErrSys("Error closing table file");
      result += StatusWarn;
    }
  }

  _stdInt = new StandardInterface(*_schema, _tableFile);
  _relId = new RelationId;
  *_relId = RELATION_MNGR.registerNewRelation(*_stdInt);
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
    result += StatusFailed;
    return result;
  }
#if defined(DEBUG)
  printf("  Created master table file %s\n", _tableFile);
  cout << "    relation ID: " << *_relId << "\n";
#endif

#endif

  return result;
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

  delete [] _name;
  _name = NULL;
  delete [] _tdataName;
  _tdataName = NULL;
  delete [] _masterAttrName;
  _masterAttrName = NULL;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  RELATION_MNGR.deleteRelation(*_relId);
  delete _relId;
  _relId = NULL;

  delete _schema;
  _schema = NULL;

  delete _stdInt;
  _stdInt = NULL;

  if (_inserter != NULL) {
    delete _inserter;
    _inserter = NULL;
    if (currExcept) {
      cerr << currExcept->toString() << endl;
      currExcept = NULL;
    }
  }
#endif

  if (_tableFile != NULL) {
    unlink(_tableFile);
    delete [] _tableFile;
    _tableFile = NULL;
  }

  ClearAll();
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::Initialize
 * Initialize the table.
 */
DevStatus
DerivedTable::Initialize()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("DerivedTable(%s)::Initialize()\n", _name);
#endif

  DevStatus result = StatusOk;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  _inserter = new UniqueInserter(_tableFile, _schema->getAdt(), ios::out);
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
    result += StatusFailed;
  }

  _recordCount = 0;
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::InsertValues
 * Insert values into the table.
 */
DevStatus
DerivedTable::InsertValues(TData *tdata, int recCount, void **tdataRecs)
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("DerivedTable(%s)::InsertValues()\n", _name);
#endif

  DevStatus result = StatusOk;

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  //
  // Find offset and type of master attribute.
  //
  AttrList *tdAttrs = tdata->GetAttrList();
  AttrInfo *attrInfo = tdAttrs->Find(_masterAttrName);
  if (attrInfo == NULL) {
    char errBuf[256];
    sprintf(errBuf, "Can't find attribute <%s>", _masterAttrName);
    reportErrNosys(errBuf);
    result += StatusFailed;
  }

  //
  // Insert master attribute values into the table.
  //
  if (result.IsComplete()) {
    Tuple tuple[1];

    for (int recNum = 0; recNum < recCount; recNum++) {
      void *recordP = tdataRecs[recNum];
      void *attrP = (char *)recordP + attrInfo->offset;

      switch (attrInfo->type) {
        case IntAttr: {
	  int tmpInt;
	  memcpy((void *)&tmpInt, attrP, sizeof(tmpInt));
#if defined(DEBUG)
	  printf("  Inserting %d\n", tmpInt);
#endif
	  tuple[0] = DteIntAdt::getTypePtr(tmpInt);
          break;
        }

        case FloatAttr: {
	  float tmpFloat;
	  memcpy((void *)&tmpFloat, attrP, sizeof(tmpFloat));
	  double tmpDouble = (double) tmpFloat;
#if defined(DEBUG)
	  printf("  Inserting %g\n", tmpDouble);
#endif
	  tuple[0] = DteDoubleAdt::getTypePtr(tmpDouble);
          break;
	}

        case DoubleAttr: {
	  double tmpDouble;
	  memcpy((void *)&tmpDouble, attrP, sizeof(tmpDouble));
#if defined(DEBUG)
	  printf("  Inserting %g\n", tmpDouble);
#endif
	  tuple[0] = DteDoubleAdt::getTypePtr(tmpDouble);
          break;
	}

        case StringAttr: {
	  char *tmpStr = (char *)attrP;
	  tuple[0] = DteStringAdt::getTypePtr(tmpStr);
#if defined(DEBUG)
	  printf("  Inserting %s\n", tmpStr);
#endif
          break;
	}

        case DateAttr: {
          reportErrNosys("TAttrLinks not yet implemented for dates");//TEMP
          result += StatusFailed;//TEMP
          break;
	}

        default: {
          reportErrNosys("Illegal attribute type");
          result += StatusFailed;
          break;
	}
      }

      _inserter->insert(tuple);
    }
  }

  _recordCount += recCount;
#endif

  return result;
}

/*------------------------------------------------------------------------------
 * function: DerivedTable::Done
 * Clean up table when insertions are done.
 */
DevStatus
DerivedTable::Done()
{
  DOASSERT(_objectValid.IsValid(), "operation on invalid object");
#if defined(DEBUG)
  printf("DerivedTable(%s)::Done()\n", _name);
#endif

  DevStatus result = StatusOk;

#if defined(DEBUG)
  printf("  %d records (including duplicates) were inserted into table %s\n",
      _recordCount, _name);
#endif

#if defined(DTE_WARN)
  fprintf(stderr, "Warning: calling DTE at %s: %d\n", __FILE__, __LINE__);
#endif
#if !defined(NO_DTE)
  //
  // Close and delete inserter.
  //
  //_inserter->close();
  delete _inserter;
  _inserter = NULL;
  if (currExcept) {
    cerr << currExcept->toString() << endl;
    currExcept = NULL;
    result += StatusFailed;
  }

#endif

  return result;
}

/*============================================================================*/
