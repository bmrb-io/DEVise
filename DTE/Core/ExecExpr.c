#include "ExecExpr.h"
#include <ctype.h>


ExecExpr* ExecExpr::createExpr(const string& fnname, ExprList* argp)
{
  ExprList& args = *argp;

  // strip 'op' from 'op<', 'op>=', etc
  const char* f = fnname.c_str();
  if( f[0] == 'o' && f[1] == 'p' && !isalpha(f[2]) ) {
    f = &f[2];
  }
  string fn(f);

  if(args.size() == 1){
    return new ExecTypeCast(args[0], fn);
    args.clear();                 // to keep from deleting expr
    delete argp;
  }
  else if(args.size() == 2){
    return new ExecOperator(args[0], args[1], fn);
    args.clear();                 // to keep from deleting expr
    delete argp;
  }
  else{
    return new ExecConstructor(argp, fn);
  }
}


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

ExecConstructor::ExecConstructor(ExprList* input, const string& name)
	: ExecExpr(""), input(input)
{
	assert(input);
	int numFlds = input->size();
	inputVals = new Array<const Type*>(numFlds);
	TypeID* inpTypes = new TypeID[numFlds];
     for(int i = 0; i < numFlds; i++){
          inpTypes[i] = (*input)[i]->getTypeID();
     }
	consPtr = getConstructorPtr(name, inpTypes, numFlds, typeID);
	delete [] inpTypes;
	value = allocateSpace(typeID, valueSize);
	destroyPtr = getDestroyPtr(typeID);
}
