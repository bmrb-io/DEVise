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
  Revision 1.11  1997/08/10 20:30:55  donjerko
  Fixed the NO_RTREE option.

  Revision 1.10  1997/07/22 15:00:53  donjerko
  *** empty log message ***

  Revision 1.9  1997/06/21 22:48:00  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.8  1997/06/16 16:04:43  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.6  1997/03/28 16:07:25  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#define DEBUG

#include "RTreeRead.h"
#include "RTree.h"

RTreeReadExec::RTreeReadExec(
	genrtree_m* rtree_m, gen_rt_cursor_t* cursor, int dataSize,
	int numKeyFlds, int numAddFlds, Tuple* tuple,
	UnmarshalPtr* unmarshalPtrs, int* rtreeFldLens, int ridPosition) :
	rtree_m(rtree_m), cursor(cursor), dataSize(dataSize),
	numKeyFlds(numKeyFlds), numAddFlds(numAddFlds),
	tuple(tuple), unmarshalPtrs(unmarshalPtrs), 
	rtreeFldLens(rtreeFldLens){
	
	ret_key = new gen_key_t;
	dataContent = new char[dataSize + sizeof(Offset) + 100];
		// This extra space is required because of some bug in RTree.
	
	assert(ridPosition >= 0 && ridPosition < numKeyFlds + numAddFlds);

	ridOffset = 0;
	if(ridPosition < numKeyFlds){
		ridInKey = true;
		for(int i = 0; i < ridPosition; i++){
			ridOffset += rtreeFldLens[i];
		}
	}
	else{
		ridInKey = false;
		for(int i = numKeyFlds; i < ridPosition; i++){
			ridOffset += rtreeFldLens[i];
		}
	}
}

RTreeReadExec::~RTreeReadExec(){
	delete ret_key;
	delete cursor;
	delete [] tuple;
	delete [] unmarshalPtrs;
	delete [] rtreeFldLens;
	delete [] dataContent;
}

RTreeIndex::~RTreeIndex(){
	delete queryBox;
}

Iterator* RTreeIndex::createExec(){
	assert(rTreeQuery);
	int numKeyFlds = getNumKeyFlds();
	int numAddFlds = getNumAddFlds();

	int querySize = queryBoxSize();
	char* bounds = new char[querySize];
	int offset = 0;
	for(int j = 0; j < 2; j++){	// conver lower than upper bounds
		for(int i = 0; i < numKeyFlds; i++){
			assert(offset < querySize);
			offset += rTreeQuery[i].values[j]->toBinary(bounds + offset);
		}
	}
	page_id_t* Root = new page_id_t;
	Root->pid = indexDesc->getRootPg();
//	cerr << "Root->pid = " << Root->pid << endl;
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

#ifdef DEBUG
	cout << "queryBox = ";
	queryBox->print();
	cout << "numKeyFlds = " << numKeyFlds << endl;
	cout << "typeEnc = " << typeEnc << endl;
#endif

	gen_rt_cursor_t* cursor = new gen_rt_cursor_t(*queryBox);
	assert(cursor);

	genrtree_m* rtree_m = new genrtree_m;
	assert(rtree_m);
	if (rtree_m->fetch_init (*Root, *cursor) != RCOK){
		printf("error in init\n");
		assert(0);
	}
	int dataSize;
	TRY(dataSize = packSize(&(typeIDs[numKeyFlds]), numAddFlds), NULL);

#ifdef DEBUG
	cout << "RTree scan initialized with:\n";
	display(cout);
#endif

	Tuple* tuple = new Tuple[numFlds];
	UnmarshalPtr* unmarshalPtrs = new UnmarshalPtr[numFlds];
	int* rtreeFldLens = new int[numFlds];
	for(int i = 0; i < numFlds; i++){
		tuple[i] = allocateSpace(typeIDs[i]);
		rtreeFldLens[i] = packSize(typeIDs[i]);
		unmarshalPtrs[i] = getUnmarshalPtr(typeIDs[i]);
	}
	int ridIndex;
	for(ridIndex = 0; ridIndex < numFlds; ridIndex++){
		if(attributeNames[ridIndex] == String("recId")){
			break;
		}
	}
	if(ridIndex >= numFlds){
		cerr << "Index must contain recId field\n";
		exit(1);
	}
	return new RTreeReadExec(rtree_m, cursor, dataSize, numKeyFlds, 
		numAddFlds, tuple, unmarshalPtrs, rtreeFldLens, ridIndex);
}

int RTreeIndex::queryBoxSize(){
	int numKeyFlds = getNumKeyFlds();
	int size = 0;
	for(int j = 0; j < 2; j++){	// add lower than upper bounds
		for(int i = 0; i < numKeyFlds; i++){
			ConstantSelection* cs = rTreeQuery[i].values[j];
			assert(cs);
			size += cs->binarySize();
		}
	}
	return size;
}

const Tuple* RTreeReadExec::getNext(){
	bool eof = false;
	int offsetLen;

	assert(cursor);
	assert(ret_key);
	if (rtree_m->fetch(*cursor, *ret_key, dataContent, offsetLen, eof) != RCOK){
		assert(0);
	}
	Offset offset;
	memcpy(&offset, (char*) dataContent + dataSize, sizeof(Offset));
	assert(tuple);
	if(!eof){
		// ret_key->print();
		int offs = 0;
		int numFlds = numKeyFlds + numAddFlds;
		for(int i = 0; i < numKeyFlds; i++){
			char* from = ((char*) ret_key->data) + offs;
			unmarshalPtrs[i](from, tuple[i]);
			offs += rtreeFldLens[i];
		}
		offs = 0;
		for(int i = numKeyFlds; i < numFlds; i++){
			char* from = ((char*) dataContent) + offs;
			unmarshalPtrs[i](from, tuple[i]);
			offs += rtreeFldLens[i];
		}
		#ifdef DEBUG
		cout << "Offset = " << offset << endl;
		#endif
		return tuple;
	}
	else{
		return NULL;
	}
}

Offset RTreeReadExec::getNextOffset(){
	bool eof = false;
	int offsetLen;

	assert(cursor);
	assert(ret_key);
	if (rtree_m->fetch(*cursor, *ret_key, dataContent, offsetLen, eof) != RCOK){
		assert(0);
	}
	Offset offset;
	memcpy(&offset, (char*) dataContent + dataSize, sizeof(Offset));
	if(!eof){
		// cout << "Returning Offset = " << offset << endl;
		return offset;
	}
	else{
		return Offset();
	}
}

RecId RTreeReadExec::getRecId(){
	int recId;
	char* from = NULL;
	if(ridInKey){
		from = ((char*) ret_key->data) + ridOffset;
	}
	else{
		from = ((char*) dataContent) + ridOffset;
	}
	memcpy(&recId, from, sizeof(int));
	return recId;
}

bool RTreeIndex::canUse(BaseSelection* predicate){	// Throws exception
	String attr;
	String opName;
	BaseSelection* constant;
	if(predicate->isIndexable(attr, opName, constant)){
		int numKeyFlds = getNumKeyFlds();
		for(int i = 0; i < numKeyFlds; i++){
			if(attributeNames[i] == attr){
#ifdef DEBUG
				cout << "Updating rtree query on att " << i;
				cout << " with: " << opName << " ";
				constant->display(cout);
				cout << endl;
#endif
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
		assert(basePlanOp);
		Stats* baseStats = basePlanOp->getStats();
		assert(baseStats);
		int cardinality = int(selectivity * baseStats->cardinality);
		int* sizes = baseStats->sizes;
		int nf = basePlanOp->getNumFlds();
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
