#ifndef ODBCInfo_H
#define ODBCInfo_H

#include "sysdep.h"
#include <string>

#ifndef __GNUG__
using namespace std;
#endif

class ODBCInfo {
public:
	void dsnInsert(string DSN_Name, string others);
	void dsnDelete(string DSN_Name);
	string dsnFindInfo(string DSN_name);
};

#endif
