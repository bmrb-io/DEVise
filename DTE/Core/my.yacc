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
  Revision 1.41  1998/06/24 05:03:00  okan
  Added ODBC List & DSN List commands
  Added insert & delete commands to update ODBC entries in related catalogs

  Revision 1.40  1998/03/17 17:19:08  donjerko
  Added new namespace management through relation ids.

  Revision 1.39  1998/03/12 18:23:29  donjerko
  *** empty log message ***

  Revision 1.38  1998/02/09 21:12:21  donjerko
  Added Bin by clause and implementation.

  Revision 1.37  1997/12/04 04:05:19  donjerko
  *** empty log message ***

  Revision 1.36  1997/11/18 19:49:21  okan
  Made several changes for NT compilation

  Revision 1.35  1997/11/14 22:37:20  donjerko
  *** empty log message ***

  Revision 1.34  1997/11/08 21:02:29  arvind
  Completed embedded moving aggregates: mov aggs with grouping.

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
//#include <iostream.h>    erased for sysdep
#include <assert.h>
#include <stdlib.h>

#include "sysdep.h"

extern int yylex();
extern ParseTree* globalParseTree;
int yyerror(char* msg);
extern char* queryString;
static int my_yyaccept();
BaseSelection* withPredicate;
BaseSelection* havingPredicate;
string* sortOrdering;

#if defined(_WINDOWS) || defined(_CONSOLE)
#define alloca _alloca
#endif

%}
%union{
     string* stringLit;
	int integer;
	BaseSelection* sel;
	ConstantSelection* constantSel;
	vector<BaseSelection*>* selList;
	List<ConstantSelection*>* constList;
	vector<TableAlias*>* tableList;
	TableAlias* tabAlias;
	List<string*>* listOfStrings;
	ParseTree* parseTree;
	vector<IdentType*>* ident_type_vec;
	IdentType* ident_type;
}
%token <integer> INTY
%token <stringLit> DOUBLEY
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
%token ODBCTABLE
%token ODBCDSN
%token ODBCTABLELIST
%token DSNENTRIES
%token TABLE
%token DROP
%token INDEX
%token ON
%token OVER 
%token WITH 
%token INSERT
%token INTO
%token VALUES
%token DELETEY
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

// %type <stringLit> JoinString 
// %type <selList> optOverClause

%type <sel> selection
%type <integer> optShiftVal 
%type <selList> listOfSelections
%type <selList> listOfSelectionsOrStar
%type <constList> listOfConstants
%type <sel> optWithClause
%type <sel> optHavingClause
%type <selList> optGroupByClause
%type <selList> optOrderByClause
%type <stringLit> optOrdering
%type <tableList> listOfTables
%type <tabAlias> tableAlias
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
%type <ident_type_vec> ident_type_pairs
%type <ident_type> ident_type_pair
%%
input : query {
		globalParseTree = $1;
		return my_yyaccept();
	}
	| definition
	;
definition: CREATE optIndType INDEX index_name ON table_name 
	'(' keyAttrs ')' optIndAdd {
		globalParseTree = new IndexParse($2, $4, $6, $8, $10);
		YYACCEPT;
		// return my_yyaccept();  
		// this does not work unless the last argument is optional
	}
	| CREATE TABLE '(' ident_type_pairs ')' {
		globalParseTree = new CreateTableParse($4);
		YYACCEPT;
	}
	| DROP INDEX table_name index_name {
		globalParseTree = new DropIndexParse($3, $4);
		YYACCEPT;
	}
	| INSERT INTO table_name VALUES '(' listOfConstants ')' {
		globalParseTree = new InsertParse($3, $6);
		YYACCEPT;
	}
	| DELETEY table_name AS STRING WHERE predicate {
		globalParseTree = new DeleteParse($2, $4, $6);
		YYACCEPT;
	}
	| SCHEMA table_name {
		globalParseTree = new ISchemaParse($2);
		YYACCEPT;
	}
	| MATERIALIZE table_name {
		globalParseTree = new MaterializeParse($2);
		YYACCEPT;
	}
	| CREATE ODBCTABLE STRING ',' STRING ',' STRING {
		globalParseTree = new ODBCTableAddParse($3,$5,$7);
		YYACCEPT;
	}
	| CREATE ODBCDSN STRING ',' STRING_CONST {
		globalParseTree = new ODBCDSNAddParse($3,$5);
		YYACCEPT;
	}
	| DELETEY ODBCTABLE STRING {
		globalParseTree = new ODBCTableDeleteParse($3);
		YYACCEPT;
	}
	| DELETEY ODBCDSN STRING {
		globalParseTree = new ODBCDSNDeleteParse($3);
		YYACCEPT;
	}
	| DSNENTRIES {
		globalParseTree = new DSNEntriesParse();
		YYACCEPT;
	}
	| ODBCTABLELIST STRING {
		globalParseTree = new ODBCTablesParse($2);
		YYACCEPT;
	}
	;
keyAttrs : listOfStrings
	;
ident_type_pairs : ident_type_pairs ',' ident_type_pair {
		$1->push_back($3);
		$$ = $1;
	}
	| ident_type_pair {
		$$ = new vector<IdentType*>;
		$$->push_back($1);
	}
	;
ident_type_pair : STRING STRING {
		$$ = new IdentType($1, $2);
	}
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
	| INTY {
		$$ = new ConstantSelection("int", (Type*) $1);
	}
	| DOUBLEY {
		IDouble* idouble = new IDouble(atof($1->c_str()));
		$$ = new ConstantSelection("double", idouble);
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
		bool isSelectStar = false;
		if($2 == NULL){
			$2 = new vector<BaseSelection*>;
			isSelectStar = true;
		}
		$$ = new QueryTree($2,$4,$5,$6,havingPredicate, $7,
			withPredicate, $8, sortOrdering, isSelectStar);
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
		$1->push_back($3);
		$$ = $1;
	}
	| predicate {
		$$ = new vector<BaseSelection*>;
		$$->push_back($1);
	}
	| {
		$$ = new vector<BaseSelection*>;
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
listOfTables : listOfTables ',' tableAlias {
		$1->push_back($3);
		$$ = $1;
	}
	| tableAlias {
		$$ = new vector<TableAlias*>;
		$$->push_back($1);
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
		$$ = new vector<BaseSelection*>;
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
		$$ = new vector<BaseSelection*>; 
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
		$$ = new vector<BaseSelection*>;
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
//	| STRING '(' listOfSelections ')' optOverClause {
	| STRING '(' listOfSelections ')' {
		assert($3);
//		if ($5 != NULL){
//			$3->addList($5);
//		}
		$$ = new Constructor($1, $3);
	}
	| constant {
	}
	| '(' predicate ')' {
		$$ = $2;
	}
	;

/*

// not supported any more

optOverClause:	
	OVER '(' listOfSelections ')' {
			$$ = $3;
	}
	| {
		$$ = NULL;
	}
	;

JoinString : JOINPREV {
		$$ = new string("joinprev");
	}
	| JOINNEXT {
		$$ = new string("joinnext");
	}
	;
*/

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
	| DOUBLEY AS STRING {	// this maches int.int
		int dotPos = $1->find('.');
		int serv = atoi($1->substr(0, dotPos).c_str());
		int loc = atoi($1->substr(dotPos + 1).c_str());
		$$ = new NumberAlias(serv, loc, $3);
	}
	;
optShiftVal: ',' INTY {
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
