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
