#include "./ExecExpr.h"
//#include <math.h>

/*
  - function knows its return type only after it is given args
  - some functions depend on type args and some do not.
      - dec add with same prec and scale is faster than diff prec and scale
      - string compare doesn't care

Promotions:
 1. exact match
 2. trivial promotions
    non-null -> nullable
 3. standard promotions
    int4 -> dec<18,0>
 4. conversions
    int4 -> dec<9,0>
    int4 -> float8
*/

#include "DTE/util/DteAlgo.h"
#include "DTE/types/DteTupleAdt.h"
#include "DTE/types/DteBoolAdt.h"
#include "DTE/types/DteIntAdt.h"
#include "DTE/types/DteDoubleAdt.h"
#include "DTE/types/DteStringAdt.h"
#include "DTE/types/DteDateAdt.h"
#include "DTE/Core/MapAttr.h"
#include "DTE/mql/MqlSession.h"

//kb: move these type ids
static const int DteBoolId = DteBoolAdt::typeID;
static const int DteIntId = DteIntAdt::typeID;
static const int DteDoubleId = DteDoubleAdt::typeID;
static const int DteStringId = DteStringAdt::typeID;
static const int DteDateId = DteDateAdt::typeID;


template <class Arg1, class Arg2, class Arg3, class Result>
struct ternary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Arg3 third_argument_type;
    typedef Result result_type;
};      



class ExecExprField
: public ExecExpr
{
protected:

  int slot;

public:

  ExecExprField(const DteAdt& adt, int slot)
    : ExecExpr(adt), slot(slot) {}

  ~ExecExprField() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    return tup1[slot];
  }

private:

  ExecExprField(const ExecExprField& x);
  ExecExprField& operator=(const ExecExprField& x);
};


class ExecExprFieldRight
: public ExecExpr
{
protected:

  int slot;

public:

  ExecExprFieldRight(const DteAdt& adt, int slot)
    : ExecExpr(adt), slot(slot) {}

  ~ExecExprFieldRight() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    return tup2[slot];
  }

private:

  ExecExprFieldRight(const ExecExprFieldRight& x);
  ExecExprFieldRight& operator=(const ExecExprFieldRight& x);
};

template <class T>
struct BucketOp : public ternary_function<T, T, T, int4>
{
	int4 operator()(const T& x, const T& start, const T& step) const {
		// be safe about negative cast to int; assumes step > 0
		if( x > start ) {
			return (int4)( (x-start) / step );
		}
		return -(int4)( (start-x) / step ) - 1;
	}
};

template <class T>
struct BinOp : public ternary_function<T, T, T, T>
{
	T operator()(const T& x, const T& start, const T& step) const {
		// be safe about negative cast to int; assumes step > 0
		if( x > start ) {
			return ((int4)( (x-start) / step )) * step + start;
		}
		return (-(int4)( (start-x) / step ) - 1) * step + start;
	}
};


class ExecExprString
: public ExecExpr
{
protected:

  char* result;
  int resultLength;

public:

  // setResultAdt must be called later
  ExecExprString() { result = NULL; }

  ~ExecExprString() {
    delete [] result;
  }

  // this can only be called once
  void setResultAdt(const DteAdt& resAdt) {
    assert(resAdt.getTypeID() == DteStringId);
    ExecExpr::setResultAdt(resAdt);
    resultLength = resAdt.getMaxSize();
    assert(result == NULL);
    result = new char[resultLength];
  }

private:

  ExecExprString(const ExecExprString& x);
  ExecExprString& operator=(const ExecExprString& x);
};


class ExecExprStringUnary
: public ExecExprString
{
protected:

  ExecExpr* arg;

public:

  ExecExprStringUnary(ExecExprList args) {
    assert( args.size() == 1 );
    arg = args[0];
  }

  ExecExprStringUnary(ExecExpr* arg) {
    this->arg = arg;
  }

  ~ExecExprStringUnary() {
    delete arg;
  }

private:

  ExecExprStringUnary(const ExecExprStringUnary& x);
  ExecExprStringUnary& operator=(const ExecExprStringUnary& x);
};


