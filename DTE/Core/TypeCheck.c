#include "myopt.h"
#include "TypeCheck.h"
#include "SymbolTable.h"

void TypeCheck::load(const vector<TableAlias*>& tableList){
}

void TypeCheck::setupSelList(vector<BaseSelection*>& list){
#ifdef DEBUG
	cerr << "Creating select list\n";
#endif
}

void TypeCheck::load(const vector<BaseSelection*>& list){
#ifdef DEBUG
	cerr << "Type checking list: ";
	displayList(cerr, list);
	cerr << endl;
#endif
}
