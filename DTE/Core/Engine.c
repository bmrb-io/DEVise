/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
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
  Revision 1.17  1997/08/22 23:13:01  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.16  1997/08/21 21:04:16  donjerko
  Implemented view materialization

  Revision 1.15  1997/08/09 00:54:42  donjerko
  Added indexing of select-project unmaterialized views.

  Revision 1.14  1997/03/20 20:42:18  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.13  1997/02/03 04:11:26  donjerko
  Catalog management moved to DTE

  Revision 1.12  1996/12/26 03:42:00  kmurli
  MOdified to make joinprev work right

  Revision 1.11  1996/12/25 19:53:58  kmurli
  Files to do joinprev,joinnext and function calls.

  Revision 1.10  1996/12/24 21:00:51  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.9  1996/12/21 22:21:43  donjerko
  Added hierarchical namespace.

  Revision 1.8  1996/12/19 08:25:46  kmurli
  Changes to include the with predicate in sequences.

  Revision 1.7  1996/12/16 11:13:04  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.6  1996/12/09 10:01:50  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.5  1996/12/07 15:14:24  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/05 16:05:58  wenger
  Added standard Devise file headers.

 */

//#include<memory.h>   erased for sysdep.h
#include "sysdep.h"
#include <string>
#include<assert.h>
#include<math.h>
//#include<stdlib.h>   erased for sysdep.h
#include "queue.h"
#include "myopt.h"
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "ParseTree.h"
#include "joins.h"
#include "ExecOp.h"

extern int my_yyparse();
extern int yydebug;
Exception* currExcept;

ParseTree* parseTree = NULL;
List<string*>* namesToResolve = NULL;
BaseSelection * sequenceby;
const char* queryString;
bool rescan;
BaseSelection * withPredicate;
DefaultExceptHndl defaultExceptHndl;
List<JoinTable*>* joinList = NULL;
JoinTable * joinTable = NULL;
JoinTable * jTable = NULL;
ITimer iTimer;

int Engine::optimize(){
	queryString = query.c_str();
	rescan = true;
	namesToResolve = new List<string*>;
	TRY(int parseRet = my_yyparse(), 0);
	if(parseRet != 0){
		string msg = "parse error in: " + string(queryString);
		THROW(new Exception(msg), 0);
	}
	assert(parseTree);
	TRY(topNode = parseTree->createSite(), 0);
	delete namesToResolve;
	namesToResolve = NULL;
	return 0;
}

Iterator* ViewEngine::createExec(){
	return new RidAdderExec(topNode->createExec(), numFlds);
}
