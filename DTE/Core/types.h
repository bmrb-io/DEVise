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
  Revision 1.5  1996/12/15 06:41:11  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/09 10:01:55  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.3  1996/12/05 16:06:06  wenger
  Added standard Devise file headers.

 */

#ifndef TYPES_H
#define TYPES_H

#include <iostream.h>
#include <assert.h>
#include <String.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "exception.h"
#include "AttrList.h"

class BaseSelection;

struct Stats{
	int* fldSizes;
	int cardinality;
	int numFlds;
	Stats(int numFlds, int* fldSizes, int c) :
		fldSizes(fldSizes), cardinality(c), numFlds(numFlds) {}
	Stats(int numflds){

          // default stats

          numFlds = numflds;
          fldSizes = new int[numFlds];
          for(int i = 0; i < numFlds; i++){
               fldSizes[i] = 10;
          }
          cardinality = 1000;
	}
	void read(istream* in){
		(*in) >> cardinality;
		for(int i = 0; i < numFlds; i++){
			(*in) >> fldSizes[i];
		}
	}
	void display(ostream& out){
		out << "cardinal: " << cardinality << "; ";
		out << "fldSizes: ";
		for(int i = 0; i < numFlds; i++){
			out << fldSizes[i] << " ";
		}
	}
};

class Offset{
	int offset;
	friend ostream& operator<<(ostream& out, Offset offset);
public:
	Offset() : offset(-1){}
	Offset(int offset) : offset(offset){}
	int getOffset(){
		return offset;
	}
	bool isNull(){
		return offset == -1;
	}
};

inline ostream& operator<<(ostream& out, Offset off){
	return out << off.offset;
}

typedef void Type;
typedef String TypeID;
typedef Type* Tuple;
typedef Type* (*OperatorPtr)(Type*, Type*);
typedef Type* (*ReadPtr)(istream&);
typedef int (*SizePtr)(int, int);
typedef double (*SelectyPtr)(BaseSelection* left, BaseSelection* right);

struct GeneralPtr{
	OperatorPtr opPtr;
	SizePtr	sizePtr;
	SelectyPtr selectyPtr;
	GeneralPtr(
		OperatorPtr opPtr, SizePtr sizePtr, SelectyPtr selectyPtr = NULL) : 
		opPtr(opPtr), sizePtr(sizePtr), selectyPtr(selectyPtr) {}
};

int boolSize(int a, int b);
int sameSize(int a, int b);

double oneOver2(BaseSelection* left, BaseSelection* right);
double oneOver3(BaseSelection* left, BaseSelection* right);
double oneOver10(BaseSelection* left, BaseSelection* right);
double oneOver100(BaseSelection* left, BaseSelection* right);

Type* intAdd(Type* arg1, Type* arg2);
Type* intSub(Type* arg1, Type* arg2);
Type* intEq(Type* arg1, Type* arg2);
Type* intLT(Type* arg1, Type* arg2);
Type* intGT(Type* arg1, Type* arg2);
Type* intComp(Type *arg1,Type *arg2);

Type* intDoubleAdd(Type* arg1, Type* arg2);
Type* intDoubleSub(Type* arg1, Type* arg2);
Type* intDoubleEq(Type* arg1, Type* arg2);
Type* intDoubleLT(Type* arg1, Type* arg2);
Type* intDoubleGT(Type* arg1, Type* arg2);
Type* intDoubleDiv(Type *arg1,Type* arg2);

Type* doubleAdd(Type* arg1, Type* arg2);
Type* doubleIntAdd(Type* arg1, Type* arg2);
Type* doubleSub(Type* arg1, Type* arg2);
Type* doubleDiv(Type* arg1, Type* arg2);
Type* doubleIntSub(Type* arg1, Type* arg2);
Type* doubleEq(Type* arg1, Type* arg2);
Type* doubleIntEq(Type* arg1, Type* arg2);
Type* doubleLT(Type* arg1, Type* arg2);
Type* doubleIntLT(Type* arg1, Type* arg2);
Type* doubleGT(Type* arg1, Type* arg2);

