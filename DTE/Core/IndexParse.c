#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<stdlib.h>

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

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

void IndexParse::resolveNames(){	// throws exception
	namesToResolve->rewind();
	String* replacement = tableName;
	for(int i = 0; i < namesToResolve->cardinality(); i++){
		String* current = namesToResolve->get();
		*current = *replacement;
		namesToResolve->step();
	}
}

Site* IndexParse::createSite(){
	if(!namesToResolve->isEmpty()){
		TRY(resolveNames(), 0);
	}
/*
	LOG(logFile << "Query was:\n";)
	LOG(logFile << "   select ";)
	LOG(displayList(logFile, selectList, ", ");)
	LOG(logFile << endl << "   from ";)
	LOG(displayList(logFile, tableList);)
	List<BaseSelection*>* predicateList = new List<BaseSelection*>;
	LOG(logFile << endl << "Predicate list:\n   ";)
	LOG(displayList(logFile, predicateList);)
	LOG(logFile << endl;)

	tableList->rewind();
	int numSites = 0;
	Catalog catalog;
	String catalogName;
	catalogName += getenv("DEVISE_SCHEMA");
	catalogName += "/catalog.dte";
	TRY(catalog.read(catalogName), 0);
     List<Site*>* sites = new List<Site*>;
	while(!tableList->atEnd()){
		TableAlias* ta = tableList->get();
          Catalog::Interface* interf = NULL;
		if(ta->isQuote()){
			TRY(interf = catalog.toInterface(*ta->table), 0);
		}
		else{
			TRY(interf = catalog.find(*ta->table), 0);
		}
		assert(interf);
		TRY(Site* site = interf->getSite(), 0);	// can be old site
		site->addTable(ta);
		if(!sites->exists(site)){
			sites->append(site);
			numSites++;
		}
		tableList->step();
	}
	Aggregates aggregates(selectList);
	if(aggregates.isApplicable()){
		cout << "Aggregates not implemented\n";
		exit(1);
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
          Site* current = sites->get();
		List<Site*>* alters = current->generateAlternatives();
		assert(alters);
		cout << "Alternatives for \"" << current->getName() << "\" are:\n";
		displayList(cout, alters, "\n");
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
	LOG(logFile << "Plan: \n";)
	LOG(siteGroup->display(logFile, DETAIL);)
	LOG(logFile << endl;)
	assert(predicateList->cardinality() == 0);
	LOG(logFile << "Enumeration:\n";)
	TRY(siteGroup->enumerate(), 0);
	LOG(siteGroup->display(logFile, DETAIL);)
	LOG(logFile << endl;)
	delete predicateList;	// destroy list too
*/
	return new Site();
}
