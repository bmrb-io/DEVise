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
  Revision 1.33  1997/11/05 00:19:46  donjerko
  Separated typechecking from optimization.

  Revision 1.32  1997/10/15 02:23:36  arvind
  Sequence by handles multiple attributes

  Revision 1.31  1997/10/14 05:17:19  arvind
  Implemented a first version of moving aggregates (without group bys).

  Revision 1.30  1997/10/07 18:33:38  donjerko
  *** empty log message ***

  Revision 1.29  1997/09/17 02:35:47  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.28  1997/09/05 22:20:18  donjerko
  Made changes for port to NT.

  Revision 1.27  1997/08/21 21:04:32  donjerko
  Implemented view materialization

  Revision 1.26  1997/08/15 21:19:11  donjerko
  Added / operator for ints and doubles

  Revision 1.25  1997/07/22 15:00:56  donjerko
  *** empty log message ***

  Revision 1.24  1997/06/16 17:42:51  donjerko
  Bug fix

  Revision 1.23  1997/06/16 16:04:51  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.22  1997/04/14 20:44:13  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.21  1997/04/10 21:50:28  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.20  1997/04/08 01:47:35  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.19  1997/03/20 20:42:22  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.18  1997/03/14 18:36:12  donjerko
  Making space for the SQL UNION operator.

  Revision 1.17  1997/03/06 02:35:31  donjerko
  Undefined DEBUG

  Revision 1.16  1997/02/21 01:38:07  donjerko
  Fixed some problems with "group by" clause.

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
#include <string>
#include <assert.h>
//#include <stdio.h>    erased for sysdep.h

extern int yylex();
extern ParseTree* parseTree;
extern List<string*>* namesToResolve;
extern List<JoinTable*>* joinList;
extern JoinTable * joinTable;
extern JoinTable * jTable;
int yyerror(char* msg);
extern char* queryString;
static int my_yyaccept();
BaseSelection* withPredicate;
BaseSelection* havingPredicate;
string* sortOrdering;

%}
%union{
     string* stringLit;
	int integer;
	double real;
	BaseSelection* sel;
	ConstantSelection* constantSel;
	List<BaseSelection*>* selList;
	List<ConstantSelection*>* constList;
	List<TableAlias*>* tableList;
	TableAlias* tabAlias;
	List<string*>* listOfStrings;
	ParseTree* parseTree;
}
%token <integer> INT
%token <real> DOUBLE
%token TYPE_CHECK
%token SELECT
%token FROM
%token AS
%token WHERE
%token SEQUENCE
%token GROUP 
%token ORDER 
%token BY 
%token HAVING 
%token ASC
%token DESC
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
%token MATERIALIZE
%token ADD
%token <stringLit> STRING_CONST
%left UNION
%left '.'
%left OR
%left AND
%left '='
%left <stringLit> LESSGREATER
%left '-' '+'
%left '*' '/'
%left <stringLit> STRING 
%type <stringLit> JoinString 
%type <sel> selection
%type <integer> optShiftVal 
%type <selList> listOfSelections
%type <selList> listOfSelectionsOrStar
%type <constList> listOfConstants
%type <selList> optOverClause
%type <sel> optWithClause
%type <sel> optHavingClause
%type <selList> optGroupByClause
%type <selList> optOrderByClause
%type <stringLit> optOrdering
%type <tableList> listOfTables
%type <tabAlias> tableAlias
%type <tableList> JoinList 
%type <sel> optWhereClause
%type <sel> predicate
%type <constantSel> constant
%type <selList> optSequenceByClause
%type <stringLit> index_name
%type <listOfStrings> table_name
%type <stringLit> optIndType
%type <listOfStrings> keyAttrs
%type <listOfStrings> optIndAdd 
%type <listOfStrings> listOfStrings 
%type <parseTree> query
%%
input : query {
		parseTree = $1;
		return my_yyaccept();
	}
	| TYPE_CHECK query {
		parseTree = $2;
		parseTree->setTypeCheckOnlyFlag();
		return my_yyaccept();
	}
	| definition
	;