Type* doubleIntGT(Type* arg1, Type* arg2);

Type* stringEq(Type* arg1, Type* arg2);
Type* stringLT(Type* arg1, Type* arg2);
Type* stringGT(Type* arg1, Type* arg2);
Type* boolOr(Type* arg1, Type* arg2);
Type* boolEq(Type* arg1, Type* arg2);
Type* boolLT(Type* arg1, Type* arg2);

Type* intRead(istream&);
Type* stringRead(istream&);
Type* doubleRead(istream&);
Type* boolRead(istream&);

class IInt {
     int value;
public:
     IInt() {}
     IInt(int i) : value(i) {}
	int getValue(){
		return value;
	}
	void display(ostream& out){
		out << value;
	}
	int packSize(){
		return sizeof(int);
	}
	void marshal(char* to){

		// int tmp = htonl(value);
		// memcpy(to, &tmp, sizeof(int));

		memcpy(to, &value, sizeof(int));
	}
	void unmarshal(const char* from){
		// int tmp;
		// memcpy(&tmp, from, sizeof(int));
		// value = ntohl(tmp);

		memcpy(&value, from, sizeof(int));
	}
	static GeneralPtr* getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		if(arg == "int"){
			if(name == "+"){
				retType = "int";
				return new GeneralPtr(intAdd, sameSize);
			}
			else if(name == "-"){
				retType = "int";
				return new GeneralPtr(intSub, sameSize);
			}
			else if(name == "="){
				retType = "bool";
				return new GeneralPtr(intEq, boolSize, oneOver10);
			}
			else if(name == "<"){
				retType = "bool";
				return new GeneralPtr(intLT, boolSize, oneOver3);
			}
			else if(name == ">"){
				retType = "bool";
				return new GeneralPtr(intGT, boolSize, oneOver3);
			}
			else if(name == "compReturnInt"){
				retType = "int";
				return new GeneralPtr(intSub, sameSize );
			}
			else{
				return NULL;
			}
		}
		else if(arg == "double"){
			if(name == "+"){
				retType = "double";
				return new GeneralPtr(intDoubleAdd, sameSize);
			}
			else if(name == "-"){
				retType = "double";
				return new GeneralPtr(intDoubleSub, sameSize);
			}
			else if(name == "="){
				retType = "bool";
				return new GeneralPtr(intDoubleEq, boolSize, oneOver10);
			}
			else if(name == "<"){
				retType = "bool";
				return new GeneralPtr(intDoubleLT, boolSize, oneOver3);
			}
			else if(name == ">"){
				retType = "bool";
				return new GeneralPtr(intDoubleGT, boolSize, oneOver3);
			}
			else if(name == "/"){
				retType = "double";
				return new GeneralPtr(intDoubleDiv, sameSize );
			}
			else{
				return NULL;
			}
		}
		else{
			return NULL;
		}
	}
};

class IBool {
private:
	bool value;
public:
	IBool(bool value) : value(value){}
	bool getValue(){
		return value;
	}
     ostream& display(ostream& out){
          return out << value;
     }
	static GeneralPtr* getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		retType = "bool";
		if(arg != "bool"){
			return NULL;
		}
		else if(name == "or"){
			return new GeneralPtr(boolOr, boolSize, oneOver2);
		}
		else if(name == "="){
			return new GeneralPtr(boolEq, boolSize, oneOver2);
		}
		else if(name == "<"){
			return new GeneralPtr(boolLT, boolSize, oneOver2);
		}
		else{
			return NULL;
		}
     }
};

