#ifndef STANDARD_READ_H
#define STANDARD_READ_H

#include <iostream.h>
#include <String.h>
#include "GeneralRead.h"
#include "types.h"

class StandardRead : public GeneralRead {
protected:
	istream* in;
	int numFlds;
	String* typeIDs;
	String* attributeNames;
	ReadPtr* readPtrs;
	Stats* stats;
public:
     StandardRead(istream* in) : 
		in(in), numFlds(0), typeIDs(NULL), attributeNames(NULL),
		readPtrs(NULL), stats(NULL) {}
	void open();	// Throws exception
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual String *getTypeIDs(){
		return typeIDs;
	}
	virtual String *getAttributeNames(){
		return attributeNames;
	}
	virtual Tuple* getNext(){
		Tuple* tuple = new Tuple[numFlds];
          for(int i = 0; i < numFlds; i++){
			tuple[i] = (readPtrs[i])(*in);
		}
		if(*in){
			return tuple;
		}
		else{
			delete tuple;
			return NULL;
		}
	}
     virtual Stats* getStats(){
          return stats;
     }
	virtual ostream& display(ostream& out){
		out << "Num fields: " << numFlds << endl;
		out << "(";
		for(int i = 0; i < numFlds; i++){
			out << typeIDs[i] << " " << attributeNames[i];
			out << (i == numFlds - 1 ? "" : ", ");
		}
		out << ")";
		stats->display(out);
		return out;
	}
};

class NCDCRead : public StandardRead {
public:
     NCDCRead(istream* in) : StandardRead(in) {}
	void open();	// Throws exception
};

#endif
