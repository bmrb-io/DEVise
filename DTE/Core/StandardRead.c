/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.13  1997/08/12 19:58:42  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.12  1997/06/30 23:05:03  donjerko
  CVS:

  Revision 1.11  1997/06/21 22:48:04  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.10  1997/06/16 16:04:45  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.9  1997/04/10 21:50:25  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.8  1997/03/23 23:45:21  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.7  1997/02/18 18:06:02  donjerko
  Added skeleton files for sorting.

  Revision 1.6  1997/02/03 04:11:32  donjerko
  Catalog management moved to DTE

  Revision 1.5  1996/12/24 21:00:52  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.4  1996/12/21 22:21:46  donjerko
  Added hierarchical namespace.

  Revision 1.3  1996/12/16 11:13:07  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.2  1996/12/05 16:06:00  wenger
  Added standard Devise file headers.

 */

#include <strstream.h>
#include <string.h>
#include "StandardRead.h"
#include "exception.h"
#include "assert.h"
#include "url.h"
#include "ExecOp.h"

StandReadExec::StandReadExec(const ISchema& schema, istream* in) :
	in(in)
{
	numFlds = schema.getNumFlds();
	const TypeID* typeIDs = schema.getTypeIDs();
	readPtrs = new ReadPtr[numFlds];
	destroyPtrs = new DestroyPtr[numFlds];
	currentSz = new size_t[numFlds];
	tuple = new Tuple[numFlds];
	for(int i = 0; i < numFlds; i++){
		readPtrs[i] = getReadPtr(typeIDs[i]);
		destroyPtrs[i] = getDestroyPtr(typeIDs[i]);
		assert(destroyPtrs[i]);
		tuple[i] = allocateSpace(typeIDs[i], currentSz[i]);
	}
}

void StandardRead::open(istream* in, int numFlds, const TypeID* typeIDs){
	this->numFlds = numFlds;
	this->typeIDs = new TypeID[numFlds];
	assert(in && in->good());
	this->in = in;
	for(int i = 0; i < numFlds; i++){
		this->typeIDs[i] = typeIDs[i];
	}
}

void StandardRead::open(const ISchema& schema, istream* in){
	this->numFlds = schema.getNumFlds();
	this->typeIDs = new TypeID[numFlds];
	this->attributeNames = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		typeIDs[i] = schema.getTypeIDs()[i];
		attributeNames[i] = schema.getAttributeNames()[i];
	}
	assert(in && in->good());
	this->in = in;
}

Iterator* RidAdder::createExec(){
	TRY(Iterator* inpIter = input->createExec(), NULL);
	return new RidAdderExec(inpIter, numFlds);
}

void NCDCRead::open(istream* in){	// Throws exception
     char buff[100];
     ostrstream err;
     while(in->good()){
          in->read(buff, 100);
          err.write(buff, in->gcount());
     }
     err << ends;
     char* response = err.str();
     char* tmp = response;

     // find the third occurence of "http:"

     assert(tmp);
     for(int i = 0; i < 3; i++){
          tmp = tmp + 1;
          tmp = strstr(tmp, "http:");
          if(!tmp){
               string msg = "Unexpected response from the NCDC server:\n" +
                    string(response);
               THROW(new Exception(msg), );
          }
     }
     int j = 0;
     while(tmp[j] != '"' && tmp[j]){
          j++;
     }
     assert(tmp[j]);
     tmp[j] = '\0';
     string tableUrlStr = string(tmp);
     delete response;
     URL* tableUrl = new URL(tableUrlStr);
	istream* tablein = NULL;
     TRY(tablein = tableUrl->getInputStream(), );
	delete tableUrl;

     // ignore first 8 lines

     for(int k = 0; k < 8; k++){
          assert(*tablein);
          tablein->getline(buff, 100);
     }

     numFlds = 2;
	typeIDs = new string[numFlds];
     typeIDs[0] = "int";
     typeIDs[1] = "int";
	attributeNames = new string[numFlds];
	attributeNames[0] = "day";
	attributeNames[1] = "temp";
	stats = new Stats(numFlds);
	in = tablein;
}
