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
  Revision 1.34  1998/11/06 19:18:54  beyer
  added initialize() function

  Revision 1.33  1998/07/09 19:31:01  wenger
  Fixed bug 374 (Tables failing on SPARC/Solaris).

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

#include "types.h"
#include "UniData.h"

#if !defined(SGI)
#undef assert
#endif
#include <assert.h>

#include "DevRead.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteDateAdt.h"


void DevReadExec::translateSchema(UniData* ud, ISchema& schema)
{
  int numFlds = ud->schema()->NumFlatAttrs();
  AttrStk *stk = ud->schema()->GetFlatAttrs();
  for(int i = 0; i < numFlds; i++){
    Attr *at = stk->ith(i);
    string attrName(at->flat_name());
    switch(at->type()) {
    case Int_Attr:
      schema.push_back(DteIntAdt(), attrName);
      break;

    case Double_Attr:
      schema.push_back(DteDoubleAdt(), attrName);
      break;

    case String_Attr:
      schema.push_back(DteStringAdt(at->size_of()), attrName);
      break;

    case DateTime_Attr:
      schema.push_back(DteDateAdt(), attrName);
      break;

      //case Float_Attr:
      //case UnixTime_Attr:
      //case Invalid_Attr:
      //case UserDefined_Attr:
    default:
      cerr << "This type isn't handled yet: " << (int)at->type() << endl;
      assert(0);
    }
  }
}


DevReadExec::DevReadExec(UniData* ud)
: ud(ud)
{
  ISchema schema;
  translateSchema(ud, schema);
  resultAdt = schema.getAdt();

  int buffSize = ud->recSze();
  buff = (char*) new double[(buffSize / sizeof(double)) + 1];
  buff[buffSize - 1] = '\0';

  int numFlds = resultAdt.getNumFields();
  tuple.resize(numFlds);
  AttrStk *stk = ud->schema()->GetFlatAttrs();
  for(int i = 0; i < numFlds; i++) {
    int offset = stk->ith(i)->offset(); // better be aligned
    tuple[i] = buff + offset;
  }
}


DevReadExec::~DevReadExec(){
	delete [] buff;
	delete ud;
}


Offset DevReadExec::getOffset()
{
  return off;
}


void DevReadExec::initialize()
{
  ud->reset();
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
	return tuple;
}

const Tuple* DevReadExec::getThis(Offset offset)
{
  UD_Status stat;
  if(!ud->isOk()){	// should not happen
    return NULL;
  }
  stat = ud->getRndRec(buff, offset.getOffset());
  if(stat == UD_EOF){
    return NULL;
  }
  assert(stat == UD_OK);
  return tuple;
}



#if 0
void DevRead::Open(char* schemaFile, char* dataFile){ // throws
	ud = new UniData(dataFile, schemaFile);
	if(!ud || !ud->isOk()){
		string msg = string("Cannot create Unidata table(") +
			dataFile + ", " + schemaFile + ")";
		THROW(new Exception(msg), );
		// throw Exception(msg);
	}
        
	numFlds = ud->schema()->NumFlatAttrs();
	typeIDs = new TypeID[numFlds];
	attributeNames = new string[numFlds];
	AttrStk *stk = ud->schema()->GetFlatAttrs();
	for(int i = 0; i < numFlds; i++){
		Attr *at = stk->ith(i);
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

void DevRead::Close(){

	delete ud;
	delete [] typeIDs;
	typeIDs = NULL;
	delete [] attributeNames;
	attributeNames = NULL;
	delete order;
	order = NULL;
}

#endif
