/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.46  1997/11/24 23:13:15  weaver
  Changes for the new ColorManager.

  Revision 1.45  1997/11/23 21:23:30  donjerko
  Added ODBC stuff.

  Revision 1.44  1997/11/13 22:19:23  okan
  Changes about compilation on NT

  Revision 1.43  1997/11/12 23:17:30  donjerko
  Improved error checking.

  Revision 1.42  1997/11/08 21:02:27  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

  Revision 1.41  1997/11/05 00:19:40  donjerko
  Separated typechecking from optimization.

  Revision 1.40  1997/10/14 05:16:34  arvind
  Implemented a first version of moving aggregates (without group bys).

  Revision 1.39  1997/10/07 18:33:37  donjerko
  *** empty log message ***

  Revision 1.38  1997/09/29 02:51:56  donjerko
  Eliminated class GlobalSelect.

  Revision 1.37  1997/09/17 02:35:43  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.36  1997/09/05 22:20:08  donjerko
  Made changes for port to NT.

  Revision 1.35  1997/08/25 15:28:10  donjerko
  Added minmax table

  Revision 1.34  1997/08/22 23:13:03  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.33  1997/08/21 21:04:24  donjerko
  Implemented view materialization

  Revision 1.32  1997/08/15 00:17:33  donjerko
  Completed the Iterator destructor code.

  Revision 1.31  1997/08/14 02:08:52  donjerko
  Index catalog is now an independent file.

  Revision 1.30  1997/08/12 19:58:41  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.29  1997/08/11 17:10:16  donjerko
  Added special processing for min/max queries.

  Revision 1.28  1997/08/10 18:09:15  donjerko
  Fixed the group by clause

  Revision 1.27  1997/07/30 21:39:18  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.26  1997/07/22 15:00:52  donjerko
  *** empty log message ***

  Revision 1.25  1997/06/21 22:48:00  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.24.2.1  1997/05/21 20:34:24  weaver
  Changes needed by new ColorManager

  Revision 1.24  1997/05/07 18:55:49  donjerko
  Fixed the problem when constants are selected.

  Revision 1.23  1997/04/28 06:56:05  donjerko
  *** empty log message ***

  Revision 1.22  1997/04/18 20:46:17  donjerko
  Added function pointers to marshall types.

  Revision 1.21  1997/04/14 20:44:12  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.20  1997/04/08 01:47:33  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.19  1997/03/28 16:07:25  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

// #define DEBUG

// Changed to non-pragma templates method. CEW 5/12/97
//#ifdef __GNUG__
//#pragma implementation "queue.h"
//#endif

#define MAX_AGG 12
#include "queue.h"
#include "myopt.h"
#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "DevRead.h"
#include "listop.h"
#include "Aggregates.h"
#include "ParseTree.h"
#include "joins.h"
#include "Sort.h"
#include "TypeCheck.h"
#include "MinMax.h"
#include "Interface.h"

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include <string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
#include "sysdep.h"

class ExecExpr;

List<ConstantSelection*>* dummy;	// Just needed for pragma implementation
List<char*>* dummy2;	// Just needed for pragma implementation

extern List<JoinTable*>*joinList;

const int DETAIL = 1;
LOG(ofstream logFile("log_file.txt");)

void QueryTree::resolveNames(){	// throws exception
     if(tableList->cardinality() > 1){
          string msg = "cannot resolve attribute referencies";
		THROW(new Exception(msg), NVOID );
		// throw Exception(msg);
     }
     else{
          tableList->rewind();
          namesToResolve->rewind();
          string* replacement = tableList->get()->getAlias();
		assert(replacement);
          for(int i = 0; i < namesToResolve->cardinality(); i++){
               string* current = namesToResolve->get();
			cout << *current << " " << *replacement << endl;
               *current = *replacement;
               namesToResolve->step();
          }
     }
}

//****************************************
// Getting around the template problem for now...
//****************************************
//template<class T>
//void translate(const vector<T>& vec, List<T>*& list){
//	delete list;
//	list = new List<T>;

