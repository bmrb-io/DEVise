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
  Revision 1.7  1996/12/15 06:41:09  donjerko
  Added support for RTree indexes

  Revision 1.6  1996/12/09 10:01:52  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.5  1996/12/07 15:14:28  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/05 16:06:02  wenger
  Added standard Devise file headers.

 */

%{

#include "myopt.h"
#include "queue.h"
#include "ParseTree.h"
#include <iostream.h>
#include <String.h>
#include <assert.h>
#include <stdio.h>

extern int yylex();
extern ParseTree* parseTree;
extern List<String*>* namesToResolve;
/* extern BaseSelection * sequencebyTable;*/
extern String *sequencebyTable; 
int yyerror(char* msg);

%}
%union{
     String* string;
	int integer;
	double real;
	BaseSelection* sel;
	Path* path;
	List<BaseSelection*>* selList;
	List<TableAlias*>* tableList;
	TableAlias* tabAlias;
}
%token <integer> INT
%token <real> DOUBLE
%token SELECT
%token FROM
%token WHERE
%token SEQUENCEBY 
%token CREATE
%token INDEX
%token ON
%token <string> STRING_CONST
%left '.'
%left OR
%left AND
%left '='
%left <string> LESSGREATER
%left '-' '+'
%left '*' '/'
%left <string> STRING 
%type <sel> selection
%type <path> expression
%type <selList> listOfSelections
%type <tableList> listOfTables
%type <tabAlias> tableAlias
%type <sel> optWhereClause
%type <sel> predicate
%type <string> optString
%type <string> optSequenceByClause
%type <string> TableName
%type <sel> attribute
%type <string> index_name
%type <string> table_name
%%
input : query
	| definition
	;
definition: CREATE INDEX index_name ON table_name '(' listOfSelections ')' ';'{
		parseTree = new IndexParse($3, $5, $7, namesToResolve);
		YYACCEPT;
	}
	;
index_name : STRING
	;
table_name : STRING
	;
query : SELECT listOfSelections 
	   FROM listOfTables optWhereClause optSequenceByClause ';' {
		parseTree = new QueryTree($2, $4, $5, $6, namesToResolve);
		return 0;
	}
	| SELECT '*' FROM listOfTables optWhereClause optSequenceByClause ';' {
		parseTree = new QueryTree(NULL, $4, $5, $6, namesToResolve);
		return 0;
	}
     ;
listOfSelections : listOfSelections ',' predicate {
		$1->append($3);
		$$ = $1;
	}
	| predicate {
		$$ = new List<BaseSelection*>;
		$$->append($1);
	}
	| {
		$$ = new List<BaseSelection*>;
	}
     ;
listOfTables : listOfTables ',' tableAlias {
		$1->append($3);
		$$ = $1;
	}
	| tableAlias {
		$$ = new List<TableAlias*>;
		$$->append($1);
	}
	;
optWhereClause : WHERE predicate {
         $$ = $2;
	}
	| {
		$$ = NULL;
	}
	;

optSequenceByClause : SEQUENCEBY TableName{
        $$ = $2;
	}
	| {
		$$ = NULL;
	}
	;
predicate : predicate OR predicate {
          $$ = new OrOperator($1, $3);
	}
	| predicate AND predicate {
		$$ = new AndOperator($1, $3);
	}
     | predicate '=' predicate {
          $$ = new Operator("=", $1, $3);
	}
	| predicate LESSGREATER predicate {
		$$ = new Operator(*$2, $1, $3);
	}
     | predicate '+' predicate {
          $$ = new Operator("+", $1, $3);
	}
	| predicate STRING predicate {
		$$ = new Operator(*$2, $1, $3);
	}
	| selection
	;
TableName: STRING {
		$$ = $1;
	}
attribute:
	STRING '.' expression {
		$$ = new PrimeSelection($1, $3);
	}

selection :
	STRING '.' expression {
		$$ = new PrimeSelection($1, $3);
	}
	| STRING {
		String* dummy = new String("");
		namesToResolve->append(dummy); 
		$$ = new PrimeSelection(dummy, new Path($1, NULL));
	}
	| STRING '(' listOfSelections ')' {
		String* dummy = new String;
		$$ = new PrimeSelection(dummy, new Method($1, $3, NULL));
	}
	| STRING_CONST {
		$$ = new ConstantSelection("string", new IString($1));
	}
	| INT {
		$$ = new ConstantSelection("int", new IInt($1));
	}
	| DOUBLE {
		$$ = new ConstantSelection("double", new IDouble($1));
	}
	| '(' predicate ')' {
		$$ = $2;
	}
	;
tableAlias : STRING optString {
		$$ = new TableAlias($1, $2);
	}
	| STRING_CONST optString {
		$$ = new TableAlias($1, $2, true);
	}
	;
optString : STRING {
		$$ = $1;
	}
	| {
		$$ = NULL;
	}
	;
expression : STRING {
		$$ = new Path($1, NULL);
	}
	| STRING '(' listOfSelections ')' {
		$$ = new Method($1, $3, NULL);
	}
	| expression '.' expression {
		$1->append($3);
		$$ = $1;
	}
	;
%%
int yyerror(char* msg){
	return 0;
}