class IDouble {
private:
	double value;
public:
	IDouble(double val = 0) : value(val){}
	double getValue(){
		return value;
	}
     ostream& display(ostream& out){
          return out << value;
     }
	int packSize(){
		return sizeof(double);
	}
	void marshal(char* to){

		// int tmp = htonl(value);
		// memcpy(to, &tmp, sizeof(int));

		memcpy(to, &value, sizeof(double));
	}
	void unmarshal(const char* from){
		memcpy(&value, from, sizeof(double));
	}
	static GeneralPtr* getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		if(arg == "double"){
			if(name == "+"){
				retType = "double";
				return new GeneralPtr(doubleAdd, sameSize);
			}
			else if(name == "-"){
				retType = "double";
				return new GeneralPtr(doubleSub, sameSize);
			}
			else if(name == "="){
				retType = "bool";
				return new GeneralPtr(doubleEq, boolSize, oneOver100);
			}
			else if(name == "<"){
				retType = "bool";
				return new GeneralPtr(doubleLT, boolSize, oneOver3);
			}
			else if(name == ">"){
				retType = "bool";
				return new GeneralPtr(doubleGT, boolSize, oneOver3);
			}
			else if(name == "/"){
				retType = "double";
				return new GeneralPtr(doubleDiv, sameSize);
			}
			else{
				return NULL;
			}
		}
		else if(arg == "int"){
			if(name == "+"){
				retType = "double";
				return new GeneralPtr(doubleIntAdd, sameSize);
			}
			else if(name == "-"){
				retType = "double";
				return new GeneralPtr(doubleIntSub, sameSize);
			}
			else if(name == "="){
				retType = "bool";
				return new GeneralPtr(doubleIntEq, boolSize, oneOver100);
			}
			else if(name == "<"){
				retType = "bool";
				return new GeneralPtr(doubleIntLT, boolSize, oneOver3);
			}
			else if(name == ">"){
				retType = "bool";
				return new GeneralPtr(doubleIntGT, boolSize, oneOver3);
			}
			else{
				return NULL;
			}
		}
		else{
			return NULL;
		}
     }
};

class IString {
     int length;
	char* string;
public:
     IString() : length(0), string(NULL){}
	IString(char* s) : length(strlen(s) + 1){
		string = strdup(s);
	}
	IString(String* s){
		length = s->length() + 1; 
		string = strdup(s->chars());
	}
     IString(char* s, int l) : length(l), string(s){}
	char* getValue(){
		return string;
	}
	IString(IString & str){
		length = str.length;
		string = strdup(str.string);
	}
	void display(ostream& out){
		assert(string);
		out << string;
	}
	IString & operator=(IString & str){
		length = str.length;
		string = strdup(str.string);
		return *this;
	}
		
	int packSize(){

		// return sizeof(int) + length;

		return length;
	}
	void marshal(char* to){

		// int tmp = htonl(length);
		// memcpy(to, &tmp, sizeof(int));
		// memcpy(to + sizeof(int), string, length);

		memcpy(to, string, length);
	}
	void unmarshal(char* from){
		int tmp;
		memcpy(&tmp, from, sizeof(int));
		length = ntohl(tmp);
		string = new char[length];
		memcpy(string, from + sizeof(int), length);
	}
	static GeneralPtr* getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		if(arg != "string"){
			return NULL;
		}
		else if(name == "="){
			retType = "bool";
			return new GeneralPtr(stringEq, boolSize, oneOver100);
		}
		else if(name == "<"){
			retType = "bool";
			return new GeneralPtr(stringLT, boolSize, oneOver3);
		}
		else if(name == ">"){
			retType = "bool";
			return new GeneralPtr(stringGT, boolSize, oneOver3);
		}
		else{
			return NULL;
		}
	}
};

void displayAs(ostream& out, void* adt, String type);

int packSize(void* adt, String type);

int packSize(Tuple* tup, TypeID* types, int numFlds);

String rTreeEncode(String type);

void marshal(Type* adt, char* to, String type);

void marshal(Tuple* tup, char* to, TypeID* types, int numFlds);

Type* unmarshal(char* from, String type);

GeneralPtr* getOperatorPtr(
	String name, TypeID root, TypeID arg, TypeID& retType);

ReadPtr getReadPtr(TypeID root);

Type * getNullValue(TypeID &);

AttrType getDeviseType(String type);

Type* createNegInf(TypeID typeID);

Type* createPosInf(TypeID typeID);

#endif
