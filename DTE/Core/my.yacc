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
  Revision 1.15  1997/02/18 18:06:04  donjerko
  Added skeleton files for sorting.

  Revision 1.14  1997/02/03 04:11:34  donjerko
  Catalog management moved to DTE

  Revision 1.13  1996/12/27 16:51:50  kmurli
  Changed my.yacc to not put non-join tables in the joinLIst

  Revision 1.12  1996/12/27 04:38:01  kmurli
  Nodified joins.h to remove erros in joinprev in case of more than one table

  Revision 1.11  1996/12/24 21:00:53  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.10  1996/12/21 22:21:49  donjerko
  Added hierarchical namespace.

  Revision 1.9  1996/12/19 08:25:47  kmurli
  Changes to include the with predicate in sequences.

  Revision 1.8  1996/12/16 11:13:09  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.7  1996/12/15 06:41:09  donjerko
  Added support for RTree indexes

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
extern char* queryString;
static int my_yyaccept();

%}
%union{
     String* string;
	int integer;
	double real;
	BaseSelection* sel;
	ConstantSelection* constantSel;
	Path* path;
	List<BaseSelection*>* selList;
	List<ConstantSelection*>* constList;
	List<TableAlias*>* tableList;
	TableAlias* tabAlias;
	List<String*>* listOfStrings;
}
%token <integer> INT
%token <real> DOUBLE
%token SELECT
%token FROM
%token AS
%token WHERE
%token SEQUENCE
%token GROUP 
%token BY 
%token JOINNEXT 
%token JOINPREV 
%token CREATE
%token DROP
%token INDEX
%token ON
%token OVER 
%token WITH 
%token INSERT
%token INTO
%token VALUES
%token DELETE
%token SCHEMA
%token ADD
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
%type <constList> listOfConstants
%type <selList> optOverClause
%type <sel> optWithClause
%type <selList> optGroupByClause
%type <tableList> listOfTables
%type <tabAlias> tableAlias
%type <tableList> JoinList 
%type <sel> optWhereClause
%type <sel> predicate
%type <constantSel> constant
/* %type <string> optString */
%type <string> optSequenceByClause
%type <string> index_name
%type <listOfStrings> table_name
%type <string> optIndType
%type <listOfStrings> keyAttrs
%type <listOfStrings> optIndAdd 
%type <listOfStrings> listOfStrings 
%%
input : query
	| definition
	;
definition: CREATE optIndType INDEX index_name ON table_name 
	'(' keyAttrs ')' optIndAdd {
		parseTree = new IndexParse($2, $4, $6, $8, $10);
		YYACCEPT;
	}
	| DROP INDEX table_name index_name {
		parseTree = new DropIndexParse($3, $4);
		YYACCEPT;
	}
	| INSERT INTO table_name VALUES '(' listOfConstants ')' {
		parseTree = new InsertParse($3, $6);
		YYACCEPT;
	}
	| DELETE table_name AS STRING WHERE predicate {
		parseTree = new DeleteParse($2, $4, $6);
		YYACCEPT;
	}
	| SCHEMA table_name {
		parseTree = new SchemaParse($2);
		YYACCEPT;
	}
	;
keyAttrs : listOfStrings
	;
optIndType: STRING {
		$$ = $1;
	}
	| {
		$$ = NULL;
	}
	;
optIndAdd: ADD '(' listOfStrings ')' {
		$$ = $3;
	}
	| {
		$$ = new List<String*>;
	}
	;
constant :
	STRING_CONST {
		$$ = new ConstantSelection("string", new IString($1));
	}
	| INT {
		$$ = new ConstantSelection("int", new IInt($1));
	}
	| DOUBLE {
		$$ = new ConstantSelection("double", new IDouble($1));
	}
	;
index_name : STRING
	;
table_name : table_name '.' STRING {
		$1->append($3);
		$$ = $1;
	}
	| '.' STRING {
		$$ = new List<String*>;
		$$->append($2);
	}
	| '.' {
		$$ = new List<String*>;
	}
	;
