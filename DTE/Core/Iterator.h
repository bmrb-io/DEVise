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
     virtual void initialize(){}
     virtual const Tuple* getNext() = 0;
     virtual const Tuple* getThis(Offset offset, RecId recId){
		assert(!"getThis not supported for this iterator");
		return NULL;
	}
     virtual Offset getNextOffset(){
          assert(!"getNextOffset not supported for this iterator");
          return Offset();
     }
     virtual Offset getOffset(){
          assert(!"getOffset not supported for this iterator");
          return Offset();
     }
     virtual void reset(int lowRid, int highRid){
          String msg = "reset not implemented for this reader";
          THROW(new Exception(msg), );
     }
	virtual void finalize(){}
	virtual ~Iterator(){}
};

class PlanOp {
public:
	virtual ~PlanOp() {}
	virtual int getNumFlds() = 0;
	virtual const String *getTypeIDs() = 0;
	virtual const String *getAttributeNames() = 0;
	virtual String *getOrderingAttrib(){
		assert(0);
		return NULL;
	}
	virtual Stats* getStats(){

		// default stats

		return new Stats(getNumFlds());
	}
	virtual void open(istream* in){
		assert(!"open not implemented on some reader");
	}
	virtual ostream& display(ostream& out){
		out << "display not implemented for this reader" << endl;
		return out;
	}
	virtual void writeHeader(ostream& out){
		assert(0);
	}
	virtual Iterator* createExec() = 0;
};

#endif
