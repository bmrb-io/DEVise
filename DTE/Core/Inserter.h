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
  Revision 1.11  1998/06/04 23:06:48  donjerko
  Added DataReader.

  Revision 1.10  1998/03/17 17:18:55  donjerko
  Added new namespace management through relation ids.

  Revision 1.9  1997/09/05 22:20:05  donjerko
  Made changes for port to NT.

  Revision 1.8  1997/08/21 21:04:21  donjerko
  Implemented view materialization

  Revision 1.7  1997/08/14 02:08:51  donjerko
  Index catalog is now an independent file.

  Revision 1.6  1997/08/12 19:58:40  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.5  1997/06/21 22:47:59  donjerko
  Separated type-checking and execution into different classes.

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

//#include<iostream.h>   erased for sysdep.h

#include "types.h"
#include "sysdep.h"

#include <vector>

#ifndef __GNUG__
using namespace std;
#endif

class Inserter {
	ostream* out;
	int numFlds;
	vector<WritePtr> writePtrs;
public:
	Inserter(){
		out = NULL;
	}
	virtual ~Inserter(){
		delete out;
	}
	void open(const ISchema& schema, string urlstring, int mode = ios::app); 
		// throws

	void open(ostream* out, int numFlds, const TypeID* typeIDs){ // throws
		this->out = out;
		this->numFlds = numFlds;
		TypeIDList types(typeIDs, numFlds);
		TRY(writePtrs = getWritePtrs(types), NVOID );
	}
	void insert(const Tuple* tuple){ // throws
		assert(out);
		for(int i = 0; i < writePtrs.size(); i++){
			writePtrs[i](*out, tuple[i]);
			*out << " ";
		}
		*out << "\n";
	}
	virtual void close(){
		delete out;
		out = NULL;
	}
};

class UniqueInserter : public Inserter {
	string finalFile;
	string tmpFile;
	ISchema schema;
	int mode;
public:
	UniqueInserter(const ISchema& schema, const string& urlstring, 
		int mode = ios::app);
	virtual void close();
};

class Modifier {
	ISchema schema;
	string fileName;
public:
	Modifier(const ISchema& schema, const string& fileName) :
		schema(schema), fileName(fileName) {}
	void replace
		(const string* key, const Type* object, const string* key2 = NULL);
	// throws

private:
	bool predicate(const string* key1, const string* key2, const Tuple* tup);
};

inline bool Modifier::predicate
	(const string* key1, const string* key2, const Tuple* tup)
{
	assert(key1);
	if(*key1 != IString::getCStr(tup[0])){
		return false;
	}
	if(key2 == NULL){
		return true;
	}
	return *key2 == IString::getCStr(tup[1]);
}

#endif
