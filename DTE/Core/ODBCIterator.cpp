#include "ODBCIterator.h"
#include "ODBC.h"

ODBCIterator::ODBCIterator(
	const string& dataSourceName, const string& userName,
	const string& passwd, const string& tableName, const string& query, int numFlds,
	const TypeID* typeIDs)
	: dataSourceName(dataSourceName), userName(userName),
	passwd(passwd), tableName(tableName), query(query), numFlds(numFlds), destroyPtrs(NULL)
{
	this->typeIDs = new TypeID[numFlds];
	next = new Tuple[numFlds];
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
	myODBC = new ODBC_Data(dataSourceName,userName,passwd,tableName);
	TRY(aa = myODBC->ODBC_Connect(),NVOID);  //Connect to ODBC driver
	TRY(aa = myODBC->ODBC_Stmt_Handle(),NVOID);  //Allocate handle for statement

	Col_Max = TRY(myODBC->ODBC_Get_ColNums(),NVOID);  //Get Number of Columns

	Col_Types = new short [Col_Max];
	DTE_Types = new string [Col_Max];
	Col_Attr_Names = new string [Col_Max];
	Col_Lengths = new SQLINTEGER [Col_Max];

	TRY(myODBC->ODBC_Column_Info(Col_Types,Col_Attr_Names,Col_Lengths,Col_Max),NVOID);

	for (aa = 0; aa < Col_Max;aa++) {
		if (Col_Types[aa] == SQL_C_CHAR) {
			ostringstream os;
			os << Col_Lengths[aa] << ends;
			DTE_Types[aa] = "string" + os.str();
		} 
		if (Col_Types[aa] == SQL_C_DOUBLE) {
			DTE_Types[aa] = "double" ;
		}
		if (Col_Types[aa] == SQL_C_SLONG) {
			DTE_Types[aa] = "int" ;
		}
		if (Col_Types[aa] == SQL_C_TIMESTAMP) {
			DTE_Types[aa] = "date" ;
		}
	}
	for (aa = 0; aa < Col_Max; aa++) {
		if (DTE_Types[aa] != typeIDs[aa]) {
			cerr << aa << " " << DTE_Types[aa]  << " " << typeIDs[aa] << endl ;
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

	TRY(aa = myODBC->ODBC_Get_Rec(Col_Types,Col_Lengths,typePtrs,Col_Max),NULL);
	if ( aa > 0) {
		return next; 
	} else {
		return NULL;
	}
}
