#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<stdlib.h>
#include<stdio.h>	// for perror
#include<errno.h>

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
#include "RTree.h"
#include "RTreeRead.h"

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
	LOG(logFile << "Creating Index " << *indexName;)
	LOG(logFile << " on " << *tableName << "(";)
	LOG(displayList(logFile, attributeList, ", ");)
	LOG(logFile << ")" << endl;)

	Catalog catalog;
	String catalogName;
	catalogName += getenv("DEVISE_SCHEMA");
	catalogName += "/catalog.dte";
	TRY(catalog.read(catalogName), 0);
	Catalog::Interface* interf = NULL;
	TRY(interf = catalog.find(*tableName), 0);
	assert(interf);
	TRY(Site* site = interf->getSite(), 0);	// can be old site
	site->addTable(new TableAlias(tableName));

	List<BaseSelection*>* emptyList = new List<BaseSelection*>;
	site->filter(attributeList, emptyList);
	delete emptyList;
	TRY(checkOrphanInList(attributeList), 0);

	String option = "execute";
	TRY(site->typify(option), 0);
	LOG(logFile << "Typified sites\n");
	LOG(logFile << site->getName());
	LOG(site->display(logFile));
	LOG(logFile << endl);

	String* attrNames = site->getAttNamesOnly();
	LOG(logFile << "Enumeration:\n";)
	TRY(site->enumerate(), 0);
	LOG(site->display(logFile, DETAIL);)
	LOG(logFile << endl;)

	int numFlds = site->getNumFlds();
	String* types = site->getTypeIDs();
	String rtreeSchema;
	for(int i = 0; i < numFlds; i++){
		rtreeSchema += rTreeEncode(types[i]);
	}

	ostrstream line1;
	int recIDSize = sizeof(Offset);	// for now
	int points = 1; // set to 0 for rectangles
	line1 << numFlds << " " << recIDSize << " " << points << " ";
	line1 << rtreeSchema;
	int fillSize  = (line1.pcount() + 1) % 8;	// allign on 8 byte boundary
	for(int i = 0; i < fillSize; i++){
		line1 << " ";
	}
	line1 << endl;
	String bulkfile = *indexName + ".bulk";
	ofstream ind(bulkfile);
	assert(ind);
	ind << line1.rdbuf();
	Tuple* tup;
	int fixedSize; 
	int tupSize;
	Offset offset = site->getOffset();
	cout << "offset = " << offset << endl;
     tup = site->getNext();
	assert(tup); // make sure this is not empty
	fixedSize = packSize(tup, types, numFlds);
	char* flatTup = new char[fixedSize];
	marshal(tup, flatTup, types, numFlds);
	ind.write(flatTup, fixedSize);
	ind.write((char*) &offset, sizeof(Offset));
	offset = site->getOffset();
	cout << "offset = " << offset << endl;
     while((tup = site->getNext())){
		tupSize = packSize(tup, types, numFlds);
		if(tupSize != fixedSize){
			assert(0);
		}
		marshal(tup, flatTup, types, numFlds);
		ind.write(flatTup, fixedSize);
		ind.write((char*) &offset, sizeof(Offset));
		offset = site->getOffset();
		cout << "offset = " << offset << endl;
     }
	delete flatTup;
	ind.close();
	String convBulk = bulkfile + ".conv";
	String cmd = "convert_bulk < " + bulkfile + " > " + convBulk;
	if(system(cmd) == -1){
		perror("system:");
		String msg = "Failed to convert bulk data";
		THROW(new Exception(msg), NULL);
	}

	page_id_t root1;
	int bulk_file = open(convBulk.chars(), O_RDWR, 0600);

	genrtree_m rtree_m;
	rtree_m.bulk_load(bulk_file, root1, false); // example bulkload
	// this has created index

	close(bulk_file);
	printf("Created index with root page: %d\n", root1.pid);
	// note, you MUST keep root page

	RTreeIndex* index = new RTreeIndex(numFlds, types, attrNames, root1.pid);
	interf->addIndex(index);		// add this index to the catalog
	catalog.write(catalogName);

	return new Site();
}
