#include "url.h"
#include "Inserter.h"
#include "StandardRead.h"

void Inserter::open(String urlString){ // throws
	this->urlString = urlString;
	TRY(URL* url = new URL(urlString), );
	TRY(istream* in = url->getInputStream(), );
	StandardRead* iterator = new StandardRead();
	TRY(iterator->open(in), );
	numFlds = iterator->getNumFlds();
	const TypeID* types = iterator->getTypeIDs();
	TRY(writePtrs = newWritePtrs(types, numFlds), );
	delete in;
	TRY(out = url->getOutputStream(ios::app), );
	delete url;
}