//	vector<T>::const_iterator it;
//	for(it = vec.begin(); it != vec.end(); ++it){
//		list->append(*it);
//	}
//}

//template<class T>
//void translate(List<T>* list,  vector<T>& vec){
//	assert(vec.empty()); 
//	if(list){
//		for(list->rewind(); !list->atEnd(); list->step()){
//			vec.push_back(list->get());
//		}
//	}
//}

void translate(const vector<TableAlias*>& vec, List<TableAlias*>*& list);
void translate(List<TableAlias*>* list,  vector<TableAlias*>& vec);
void translate(const vector<BaseSelection*>& vec, List<BaseSelection*>*& list);
void translate(List<BaseSelection*>* list,  vector<BaseSelection*>& vec);

void translate(const vector<TableAlias*>& vec, List<TableAlias*>*& list){
	delete list;
	list = new List<TableAlias*>;

	vector<TableAlias*>::const_iterator it;
	for(it = vec.begin(); it != vec.end(); ++it){
		list->append(*it);
	}
}

void translate(List<TableAlias*>* list,  vector<TableAlias*>& vec){
	assert(vec.empty()); 
	if(list){
		for(list->rewind(); !list->atEnd(); list->step()){
			vec.push_back(list->get());
		}
	}
}

void translate(const vector<BaseSelection*>& vec, List<BaseSelection*>*& list){
	delete list;
	list = new List<BaseSelection*>;

	vector<BaseSelection*>::const_iterator it;
	for(it = vec.begin(); it != vec.end(); ++it){
		list->append(*it);
	}
}

void translate(List<BaseSelection*>* list,  vector<BaseSelection*>& vec){
	assert(vec.empty()); 
	if(list){
		for(list->rewind(); !list->atEnd(); list->step()){
			vec.push_back(list->get());
		}
	}
}
//****************************************

