#ifndef TYPECHECK_H
#define TYPECHECK_H

#if 0
//kb: remove this file

#include <vector>
#include <string>
#include <map>
#include "StringLess.h"

#ifndef __GNUG__
using namespace std;
#endif

// This class is the owner of all OptExpr objects

class OptExpr;
class TableAlias;

class TypeCheck {
	map<string, OptExpr*, StringLess> symtab;
public:
	void initialize(const vector<TableAlias*>& tableList);
	void insert(OptExpr* element);
     void setupSelList(vector<OptExpr*>& list);
	OptExpr* TypeCheck::resolve(OptExpr* curr);
     void resolve(vector<OptExpr*>& list);	// throws
	~TypeCheck();
};
#endif

#endif
