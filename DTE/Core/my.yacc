%{

#include "myopt.h"
#include "queue.h"
#include <iostream.h>
#include <String.h>
#include <assert.h>
#include <stdio.h>
extern int yylex();
extern List<BaseSelection*>* selectList;
extern List<TableAlias*>* tableList;
extern BaseSelection* predicates;
extern List<String*>* namesToResolve;
int yyerror(char* msg);
%}
%union{
     String* string;
	int integer;
	BaseSelection* sel;
	Path* path;
	List<BaseSelection*>* selList;
	List<TableAlias*>* tableList;
	TableAlias* tabAlias;
}
%token <integer> INT
%token SELECT
%token FROM
%token WHERE
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
%%
query : SELECT listOfSelections FROM listOfTables optWhereClause ';' {
		selectList = $2;
		tableList = $4;
		predicates = $5;
		return 0;
	}
	| SELECT '*' FROM listOfTables optWhereClause ';' {
		selectList = NULL;
		tableList = $4;
		predicates = $5;
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
		String* dummy = new String;
		namesToResolve->append(dummy); 
		$$ = new PrimeSelection(dummy, new Path($1, NULL));
	}
	| STRING_CONST {
		$$ = new StringConstant($1);
	}
	| INT {
		$$ = new IntegerConstant($1);
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
