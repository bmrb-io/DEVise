/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:14:06  jussi
  Added CVS header.
*/

#ifndef Bitmap_h
#define Bitmap_h
#include "DeviseTypes.h"

class Bitmap{
public:
	/* Init bitmap with given number of bits.
	Bits are numbered 0..numBits-1*/
	Bitmap(int numBits);

	/* set a bit */
	void SetBit(int bitNum);

	/* clear a bit */
	void ClearBit(int bitNum);

	/* Test if a bit has been set */
	Boolean TestBit(int bitNum);

	/* Clear bitmap */
	void ClearBitmap();
private:
	/* find the array index given bitNum */
	int ArrayIndex(int bitNum) { return bitNum / 8; };

	/* Find the position within array given bitNum */
	int BitPos(int bitNum) { return bitNum % 8; };

	/* Return size of array given # of bits */
	int ArraySize(int numBits){ return numBits/8+1; };

	int _numBits;	/* # of bits */
	char *_bits;	/* array of bits */
};

#endif
