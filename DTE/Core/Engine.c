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
  Revision 1.5  1996/12/07 15:14:24  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/05 16:05:58  wenger
  Added standard Devise file headers.

 */

#include<memory.h>
#include<string.h>
#include<assert.h>
#include<math.h>
#include<stdlib.h>

#include "queue.h"
#include "myopt.h"
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "ParseTree.h"

extern int yyparse();
extern int yydebug;
Exception* currExcept;

ParseTree* parseTree = NULL;
List<String*>* namesToResolve;
BaseSelection * sequenceby;
char* queryString;

DefaultExceptHndl defaultExceptHndl;
ITimer iTimer;

int Engine::optimize(){
	queryString = strdup(query.chars());
	namesToResolve = new List<String*>;
	if(yyparse() != 0){
		String msg = "parse error in: " + String(queryString);
		THROW(new Exception(msg), 0);
	}
	delete namesToResolve;
	assert(parseTree);
	TRY(topNode = parseTree->createSite(), 0);
	return 0;
}
