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
  Implementation of DevRead and related classes (used by the DTE to read
  data via Shaun's UniData classes).
 */

/*
  $Id$

  $Log$
  Revision 1.29  1998/06/03 21:16:05  wenger
  Temporarily backed out DataReader code in DTE so DEVise will link.

  Revision 1.26  1998/03/17 17:18:52  donjerko
  Added new namespace management through relation ids.

  Revision 1.25  1998/01/07 19:25:56  wenger
  Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
  server library into Devise); updated solaris, sun, linux, and hp
  dependencies.

  Revision 1.22.2.2  1997/12/29 22:12:13  wenger
  Got DEVise 1.4.7 to compile, link, and run on SGI.

  Revision 1.22.2.1  1997/12/17 17:29:18  wenger
  Fixed bugs 235 and 258 (core dump on Tables with long strings -- caused
  by insufficient buffer size in DTE); added headers to some DTE files
  and fixed some compile warnings.

 */

#include <strstream.h>
#include "types.h"
#include "UniData.h"

#if !defined(SGI)
#undef assert
#endif
#include <assert.h>

#include "DevRead.h"


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
       string retVal(tmp2);
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
		string msg = string("Cannot create Unidata table(") +
			dataFile + ", " + schemaFile + ")";
		THROW(new Exception(msg), );
		// throw Exception(msg);
	}
	numFlds = ud->schema()->NumFlatAttrs() + 1;	// for recId
	typeIDs = new TypeID[numFlds];
	attributeNames = new string[numFlds];
	AttrStk *stk = ud->schema()->GetFlatAttrs();
	typeIDs[0] = INT_TP;
	attributeNames[0] = string("recId"); 
	for(int i = 1; i < numFlds; i++){
		Attr *at = stk->ith(i - 1);
		typeIDs[i] = translateUDType(at);
		attributeNames[i] = string(at->flat_name()); 
	}
}

Iterator* DevRead::createExec(){
	UnmarshalPtr* unmarshalPtrs = new UnmarshalPtr[numFlds];
	DestroyPtr* destroyPtrs = new DestroyPtr[numFlds];
	size_t* currentSz = new size_t[numFlds];
	Type** tuple = new Type*[numFlds];
	int* offsets = new int[numFlds];
	AttrStk *stk = ud->schema()->GetFlatAttrs();
	for(int i = 1; i < numFlds; i++){
		Attr *at = stk->ith(i - 1);
		offsets[i] = at->offset();
	}
	for(int i = 0; i < numFlds; i++){
		unmarshalPtrs[i] = getUnmarshalPtr(typeIDs[i]);
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		assert(destroyPtrs[i]);
		tuple[i] = allocateSpace(typeIDs[i], currentSz[i]);
	}
	DevReadExec* retVal = new DevReadExec(
		ud, unmarshalPtrs, destroyPtrs, tuple, offsets, numFlds);
	ud = NULL;	// not the owner any more
	return retVal;
}

DevReadExec::DevReadExec(UniData* ud, UnmarshalPtr* unmarshalPtrs,
	DestroyPtr* destroyPtrs,
	Type** tuple, int* offsets, int numFlds) :
	ud(ud), unmarshalPtrs(unmarshalPtrs),
	destroyPtrs(destroyPtrs), tuple(tuple),
	offsets(offsets), numFlds(numFlds) {

	buffSize = ud->recSze();
	buff = (char*) new double[(buffSize / sizeof(double)) + 1];
	buff[buffSize - 1] = '\0';
	recId = 0;
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

const Tuple* DevReadExec::getThis(Offset offset, RecId recId){
	this->recId = recId;
	UD_Status stat;
	if(!ud->isOk()){	// should not happen
		return NULL;
	}
	stat = ud->getRndRec(buff, offset.getOffset());
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

	delete ud;
	delete [] typeIDs;
	typeIDs = NULL;
	delete [] attributeNames;
	attributeNames = NULL;
	delete order;
	order = NULL;
}

