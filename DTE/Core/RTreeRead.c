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

  Revision 1.6  1997/03/28 16:07:25  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "RTreeRead.h"

void RTreeIndex::initialize(){
	assert(rTreeQuery);
	assert(!initialized);
	initialized = true;

	int querySize = queryBoxSize();
	char* bounds = new char[querySize];
	int offset = 0;
	int numKeyFlds = getNumKeyFlds();
	int numAddFlds = getNumAddFlds();
	for(int j = 0; j < 2; j++){	// conver lower than upper bounds
		for(int i = 0; i < numKeyFlds; i++){
			assert(offset < querySize);
			offset += rTreeQuery[i].values[j]->toBinary(bounds + offset);
		}
	}
	page_id_t* Root = new page_id_t;
	Root->pid = indexDesc->getRootPg();
	cout << "Root->pid = " << Root->pid << endl;
	String typeEncS;
	for(int i = 0; i < numKeyFlds; i++){
		typeEncS += rTreeEncode(typeIDs[i]);
	}

	char* typeEnc = strdup(typeEncS.chars());
	queryBox = new gen_key_t(
		(char *)bounds, 	// binary representation of the search key
		numKeyFlds,			// dimensionality 
		typeEnc, 			// encoded types as char*
		0				// is point data (bool)
	);

	cout << "queryBox = ";
	queryBox->print();
	cout << "numKeyFlds = " << numKeyFlds << endl;
	cout << "typeEnc = " << typeEnc << endl;
	cursor = new gen_rt_cursor_t(*queryBox);
	assert(cursor);

	if (rtree_m.fetch_init (*Root, *cursor) != RCOK){
		printf("error in init\n");
		assert(0);
	}
	TRY(dataSize = packSize(&(typeIDs[numKeyFlds]), numAddFlds), );

	cout << "RTree scan initialized with:\n";
	display(cout);
}

int RTreeIndex::queryBoxSize(){
	int size = 0;
	int numKeyFlds = getNumKeyFlds();
	for(int j = 0; j < 2; j++){	// add lower than upper bounds
		for(int i = 0; i < numKeyFlds; i++){
			ConstantSelection* cs = rTreeQuery[i].values[j];
			assert(cs);
			size += cs->binarySize();
		}
	}
	return size;
}

const Tuple* RTreeIndex::getNext(){
	assert(initialized);
	gen_key_t ret_key;
	bool eof = false;
	int offsetLen;
	char dataContent[dataSize + sizeof(Offset) + 100];
		// This extra space is required because of some bug in RTree.

	assert(cursor);
	if (rtree_m.fetch(*cursor, ret_key, dataContent, offsetLen, eof) != RCOK){
		assert(0);
	}
	Offset offset;
	memcpy(&offset, (char*) dataContent + dataSize, sizeof(Offset));
	assert(retVal);
	if(!eof){
		// ret_key.print();
		int offs = 0;
		int numKeyFlds = getNumKeyFlds();
		int numAddFlds = getNumAddFlds();
		assert(numFlds == numKeyFlds + numAddFlds);
		for(int i = 0; i < numKeyFlds; i++){
			char* from = ((char*) ret_key.data) + offs;
			Type* adt = unmarshal(from, typeIDs[i]);
			offs += packSize(adt, typeIDs[i]);
			retVal[i] = adt;
		}
		offs = 0;
		for(int i = numKeyFlds; i < numFlds; i++){
			char* from = ((char*) dataContent) + offs;
			Type* adt = unmarshal(from, typeIDs[i]);
			offs += packSize(adt, typeIDs[i]);
			retVal[i] = adt;
		}
		#ifdef DEBUG
		cout << "Offset = " << offset << endl;
		#endif
		return retVal;
	}
	else{
		return NULL;
	}
}

Offset RTreeIndex::getNextOffset(){
	assert(initialized);
	assert(!indexDesc->isStandAlone());
	gen_key_t ret_key;
	bool eof = false;
	int offsetLen;
	char dataContent[dataSize + sizeof(Offset) + 100];
		// This extra space is required because of some bug in RTree.
	assert(cursor);
	if (rtree_m.fetch(*cursor, ret_key, dataContent, offsetLen, eof) != RCOK){
		assert(0);
	}
	Offset offset;
	memcpy(&offset, (char*) dataContent + dataSize, sizeof(Offset));
	if(!eof){
		cout << "Returning Offset = " << offset << endl;
		return offset;
	}
	else{
		return Offset();
	}
}

bool RTreeIndex::canUse(BaseSelection* predicate){	// Throws exception
	String attr;
	String opName;
	BaseSelection* constant;
	if(predicate->isIndexable(attr, opName, constant)){
		int numKeyFlds = getNumKeyFlds();
		for(int i = 0; i < numKeyFlds; i++){
			if(attributeNames[i] == attr){
				cout << "Updating rtree query on att " << i;
				cout << " with: " << opName << " ";
				constant->display(cout);
				cout << endl;
				TRY(rTreeQuery[i].update(opName, constant), false);
				return true;
			}
		}
	}
	return false;
}

/*
istream& RTreeIndex::read(istream& catalogStr){	// throws exception
	assert(catalogStr);
	catalogStr >> numFlds;
	stats = new Stats(numFlds);

	//	Needs to fix stats, something like this:

	void setStats(){
		double selectivity = listSelectivity(indexPreds);
		assert(baseIterator);
		Stats* baseStats = baseIterator->getStats();
		assert(baseStats);
		int cardinality = int(selectivity * baseStats->cardinality);
		int* sizes = baseStats->sizes;
		int nf = baseIterator->getNumFlds();
		stats = new Stats(nf, sizes, cardinality);
	}

	if(!catalogStr){
		String msg = "Number of index attributes expected";
		THROW(new Exception(msg), catalogStr);
	}
	attributeNames = new String[numFlds];
	typeIDs = new TypeID[numFlds];
	for(int i = 0; i < numFlds; i++){
		catalogStr >> typeIDs[i];
		catalogStr >> attributeNames[i];
		if(!catalogStr){
			String msg = 
				"Type and name of the index attribute expected";
			THROW(new Exception(msg), catalogStr);
		}
	}
//	catalogStr >> pageId;
	if(!catalogStr){
		String msg = "PageId expected in RTree index";
		THROW(new Exception(msg), catalogStr);
	}
	rTreeQuery = new RTreePred[numFlds];
	for(int i = 0; i < numFlds; i++){
		rTreeQuery[i].setTypeID(typeIDs[i]);
	}
	return catalogStr;
}
*/
