%{

  /* (C) 1998- by Kevin S. Beyer */

#include "PlanParser.h"
#define yyparse PlanParser::yyparse


#include "types.h"
#include "ExecOp.h"
#include "StandardRead.h"
  //#include "sysdep.h"
  //#include "PQueue.h"             // for SortOrder
#include "Aggregates.h"
//#include "SrqlExec.h"
#include "Sort.h"
#include "DataRead.h"
#include "DataReader.h"
#include "RTreeRead.h"
#include "DTE/proc/RadixCube.h"

  //#include "dte_tuple_stream.h"
#include "DTE/util/Del.h"
#include "DTE/plan/SrqlExec.h"           // move this
#include "DTE/plan/HeapReader.h"         // move this


class dstring : public string
{
public:
  dstring(char* cp) : string(cp) { delete [] cp; }
};

%}

%pure_parser

%token RUN QUIT
%token TEMPFILE FILE_t DELETE
%token PIPE PRINT CONSUME WRITE
%token WRITEPIPE LOADPIPE
%token SEND PLOT CREATE RTREE RADIXCUBE CHISQUARED
%token SELPROJ TABLELOOKUP SORT UNIQUESORT UNION RIDADDER
%token SIMPLEREADER DATAREADER HEAPREADER
%token RTREELOOKUP
%token NLJOIN MERGEJOIN MERGEJOINDEDUPRIGHT 
%token BNLJOIN SIMPLEHASHJOIN
%token AGG SORTEDGROUPAGG
%token HASHGROUPAGG 
%token SEQUENCE SHIFT_RELATIVE SHIFTALL_RELATIVE 
%token SHIFT_ABSOLUTE SHIFTALL_ABSOLUTE SEQAGG
%token ASC DESC DEDUP
%token VAR
%token<string_val> ID STRING_VAL
%token<int_val> INT_VAL
%token<double_val> DOUBLE_VAL
%token<bool_val> BOOL_VAL
%token INNER_JOIN_TOK LEFT_OUTER_JOIN_TOK 
%token RIGHT_OUTER_JOIN_TOK FULL_OUTER_JOIN_TOK

%type <iter> iterator join_method aggregate srql
//%type <iter> index_lookup join_method
%type <random_iter> random_access
%type <field_list> field_list opt_field_list 
%type <bool_list> sort_order sort_order_list
%type <bool_val> sort_order_aux
%type <bool_val> opt_dedup
%type <agg_list> agg_list agg_list_aux
%type <agg_fn> agg_fn
%type <field> field
%type <field_list> field_list field_list_aux
%type <int_val> offset offset_field root_page pages /* win_low win_high */
%type <string_val> filename schemafile datafile pipe_id
//%type <bbox> bounding_box bounding_box_aux
//%type <range> range       
%type <tupAdt> added_types type_list type_list_aux opt_type_list
%type <adt> type
%type <expr> expr const_expr
%type <expr_list> project where compare expr_list opt_expr_list
//%type <type_ptr> constant
%type <int_list> int_list int_list_aux opt_int_list
%type <int_list> pos_list pos_list_aux opt_pos_list key_pos_list add_pos_list
%type <int_list> group_fields seq_fields
%type <int_val> opt_join_style join_style
//%type <pipe> pipe

%%


plan:
          plan_list ';' RUN { YYACCEPT; }
        | plan_list RUN     { YYACCEPT; }
        | /* empty */       { YYACCEPT; }
        | QUIT              { quit = true; YYACCEPT; }
        ;


plan_list:
          plan_step
        | plan_list ';' plan_step
        ;
        
plan_step1:
          plan_step { assert( iterStack.empty() && "extra iterators on stack!"); }

plan_step:
          iterator PRINT
          { _plan_steps.push_back(new PrintIteratorProc($1)); }
        | iterator /* PRINT */ 
          { _plan_steps.push_back(new PrintIteratorProc($1)); }
        | iterator CONSUME
          { _plan_steps.push_back(new ConsumeIteratorProc($1)); }
        | iterator WRITE filename
          { _plan_steps.push_back(new WriteIteratorProc($1,dstring($3))); }
        | iterator RADIXCUBE int_list INT_VAL INT_VAL opt_dedup
          { RadixCubeProc* p = new RadixCubeProc($1, *$3, $4, $5, $6);
            _plan_steps.push_back(p);
            delete $3;
          }
