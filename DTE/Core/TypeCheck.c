#include "TypeCheck.h"
#include "queue.h"
#include "myopt.h"

void TypeCheck::load(List<TableAlias*>* tableList){
#ifdef DEBUG
	cerr << "loading tables: ";
	displayList(cerr, tableList);
	cerr << endl;
#endif
}

List<BaseSelection*>* TypeCheck::createSelList(){
#ifdef DEBUG
	cerr << "Creating select list\n";
#endif
	return NULL;
}

void TypeCheck::load(List<BaseSelection*>* list){
#ifdef DEBUG
	cerr << "Type checking list: ";
	displayList(cerr, list);
	cerr << endl;
#endif
}
