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
 */

#ifndef ITERATOR_H
#define ITERATOR_H

#include "types.h"

class Iterator {
public:
	virtual ~Iterator() {}
	virtual int getNumFlds() = 0;
	virtual String *getTypeIDs() = 0;
	virtual String *getAttributeNames() = 0;
	virtual WritePtr* getWritePtrs(){
		assert(!"getWritePtrs not implemented");
		return NULL;
	}
	virtual String *getOrderingAttrib(){
		assert(0);
	}
	virtual Tuple  *getNext() = 0;
	virtual Stats* getStats(){

		// default stats

		return new Stats(10);
	}
	virtual void open(){
		assert(!"open not implemented on some reader");
	}
	virtual ostream& display(ostream& out){
		out << "display not implemented for this reader" << endl;
		return out;
	}
     virtual void reset(int lowRid, int highRid){
		String msg = "reset not implemented for this reader";
		THROW(new Exception(msg), );
     }
	virtual Offset getNextOffset(){
		assert(!"getNextOffset not supported for this iterator");
		return Offset();
	}
	virtual Offset getOffset(){
		assert(!"getOffset not supported for this iterator");
		return Offset();
	}
	virtual void setOffset(Offset offset){
		assert(!"setOffset not supported for this iterator");
	}
	virtual void initialize(){
	}
	virtual void writeHeader(ostream& out){
		assert(0);
	}
};

#endif