/*
        | PIPE pipe_id '=' pipe
          { dstring pipe_name($2); _pipe_map.insert(pipe_name, $4); }
        | WRITEPIPE '(' filename ',' pipe ')'
          { dstring filename($3);
            _plan_steps.push_back(new write_pipe_t(*$5, filename));
          }
        | SEND '(' INT_VAL ',' iterator ')'
          { extern int send_fd; extern mql_mutex_t send_mutex;
            _plan_steps.push_back( 
                new send_proc_t($5, $3, send_fd, send_mutex));
          }
        | PLOT '(' INT_VAL ',' INT_VAL ',' INT_VAL ','
                   DOUBLE_VAL ',' DOUBLE_VAL ',' 
                   DOUBLE_VAL ',' DOUBLE_VAL ',' 
                   iterator ')'
          { extern int send_fd; extern mql_mutex_t send_mutex;
            _plan_steps.push_back( 
                new plot_proc_t($17, $3, send_fd, send_mutex,
                                $5, $7, $9, $11, $13, $15));
          }
        | CREATE RTREE filename key_pos_list add_pos_list iterator
          { build_rtree_proc_t* p
                = new build_rtree_proc_t(dstring($3), $4, $5, $6);
            _plan_steps.push_back(p);
          }
        | CHISQUARED '(' iterator ',' INT_VAL ',' INT_VAL ')'
          { ChiSquaredProc* p = new ChiSquaredProc($3, $5, $7);
            _plan_steps.push_back(p);
          }
*/
        ;

/*
pipe:
          LOADPIPE '(' pages ',' iterator ')'
          { thread_page_pipe_t* pipe = new thread_page_pipe_t(
                    convert_to_mql_types($5->getTypes()), $3 );
            dte_pipe_t* dte_pipe = new dte_pipe_t($5, *pipe);
            _plan_steps.push_back(dte_pipe);
            $$ = pipe;
          }
        | LOADPIPE '(' pages ',' type_list ',' filename ')'
          { int pages = $3;
            Del<TypeIDList> types = $5;
            dstring filename($7);
            thread_page_pipe_t* pipe
              = new thread_page_pipe_t(convert_to_mql_types(*types), pages);
            load_pipe_t* load_pipe = new load_pipe_t(filename, *pipe);
            _plan_steps.push_back(load_pipe);
            $$ = pipe;
          }
        | MERGEJOIN '(' pages ',' iterator ',' random_access ','
                        compare ',' project opt_join_style ')'
          { MergeJoinProc* p = new MergeJoinProc($3, $5, $7, $9, $11,
                                                 DteJoinStyle($12));
            _plan_steps.push_back(p);
            $$ = p->getOutputPipe();
          }
        | pipe_id
          { dstring pipe_name($1); $$ = &_pipe_map.find(pipe_name); }
        ;
*/
        

iterator:
          random_access { $$ = $1; }
 /*
        | index_lookup { $$ = $1; }
 */
        | join_method { $$ = $1; }

        | aggregate { $$ = $1; }

        | srql { $$ = $1; }

        | iterator SELPROJ {pushIter($1);} where project
          { popIter(); $$ = new SelProjExec( $1, $4, $5 ); }

        | iterator random_access TABLELOOKUP { pushIter($2); pushIter($1); }
          offset_field where project
          { popIter(); popIter(); 
            $$ = new TableLookupExec( $1, $2, $5, $6, $7 );
          }

        | iterator SORT {pushIter($1);} sort_order opt_dedup
          { popIter();
            $$ = new SortExec($1, *$4, $5);
            delete $4;
          }

        | iterator iterator UNION
          { $$ = new UnionExec( $1, $2 ); }

        | iterator RIDADDER
          { $$ = new RidAdderExec( $1 ); }
/*
        | PIPE '(' pipe ')'
          { $$ = new dte_tuple_stream_reader_t(*$3); }
*/
        ;

random_access:
          SIMPLEREADER type_list datafile
          { $$ = new StandReadExec( *$2, dstring($3) ); delete $2; }
        | DATAREADER schemafile datafile
          { $$ = new DataReadExec(dstring($2), dstring($3)); }
        | HEAPREADER type_list datafile
          { $$ = new HeapReader(*$2, dstring($3)); delete $2; }
        ;

/*
index_lookup:
          RTREELOOKUP '(' filename ',' root_page ',' bounding_box ','
                          added_types ')'
          { $$ = new RTreeReadExec(dstring($3), $5, $7, *$9); delete $9; }
        ;
*/

join_method:
          iterator iterator NLJOIN {pushIter($2);pushIter($1)}
          where project
          { popIter(); popIter(); $$ = new NLJoinExec($1,$2,$5,$6); }

        | iterator random_access MERGEJOIN {pushIter($2);pushIter($1)}
          where where project opt_join_style
          { popIter(); popIter(); 
            $$ = new MergeJoinExec($1, $2, $5, $6, $7, DteJoinStyle($8));
          }

        | iterator iterator MERGEJOINDEDUPRIGHT {pushIter($2);pushIter($1)}
          where where project opt_join_style
          { popIter(); popIter(); 
            $$ = new MergeJoinDedupRightExec($1, $2, $5, $6, $7,
                                             DteJoinStyle($8));
          }
        ;


