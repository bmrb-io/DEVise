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
  Revision 1.44  1998/10/16 20:57:32  beyer
  parser now accepts negative numbers and unary minus (I hope!)

  Revision 1.43  1998/10/01 21:00:31  yunrui
  Add Gestalt stuff

  Revision 1.42  1998/06/28 21:47:45  beyer
  major changes to the interfaces all of the execution classes to make it easier
  for the plan reader.

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
//#include "queue.h"
#include "ParseTree.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteBoolAdt.h"
#include "DTE/util/Del.h"
//#include <iostream.h>    erased for sysdep
#include <assert.h>
#include <stdlib.h>

#include "sysdep.h"

extern int yylex();
extern ParseTree* globalParseTree;
int yyerror(char* msg);
string* sortOrdering;

#if defined(_WINDOWS) || defined(_CONSOLE)
#define alloca _alloca
#endif

typedef Del<string> SDel;
typedef Del<TableName> TabDel;

%}
%union{
     string* stringLit;
	int integer;
	OptExpr* sel;
        OptExprList* selList;
	vector<TableAlias*>* tableList;
	TableAlias* tabAlias;
	vector<string>* listOfStrings;
	ParseTree* parseTree;
	vector<IdentType>* ident_type_vec;
	IdentType* ident_type;
        TableName* tableName;
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
%token GESTALT
%token REGISTER
%token UNREGISTER
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
%nonassoc UMINUS
%left <stringLit> STRING 

// %type <stringLit> JoinString 
// %type <selList> optOverClause

%type <sel> selection
// %type <integer> optShiftVal 
%type <selList> listOfSelections
%type <selList> listOfSelectionsOrStar
%type <sel> optWithClause
%type <sel> optHavingClause
%type <selList> optGroupByClause
%type <selList> optOrderByClause
%type <stringLit> optOrdering
%type <tableList> listOfTables
%type <tabAlias> tableAlias
%type <sel> optWhereClause
%type <sel> predicate
%type <sel> constant
%type <selList> optSequenceByClause
%type <stringLit> index_name
%type <tableName> table_name
%type <listOfStrings> table_path
%type <stringLit> index_type
%type <listOfStrings> keyAttrs
%type <listOfStrings> optIndAdd 
%type <listOfStrings> listOfStrings 
%type <parseTree> sql query definition
%type <ident_type_vec> ident_type_pairs
%type <ident_type> ident_type_pair

%%
input : sql opt_semicolon {
                globalParseTree = $1;
                if(yychar == YYEOF || yychar == YYEMPTY) {
                  YYACCEPT;
                } else {
                  extern char* yytext;
                  cerr << "extra characters at end of query:";
                  cerr << "text: " << yytext << endl;
                  YYABORT;
                }
        }
        ;

opt_semicolon: /* empty */ | ';' ;

sql:      query
        | definition
        ;

definition:
          CREATE index_type INDEX index_name ON table_name 
	  '(' keyAttrs ')' optIndAdd {
                bool includePointer = true;
		$$ = new IndexParse($2, $4, $6, $8, includePointer, $10);
	}
	| CREATE TABLE '(' ident_type_pairs ')' {
		$$ = new CreateTableParse($4);
	}
	| CREATE GESTALT table_name '(' ident_type_pairs ')' STRING {
		$$ = new CreateGestaltParse($3, $5, $7);
	}
	| REGISTER table_name INTO GESTALT table_name {
		$$ = new RegisterIntoGestaltParse($5, $2);
	}
	| UNREGISTER table_name FROM GESTALT table_name {
		$$ = new UnregisterFromGestaltParse($5, $2);
	}
	| DROP INDEX table_name index_name {
		$$ = new DropIndexParse($3, $4);
	}
	| INSERT INTO table_name VALUES '(' listOfSelections ')' {
		$$ = new InsertParse($3, $6);
	}
	| DELETEY FROM tableAlias optWhereClause {
		$$ = new DeleteParse($3, $4);
	}
	| SCHEMA table_name {
		$$ = new ISchemaParse($2);
	}
	| MATERIALIZE table_name {
		$$ = new MaterializeParse($2);
	}
	| CREATE ODBCTABLE STRING ',' STRING ',' STRING {
		$$ = new ODBCTableAddParse($3,$5,$7);
	}
	| CREATE ODBCDSN STRING ',' STRING_CONST {
		$$ = new ODBCDSNAddParse($3,$5);
	}
	| DELETEY ODBCTABLE STRING {
		$$ = new ODBCTableDeleteParse($3);
		YYACCEPT;
	}
	| DELETEY ODBCDSN STRING {
		$$ = new ODBCDSNDeleteParse($3);
		YYACCEPT;
	}
	| DSNENTRIES {
		$$ = new DSNEntriesParse();
		YYACCEPT;
	}
	| ODBCTABLELIST STRING {
		$$ = new ODBCTablesParse($2);
		YYACCEPT;
	}
	;

keyAttrs : listOfStrings
	;

ident_type_pairs : ident_type_pairs ',' ident_type_pair {
		$1->push_back(*Del<IdentType>($3));
		$$ = $1;
	}
	| ident_type_pair {
		$$ = new vector<IdentType>;
		$$->push_back(*Del<IdentType>($1));
	}
	;
ident_type_pair : STRING STRING {
		$$ = new IdentType(*SDel($1), *SDel($2));
	}
	;
index_type: STRING { $$ = $1; }
	;
optIndAdd: ADD '(' listOfStrings ')' {
		$$ = $3;
	}
	| {
		$$ = new vector<string>;
	}
	;
constant :
	STRING_CONST {
          SDel str($1);
          Type* value = DteStringAdt::allocateCopy(*str);
          $$ = new OptConstant(new DteStringAdt(str->length()), value);
	}
	| INTY {
          Type* value = DteIntAdt::allocateCopy($1);
          $$ = new OptConstant(new DteIntAdt, value);
	}
	| DOUBLEY {
          SDel str($1);
          float8 x = atof(str->c_str());
          Type* value = DteDoubleAdt::allocateCopy(x);
          $$ = new OptConstant(new DteDoubleAdt, value);
	}
	;
index_name : STRING
	;

table_name : table_path { $$ = new TableName($1); }
        ;

table_path : table_path '.' STRING {
		$1->push_back(*SDel($3));
		$$ = $1;
	}
	| '.' STRING {
		$$ = new vector<string>;
		$$->push_back(*SDel($2));
	}
	| '.' {
		$$ = new vector<string>;
	}
	;
query : SELECT listOfSelectionsOrStar FROM listOfTables optWhereClause 
	optGroupByClause optSequenceByClause optWithClause optHavingClause
        optOrderByClause {
		$$ = new QueryTree($2, $4, $5, $6, $7, $8, $9, $10,
                                   sortOrdering);
	}
	| query UNION query {
		$$ = new UnionParse($1, $3);
	}
     ;
listOfSelectionsOrStar : listOfSelections {
		$$ = $1;
	}
	| '*' {
		$$ = new OptExprList;
	}
	;
listOfSelections : listOfSelections ',' predicate {
		$1->push_back($3);
		$$ = $1;
	}
	| predicate {
		$$ = new OptExprList;
		$$->push_back($1);
	}
 /* kb: why can listOfSelections be empty?
	| {
		$$ = new OptExprList;
	}
 */
     ;
listOfStrings : listOfStrings ',' STRING {
		$1->push_back(*SDel($3));
		$$ = $1;
	}
	| STRING {
		$$ = new vector<string>;
		$$->push_back(*SDel($1));
	}
	| {
		$$ = new vector<string>;
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
 /*
	| WHERE {               // kb: why is this legal?
		$$ = NULL;
	}
 */
	| {
		$$ = NULL;
	}
	;

optGroupByClause: GROUP BY listOfSelections {
		$$ = $3;
	}
	|{
		$$ = new OptExprList; 
	}
	;
	
optSequenceByClause :SEQUENCE BY listOfSelections {
		$$ = $3;
	}
	| {
		$$ = new OptExprList;
	}
	;

optWithClause: WITH predicate {
		$$ = $2;
	}
	|{
		$$ = NULL;
	}
	;

optHavingClause: HAVING predicate {
		$$ = $2;
	}
	|{
		$$ = NULL;
	}
	;

 //kb: fix order by: should have asc/desc per attr, only column names or pos.
optOrderByClause: ORDER BY listOfSelections optOrdering{
		$$ = $3;
		sortOrdering = $4;
	}
	|{
		$$ = new OptExprList;
		//sortOrdering = NULL;
		sortOrdering = new string("na");
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
          $$ = new OptOr($1, $3);
	}
	| predicate AND predicate { //kb: need NOT
		$$ = new OptAnd($1, $3);
	}
	| predicate '=' predicate {
        $$ = new OptFunction("=", $1, $3);
	}
	| predicate LESSGREATER predicate {
		$$ = new OptFunction(*$2, $1, $3);
	}
        | predicate '+' predicate {
          $$ = new OptFunction("+", $1, $3);
	}
        | predicate '-' predicate {
          $$ = new OptFunction("-", $1, $3);
	}
        | predicate '*' predicate {
          $$ = new OptFunction("*", $1, $3);
	}
        | predicate '/' predicate {
          $$ = new OptFunction("/", $1, $3);
	}
        | '-' predicate %prec UMINUS {
          //kb: simplification should be done after parsing on all
          //kb: constant expressions, not just uminus
          bool simplified = false;
          OptExpr* oe = $2;
          if( oe->getExprType() == OptExpr::CONST_ID ) { // simplify
              OptConstant* ce = (OptConstant*)oe;
              int t = ce->getAdt().getTypeID();
              if( t == DteIntAdt::typeID ) {
                // cheat and muck with the actual value
                int4& x = DteIntAdt::cast((Type*)ce->getValue());
                x = -x;
                $$ = ce;
                simplified = true;
              } else if( t == DteDoubleAdt::typeID ) {
                float8& x = DteDoubleAdt::cast((Type*)ce->getValue());
                x = -x;
                $$ = ce;
                simplified = true;
              }
          }
          if( !simplified ) {
            //kb: hack alert! use binary minus for unary minus with int zero
            Type* zeroVal = DteIntAdt::allocateCopy(0);
            OptConstant* zero = 
              new OptConstant(new DteIntAdt, zeroVal);
            $$ = new OptFunction("-", zero, oe);
          }
	}
        | '+' predicate %prec UMINUS {
          $$ = $2;
	}
	| predicate STRING predicate {
		$$ = new OptFunction(*SDel($2), $1, $3);
	}
	| selection
	;

selection :
	STRING {
		$$ = new OptField("", *SDel($1));
	}
	| STRING '.' STRING {
		$$ = new OptField(*SDel($1), *SDel($3));
	}
//	|
//	selection '.' STRING {
//		$$ = new Member(*$3, $1); delete $3;
//	}
//	| STRING '(' listOfSelections ')' optOverClause {
	| STRING '(' listOfSelections ')' {
//		if ($5 != NULL){
//			$3->addList($5);
//		}
		$$ = new OptFunction(*SDel($1), *Del<OptExprList>($3));
	}
	| STRING '(' ')' {
		$$ = new OptFunction(*SDel($1), OptExprList());
	}
	| STRING '(' '*' ')' { // eg, count(*)
                // add dummy field
                Type* value = DteBoolAdt::allocateCopy(true);
                OptExprList e(1);
                e[0] = new OptConstant(new DteBoolAdt(), value);
		$$ = new OptFunction(*SDel($1), e);
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

tableAlias : table_name AS STRING {
		$$ = new TableAlias(*TabDel($1), *SDel($3));
	}
	| table_name {
                TabDel tab($1);
                $$ = new TableAlias(*tab, tab->fileName());
	}
	| DOUBLEY AS STRING {	// this maches int.int
                SDel str($1);
		int dotPos = str->find('.');
		int serv = atoi(str->substr(0, dotPos).c_str());
		int loc = atoi(str->substr(dotPos + 1).c_str());
                RelationId relId(serv, loc);
		$$ = new NumberAlias(relId, *SDel($3));
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
int yyerror(char* msg)
{
        cerr << msg;
	return 0;
}

