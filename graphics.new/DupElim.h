/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of DupElim class -- enables us to avoid drawing multiple
  duplicate symbols in a view.
 */

/*
  $Id$

  $Log$
 */

#ifndef _DupElim_h_
#define _DupElim_h_

#include <sys/types.h>

#include "ObjectValid.h"
#include "DevStatus.h"
#include "HashTable.h"

class ViewGraph;
class Transform2D;

class DupElim {
public:
  DupElim();
  ~DupElim();

  DevStatus Init(ViewGraph *view);

  Boolean DrawSymbol(void *gdataRec);

private:
  ObjectValid _valid;

  Boolean _initialized;

  int _xOffset; // offset into GData record of X value
  int _yOffset; // offset into GData record of Y value

  Transform2D *_transform;

  HashTable<int, int> *_hashT;
};

#endif // _DupElim_h_

/*============================================================================*/
