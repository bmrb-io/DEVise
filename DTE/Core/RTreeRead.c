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
  Revision 1.19  1998/03/17 17:18:59  donjerko
  Added new namespace management through relation ids.

  Revision 1.18  1997/11/12 23:17:31  donjerko
  Improved error checking.

  Revision 1.17  1997/11/07 16:51:53  donjerko
  *** empty log message ***

  Revision 1.16  1997/11/05 00:19:40  donjerko
  Separated typechecking from optimization.

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



RTreeReadExec::RTreeReadExec(const string& _filename, int _root_page,
                             BoundingBox* _bbox, const TypeIDList& _added_types)
: filename(_filename), root_page(_root_page), bbox(_bbox),
  added_types(_added_types)
{
  types = bbox->getTypes();
  types.append(added_types);

  // open rtree
  db_mgr = new db_mgr_jussi(filename.c_str(), cacheMgr);
  rtree = new typed_rtree_t(db_mgr);
  page_id_t root;
  memcpy(root.data, &root_page, sizeof(int));
  assert( rtree->open(root) == RC_OK );

  numKeyFlds = bbox->dims();
  numAddFlds = added_types.size();

  // create query
  char* bounds = bbox->rtreePack();
  string rtreeKeyTypes = bbox->rtreeTypes();
  queryBox = new typed_key_t(bounds, // binary rep. of the search key
                             numKeyFlds,     // dimensionality 
                             rtreeKeyTypes.c_str(),  // encoded types as char*
                             false);         // is point data
  delete [] bounds;
  cursor = new typed_cursor_t();

  // get ready for results
  dataSize = 0;
  for(int i = 0 ; i < numAddFlds ; i++) {
    int l = packSize(added_types[i]);
    data_off.push_back(dataSize);
    dataSize += l;
  }
  keyUnmarshalPtrs = getUnmarshalPtrs(bbox->getTypes());
  dataUnmarshalPtrs = getUnmarshalPtrs(added_types);
  tuple = allocateTuple(types);
}


RTreeReadExec::~RTreeReadExec()
{
  deleteTuple(tuple, types);
  delete bbox;
  delete cursor;
  delete queryBox;
  delete rtree;
  delete db_mgr;
}


void RTreeReadExec::initialize()
{
  // rtree->debug(stderr);

  // kb: cursor should probably be closed if this is the second call,
  // but I can't find a way to do that...
  if (rtree->fetch_init(*cursor, *queryBox) != RC_OK){
    printf("error in init\n");
    assert(0);
  }
}


const Tuple* RTreeReadExec::getNext()
{
  bool eof = false;
  size_t dataLen;
  typed_key_t* ret_key;
  void* data;
  assert(cursor);

  if (rtree->fetch(*cursor, ret_key, data, dataLen, eof) != RC_OK) {
    assert(0);
  }
  if(!eof) {
    for(int i = 0; i < numKeyFlds; i++) {
      void* ptr;
      char* typeEncoding;
      ret_key->min(i + 1, ptr, typeEncoding);
      keyUnmarshalPtrs[i]((char*)ptr, tuple[i]);
    }
    int t = numKeyFlds;
    for(int i = 0 ; i < numAddFlds; i++, t++) {
      dataUnmarshalPtrs[i]( (char*)data + data_off[i], tuple[t]);
    }
    return tuple;
  }
  else {
    return NULL;
  }
}



const TypeIDList& RTreeReadExec::getTypes()
{
  return types;
}


//---------------------------------------------------------------------------


RTreeIndex::~RTreeIndex(){
}


Iterator* RTreeIndex::createExec(){
  assert(rTreeQuery);
  int numKeyFlds = getNumKeyFlds();
  int numAddFlds = getNumAddFlds();
  
  BoundingBox* bbox = new BoundingBox;
  for(int i = 0; i < numKeyFlds; i++) {
    Type* lo = duplicateObject(typeIDs[i], rTreeQuery[i].values[0]->getValue());
    Type* hi = duplicateObject(typeIDs[i], rTreeQuery[i].values[1]->getValue());
    bbox->push_back(new Range(typeIDs[i], lo, hi));
  }
  
  TypeIDList added_types;
  for(int i = numKeyFlds ; i < numFlds ; i++) {
    added_types.push_back(typeIDs[i]);
  }
  
  const char* rtree_file = "./testRTree";  //kb: fix this later
  int root = indexDesc->getRootPg();
  
  return new RTreeReadExec(rtree_file, root, bbox, added_types);
}


#if 0
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

	Type** tuple = new Type*[numFlds];
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
#endif


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
		// throw Exception(msg);
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
			// throw Exception(msg);
		}
	}
//	catalogStr >> pageId;
	if(!catalogStr){
		string msg = "PageId expected in RTree index";
		THROW(new Exception(msg), catalogStr);
		// throw Exception(msg);
	}
	rTreeQuery = new RTreePred[numFlds];
	for(int i = 0; i < numFlds; i++){
		rTreeQuery[i].setTypeID(typeIDs[i]);
	}
	return catalogStr;
}
*/

RTreePred::RTreePred(string opName, ConstantSelection* constant){
	bounded[0] = bounded[1] = false;
	closed[0] = closed[1] = true;
	values[0] = values[1] = NULL;
	if(opName == "="){
		bounded[0] = bounded[1] = true;
		values[0] = values[1] = constant;
	}
	else if(opName == "<="){
		bounded[1] = true;
		values[1] = constant;
	}
	else if(opName == ">="){
		bounded[0] = true;
		values[0] = constant;
	}
	else if(opName == "<"){
		bounded[1] = true;
		closed[1] = false;
		values[1] = constant;
	}
	else if(opName == ">"){
		bounded[0] = true;
		closed[0] = false;
		values[0] = constant;
	}
	else {
		cout << "Operator \"" << opName; 
		cout << "\" should not be passed to this function\n";
		assert(0);
	}
}

void RTreePred::intersect(const RTreePred& pred){

	if(pred.bounded[0] && !bounded[0]){
		bounded[0] = true;
		closed[0] = pred.closed[0];
		values[0] = pred.values[0];
	}
	else if(pred.bounded[0] && bounded[0]){
		assert(pred.values[0]);
		assert(values[0]);
		if(!(*pred.values[0] < *values[0])){
			values[0] = pred.values[0];
			closed[0] = pred.closed[0];
		}
		else if(*pred.values[0] == *values[0]){
			if(!pred.closed[0]){
				closed[0] = false;
			}
		}
	}
	if(pred.bounded[1] && !bounded[1]){
		bounded[1] = true;
		closed[1] = pred.closed[1];
		values[1] = pred.values[1];
	}
	else if(pred.bounded[1] && bounded[1]){
		assert(pred.values[1]);
		assert(values[1]);
		if(*pred.values[1] < *values[1]){
			values[1] = pred.values[1];
			closed[1] = pred.closed[1];
		}
		else if(*pred.values[1] == *values[1]){
			if(!pred.closed[1]){
				closed[1] = false;
			}
		}
	}
}
