#include "site.h"
#include "catalog.h"
#include "GeneralRead.h"
#include "DevRead.h"
#include "StandardRead.h"
#include "ApInit.h" /* for DoInit */
#include "string.h"
#include "url.h"
#include <iostream.h>

Site* Catalog::DeviseInterface::getSite(){
	char* schema = strdup(schemaNm.chars());
	char* data = strdup(dataNm.chars());
#ifdef StandAlone
	Init::DoInit();
#endif
	GeneralRead* unmarshal = new DevRead(schema, data);
     return new LocalTable("", unmarshal);	
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
     return new LocalTable("", unmarshal);	
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
	return in;
}
