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
  Revision 1.10  1996/12/21 22:21:49  donjerko
  Added hierarchical namespace.

  Revision 1.9  1996/12/19 08:25:47  kmurli
  Changes to include the with predicate in sequences.

  Revision 1.8  1996/12/16 11:13:09  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

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
#include "exception.h"
#include "queue.h"
#include "ParseTree.h"
#include "joins.h"
#include <iostream.h>
#include <String.h>
#include <assert.h>
#include <stdio.h>

extern int yylex();
extern ParseTree* parseTree;
extern List<String*>* namesToResolve;
extern List<JoinTable*>* joinList;
extern JoinTable * joinTable;
extern JoinTable * jTable;
extern BaseSelection* withPredicate;
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
	List<String*>* tableName;
}
%token <integer> INT
%token <real> DOUBLE
%token SELECT
%token FROM
%token AS
%token WHERE
%token SEQUENCEBY 
%token JOINNEXT 
%token JOINPREV 
%token CREATE
%token INDEX
%token ON
%token OVER 
%token WITH 
%token <string> STRING_CONST
%left '.'
%left OR
%left AND
%left '='
%left <string> LESSGREATER
%left '-' '+'
%left '*' '/'
%left <string> STRING 
%type <string> JoinString 
%type <sel> selection
%type <integer> optShiftVal 
%type <path> expression
%type <selList> listOfSelections
%type <selList> optOverClause
%type <integer> optOffset 
%type <sel> optWithClause
%type <tableList> listOfTables
%type <tabAlias> tableAlias
%type <tableList> JoinList 
%type <sel> optWhereClause
%type <sel> predicate
%type <string> optString
%type <string> optSequenceByClause
%type <string> index_name
%type <tableName> table_name
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
table_name : table_name '/' STRING {
		$1->append($3);
		$$ = $1;
	}
	| '/' STRING {
		$$ = new List<String*>;
		$$->append($2);
	}
	| '/' {
		$$ = new List<String*>;
	}
	;
query : SELECT listOfSelections 
	   FROM listOfTables optWhereClause optSequenceByClause ';' {
		parseTree = new QueryTree($2, $4, $5, $6, withPredicate,namesToResolve);
		return 0;
	}
	| SELECT '*' FROM listOfTables optWhereClause optSequenceByClause ';' {
	 parseTree = new QueryTree(NULL, $4, $5, $6, withPredicate,namesToResolve);
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
listOfTables : 
		listOfTables ',' JoinList{
		$1->addList($3);
		$$ = $1;
		if (!joinList)
			joinList = new List<JoinTable*>;
		joinList->append(joinTable);
		joinTable = NULL;
	}
	| JoinList {
		$$ = new List<TableAlias*>;
		$$->addList($1);
		if (!joinList)
			joinList = new List<JoinTable*>;
		joinList->append(joinTable);
		joinTable = NULL;
	}
	;
optWhereClause : WHERE predicate {
         $$ = $2;
	}
	| {
		$$ = NULL;
	}
	;

optSequenceByClause :SEQUENCEBY STRING optWithClause{
        $$ = $2;
		withPredicate= $3;
	}
	| {
		$$ = NULL;
		withPredicate= NULL;
	}
	;
optWithClause: WITH predicate {
		$$ = $2;
	}
	|{
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

selection :
	STRING '.' expression {
		$$ = new PrimeSelection($1, $3);
	}
	| STRING {
		String* dummy = new String("");
		namesToResolve->append(dummy); 
		$$ = new PrimeSelection(dummy, new Path($1, NULL));
	}
	| STRING '(' selection ')' optOverClause {
		String* dummy = new String;
		List <BaseSelection *> * dummylist = new List<BaseSelection*>;
		if ($5 != NULL){
			dummylist->addList($5);
		}
		dummylist->prepend($3);
		$$ = new PrimeSelection(dummy, new Method($1, dummylist));
	}
	| STRING '(' listOfSelections ')' {
		String * dummy = new String("");	
		$$ = new PrimeSelection(dummy, new Method($1,$3));
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

optOverClause:	
	OVER '(' listOfSelections ')' {
			$$ = $3;
	}
	| {
		$$ = NULL;
	}
	;

JoinList: JoinList JoinString JoinList{
		$$ = $1;
		$$->addList($3);
		//cout << " Calling JoinTable " << jTable << "- " << joinTable << endl;
		joinTable = new JoinTable(joinTable,jTable,$2);
		jTable = NULL;
		//cout << " joinTable created with joinprev "<< endl;
	}
	| tableAlias {
		$$ = new List<TableAlias*>;
		$$->append($1);
		if (!joinTable){	
			joinTable = new JoinTable($1);
			//cout << " joinTable created with " << *$1->table << endl;
		}
		else{
			jTable = new JoinTable($1);
			//cout << " jTable created with " << *$1->table << endl;
		}
	}
	;

JoinString : JOINPREV {
		$$ = new String("joinprev");
	}
	| JOINNEXT {
		$$ = new String("joinnext");
	}
	;

optOffset: '(' INT ')' {
		$$ = $2;
	}
	|{
		$$ = 1;
	}
	;
tableAlias : STRING '(' table_name optShiftVal ')' AS STRING{
		$$ = new TableAlias(new TableName($3),$7,$1,$4);
	}
	|
	table_name AS STRING {
		$$ = new TableAlias(new TableName($1), $3);
	}
	| table_name {
        if($1->cardinality() == 1){
            $1->rewind();
            String* tmp = new String(*$1->get());
            $$ = new TableAlias(new TableName($1), tmp);
        }
        else{
            String msg = "Sorry, you need to specify alias";
            THROW(new Exception(msg), 0);
        }
    }
	| STRING_CONST optString {
		$$ = new QuoteAlias($1, $2);
	}
	;
optShiftVal: ',' INT {
		$$ = $2;
	}
	|{
		$$ = 0;
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
