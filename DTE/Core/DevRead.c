#include <strstream.h>
#include "types.h"
#include "DevRead.h"
#include "UniData.h"

#undef assert
#include <assert.h>

TypeID translateUDType(Attr* at){

   switch (at->type()) {

	case Int_Attr:
       return "int";	

	case Float_Attr:
       return "float";

	case Double_Attr:
       return "double";

	case String_Attr:{
       int size = at->size_of();
       ostrstream tmp;
       tmp << "string" << size << ends;
       char* tmp2 = tmp.str();
       String retVal(tmp2);
       delete [] tmp2;
       return retVal;
     } 

	case UnixTime_Attr:
       return "time_t";

	case DateTime_Attr:
       return "date";

	case Invalid_Attr:
	case UserDefined_Attr:
	  cout << "This type isn't handled yet: " << at->type() << endl;
	  break;
   }
   return "unknown";
}

void DevRead::Open(char* schemaFile, char* dataFile){ // throws
	ud = new UniData(dataFile, schemaFile);
	if(!ud || !ud->isOk()){
		String msg = String("Cannot create table(") +
			dataFile + ", " + schemaFile + ")";
		THROW(new Exception(msg), );
	}
	numFlds = ud->schema()->NumFlatAttrs() + 1;	// for recId
	typeIDs = new TypeID[numFlds];
	offsets = new int[numFlds];
	attributeNames = new String[numFlds];
	AttrStk *stk = ud->schema()->GetFlatAttrs();
	typeIDs[0] = INT_TP;
	attributeNames[0] = String("recId"); 
	for(int i = 1; i < numFlds; i++){
		Attr *at = stk->ith(i - 1);
		typeIDs[i] = translateUDType(at);
		attributeNames[i] = String(at->flat_name()); 
		offsets[i] = at->offset();
	}
}

Iterator* DevRead::createExec(){
	UnmarshalPtr* unmarshalPtrs = new UnmarshalPtr[numFlds];
	DestroyPtr* destroyPtrs = new DestroyPtr[numFlds];
	size_t* currentSz = new size_t[numFlds];
	Tuple* tuple = new Tuple[numFlds];
	for(int i = 0; i < numFlds; i++){
		unmarshalPtrs[i] = getUnmarshalPtr(typeIDs[i]);
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		assert(destroyPtrs[i]);
		tuple[i] = allocateSpace(typeIDs[i], currentSz[i]);
	}
	return new DevReadExec(
		ud, unmarshalPtrs, destroyPtrs, tuple, offsets, numFlds);
}

DevReadExec::~DevReadExec(){
	delete [] buff;
	delete ud;
	delete [] unmarshalPtrs;
	destroyTuple(tuple, numFlds, destroyPtrs);
	delete [] destroyPtrs;
	delete [] offsets;
}

const Tuple* DevReadExec::getNext(){
	UD_Status stat;
	if(!ud->isOk()){	// should not happen
		return NULL;
	}
	stat = ud->getRec(buff,&off);
	if(stat == UD_EOF){
		return NULL;
	}
	assert(stat == UD_OK);
	intCopy((Type*) recId, tuple[0]);
	for(int i = 1; i < numFlds; i++){
		unmarshalPtrs[i](&buff[offsets[i]], tuple[i]);
	}
	recId++;
	return tuple;
}

void DevRead::Close(){

	// do not delete ud and offsets, they are passed to the Exec

	delete [] typeIDs;
	typeIDs = NULL;
	delete [] attributeNames;
	attributeNames = NULL;
	delete order;
	order = NULL;
}