Site* QueryTree::createSite(){
	if(!namesToResolve->isEmpty()){
		TRY(resolveNames(), 0);
	}
	LOG(logFile << "Query was:\n";)
	LOG(logFile << "   select ";)
	LOG(displayList(logFile, selectList, ", ");)
	LOG(logFile << endl << "   from ";)
	LOG(displayList(logFile, tableList);)
	predicateList = new List<BaseSelection*>;
	if(predicates){
		LOG(logFile << endl << "   where ";)
		LOG(predicates->display(logFile);)
		LOG(logFile << endl;)
		LOG(logFile << "Predicates after cnf:\n";)
		BaseSelection* newPredicates = predicates->cnf();
		if(newPredicates){
			delete predicates;
			predicates = newPredicates;
		}
		LOG(logFile << "   ";)
		LOG(predicates->display(logFile);)
		if(predicates->insertConj(predicateList)){
			delete predicates;
		}
	}
	LOG(logFile << endl << "Predicate list:\n   ";)
	LOG(displayList(logFile, predicateList);)
	LOG(logFile << endl;)

	// typecheck the query

	vector<TableAlias*> tableVec;
	vector<BaseSelection*> selectVec;
	vector<BaseSelection*> predicateVec;
	vector<BaseSelection*> groupByVec;
	vector<BaseSelection*> orderByVec;

	translate(tableList, tableVec);
	translate(predicateList, predicateVec);
	translate(groupBy, groupByVec);
	translate(orderBy, orderByVec);

	TRY(typeCheck.initialize(tableVec), 0);

	if(!selectList){

		// select *

		typeCheck.setupSelList(selectVec);
	}
	else {

		aggregates = new 
			Aggregates(selectList,sequenceby,withPredicate,groupBy);

		TRY(aggregates->typeCheck(typeCheck), 0);
		translate(selectList, selectVec);
	}
	TRY(typeCheck.resolve(predicateVec), 0);
	TRY(typeCheck.resolve(groupByVec), 0);
	TRY(typeCheck.resolve(orderByVec), 0);

	if(typeCheckOnly){
		int numFlds = selectVec.size();
		TypeID* types = new TypeID[numFlds];
		string* attrs = new string[numFlds];
		vector<BaseSelection*>::const_iterator it;
		int i = 0;
		for(it = selectVec.begin(); it != selectVec.end(); ++it, i++){
			types[i] = (*it)->getTypeID();
			attrs[i] = (*it)->toString();
		}
		return new ISchemaSite(numFlds, types, attrs);
	}

	// check if this is the min-max query
	// if so, lookup a min-max table to see if there exists an entry
	// if entry for this table found, switch the table name

	bool minMaxCond = tableVec.size() == 1 &&
				predicateVec.empty() &&
				groupByVec.empty() &&
				orderByVec.empty();

	if(minMaxCond && MinMax::isApplicable(selectVec)){
		TableAlias* table = tableVec.front();
		TRY(TableAlias* replacement = MinMax::createReplacement(table), 0);
		if(replacement){
			tableVec.front() = replacement;
			delete table;
		}
	}

	// optimization starts here

	translate(tableVec, tableList);
	translate(selectVec, selectList);
	translate(predicateVec, predicateList);
	translate(groupByVec, groupBy);
	translate(orderByVec, orderBy);

	tableList->rewind();
	int numSites = 0;
     List<Site*>* sites = new List<Site*>;
	while(!tableList->atEnd()){
		TableAlias* ta = tableList->get();
		string fullPathNm = ta->getTable()->toString();
		Site* site = NULL;
		TRY(site = ta->createSite(), 0);
		assert(site);
		site->addTable(ta);
		site->setFullNm(fullPathNm);	// used to retreive indexes
		if(!sites->exists(site)){
			sites->append(site);
			numSites++;
		}
		tableList->step();
	}
	if(aggregates && aggregates->isApplicable()){
			   
	   	// Change the select list
		TRY(selectList = aggregates->filterList(),NULL);

#if defined(DEBUG)
		cerr << " Removing aggregates from the list\n";
		displayList(cerr, selectList, ", ");
		cerr << endl;
#endif
	}
	assert(groupBy);

	LOG(logFile << "Decomposing query on " << numSites << " sites\n";)
     sites->rewind();
     while(!sites->atEnd()){
          Site* current = sites->get();
          current->filter(selectList, predicateList);
		LOG(logFile << current->getName());
          LOG(current->display(logFile));
		LOG(logFile << endl);
          sites->step();
    }

	// checkOrphan function is obsolete
	// this will be check during typechecking

//	TRY(checkOrphanInList(selectList), 0);
//	TRY(checkOrphanInList(predicateList), 0);

     LOG(logFile << "Global query:\n";)
	LOG(logFile << "   select ";)
     LOG(displayList(logFile, selectList, ", "));
     LOG(logFile << "\n   where ";)
     LOG(displayList(logFile, predicateList, ", "));
	LOG(logFile << endl;)
//	string* types;
	string option = "execute";
	TRY(typifyList(sites, option), 0);
	sites->rewind();
	LOG(logFile << "Typified sites\n");
	Site* dirtyDelete = NULL;
     while(!sites->atEnd()){
		TRY(List<Site*>* alters = 
			sites->get()->generateAlternatives(), NULL);
		assert(alters);
		if(!alters->isEmpty()){
			Site* current = sites->get();
#ifdef DEBUG
			cout << "Alters for \"" << current->getName() << "\" are:\n";
			displayList(cout, alters, "\n");
#endif
			alters->rewind();
			Site* minCostAlt = alters->get();
			double minCost = minCostAlt->evaluateCost();
			while(!alters->atEnd()){
				Site* currAlt = alters->get();
				double currCost = currAlt->evaluateCost();
				if(currCost < minCost){
					minCost = currCost;
					minCostAlt = currAlt;
				}
				alters->step();
			}
			sites->replace(minCostAlt);

			// must not delete current because it is used to
			// typify the global query
			// memory and fd leak!!!

			dirtyDelete = current;

		}
		Site* current = sites->get();
		LOG(logFile << current->getName());
          LOG(current->display(logFile));
		LOG(logFile << endl);
          sites->step();
     }
	assert(selectList);
	if(!selectList){
		selectList = createGlobalSelectList(sites);
		// already typified
	}
	else{
//		TRY(typifyList(selectList, sites), 0);
	}
//	TRY(typifyList(predicateList, sites), 0);
	TRY(boolCheckList(predicateList), 0);
	
	// This is to put the sequenceby table in the front
	// of the list making it the outer instead of the inner.

	/*
	sites->rewind();
	if (sequencebyTable){
		while(!sites->atEnd()){
			Site * check = sites->get();
			if (check->have(*sequencebyTable)){
				sites->remove();
				sites->append(check);
				break;
			}
			sites->step();
		}
	}	
	*/

	if (joinList && !joinList->isEmpty()){
		List<Site*>* joinGroups = new List<Site *>;
		while(!joinList->atEnd()){
			JoinTable * jTable = joinList->get();
			joinList->step();
			Site* st;
			TRY( st = jTable->Plan(sites,selectList,predicateList),0);
			//sites->append(st);
			joinGroups->append(st);
		}
		sites->addList(joinGroups);
	}	
	sites->rewind();
	Site * inner = sites->get();
	sites->step();
	Site* siteGroup = NULL;
	
	while(!sites->atEnd()){
		Site* outer = sites->get();
		
		siteGroup = new SiteGroup(inner, outer);
		inner = siteGroup;
		siteGroup->filter(selectList, predicateList);
		siteGroup->typify(option);
		sites->step();
	}
	if(!siteGroup){
		siteGroup = inner;
	}

	if(!predicateList->isEmpty()){
		cerr << "Predicate list should be empty: ";
		displayList(cerr, predicateList, ", ");
	}
	assert(predicateList->isEmpty());
	if(selectList->cardinality() > siteGroup->getSelectList()->cardinality()){

		// Every site has taken what belongs to it.
		// Create top site that takes everything left over 
		// (constants or nothing)

		siteGroup = new LocalTable(siteGroup->getName(), siteGroup);
		siteGroup->filterAll(selectList);
		TRY(siteGroup->typify(option), NULL);
	}

	assert(groupBy);
	if(!groupBy->isEmpty()){

		// group by requires sorted input 

	  // if sequence by is present, 
	  // sort on concat of group by and sequence by fields
	  if (sequenceby && !sequenceby->isEmpty()) {
	    grpAndSeqFields = new List<BaseSelection*>;

	    for (groupBy->rewind(); !groupBy->atEnd(); groupBy->step()) {
	      grpAndSeqFields->append(groupBy->get());
	    }
	    
	    for(sequenceby->rewind();!sequenceby->atEnd();sequenceby->step()){
	      grpAndSeqFields->append(sequenceby->get());
	    }

	    siteGroup = new Sort(siteGroup->getName(), grpAndSeqFields, siteGroup, sortOrdering);
		
	  }
	  else {

	    siteGroup = new Sort(siteGroup->getName(), groupBy, siteGroup, sortOrdering);
	  }

	  TRY(siteGroup->typify(option), NULL);
	}

	if(aggregates && aggregates->isApplicable()){
		TRY(aggregates->typify(siteGroup->getName(), siteGroup), NULL);
		siteGroup = aggregates;
	}

	assert(orderBy);
	if(!orderBy->isEmpty()){
		siteGroup = new Sort(siteGroup->getName(), orderBy, siteGroup,sortOrdering);
		TRY(siteGroup->typify(option), NULL);
	}

	LOG(logFile << "Global Plan: \n";)
	LOG(siteGroup->display(logFile, DETAIL);)
	LOG(logFile << endl;)
	assert(predicateList->cardinality() == 0);
	delete predicateList;	// destroy list too
	return siteGroup;
}
