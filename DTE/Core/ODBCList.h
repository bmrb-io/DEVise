#ifndef ODBCLIST_H
#define ODBCLIST_H

#include "exception.h"
#include "sysdep.h"
#include "types.h"
#include "Iterator.h"

#if defined(_WINDOWS) || defined(_CONSOLE)
	#include <windows.h>
	#include "SQL.H"
	#include <sqlext.h>
#endif

void ODBCError(SQLRETURN err_Stat,string err_msg);

class DSNListIterator : public Iterator {
	int numFlds;
	string* attrNames;
	TypeIDList typeIDs;
	Type** nextRecord;
	DestroyPtr* destroyPtrs;
	HENV henv;
	HDBC hdbc;

public:
	DSNListIterator();
	virtual void initialize();
	void getInfo(int& num, TypeID*& tPs, string*& aNs);
	virtual const Tuple* getNext();
	virtual const TypeIDList& getTypes();
	virtual ~DSNListIterator();
};

class TableListIterator : public Iterator {
	int numFlds;
	string DSNInfo;
	string* attrNames;
	TypeIDList typeIDs;
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
	virtual const TypeIDList& getTypes();
	virtual ~TableListIterator();
};

#endif
