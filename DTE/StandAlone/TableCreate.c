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
  Development Group.sel
*/

#include "RelationManager.h"
#include "types.h"
#include "Inserter.h"

int main(){
	int numFlds = 1;
	TypeID* const types = new TypeID[numFlds];
	string* const attrs = new string[numFlds];
	types[0] = INT_TP;
	attrs[0] = "A";
     ISchema schema(numFlds, types, attrs);
     delete [] types;
     delete [] attrs;
	string filePath = "/tmp/junk";
     StandardInterface stdInt(schema, filePath);

     RelationId relId;
     TRY(relId = RELATION_MNGR.registerNewRelation(stdInt), 1);

	cerr << "Relation id is: " << relId.toString() << endl;

	TRY(UniqueInserter in(schema, filePath, ios::out), 0);

	Tuple* tuple = new Tuple[numFlds];
	int* ip = new int;

	for(int i = 0; i < 1000; i++){
		*ip = i;
		tuple[0] = IInt::getTypePtr(ip);
		in.insert(tuple);
	}
	*ip = 0;
	tuple[0] = IInt::getTypePtr(ip);
	in.insert(tuple);
	*ip = 5;
	tuple[0] = IInt::getTypePtr(ip);
	in.insert(tuple);

	TRY(in.close(), 0);

	delete [] tuple;
	delete ip;
     TRY(RELATION_MNGR.deleteRelation(relId), 1);

	return 0;
}
