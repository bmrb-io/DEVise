/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.6  1997/03/23 23:45:23  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.5  1996/12/21 22:21:48  donjerko
  Added hierarchical namespace.

  Revision 1.4  1996/12/15 06:41:08  donjerko
  Added support for RTree indexes

  Revision 1.3  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.2  1996/12/05 16:06:02  wenger
  Added standard Devise file headers.

 */

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

bool exclusiveList(List<BaseSelection*>* list, String* attNms, int n){
	list->rewind();
	while(!list->atEnd()){
		if(!list->get()->exclusive(attNms, n)){
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

void displayList(ostream& out, List<Site*>* list, String sep){
     list->rewind();
     while(!list->atEnd()){
		list->get()->display(out);
          list->step();
		if(!list->atEnd()){
			out << sep;
		}
	}
}

void displayList(ostream& out, List<String*>* list, String sep){
     list->rewind();
     while(!list->atEnd()){
		out << *list->get();
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
		bool value = list->get()->evaluate(left, right);
		if(value == false){
			return false;
		}
		list->step();
	}
	return true;
}

void tupleFromList(Tuple* retVal, 
		List<BaseSelection*>* list, Tuple* left, Tuple* right){
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		retVal[i] = list->get()->evaluate(left, right);
		list->step();
		i++;
	}
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
		retVal[i] = String(tmp.str());
		i++;
		list->step();
	}
	return retVal;
}

String* getAttStringsOnly(List<BaseSelection*>* list){
	String* retVal = new String[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		ostrstream tmp;
		retVal[i] = list->get()->toStringAttOnly();
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
