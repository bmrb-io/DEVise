#ifndef GENERAL_READ_H
#define GENERAL_READ_H

#include "types.h"

class GeneralRead {
public:
	virtual int getNumFlds() = 0;
	virtual String *getTypeIDs() = 0;
	virtual String *getAttributeNames() = 0;
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
};

#endif
