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
  Revision 1.7  1999/01/20 22:46:03  beyer
  Major changes to the DTE.
  * Added a new type system.
  * Rewrote expression evaluation and parsing
  * And many other changes...

  Revision 1.6  1999/01/18 22:34:05  wenger
  Considerable changes to the DataReader:  reading is now per-field rather
  than per-character (except for dates); the "extractor" functions now do
  all the work, and the "value" functions have been eliminated; return values
  are more clear, and behaviour in "boundary conditions" is better-defined;
  fixed a number of bugs in the course of making these changes.  (The
  DataReader could still use some more cleanup.)

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
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteDateAdt.h"


DataReadExec::DataReadExec(const string& schemaFile, const string& dataFile)
{
  dr = new DataReader(dataFile.c_str(), schemaFile.c_str());
  init();
}


DataReadExec::DataReadExec(DataReader* dr)
: dr(dr)
{
  init();
}


void DataReadExec::init()
{
  assert(dr);
  assert( dr->isOk() );
  ISchema schema;
  translateSchema(dr, schema);
  resultAdt = schema.getAdt();
  numFlds = resultAdt.getNumFields();

  tuple.resize(numFlds);

  buffSize = dr->myDRSchema->getRecSize();
  // ensure double alignment
  buff = (char*) new double[(buffSize+sizeof(double)-1) / sizeof(double)];

  int count = 0;
  for(int i = 0; i < dr->myDRSchema->qAttr ; i++){
    if (dr->myDRSchema->tableAttr[i]->getFieldName() != NULL) {
      int offset = dr->myDRSchema->tableAttr[i]->offset;
      tuple[count++] = buff + offset;
    }
  }
  assert(numFlds == count);
}


DataReadExec::~DataReadExec()
{
  delete [] (double*)buff;
  delete dr;
}


void DataReadExec::translateSchema(DataReader* dr, ISchema& schema)
{
  int fields = dr->myDRSchema->qAttr;
  for(int i = 0 ; i < fields ; i++) {
    if ((dr->myDRSchema->tableAttr[i])->getFieldName() == NULL) {
      //skipping SKIP attributes
      continue;
    }
    int fType = dr->myDRSchema->tableAttr[i]->getType();
    string attrName = dr->myDRSchema->tableAttr[i]->getFieldName();
    switch (fType) {
    case TYPE_INT:
      schema.push_back(DteIntAdt(), attrName);
      break;
    case TYPE_STRING:
      { int maxlen = dr->myDRSchema->tableAttr[i]->getMaxLen();
        if( maxlen <= 0 )
          maxlen = dr->myDRSchema->tableAttr[i]->getFieldLen();
        schema.push_back(DteStringAdt(maxlen), attrName);
      }
      break;
    case TYPE_DOUBLE:
      schema.push_back(DteDoubleAdt(), attrName);
      break;
    case TYPE_DATE:
      schema.push_back(DteDateAdt(), attrName);
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
  while( !dr->isEof() ) {
    if( dr->getRecord(buff) ) {
      return tuple;
    }
  }
  return NULL;
}


Offset DataReadExec::getOffset()
{
  //kb: this is never set!!!
  assert(!"datareader needs to be fixed!! - it doesn't return offsets");
  return off;
}


const Tuple* DataReadExec::getThis(Offset offset)
{
  if( !dr->isOk() ) {	// should not happen
    return NULL;
  }
  if( dr->getRndRec(buff, offset.getOffset()) ) {
    return tuple;
  }
  return NULL;
}


//---------------------------------------------------------------------------

#if 0
void DataRead::translateUDInfo() {
	numFlds = dr->myDRSchema->tAttr;
	int temNum = dr->myDRSchema->qAttr;
	int i = 0;
	int fType;
	typeIDs = new TypeID[numFlds];
	attributeNames = new string[numFlds];

	for (int count = 0; count < temNum; count ++) {

		if ((dr->myDRSchema->tableAttr[count])->getFieldName() == NULL) { //skipping SKIP attributes
			continue;
		}
		i++;
		fType = dr->myDRSchema->tableAttr[count]->getType();
		switch (fType) {
			case TYPE_INT:
				typeIDs[i] = string("int");
				break;
			case TYPE_STRING: {
                          ostringstream tmp;
                          tmp << "string" << (dr->myDRSchema->tableAttr[count]->getMaxLen() > 0 ? dr->myDRSchema->tableAttr[count]->getMaxLen() : dr->myDRSchema->tableAttr[count]->getFieldLen()) << ends;
                          typeIDs[i] = tmp.str();
                          tmp.seekp(0);
                          break;
                        }
			case TYPE_DOUBLE:
				typeIDs[i] = string("double");
				break;
			case TYPE_DATE:
				typeIDs[i] = string("date");
				break;
			default:
				cout <<"This type isn't handled yet: " << dr->myDRSchema->tableAttr[count]->getType() << endl;
				typeIDs[i] = string("unknown");
		}
		attributeNames[i] = string(dr->myDRSchema->tableAttr[count]->getFieldName());
	}
}

DataRead::DataRead(const string& schemaFile, const string& dataFile) :
          dr(NULL), numFlds(0), typeIDs(NULL),
          attributeNames(NULL),
          order(NULL)
{
	dr = new DataReader(dataFile.c_str(), schemaFile.c_str());
	if(!dr || !dr->isOk()){
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
  DataReadExec* retVal = new DataReadExec(dr);
  dr = NULL;	// not the owner any more
  return retVal;
}




void DataRead::Close()
{
  delete dr;
  delete [] typeIDs;
  typeIDs = NULL;
  delete [] attributeNames;
  attributeNames = NULL;
  delete order;
  order = NULL;
}


#endif
