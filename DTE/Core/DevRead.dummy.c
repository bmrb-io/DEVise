#include "sysdep.h"
//#include <strstream.h>   erased for sysdep.h

#undef assert
#include <assert.h>

#include "types.h"
#include "DevRead.h"


TypeID translateUDType(Attr* at){
   return "unknown";
}

void DevRead::Open(char* schemaFile, char* dataFile){ // throws
}

Iterator* DevRead::createExec(){
	return NULL;
}

DevReadExec::~DevReadExec(){
}

const Tuple* DevReadExec::getNext(){
	return NULL;
}

const Tuple* DevReadExec::getThis(Offset offset, RecId recId){
	return NULL;
}

void DevRead::Close(){
}
