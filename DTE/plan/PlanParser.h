#ifndef _PLAN_PARSER_H_
#define _PLAN_PARSER_H_

// (C) 1998- by Kevin S. Beyer

#include <unistd.h>
#include <assert.h>
#include <iostream.h>
#include <strstream.h>

#define YYSTYPE PlanParser::yystype

#if !defined(yyFlexLexer)
#define yyFlexLexer PlanFlexLexer
#include <FlexLexer.h>
#endif

#include "DTE/proc/DteProc.h"
#include <map>
#include <string>

extern int yydebug;

#if 0
class thread_page_pipe_t;

class pipe_map_t
  : private map< string, thread_page_pipe_t* >
{
public:
  typedef map< string, thread_page_pipe_t* > parent;

  pipe_map_t() {}

  ~pipe_map_t() {
    clear();
  }

  void clear() {
    iterator i;
    for(i = begin() ; i != end() ; i++) {
      delete i->second;
    }
    parent::clear();
  }


  thread_page_pipe_t& find(const string& name) {
    iterator i = parent::find(name);
    if( i == end() ) {
      cerr << "pipe not defined: " << name << endl;
      exit(1);
    }
    assert(i->second != NULL);
    return *(i->second);
  }

  void insert(const string& name, thread_page_pipe_t* pipe) {
    iterator i = parent::find(name);
    if( i != end() ) {
      cerr << "pipe already defined: " << name << endl;
      exit(1);
    }
    parent::insert(value_type(name,pipe));
  }

private:
  pipe_map_t(const pipe_map_t& x);
  pipe_map_t& operator=(const pipe_map_t& x);
};
#endif




class PlanParser
  : protected PlanFlexLexer
{
public:

  union yystype {
    // int	tok_val;
    int	int_val;
    double  double_val;
    char*	string_val;
    bool    bool_val;
    //int     date_val;  // kb: fix this

    class Iterator* iter;
    class RandomAccessIterator* random_iter;
    void* type_ptr;
    class DteTupleAdt* tupAdt;
    class DteAdt* adt;
    class Field* field;
    vector<class Field>* field_list;
    //class FieldList* field_list;
    class ExecExpr* expr;
    vector<class ExecExpr*>* expr_list;
    //class ExprList* expr_list;
    vector<class ExecAgg*>* agg_list;
    class ExecAgg* agg_fn;
    class BoundingBox* bbox;
    class Range* range;
    vector<int>* int_list;
    vector<bool>* bool_list;
    class thread_page_pipe_t* pipe;
    class SrqlAgg* srql_agg;
  };

  PlanParser(istream* ins = 0, ostream* outs = 0) 
    : PlanFlexLexer(ins, outs) { quit = false; }

  ~PlanParser() {}

  int parse() {
    _line_nr = 1;
    _plan_steps.clear();
    //_pipe_map.clear();
    return yyparse();
  }

  void debug() { yydebug = 1; }
  void nodebug() { yydebug = 0; }

  // this should be private...
  DteProcList _plan_steps;

  bool quitRequested() { return quit; }

protected:

  int _line_nr;

  ostrstream _str;

  //pipe_map_t _pipe_map;

  bool quit;

  vector<Iterator*> iterStack;
  void pushIter(Iterator* i) {
    iterStack.push_back(i);
  }
  Iterator* topIter() {
    assert( !iterStack.empty() && "not enough input iterators!");
    Iterator* i = iterStack.back();
    return i;
  }
  Iterator* nthIter(int n) {
    int numIters = iterStack.size();
    assert( numIters >= n && "not enough input iterators!");
    Iterator* i = iterStack[numIters - n];
    return i;
  }
  Iterator* popIter() {
    Iterator* i = topIter();
    iterStack.pop_back();
    return i;
  }
#if 0
  RandomAccessIterator* popRandIter() {
    Iterator* i = popIter();
    assert( i->GetIteratorType() == Iterator::RANDOM_ITERATOR );
    return static_cast<RandomAccessIterator*>(i);
  }
#endif

  //int yylex();

  int yylex(yystype*); // { yylex(); } // parm == &yylval

  int yyparse();

  void yyerror(char* msg) {
    cerr << _line_nr << ": " << msg << " at " << YYText() << '\n';
  }

private:

  // N/A
  PlanParser(const PlanParser& other);
  PlanParser& operator=(const PlanParser& other);
};


#endif // _PLAN_PARSER_H_
