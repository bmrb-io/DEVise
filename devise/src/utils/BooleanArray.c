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
  Implementation of BooleanArray class.  We probably want to eventually
  change the implementation to use a bitmap instead of an array of
  Booleans.
 */

/*
  $Id$

  $Log$
  Revision 1.2  1998/02/13 15:52:01  wenger
  Changed ViewData to be based on old ViewScatter class instead of
  TDataViewX; ViewData now returns a list of the records drawn to
  the query processor; removed unused GDataBinX class.

  Revision 1.1  1998/02/10 21:13:45  wenger
  Changed signatures of ReturnGData() in QueryCallback and its subclasses
  to pass back lists of records drawn (not implemented yet); moved
  declaration of ViewGraph_QueryCallback from ViewGraph.h to ViewGraph.c.

 */

#include <stdio.h>

#include "BooleanArray.h"
#include "Exit.h"

//#define DEBUG
#define BOUNDS_CHECKING 1

#if !defined(lint) && defined(RCSID)
static char rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

/*------------------------------------------------------------------------------
 * function: BooleanArray::BooleanArray
 * Constructor.
 */
BooleanArray::BooleanArray(int size, int baseIndex)
{
  _size = size;
  _baseIndex = baseIndex;
  _data = new Boolean[size];
  DOASSERT(_data, "Out of memory");
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::~BooleanArray
 * Destructor.
 */
BooleanArray::~BooleanArray()
{
  delete [] _data;
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Clear
 * Clear the array (set everything to false).
 */
void
BooleanArray::Clear()
{
  int index;
  for (index = 0; index < _size; index++) {
    _data[index] = false;
  }
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Set
 * Set an array element.
 */
void
BooleanArray::Set(int index, Boolean value)
{
  index -= _baseIndex;

#if BOUNDS_CHECKING
  DOASSERT(index >= 0 && index < _size, "Array bounds error");
#endif

  _data[index] = value;
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Get
 * Get the value of an array element.
 */
Boolean
BooleanArray::Get(int index) const
{
  index -= _baseIndex;

#if BOUNDS_CHECKING
  DOASSERT(index >= 0 && index < _size, "Array bounds error");
#endif

  return _data[index];
}

/*------------------------------------------------------------------------------
 * function: BooleanArray::Print
 * Print the array.
 */
void
BooleanArray::Print()
{
  printf("BooleanArray:\n");

  int index;
  for (index = 0; index < _size; index++) {
    printf("  [%d] = %s\n", index + _baseIndex,
	_data[index] ? "true" : "false");
  }
}

/*============================================================================*/
