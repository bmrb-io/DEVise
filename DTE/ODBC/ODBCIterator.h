#ifndef ODBC_ITERATOR_H
#define ODBC_ITERATOR_H

#include "Iterator.h"
#include "ODBC.h"

class ODBCIterator : public Iterator {
protected:
    string connectString;
	string query;
	Tuple* next;
	Tuple* typePtrs;
	DestroyPtr* destroyPtrs;
	int numFlds;
	TypeID* typeIDs;
	ODBC_Data* myODBC;
public:
	ODBCIterator(const string& dataSourceName, const string& query, int numFlds,
		const TypeID* typeIDs);

	virtual ~ODBCIterator(){
		destroyTuple(next, numFlds, destroyPtrs);
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
