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
  Revision 1.5  1998/11/03 18:08:49  okan
  Fixed Several Bugs of DataReader
   ----------------------------------------------------------------------

  Revision 1.3  1998/06/28 21:47:34  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.2  1998/06/04 23:06:45  donjerko
  Added DataReader.

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
#include "stdio.h"              // for sprintf


DataReadExec::DataReadExec(const string& schemaFile, const string& dataFile)
{
  ud = new DataReader(dataFile.c_str(), schemaFile.c_str());
  init();
}


DataReadExec::DataReadExec(DataReader* ud)
: ud(ud)
{
  init();
}


void DataReadExec::init()
{
  translateTypes();
  numFlds = types.size();

  unmarshalPtrs = new UnmarshalPtr[numFlds];
  destroyPtrs = new DestroyPtr[numFlds];
  tuple = new Type*[numFlds];
  offsets = new int[numFlds];

  offsets[0] = 0;                // not used, recid
  int count = 1;
  for(int i = 0; i < ud->myDRSchema->qAttr ; i++){
    if (ud->myDRSchema->tableAttr[i]->getFieldName() != NULL) {
      offsets[count++] = ud->myDRSchema->tableAttr[i]->offset;
    }
  }
  assert(numFlds == count);

  for(int i = 0; i < numFlds; i++) {
    unmarshalPtrs[i] = getUnmarshalPtr(types[i]);
    destroyPtrs[i] = getDestroyPtr(types[i]);
    assert(destroyPtrs[i]);
    size_t currentSz;
    tuple[i] = allocateSpace(types[i], currentSz);
  }

  buffSize = ud->myDRSchema->getRecSize();
  buff = (char*) new double[(buffSize / sizeof(double)) + 1];
  buff[buffSize - 1] = '\0';
  recId = 0;
}


DataReadExec::~DataReadExec()
{
  delete [] buff;
  delete ud;
  delete [] unmarshalPtrs;
  destroyTuple(tuple, numFlds, destroyPtrs);
  delete [] destroyPtrs;
  delete [] offsets;
}


void DataReadExec::translateTypes()
{
  char buf[100];

  types.push_back(INT_TP);      // for recid

  int fields = ud->myDRSchema->qAttr;
  for(int i = 0 ; i < fields ; i++) {
    if ((ud->myDRSchema->tableAttr[i])->getFieldName() == NULL) {
      //skipping SKIP attributes
      continue;
    }
    int fType = ud->myDRSchema->tableAttr[i]->getType();
    switch (fType) {
    case TYPE_INT:
      types.push_back(INT_TP);
      break;
    case TYPE_STRING:
      { int maxlen = ud->myDRSchema->tableAttr[i]->getMaxLen();
        if( maxlen <= 0 )
          maxlen = ud->myDRSchema->tableAttr[i]->getFieldLen();
        sprintf(buf, "string%d", maxlen);
        types.push_back(string(buf));
      }
      break;
    case TYPE_DOUBLE:
      types.push_back(DOUBLE_TP);
      break;
    case TYPE_DATE:
      types.push_back(DATE_TP);
      break;
    default:
      cout <<"This datareader type isn't handled yet: " << fType << endl;
      exit(1);
    }
  }
}


void DataReadExec::initialize()
{
  //kb: shouldn't this seek to beginning??
}


const Tuple* DataReadExec::getNext()
{
  bool stat;
  if(!ud->isOk()){	// should not happen
    return NULL;
  }
  stat = ud->getRecord(buff);
  if (!stat) {
    return NULL;
  }
  intCopy((Type*) recId, tuple[0]);
  for(int i = 1; i < numFlds; i++){
    unmarshalPtrs[i](&buff[offsets[i]], tuple[i]);
  }
  recId++;
  return tuple;
}


Offset DataReadExec::getOffset()
{
  //kb: this is never set!!!
  assert(!"dataread needs to be fixed!!");
  return off;
}


const Tuple* DataReadExec::getThis(Offset offset)
{
  this->recId = -999;
  bool stat;
  stat = ud->getRndRec(buff, offset.getOffset());
  if(!stat){
    return NULL;
  }
  intCopy((Type*) recId, tuple[0]);
  for(int i = 1; i < numFlds; i++){
    unmarshalPtrs[i](&buff[offsets[i]], tuple[i]);
  }
  return tuple;
}


const TypeIDList& DataReadExec::getTypes()
{
  return types;
}


//---------------------------------------------------------------------------

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

Iterator* DataRead::createExec()
{
  DataReadExec* retVal = new DataReadExec(ud);
  ud = NULL;	// not the owner any more
  return retVal;
}




void DataRead::Close()
{
  delete ud;
  delete [] typeIDs;
  typeIDs = NULL;
  delete [] attributeNames;
  attributeNames = NULL;
  delete order;
  order = NULL;
}


