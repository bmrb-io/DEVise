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
  Revision 1.57  1998/12/28 21:32:22  donjerko
  Optimizer is now used by default.

  Revision 1.56  1998/12/11 17:00:04  beyer
  small cleanup

  Revision 1.55  1998/11/23 19:18:51  donjerko
  Added support for gestalts

  Revision 1.54  1998/10/08 22:27:10  donjerko
  *** empty log message ***

  Revision 1.53  1998/07/24 04:37:53  donjerko
  *** empty log message ***

  Revision 1.52  1998/06/28 21:47:40  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.51  1998/06/17 22:20:04  donjerko
  *** empty log message ***

  Revision 1.50  1998/03/12 18:23:24  donjerko
  *** empty log message ***

  Revision 1.49  1998/02/09 21:12:15  donjerko
  Added Bin by clause and implementation.


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

  Revision 1.25  1997/06/21 22:48:00  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.24.2.1  1997/05/21 20:34:24  weaver
  Changes needed by new ColorManager

  Revision 1.24  1997/05/07 18:55:49  donjerko
  Fixed the problem when constants are selected.

  Revision 1.22  1997/04/18 20:46:17  donjerko
  Added function pointers to marshall types.

  Revision 1.21  1997/04/14 20:44:12  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.20  1997/04/08 01:47:33  donjerko
  Set up the basis for ORDER BY clause implementation.

 */

// #define DEBUG

#define USE_OPTIMIZER

// Changed to non-pragma templates method. CEW 5/12/97
//#ifdef __GNUG__
//#pragma implementation "queue.h"
//#endif

//#include "queue.h"
#include "myopt.h"
//#include "site.h"
#include "types.h"
#include "exception.h"
#include "catalog.h"
#include "DevRead.h"
//#include "listop.h"
#include "Aggregates.h"
#include "ParseTree.h"
#include "Sort.h"
//#include "TypeCheck.h"
#include "MinMax.h"
#include "Interface.h"
#include "Optimizer.h"
#include <algorithm>
#include "DteSymbolTable.h"

//#include<iostream.h>   erased for sysdep.h
//#include<memory.h>   erased for sysdep.h
#include <string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
#include "sysdep.h"
#include "DTE/util/DteAlgo.h"


const int DETAIL = 1;
LOG(ofstream logFile("log_file.txt"));


QueryTree::QueryTree(OptExprList* selectList, // SELECT
                     vector<TableAlias*>* tableVec, // FROM
                     OptExpr* predicates,     // WHERE
                     OptExprList* groupBy,
                     OptExprList* sequenceby,
                     OptExpr* withPredicate,
                     OptExpr* havingPredicate,
                     OptExprList* orderBy,
                     string* sortOrdering)
  : selectVec(*selectList), tableVec(*tableVec), 
    predicateVec(), groupByVec(*groupBy), 
    sequenceByVec(*sequenceby), withPredicate(withPredicate), 
    havingPredicate(havingPredicate),
    orderByVec(*orderBy), sortOrdering(*sortOrdering), schema(0)
{
  assert(selectList);
  assert(tableVec);
  assert(groupBy);
  assert(sequenceby);
  assert(orderBy);
  assert(sortOrdering);
  delete selectList;
  delete tableVec;
  delete groupBy;
  delete sequenceby;
  delete orderBy;
  delete sortOrdering;
  if( predicates ) {
    predicateVec.push_back(predicates);
  }
}


QueryTree::~QueryTree()
{
  delete havingPredicate;
  delete withPredicate;
  delete_all(tableVec);
  delete_all(selectVec);
  delete_all(predicateVec);
  delete_all(groupByVec);
  delete_all(sequenceByVec);
  delete_all(orderByVec);
  delete schema;
}

string QueryTree::guiRepresentation() const {
	string retVal;
	ostringstream out;
	if(selectVec.size() == 0) { // select *
		out << "*";
	}
	else{
		print_list(out, selectVec);
	}
	retVal += addQuotes(out.str()) + " ";
	ostringstream out1;
	print_list(out1, tableVec);
	retVal += addQuotes(out1.str()) + " ";
	if(predicateVec.size() > 0) {
                assert(predicateVec.size() == 1);
		retVal += addQuotes(predicateVec[0]->toString());
	}
	else{
		retVal += "\"\"";
	}
	retVal += " ";
	ostringstream out2;
	print_list(out2, groupByVec);
	retVal += addQuotes(out2.str()) + " ";
	ostringstream out3;
	print_list(out3, sequenceByVec);
	retVal += addQuotes(out3.str()) + " ";
	ostringstream out4;
	print_list(out4, orderByVec);
	retVal += addQuotes(out4.str()) + " ";
	return retVal;
}

//kb: move this stuff
template <class Operation> 
class binder2nd_ref
  : public unary_function<typename Operation::first_argument_type,
                          typename Operation::result_type> {
protected:
  Operation op;
  typename Operation::second_argument_type value;
public:
  binder2nd_ref(const Operation& x,
                const typename Operation::second_argument_type y) 
    : op(x), value(y) {}
  typename Operation::result_type
  operator()(const typename Operation::first_argument_type& x) const {
    return op(x, value); 
  }
};

template <class Operation, class T>
inline binder2nd_ref<Operation> bind2nd_ref(const Operation& op, const T& x) {
  return binder2nd_ref<Operation>(op, x);
}

template <class Container, class Function>
void for_each(Container& c, Function f)
{
  for_each(c.begin(), c.end(), f);
}