class ExecExprStringBinary
: public ExecExprString
{
protected:

  ExecExpr* arg1;
  ExecExpr* arg2;
  ExecExpr* arg3;

public:

  ExecExprStringBinary(ExecExpr* arg1, ExecExpr* arg2) {
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->arg3 = arg3;
  }

  ~ExecExprStringBinary() {
    delete arg1;
    delete arg2;
    delete arg3;
  }

private:

  ExecExprStringBinary(const ExecExprStringBinary& x);
  ExecExprStringBinary& operator=(const ExecExprStringBinary& x);
};


class ExecExprStringTernary
: public ExecExprString
{
protected:

  ExecExpr* arg1;
  ExecExpr* arg2;
  ExecExpr* arg3;

public:

  ExecExprStringTernary(ExecExpr* arg1, ExecExpr* arg2, ExecExpr* arg3) {
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->arg3 = arg3;
  }

  ~ExecExprStringTernary() {
    delete arg1;
    delete arg2;
    delete arg3;
  }

private:

  ExecExprStringTernary(const ExecExprStringTernary& x);
  ExecExprStringTernary& operator=(const ExecExprStringTernary& x);
};


template<class E> static
ExecExpr* createUnary(ExecExprList& args)
{
  assert(args.size() == 1);
  return new E(args[0]);
}

template<class E> static
ExecExpr* createBinary(ExecExprList& args)
{
  assert(args.size() == 2);
  return new E(args[0], args[1]);
}

template<class E> static
ExecExpr* createTernary(ExecExprList& args)
{
  assert(args.size() == 3);
  return new E(args[0], args[1], args[2]);
}

template<class E> static
ExecExpr* createNary(ExecExprList& args)
{
  return new E(args);
}

class ExecExprStringTrimBoth
: public ExecExprStringUnary
{
public:

  ExecExprStringTrimBoth(ExecExpr* arg) : ExecExprStringUnary(arg) {
    setResultAdt(arg->getAdt());
  }

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const char* x = (const char*)arg->eval(tup1, tup2);
    if( !x ) return NULL;
    while( *x == ' ' ) x++;
    int len = strlen(x);
    const char* y = x + len - 1;
    while( len > 0 && *y == ' ' ) {
      y--;
      len--;
    }
    for(int i = 0 ; i < len ; i++) {
      result[i] = x[i];
    }
    result[len] = '\0';
    return result;
  }
};

class ExecExprStringConcat
: public ExecExprStringBinary
{
public:

  ExecExprStringConcat(ExecExpr* arg1, ExecExpr* arg2)
    : ExecExprStringBinary(arg1, arg2) {
      const DteAdt& t1 = arg1->getAdt();
      const DteAdt& t2 = arg2->getAdt();
      int len = t1.getMaxSize() + t2.getMaxSize();
      //kb: check for max string length size
      bool nullable = t1.isNullable() || t2.isNullable();
      setResultAdt(DteStringAdt(len, nullable));
    }

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const char* x = (const char*)arg1->eval(tup1,tup2);
    if( !x ) return NULL;
    const char* y = (const char*)arg2->eval(tup1,tup2);
    if( !y ) return NULL;

    int len = 0;
    //while( *x && len < maxlen ) {
    while( *x ) {
      result[len++] = *(x++);
    }
    //while( *y && len < maxlen ) {
    while( *y ) {
      result[len++] = *(y++);
    }
    //if( *x || *y ) { // didn't fit
    //}
    result[len] = '\0';
    return result;
  }
};

class ExecExprStringSubstr
: public ExecExprStringTernary
{
public:

  ExecExprStringSubstr(ExecExpr* arg1, ExecExpr* arg2, ExecExpr* arg3)
    : ExecExprStringTernary(arg1, arg2, arg3) {
      setResultAdt(arg1->getAdt());
    }

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const char* s = (const char*)arg1->eval(tup1,tup2);
    if( !s ) return NULL;
    const Type* x = arg2->eval(tup1,tup2);
    if( !x ) return NULL;
    int4 start = DteIntAdt::cast(x) - 1;
    x = arg3->eval(tup1,tup2);
    if( !x ) return NULL;
    int4 chars = DteIntAdt::cast(x);

    if( start < 0 ) start = 0;
    int len = strlen(s);
    if( start > len ) {
      result[0] = '\0';
      return result;
    }
    len -= start;
    if( chars >= 0 && chars < len ) {
      len = chars;
    }
    for(int i = 0 ; i < len ; i++) {
      result[i] = s[i];
    }
    result[len] = '\0';
    return result;
  }
};


