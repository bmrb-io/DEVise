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

#ifdef __GNUG__
#pragma implementation "queue.h"
#endif

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

Site* QueryTree::createSite(){
	if(!namesToResolve->isEmpty()){
		TRY(resolveNames(), 0);
	}
	LOG(logFile << "Query was:\n";)
	LOG(logFile << "   select ";)
	LOG(displayList(logFile, selectList, ", ");)
	LOG(logFile << endl << "   from ";)
	LOG(displayList(logFile, tableList);)
	List<BaseSelection*>* predicateList = new List<BaseSelection*>;
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

	TypeCheck typeCheck;
	vector<TableAlias*> tableVec;		// TableList
	vector<BaseSelection*> selectVec;		// TableList
	vector<BaseSelection*> predicateVec;		// TableList
	vector<BaseSelection*> groupByVec;		// TableList
	vector<BaseSelection*> orderByVec;		// TableList

	typeCheck.load(tableVec);
	if(!selectList){

		// select *

		typeCheck.setupSelList(selectVec);
	}
	else {
		typeCheck.load(selectVec);
	}
	typeCheck.load(predicateVec);
	typeCheck.load(groupByVec);
	typeCheck.load(orderByVec);

	// check if this is the min-max query
	// if so, lookup a min-max table to see if there exists an entry
	// if entry for this table found, switch the table name

	assert(predicateList && groupBy && orderBy);
	bool minMaxCond = tableList->cardinality() == 1 &&
				predicateList->isEmpty() &&
				groupBy->isEmpty() &&
				orderBy->isEmpty();

	if(minMaxCond && MinMax::isApplicable(selectList)){
		tableList->rewind();
		TableAlias* table = tableList->get();
		TRY(TableAlias* replacement = MinMax::createReplacement(table), 0);
		if(replacement){
			tableList->replace(replacement);
			delete table;
		}
	}

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

	// For the sequenceby clause;
	// find the sequecing attribute..(Only table name is known initially)

	/* The seq attribute is now derived directly from the query
	BaseSelection * sequenceby = NULL;
	if (sequencebyTable){
		sites->rewind();
		while(!sites->atEnd()){
		
			Site * check = sites->get();
			sites->step();
			if (check->have(*sequencebyTable)){
				string * attrib = check->getOrderingAttrib();
				if (!attrib  || *attrib == ""){
					string msg = "Table "+*sequencebyTable+" is not a sequence";
					THROW(new Exception(msg),NULL);
				} 
				sequenceby=new PrimeSelection(
					sequencebyTable, new string(*attrib));
			}	
		}
	}
	*/

	// Need to fix a mamimum for this..
	Aggregates **aggregates =new Aggregates*[MAX_AGG];
	int count = 0;
    aggregates[count] = 
    	new Aggregates(selectList,sequenceby,withPredicate,groupBy);

	// while(aggregates[count]->isApplicable())
	if(aggregates[count]->isApplicable()){
			   
	   	// Change the select list
		TRY(selectList = aggregates[count]->filterList(),NULL);

#if defined(DEBUG)
		cerr << " Removing aggregates from the list\n";
		displayList(cerr, selectList, ", ");
		cerr << endl;
#endif
		count ++;
		if (count == MAX_AGG){
			THROW(new Exception(" Numbr of nesting levels too high "),NULL);
		}
    	aggregates[count] = new Aggregates(
		selectList,sequenceby,withPredicate,groupBy);
	}
	count --;

	assert(groupBy);
	if(count == -1 && !groupBy->isEmpty()){
		aggregates[0] = new 
			Aggregates(selectList,sequenceby,withPredicate,groupBy);
		count = 0;
	}

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
	string* types;
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
	if(!selectList){
		selectList = createGlobalSelectList(sites);
		// already typified
	}
	else{
		TRY(typifyList(selectList, sites), 0);
	}
	TRY(typifyList(predicateList, sites), 0);
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

		siteGroup = new Sort(siteGroup->getName(), groupBy, siteGroup);
		TRY(siteGroup->typify(option), NULL);
	}

	for(int k = count; k >= 0;k--){
		TRY(aggregates[k]->typify(siteGroup->getName(), siteGroup), NULL);
		siteGroup = aggregates[k];
	}

	assert(orderBy);
	if(!orderBy->isEmpty()){
		siteGroup = new Sort(siteGroup->getName(), orderBy, siteGroup);
		TRY(siteGroup->typify(option), NULL);
	}

	LOG(logFile << "Global Plan: \n";)
	LOG(siteGroup->display(logFile, DETAIL);)
	LOG(logFile << endl;)
	assert(predicateList->cardinality() == 0);
	delete predicateList;	// destroy list too
//	delete dirtyDelete;  	// it core dumps
	return siteGroup;
}
