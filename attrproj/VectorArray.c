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
}

/*------------------------------------------------------------------------------
 * function: VectorArray::~VectorArray
 * VectorArray destructor.
 */
VectorArray::~VectorArray()
{
	DO_DEBUG(printf("VectorArray::~VectorArray()\n"));
}

/*------------------------------------------------------------------------------
 * function: VectorArray::Init
 * Initialize a Vector in the VectorArray.
 */
DevStatus
VectorArray::Init(int vectorNum, int vectorDim)
{
	DO_DEBUG(printf("VectorArray::Init()\n"));

	DevStatus		result = StatusOk;


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
 * function: VectorArray::GetVectors
 * Return the array of vectors.
 */
Vector * const
VectorArray::GetVectors()
{
	DO_DEBUG(printf("VectorArray::GetVectors()\n"));

	return _vectors;
}

/*============================================================================*/
