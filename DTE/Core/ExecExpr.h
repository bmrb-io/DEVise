#ifndef _EXECEXPR_H_
#define _EXECEXPR_H_

// (C) 1998- by Kevin S. Beyer

#include <vector>
#include "DTE/types/DteAdt.h"


typedef vector<class ExecExpr*> ExecExprList;


class ExecExpr
{
public:

  // setResultAdt() must be called later
  ExecExpr() : resultAdt(NULL) {}

  ExecExpr(const DteAdt& resAdt) : resultAdt(resAdt.clone()) {}

  virtual ~ExecExpr() { delete resultAdt; }

  virtual const Type* eval(const Tuple* tup1, const Tuple* tup2) = 0;

  DteAdt& getAdt() { return *resultAdt; }

  // returns null on error.
  // result must be deleted by caller.
  static DteAdt* typeCheck(const string& functionName,
                           const DteAdtList& argTypes);

	static bool canPromote(const DteAdt& from, const DteAdt& to);

	static ExecExpr* getPromotion(const DteAdt& from, const DteAdt& to,
																ExecExpr* expr);

  static ExecExpr* createFunction(const string& functionName,
                                  ExecExprList& args);

  static ExecExpr* createFunction(const string& functionName,
                                  ExecExpr* arg1);

  static ExecExpr* createFunction(const string& functionName,
                                  ExecExpr* arg1, ExecExpr* arg2);

  static ExecExpr* createFunction(const string& functionName,
                                  ExecExpr* arg1, ExecExpr* arg2,
                                  ExecExpr* arg3);

  static ExecExpr* createField(const DteAdt& adt, int tupNum, int slot);

  // x now belongs to this expr
  static ExecExpr* createConstant(const DteAdt& adt, Type* x);

  static ExecExpr* createVariable(const string& varName);

  //kb: need wildcard type for null...
  static ExecExpr* createNull();

  static ExecExpr* createBool(bool x);

  static ExecExpr* createInt4(int4 x);

  static ExecExpr* createFloat8(float8 x);

  static ExecExpr* createString(const string& x);

  static ExecExpr* createString(const char* x);

protected:

  DteAdt* resultAdt;

  // this can only be called once
  void setResultAdt(const DteAdt& resAdt) {
    assert(this->resultAdt == NULL);
    this->resultAdt = resAdt.clone();
  }
  
private:

  ExecExpr(const ExecExpr& x);
  ExecExpr& operator=(const ExecExpr& x);
};

typedef vector<ExecExpr*> ExprList;

#endif // _EXECEXPR_H_
