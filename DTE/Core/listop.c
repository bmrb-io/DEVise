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
  Revision 1.20  1997/11/24 23:13:16  weaver
  Changes for the new ColorManager.

  Revision 1.19  1997/11/13 22:19:25  okan
  Changes about compilation on NT

  Revision 1.18  1997/11/05 00:19:45  donjerko
  Separated typechecking from optimization.

  Revision 1.17  1997/09/29 02:51:56  donjerko
  Eliminated class GlobalSelect.

  Revision 1.16  1997/09/17 02:35:47  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.15  1997/09/05 22:20:17  donjerko
  Made changes for port to NT.

  Revision 1.14  1997/08/22 23:13:05  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.13  1997/08/21 21:04:31  donjerko
  Implemented view materialization

  Revision 1.12  1997/07/30 21:39:24  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.11  1997/07/22 15:00:55  donjerko
  *** empty log message ***

  Revision 1.10  1997/06/16 16:04:50  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.9  1997/04/14 20:44:12  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.8  1997/04/08 01:47:34  donjerko
  Set up the basis for ORDER BY clause implementation.

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

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include <string>
#include<assert.h>
#include<math.h>
#include "queue.h"
#include "myopt.h"
#include "types.h"
#include "site.h"
#include "exception.h"
#include "catalog.h"
#include "ExecExpr.h"
#include "sysdep.h"

int* findPositions(List<BaseSelection*>* list, 
			List<BaseSelection*>* elements){ 		// throws
	int* retVal = new int[elements->cardinality()];
	int i, j;
	for(i = 0, elements->rewind(); !elements->atEnd(); elements->step(), i++){
		for(j = 0, list->rewind(); true; list->step(), j++){
			if(list->atEnd()){
				string msg = string("Cannot match a selection from") +
					" ORDER BY to any of the SELECT clause";
				THROW(new Exception(msg), NULL);
			}
			if(list->get()->match(elements->get())){
				retVal[i] = j;
				break;
			}
		}
	}
	return retVal;
}

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

bool exclusiveList(List<BaseSelection*>* list, string* attNms, int n){
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
	ostream& out, List<BaseSelection*>* list, string sep, int detail){
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

void displayList(ostream& out, List<TableAlias*>* list, string sep){
     list->rewind();
     while(!list->atEnd()){
		list->get()->display(out);
          list->step();
		if(!list->atEnd()){
			out << sep;
		}
	}
}

void displayList(ostream& out, List<Site*>* list, string sep){
     list->rewind();
     while(!list->atEnd()){
		list->get()->display(out);
          list->step();
		if(!list->atEnd()){
			out << sep;
		}
	}
}

void displayList(ostream& out, List<string*>* list, string sep){
     list->rewind();
     while(!list->atEnd()){
		out << *list->get();
          list->step();
		if(!list->atEnd()){
			out << sep;
		}
	}
}

ExprList* enumerateList(List<BaseSelection*>* list,
                        Site* site1, Site* site2)
{
  ExprList* retVal = new ExprList;
  list->rewind();
  for(int i = 0; !list->atEnd(); i++){
    BaseSelection* bs = list->get();
    assert(bs);
    ExecExpr* tmp;
    TRY(tmp = bs->createExec(site1, site2), NULL);
    assert(tmp);
    retVal->push_back(tmp);
    list->step();
  }
  return retVal;
}

/*
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
*/

void typifyList(List<Site*>* list, string option){
	assert(list);
	list->rewind();
	while(!list->atEnd()){
		Site* tmp = list->get();
		TRY(tmp->typify(option), NVOID );
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
		if(!(current->getTypeID() == "bool")){
			stringstream msg;
			current->display(msg);
			msg << ends;
			string msgc = msg.str();
			string msgs = "Predicate " + msgc + " is of type " +
				current->getTypeID() + " (boolean expected)";
			THROW(new Exception(msgs), false);
		}
		list->step();
	}
	return true;
}

bool evaluateList(
	Array<ExecExpr*>* list, const Tuple* left, const Tuple* right){

	for(int i = 0; i < list->length; i++){
		bool value = ((*list)[i]->evaluate(left, right) ? true : false);
		if(value == false){
			return false;
		}
	}
	return true;
}

void tupleFromList(Tuple* retVal,
		Array<ExecExpr*>* list, const Tuple* left, const Tuple* right){
	for(int i = 0; i < list->length; i++){
		retVal[i] = (Type*) (*list)[i]->evaluate(left, right);
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
		TRY(list->get()->checkOrphan(), NVOID );
		list->step();
	}
}

string* getStringsFrom(List<BaseSelection*>* list){
	string* retVal = new string[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		ostringstream tmp;
		list->get()->display(tmp);
		tmp << ends;
		retVal[i] = tmp.str();
		i++;
		list->step();
	}
	return retVal;
}

string* getAttStringsOnly(List<BaseSelection*>* list){
	string* retVal = new string[list->cardinality()];
	list->rewind();
	int i = 0;
	while(!list->atEnd()){
		ostringstream tmp;
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
			retVal->append(currBase);
			currSelect->step();
		}
		sites->step();
	}
	return retVal;
}
