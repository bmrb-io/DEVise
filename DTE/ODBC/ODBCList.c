#include "ODBCList.h"
#include <SQL.H>
#include <sqlext.h>
#include "ODBCInfo.h"


void ODBCError(SQLRETURN err_Stat,string err_msg) {
	if ((err_Stat != SQL_SUCCESS) && (err_Stat != SQL_SUCCESS_WITH_INFO)) {
		THROW(new Exception(err_msg),NVOID);
	}
}

DSNListIterator::DSNListIterator() {
	numFlds = 0;
	initialize();
}

void DSNListIterator::initialize() {
	if (numFlds != 0) {  // already initialized
		return;
	}

	SQLRETURN retcode;
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	TRY(ODBCError(retcode,"Cannot allocate HENV in ODBC"),NVOID);

	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER);
	TRY(ODBCError(retcode,"Cannot Set Version of ODBC"),NVOID);

	numFlds = 3;
	int i = 0;
	typeIDs = new TypeID[numFlds];
	attrNames = new string[3];
	
	typeIDs[0] = "string40";
	typeIDs[1] = "string512";
	typeIDs[2] = "string512";

	attrNames[0] = "dsn";
	attrNames[1] = "description";
	attrNames[2] = "need";

   	nextRecord = new Type*[numFlds];
	destroyPtrs = new DestroyPtr[numFlds];
	for (i = 0; i < numFlds ; i++) {
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		nextRecord[i] = allocateSpace(typeIDs[i]);
	}

}

void DSNListIterator::getInfo(int& num, TypeID*& tPs, string*& aNs) {
	num = numFlds;
	tPs = typeIDs;
	aNs = attrNames;
}


const Tuple* DSNListIterator::getNext() {
	SQLRETURN retcode;
	short int sLength = 40; // Send this
	short int sourceLength; // Receive this
	short int dLength = 512; // Send this
	short int descLength;    // Receive this

	retcode = SQLDataSources(henv, SQL_FETCH_NEXT, (SQLCHAR*)(nextRecord[0]), sLength, &sourceLength, 
		(SQLCHAR*)(nextRecord[1]), dLength, &descLength);

	if (retcode == SQL_NO_DATA) {
		return NULL;
	} else {
		TRY(ODBCError(retcode, "Cannot retrieve DSN from ODBC"), NULL);
	}

	string tmpDrv = "DSN=" + string((char*)(nextRecord[0])) ;

	SQLCHAR* driver = (SQLCHAR*)(tmpDrv.c_str());

	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	TRY(ODBCError(retcode, "Cannot allocate HDBC in ODBC"), NULL);

	retcode = SQLBrowseConnect(hdbc, driver, SQL_NTS, (SQLCHAR*)(nextRecord[2]), dLength, &descLength);
	
	if (retcode == SQL_ERROR) {
		*(char*)(nextRecord[2]) = '\0';
	}

	SQLDisconnect(hdbc);

	return nextRecord;
}

DSNListIterator::~DSNListIterator() {
	SQLRETURN retcode;
	destroyTuple(nextRecord, numFlds, destroyPtrs);
	delete [] typeIDs;
	delete [] destroyPtrs;
	delete [] attrNames;

	retcode = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	TRY(ODBCError(retcode, "Cannot free HDBC in ODBC"), NVOID);

	retcode = SQLFreeHandle(SQL_HANDLE_ENV, henv);
	TRY(ODBCError(retcode, "Cannot free HENV in ODBC"), NVOID);
}

TableListIterator::TableListIterator(string DSN) {
	ODBCInfo myInfo;
	TRY(DSNInfo = myInfo.dsnFindInfo(DSN), NVOID);
	if (DSNInfo == "") {
		THROW(new Exception("Couldn't Find : " + DSN + " in .dsn table"),NVOID);
	}
	numFlds = 0;
	initialize();
}


