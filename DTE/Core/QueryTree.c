#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<stdlib.h>

#ifdef __GNUG__
#pragma implementation "queue.h"
#endif

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

const int DETAIL = 1;
LOG(ofstream logFile("log_file.txt");)

void QueryTree::resolveNames(){	// throws exception
     if(tableList->cardinality() > 1){
          String msg = "cannot resolve attribute referencies";
          THROW(new Exception(msg), );
     }
     else{
          tableList->rewind();
          namesToResolve->rewind();
          String* replacement = tableList->get()->getAlias();
		assert(replacement);
          for(int i = 0; i < namesToResolve->cardinality(); i++){
               String* current = namesToResolve->get();
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

	tableList->rewind();
	int numSites = 0;
	String catalogName;
	char* dev_schema = getenv("DEVISE_SCHEMA");
	if(dev_schema){
		catalogName = String(dev_schema) + "/catalog.dte";
	}
	else {
		catalogName = "catalog.dte";
	}
	Catalog catalog(catalogName);
     List<Site*>* sites = new List<Site*>;
	while(!tableList->atEnd()){
		TableAlias* ta = tableList->get();
		Site* site = NULL;
		if(ta->isQuote()){
			QuoteAlias* qa = (QuoteAlias*) ta;
			// TRY(site = catalog.toSite(*qa->getQuote()), 0);
		}
		else{
			TRY(site = catalog.find(ta->getTable()), 0);
		}
		assert(site);

		// To do: check if this site already exists in sites

		site->addTable(ta);
		if(!sites->exists(site)){
			sites->append(site);
			numSites++;
		}
		tableList->step();
	}
	// For the sequenceby clause;
	// find the sequecing attribute..(Only table name is known initially)
	BaseSelection * sequenceby = NULL;
	if (sequencebyTable){
		sites->rewind();
		while(!sites->atEnd()){
		
			Site * check = sites->get();
			sites->step();
			if (check->have(sequencebyTable)){
				String * attrib = check->getOrderingAttrib();
				if (!attrib  || *attrib == ""){
					String msg = "Table "+*sequencebyTable+" is not a sequence";
					THROW(new Exception(msg),NULL);
				} 
				sequenceby=new PrimeSelection(sequencebyTable,new Path(attrib));
			}	
		}
	}
	Aggregates *aggregates =new Aggregates(selectList,sequenceby,withPredicate);
	if(aggregates->isApplicable()){
			   
	   	// Change the select list
		TRY(selectList = aggregates->filterList(),NULL);
		LOG( logFile << " Removing aggregates from the list\n" );
		LOG(displayList(logFile, selectList, ", "));
		LOG(logFile << endl);
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

	TRY(checkOrphanInList(selectList), 0);
	TRY(checkOrphanInList(predicateList), 0);
	if(!selectList){
		APPLY(makeNonComposite(), predicateList);
	}
     LOG(logFile << "Global query:\n";)
	LOG(logFile << "   select ";)
     LOG(displayList(logFile, selectList, ", "));
     LOG(logFile << "\n   where ";)
     LOG(displayList(logFile, predicateList, ", "));
	LOG(logFile << endl;)
	String* types;
	String option = "execute";
	TRY(typifyList(sites, option), 0);
	sites->rewind();
	LOG(logFile << "Typified sites\n");
     while(!sites->atEnd()){
		TRY(List<Site*>* alters = 
			sites->get()->generateAlternatives(), NULL);
		assert(alters);
		if(!alters->isEmpty()){
			Site* current = sites->get();
			cout << "Alters for \"" << current->getName() << "\" are:\n";
			displayList(cout, alters, "\n");
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

	sites->rewind();
	if (sequencebyTable){
		while(!sites->atEnd()){
			Site * check = sites->get();
			if (check->have(sequencebyTable)){
				sites->remove();
				sites->prepend(check);
				break;
			}
			sites->step();
		}
	}	
	
	sites->rewind();
	Site* inner = sites->get();
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
	if (aggregates->isApplicable()){

		aggregates->typify(siteGroup);
		siteGroup = aggregates;
	}

	LOG(logFile << "Plan: \n";)
	LOG(siteGroup->display(logFile, DETAIL);)
	LOG(logFile << endl;)
	assert(predicateList->cardinality() == 0);
	LOG(logFile << "Enumeration:\n";)
	TRY(siteGroup->enumerate(), 0);
	LOG(siteGroup->display(logFile, DETAIL);)
	LOG(logFile << endl;)
	delete predicateList;	// destroy list too
	return siteGroup;
}