class ExecExprUnary
: public ExecExpr
{
protected:

  ExecExpr* arg;

public:

  ExecExprUnary(ExecExprList args) {
    assert( args.size() == 1 );
    arg = args[0];
  }

  ExecExprUnary(ExecExpr* arg) {
    this->arg = arg;
  }

  ~ExecExprUnary() {
    delete arg;
  }

private:

  ExecExprUnary(const ExecExprUnary& x);
  ExecExprUnary& operator=(const ExecExprUnary& x);
};



class ExecExprBinary
: public ExecExpr
{
protected:

  ExecExpr* left;
  ExecExpr* right;

public:

  ExecExprBinary(ExecExprList args) {
    assert( args.size() == 2 );
    left = args[0];
    right = args[1];
  }

  ExecExprBinary(ExecExpr* left, ExecExpr* right) {
    this->left = left;
    this->right = right;
  }

  ~ExecExprBinary() {
    delete left;
    delete right;
  }

private:

  ExecExprBinary(const ExecExprBinary& x);
  ExecExprBinary& operator=(const ExecExprBinary& x);
};


class ExecExprTernary
: public ExecExpr
{
protected:

  ExecExpr* arg1;
  ExecExpr* arg2;
  ExecExpr* arg3;

public:

  ExecExprTernary(ExecExprList args) {
    assert( args.size() == 3 );
    arg1 = args[0];
    arg2 = args[1];
    arg3 = args[2];
  }

  ExecExprTernary(ExecExpr* arg1, ExecExpr* arg2, ExecExpr* arg3) {
    this->arg1 = arg1;
    this->arg2 = arg2;
    this->arg3 = arg3;
  }

  ~ExecExprTernary() {
    delete arg1;
    delete arg2;
    delete arg3;
  }

private:

  ExecExprTernary(const ExecExprTernary& x);
  ExecExprTernary& operator=(const ExecExprTernary& x);
};




template<class Adt, class Op>
class ExecExprUnaryOp
: public ExecExprUnary
{
public:

  typedef typename Op::argument_type Arg;
  typedef typename Adt::BasicType Res;

  ExecExprUnaryOp(ExecExpr* arg) : ExecExprUnary(arg) {
    bool nullable = arg->getAdt().isNullable();
    setResultAdt(Adt(nullable));
  }

  ~ExecExprUnaryOp() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const Type* x = arg->eval(tup1,tup2);
    if( x ) {
      result = op(*(Arg*)x);
      return &result;
    }
    return NULL;
  }

protected:

  Res result;
  Op op;
};

template<class Adt, class Op>
class ExecExprOp
: public ExecExprBinary
{
public:

  typedef typename Op::first_argument_type Arg1;
  typedef typename Op::second_argument_type Arg2;
  typedef typename Adt::BasicType Res;

  ExecExprOp(ExecExpr* left, ExecExpr* right) : ExecExprBinary(left, right) {
    bool nullable = left->getAdt().isNullable()
      || right->getAdt().isNullable();
    setResultAdt(Adt(nullable));
  }

  ~ExecExprOp() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const Type* l = left->eval(tup1,tup2);
    if( l ) {
      const Type* r = right->eval(tup1,tup2);
      if( r ) {
        result = op(*(Arg1*)l, *(Arg2*)r);
        return &result;
      }
    }
    return NULL;
  }

protected:

  Res result;
  Op op;
};

template<class Adt, class Op>
class ExecExprPtrOp
: public ExecExprBinary
{
public:

  typedef typename Op::first_argument_type Arg1;
  typedef typename Op::second_argument_type Arg2;
  typedef typename Adt::BasicType Res;

  ExecExprPtrOp(ExecExpr* left, ExecExpr* right) : ExecExprBinary(left, right) {
    bool nullable = left->getAdt().isNullable() || right->getAdt().isNullable();
    setResultAdt(Adt(nullable));
  }

  ~ExecExprPtrOp() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const Type* l = left->eval(tup1,tup2);
    if( l ) {
      const Type* r = right->eval(tup1,tup2);
      if( r ) {
        result = op((Arg1)l, (Arg2)r);
        return &result;
      }
    }
    return NULL;
  }

