#include "myopt.h"
#include "TypeCheck.h"
#include "catalog.h"
#include "Interface.h"

void TypeCheck::insert(BaseSelection* element){
#ifdef DEBUG
	cerr << "inserting: ";
	element->display(cerr);
	cerr << endl;
#endif
	symtab[element->toString()] = element;
}

void TypeCheck::setupSelList(vector<BaseSelection*>& list){
	map<string, BaseSelection*, StringLess>::iterator it;
	for(it = symtab.begin(); it != symtab.end(); ++it){
		list.push_back((*it).second);
	}
}

BaseSelection* TypeCheck::resolve(BaseSelection* curr){
	string selstr = curr->toString();
	BaseSelection* present = symtab[selstr];
	if(present){
		curr->destroy();
		delete curr;
		return present;
	}
	else{
		vector<BaseSelection*> children = curr->getChildren();	
		resolve(children);
		curr->setChildren(children);
		TRY(curr->typeCheck(), NULL);
		symtab[selstr] = curr;
		return curr;
	}
}

void TypeCheck::resolve(vector<BaseSelection*>& list){
	vector<BaseSelection*>::iterator it;
	for(it = list.begin(); it != list.end(); ++it){
		TRY(*it = resolve(*it), );
	}
}

void TypeCheck::initialize(List<TableAlias*>* tableList){
     Interface* interf;
	for(tableList->rewind(); !tableList->atEnd(); tableList->step()){
		TableAlias* current = tableList->get();
		TableName* tableName = current->getTable();
		TableName* tableNameCopy = new TableName(*tableName);
		TRY(interf = ROOT_CATALOG.createInterface(tableNameCopy), );
		TRY(const ISchema* schema = interf->getISchema(tableNameCopy), );
		int numFlds = schema->getNumFlds();
		const string* attrs = schema->getAttributeNames();
		const TypeID* types = schema->getTypeIDs();
		for(int i = 0; i < numFlds; i++){
			string* aliasCopy = new string(*current->getAlias());
			string* attCpy = new string(attrs[i]);
			insert(new PrimeSelection(aliasCopy, attCpy, types[i]));
		}
		delete interf;
	}
}

TypeCheck::~TypeCheck(){
/*
	map<string, BaseSelection*, StringLess>::iterator it;
	for(it = symtab.begin(); it != symtab.end(); it++){
		delete (*it).second;
	}
*/
}
