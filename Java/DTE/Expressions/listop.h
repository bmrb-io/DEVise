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
  Revision 1.14  1997/12/04 04:05:18  donjerko
  *** empty log message ***

  Revision 1.13  1997/11/05 00:19:45  donjerko
  Separated typechecking from optimization.

  Revision 1.12  1997/09/29 02:51:57  donjerko
  Eliminated class GlobalSelect.

  Revision 1.11  1997/09/05 22:20:17  donjerko
  Made changes for port to NT.

  Revision 1.10  1997/08/21 21:04:31  donjerko
  Implemented view materialization

  Revision 1.9  1997/07/30 21:39:24  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.8  1997/06/16 16:04:50  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.7  1997/04/08 01:47:35  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.5  1996/12/21 22:21:48  donjerko
  Added hierarchical namespace.

  Revision 1.4  1996/12/15 06:41:08  donjerko
  Added support for RTree indexes

  Revision 1.3  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.2  1996/12/05 16:06:02  wenger
  Added standard Devise file headers.

 */

#ifndef LISTOP_H
#define LISTOP_H

#include <assert.h>
#include <string>
#include <vector>
//#include <iostream.h>   erased for sysdep.h
//#include <fstream.h>   erased for sysdep.h
//#include <strstream.h>   erased for sysdep.h
#include "queue.h"
#include "Array.h"
#include "types.h"
#include "exception.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

#define APPLY(FUNC, LIST){\
     LIST->rewind();\
     while(!LIST->atEnd()){\
          LIST->get()->FUNC;\
          LIST->step();\
     }\
}

#define DESTROY_LIST(LIST){\
     LIST->rewind();\
     while(!LIST->atEnd()){\
          LIST->get()->destroy();\
          delete LIST->get();\
          LIST->step();\
     }\
}

class BaseSelection;
class Site;
class TableAlias;
class ExecExpr;

template<class T>
void translate(const vector<T>& vec, List<T>*& list){
	list = new List<T>;

	vector<T>::const_iterator it;
	for(it = vec.begin(); it != vec.end(); ++it){
		list->append(*it);
	}
}

template<class T>
void translate(List<T>* list,  vector<T>& vec){
	assert(vec.empty()); 
	if(list){
		for(list->rewind(); !list->atEnd(); list->step()){
			vec.push_back(list->get());
		}
	}
}

int* findPositions(List<BaseSelection*>* list, 
	List<BaseSelection*>* elements);	// throws
bool exclusiveF(List<BaseSelection*>* list, Site* site);
bool exclusiveList(List<BaseSelection*>* list, string* attNms, int n);
List<BaseSelection*>* duplicateF(List<BaseSelection*>* list);
void displayList(ostream& out, List<TableAlias*>* list, string sep = ", ");
void displayList(ostream& out, List<BaseSelection*>* list, 
	string sep = ", ", int detail = 0); 
void displayList(ostream& out, List<Site*>* list, string sep);
void displayList(ostream& out, List<string*>* list, string sep);

template <class T>
void displayVec(ostream& out, const vector<T>& vec, string sep = ", "){
	vector<T>::const_iterator it;
	for(it = vec.begin(); it != vec.end(); ){
		(*it)->display(out);
		++it;
		if(it == vec.end()){
			break;
		}
		out << sep;
	}
}

void collectFrom(
	List<BaseSelection*>* from, Site* site, List<BaseSelection*>* to);
Array<ExecExpr*>* enumerateList(List<BaseSelection*>* list, 
	Site* site1, Site* site2 = NULL);
bool boolCheckList(List<BaseSelection*>* list);
bool evaluateList(
	Array<ExecExpr*>* list, const Tuple* left, const Tuple* right = NULL);
void tupleFromList(Tuple* next,
	Array<ExecExpr*>* list, const Tuple* left, const Tuple* right = NULL);
void typifyList(List<Site*>* sites, string option);
TypeID* getTypesFromList(List<BaseSelection*>* list);
double listSelectivity(List<BaseSelection*>* list);
int* sizesFromList(List<BaseSelection*>* list);
void checkOrphanInList(List<BaseSelection*>* list);
string* getStringsFrom(List<BaseSelection*>* list);
List<BaseSelection*>* createGlobalSelectList(List<Site*>* sites);
string* getAttStringsOnly(List<BaseSelection*>* list);

#endif
