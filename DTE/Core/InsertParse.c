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
#include "listop.h"
#include "ParseTree.h"

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

Site* InsertParse::createSite(){
	LOG(logFile << "Inserting into ");
	LOG(tableName->display(logFile));
	LOG(logFile << " (";)
	LOG(displayList(logFile, (List<BaseSelection*>*) fieldList, ", ");)
	LOG(logFile << ")" << endl;)

     Catalog* catalog = getRootCatalog();
     assert(catalog);
     TRY(Site* site = catalog->find(tableName), NULL);
     delete catalog;

	assert(site);

	TRY(site->typify(""), NULL);
	int numFlds = site->getNumFlds();
	if(numFlds != fieldList->cardinality()){
		THROW(new Exception("Number of fields do not match"), NULL);
	}
	String* types = site->getTypeIDs();

// What follows is just a temporary kludge. 
// One should construct a tuple and inset it

	fieldList->rewind();
	assert(fieldList->cardinality() == 1);
	ConstantSelection* sel = fieldList->get();
	strstream tmp;
	sel->display(tmp);
	String inStr = stripQuotes(tmp);
	TRY(site->writeOpen(), NULL);
	site->write(inStr);
	site->writeClose();
	delete site;
	return new Site();
}
