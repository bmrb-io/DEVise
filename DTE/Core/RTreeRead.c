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
  Revision 1.15  1997/08/25 15:28:11  donjerko
  Added minmax table

  Revision 1.14  1997/08/21 21:04:25  donjerko
  Implemented view materialization

  Revision 1.13  1997/08/15 00:17:34  donjerko
  Completed the Iterator destructor code.

  Revision 1.12  1997/08/12 19:58:41  donjerko
  Moved StandardTable headers to catalog.

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

#define DTE_DEBUG

#include "RTreeRead.h"
#include "SBMInit.h"
#include "typed_rtree.h"
#include "dbJussi.h"

RTreeReadExec::RTreeReadExec(
	const IndexDesc& indexDesc, int dataSize,
	Tuple* tuple,
	UnmarshalPtr* unmarshalPtrs, int* rtreeFldLens, int ridPosition,
	typed_key_t* queryBox) :
	rtree(NULL), cursor(NULL), dataSize(dataSize),
	tuple(tuple), unmarshalPtrs(unmarshalPtrs), 
	rtreeFldLens(rtreeFldLens), queryBox(queryBox) {
	
	db_mgr = NULL;
	numKeyFlds = indexDesc.getNumKeyFlds();
	numAddFlds = indexDesc.getNumAddFlds();
	rootPgId = indexDesc.getRootPg();

	ret_key = new typed_key_t;
	dataContent = NULL;
	
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
	delete rtree;
	delete cursor;
	delete ret_key;
	delete queryBox;
	delete db_mgr;
	delete [] tuple;
	delete [] unmarshalPtrs;
	delete [] rtreeFldLens;
}

RTreeIndex::~RTreeIndex(){
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
	string typeEncS;
	for(int i = 0; i < numKeyFlds; i++){
		typeEncS += rTreeEncode(typeIDs[i]);
	}

	char* typeEnc = strdup(typeEncS.c_str());
	typed_key_t* queryBox = new typed_key_t(
		(char *)bounds, 	// binary representation of the search key
		numKeyFlds,			// dimensionality 
		typeEnc, 			// encoded types as char*
		0				// is point data (bool)
	);

#ifdef DTE_DEBUG
	cout << "queryBox = ";
	queryBox->debug();
	cout << "numKeyFlds = " << numKeyFlds << endl;
	cout << "typeEnc = " << typeEnc << endl;
#endif

	int dataSize;
	TRY(dataSize = packSize(&(typeIDs[numKeyFlds]), numAddFlds), NULL);

#ifdef DTE_DEBUG
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
		if(attributeNames[ridIndex] == string("recId")){
			break;
		}
	}
	if(ridIndex >= numFlds){
		cerr << "Index must contain recId field\n";
		exit(1);
	}
	return new RTreeReadExec(*indexDesc, dataSize,
		tuple, unmarshalPtrs, rtreeFldLens, ridIndex, queryBox);
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

void RTreeReadExec::initialize(){

	page_id_t root;
	memcpy(root.data, &rootPgId, sizeof(int));
	
     const char* fileToContainRTree = "./testRTree";  // fix this later

     db_mgr = new db_mgr_jussi(fileToContainRTree, cacheMgr);

	typed_rtree_t* rtree = new typed_rtree_t(db_mgr);

	rtree->open(root);

//	rtree->debug(stderr);

	cursor = new typed_cursor_t();

	if (rtree->fetch_init(*cursor, *queryBox) != RC_OK){
		printf("error in init\n");
		assert(0);
	}
}

const Tuple* RTreeReadExec::getNext(){
	bool eof = false;
	size_t dataLen;

	assert(cursor);
	assert(ret_key);
	void* dataVoidPtr;
	if (rtree->fetch(*cursor, ret_key, dataVoidPtr, dataLen, eof) != RC_OK){
		assert(0);
	}
	dataContent = (char*) dataVoidPtr;
	if(!eof){
		Offset offset;
		assert(dataSize + sizeof(Offset) <= dataLen);
		memcpy(&offset, (char*) dataContent + dataSize, sizeof(Offset));
		assert(tuple);
		// ret_key->print();
		int numFlds = numKeyFlds + numAddFlds;
		for(int i = 0; i < numKeyFlds; i++){
			void* from;
			char* typeEncoding;
			ret_key->min(i + 1, from, typeEncoding);
			unmarshalPtrs[i]((char*) from, tuple[i]);
		}
		int offs = 0;
		for(int i = numKeyFlds; i < numFlds; i++){
			char* from = ((char*) dataContent) + offs;
			unmarshalPtrs[i](from, tuple[i]);
			offs += rtreeFldLens[i];
		}
		#ifdef DTE_DEBUG
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
	size_t dataLen;

	assert(cursor);
	assert(ret_key);
	void* dataVoidPtr;
	if (rtree->fetch(*cursor, ret_key, dataVoidPtr, dataLen, eof) != RC_OK){
		assert(0);
	}
	dataContent = (char*) dataVoidPtr;
	if(!eof){
		Offset offset;
		memcpy(&offset, (char*) dataContent + dataSize, sizeof(Offset));
		// cout << "Returning Offset = " << offset << endl;
		return offset;
	}
	else{
		return Offset();
	}
}

RecId RTreeReadExec::getRecId(){
	int recId;
	void* from;
	char* typeEncoding;
	if(ridInKey){
		assert(0);	// ridOffset should be an index
		ret_key->min(ridOffset + 1, from, typeEncoding);
	}
	else{
		from = ((char*) dataContent) + ridOffset;
	}
	memcpy(&recId, (char*) from, sizeof(int));
	return recId;
}

bool RTreeIndex::canUse(BaseSelection* predicate){	// Throws exception
	string attr;
	string opName;
	BaseSelection* constant;
	if(predicate->isIndexable(attr, opName, constant)){
		int numKeyFlds = getNumKeyFlds();
		for(int i = 0; i < numKeyFlds; i++){
			if(attributeNames[i] == attr){
#ifdef DTE_DEBUG
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
		string msg = "Number of index attributes expected";
		THROW(new Exception(msg), catalogStr);
	}
	attributeNames = new string[numFlds];
	typeIDs = new TypeID[numFlds];
	for(int i = 0; i < numFlds; i++){
		catalogStr >> typeIDs[i];
		catalogStr >> attributeNames[i];
		if(!catalogStr){
			string msg = 
				"Type and name of the index attribute expected";
			THROW(new Exception(msg), catalogStr);
		}
	}
//	catalogStr >> pageId;
	if(!catalogStr){
		string msg = "PageId expected in RTree index";
		THROW(new Exception(msg), catalogStr);
	}
	rTreeQuery = new RTreePred[numFlds];
	for(int i = 0; i < numFlds; i++){
		rTreeQuery[i].setTypeID(typeIDs[i]);
	}
	return catalogStr;
}
*/
