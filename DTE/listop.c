#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include "queue.h"
#include "myopt.h"
#include "types.h"
#include "site.h"
#include "exception.h"
#include "catalog.h"

bool exclusiveF(List<BaseSelection*>* list, Site* site){
	list->rewind();
	while(!list->atEnd()){
		if(!list->get()->exclusive(site)){
			return false;
		}
		list->step();
	}
	return true;
}

List<BaseSelection*>* duplicateF(List<BaseSelection*>* list){
     List<BaseSelection*>* duplList = new List<BaseSelection*>;
	list->rewind();
	while(!list->atEnd()){
		duplList->append(list->get()->duplicate());
		list->step();
	}
	return duplList;
}

void displayList(
	ostream& out, List<BaseSelection*>* list, String sep, int detail){
	if(list == NULL){
		out << '*';
		return;
	}
     list->rewind();
     while(!list->atEnd()){
		list->get()->display(out, detail);
          list->step();
		if(!list->atEnd()){
			out << sep;
		}
	}
}

void displayList(ostream& out, List<TableAlias*>* list, String sep){
     list->rewind();
     while(!list->atEnd()){
		list->get()->display(out);
          list->step();
		if(!list->atEnd()){
			out << sep;
		}
	}
}

void filterList(List<BaseSelection*>* list, Site* site){
	if(!list){
		return;
	}
	list->rewind();
	while(!list->atEnd()){
		BaseSelection* current = list->get();
		BaseSelection* global = current->filter(site);
		if(global){
			list->replace(global);     // nothing is discarded
		}
		list->step();
	}
}

void enumerateList(List<BaseSelection*>* list,
     String site1, List<BaseSelection*>* list1,
	String site2, List<BaseSelection*>* list2){

	list->rewind();
	while(!list->atEnd()){
		BaseSelection* tmp;
		TRY(tmp = list->get()->enumerate(site1, list1, site2, list2), );
		assert(tmp);
		list->replace(tmp);
		list->step();
	}
}

TypeID* typifyList(List<BaseSelection*>* list, List<Site*>* sites){
	assert(list);
	list->rewind();
	TypeID* retVal = new TypeID[list->cardinality()];
	int i = 0;
	while(!list->atEnd()){
		TRY(retVal[i] = list->get()->typify(sites), NULL);
		list->step();
		i++;
	}
	return retVal;
}

void typifyList(List<Site*>* list, String option){
	assert(list);
	list->rewind();
	while(!list->atEnd()){
		Site* tmp = list->get();
		TRY(tmp->typify(option), );
		list->step();
	}
}

TypeID* getTypesFromList(List<BaseSelection*>* list){
	TypeID* retVal = new TypeID[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		retVal[i] = list->get()->getTypeID();
		list->step();
		i++;
	}
	return retVal;
}

bool boolCheckList(List<BaseSelection*>* list){
	list->rewind();
	while(!list->atEnd()){
		BaseSelection* current = list->get();
		if(current->getTypeID() != "bool"){
			strstream msg;
			current->display(msg);
			msg << ends;
			char* msgc = msg.str();
			String msgs = "Predicate " + String(msgc) + " is not boolean";
			delete msgc;
			THROW(new Exception(msgs), false);
		}
		list->step();
	}
	return true;
}

bool evaluateList(List<BaseSelection*>* list, Tuple* left, Tuple* right){
	list->rewind();
	while(!list->atEnd()){
		bool value = ((IBool*)list->get()->evaluate(left, right))->getValue();
		if(value == false){
			return false;
		}
		list->step();
	}
	return true;
}

Tuple* tupleFromList(List<BaseSelection*>* list, Tuple* left, Tuple* right){
     Tuple* retVal = new Tuple[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		retVal[i] = list->get()->evaluate(left, right);
		list->step();
		i++;
	}
	return retVal;
}

void collectFrom(List<BaseSelection*>* from, 
			  Site* site, List<BaseSelection*>* to){
	assert(from);
	from->rewind();
	while(!from->atEnd()){
		from->get()->collect(site, to);
		from->step();
	}
}

double listSelectivity(List<BaseSelection*>* list){
	list->rewind();
	double retVal = 1;
	while(!list->atEnd()){
		retVal *= list->get()->getSelectivity();
		list->step();
	}
	return retVal;
}

int* sizesFromList(List<BaseSelection*>* list){
	int* retVal = new int[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		retVal[i] = list->get()->getSize();
		list->step();
		i++;
	}
	return retVal;
}

void checkOrphanInList(List<BaseSelection*>* list){
	if(list == NULL){
		return;
	}
	list->rewind();
	while(!list->atEnd()){
		TRY(list->get()->checkOrphan(), );
		list->step();
	}
}

String* getStringsFrom(List<BaseSelection*>* list){
	String* retVal = new String[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		ostrstream tmp;
		list->get()->displayFlat(tmp);
		tmp << ends;
		retVal[i] = tmp.str();
		i++;
		list->step();
	}
	return retVal;
}

List<BaseSelection*>* createGlobalSelectList(List<Site*>* sites){
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	sites->rewind();
	while(!sites->atEnd()){
		Site* current = sites->get();
		List<BaseSelection*>* currSelect = current->getSelectList();
		assert(currSelect);
		currSelect->rewind();
		while(!currSelect->atEnd()){
			BaseSelection* currBase = currSelect->get();
			retVal->append(new GlobalSelect(current, currBase, NULL));
			currSelect->step();
		}
		sites->step();
	}
	return retVal;
}