aggregate:
          iterator AGG {pushIter($1);} agg_list
          { popIter();
            $$ = new StandAggsExec($1,*$4); delete $4; }
        | iterator SORTEDGROUPAGG {pushIter($1);} group_fields agg_list
          {// input must be sorted by group_fields
           popIter();
           $$ = new SortedGroupByExec($1,*$4,*$5); delete $4; delete $5;
          }
        ;


srql:   // for these, input must be sorted by group_fields, seq_fields,
        // and the group-id and ordinal fields (from SEQUENCE)
        // must be the first two fields
          iterator SEQUENCE {pushIter($1);} group_fields seq_fields
          { popIter();
	    $$ = new SrqlSequenceExec($1,*$4,*$5); delete $4; delete $5; }
        | iterator random_access SEQAGG {pushIter($2); pushIter($1);}
	  expr expr agg_fn
          { popIter(); popIter();
	    $$ = new SrqlAggregateExec($1,$2,$5,$6,$7); }
/*
        | SHIFT_RELATIVE '(' iterator ',' offset ')'
          { }
        | SHIFTALL_RELATIVE '(' iterator ',' offset ')'
          { }
        | SHIFT_ABSOLUTE '(' iterator ',' offset ')'
          { }
        | SHIFTALL_ABSOLUTE '(' iterator ',' offset ')'
          { }
*/
        ;

project:        '[' expr_list ']' { $$ = $2; } ;

where:          '[' opt_expr_list ']'  { $$ = $2; } ;

compare:        '[' expr_list ']' { $$ = $2; } ;

group_fields:   opt_pos_list ;

seq_fields:     opt_pos_list ;

sort_order:     '[' sort_order_list ']' { $$ = $2; } ;

sort_order_list:
          sort_order_aux
          { $$ = new vector<bool>(); $$->push_back($1); }
        | sort_order_list ',' sort_order_aux
          { $$ = $1; $$->push_back($3); }
        ;

sort_order_aux:
          ASC   { $$ = true; }
        | DESC  { $$ = false; }
        ;

opt_dedup:
          /* empty */ { $$ = false; }
        | DEDUP       { $$ = true; }
        ;

agg_list:
          '[' agg_list_aux ']' { $$ = $2; } ;

agg_list_aux:
          agg_fn
          { $$ = new ExecAggList; $$->push_back($1); }
        | agg_list_aux ',' agg_fn
          { $$ = $1; $$->push_back($3); }
        ;
       
agg_fn:   ID '(' field ')'
          { $$ = ExecAgg::create(dstring($1), $3->getAdt(), $3->getPos());
            delete $3;
          }
        | ID '(' opt_star ')'
          { $$ = ExecAgg::create(dstring($1), DteTupleAdt(), -1); }
        ;

/*
bounding_box:   '[' bounding_box_aux ']' { $$ = $2; }
          
bounding_box_aux:
          range
          { $$ = new BoundingBox(); $$->push_back($1); }
        | bounding_box_aux ',' range
          { $$ = $1; $$->push_back($3); }
        ;

range:
          type constant ':' constant
          { $$ = new Range($1,$2,$4); }
        | type constant ':' '*'
          { $$ = new Range($1,$2,NULL); }
        | type '*' ':' constant
          { $$ = new Range($1,NULL,$4); }
        | type '*' ':' '*' // -inf to +inf
          { $$ = new Range($1); }
        | type // same as previous
          { $$ = new Range($1); }
        ;
*/

field:
          '#' INT_VAL ':' INT_VAL
          { Iterator* iter = nthIter($2);
            const DteAdt& adt = iter->getAdt().getAdt($4-1);
            //cerr << "got field: " << $2 << ':' << $4 << ' ' << adt.getTypeSpec() << endl;
            $$ = new Field(adt, $2-1, $4-1); }
        | '#' INT_VAL
          { Iterator* iter = topIter();
            const DteAdt& adt = iter->getAdt().getAdt($2-1);
            //cerr << "got field: " << $2 << ' ' << adt.getTypeSpec() << endl;
            $$ = new Field(adt, 0, $2-1); }
        ;

/*
field:
          type '@' INT_VAL ':' INT_VAL
          { $$ = new Field(*$1, $3-1, $5-1); delete $1; }
        | type '@' INT_VAL
          { $$ = new Field(*$1, $3-1); delete $1; }
        ;
*/


opt_field_list:
          '[' field_list_aux ']'  { $$ = $2; }
        | '[' ']'                 { $$ = new FieldList; }
        ;