void TableListIterator::initialize() {
	if (numFlds != 0) {  // already initialized
		return;
	}

	SQLRETURN retcode;
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
	TRY(ODBCError(retcode,"Cannot allocate HENV in ODBC"),NVOID);

	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC2, SQL_IS_INTEGER);
	TRY(ODBCError(retcode,"Cannot Set Version of ODBC"),NVOID);

	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	TRY(ODBCError(retcode,"Cannot allocate HDBC in ODBC"),NVOID);

	SQLCHAR* outBuffer;
	SQLCHAR* tableNames = (SQLCHAR*)("%"); // can  be a subset of tables
	SQLCHAR* tableTypes = (SQLCHAR*)("TABLE, VIEW");

	outBuffer = new SQLCHAR[1024];
	short int bLen = 1024; // send this
	short int bufLen;      // receive this

	retcode = SQLDriverConnect(hdbc, NULL, (UCHAR*)(DSNInfo.c_str()), (short int)DSNInfo.length(), 
		outBuffer, bLen, &bufLen, SQL_DRIVER_NOPROMPT);

	TRY(ODBCError(retcode,"Cannot connect to :" + DSNInfo + " in ODBC"),NVOID);

	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	TRY(ODBCError(retcode,"Cannot allocate HSTMT in ODBC"),NVOID);

	retcode = SQLTables(hstmt, NULL, 0, NULL, 0, tableNames, SQL_NTS, tableTypes, SQL_NTS);
	TRY(ODBCError(retcode,"Cannot get table names in ODBC"),NVOID);

	delete [] outBuffer;

	numFlds = 3;
	int i = 0;
	typeIDs = new TypeID[numFlds];
	attrNames = new string[3];

	typeIDs[0] = "string128";
	typeIDs[1] = "string128";
	typeIDs[2] = "string128";
	attrNames[0] = "owner";
	attrNames[1] = "name";
	attrNames[2] = "type";

	nextRecord = new Type*[numFlds];
	destroyPtrs = new DestroyPtr[numFlds];
	for (i = 0; i < numFlds ; i++) {
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		nextRecord[i] = allocateSpace(typeIDs[i]);
	}
	

}

void TableListIterator::getInfo(int& num, TypeID*& tPs, string*& aNs) {
	num = numFlds;
	tPs = typeIDs;
	aNs = attrNames;
}

const Tuple*  TableListIterator::getNext() {
	SQLRETURN retcode;
	SQLINTEGER dataStatus;
	retcode = SQLFetch(hstmt);

	if (retcode == SQL_NO_DATA) {
		return NULL;
	} else {
		TRY(ODBCError(retcode, "Cannot retrieve table names from ODBC"), NULL);
	}

	retcode = SQLGetData(hstmt, 2, SQL_C_CHAR, (SQLCHAR*)(nextRecord[0]), 128, &dataStatus);
	if (dataStatus == SQL_NULL_DATA) {
		TRY(ODBCError(retcode, "Cannot retrieve table info from ODBC"), NULL);
	}

	retcode = SQLGetData(hstmt, 3, SQL_C_CHAR, (SQLCHAR*)(nextRecord[1]), 128, &dataStatus);
	if (dataStatus == SQL_NULL_DATA) {
		TRY(ODBCError(retcode, "Cannot retrieve table info from ODBC"), NULL);
	}

	retcode = SQLGetData(hstmt, 4, SQL_C_CHAR, (SQLCHAR*)(nextRecord[2]), 128, &dataStatus);
	if (dataStatus == SQL_NULL_DATA) {
		TRY(ODBCError(retcode, "Cannot retrieve table info from ODBC"), NULL);
	}
	return nextRecord;
}


TableListIterator::~TableListIterator() {
	SQLRETURN retcode;
	destroyTuple(nextRecord, numFlds, destroyPtrs);
	delete [] attrNames;
	delete [] typeIDs;
	delete [] destroyPtrs;
	
	retcode = SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
	TRY(ODBCError(retcode, "Cannot free HSTMT in ODBC"), NVOID);

	retcode = SQLDisconnect(hdbc);
	TRY(ODBCError(retcode, "Cannot Disconnect from ODBC"), NVOID);

	retcode = SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	TRY(ODBCError(retcode, "Cannot free HDBC in ODBC"), NVOID);

	retcode = SQLFreeHandle(SQL_HANDLE_ENV, henv);
	TRY(ODBCError(retcode, "Cannot free HENV in ODBC"), NVOID);

}
