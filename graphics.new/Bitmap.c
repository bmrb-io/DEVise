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
  $Id$

  $Log$
  Revision 1.2  1995/09/05 22:14:05  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "Bitmap.h"

/**************************************************************
Create a new bitmap
**************************************************************/

Bitmap::Bitmap(int numBits)
{
  if (numBits <= 0) {
    fprintf(stderr, "Bitmap::Bitmap: numBits %d <= 0\n", numBits);
    Exit::DoExit(1);
  }
  
  _numBits = numBits;
  _bits = new char[ArraySize(_numBits)];
  ClearBitmap();
}

/******************************************************************
Set a bit in the bitmap
*******************************************************************/

void Bitmap::SetBit(int bitNum)
{
  if (bitNum < 0 || bitNum >= _numBits) {
    fprintf(stderr, "Bitmap::SetBit: invalid bit %d\n", bitNum);
    Exit::DoExit(1);
  }
  
  int index = ArrayIndex(bitNum);
  int bitPos = BitPos(bitNum);
  
  _bits[index] |= (1<< bitPos);
}

/******************************************************************
Clear a bit in the bitmap
*******************************************************************/

void Bitmap::ClearBit(int bitNum)
{
  if (bitNum < 0 || bitNum >= _numBits) {
    fprintf(stderr, "Bitmap::ClearBit: invalid bit %d\n", bitNum);
    Exit::DoExit(1);
  }
  
  int index = ArrayIndex(bitNum);
  int bitPos = BitPos(bitNum);
  
  _bits[index] &= (0xff ^ (1<< bitPos));
}

/******************************************************************
Test if a bit has been set in the bitmap 
*******************************************************************/

Boolean Bitmap::TestBit(int bitNum)
{
  if (bitNum < 0 || bitNum >= _numBits) {
    fprintf(stderr, "Bitmap::TestBit: invalid bit %d\n", bitNum);
    Exit::DoExit(1);
  }

  int index = ArrayIndex(bitNum);
  int bitPos = BitPos(bitNum);
  
  return (_bits[index] & (1<< bitPos));
}

/********************************************************************
Clear the whole bitmap
*********************************************************************/

void Bitmap::ClearBitmap()
{
  int arraySize = ArraySize(_numBits);
  for(int i = 0; i < arraySize; i++)
    _bits[i] = 0;
}
