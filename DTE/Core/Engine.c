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
#include "joins.h"

extern int yyparse();
extern int yydebug;
Exception* currExcept;

ParseTree* parseTree = NULL;
List<String*>* namesToResolve = NULL;
BaseSelection * sequenceby;
char* queryString;
BaseSelection * withPredicate;
DefaultExceptHndl defaultExceptHndl;
List<JoinTable*>* joinList = NULL;
JoinTable * joinTable = NULL;
JoinTable * jTable = NULL;
ITimer iTimer;

int Engine::optimize(){
	queryString = strdup(query.chars());
	namesToResolve = new List<String*>;
	TRY(int parseRet = yyparse(), 0);
	if(parseRet != 0){
		String msg = "parse error in: " + String(queryString);
		THROW(new Exception(msg), 0);
	}
	assert(parseTree);
	TRY(topNode = parseTree->createSite(), 0);
	delete namesToResolve;
	namesToResolve = NULL;
	return 0;
}

void printString(String & str){
	
	cout << str.chars() << endl;

}
