#if 0
//kb: delete me
#include "myopt.h"
#include "TypeCheck.h"
#include "catalog.h"
#include "Interface.h"

void TypeCheck::insert(OptExpr* element){
#ifdef DEBUG
	cerr << "inserting: ";
	element->display(cerr);
	cerr << endl;
#endif
	symtab[element->toString()] = element;
}

void TypeCheck::setupSelList(vector<OptExpr*>& list){
	map<string, OptExpr*, StringLess>::iterator it;
	for(it = symtab.begin(); it != symtab.end(); ++it){
		list.push_back((*it).second);
	}
}

OptExpr* TypeCheck::resolve(OptExpr* curr){
	string selstr = curr->toString();
cerr << "looking up OptExpr symbol [" << selstr << "]... ";
	OptExpr* present = symtab[selstr];
	if(present){
cerr << "found\n";
//kb: this file is deleting optexprs
		delete curr;
		return present;
	}
	else{
cerr << "not found\n";
		vector<OptExpr*> children = curr->getChildren();	
		TRY(resolve(children), NULL);
		curr->setChildren(children);
		TRY(curr->typeCheck(), NULL);
		//kb: symtab[selstr] = curr;
		return curr;
	}
}

void TypeCheck::resolve(vector<OptExpr*>& list){
	vector<OptExpr*>::iterator it;
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
		const vector<string>& attrs = schema.getAttributeNames();
		const DteTupleAdt& adt = schema.getAdt();
		for(int i = 0; i < numFlds; i++) {
                  int avgSize = 0;
                  OptField* f = new OptField(current->getAlias(), attrs[i],
                                             adt.getAdt(i).clone(),
                                             avgSize, current);
                  f->setTableMap(tableList);
                  insert(f);
		}
	}
}

TypeCheck::~TypeCheck(){
/*
	map<string, OptExpr*, StringLess>::iterator it;
	for(it = symtab.begin(); it != symtab.end(); it++){
		delete (*it).second;
	}
*/
}
#endif
