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

#include <stdio.h>

#include "BooleanArray.h"
#include "Exit.h"

#define DEBUG//TEMPTEMP
#define BOUNDS_CHECKING 1//TEMPTEMP?

#if !defined(lint) && defined(RCSID)
static char rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: BooleanArray::BooleanArray
 * TEMPTEMP
 */
BooleanArray::BooleanArray(int size)
{
  _size = size;
  _data = new Boolean[size];
  DOASSERT(_data, "Out of memory");
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::~BooleanArray
 * TEMPTEMP
 */
BooleanArray::~BooleanArray()
{
  delete [] _data;
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Clear
 * TEMPTEMP
 */
void
BooleanArray::Clear()
{
  int index;
  for (index = 0; index < _size; index++) {
    Set(index, false);
  }
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Set
 * TEMPTEMP
 */
void
BooleanArray::Set(int index, Boolean value)
{

#if BOUNDS_CHECKING
  DOASSERT(index >= 0 && index < _size, "Array bounds error");
#endif

  _data[index] = value;
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Get
 * TEMPTEMP
 */
Boolean
BooleanArray::Get(int index)
{

#if BOUNDS_CHECKING
  DOASSERT(index >= 0 && index < _size, "Array bounds error");
#endif

  return _data[index];
}

/*============================================================================*/
