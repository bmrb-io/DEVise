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
  Revision 1.29  1997/09/05 22:56:09  donjerko
  *** empty log message ***

  Revision 1.28  1997/09/05 22:20:15  donjerko
  Made changes for port to NT.

  Revision 1.27  1997/08/25 15:28:12  donjerko
  Added minmax table

  Revision 1.26  1997/08/22 23:13:04  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.25  1997/08/21 21:04:29  donjerko
  Implemented view materialization

  Revision 1.24  1997/08/14 02:08:54  donjerko
  Index catalog is now an independent file.

  Revision 1.23  1997/08/12 19:58:43  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.22  1997/08/09 00:54:44  donjerko
  Added indexing of select-project unmaterialized views.

  Revision 1.21  1997/07/30 21:39:23  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.20  1997/07/22 15:00:54  donjerko
  *** empty log message ***

  Revision 1.19  1997/06/30 23:05:04  donjerko
  CVS:

  Revision 1.18  1997/06/21 22:48:06  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.17  1997/06/16 16:04:47  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.16  1997/05/21 14:31:40  wenger
  More changes for Linux.

  Revision 1.15  1997/04/10 21:50:26  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.13  1997/03/23 23:45:22  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.12  1997/03/20 20:42:21  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.11  1997/03/14 18:36:11  donjerko
  Making space for the SQL UNION operator.

  Revision 1.10  1997/02/25 22:14:52  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

 */

#include <string>
//#include <fstream.h>   erased for sysdep.h
//#include <iostream.h>   erased for sysdep.h
#include "StandardRead.h"
#include "catalog.h"
#include "Iterator.h"
// #include "DevRead.h"
#include "url.h"
#include "site.h"
#include "Utility.h"
// #include "Engine.h"
#include "Inserter.h"
#include "Interface.h"
#include "sysdep.h"

void readFilter(string viewNm, string& select, 
		string*& attributeNames, int& numFlds, string& where){
	ifstream vin(viewNm.c_str());
	if(!vin){
		string msg = "Could not open view file \"" + viewNm + "\"";
		THROW(new Exception(msg), NVOID );
	}
	vin >> numFlds;
	if(!vin){
		string msg = "Number of fieds expected in file \"" + viewNm + "\"";
		THROW(new Exception(msg), NVOID );
	}
	attributeNames = new string[numFlds];
     for(int i = 0; i < numFlds; i++){
          vin >> attributeNames[i];
     }
     TRY(stripQuotes(vin, select), NVOID );
     TRY(stripQuotes(vin, where), NVOID );
}

Site* Catalog::find(TableName* path) const { // Throws Exception
	TRY(Interface* interf = createInterface(path), NULL);	
	Site* retVal = interf->getSite();
	delete interf;
	return retVal;
}

void Directory::replace(const string& entry, const Interface* interf) const
{
	Modifier modifier(DIR_SCHEMA, fileName);

	TRY(modifier.replace(&entry, interf), NVOID );
}

Interface* Directory::createInterface(const string& entry) const
{ 

// Throws Exception

//	cerr << "searching for " << entry << " in " << fileName << endl;

	ifstream* in = new ifstream(fileName.c_str());
	assert(in);
	if(!(*in)){
		delete in;
		string msg = "Could not open file " + fileName;
		THROW(new Exception(msg), NULL);
	}
	int dsnf = DIR_SCHEMA.getNumFlds();
	const TypeID* dst = DIR_SCHEMA.getTypeIDs();
	Iterator* iterator = new StandReadExec(dsnf, dst, in);
	assert(iterator);
	iterator->initialize();

	const Tuple* tuple;
	tuple = iterator->getNext();
	CHECK("Illegal catalog format", NULL);
	while(tuple){
		const char* tableNm = IString::getCStr(tuple[0]);
		if(entry == tableNm){
			const Interface* tmp = InterfWrapper::getInterface(tuple[1]);
			Interface* retVal = tmp->duplicate();
			delete iterator;
			return retVal;
		}
		tuple = iterator->getNext();
		CHECK("Illegal catalog format", NULL);
	}
	delete iterator;
	string msg = "Table " + entry + " not defined in file: " +
		fileName;
	THROW(new Exception(msg), NULL);
}

Interface* Catalog::createInterface(TableName* path) const { 

// Throws Exception

	if(path->isEmpty()){
		// return new StandardInterface(DIR_SCHEMA, fileName);
		return new CatalogInterface(fileName);
	}
	string firstPathNm = *path->getFirst();
	path->deleteFirst();
	Directory dir(fileName);
	TRY(Interface* interf = dir.createInterface(firstPathNm), NULL);
	if(interf->getType() == Interface::CATALOG){
		CatalogInterface* tmp = (CatalogInterface*) interf;
		TRY(string newFileNm = tmp->getCatalogName(), NULL);
		Catalog tmpCat(newFileNm);
		return tmpCat.createInterface(path);
	}
	else if(interf->getType() != Interface::QUERY &&
			path->cardinality() > 0){
		string msg = "Table " + firstPathNm + " is not a catalog";
		THROW(new Exception(msg), NULL);
	}
	return interf;
}

