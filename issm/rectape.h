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
  Revision 1.4  1996/04/16 20:56:38  jussi
  Replaced assert() calls with DOASSERT macro.

  Revision 1.3  1995/11/02 16:52:45  jussi
  Updated copyright message.

  Revision 1.2  1995/09/22 15:49:47  jussi
  Added copyright message.

  Revision 1.1  1995/09/06 15:28:58  jussi
  Initial revision of archive.
*/

#ifndef RECTAPE_H
#define RECTAPE_H

#include "tapedrive.h"
#include "binconv.h"
#include "Exit.h"

class RecTape : protected TapeDrive {
public:
  RecTape(char *name, char *mode, int fileno = -1,
	  int blockSize = TAPE_BLOCKSIZE) :
	TapeDrive(name, mode, fileno, blockSize) {}
  
  // Return 1 if properly initialized
  operator int() { return TapeDrive::initialized; }

  // return current byte offset on device file
  long long tell() { return TapeDrive::tell(); }

  // move to given byte offset on device file
  long long seek(long long pos) { return TapeDrive::seek(pos); }

  // get record of characters
  int getrecc(char *var) {
    int len = getrec(var);
    DOASSERT(len >= 0, "Invalid data record");
    BinaryConversion::text(var, len);
    return len;
  }

  // get record of short integers
  int getrec2(char *var) {
    int len = getrec(var);
    DOASSERT(len >= 0 && len % 2 == 0, "Invalid data record");
    BinaryConversion::twobyte(var, len / 2);
    return len;
  }

  // get record of integers
  int getrec4(char *var) {
    int len = getrec(var);
    DOASSERT(len >= 0 && len % 4 == 0, "Invalid data record");
    BinaryConversion::fourbyte(var, len / 4);
    return len;
  }

  // get record of floats
  int getrecr(char *var) {
    int len = getrec(var);
    DOASSERT(len >= 0 && len % 4 == 0, "Invalid data record");
    BinaryConversion::real(var, len / 4);
    return len;
  }

  // get mapped record
  int getrecm(char *var, int *map);
  
protected:
  int getrec(char *var);                // get next record
};

#endif
