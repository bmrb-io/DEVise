/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Implementation of VectorArray class.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1996/04/30 15:31:55  wenger
  Attrproj code now reads records via TData object; interface to Birch
  code now in place (but not fully functional).

 */

#define _VectorArray_c_

//#define DEBUG

#include <stdio.h>

#include "VectorArray.h"
#include "Util.h"

#if !defined(lint) && defined(RCSID)
static char		rcsid[] = "$RCSfile$ $Revision$ $State$";
#endif

static char *	srcFile = __FILE__;

/*------------------------------------------------------------------------------
 * function: VectorArray::VectorArray
 * VectorArray constructor.
 */
VectorArray::VectorArray(int vectorCount)
{
	DO_DEBUG(printf("VectorArray::VectorArray(%d)\n", vectorCount));

	_vectorCount = vectorCount;
	_vectors = new Vector[_vectorCount];
}

/*------------------------------------------------------------------------------
 * function: VectorArray::~VectorArray
 * VectorArray destructor.
 */
VectorArray::~VectorArray()
{
	DO_DEBUG(printf("VectorArray::~VectorArray()\n"));

	delete [] _vectors;
}

/*------------------------------------------------------------------------------
 * function: VectorArray::Init
 * Initialize a Vector in the VectorArray.
 */
DevStatus
VectorArray::Init(int vecNum, int vecDim)
{
	DO_DEBUG(printf("VectorArray::Init()\n"));

	DevStatus		result = StatusOk;

	_vectors[vecNum].Init(vecDim);

	return result;
}

/*------------------------------------------------------------------------------
 * function: VectorArray::GetVecCount
 * Return the number of vectors in the array.
 */
int
VectorArray::GetVecCount()
{
	DO_DEBUG(printf("VectorArray::GetVecCount()\n"));

	return _vectorCount;
}

/*------------------------------------------------------------------------------
 * function: VectorArray::GetVector
 * Return one of the vectors.
 */
Vector *
VectorArray::GetVector(int vecNum)
{
	DO_DEBUG(printf("VectorArray::GetVector()\n"));

	return &_vectors[vecNum];
}

/*============================================================================*/
