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
  Implementation of DataRead and related classes (used by the DTE to read
  data via DataReader classes).
 */

/*
  $Id$

  $Log$
  Revision 1.1  1998/06/03 22:09:49  okan
  *** empty log message ***

  Revision 1.28  1998/06/02 04:11:43  okan
  *** empty log message ***

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

#include "sysdep.h"
#include "types.h"
#include "DataReader.h"

#if !defined(SGI)
#undef assert
#endif
#include <assert.h>

#include "DataRead.h"


void DataRead::translateUDInfo() {
	numFlds = ud->myDRSchema->tAttr + 1; //for RECID
	int temNum = ud->myDRSchema->qAttr + 1;
	int i = 0;
	int fType;
	ostringstream tmp;
	typeIDs = new TypeID[numFlds];
	attributeNames = new string[numFlds];

	typeIDs[0] = INT_TP;
	attributeNames[0] = string("recId");

	for (int count = 1; count < temNum; count ++) {

		if ((ud->myDRSchema->tableAttr[count-1])->getFieldName() == NULL) { //skipping SKIP attributes
			continue;
		}
		i++;
			
		fType = ud->myDRSchema->tableAttr[count-1]->getType();
		switch (fType) {
			case TYPE_INT:
				typeIDs[i] = string("int");
				break;
			case TYPE_STRING:
				tmp << "string" << (ud->myDRSchema->tableAttr[count-1]->getMaxLen() > 0 ? ud->myDRSchema->tableAttr[count-1]->getMaxLen() : ud->myDRSchema->tableAttr[count-1]->getFieldLen()) << ends;
				typeIDs[i] = string(tmp.str());
				tmp.seekp(0);
				break;
			case TYPE_DOUBLE:
				typeIDs[i] = string("double");
				break;
			case TYPE_DATE:
				typeIDs[i] = string("date");
				break;
			default:
				cout <<"This type isn't handled yet: " << ud->myDRSchema->tableAttr[count-1]->getType() << endl;
				typeIDs[i] = string("unknown");
		}
		attributeNames[i] = string(ud->myDRSchema->tableAttr[count-1]->getFieldName());
	}
}

DataRead::DataRead(const string& schemaFile, const string& dataFile) :
          ud(NULL), numFlds(0), typeIDs(NULL),
          attributeNames(NULL),
          order(NULL)
{
	ud = new DataReader(dataFile.c_str(), schemaFile.c_str());
	if(!ud || !ud->isOk()){
		string msg = string("Cannot create DataReader table(") +
			dataFile + ", " + schemaFile + ")";
		CON_THROW(new Exception(msg));
		// throw Exception(msg);
	}
	translateUDInfo();
	CON_END:;
}

Iterator* DataRead::createExec(){
	int i;
	int count = 0;
	UnmarshalPtr* unmarshalPtrs = new UnmarshalPtr[numFlds];
	DestroyPtr* destroyPtrs = new DestroyPtr[numFlds];
	size_t* currentSz = new size_t[numFlds];
	Type** tuple = new Type*[numFlds];
	int* offsets = new int[numFlds];

	for (i = 1; i < (ud->myDRSchema->qAttr + 1) ; i++) {
		if (ud->myDRSchema->tableAttr[i-1]->getFieldName() != NULL) {
			count++;
			offsets[count] = ud->myDRSchema->tableAttr[i-1]->offset;
		}
	}

	for(i = 0; i < numFlds; i++){
		unmarshalPtrs[i] = getUnmarshalPtr(typeIDs[i]);
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		assert(destroyPtrs[i]);
		tuple[i] = allocateSpace(typeIDs[i], currentSz[i]);
	}
	DataReadExec* retVal = new DataReadExec(
		ud, unmarshalPtrs, destroyPtrs, tuple, offsets, numFlds);
	ud = NULL;	// not the owner any more
	return retVal;
}

DataReadExec::DataReadExec(DataReader* ud, UnmarshalPtr* unmarshalPtrs,
	DestroyPtr* destroyPtrs,
	Type** tuple, int* offsets, int numFlds) :
	ud(ud), unmarshalPtrs(unmarshalPtrs),
	destroyPtrs(destroyPtrs), tuple(tuple),
	offsets(offsets), numFlds(numFlds) {

	buffSize = ud->myDRSchema->getRecSize();
	buff = (char*) new double[(buffSize / sizeof(double)) + 1];
	buff[buffSize - 1] = '\0';
	recId = 0;
}

DataReadExec::~DataReadExec(){
	delete [] buff;
	delete ud;
	delete [] unmarshalPtrs;
	destroyTuple(tuple, numFlds, destroyPtrs);
	delete [] destroyPtrs;
	delete [] offsets;
}

const Tuple* DataReadExec::getNext(){
	Status stat;
	if(!ud->isOk()){	// should not happen
		return NULL;
	}
	buff[offsets[1]] = '\0';
	stat = ud->getRecord(buff);
	if ((stat == FOUNDEOF) && (buff[offsets[1]] == '\0')) {
		return NULL;
	}
	assert((stat == OK) || (stat == FOUNDEOL) || (stat == FOUNDEOF));
	intCopy((Type*) recId, tuple[0]);
	for(int i = 1; i < numFlds; i++){
		unmarshalPtrs[i](&buff[offsets[i]], tuple[i]);
	}
	recId++;
	return tuple;
}

const Tuple* DataReadExec::getThis(Offset offset, RecId recId){
	this->recId = recId;
	Status stat;
	if(!ud->isOk()){	// should not happen
		return NULL;
	}
	stat = ud->getRndRec(buff, offset.getOffset());
	if(stat == FOUNDEOF){
		return NULL;
	}
	assert(stat == OK);
	intCopy((Type*) recId, tuple[0]);
	for(int i = 1; i < numFlds; i++){
		unmarshalPtrs[i](&buff[offsets[i]], tuple[i]);
	}
	recId++;
	return tuple;
}

void DataRead::Close(){

	delete ud;
	delete [] typeIDs;
	typeIDs = NULL;
	delete [] attributeNames;
	attributeNames = NULL;
	delete order;
	order = NULL;
}

