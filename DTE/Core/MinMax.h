#ifndef MINMAX_H
#define MINMAX_H

#include "myopt.h"
#include "StandardRead.h"

class MinMax {
	static StandardRead minMaxTable;
public:
	static bool isApplicable(List<BaseSelection*>* selectList);
	static TableAlias* createReplacement(TableAlias* table);
	static void insert(const String& tableName, const String& fileName);
};

#endif
