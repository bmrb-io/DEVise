/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of BooleanArray class (an array of Booleans with bounds
  checking, etc.).
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/02/13 15:52:02  wenger
  Changed ViewData to be based on old ViewScatter class instead of
  TDataViewX; ViewData now returns a list of the records drawn to
  the query processor; removed unused GDataBinX class.

  Revision 1.1  1998/02/10 21:13:45  wenger
  Changed signatures of ReturnGData() in QueryCallback and its subclasses
  to pass back lists of records drawn (not implemented yet); moved
  declaration of ViewGraph_QueryCallback from ViewGraph.h to ViewGraph.c.

 */

#ifndef _BooleanArray_h_
#define _BooleanArray_h_

#include <sys/types.h>

#include "DeviseTypes.h"

class BooleanArray {
public:
  BooleanArray(int size, int baseIndex = 0);
  ~BooleanArray();

  void Clear();

  void Set(int index, Boolean value);
  Boolean Get(int index);

  void Print();

private:
  int _size;
  int _baseIndex;
  Boolean *_data;
};

#endif /* _BooleanArray_h_ */

/*============================================================================*/
