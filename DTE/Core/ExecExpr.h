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
	// static ExecExpr* createExpr(const string& fnname, const ExprList& args);
};

class ExecSelect : public ExecExpr {
     int leftRight;
     int fieldNo;
public:	
     ExecSelect(const TypeID& type, int lr, int field) : 
		ExecExpr(type), leftRight(lr), fieldNo(field) {}
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
	Array<ExecExpr*>* input;
	ConstructorPtr consPtr;
	Type* value;
	size_t valueSize;
	DestroyPtr destroyPtr;
	Array<const Type*>* inputVals;
public:
	ExecConstructor(Array<ExecExpr*>* input, const string& name);
	virtual ~ExecConstructor(){
		delete input;
		delete inputVals;
		assert(destroyPtr);
		destroyPtr(value);
	}
	const Type* evaluate(const Tuple* leftT, const Tuple* rightT) {
		for(int i = 0; i < input->length; i++){
			(*inputVals)[i] = (*input)[i]->evaluate(leftT, rightT);
		}
		consPtr(*inputVals, value, valueSize);
		return value;
	}
};

#endif
