#ifndef INSERTER_H
#define INSERTER_H

#include<iostream.h>
#include "url.h"

class Inserter {
	String urlString;
	ostream* out;
	int numFlds;
	WritePtr* writePtrs;
public:
	Inserter(String urlString) : urlString(urlString) {
		out = NULL;
	}
	~Inserter(){
		delete [] writePtrs;
		delete out;
	}
	void open(){ // throws
		TRY(URL* url = new URL(urlString), );
		TRY(istream* in = url->getInputStream(), );
		Iterator* iterator = new StandardRead(in);
		TRY(iterator->open(), );
		numFlds = iterator->getNumFlds();
		TRY(writePtrs = iterator->getWritePtrs(), );
		delete in;
		TRY(out = url->getOutputStream(ios::app), );
		delete url;
	}
	void insert(Tuple* tuple){ // throws
		assert(out);
		for(int i = 0; i < numFlds; i++){
			writePtrs[i](*out, tuple[i]);
			*out << " ";
		}
		*out << endl;
	}
};

#endif
