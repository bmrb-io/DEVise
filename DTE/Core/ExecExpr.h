#ifndef EXECEXPR_H
#define EXECEXPR_H

#include <assert.h>
#include "types.h"

#ifndef __GNUG__
using namespace std;
#endif


class ExecExpr {
protected:
	TypeID typeID;
public:
	ExecExpr(const TypeID& type) : typeID(type) {}
	virtual ~ExecExpr() {};
	virtual const Type* evaluate(const Tuple* left, const Tuple* right) = 0;
	const TypeID& getTypeID(){
		return typeID;
	}
	static ExecExpr* createExpr(const string& fnname, class ExprList* argp);
};


//---------------------------------------------------------------------------


class ExprList
{
public:

  ExprList() {}

  ~ExprList();

  int size() { return _expr.size(); }

  ExecExpr* operator[](int i) { return _expr[i]; }

  void push_back(ExecExpr* e);

  const TypeIDList& getTypes();

  void clear();                 // clears without deleting

protected:

  vector<ExecExpr*> _expr;

  TypeIDList _types;

private:

  ExprList(const ExprList& x);
  ExprList& operator=(const ExprList& x);
};


inline
ExprList::~ExprList()
{
  for(vector<ExecExpr*>::iterator i = _expr.begin() ; i != _expr.end() ; i++) {
    delete *i;
  }
}


inline
void ExprList::push_back(ExecExpr* e)
{
  _expr.push_back(e);
  _types.push_back(e->getTypeID());
}


inline
const TypeIDList& ExprList::getTypes()
{
  return _types;
}


inline
void ExprList::clear()
{
  // _expr.clear();   // vector::clear() not defined in gcc 2.7
  _expr.erase(_expr.begin(),_expr.end());
  _types.clear();
}


//---------------------------------------------------------------------------


class ExecSelect : public ExecExpr {
     int leftRight;
     int fieldNo;
public:	
     ExecSelect(const TypeID& type, int lr, int field) : 
		ExecExpr(type), leftRight(lr), fieldNo(field) {}
     ExecSelect(const Field& f)
       : ExecExpr(f.getType()), leftRight(f.isRight()), fieldNo(f.getPos()) {}
     virtual ~ExecSelect() {}
     const Type* evaluate(const Tuple* left, const Tuple* right) {
          const Type* base = (leftRight ? right[fieldNo] : left[fieldNo]);
          return base;
     }
};

class ExecConst : public ExecExpr {
	Type* value;
public:
	ExecConst(const TypeID& type, const Type* val) : ExecExpr(type) {
		value = duplicateObject(typeID, val);
	}
	virtual ~ExecConst(){
		DestroyPtr destroyPtr = getDestroyPtr(typeID);
		assert(destroyPtr);
		destroyPtr(value);
	}
     const Type* evaluate(const Tuple* left, const Tuple* right) {
		return value;
	}
};

class ExecTypeCast : public ExecExpr {
	ExecExpr* input;
     PromotePtr promotePtr;
     Type* value;
     size_t valueSize;
	DestroyPtr destroyPtr;
public:
	ExecTypeCast(ExecExpr* input, const TypeID& type);
	virtual ~ExecTypeCast(){
		delete input;
		assert(destroyPtr);
		destroyPtr(value);
	}
	const Type* evaluate(const Tuple* left, const Tuple* right){
		const Type* tmp = input->evaluate(left, right);
		assert(promotePtr);
		promotePtr(tmp, value);
		return value;
	}
};

class ExecMember : public ExecExpr {
	ExecExpr* input;
	MemberPtr memberPtr;
     Type* value;
     size_t valueSize;
	DestroyPtr destroyPtr;
public:
	ExecMember(ExecExpr* input, const string& name);
	virtual ~ExecMember(){
		delete input;
		destroyPtr(value);
	}
	const Type* evaluate(const Tuple* left, const Tuple* right) {
          memberPtr(input->evaluate(left, right), value);
          return value;
	}
};

class ExecOperator : public ExecExpr{
	ExecExpr* left;
	ExecExpr* right;
     OperatorPtr opPtr;
	Type* value;
	size_t valueSize;
	DestroyPtr destroyPtr;
public:
	ExecOperator(ExecExpr* l, ExecExpr* r, const string& name);
	virtual ~ExecOperator(){
		delete left;
		delete right;
		destroyPtr(value);
	}
	const Type* evaluate(const Tuple* leftT, const Tuple* rightT) {
		const Type* arg1 = left->evaluate(leftT, rightT);
		const Type* arg2 = right->evaluate(leftT, rightT);
		opPtr(arg1, arg2, value, valueSize);
		return value;
	}
};

class ExecConstructor : public ExecExpr{
        ExprList* input;
	ConstructorPtr consPtr;
	Type* value;
	size_t valueSize;
	DestroyPtr destroyPtr;
	Array<const Type*>* inputVals;
public:
	ExecConstructor(ExprList* input, const string& name);
	virtual ~ExecConstructor(){
		delete input;
		delete inputVals;
		assert(destroyPtr);
		destroyPtr(value);
	}
	const Type* evaluate(const Tuple* leftT, const Tuple* rightT) {
                int n = input->size();
		for(int i = 0; i < n ; i++){
			(*inputVals)[i] = (*input)[i]->evaluate(leftT, rightT);
		}
		consPtr(*inputVals, value, valueSize);
		return value;
	}
};





#endif
