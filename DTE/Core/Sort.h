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
     virtual Tuple  *getNext();
};

#endif
