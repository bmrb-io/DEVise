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
  Revision 1.3  1997/03/28 16:07:27  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef SORT_H
#define SORT_H

#include "Iterator.h"

class Sort : public Iterator {
Iterator* input;
public:
     virtual ~Sort() {}
     virtual int getNumFlds(){
		return input->getNumFlds();
	}
     virtual String *getTypeIDs() = 0;
     virtual String *getAttributeNames() = 0;
     virtual WritePtr* getWritePtrs(){
          assert(!"getWritePtrs not implemented");
          return NULL;
     }
     virtual String *getOrderingAttrib(){
          assert(0);
     }
	virtual void initialize();
     virtual bool getNext(Tuple* next);
};

#endif
