/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1999-2002
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of DupElim class.
 */

/*
  $Id$

  $Log$
  Revision 1.3.10.3  2002/09/05 19:25:21  wenger
  Forgot to remove temporary comment.

  Revision 1.3.10.2  2002/09/05 19:24:48  wenger
  Oops -- I really *didn't* need to make the previous change -- backing
  out.

  Revision 1.3.10.1  2002/08/29 16:47:00  wenger
  Fixed bug in DupElim -- missing compare function for hash table.

  Revision 1.3  2001/04/03 19:57:39  wenger
  Cleaned up code dealing with GData attributes in preparation for
  "external process" implementation.

  Revision 1.2  1999/05/21 14:52:23  wenger
  Cleaned up GData-related code in preparation for including bounding box
  info.

  Revision 1.1  1999/03/12 18:46:04  wenger
  Implemented duplicate symbol elimination.

 */

#include <stdio.h>
#include <values.h>

#include "DupElim.h"
#include "ViewGraph.h"
#include "Exit.h"
#include "Transform.h"
#include "HashTable.h"

#define DEBUG 0

template class HashTable<int, int>;

struct XY {
  short x;
  short y;
};

union HashObj {
  int i;
  XY xy;
  char c[4];
};

static int DupHash(int &value, int numBuckets)
{
  HashObj ho;
  ho.i = value;

  unsigned hashVal = ho.c[0];
  hashVal ^= ho.c[1];
  hashVal ^= ho.c[2];
  hashVal ^= ho.c[3];

  hashVal %= numBuckets;

  return hashVal;
}

/*------------------------------------------------------------------------------
 * Constructor.
 */
DupElim::DupElim()
{
#if (DEBUG >= 1)
  printf("DupElim(0x%p)::DupElim()\n", this);
#endif

  _hashT = new HashTable<int, int>(1024, DupHash, NULL);

  _initialized = false;

  _valid.Set();
}

/*------------------------------------------------------------------------------
 * Destructor.
 */
DupElim::~DupElim()
{
  DOASSERT(_valid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("DupElim(0x%p)::~DupElim()\n", this);
#endif

  delete _hashT;
}

/*------------------------------------------------------------------------------
 * Initialize this object at the beginning of drawing a view.
 */
DevStatus
DupElim::Init(ViewGraph *view)
{
  DOASSERT(_valid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("DupElim(0x%p)::Init(%s)\n", this, view->GetName());
#endif

  DevStatus result = StatusOk;

  // Make sure we're doing this on a two-dimensional view.
  if (result.IsComplete() && view->GetNumDimensions() != 2) {
    reportErrNosys("Cannot do duplicate elimination on a non-two-dimensional "
        "view");
    result += StatusFailed;
  }

  // Find the offsets for the GData attributes we'll have to access.
  if (result.IsComplete()) {
    TDataMap *gdataMap = view->GetFirstMap();
    const GDataAttrOffset *offsets = gdataMap->GetGDataOffset();

    _xOffset = offsets->_xOffset;
    _yOffset = offsets->_yOffset;

    WindowRep *windowRep = view->GetWindowRep();
    _transform = windowRep->TopTransform();
  }

  if (_hashT->clear() != 0) {
    reportErrNosys("Error clearing hash table");
    result += StatusFailed;
  }

  if (result.IsComplete()) _initialized = true;

  return result;
}

/*------------------------------------------------------------------------------
 * Decide whether to draw a symbol, updating the hash table of where symbols
 * have already been drawn
 */
Boolean
DupElim::DrawSymbol(void *gdataRec)
{
  DOASSERT(_valid.IsValid(), "operation on invalid object");
#if (DEBUG >= 1)
  printf("DupElim(0x%p)::DrawSymbol()\n", this);
#endif

  Boolean result = true;

  if (_initialized) {

    // Note: we are assuming that the GData is aligned on double (8-bit)
    // boundaries.
    double *xP = (double *)((char *)gdataRec + _xOffset);
    double *yP = (double *)((char *)gdataRec + _yOffset);

    Coord pixX, pixY;
    _transform->Transform(*xP, *yP, pixX, pixY);

    if (pixX < 0.0 || pixX > MAXSHORT || pixY < 0.0 || pixY > MAXSHORT) {
      reportErrNosys("Pixel value overflow");
    } else {
      HashObj ho;

      // Round from Coord to int.
      ho.xy.x = (short)(pixX + 0.5);
      ho.xy.y = (short)(pixY + 0.5);

      // We're not actually doing anything with the value part of the hash
      // table.
      int tmpVal = 0;
      if (_hashT->lookup(ho.i, tmpVal) == 0) {
        result = false;
      } else {
        _hashT->insert(ho.i, tmpVal);
      }
    }
  }

#if (DEBUG >= 2)
  printf("  DupElim()::DrawSymbol() returning %d\n", result);
#endif
  return result;
}

/*============================================================================*/
