#ifndef TYPECHECK_H
#define TYPECHECK_H

#include "queue.h"

// This class is the owner of all BaseSelection objects

class SymbolTable;
class BaseSelection;
class TableAlias;

class TypeCheck {
	SymbolTable* symtab;
public:
     void load(List<TableAlias*>* tableList);
     List<BaseSelection*>* createSelList();
     void load(List<BaseSelection*>* list);
};

#endif
