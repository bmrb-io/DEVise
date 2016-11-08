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
  Declaration of SlaveTable class (knows how to create tables for views
  that are slaves of TAttrLinks).
 */

/*
  $Id$

  $Log$
 */

#ifndef _SlaveTable_h_
#define _SlaveTable_h_


#include <sys/types.h>

#include "DevStatus.h"
#include "ObjectValid.h"

class ViewGraph;
class TData;
class RelationId;

class SlaveTable {
public:
  SlaveTable(ViewGraph *view);
  ~SlaveTable();

  DevStatus UpdateTData();

protected:
  DevStatus CreateRelation();
  DevStatus DestroyRelation();
  TData *CreateTData(char *name);
  DevStatus DestroyTData();

  ViewGraph *_view;
  TData *_tdata;
  RelationId *_relId;

private:
  ObjectValid _objectValid;
};

#endif // _SlaveTable_h_

/*============================================================================*/