definition: CREATE optIndType INDEX index_name ON table_name 
	'(' keyAttrs ')' optIndAdd {
		parseTree = new IndexParse($2, $4, $6, $8, $10);
		YYACCEPT;
		// return my_yyaccept();  
		// this does not work unless the last argument is optional
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
		parseTree = new ISchemaParse($2);
		YYACCEPT;
	}
	| MATERIALIZE table_name {
		parseTree = new MaterializeParse($2);
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
		$$ = new List<string*>;
	}
	;
constant :
	STRING_CONST {
		$$ = new ConstantSelection("string", strdup($1->c_str()));
	}
	| INT {
		$$ = new ConstantSelection("int", (Type*) $1);
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
		$$ = new List<string*>;
		$$->append($2);
	}
	| '.' {
		$$ = new List<string*>;
	}
	;
query : SELECT listOfSelectionsOrStar 
		FROM listOfTables optWhereClause 
		optGroupByClause optSequenceByClause optOrderByClause {
		$$ = new QueryTree($2,$4,$5,$6,havingPredicate, 
			$7,withPredicate,$8,sortOrdering,namesToResolve);
	}
	| query UNION query {
		$$ = new UnionParse($1, $3);
	}
     ;
listOfSelectionsOrStar : listOfSelections {
		$$ = $1;
	}
	| '*' {
		$$ = NULL;
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
		$$ = new List<string*>;
		$$->append($1);
	}
	| {
		$$ = new List<string*>;
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
	| WHERE {
		$$ = NULL;
	}
	| {
		$$ = NULL;
	}
	;

optSequenceByClause :SEQUENCE BY listOfSelections optWithClause {
		$$ = $3;
		withPredicate = $4;
	}
	| {
		$$ = NULL;
		withPredicate = NULL;
	}
	;
optWithClause: WITH predicate {
		$$ = $2;
	}
	|{
		$$ = NULL;
	}
	;
optGroupByClause: GROUP BY listOfSelections optHavingClause{
		$$ = $3;
		havingPredicate = $4;
	}
	|{
		$$ = new List<BaseSelection*>; 
		havingPredicate = NULL;
	}
	;
	
optHavingClause: HAVING predicate {
		$$ = $2;
	}
	|{
		$$ = NULL;
	}
	;

optOrderByClause: ORDER BY listOfSelections optOrdering{
		$$ = $3;
		sortOrdering = $4;
	}
	|{
		$$ = new List<BaseSelection*>;
		sortOrdering = NULL;
	}
	;
optOrdering: ASC {
		$$ = new string("ascending");
	}
	| DESC {
		$$ = new string("descending");
	} 
	| {
		$$ = new string("ascending");
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
     | predicate '-' predicate {
          $$ = new Operator("-", $1, $3);
	}
     | predicate '*' predicate {
          $$ = new Operator("*", $1, $3);
	}
     | predicate '/' predicate {
          $$ = new Operator("/", $1, $3);
	}
	| predicate STRING predicate {
		$$ = new Operator(*$2, $1, $3);
	}
	| selection
	;

selection :
	STRING '.' STRING {
		$$ = new PrimeSelection($1, $3);
	}
	|
	selection '.' STRING {
		$$ = new Member($3, $1);
	}
	| STRING '(' listOfSelections ')' optOverClause {
		assert($3);
		if ($5 != NULL){
			$3->addList($5);
		}
		$$ = new Constructor($1, $3);
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
		$$ = new string("joinprev");
	}
	| JOINNEXT {
		$$ = new string("joinnext");
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
            string* tmp = new string(*$1->get());
            $$ = new TableAlias(new TableName($1), tmp);
        }
        else{
            string msg = "Sorry, you need to specify alias";
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