query : SELECT listOfSelections 
		FROM listOfTables optWhereClause 
		optSequenceByClause optGroupByClause {
		parseTree = new QueryTree(
			$2,$4,$5,$6,withPredicate,$7,namesToResolve);
		return my_yyaccept();
	}
	| SELECT '*' FROM listOfTables optWhereClause 
			optSequenceByClause optGroupByClause  {
		parseTree = new QueryTree(
	 	NULL, $4, $5, $6, withPredicate,$7,namesToResolve);
		return my_yyaccept();
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
listOfConstants : listOfConstants ',' constant {
		$1->append($3);
		$$ = $1;
	}
	| constant {
		$$ = new List<ConstantSelection*>;
		$$->append($1);
	}
     ;
listOfStrings : listOfStrings ',' STRING {
		$1->append($3);
		$$ = $1;
	}
	| STRING {
		$$ = new List<String*>;
		$$->append($1);
	}
	| {
		$$ = new List<String*>;
	}
     ;
listOfTables : 
		listOfTables ',' JoinList{
		$1->addList($3);
		$$ = $1;
		if (joinTable){
			if (!joinList)
				joinList = new List<JoinTable*>;
			joinList->append(joinTable);
		}
		joinTable = NULL;
	}
	| JoinList {
		$$ = new List<TableAlias*>;
		$$->addList($1);
		if (joinTable){
			if (!joinList)
				joinList = new List<JoinTable*>;
			joinList->append(joinTable);
		}
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

optSequenceByClause :SEQUENCE BY STRING optWithClause{
		$$ = $3;
		withPredicate= $4;
	}
	| {
		$$ = NULL;
		withPredicate= NULL;
	}
	;

optGroupByClause: GROUP BY listOfSelections{
		$$ = $3;
	}
	|{
		$$ = new List<BaseSelection*>;
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
	/*
	| STRING {
		String* dummy = new String("");
		namesToResolve->append(dummy); 
		$$ = new PrimeSelection(dummy, new Path($1, NULL));
	}
	*/
	/*
	| STRING '(' selection ')' optOverClause {
		String* dummy = new String;
		List <BaseSelection *> * dummylist = new List<BaseSelection*>;
		if ($5 != NULL){
			dummylist->addList($5);
		}
		dummylist->prepend($3);
		$$ = new PrimeSelection(dummy, new Method($1, dummylist));
	}
	*/
	| STRING '(' listOfSelections ')' optOverClause {
		String * dummy = new String("");	
		assert($3);
		if ($5 != NULL){
			$3->addList($5);
		}
		$$ = new PrimeSelection(dummy, new Method($1,$3));
	}
	| constant {
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

JoinList: JoinList JoinString tableAlias{
		$$ = $1;
		$$->append($3);
		
		if (!joinTable)
			joinTable = new JoinTable($1->get());
		
		jTable = new JoinTable($3);

		joinTable = new JoinTable(joinTable,jTable,$2);
		jTable = NULL;
	}
	| tableAlias {
		$$ = new List<TableAlias*>;
		$$->append($1);
	}
	;

JoinString : JOINPREV {
		$$ = new String("joinprev");
	}
	| JOINNEXT {
		$$ = new String("joinnext");
	}
	;

tableAlias : STRING '(' table_name optShiftVal ')' AS STRING {
		$$ = new TableAlias(new TableName($3),$7,$1,$4);
	}
	| table_name AS STRING {
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
	| STRING_CONST AS STRING {
		$$ = new QuoteAlias($1, $3);
	}
	;
optShiftVal: ',' INT {
		$$ = $2;
	}
	|{
		$$ = 0;
	}
	;
/*
optString : STRING {
		$$ = $1;
	}
	| {
		$$ = NULL;
	}
	;
*/
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

static int my_yyaccept(){
	if(yychar != YYEOF){
		YYABORT;
	}
	YYACCEPT;
}
