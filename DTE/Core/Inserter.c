#include "url.h"
#include "Inserter.h"
#include "StandardRead.h"

void Inserter::open(const ISchema& schema, String urlString){ // throws
	TRY(URL* url = new URL(urlString), );
	numFlds = schema.getNumFlds();
	const TypeID* types = schema.getTypeIDs();
	TRY(writePtrs = newWritePtrs(types, numFlds), );
	TRY(out = url->getOutputStream(ios::app), );
	delete url;
}
