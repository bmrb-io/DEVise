#ifndef TYPECHECK_H
#define TYPECHECK_H

#include <vector>
#include "SymbolTable.h"

#ifndef __GNUG__
using namespace std;
#endif

// This class is the owner of all BaseSelection objects

class SymbolTable;
class BaseSelection;
class TableAlias;

class TypeCheck {
	SymbolTable symtab;
public:
     void load(const vector<TableAlias*>& tableList);
     void setupSelList(vector<BaseSelection*>& list);
     void load(const vector<BaseSelection*>& list);
};

#endif
