#include "ODBCIterator.h"

ODBCIterator::ODBCIterator(
	const string& dataSourceName, const string& userName,
	const string& passwd, const string& query, int numFlds,
	const TypeID* typeIDs)
	: dataSourceName(dataSourceName), userName(userName),
	passwd(passwd), query(query), numFlds(numFlds) destroyPtrs(NULL)
{
	this->typeIDs = new TypeID[numFlds];
	next = new Tuple[numFlds];
	destroyPtrs = new DestroyPtr[numFlds];
	typePtrs = new void*[numFlds];
	for(int i = 0; i < numFlds; i++){
		this->typeIDs[i] = typeIDs[i];
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		tuple[i] = allocateSpace(typeIDs[i]);
		if(isInlined(typIDs[i]){
			typePtrs[i] = &tuple[i];
		}
		else{
			typePtrs[i] = tuple[i];
		}
	}
}

void ODBCIterator::initialize(){

	// Connect to ODBC
	// optionally, figure out the number and types of return
	// fields and make sure they agree with what we have

}

const Tuple* ODBCIterator::getNext(){

	// load real data into space pointed by typePtrs
	// getData(..., typePtrs[i], ...) // fill the typePtrs

	return next;
}
