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
  Revision 1.10  1997/06/16 16:05:22  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.8  1997/03/28 16:08:05  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include<iostream.h>
#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include "types.h"
#include "exception.h"
#include "Engine.h"
// #include "ApInit.h" /* for DoInit */
#include "RTreeCommon.h"

const int DETAIL = 1;

int main(int argc, char** argv){

//	Init::DoInit();	// to initialize Devise reading stuff
	String query;
	char c = '\0';
	while(1){
		cin.get(c);
		if(c == ';'){
			cin.get(c);
			if(c == '\n'){
				break;
			}
			else{
				query += String(';') + String(c);
			}
		}
		else{
			query += c;
		}
	}
	iTimer.reset();
	cout << "Query in main is: " << query << endl; 

#ifndef NO_RTREE
     initialize_system(VolumeName, RTreeFile, VolumeSize);
#endif

	Engine engine(query);
	TRY(engine.optimize(), 0);
	int numFlds = engine.getNumFlds();
	if(numFlds > 0){
		const TypeID* typeIDs = engine.getTypeIDs();
		TRY(WritePtr* writePtrs = newWritePtrs(typeIDs, numFlds), 0);
		const String* attrs = engine.getAttributeNames();
		for(int i = 0; i < numFlds; i++){
			cout << attrs[i] << " ";
		}
		cout << endl << "---------------------------------" << endl;
		const Tuple* tup;

		engine.initialize();
		while((tup = engine.getNext())){
			for(int i = 0; i < numFlds; i++){
				writePtrs[i](cout, tup[i]);
				cout << '\t';
			}
			cout << endl;
		}
		engine.finalize();

		delete [] writePtrs;
	}

     // shutdown
#ifndef NO_RTREE
     shutdown_system(VolumeName, RTreeFile, VolumeSize);
#endif

	LOG(logFile << "Query completed at ");
	LOG(iTimer.display(logFile));
}
