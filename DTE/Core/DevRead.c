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
  Revision 1.32  1998/07/07 21:50:15  wenger
  Fixed part of the SPARC/Solaris and Intel/Linux crash problem.

  Revision 1.31  1998/06/28 21:47:34  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.30  1998/06/04 23:06:46  donjerko
  Added DataReader.

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

Iterator* DevRead::createExec()
{
  TypeIDList types;
  for(int i = 0 ; i < numFlds ; i++) {
    types.push_back(typeIDs[i]);
  }
  DevReadExec* retVal = new DevReadExec(ud, types);
  ud = NULL;	// not the owner any more
  return retVal;
}


DevReadExec::DevReadExec(UniData* ud, const TypeIDList& types)
: ud(ud), types(types)
{
  numFlds = types.size();
  assert( numFlds == ud->schema()->NumFlatAttrs() + 1 ); // for recId
  unmarshalPtrs = new UnmarshalPtr[numFlds];
  destroyPtrs = new DestroyPtr[numFlds];
  tuple = new Type*[numFlds];
  offsets = new int[numFlds];

  AttrStk *stk = ud->schema()->GetFlatAttrs();
  offsets[0] = 0;               // not used, recid
  for(int i = 0; i < numFlds; i++) {
    // The 'if' in the next line prevents core dumps on some architectures
    // because otherwise we try to use a negative array subscript.
    // RKW 7/7/98.
    if (i > 0) offsets[i] = stk->ith(i - 1)->offset();
    unmarshalPtrs[i] = getUnmarshalPtr(types[i]);
    destroyPtrs[i] = getDestroyPtr(types[i]);
    assert(destroyPtrs[i]);
    size_t currentSz;
    tuple[i] = allocateSpace(types[i], currentSz);
  }

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


Offset DevReadExec::getOffset()
{
  return off;
}


const Tuple* DevReadExec::getNext(){
	UD_Status stat;
	if(!ud->isOk()){	// should not happen
		fprintf(stderr, "Error in UniData\n");
		return NULL;
	}
	stat = ud->getRec(buff,&off);
	if(stat == UD_EOF){
		fprintf(stderr, "End of file in UniData\n");
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

const Tuple* DevReadExec::getThis(Offset offset)
{
  this->recId = -999;
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


const TypeIDList& DevReadExec::getTypes()
{
  return types;
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

