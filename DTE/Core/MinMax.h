#ifndef MINMAX_H
#define MINMAX_H

#include "myopt.h"
#include "StandardRead.h"

#ifndef __GNUG__
using namespace std;
#endif

class MinMax {
public:
	static bool isApplicable(List<BaseSelection*>* selectList);
	static TableAlias* createReplacement(TableAlias* table);
	static void replace(const string& entry, const Interface* interf);
	static string getPathName(const string& tableName);
};

#endif