protected:

  Res result;
  Op op;
};


template<class Adt, class Op>
class ExecExprOp3
: public ExecExprTernary
{
public:

  typedef typename Op::first_argument_type Arg1;
  typedef typename Op::second_argument_type Arg2;
  typedef typename Op::third_argument_type Arg3;
  typedef typename Adt::BasicType Res;

  ExecExprOp3(ExecExpr* arg1, ExecExpr* arg2, ExecExpr* arg3)
		: ExecExprTernary(arg1, arg2, arg3)
	{
    bool nullable = arg1->getAdt().isNullable()
			|| arg2->getAdt().isNullable() || arg3->getAdt().isNullable();
    setResultAdt(Adt(nullable));
  }

  ~ExecExprOp3() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2) {
    const Type* x1 = arg1->eval(tup1,tup2);
    if( x1 ) {
      const Type* x2 = arg2->eval(tup1,tup2);
      if( x2 ) {
				const Type* x3 = arg3->eval(tup1,tup2);
				if( x3 ) {
					result = op(*(Arg1*)x1, *(Arg2*)x2, *(Arg3*)x3);
					return &result;
				}
      }
    }
    return NULL;
  }

protected:

  Res result;
  Op op;
};



//kb: improve and move these
Type* allocateBool(bool x) {
  char* p = new char;
  *p = x;
  return p;
}

Type* allocateInt4(int4 x) {
  int4* p = new int4;
  *p = x;
  return p;
}

Type* allocateFloat8(float8 x) {
  float8* p = new float8;
  *p = x;
  return p;
}

Type* allocateString(const string& x) {
  int len = x.length();
  char* p = new char[len+1];
  strcpy(p, x.c_str());
  return p;
}

Type* allocateString(const char* x) {
  int len = strlen(x);
  char* p = new char[len+1];
  strcpy(p, x);
  return p;
}


//---------------------------------------------------------------------------

class ExecConstant : public ExecExpr
{
  Type* value;

public:

  ExecConstant(const DteAdt& adt, Type* x)
    : ExecExpr(adt), value(x) {}

  ~ExecConstant() { resultAdt->deallocate(value); }

  const Type* eval(const Tuple* tup1, const Tuple* tup2) { return value; }

private:

  ExecConstant(const ExecConstant& x);
  ExecConstant& operator=(const ExecConstant& x);
};


//---------------------------------------------------------------------------

class ExecVariable : public ExecExpr
{
  OptConstant* value;           // do NOT delete

public:

  ExecVariable(OptConstant* v)
    : ExecExpr(v->getAdt()), value(v) {}

  ~ExecVariable() {}

  const Type* eval(const Tuple* tup1, const Tuple* tup2)
    { return value->getValue(); }

private:

  ExecVariable(const ExecVariable& x);
  ExecVariable& operator=(const ExecVariable& x);
};


//---------------------------------------------------------------------------

#include <stl_function.h>

template <class From, class To>
struct CastFn : public unary_function<From, To> {
  To operator()(const From& x) const { return To(x); }
};

struct ToUnixTime : public unary_function<EncodedDTF, int4>
{
  int4 operator()(const EncodedDTF& x) const {
    return DteDateAdt::toUnixTime(x);
  }
};



#if 0
//kb: these work, but are they really safe to use; use cstring_ versions for now
bool equal_to<char*>::operator()(char * const & x, char* const & y) const
{ return strcmp(x,y) == 0; }

bool equal_to<const char*>::operator()(const char * const & x,
                                       const char* const & y) const
{ return strcmp(x,y) == 0; }
#endif

struct cstring_equal_to
  : public binary_function<const char*, const char*, bool> {
  bool operator()(const char* x, const char* y) const
    { return strcmp(x,y) == 0; }
};

struct cstring_not_equal_to
  : public binary_function<const char*, const char*, bool> {
  bool operator()(const char* x, const char* y) const
    { return strcmp(x,y) != 0; }
};

struct cstring_less
  : public binary_function<const char*, const char*, bool> {
  bool operator()(const char* x, const char* y) const
    { return strcmp(x,y) < 0; }
};

