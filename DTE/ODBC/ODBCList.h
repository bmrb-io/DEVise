#ifndef ODBCLIST_H
#define ODBCLIST_H

#include "exception.h"
#include "sysdep.h"
#include "types.h"
#include "Iterator.h"
#include <windows.h>
#include "SQL.H"
#include <sqlext.h>

void ODBCError(SQLRETURN err_Stat,string err_msg);

class DSNListIterator : public Iterator {
	int numFlds;
	string* attrNames;
	TypeID* typeIDs;
	Type** nextRecord;
	DestroyPtr* destroyPtrs;
	HENV henv;
	HDBC hdbc;

public:
	DSNListIterator();
	virtual void initialize();
	void getInfo(int& num, TypeID*& tPs, string*& aNs);
	virtual const Tuple* getNext();
	virtual ~DSNListIterator();
};

class TableListIterator : public Iterator {
	int numFlds;
	string DSNInfo;
	string* attrNames;
	TypeID* typeIDs;
	Type** nextRecord;
	DestroyPtr* destroyPtrs;
	HENV henv;
	HDBC hdbc;
	HSTMT hstmt;
public:
	TableListIterator(string DSN);
	virtual void initialize();
	void getInfo(int& num, TypeID*& tPs, string*& aNs);
	virtual const Tuple*  getNext();
	virtual ~TableListIterator();
};

#endif
