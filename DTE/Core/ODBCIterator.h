#ifndef ODBC_ITERATOR_H
#define ODBC_ITERATOR_H

#include "Iterator.h"

class ODBCIterator : public Iterator {
protected:
     string dataSourceName;
     string userName;
     string passwd;
     string query;
	Tuple* next;
	void** typePtrs;
	DestroyPtr* destroyPtrs;
	int numFlds;
	TypeID* typeIDs;
public:
	ODBCIterator(const string& dataSourceName, const string& userName,
		const string& passwd, const string& query, int numFlds,
		const TypeID* typeIDs);

	virtual ~ODBCIterator(){
		destroyTuple(tuple, numFlds, destroyPtrs);
		delete [] typePtrs;
		delete [] typeIDs;
		delete [] destroyPtrs;
	}
     virtual void initialize();

     virtual const Tuple* getNext();

     virtual Offset getOffset(){
		assert(!"getOffset not supported for ODBC iterator");
		return Offset();
     }
     virtual const Tuple* getThis(Offset offset, RecId recId){
		assert(!"getThis not supported for ODBC iterator");
		return NULL;
	}
};

#endif