struct cstring_greater
  : public binary_function<const char*, const char*, bool> {
  bool operator()(const char* x, const char* y) const
    { return strcmp(x,y) > 0; }
};

struct cstring_less_equal
  : public binary_function<const char*, const char*, bool> {
  bool operator()(const char* x, const char* y) const
    { return strcmp(x,y) <= 0; }
};

struct cstring_greater_equal
  : public binary_function<const char*, const char*, bool> {
  bool operator()(const char* x, const char* y) const
    { return strcmp(x,y) >= 0; }
};


//kb: unify unary, binary, etc functions?
//kb: what about fuctions that take tuples?
//kb: eg, f(tuple<int,int>, tuple<int,int>) -> tuple<int,int>
static const int MAX_FUNCTION_ARGS = 3;
struct FunctionDesc
{
  const char* functionName;
  int numArgs;
  int argType[MAX_FUNCTION_ARGS];
  int returnType;               //kb: keep this?
  ExecExpr* (*createFn)(ExecExprList& args);
};

static FunctionDesc functionTable[] = {
  {"not", 1, {DteBoolId}, DteBoolId,
   createUnary<ExecExprUnaryOp<DteBoolAdt, logical_not<char> > >},
  {"-", 1, {DteIntId}, DteIntId,
   createUnary<ExecExprUnaryOp<DteIntAdt, negate<int4> > >},
  {"-", 1, {DteDoubleId}, DteDoubleId,
   createUnary<ExecExprUnaryOp<DteDoubleAdt, negate<float8> > >},
  {"double", 1, {DteIntId}, DteDoubleId,
   createUnary<ExecExprUnaryOp<DteDoubleAdt, CastFn<int4,float8> > >},
  {"float8", 1, {DteIntId}, DteDoubleId,
   createUnary<ExecExprUnaryOp<DteDoubleAdt, CastFn<int4,float8> > >},
  {"int", 1, {DteDoubleId}, DteIntId,
   createUnary<ExecExprUnaryOp<DteIntAdt, CastFn<float8,int4> > >},
  {"int4", 1, {DteDoubleId}, DteIntId,
   createUnary<ExecExprUnaryOp<DteIntAdt, CastFn<float8,int4> > >},

  {"and", 2, {DteBoolId, DteBoolId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, logical_and<char> > >},
  {"or", 2, {DteBoolId, DteBoolId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, logical_or<char> > >},

  {"=", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, equal_to<int4> > >},
  {"<>", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, not_equal_to<int4> > >},
  {"!=", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, not_equal_to<int4> > >},
  {"<", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, less<int4> > >},
  {">", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, greater<int4> > >},
  {"<=", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, less_equal<int4> > >},
  {">=", 2, {DteIntId, DteIntId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, greater_equal<int4> > >},

  {"=",  2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, equal_to<float8> > >},
  {"<>", 2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, not_equal_to<float8> > >},
  {"!=", 2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, not_equal_to<float8> > >},
  {"<",  2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, less<float8> > >},
  {">",  2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, greater<float8> > >},
  {"<=", 2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, less_equal<float8> > >},
  {">=", 2, {DteDoubleId, DteDoubleId}, DteBoolId,
   createBinary<ExecExprOp<DteBoolAdt, greater_equal<float8> > >},

  //createBinary<ExecExprPtrOp<DteBoolAdt, equal_to<const char*> > >},
  {"=",  2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_equal_to > >},
  {"<>", 2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_not_equal_to > >},
  {"!=", 2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_not_equal_to > >},
  {"<",  2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_less > >},
  {">",  2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_greater > >},
  {"<=", 2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_less_equal > >},
  {">=", 2, {DteStringId, DteStringId}, DteBoolId,
   createBinary<ExecExprPtrOp<DteBoolAdt, cstring_greater_equal > >},

  {"+", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, plus<int4> > >},
  {"-", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, minus<int4> > >},
  {"*", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, multiplies<int4> > >},
  {"/", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, divides<int4> > >},
  {"div", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, divides<int4> > >},
  {"%", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, modulus<int4> > >},
  {"mod", 2, {DteIntId, DteIntId}, DteIntId,
   createBinary<ExecExprOp<DteIntAdt, modulus<int4> > >},

  {"+", 2, {DteDoubleId, DteDoubleId}, DteDoubleId,
   createBinary<ExecExprOp<DteDoubleAdt, plus<float8> > >},
  {"-", 2, {DteDoubleId, DteDoubleId}, DteDoubleId,
   createBinary<ExecExprOp<DteDoubleAdt, minus<float8> > >},
  {"*", 2, {DteDoubleId, DteDoubleId}, DteDoubleId,
   createBinary<ExecExprOp<DteDoubleAdt, multiplies<float8> > >},
  {"/", 2, {DteDoubleId, DteDoubleId}, DteDoubleId,
   createBinary<ExecExprOp<DteDoubleAdt, divides<float8> > >},
  {"bucket", 3, {DteDoubleId, DteDoubleId, DteDoubleId}, DteIntId,
   createTernary< ExecExprOp3<DteIntAdt, BucketOp<float8> > >},
  {"bin", 3, {DteDoubleId, DteDoubleId, DteDoubleId}, DteDoubleId,
   createTernary< ExecExprOp3<DteDoubleAdt, BinOp<float8> > >},

  {"concat", 2, {DteStringId, DteStringId}, DteStringId,
   createBinary<ExecExprStringConcat>},
  {"||", 2, {DteStringId, DteStringId}, DteStringId,
   createBinary<ExecExprStringConcat>},
  {"trim", 1, {DteStringId}, DteStringId,
   createUnary<ExecExprStringTrimBoth>},

  {"toUnixTime", 1, {DteDateId}, DteIntId,
   createUnary<ExecExprUnaryOp<DteIntAdt, ToUnixTime> >},

  {"map", 1, {DteStringId}, DteIntId, createUnary< MapAttrExec<DteStringAdt> >},
  {"map", 1, {DteIntId}, DteIntId, createUnary< MapAttrExec<DteIntAdt> >},
  {"map", 1, {DteDoubleId}, DteIntId, createUnary< MapAttrExec<DteIntAdt> >},

  {NULL}
};


static bool canPromote(int from, int to)
{
  if( from == to ) {
    return true;
  }
  if( from == DteIntId ) {
    if( to == DteDoubleId ) {
      return true;
    }
  }
  return false;
}

bool ExecExpr::canPromote(const DteAdt& from, const DteAdt& to)
{
	//kb: consider type args??
	return ::canPromote(from.getTypeID(), to.getTypeID());
}

static ExecExpr* getPromoteExpr(int from, int to, ExecExpr* expr)
{
  assert(expr);
  if( from == to ) {
    return expr;
  }
  if( from == DteIntId ) {
    if( to == DteDoubleId ) {
      return new ExecExprUnaryOp<DteDoubleAdt, CastFn<int4, float8> >(expr);
    }
  }
  assert(0);
  return NULL;
}

ExecExpr* ExecExpr::getPromotion(const DteAdt& from, const DteAdt& to,
																 ExecExpr* expr)
{
	return ::getPromoteExpr(from.getTypeID(), to.getTypeID(), expr);
}


static int findFunction(const string& fn, const vector<int>& types)
{
  int N = types.size();
  int promote = -1;
  for(int i = 0 ; functionTable[i].functionName != NULL ; i++) {
    FunctionDesc& f = functionTable[i];
    if( fn == f.functionName && N == f.numArgs ) {
      bool exactMatch = true;
      bool promotable = true;
      for(int j = 0 ; j < N ; j++) {
        if( types[j] != f.argType[j] ) {
          exactMatch = false;
        }
        if( !canPromote(types[j], f.argType[j]) ) {
          promotable = false;
        }
      }
      if( exactMatch ) {
        return i;
      }
      if( promotable ) {
        if( promote == -1 ) {
          promote = i;
        } else {
          promote = -2;
        }
      }
    }
  }
  // no exact match
  if( promote >= 0 ) {
    // found a single function with a promotion
    return promote;
  }
  return -1;
}

//  class Exception
//  {
//  public:
//    string msg;
//    Exception(const string& msg) : msg(msg) {}
//  };


ExecExpr* ExecExpr::createConstant(const DteAdt& adt, Type* x)
{
  return new ExecConstant(adt, x);
}

ExecExpr* ExecExpr::createVariable(const string& varName)
{
  OptConstant* value = mqlSession.findVariable(varName);
  assert( value != NULL && "unknown variable" );
  return new ExecVariable(value);
}

//kb: need a wildcard type to handle null constants
ExecExpr* ExecExpr::createNull()
{
  return new ExecConstant(DteStringAdt(1), NULL);
}

//kb: these should call allocateCopy
ExecExpr* ExecExpr::createBool(bool x)
{
  return new ExecConstant(DteBoolAdt(), allocateBool(x));
}

ExecExpr* ExecExpr::createInt4(int4 x)
{
  return new ExecConstant(DteIntAdt(), allocateInt4(x));
}

ExecExpr* ExecExpr::createFloat8(float8 x)
{
  return new ExecConstant(DteDoubleAdt(), allocateFloat8(x));
}

ExecExpr* ExecExpr::createString(const string& x)
{
  return new ExecConstant(DteStringAdt(x.length()), allocateString(x));
}

ExecExpr* ExecExpr::createString(const char* x)
{
  return new ExecConstant(DteStringAdt(strlen(x)), allocateString(x));
}

ExecExpr* ExecExpr::createField(const DteAdt& adt, int tupNum, int slot)
{
  if( tupNum == 0 ) {
    return new ExecExprField(adt, slot);
  } else if( tupNum == 1 ) {
    return new ExecExprFieldRight(adt, slot);
  }
  assert("no support for more than two tuples");
  return NULL;
}


ExecExpr* ExecExpr::createFunction(const string& fn, ExecExprList& args)
{
  int N = args.size();
  vector<int> types(N);
  for(int i = 0 ; i < N ; i++) {
    const DteAdt& adt = args[i]->getAdt();
    types[i] = adt.getTypeID();
  }
  int k = findFunction(fn, types);
  if( k < 0 ) {
    string msg("function not found: ");
    msg += fn;
    char sep = '(';
    for(int i = 0 ; i < N ; i++) {
      msg += sep;
      const DteAdt& adt = args[i]->getAdt();
      msg += adt.getTypeName();
      sep = ',';
    }
    msg += ')';
    cerr << msg << endl;
    //throw Exception(msg);
    return NULL;
  }
  FunctionDesc& f = functionTable[k];
  for(int i = 0 ; i < N ; i++) {
    args[i] = getPromoteExpr(types[i], f.argType[i], args[i]);
    assert(args[i]);
  }
  return f.createFn(args);
}

DteAdt* ExecExpr::typeCheck(const string& functionName, 
                            const DteAdtList& argTypes)
{
  //kb: improve typeCheck function

  // This is a bit of a hack: It creates fake constants of the correct
  // types and then it creates the function only to delete it after it
  // gets the return type.  The problem is that only a executable
  // version of the function knows its return type right now.

  int N = argTypes.size();
  ExecExprList args(N);
  for(int i = 0 ; i < N ; i++) {
    args[i] = createConstant(*argTypes[i], NULL);
  }
  DteAdt* retType = NULL;
  ExecExpr* expr = NULL;
  try {
    expr = createFunction(functionName, args);
  } catch(...) {
    // should really throw here
  }
  if( expr ) {
    retType = expr->getAdt().clone();
    delete expr;
  } else {
    delete_all(args);
    //cerr << "function not found: " << functionName << endl;
  }
  return retType;
}

ExecExpr* ExecExpr::createFunction(const string& fn, ExecExpr* arg1)
{
  ExecExprList args(1);
  args[0] = arg1;
  return ExecExpr::createFunction(fn, args);
}

ExecExpr* ExecExpr::createFunction(const string& fn,
                                   ExecExpr* arg1, ExecExpr* arg2)
{
  ExecExprList args(2);
  args[0] = arg1;
  args[1] = arg2;
  return ExecExpr::createFunction(fn, args);
}

ExecExpr* ExecExpr::createFunction(const string& fn, ExecExpr* arg1,
                                   ExecExpr* arg2, ExecExpr* arg3)
{
  ExecExprList args(3);
  args[0] = arg1;
  args[1] = arg2;
  args[2] = arg3;
  return ExecExpr::createFunction(fn, args);
}

#if 0
// rest is for debugging

//---------------------------------------------------------------------------
#include "DTE/util/DteAlgo.h"

static ExecExpr* parseNumber(istream& in)
{
  string str;
  char c = 'q';
  c = in.peek();
  bool isInt = true;
  while( in ) {
    if( c == '.' || c == 'e' || c == 'E' ) {
      isInt = false;
    } else if( !isdigit(c) ) {
      break;
    }
    //cerr << "got num: " << c << endl;
    in.get();
    str += c;
    c = in.peek();
  }
  if( isInt ) {
    return ExecExpr::createInt4(atoi(str.c_str()));
  }
  return ExecExpr::createFloat8(atof(str.c_str()));
}

static ExecExpr* parseString(istream& in)
{
  string str;
  char quote;
  in >> quote;
  assert( quote == '\'' || quote == '"' );
  
  char c;
  in.get(c);
  while( in && c != quote ) {
    if( c == '\\' ) {
      in.get(c);
    }
    str += c;
    in.get(c);
  }
  return ExecExpr::createString(str);
}

static string parseId(istream& in)
{
  string str;
  char c;
  c = in.peek();
  while( in ) {
    switch(c) {
    case ' ':
    case '\n':
    case '\t':
    case '\f':
    case '(':
    case ',':
    case ')':
      goto done;
    }
    in.get();
    str += c;
    c = in.peek();
  }
 done:
  return str;
}

ExecExpr* parseExpr(istream& in);

static ExecExpr* parseFunction(string& fn, istream& in)
{
  char c;
  in.get(c);
  assert(c == '(');
  ExecExprList args;
  ExecExpr* expr;
  //try {
    while(in) {
      args.push_back(parseExpr(in));
      in.get(c);
      if( c == ')') break;
      assert( c == ',' );
    }
    assert(c == ')');
    expr = ExecExpr::createFunction(fn, args);
    //} catch(...) {
    //delete_all(args);
    //throw;
    //}
    if( !expr ) {
      delete_all(args);
    }
    return expr;
}

static ExecExpr* parseExpr(istream& in)
{
  char c = 'x';
  in.ipfx(0);                   // skip ws
  c = in.peek();
  if( !cin ) return NULL;
  //cerr << "got: " << c << endl;
  if( isdigit(c) ) {
    return parseNumber(in);
  }
  if( c == '\'' || c == '"' ) {
    return parseString(in);
  }
  string id;
  id = parseId(in);
  if( id == "true" ) {
    return ExecExpr::createBool(true);
  }
  if( id == "false" ) {
    return ExecExpr::createBool(false);
  }
  c = in.peek();
  //cerr << "got2: " << c << endl;
  if( c == '(' ) {
    return parseFunction(id, in);
  }
  throw Exception(string("parse error at ") + c);
}


int main(int ac, char* av[])
{
  try {
    while( 1 ) {
      const Tuple* tup1 = NULL;
      const Tuple* tup2 = NULL;
      ExecExpr* e = parseExpr(cin);
      if( !cin ) break;
      assert(e);
      const DteAdt& resAdt = e->getAdt();
      cout << resAdt->getTypeSpec() << endl;
      resAdt->print(cout, e->eval(tup1,tup2));
      cout << endl;
      delete e;
    }

    /*
    
    ExecExpr* e =
      ExecExpr::create("not",
        ExecExpr::create("<",
          ExecExpr::create("-",
            ExecExpr::create("+",
              ExecExpr::create("+",
                ExecExpr::createFloat8(1.2),
                ExecExpr::createInt4(2)),
              ExecExpr::create("+",
                ExecExpr::createInt4(3),
                ExecExpr::createInt4(4)))),
          ExecExpr::createInt4(3)));
    assert(e);
    
    const DteAdt& resAdt = e->getType();
    cout << resAdt->getTypeSpec() << endl;
    resAdt->print(cout, e->eval());
    cout << endl;
    delete e;
    
    e =
      ExecExpr::create("trim",
        ExecExpr::create("concat",
          ExecExpr::createString(" hello "),
          ExecExpr::createString(" world ")));
    assert(e);
    resAdt = e->getType();
    cout << resAdt->getTypeSpec() << endl;
    resAdt->print(cout, e->eval());
    cout << endl;
    delete e;
    */

  } catch( Exception ex ) {
    cerr << "oops: " << ex.msg << endl;
  }

  return 0;
}

#endif
