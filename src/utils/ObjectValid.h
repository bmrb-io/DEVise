/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1998
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  Declaration of ObjectValid class (used to test the validity of objects).

  To do this, a class for which checking is to be done should have a
  _private_ member of type ObjectValid; this member should be set in
  the constructor.  All non-static methods (at least non-private ones)
  should assert <flag>.IsValid() before they do anything else.
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/05/07 15:18:22  wenger
  This damn file somehow didn't get committed.

 */

#ifndef _ObjectValid_h_
#define _ObjectValid_h_


#include <sys/types.h>

class ObjectValid {
public:
  ObjectValid() { Clear(); }
  ~ObjectValid() { Clear(); }

  int IsValid() { return _status == _valid; }
  void Set() { _status =  _valid; }
  void Clear() { _status = 0; }

private:
  static const int _valid = 0x5b7aa1f9; // a hopefully unusual series of bits
  int _status;
};

#endif // _ObjectValid_h_

/*============================================================================*/
