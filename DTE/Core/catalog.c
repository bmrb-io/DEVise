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
 */

#include "site.h"
#include "catalog.h"
#include "GeneralRead.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "string.h"
#include "url.h"
#include <iostream.h>

Site* Catalog::DeviseInterface::getSite(){
	char* schema = strdup(schemaNm.chars());
	char* data = strdup(dataNm.chars());
	GeneralRead* unmarshal = new DevRead(schema, data);
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
	int entryLen;
	in >> urlString;
	in >> entryLen;
	if(!in){
		String e = "Catalog error: number of entries expected";
		THROW(new Exception(e), in);
	}

	CGISite::Entry* entries = NULL;
	entries = new CGISite::Entry[entryLen];
	for(int i = 0; i < entryLen; i++){
		TRY(entries[i].read(in), in);
	}
	site = new CGISite(urlString, entries, entryLen);
	assert(indexes.isEmpty());
	return in;
}
