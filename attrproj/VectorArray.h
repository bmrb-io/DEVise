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
  Header file for VectorArray class.
 */

/*
  $Id$

  $Log$
 */

#ifndef _VectorArray_h_
#define _VectorArray_h_

#include "DeviseTypes.h"

#include "iostream.h"	// Needed for vector.h.
#include "fstream.h"	// Needed for vector.h.
#include "vector.h"


class VectorArray
{
public:
	VectorArray(int vectorCount);
	~VectorArray();

	DevStatus Init(int vectorNum, int vectorDim);

	int GetVecCount();
	Vector * const GetVectors();

private:
	int			_vectorCount;
	Vector *	_vectors;
};


#endif /* _VectorArray_h_ */

/*============================================================================*/
