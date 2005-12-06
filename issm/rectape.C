/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
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
  Revision 1.5.14.1  2005/09/12 19:42:20  wenger
  Got DEVise to compile on basslet.bmrb.wisc.edu (AMD 64/gcc
  4.0.1).

  Revision 1.5  1999/11/30 22:28:45  wenger
  Temporarily added extra debug logging to figure out Omer's problems;
  other debug logging improvements; better error checking in setViewGeometry
  command and related code; added setOpeningSession command so Omer can add
  data sources to the temporary catalog; added removeViewFromPile (the start
  of allowing piling of only some views in a window).

  Revision 1.4  1995/12/28 17:33:52  jussi
  Fixed comparisons between signed and unsigned integers.

  Revision 1.3  1995/11/02 16:52:45  jussi
  Updated copyright message.

  Revision 1.2  1995/09/22 15:49:46  jussi
  Added copyright message.

  Revision 1.1  1995/09/06 15:28:57  jussi
  Initial revision of archive.
*/

#include <iostream.h>
#include <stdlib.h>

#include "rectape.h"
#include "DevError.h"

/********************************************************************
* getrecm() - get records containing multiple data types  
*
* arguments:
*    varptr - address of beginning or FORTRAN common block (or 
*	      structure), containing the variables to be read.
*
*    map - integer*4 array indicating the data types for each
*	   variable:
*
*               first word indicates total number of values in     
*                   map array (including this one)
*		0 indicates INTEGER*4 value
*		1 indicates INTEGER*2 value
*		2 indicates REAL*4 value
*		3 indicates CHARACTER value. This must be 
*                 followed by the size of the character array 
*
*******************************************************************/

int RecTape::getrecm(char *varptr, int *map)
{
  const int bufSize = 1024;
  static char buf[bufSize];

  int len = getrec(buf);
  if (len > bufSize) {
    cerr << "Record size exceeds getrecm buffer size" << endl;
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    exit(1);
  }

  char *bufptr = buf;
  int varsiz = 0;

  for(int mapcnt = 1; mapcnt < map[0]; mapcnt++) {
    switch (map[mapcnt]) {
    case 0:		                // 0 means int*4
      varsiz = 4;	
#ifdef NEED_WORD_ALIGNED
      if ((long) varptr % varsiz)
	varptr += (varsiz - ((long) varptr % varsiz));
#endif
      memcpy(varptr, bufptr, varsiz);
      BinaryConversion::fourbyte(varptr, 1);
      bufptr += varsiz;
      varptr += varsiz;
      break;

    case 1:		                // 1 means int*2
      varsiz = 2;
#ifdef NEED_WORD_ALIGNED
      if ((long) varptr % varsiz)
	varptr += (varsiz - ((long) varptr % varsiz));
#endif
      BinaryConversion::twobyte(varptr, 1);
      memcpy(varptr, bufptr, varsiz);
      bufptr += varsiz;
      varptr += varsiz;
      break;

    case 2:		                // 2 means float*4
      varsiz = 4;	
#ifdef NEED_WORD_ALIGNED
      if ((long) varptr % varsiz)
	varptr += (varsiz - ((long) varptr % varsiz));
#endif
      memcpy(varptr, bufptr, varsiz);
      BinaryConversion::real(varptr, 1);
      bufptr += varsiz;
      varptr += varsiz;
      break;

    case 3:		                // 3 means character
      varsiz = (1 * map[++mapcnt]);
      memcpy(varptr, bufptr, varsiz);
      BinaryConversion::text(varptr, varsiz);
      BinaryConversion::nullTerm(varptr, varsiz);
      bufptr += varsiz;
      varptr += varsiz;
      break;

    default:
      cerr << "bad variable type in getrecm" << endl;
      reportErrNosys("Fatal error");//TEMP -- replace with better message
      exit(1);
    }
  }

  return len;
}

int RecTape::getrec(char *buf)
{
  int size1;
  int status = read((char *)&size1, sizeof size1);
  if (!status)
    return status;
  if (status < (int)(sizeof size1))
    return -1;

  BinaryConversion::fourbyte((char *)&size1, 1);

  if (size1 == -1)
    return -1;

  if (read(buf, size1) < size1)
    return -1;

  int size2;
  if (read((char *)&size2, sizeof size2) < (int)(sizeof size2))
    return -1;

  BinaryConversion::fourbyte((char *)&size2, 1);

  if (size1 != size2) {
    cerr << "Bad record (sizes don't match) in getrec" << endl;
    reportErrNosys("Fatal error");//TEMP -- replace with better message
    exit(1);
  }
  
  return size1;
}
