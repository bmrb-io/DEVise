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
  Revision 1.3  1998/06/28 21:47:49  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.2  1998/04/09 20:26:32  donjerko
  *** empty log message ***

  Revision 1.1  1997/11/23 21:23:57  donjerko
  Added dte server.

  Revision 1.18  1997/11/20 05:31:11  okan
  *** empty log message ***

  Revision 1.17  1997/11/05 00:20:54  donjerko
  Initial version.

  Revision 1.16  1997/09/05 22:20:39  donjerko
  Made changes for port to NT.

  Revision 1.15  1997/08/23 19:41:40  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.14  1997/08/21 21:04:57  donjerko
  Implemented view materialization

  Revision 1.13  1997/08/12 19:58:58  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.12  1997/08/10 20:31:06  donjerko
  Fixed the NO_RTREE option.

  Revision 1.11  1997/07/30 21:41:23  donjerko
  *** empty log message ***

  Revision 1.10  1997/06/16 16:05:22  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.8  1997/03/28 16:08:05  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <assert.h>
#include <math.h>
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include <string>
#include "sysdep.h"

string extractQuery(istream& cin){
	string query;
	char c = '\0';
	while(1){
		cin.get(c);
		if(c == ';'){
			break;
		}
		else{
			query += c;
		}
	}
	return query;
}

void processQuery(const string& query, ostream& cout, const string& header){

	cerr << "Received query: " << query << endl;

	Engine engine(query);
	TRY(engine.optimize(), NVOID);
	int numFlds = engine.getNumFlds();
	if(numFlds > 0){
		const TypeID* typeIDs = engine.getTypeIDs();
		TRY(WritePtr* writePtrs = newWritePtrs(typeIDs, numFlds), NVOID);
		const Tuple* tup;

		TRY(tup = engine.getFirst(), NVOID);
		cout << header;
		while(tup){
			for(int i = 0; i < numFlds; i++){
				writePtrs[i](cout, tup[i]);
				cout << '\t';
			}
			cout << endl;
			tup = engine.getNext();
		}
		//engine.finalize();

		delete [] writePtrs;
	}

}
