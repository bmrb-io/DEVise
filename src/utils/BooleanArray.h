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
  Description of module.
  TEMPTEMP
 */

/*
  $Id$

  $Log$
 */

#ifndef _BooleanArray_h_
#define _BooleanArray_h_

#include <sys/types.h>

#include "DeviseTypes.h"

class BooleanArray {
public:
  BooleanArray(int size);
  ~BooleanArray();

  void Clear();

  void Set(int index, Boolean value);
  Boolean Get(int index);

private:
  int _size;
  //TEMPTEMP -- convert to bitmap?
  Boolean *_data;
};

#endif /* _BooleanArray_h_ */

/*============================================================================*/