field_list:     '[' field_list_aux ']'  { $$ = $2; };


field_list_aux:
          field
          { $$ = new FieldList; $$->push_back(*$1); delete $1; }
        | field_list_aux ',' field
          { $$ = $1; $$->push_back(*$3); delete $3; }
        ;


added_types:    opt_type_list ;

opt_type_list:
          type_list
        | '[' ']'
          { $$ = new DteTupleAdt; }
        ;
        

type_list:      '[' type_list_aux ']' { $$ = $2; }

type_list_aux:
          type
          { $$ = new DteTupleAdt; $$->push_back(*$1); delete $1; }
        | type_list_aux ',' type
          { $$ = $1; $$->push_back(*$3); delete $3; }
      ;

 //kb: should types get parsed as more than strings??
type:
          ID            { $$ = DteAdt::createAdt(dstring($1)); }
        | STRING_VAL    { $$ = DteAdt::createAdt(dstring($1)); }
        ;

opt_expr_list:
          /* empty */
          { $$ = new ExprList; }
        | expr_list
        ;

expr_list:
          expr
          { $$ = new ExprList; $$->push_back($1); }
        | expr_list ',' expr
          { $$ = $1; $$->push_back($3); }
        ;


expr:
          ID '(' expr_list ')'
          { $$ = ExecExpr::createFunction(dstring($1), *$3); 
            assert($$ && "failed to get function");
            delete $3; }
        | ID '(' ')'
          { ExecExprList noArgs; //kb: add ExecExpr::createFunction(fn)
            $$ = ExecExpr::createFunction(dstring($1), noArgs);
            assert($$); }
        | const_expr
          { $$ = $1; }
        | field
          { $$ = ExecExpr::createField($1->getAdt(),
                                       $1->isRight(), $1->getPos());
            delete $1; }
        | VAR '(' INT_VAL ')'
          { assert(0); } //MqlVariable* var = _var_map.find($3);
            //assert(var);
            //$$ = new VarExecExpr($3); }
        | '(' expr ')'
          { $$ = $2; }
        ;


const_expr:
          INT_VAL
          { $$ = ExecExpr::createInt4($1); }
        | DOUBLE_VAL
          { $$ = ExecExpr::createFloat8($1); }
        | STRING_VAL
          { $$ = ExecExpr::createString(dstring($1)); }
        | BOOL_VAL
          { $$ = ExecExpr::createBool($1); }
        ;

/*
//kb: fix these
constant:
          INT_VAL
          { $$ = (void*)$1; }
        | DOUBLE_VAL
          { double* d = new double; *d = $1; $$ = d; }
        | STRING_VAL
          { $$ = $1; }
        | BOOL_VAL
          { $$ = $1; }
        ;
*/

int_list:       '[' int_list_aux ']' { $$ = $2; } ;

opt_int_list:
          '[' int_list_aux ']'  { $$ = $2; }
        | '[' ']'               { $$ = new vector<int>; }
        ;

int_list_aux:
          INT_VAL
          { $$ = new vector<int>; $$->push_back($1); }
        | int_list_aux ',' INT_VAL
          { $$ = $1; $$->push_back($3); }
        ;

pos_list:       '[' pos_list_aux ']' { $$ = $2; } ;

opt_pos_list:
          '[' pos_list_aux ']'  { $$ = $2; }
        | '[' ']'               { $$ = new vector<int>; }
        ;

pos_list_aux:
          '#' INT_VAL
          { $$ = new vector<int>; $$->push_back($2-1); }
        | pos_list_aux ',' '#' INT_VAL
          { $$ = $1; $$->push_back($4-1); }
        ;


opt_join_style:
          // empty
          { $$ = DteJoinStyle::INNER; }
        | join_style
          { $$ = $1; }
        ;

join_style:
          INNER_JOIN_TOK        { $$ = DteJoinStyle::INNER; }
        | LEFT_OUTER_JOIN_TOK   { $$ = DteJoinStyle::LEFT_OUTER; }
        | RIGHT_OUTER_JOIN_TOK  { $$ = DteJoinStyle::RIGHT_OUTER; }
        | FULL_OUTER_JOIN_TOK   { $$ = DteJoinStyle::FULL_OUTER; }
        ;

schemafile:     STRING_VAL ;

datafile:       STRING_VAL ;

filename:       STRING_VAL ;

pipe_id:        ID ;

root_page:      INT_VAL ;

offset:         INT_VAL ;

offset_field:   INT_VAL ;

 /* win_low:        INT_VAL ; */

 /* win_high:       INT_VAL ; */

pages:          INT_VAL ;

key_pos_list:   pos_list ;

add_pos_list:   opt_pos_list ;

opt_star:       /* empty */ | '*' ; 

%%
