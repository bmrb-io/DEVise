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
  Declaration of DerivedTable class (tables derived from views, used at
  least initially for TAttrLinks).
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/04/30 14:24:18  wenger
  DerivedTables are now owned by master views rather than links;
  views now unlink from master and slave links in destructor.

  Revision 1.1  1998/04/29 17:53:50  wenger
  Created new DerivedTable class in preparation for moving the tables
  from the TAttrLinks to the ViewDatas; found bug 337 (potential big
  problems) while working on this.

 */

#ifndef _DerivedTable_h_
#define _DerivedTable_h_


#include <sys/types.h>

#include "DevStatus.h"
#include "ObjectValid.h"

class TData;
class ISchema;
class StandardInterface;
class RelationId;
class UniqueInserter;

class DerivedTable {
public:
  DerivedTable(char *name, TData *tdata, char *masterAttrName,
      DevStatus &result);
  virtual ~DerivedTable();

  virtual char *GetName() { return _name; }

  virtual DevStatus Initialize();
  virtual DevStatus InsertValues(TData *tdata, int recCount, void **tdataRecs);
  virtual DevStatus Done();

  virtual char *GetTDataName() { return _tdataName; }
  virtual RelationId *GetRelationId() { return _relId; }

protected:
  char *_name;

  char *_tdataName; // name of TData from which this table is derived

  char *_masterAttrName;

  char *_tableFile; // file containing table tuples
  int _recordCount; // number of records inserted into table

  ISchema *_schema;
  StandardInterface *_stdInt;
  RelationId *_relId;
  UniqueInserter *_inserter;

private:
  ObjectValid _objectValid;
};

#endif // _DerivedTable_h_

/*============================================================================*/
