/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
 */

#include "types.h"
#include <String.h>

Type* intAdd(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IInt(val1 + val2);
}

Type* intDoubleAdd(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IDouble(val1 + val2);
}

Type* intSub(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IInt(val1 - val2);
}

Type* intDoubleSub(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IDouble(val1 - val2);
}

Type* intEq(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IBool(val1 == val2);
}

Type* intDoubleEq(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IBool(val1 == val2);
}

Type* intLT(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IBool(val1 < val2);
}

Type* intDoubleLT(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IBool(val1 < val2);
}

Type* intGT(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IBool(val1 > val2);
}

Type* intDoubleGT(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IBool(val1 > val2);
}

Type* doubleAdd(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IDouble(val1 + val2);
}

Type* doubleIntAdd(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IDouble(val1 + val2);
}

Type* doubleSub(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IDouble(val1 - val2);
}

Type* doubleIntSub(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IDouble(val1 - val2);
}

Type* doubleEq(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IBool(val1 == val2);
}

Type* doubleIntEq(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IBool(val1 == val2);
}

Type* doubleLT(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IBool(val1 < val2);
}

Type* doubleIntLT(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IBool(val1 < val2);
}

Type* doubleGT(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IBool(val1 > val2);
}

Type* doubleIntGT(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	int val2 = ((IInt*)arg2)->getValue();
     return new IBool(val1 > val2);
}

Type* boolEq(Type* arg1, Type* arg2){
	bool val1 = ((IBool*)arg1)->getValue();
	bool val2 = ((IBool*)arg2)->getValue();
     return new IBool(val1 == val2);
}

Type* boolOr(Type* arg1, Type* arg2){
	bool val1 = ((IBool*)arg1)->getValue();
	bool val2 = ((IBool*)arg2)->getValue();
     return new IBool(val1 || val2);
}

Type* boolLT(Type* arg1, Type* arg2){
	bool val1 = ((IBool*)arg1)->getValue();
	bool val2 = ((IBool*)arg2)->getValue();
     return new IBool(val1 < val2);
}

Type* stringEq(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	int cmp = strcmp(val1, val2);
	return new IBool(cmp == 0);
}

Type* stringLT(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	int cmp = strcmp(val1, val2);
	return new IBool(cmp == -1);
}

Type* stringGT(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	int cmp = strcmp(val1, val2);
	return new IBool(cmp == 1);
}

Type* intRead(istream& in){
	int i;
	in >> i;
	return new IInt(i);
}

Type* doubleRead(istream& in){
	double i;
	in >> i;
	return new IDouble(i);
}

Type* stringRead(istream& in){
	String* i = new String;
	in >> *i;
	return new IString((char*) i->chars());
}

Type* boolRead(istream& in){
	bool i;
	in >> i;
	return new IBool(i);
}

int boolSize(int a, int b){
	return 1;
}
int sameSize(int a, int b){
	return a;
}

double oneOver2(BaseSelection* left, BaseSelection* right){
	return 0.5;
}

double oneOver3(BaseSelection* left, BaseSelection* right){
	return 0.3;
}

double oneOver10(BaseSelection* left, BaseSelection* right){
	return 0.1;
}

double oneOver100(BaseSelection* left, BaseSelection* right){
	return 0.01;
}

void displayAs(ostream& out, void* adt, String type){
	if(type == "int"){
		((IInt*) adt)->display(out);
	}
	else if(type == "string"){
		((IString*) adt)->display(out);
	}
	else if(type == "bool"){
		((IBool*) adt)->display(out);
	}
	else if(type == "double"){
		((IDouble*) adt)->display(out);
	}
	else{
		cout << "Don't know how to display type: " << type << endl;
		assert(0);
	}
}

int packSize(void* adt, String type){
	if(type == "int"){
		return ((IInt*) adt)->packSize();
	}
	else if(type == "string"){
		return ((IString*) adt)->packSize();
	}
	else if(type == "double"){
		return ((IDouble*) adt)->packSize();
	}
	else{
		cout << "Don't know how to pack type: " << type << endl;
		assert(0);
	}
}

void marshal(Type* adt, char* to, String type){
	if(type == "int"){
		((IInt*) adt)->marshal(to);
	}
	else if(type == "string"){
		((IString*) adt)->marshal(to);
	}
	else if(type == "double"){
		((IDouble*) adt)->marshal(to);
	}
	else{
		cout << "Don't know how to marshal type: " << type << endl;
		assert(0);
	}
}

Type* unmarshal(char* from, String type){
	if(type == "int"){
		IInt* adt = new IInt;
		adt->unmarshal(from);
		return adt;
	}
	else if(type == "string"){
		IString* adt = new IString;
		adt->unmarshal(from);
		return adt;
	}
	else{
		cout << "Don't know how to unmarshal type: " << type << endl;
		assert(0);
	}
}

GeneralPtr* getOperatorPtr(
	String name, TypeID root, TypeID arg, TypeID& retType){
	if(root == "int"){
		return IInt::getOperatorPtr(name, arg, retType);
	}
	else if(root == "string"){
		return IString::getOperatorPtr(name, arg, retType);
	}
	else if(root == "bool"){
		return IBool::getOperatorPtr(name, arg, retType);
	}
	else if(root == "double"){
		return IDouble::getOperatorPtr(name, arg, retType);
	}
	else{
		cout << "No such type: " << root << endl;
		assert(0);
	}
}

ReadPtr getReadPtr(TypeID root){
	if(root == "int"){
		return intRead;
	}
	else if(root == "string"){
		return stringRead;
	}
	else if(root == "bool"){
		return boolRead;
	}
	else if(root == "double"){
		return doubleRead;
	}
	else{
		cout << "No such type: " << root << endl;
		assert(0);
	}
}

AttrType getDeviseType(String type){
	if(type == "int"){
		return IntAttr;
	}
	else if(type == "float"){
		return FloatAttr;
	}
	else if(type == "double"){
		return DoubleAttr;
	}
	else if(type == "string"){
		return StringAttr;
	}
	else if(type == "date"){
		return DateAttr;
	}
	else{
		cout << "Don't know DEVise type for: " << type << endl;
		assert(0);
	}
}
