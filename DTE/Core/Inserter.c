#include "url.h"
#include "Inserter.h"
#include "StandardRead.h"
#include "MemoryMgr.h"
#include "Sort.h"

void Inserter::open(const ISchema& schema, string urlString, int mode)
{ // throws
	TRY(URL* url = new URL(urlString), NVOID );
	numFlds = schema.getNumFlds();
	const TypeIDList& types = schema.getTypeIDs();
	TRY(writePtrs = getWritePtrs(types), NVOID );
	TRY(out = url->getOutputStream(mode), NVOID );
	delete url;
}

void Modifier::replace
	(const string* key, const Type* object, const string* key2)
{
	int objIndex = (key2 == NULL ? 1 : 2);
	ifstream* ins = new ifstream(fileName.c_str());
	if(!ins->good()){
		string msg = "Could not open file: " + fileName;
		THROW(new Exception(msg), NVOID);
		// throw Exception(msg);
	}
	assert(ins && ins->good());
	int numFlds = schema.getNumFlds();
	const TypeIDList& types = schema.getTypeIDs();
	StandReadExec* iter = new StandReadExec(types, ins);
     iter->initialize();
	const Tuple* tmpTuple;
	List<Tuple*> tupleList;
	TupleLoader tupleLoader;

	TypeID* typeIDs = makeArray(types);
	TRY(tupleLoader.open(numFlds, typeIDs), NVOID);
	delete [] typeIDs;

	TRY(tmpTuple = iter->getNext(), NVOID);

	bool isReplaced = false;
	
	Tuple* copy;
	while(tmpTuple){
		if(predicate(key, key2, tmpTuple)){
			ConstTuple* localTup = new ConstTuple[numFlds];
			for(int i = 0; i < numFlds; i++){
				localTup[i] = tmpTuple[i];
			}
			localTup[objIndex] = object;
			copy = tupleLoader.insert(localTup);
			isReplaced = true;
		}
		else{
			copy = tupleLoader.insert(tmpTuple);
		}
		tupleList.append(copy);
		TRY(tmpTuple = iter->getNext(), NVOID );
	}
	delete iter;
	if(!isReplaced){
		ConstTuple* localTup = new ConstTuple[numFlds];
		localTup[0] = IString::getTypePtr(key->c_str());
		if(key2){
			localTup[1] = IString::getTypePtr(key2->c_str());
		}
		localTup[objIndex] = object;
		copy = tupleLoader.insert(localTup);
		tupleList.append(copy);
		delete [] localTup;
	}
	Inserter newFile;
	TRY(newFile.open(schema, fileName, ios::out), NVOID );
	for(tupleList.rewind(); !tupleList.atEnd(); tupleList.step()){
		Tuple* tuple = tupleList.get();
		newFile.insert(tuple);
	}
}

UniqueInserter::UniqueInserter(const ISchema& schema, const string& urlstring, 
          int mode)
	: finalFile(urlstring), schema(schema), mode(mode)
{
	tmpFile = tmpnam(NULL);
	open(schema, tmpFile, ios::out);	
}

void UniqueInserter::close()
{
	Inserter::close();

	// do duplicate elimination here

	//cerr << tmpFile << endl;

	int numFlds = schema.getNumFlds();
	ifstream* istr = new ifstream(tmpFile.c_str());
	assert(*istr);

	int numSortFlds = numFlds;
	int* sortFlds = new int[numFlds];
	for(int i = 0; i < numFlds; i++){
		sortFlds[i] = i;
	}

	const TypeIDList& types = schema.getTypeIDs();
	StandReadExec* inp = new StandReadExec(types, istr);
	open(schema, finalFile, mode);


        FieldList* sort_fields = new FieldList;
	for(int i = 0; i < numSortFlds; i++){
          sort_fields->push_back(Field(types[sortFlds[i]], sortFlds[i]));
        }
        // SortExec used to delete these...
        delete [] sortFlds;

	TRY(Iterator* inp2 = new UniqueSortExec(inp, sort_fields, Ascending),
            NVOID);

	const Tuple* tup;
	for(tup = inp2->getFirst(); tup; tup = inp2->getNext()){
		insert(tup);
	}

	// do not delete istr

	delete inp2;
	remove(tmpFile.c_str());
}
