#include "ExecExpr.h"

/*
ExecExpr* ExecExpr::createExpr(const string& fnname, const ExprList& args)
{
	if(args.size() == 1){
		return new ExecTypeCast(args[0], fnname);
	}
	else if(args.size() == 2){
		return new ExecOperator(args[0], args[1], fnname);
	}
	else{
		return new ExecConstructor(args, fnname);
	}
}
*/

ExecTypeCast::ExecTypeCast(ExecExpr* input, const TypeID& type) 
	: ExecExpr(type), input(input)
{
	assert(input);
	TypeID inpType = input->getTypeID();
     value = allocateSpace(typeID, valueSize);
     destroyPtr = getDestroyPtr(typeID);
	promotePtr = getPromotePtr(inpType, typeID);
}

ExecMember::ExecMember(ExecExpr* input, const string& name)
	: ExecExpr(""), input(input)
{
	assert(input);
     TypeID parentType = input->getTypeID();
     GeneralMemberPtr* genPtr;
     genPtr = getMemberPtr(name, parentType, typeID);
     assert(genPtr);
     memberPtr = genPtr->memberPtr;
     assert(memberPtr);
	value = allocateSpace(typeID, valueSize);
	destroyPtr = getDestroyPtr(typeID);
}

ExecOperator::ExecOperator(ExecExpr* l, ExecExpr* r, const string& name)
	: ExecExpr(""), left(l), right(r)
{
	assert(left);
	assert(right);
	GeneralPtr* genPtr;
	TypeID root = left->getTypeID();
	TypeID arg = right->getTypeID();
	genPtr = getOperatorPtr(name, root, arg, typeID);
	assert(genPtr);
	opPtr = genPtr->opPtr;
	value = allocateSpace(typeID, valueSize);
	destroyPtr = getDestroyPtr(typeID);
}

ExecConstructor::ExecConstructor(Array<ExecExpr*>* input, const string& name)
	: ExecExpr(""), input(input)
{
	assert(input);
	int numFlds = input->length;
	inputVals = new Array<const Type*>(input->length);
	TypeID* inpTypes = new TypeID[numFlds];
     for(int i = 0; i < numFlds; i++){
          inpTypes[i] = (*input)[i]->getTypeID();
     }
	consPtr = getConstructorPtr(name, inpTypes, numFlds, typeID);
	delete [] inpTypes;
	value = allocateSpace(typeID, valueSize);
	destroyPtr = getDestroyPtr(typeID);
}
