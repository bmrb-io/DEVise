#include "ODBCInfo.h"
#include "sysdep.h"
#include "Engine.h"
#include "types.h"

void ODBCInfo::dsnInsert(string DSN_Name, string others) {
	string query = "insert into .dsn values (\'\"" + DSN_Name + "\"\t\"" + others + "\"\')" ;
	Engine engine(query);
	TRY(engine.optimize(),NVOID);
}

void ODBCInfo::dsnDelete(string DSN_Name) {
	string query = "delete .dsn as t where t.DSN = \'" + DSN_Name + "\'" ;
	Engine engine(query);
	TRY(engine.optimize(),NVOID);
}

string ODBCInfo::dsnFindInfo(string DSN_Name) {

	string query = "select t.DSNInfo from .dsn as t where t.DSN = \'" + DSN_Name + "\'" ;

	Engine engine(query);

	TRY(engine.optimize(),"");

	int numFlds = engine.getNumFields();

	if (numFlds != 1) {
		string err = "dsn table is not defined properly";
		THROW(new Exception(err), "");
	}

	const Tuple* tup;
	int count = 0;
	string tmpStr; 

	EXIT(tup = engine.getFirst());

	while (tup) {
		tmpStr= "DSN=" + DSN_Name + ";" + string((char*)tup[0]);
		count++;
		tup = engine.getNext();
	}

	if (count == 0) {
		string err = DSN_Name + " is not found in dsn table";
		THROW(new Exception(err), "");
	} else if (count > 1) {
		string err = "There are more than one " + DSN_Name + " entries in dsn table";
		THROW(new Exception(err), "");
	}
	return tmpStr;
}
