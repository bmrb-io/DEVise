#include "url.h"
#include "Inserter.h"
#include "StandardRead.h"

void Inserter::open(const ISchema& schema, string urlString, int mode)
{ // throws
	TRY(URL* url = new URL(urlString), );
	numFlds = schema.getNumFlds();
	const TypeID* types = schema.getTypeIDs();
	TRY(writePtrs = newWritePtrs(types, numFlds), );
	TRY(out = url->getOutputStream(mode), );
	delete url;
}

void Modifier::replace
	(const string* key, const Type* object, const string* key2 = NULL)
{
	int objIndex = (key2 == NULL ? 1 : 2);
	ifstream* ins = new ifstream(fileName.c_str());
	assert(ins && ins->good());
	StandReadExec* iter = new StandReadExec(schema, ins);
     iter->initialize();
	int numFlds = schema.getNumFlds();
	const Tuple* tmpTuple;
	List<Tuple*> tupleList;
	TupleLoader tupleLoader;
	TRY(tupleLoader.open(numFlds, schema.getTypeIDs()), );
	TRY(tmpTuple = iter->getNext(), );
	while(tmpTuple){
		Tuple* copy;
		if(predicate(key, key2, tmpTuple)){
			ConstTuple localTup[numFlds];
			for(int i = 0; i < numFlds; i++){
				localTup[i] = tmpTuple[i];
			}
			localTup[objIndex] = object;
			copy = tupleLoader.insert(localTup);
		}
		else{
			copy = tupleLoader.insert(tmpTuple);
		}
		tupleList.append(copy);
		TRY(tmpTuple = iter->getNext(), );
	}
	delete iter;
	Inserter newFile;
	TRY(newFile.open(schema, fileName, ios::out), );
	for(tupleList.rewind(); !tupleList.atEnd(); tupleList.step()){
		Tuple* tuple = tupleList.get();
		newFile.insert(tuple);
	}
}
