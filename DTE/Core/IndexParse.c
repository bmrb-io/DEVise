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
  Revision 1.11  1997/06/21 22:47:58  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.10  1997/06/16 16:04:41  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.9  1997/04/18 20:46:16  donjerko
  Added function pointers to marshall types.

  Revision 1.7  1997/03/28 16:07:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

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
#include "Iterator.h"
#include "Aggregates.h"

#ifndef NO_RTREE
	#include "RTree.h"
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

	site->filter(NULL);

	int numKeyFlds = keyAttrs->cardinality();
	int numAddFlds = additionalAttrs->cardinality();

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

	String option = "execute";
	TRY(site->typify(option), 0);
	LOG(logFile << "Typified sites\n");
	LOG(logFile << site->getName());
	LOG(site->display(logFile));
	LOG(logFile << endl);

	int numTFlds = site->getNumFlds();
	const String* attrNms = site->getAttNamesOnly();

	int indirect[numTFlds];
	for(int i = 0; i < numKeyFlds; i++){
		int j;
		for(j = 0; j < numTFlds; j++){
			if(attrNms[j] == keyFlds[i]){
				indirect[i] = j;
				break;
			}
		}
		if(j == numTFlds){
			cerr << "Cannot find attribute: " << keyFlds[i] << endl;
		}
	}
	for(int i = 0; i < numAddFlds; i++){
		int j;
		for(j = 0; j < numTFlds; j++){
			if(attrNms[j] == addFlds[i]){
				indirect[i + numKeyFlds] = j;
				break;
			}
		}
		if(j == numTFlds){
			cerr << "Cannot find attribute: " << addFlds[i] << endl;
		}
	}

	int numFlds = numKeyFlds + numAddFlds;
	String* types = site->getTypeIDs();
	MarshalPtr marshalPtrs[numFlds];
	int sizes[numFlds];
	int fixedSize = 0; 
	for(int i = 0; i < numFlds; i++){
		marshalPtrs[i] = getMarshalPtr(types[indirect[i]]);
		sizes[i] = packSize(types[indirect[i]]);
		fixedSize += sizes[i];
	}
	String rtreeISchema;
	for(int i = 0; i < numKeyFlds; i++){
		rtreeISchema += rTreeEncode(types[indirect[i]]);
	}

	int recIDSize = 0;
	for(int i = 0; i < numAddFlds; i++){
		recIDSize += sizes[i + numKeyFlds];
	}
	if(!standAlone) {
		recIDSize += sizeof(Offset);
	}
	int points = 1; // set to 0 for rectangles
	ostrstream line1;
	line1 << numKeyFlds << " " << recIDSize << " " << points << " ";
	line1 << rtreeISchema;
	int fillSize  = (line1.pcount() + 1) % 8;	// allign on 8 byte boundary
	for(int i = 0; i < fillSize; i++){
		line1 << " ";
	}
	line1 << endl;
	String bulkfile = *indexName + ".bulk";
	ofstream ind(bulkfile);
	assert(ind);
	ind << line1.rdbuf();
	int tupSize;
	Offset offset;

	LOG(logFile << "Creating executable:\n";)
	TRY(Iterator* inpIter = site->createExec(), 0);

	inpIter->initialize();
	if(!standAlone){
		TRY(offset = inpIter->getOffset(), NULL);
		// cout << "offset = " << offset << endl;
	}
	const Tuple* tup = inpIter->getNext();

	MinAggregate mins[numTFlds];
	MaxAggregate maxs[numTFlds];

	for(int i = 0; i < numTFlds; i++){
		TRY(mins[i].typify(types[i]), NULL);
		TRY(maxs[i].typify(types[i]), NULL);
	}

	ExecMinMax* minExs[numTFlds];
	ExecMinMax* maxExs[numTFlds];

	for(int i = 0; i < numTFlds; i++){
		minExs[i] = mins[i].createExec();
		assert(minExs[i]);
		maxExs[i] = maxs[i].createExec();
		assert(maxExs[i]);
	}

	if(tup){ // make sure this is not empty

          for(int i = 0; i < numTFlds; i++){
			minExs[i]->initialize(tup[i]);
			maxExs[i]->initialize(tup[i]);
          }

		char flatTup[fixedSize];
		Type* indexTup[numFlds];
		for(int i = 0; i < numFlds; i++){
			indexTup[i] = tup[indirect[i]];
		}
		marshal(indexTup, flatTup, marshalPtrs, sizes, numFlds);
		ind.write(flatTup, fixedSize);
		if(!standAlone){
			ind.write((char*) &offset, sizeof(Offset));
			offset = inpIter->getOffset();
			// cout << "offset = " << offset << endl;
		}
		while((tup = inpIter->getNext())){

			for(int i = 0; i < numTFlds; i++){
				minExs[i]->update(tup[i]);
				maxExs[i]->update(tup[i]);
			}

			for(int i = 0; i < numFlds; i++){
				indexTup[i] = tup[indirect[i]];
			}
			marshal(indexTup, flatTup, marshalPtrs, sizes, numFlds);
			ind.write(flatTup, fixedSize);
			if(!standAlone){
				ind.write((char*) &offset, sizeof(Offset));
				offset = inpIter->getOffset();
				// cout << "offset = " << offset << endl;
			}
		}
	}
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
//	printf("Dump follows:\n");
//	rtree_m.olddraw(root1, stdout);
	// note, you MUST keep root page

	TypeID* keyTypes = new TypeID[numKeyFlds];
	TypeID* addTypes = new TypeID[numAddFlds];
	for(int i = 0; i < numKeyFlds; i++){
		keyTypes[i] = types[indirect[i]];
	}
	for(int i = 0; i < numAddFlds; i++){
		addTypes[i] = types[indirect[numKeyFlds + i]];
	}

	Tuple tuple[3];
	tuple[0] = (Type*) tablename.chars();
	tuple[1] = (Type*) indexName->chars();
	tuple[2] = new IndexDesc(numKeyFlds, keyFlds, numAddFlds, addFlds,
			!standAlone, root1.pid, keyTypes, addTypes);	
	TRY(insert(".sysind", tuple), NULL);
	delete catalog;

	Tuple tupleM[2];
	tupleM[0] = (Type*) tablename.chars();
//	tupleM[1] = (Type*) 

	for(int i = 0; i < numFlds; i++){
		delete minExs[i];
		delete maxExs[i];
	}
#endif
	return new Site();
}
