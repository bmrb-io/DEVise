#include "RTreeRead.h"

void RTreeIndex::initialize(){
	assert(rTreeQuery);
	assert(!initialized);
	initialized = true;

	int querySize = queryBoxSize();
	char* bounds = new char[querySize];
	int offset = 0;
	for(int j = 0; j < 2; j++){	// conver lower than upper bounds
		for(int i = 0; i < numFlds; i++){
			assert(offset < querySize);
			offset += rTreeQuery[i].values[j]->toBinary(bounds + offset);
		}
	}
	page_id_t* Root = new page_id_t;
	Root->pid = pageId;
	String typeEncS;
	for(int i = 0; i < numFlds; i++){
		typeEncS += rTreeEncode(typeIDs[i]);
	}

	char* typeEnc = strdup(typeEncS.chars());
	queryBox = new gen_key_t(
		(char *)bounds, 	// binary representation of the search key
		numFlds,			// dimensionality 
		typeEnc, 			// encoded types as char*
		0				// is point data (bool)
	);

	cout << "queryBox = ";
	queryBox->print();
	cursor = new gen_rt_cursor_t(*queryBox);
	assert(cursor);

	if (rtree_m.fetch_init (*Root, *cursor) != RCOK){
		printf("error in init\n");
		assert(0);
	}

	cout << "RTree scan initialized with:\n";
	display(cout);
}

int RTreeIndex::queryBoxSize(){
	int size = 0;
	for(int j = 0; j < 2; j++){	// add lower than upper bounds
		for(int i = 0; i < numFlds; i++){
			size += rTreeQuery[i].values[j]->binarySize();
		}
	}
	return size;
}

Tuple* RTreeIndex::getNext(){
	assert(initialized);
	gen_key_t ret_key;
	bool eof = false;
	Offset offset[10];
	int offsetLen = sizeof(Offset);
	assert(cursor);
	if (rtree_m.fetch(*cursor, ret_key, offset, offsetLen, eof) != RCOK){
		assert(0);
	}
	/*
	if(offsetLen != sizeof(Offset)){
		cout << offsetLen << " != " << sizeof(Offset) << endl;
	}
	*/
	// assert(offsetLen == sizeof(Offset));
	// cout << "Offset = " << offset << endl;
	if(!eof){
		Tuple* retVal = new Tuple[numFlds];
		int offs = 0;
		for(int i = 0; i < numFlds; i++){
			char* from = ((char*) ret_key.data) + offs;
			Type* adt = unmarshal(from, typeIDs[i]);
			offs += packSize(adt, typeIDs[i]);
			retVal[i] = adt;
		}
		return retVal;
	}
	else{
		return NULL;
	}
}

Offset RTreeIndex::getNextOffset(){
	assert(initialized);
	gen_key_t ret_key;
	bool eof = false;
	Offset offset[10];
	int offsetLen = sizeof(Offset);
	assert(cursor);
	if (rtree_m.fetch(*cursor, ret_key, offset, offsetLen, eof) != RCOK){
		assert(0);
	}
	// assert(offsetLen == sizeof(Offset));
	// cout << "Offset = " << offset << endl;
	if(!eof){
		return offset[0];
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
		for(int i = 0; i < numFlds; i++){
			if(attributeNames[i] == attr){
				cout << "Updating rtree query on att " << i;
				cout << "with: " << opName << " ";
				constant->display(cout);
				cout << endl;
				rTreeQuery[i].update(opName, constant);
				return true;
			}
		}
	}
	return false;
}
