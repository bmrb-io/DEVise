#ifndef EXECEXPR_H
#define EXECEXPR_H

#include <assert.h>
#include "types.h"

#ifndef __GNUG__
using namespace std;
#endif

class ExecExpr {
public:
	virtual ~ExecExpr() {};
	virtual const Type* evaluate(const Tuple* left, const Tuple* right) = 0;
};

class ExecSelect : public ExecExpr {
     int leftRight;
     int fieldNo;
public:	
     ExecSelect(int lr, int field) : leftRight(lr), fieldNo(field) {}
	virtual ~ExecSelect() {}
     const Type* evaluate(const Tuple* left, const Tuple* right) {
          const Type* base = (leftRight ? right[fieldNo] : left[fieldNo]);
          return base;
     }
};

class ExecConst : public ExecExpr {
	Type* value;
	DestroyPtr destroyPtr;
public:
	ExecConst(Type* value, DestroyPtr destroyPtr) : 
		value(value), destroyPtr(destroyPtr) {
		assert(destroyPtr);
	}
	virtual ~ExecConst(){
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
	ExecTypeCast(ExecExpr* input, PromotePtr promotePtr,
			Type* value, size_t valueSize, DestroyPtr destroyPtr) :
		input(input), promotePtr(promotePtr), value(value),
		valueSize(valueSize), destroyPtr(destroyPtr) {
		assert(input);
		assert(promotePtr);
		assert(destroyPtr);
	}	
	virtual ~ExecTypeCast(){
		delete input;
		destroyPtr(value);
	}
	const Type* evaluate(const Tuple* left, const Tuple* right){
		const Type* tmp = input->evaluate(left, right);
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
	ExecMember(ExecExpr* input, MemberPtr memberPtr,
			Type* value, size_t valueSize, DestroyPtr destroyPtr) :
		input(input), memberPtr(memberPtr), value(value),
		valueSize(valueSize), destroyPtr(destroyPtr) {
		assert(input);
		assert(memberPtr);
		assert(destroyPtr);
	}	
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
	ExecOperator(ExecExpr* l, ExecExpr* r, 
		OperatorPtr opPtr, Type* value, size_t valueSize,
		DestroyPtr destroyPtr) :
		left(l), right(r), opPtr(opPtr), value(value), 
		valueSize(valueSize), destroyPtr(destroyPtr) {
	
		assert(left);
		assert(right);
		assert(opPtr);
		assert(destroyPtr);
	}
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
	ExecConstructor(Array<ExecExpr*>* input,
		ConstructorPtr consPtr, Type* value, size_t valueSize,
		DestroyPtr destroyPtr) :
		input(input), consPtr(consPtr), value(value), 
		valueSize(valueSize), destroyPtr(destroyPtr) {
	
		inputVals = new Array<const Type*>(input->length);
		assert(input);
		assert(consPtr);
		assert(destroyPtr);
	}
	virtual ~ExecConstructor(){
		delete input;
		delete inputVals;
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
