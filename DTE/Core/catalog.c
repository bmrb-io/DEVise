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
  Revision 1.4  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#include "site.h"
#include "catalog.h"
#include "Iterator.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "string.h"
#include "url.h"
#include <iostream.h>

Site* Catalog::DeviseInterface::getSite(){
	char* schema = strdup(schemaNm.chars());
	char* data = strdup(dataNm.chars());
	Iterator* unmarshal = new DevRead(schema, data);
     return new LocalTable("", unmarshal, &indexes);	
}

Site* Catalog::StandardInterface::getSite(){ // Throws a exception

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

istream& Catalog::CGIInterface::read(istream& in){  // throws
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
	site = new CGISite(urlString, entries, entryLen);
	return in;
}

istream& Catalog::Entry::read(istream& in){ // Throws Exception
	String typeNm;
	in >> typeNm;
	if(!in){
		String msg = "Interface for table " + tableNm + 
			" must be specified";
		THROW(new Exception(msg), in);
	}
	if(typeNm == "DeviseTable"){
		interface = new DeviseInterface(tableNm);
		TRY(interface->read(in), in);
	}
	else if(typeNm == "StandardTable"){
		interface = new StandardInterface(tableNm);
		TRY(interface->read(in), in);
	}
	else if(typeNm == "QueryInterface"){
		interface = new QueryInterface(tableNm);
		TRY(interface->read(in), in);
	}
	else if(typeNm == "CGIInterface"){
		interface = new CGIInterface(tableNm);
		TRY(interface->read(in), in);
	}
	else{
		String msg = "Table " + tableNm + " interface: " + 
			typeNm + ", not defined";
		THROW(new Exception(msg), in);
	}
	String indexStr;
	in >> indexStr;
	while(in && indexStr != ";"){
		if(in && indexStr == "index"){
			TRY(interface->readIndex(in), in);
		}
		else {
			String msg = 
			"Invalid catalog format: \"index\" or \";\" expected";
			THROW(new Exception(msg), in);
		}
		in >> indexStr;
	}
	if(!in){
		String msg = "Premature end of catalog";
		THROW(new Exception(msg), in);
	}
	return in;
}

void Catalog::Entry::write(ostream& out){
	out << tableNm << " ";
	interface->write(out);
	out << " ; " << endl;
}
