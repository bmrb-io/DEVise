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
  Revision 1.14  1998/10/01 20:58:44  yunrui
  *** empty log message ***

  Revision 1.13  1998/08/17 18:51:32  wenger
  Updated solaris dependencies for egcs; fixed most compile warnings;
  bumped version to 1.5.4.

  Revision 1.12  1998/07/24 04:37:50  donjerko
  *** empty log message ***

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

//kb: class Inserter only works for local ascii StandardTables
//kb: class Inserter should be made abstract, with StandardInserter derived
class Inserter
{
protected:

  ofstream out;
  DteTupleAdt tupAdt;
  string filename;

public:

  Inserter() {}

  Inserter(const string& filename, const DteTupleAdt& tupAdt, 
           int mode = ios::app)
    : tupAdt(tupAdt) { open(filename, mode); }

  ~Inserter() {}

  void open(const string& filename, int mode = ios::app);

  void open(const string& filename, const DteTupleAdt& tupAdt,
            int mode = ios::app);

  //void open(ostream* out, const DteTupleAdt& tupAdt) {
  //this->out = out;
  //this->tupAdt = tupAdt;
  //}

  void insert(const Tuple* tuple); // throws
};

typedef Inserter GestaltInserter; // *** YL


class UniqueInserter : protected Inserter {
	string finalFile;
	string tmpFile;
        int mode;
public:
	UniqueInserter(const string& filename, const DteTupleAdt& tupAdt, 
                       int mode = ios::app);
        ~UniqueInserter();
        Inserter::insert;
};


#if 0
//kb: Modifier is not used
class Modifier {
	ISchema schema;
	string fileName;
public:
	Modifier(const ISchema& schema, const string& fileName) :
		schema(schema), fileName(fileName) {}
	void replace(const string* key, const Type* object,
          const string* key2 = NULL); // throws

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

#endif