void QueryTree::addSelectStar()
{
  int N = tableVec.size();
  for(int tabId = 0 ; tabId < N ; tabId++) {
    const TableAlias& ta = *(tableVec[tabId]);
    const ISchema& schema = ta.getISchema();
    const vector<string>& attrs = schema.getAttributeNames();
    //const DteTupleAdt& tupAdt = schema.getAdt();
    int M = schema.getNumFields();
    for(int slot = 0 ; slot < M ; slot++) {
      OptField* f = new OptField(ta.getAlias(), attrs[slot]);
      selectVec.push_back(f);
    }
  }
}


bool ParseTree::typeCheckVec(OptExprList& exprs, const DteSymbolTable& symtab)
{
  
  for(OptExprList::iterator i = exprs.begin() ; i != exprs.end() ; i++) {
    if( ! (*i)->typeCheck(symtab) ) {
      return false;
    }
  }
  return true;
}


bool QueryTree::typeCheck()
{
  LOG(logFile << "Query was:\n");
  LOG(logFile << "   select ");
  LOG(displayList(logFile, selectList, ", "));
  LOG(logFile << endl << "   from ");
  LOG(displayList(logFile, tableList));
  if(predicateVec.size() > 0) {
    assert(predicateVec.size() == 1);
    OptExpr* predicates = predicateVec[0];
    predicateVec.clear();
    LOG(logFile << endl << "   where ");
    LOG(predicates->display(logFile));
    LOG(logFile << endl);
    LOG(logFile << "Predicates after cnf:\n");
    OptExpr* newPredicates = predicates->cnf();
    if(newPredicates){
      delete predicates;
      predicates = newPredicates;
    }
    LOG(logFile << "   ");
    LOG(predicates->display(logFile));
    if( predicates->insertConj(predicateVec) ) {
      // top was an AND, so delete it
      delete predicates;
    }
    // all of 'predicates' is now in predicateVec in CNF
  }
  LOG(logFile << endl << "Predicate list:\n   ");
  LOG(displayList(logFile, predicateVec));
  LOG(logFile << endl);
  
  if(selectVec.size() == 0) { // select *
    //typeChecker.setupSelList(selectVec);
    addSelectStar();            // kb:move this
  }

#if !defined(USE_OPTIMIZER)
  #error old optimizer is broken
  aggregates = new Aggregates(
    selectVec, sequenceByVec, withPredicate, groupByVec);
  
  TRY(aggregates->typeCheck(typeChecker), 0);
#endif

  // typecheck the query
  //TRY(typeChecker.initialize(tableVec), 0);
  //TRY(typeChecker.resolve(selectVec), 0);
  //TRY(typeChecker.resolve(predicateVec), 0);
  //TRY(typeChecker.resolve(groupByVec), 0);
  //TRY(typeChecker.resolve(orderByVec), 0);

  DteSymbolTable symtab(tableVec);
  if( !typeCheckVec(selectVec, symtab) ) {
    return false;
  }
  if( !typeCheckVec(predicateVec, symtab) ) {
    return false;
  }
  if( !typeCheckVec(groupByVec, symtab) ) {
    return false;
  }
  if( !typeCheckVec(orderByVec, symtab) ) {
    return false;
  }
  return true;
}


const ISchema* QueryTree::getISchema()
{
	if(schema) {
		return schema;
	}
        typeCheck();
	setupSchema();
	return schema;
}

void QueryTree::setupSchema() {
	int numFlds = selectVec.size();
        DteAdtList adts(numFlds);
        vector<string> names(numFlds);
	for(int i = 0 ; i < numFlds ; i++) {
		adts[i] = selectVec[i]->getAdt();
		names[i] = selectVec[i]->toString();
	}
	schema = new ISchema(adts, names);
}

Iterator* QueryTree::createExec(){

	if(!schema){

		// type checking needs to be done

		TRY(getISchema(), 0);
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

#ifdef USE_OPTIMIZER

//	Query query(selectVec, tableVec, predicateVec, orderByVec);
	Query query(selectVec, tableVec, predicateVec);
	Optimizer optimizer(query);
	optimizer.run();
//	optimizer.display(cerr);
//	cerr << endl;
	return optimizer.createExec();

#else
#error USE_OPTIMIZER is required...
        //kb: remove this code
	// once optimizer is in place, rest of this function will go away.
	translate(tableVec, tableList);
	translate(selectVec, selectList);
	translate(predicateVec, predicateList);
	translate(groupByVec, groupBy);
	translate(sequenceByVec, sequenceby);
	translate(orderByVec, orderBy);

	tableList->rewind();
	int numSites = 0;
     List<Site*>* sites = new List<Site*>;
	while(!tableList->atEnd()){
		TableAlias* ta = tableList->get();
		string fullPathNm = ta->getTable().toString();
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
	if(selectList->cardinality() > siteGroup->getNumFields()){

		// Every site has taken what belongs to it.
		// Create top site that takes everything left over 
		// (constants or nothing)

		siteGroup = new LocalTable(siteGroup->getName(), siteGroup);
		siteGroup->filterAll(selectList);
		TRY(siteGroup->typify(option), NULL);
	}

	assert(groupBy);
	if(!groupBy->isEmpty() || (sequenceby && !sequenceby->isEmpty())){

		// group by requires sorted input 

	  // if sequence by is present, 
	  // sort on concat of group by and sequence by fields
	  if (sequenceby && !sequenceby->isEmpty()) {

	    grpAndSeqFields = new List<OptExpr*>;

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
	return siteGroup->createExec();
#endif
}
