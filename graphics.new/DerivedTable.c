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
  Revision 1.6  1999/01/20 22:47:09  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

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

  return result;
}

/*============================================================================*/
