#include "ODBCList.h"
#include "ODBCInfo.h"
#include "sysdep.h"


void ODBCError(SQLRETURN err_Stat,string err_msg) {
	return;
}

DSNListIterator::DSNListIterator() {
	numFlds = 0;
	attrNames = NULL;
   	nextRecord = NULL;
	destroyPtrs = NULL;
}

void DSNListIterator::initialize() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}

void DSNListIterator::getInfo(int& num, TypeID*& tPs, string*& aNs) {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}


const Tuple* DSNListIterator::getNext() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
	return NULL;
}

const TypeIDList& DSNListIterator::getTypes() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
        return typeIDs;
}

DSNListIterator::~DSNListIterator() {
}

TableListIterator::TableListIterator(string DSN) {
	numFlds = 0;
	attrNames = NULL;
	nextRecord = NULL;
	destroyPtrs = NULL;
}


void TableListIterator::initialize() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}

void TableListIterator::getInfo(int& num, TypeID*& tPs, string*& aNs) {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
}

const Tuple*  TableListIterator::getNext() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
	return NULL;
}

const TypeIDList& TableListIterator::getTypes() {
	cerr << "ODBC driver is not installed" << endl;
	exit(1);
        return typeIDs;
}


TableListIterator::~TableListIterator() {
}
