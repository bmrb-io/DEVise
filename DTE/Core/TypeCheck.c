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
		TRY(resolve(children), NULL);
		curr->setChildren(children);
		TRY(curr->typeCheck(), NULL);
		symtab[selstr] = curr;
		return curr;
	}
}

void TypeCheck::resolve(vector<BaseSelection*>& list){
	vector<BaseSelection*>::iterator it;
	for(it = list.begin(); it != list.end(); ++it){
		TRY(*it = resolve(*it), NVOID);
	}
}

void TypeCheck::initialize(const vector<TableAlias*>& tableList){
	vector<TableAlias*>::const_iterator it;
	for(it = tableList.begin(); it != tableList.end(); ++it){
		TableAlias* current = *it;
		ISchema schema;
		TRY(schema = current->getISchema(), NVOID);

		// get site descriptor and insert it into SDTable

//		TRY(const SiteDesc* sd = interf->getSiteDesc(), NVOID);

		int numFlds = schema.getNumFlds();
		const string* attrs = schema.getAttributeNames();
		const TypeIDList& types = schema.getTypeIDs();
		for(int i = 0; i < numFlds; i++){
			string* aliasCopy = new string(*current->getAlias());
			string* attCpy = new string(attrs[i]);
			PrimeSelection* ps;
			int avgSize = 0;
			ps = new PrimeSelection(
				aliasCopy, attCpy, types[i], avgSize, current);
			ps->setTableMap(tableList);
			insert(ps);
		}
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
