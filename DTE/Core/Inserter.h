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
  $Id$

  $Log$
  Revision 1.4  1997/06/16 16:04:42  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.3  1997/04/10 21:50:24  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.2  1997/03/28 16:07:24  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#ifndef INSERTER_H
#define INSERTER_H

#include<iostream.h>
#include "types.h"

class Inserter {
	String urlString;
	ostream* out;
	int numFlds;
	WritePtr* writePtrs;
public:
	Inserter(){
		out = NULL;
		writePtrs = NULL;
	}
	~Inserter(){
		delete [] writePtrs;
		delete out;
	}
	void open(String urlString); // throws
	void open(ostream* out, int numFlds, const TypeID* typeIDs){ // throws
		this->out = out;
		this->numFlds = numFlds;
		TRY(writePtrs = newWritePtrs(typeIDs, numFlds), );
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
