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
  Revision 1.27  1998/10/28 19:21:36  wenger
  Added code to check all data sources (runs through the catalog and tries
  to open all of them); this includes better error handling in a number of
  data source-related areas of the code; also fixed errors in the propagation
  of command results.

  Revision 1.26  1998/07/24 04:37:47  donjerko
  *** empty log message ***

  Revision 1.25  1998/07/06 21:07:02  wenger
  More memory leak hunting -- partly tracked down some in the DTE.

  Revision 1.24  1998/06/28 21:47:35  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

  Revision 1.23  1997/12/04 04:05:09  donjerko
  *** empty log message ***

  Revision 1.22  1997/11/24 23:13:14  weaver
  Changes for the new ColorManager.

  Revision 1.21  1997/11/12 23:17:22  donjerko
  Improved error checking.

  Revision 1.20  1997/11/08 21:02:25  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

  Revision 1.19  1997/11/05 00:19:36  donjerko
  Separated typechecking from optimization.

  Revision 1.18  1997/09/05 22:20:02  donjerko
  Made changes for port to NT.

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
//#include "queue.h"
#include "myopt.h"
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "ParseTree.h"
#include "ExecOp.h"
#include "DTE/types/DteIntAdt.h"

extern int my_yyparse();
extern int yydebug;
Exception* currExcept;

//kb: make parser class; remove these globals
ParseTree* globalParseTree = NULL;
const char* queryString;
bool rescan;
DefaultExceptHndl defaultExceptHndl;
ITimer iTimer;

Engine::~Engine(){
	delete topNodeIt;
	topNodeIt = 0;
	delete parseTree;
	parseTree = 0;
        delete schema;
        schema = 0;
}

const ParseTree* Engine::parse(){
#if 0 // This assertion sometimes fails if there were previously problems
      // opening a data source.  RKW 1998-10-28.
	assert(globalParseTree == 0);
#else
	delete globalParseTree;
	globalParseTree = 0;
#endif
	queryString = query.c_str();
        cerr << "parsing query: " << queryString << endl;
	rescan = true;
        //yydebug = 0;
	TRY(int parseRet = my_yyparse(), 0);
	if(parseRet != 0){
		globalParseTree = 0;
		string msg = "Failed to parse the query: " + query;
		THROW(new Exception(msg), 0);
		// throw Exception(msg);
	}
        cerr << "query parsed\n";
	assert(globalParseTree);
	parseTree = globalParseTree;
	globalParseTree = 0;
	return parseTree;
}

const ISchema* Engine::typeCheck(){
	if(!parseTree){
		TRY(parse(), 0);
	}
	assert(schema == 0);
	// This may leak a bunch of memory.  RKW 7/6/98.
	const ISchema* s;
	TRY(s = parseTree->getISchema(), 0);
        assert(s);
        schema = new ISchema(*s);
	cerr << "result schema: " << *schema << endl;
	return schema;
}

int Engine::optimize(){
	if(!schema){
		TRY(typeCheck(), 0);
	}
	// This may leak a bunch of memory.  RKW 7/6/98.
	TRY(topNodeIt = parseTree->createExec(), 0);
        if( topNodeIt ) {
          cerr << "iterator schema: " << topNodeIt->getAdt().getTypeSpec() << endl;
        } else {
          cerr << "no iterator returned\n";
        }
	return 0;
}

//---------------------------------------------------------------------------

ViewEngine::ViewEngine(const string& query, const vector<string>& attrNames)
  : Engine(query), attributeNames(attrNames)
{
}

ViewEngine::~ViewEngine()
{
}

const ISchema* ViewEngine::typeCheck()
{
  Engine::typeCheck();
  assert(schema);
  schema->setAttributeNames(attributeNames);
  schema->push_front(DteIntAdt(), RID_STRING);
  return schema;
}


Iterator* ViewEngine::createExec(){
	TRY(Iterator* tmp = parseTree->createExec(), 0);
	return new RidAdderExec(tmp);
}

