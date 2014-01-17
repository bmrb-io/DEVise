/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-2010
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
  Revision 1.5.14.1  2014/01/17 21:46:30  wenger
  Fixed a bunch of possible buffer overflows.

  Revision 1.5  2003/01/13 19:25:21  wenger
  Merged V1_7b0_br_3 thru V1_7b0_br_4 to trunk.

  Revision 1.4.14.1  2002/09/20 20:49:02  wenger
  More Purifying -- there are now NO leaks when you open and close
  a session!!

  Revision 1.4  1999/11/30 22:28:19  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.3  1996/01/12 15:12:46  jussi
  Replaced libc.h with stdlib.h. Added copyright notice.

  Revision 1.2  1995/09/05 22:14:05  jussi
  Added CVS header.
*/

#include <stdio.h>
#include <stdlib.h>

#include "Exit.h"
#include "Bitmap.h"
#include "DevError.h"
#include "Util.h"

/**************************************************************
Create a new bitmap
**************************************************************/

Bitmap::Bitmap(int numBits)
{
  if (numBits <= 0) {
	char errBuf[1024];
    int formatted = snprintf(errBuf, sizeof(errBuf),
	    "Bitmap::Bitmap: numBits %d <= 0\n", numBits);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
    Exit::DoExit(1);
  }
  
  _numBits = numBits;
  _bits = new char[ArraySize(_numBits)];
  ClearBitmap();
}

Bitmap::~Bitmap()
{
  delete [] _bits;
}

/******************************************************************
Set a bit in the bitmap
*******************************************************************/

void Bitmap::SetBit(int bitNum)
{
  if (bitNum < 0 || bitNum >= _numBits) {
	char errBuf[1024];
    int formatted = snprintf(errBuf, sizeof(errBuf),
	    "Bitmap::SetBit: invalid bit %d\n", bitNum);
    checkAndTermBuf2(errBuf, formatted);
	reportErrNosys(errBuf);
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
	char errBuf[1024];
    int formatted = snprintf(errBuf, sizeof(errBuf),
	    "Bitmap::ClearBit: invalid bit %d\n", bitNum);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
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
	char errBuf[1024];
    int formatted = snprintf(errBuf, sizeof(errBuf),
	    "Bitmap::TestBit: invalid bit %d\n", bitNum);
    checkAndTermBuf2(errBuf, formatted);
    reportErrNosys(errBuf);
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
