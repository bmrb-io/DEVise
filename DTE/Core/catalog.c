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
  Revision 1.5  1996/12/15 06:41:07  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#include <string.h>
#include <fstream.h>
#include <iostream.h>
#include "catalog.h"
#include "Iterator.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "url.h"
#include "site.h"

Site* DeviseInterface::getSite(){
	char* schema = strdup(schemaNm.chars());
	char* data = strdup(dataNm.chars());
	Iterator* unmarshal = new DevRead(schema, data);
     return new LocalTable("", unmarshal, &indexes);	
}

Site* StandardInterface::getSite(){ // Throws a exception

	LOG(logFile << "Contacting " << urlString << " @ ");
	LOG(iTimer.display(logFile) << endl);

	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	StandardRead* unmarshal = new StandardRead(in);
	TRY(unmarshal->open(), NULL);

	LOG(logFile << "Initialized @ ");
	LOG(iTimer.display(logFile) << endl);

	delete url;
     return new LocalTable("", unmarshal, &indexes);	
}

Site* CatalogInterface::getSite(){ // Throws a exception
	Site* retVal = NULL;
	if(tableName->isEmpty()){

		// If it was not for indexes, this clause could be deleted

		ifstream* catalog = new ifstream(fileName);	
		assert(catalog);
		if(!catalog->good()){
			String msg = "Cannot open catalog: " + fileName;
			THROW(new Exception(msg), NULL);
		}
		StandardRead* iterator = new StandardRead(catalog);
		TRY(iterator->open(), NULL);
		retVal = new LocalTable("", iterator, &indexes);
	}
	else {
		Catalog catalog(fileName);
		TRY(retVal = catalog.find(tableName), NULL);
		tableName = NULL;
	}
	return retVal;
}

istream& CGIInterface::read(istream& in){  // throws
	in >> urlString;
	in >> entryLen;
	if(!in){
		String e = "Catalog error: number of entries expected";
		THROW(new Exception(e), in);
	}

	entries = new CGISite::Entry[entryLen];
	for(int i = 0; i < entryLen; i++){
		TRY(entries[i].read(in), in);
	}
	return in;
}


Site* Catalog::find(TableName* path){     // Throws Exception
	ifstream* in = new ifstream(fileName);
	if(path->isEmpty()){
		StandardRead* iterator = new StandardRead(in);
		TRY(iterator->open(), NULL);
		return new LocalTable("", iterator, NULL);
	}
	if(!in->good()){
		String msg = "Cannot open catalog: " + fileName;
		THROW(new Exception(msg), NULL);
	}
	in->ignore(INFINITY, ';');	// ignore the header
	if(!in->good()){
		String msg = "Could not find end of header in: " + fileName;
		THROW(new Exception(msg), NULL);
	}
	String tableNm;
	*in >> tableNm;
	while(in->good()){
		if(path->isEmpty() || tableNm == *path->getFirst()){
			CatEntry* entry = new CatEntry(path);
			TRY(entry->read(*in), NULL); 
			delete in;
			Site* retVal = entry->getSite();
			delete entry;
			return retVal;
		}
		else{
			in->ignore(INFINITY, ';');
		}
		*in >> tableNm;
	}
	delete in;
	String msg = "Table " + *path->getFirst() + " not defined";
	THROW(new Exception(msg), NULL);
}
