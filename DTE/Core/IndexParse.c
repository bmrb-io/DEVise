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
#include "Utility.h"

#ifdef NO_RTREE
	#include "RTreeRead.dummy"
	#include "RTreeCommon.h"
#else
	#include "RTree.h"
	#include "RTreeRead.h"
#endif

static const int DETAIL = 1;
LOG(extern ofstream logFile;)

Site* IndexParse::createSite(){

#ifndef NO_RTREE
	LOG(logFile << "Creating ");
	if(standAlone){
		LOG(logFile << "StandAlone ");
	}
	LOG(logFile << "index " << *indexName;)
	LOG(logFile << " on ");
	LOG(tableName->display(logFile));
	LOG(logFile << " (";)
	LOG(displayList(logFile, keyAttrs, ", ");)
	LOG(logFile << ")");
	if(additionalAttrs){
		LOG(displayList(logFile, additionalAttrs, ", "));
	}
	LOG(logFile << endl;)

	String tablename = tableName->toString();
     Catalog* catalog = getRootCatalog();
     assert(catalog);
     TRY(Site* site = catalog->find(tableName), 0);

	assert(site);
	site->addTable(new TableAlias(tableName, new String(*indexName)));

	List<BaseSelection*>* keyList = 
		createSelectList(*indexName, keyAttrs);
	List<BaseSelection*>* addList = 
		createSelectList(*indexName, additionalAttrs);
	List<BaseSelection*>* attributeList = new List<BaseSelection*>;
	attributeList->addList(keyList);
	attributeList->addList(addList);
	site->filter(attributeList);
	TRY(checkOrphanInList(attributeList), NULL);

	int numKeyFlds = keyAttrs->cardinality();
	int numAddFlds = additionalAttrs->cardinality();

	String option = "execute";
	TRY(site->typify(option), 0);
	LOG(logFile << "Typified sites\n");
	LOG(logFile << site->getName());
	LOG(site->display(logFile));
	LOG(logFile << endl);

	LOG(logFile << "Enumeration:\n";)
	TRY(site->enumerate(), 0);
	LOG(site->display(logFile, DETAIL);)
	LOG(logFile << endl;)

	int numFlds = site->getNumFlds();
	assert(numFlds == numKeyFlds + numAddFlds);
	String* types = site->getTypeIDs();
	String rtreeSchema;
	for(int i = 0; i < numKeyFlds; i++){
		rtreeSchema += rTreeEncode(types[i]);
	}

	TRY(int recIDSize = packSize(&(types[numKeyFlds]), numAddFlds), NULL);
	if(!standAlone) {
		recIDSize += sizeof(Offset);
	}
	int points = 1; // set to 0 for rectangles
	ostrstream line1;
	line1 << numKeyFlds << " " << recIDSize << " " << points << " ";
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
	Offset offset;
	if(!standAlone){
		TRY(offset = site->getOffset(), NULL);
		cout << "offset = " << offset << endl;
	}
     tup = site->getNext();
	char* flatTup = NULL;
	if(tup){ // make sure this is not empty
		TRY(fixedSize = packSize(types, numFlds), NULL);
		flatTup = new char[fixedSize];
		marshal(tup, flatTup, types, numFlds);
		ind.write(flatTup, fixedSize);
		if(!standAlone){
			ind.write((char*) &offset, sizeof(Offset));
			offset = site->getOffset();
			cout << "offset = " << offset << endl;
		}
		while((tup = site->getNext())){
			tupSize = packSize(tup, types, numFlds);
			if(tupSize != fixedSize){
				assert(0);
			}
			marshal(tup, flatTup, types, numFlds);
			ind.write(flatTup, fixedSize);
			if(!standAlone){
				ind.write((char*) &offset, sizeof(Offset));
				offset = site->getOffset();
				cout << "offset = " << offset << endl;
			}
		}
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

	String* keyFlds = new String[numKeyFlds];
	String* addFlds = new String[numAddFlds];
	keyAttrs->rewind();
	for(int i = 0; !keyAttrs->atEnd(); i++, keyAttrs->step()){
		assert(i < numKeyFlds);
		keyFlds[i] = *keyAttrs->get();
	}
	additionalAttrs->rewind();
	for(int i = 0; !additionalAttrs->atEnd(); i++, additionalAttrs->step()){
		assert(i < numAddFlds);
		addFlds[i] = *additionalAttrs->get();
	}
	TypeID* keyTypes = new TypeID[numKeyFlds];
	TypeID* addTypes = new TypeID[numAddFlds];
	for(int i = 0; i < numKeyFlds; i++){
		keyTypes[i] = types[i];
	}
	for(int i = 0; i < numAddFlds; i++){
		addTypes[i] = types[numKeyFlds + i];
	}

	Tuple* tuple = new Tuple[3];
	tuple[0] = new IString(tablename.chars());
	tuple[1] = new IString(indexName);
	tuple[2] = new IndexDesc(numKeyFlds, keyFlds, numAddFlds, addFlds,
			!standAlone, root1.pid, keyTypes, addTypes);	
	TRY(insert(".sysind", tuple), NULL);
	delete tuple;
	delete catalog;

#endif
	return new Site();
}
