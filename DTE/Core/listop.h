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
  Revision 1.3  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.2  1996/12/05 16:06:02  wenger
  Added standard Devise file headers.

 */

#ifndef LISTOP_H
#define LISTOP_H

#include <assert.h>
#include <String.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include "queue.h"
#include "types.h"
#include "exception.h"

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

bool exclusiveF(List<BaseSelection*>* list, Site* site);
bool exclusiveList(List<BaseSelection*>* list, String* attNms, int n);
List<BaseSelection*>* duplicateF(List<BaseSelection*>* list);
void filterList(List<BaseSelection*>* list, Site* site);
void displayList(ostream& out, List<TableAlias*>* list, String sep = ", ");
void displayList(ostream& out, List<BaseSelection*>* list, 
	String sep = ", ", int detail = 0); 
void displayList(ostream& out, List<Site*>* list, String sep);
void collectFrom(
	List<BaseSelection*>* from, Site* site, List<BaseSelection*>* to);
void enumerateList(List<BaseSelection*>* list,
     String site1, List<BaseSelection*>* list1,
     String site2 = "", List<BaseSelection*>* list2 = NULL);
TypeID* typifyList(List<BaseSelection*>* list, List<Site*>* sites);
bool boolCheckList(List<BaseSelection*>* list);
bool evaluateList(
	List<BaseSelection*>* list, Tuple* left, Tuple* right = NULL);
Tuple* tupleFromList(
	List<BaseSelection*>* list, Tuple* left, Tuple* right = NULL);
void typifyList(List<Site*>* sites, String option);
TypeID* getTypesFromList(List<BaseSelection*>* list);
double listSelectivity(List<BaseSelection*>* list);
int* sizesFromList(List<BaseSelection*>* list);
void checkOrphanInList(List<BaseSelection*>* list);
String* getStringsFrom(List<BaseSelection*>* list);
List<BaseSelection*>* createGlobalSelectList(List<Site*>* sites);
String* getAttStringsOnly(List<BaseSelection*>* list);

#endif
