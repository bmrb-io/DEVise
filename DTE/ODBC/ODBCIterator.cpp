#include "ODBCIterator.h"
#include "ODBC.h"

ODBCIterator::ODBCIterator(
	const string& connectString, const string& query, int numFlds,
	const TypeID* typeIDs)
	: connectString(connectString), query(query), numFlds(numFlds), destroyPtrs(NULL)
{
	this->typeIDs = new TypeID[numFlds];
	next = new Type*[numFlds];
	destroyPtrs = new DestroyPtr[numFlds];
	typePtrs = new void*[numFlds];
	for(int i = 0; i < numFlds; i++){
		this->typeIDs[i] = typeIDs[i];
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		next[i] = allocateSpace(typeIDs[i]);

		if(isInlined(typeIDs[i])){
			typePtrs[i] = &next[i];
		}
		else{
			typePtrs[i] = next[i];
		}
	}
}

void ODBCIterator::initialize(){

	int aa;
	myODBC = new ODBC_Data(connectString,query);
	TRY(myODBC->ODBC_Connect(),NVOID);  //Connect to ODBC driver

	TRY(myODBC->ODBC_Column_Info(),NVOID);  //Get Column Info for given query

	for (aa = 0; aa < (myODBC->col_Num); aa++) {
		if (myODBC->DTE_Types[aa] != typeIDs[aa]) {
			cerr << aa << " " << myODBC->DTE_Types[aa]  << " " << typeIDs[aa] << endl ;
			string err_what = "Column types don't match for ODBC" ;
			THROW(new Exception(err_what),NVOID);
		}
	}
	// Connect to ODBC
	// optionally, figure out the number and types of return
	// fields and make sure they agree with what we have

}

const Tuple* ODBCIterator::getNext(){

	// load real data into space pointed by typePtrs
	// getData(..., typePtrs[i], ...) // fill the typePtrs
//  return next if there is something else return NULL
	int aa;

	TRY(aa = myODBC->ODBC_Get_Rec(typePtrs),NULL);
	if ( aa > 0) {
		return next; 
	} else {
		return NULL;
	}
}
