#include "url.h"
#include "Inserter.h"
#include "StandardRead.h"

void Inserter::open(const ISchema& schema, string urlString, int mode)
{ // throws
	TRY(URL* url = new URL(urlString), NVOID );
	numFlds = schema.getNumFlds();
	const TypeID* types = schema.getTypeIDs();
	TRY(writePtrs = newWritePtrs(types, numFlds), NVOID );
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
		THROW(new Exception(msg), );
	}
	assert(ins && ins->good());
	StandReadExec* iter = new StandReadExec(schema, ins);
     iter->initialize();
	int numFlds = schema.getNumFlds();
	const Tuple* tmpTuple;
	List<Tuple*> tupleList;
	TupleLoader tupleLoader;
	TRY(tupleLoader.open(numFlds, schema.getTypeIDs()), NVOID);
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
		ConstTuple localTup[numFlds];
		localTup[0] = IString::getTypePtr(key->c_str());
		if(key2){
			localTup[1] = IString::getTypePtr(key2->c_str());
		}
		localTup[objIndex] = object;
		copy = tupleLoader.insert(localTup);
		tupleList.append(copy);
	}
	Inserter newFile;
	TRY(newFile.open(schema, fileName, ios::out), NVOID );
	for(tupleList.rewind(); !tupleList.atEnd(); tupleList.step()){
		Tuple* tuple = tupleList.get();
		newFile.insert(tuple);
	}
}
