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
  Revision 1.16  1997/04/14 20:44:17  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.15  1997/04/10 21:50:31  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.14  1997/03/23 23:45:25  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.13  1997/03/14 18:36:14  donjerko
  Making space for the SQL UNION operator.

  Revision 1.12  1997/03/06 02:35:32  donjerko
  Undefined DEBUG

  Revision 1.11  1997/02/25 22:14:56  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.10  1997/02/18 18:06:08  donjerko
  Added skeleton files for sorting.

  Revision 1.9  1997/02/03 04:11:38  donjerko
  Catalog management moved to DTE

  Revision 1.8  1996/12/24 21:00:54  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.7  1996/12/21 22:21:52  donjerko
  Added hierarchical namespace.

  Revision 1.6  1996/12/16 11:13:12  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.5  1996/12/15 06:41:11  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/09 10:01:55  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

 */

#ifndef TYPES_H
#define TYPES_H

#include <iostream.h>
#include <assert.h>
#include <String.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <time.h>
#include "exception.h"
#include "AttrList.h"
#include "Utility.h"

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
public:
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
typedef Type* (*PromotePtr)(const Type*);
typedef Type* (*ADTCopyPtr)(const Type*);
typedef Type* (*MemberPtr)(Type*);
typedef void (*ReadPtr)(istream&, Type*&);
typedef void (*DestroyPtr)(Type*);
typedef void (*WritePtr)(ostream&, Type*);
typedef int (*SizePtr)(int, int);
typedef int (*MemberSizePtr)(int);
typedef double (*SelectyPtr)(BaseSelection* left, BaseSelection* right);
 
void insert(String tableStr, Tuple* tuple);	// throws exception

int domain(const TypeID adt);	// throws exception

int typeCompare(TypeID arg1, TypeID arg2);	// throws

Type* intCopy(const Type* arg);
Type* doubleCopy(const Type* arg);
Type* stringCopy(const Type* arg);

int boolSize(int a, int b);
int sameSize(int a, int b);
int memberSameSize(int a);

void intDestroy(Type* adt);
void stringDestroy(Type* adt);
void boolDestroy(Type* adt);
void doubleDestroy(Type* adt);
void catEntryDestroy(Type* adt);

struct GeneralPtr{
	OperatorPtr opPtr;
	SizePtr	sizePtr;
	SelectyPtr selectyPtr;
	GeneralPtr(OperatorPtr opPtr, 
		SizePtr sizePtr = NULL, SelectyPtr selectyPtr = NULL) : 
		opPtr(opPtr), sizePtr(sizePtr), selectyPtr(selectyPtr) {
		if(sizePtr == NULL){
			sizePtr = sameSize;
		}
	}
};

struct GeneralMemberPtr{
	MemberPtr memberPtr;
	MemberSizePtr	sizePtr;
	GeneralMemberPtr(MemberPtr opPtr, MemberSizePtr sizePtr = NULL) : 
		memberPtr(opPtr){
		if(sizePtr == NULL){
			sizePtr = memberSameSize;
		}
	}
};

double oneOver2(BaseSelection* left, BaseSelection* right);
double oneOver3(BaseSelection* left, BaseSelection* right);
double oneOver10(BaseSelection* left, BaseSelection* right);
double oneOver100(BaseSelection* left, BaseSelection* right);

Type* catEntryName(Type* arg1);
Type* catEntryType(Type* arg1);

Type* intAdd(Type* arg1, Type* arg2);
Type* intSub(Type* arg1, Type* arg2);
Type* intEq(Type* arg1, Type* arg2);
Type* intLT(Type* arg1, Type* arg2);
Type* intGT(Type* arg1, Type* arg2);
Type* intComp(Type *arg1,Type *arg2);

Type* dateEq(Type* arg1, Type* arg2);
Type* dateLT(Type* arg1, Type* arg2);
Type* dateGT(Type* arg1, Type* arg2);

Type* intToDouble(const Type* intarg);

Type* doubleAdd(Type* arg1, Type* arg2);
Type* doubleSub(Type* arg1, Type* arg2);
Type* doubleDiv(Type* arg1, Type* arg2);
Type* doubleEq(Type* arg1, Type* arg2);
Type* doubleLT(Type* arg1, Type* arg2);
Type* doubleGT(Type* arg1, Type* arg2);

Type* stringEq(Type* arg1, Type* arg2);
Type* stringLT(Type* arg1, Type* arg2);
Type* stringGT(Type* arg1, Type* arg2);
Type* boolOr(Type* arg1, Type* arg2);
Type* boolAnd(Type* arg1, Type* arg2);
Type* boolEq(Type* arg1, Type* arg2);
Type* boolLT(Type* arg1, Type* arg2);

void intRead(istream&, Type*&);
void stringRead(istream&, Type*&);
void doubleRead(istream&, Type*&);
void boolRead(istream&, Type*&);
void catEntryRead(istream&, Type*&);
void schemaRead(istream&, Type*&);
void indexDescRead(istream&, Type*&);

void intWrite(ostream&, Type*);
void stringWrite(ostream&, Type*);
void doubleWrite(ostream&, Type*);
void boolWrite(ostream&, Type*);
void catEntryWrite(ostream&, Type*);
void schemaWrite(ostream&, Type*);
void indexDescWrite(ostream&, Type*);

class IInt {
     int value;
public:
     IInt() {}
	IInt(const IInt& arg){
		value = arg.value;
	}
     IInt(int i) : value(i) {}
	int getValue(){
		return value;
	}
	void setValue(int i){
		value = i;
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
	}
     
	// This class is "intupled", meaning that the void* from the tuple
	// caries the value.
	// This solution is a bit dirty but the fast execution of
	// relational operators is critical for reasonable performance


public:
	static GeneralPtr* getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		if(arg != "int"){
			return NULL;
		}
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
		else if(name == "comp"){
			retType = "int";
			return new GeneralPtr(intComp, sameSize);
		}
		else{
			return NULL;
		}
	}
};

class IBool {

	// This class is "intupled", meaning that the void* from the tuple
	// caries the value.
	// This solution is a bit dirty but the fast execution of
	// relational operators is critical for reasonable performance

public:
	static GeneralPtr* getOperatorPtr(
		String name, TypeID arg, TypeID& retType){
		retType = "bool";
		if(arg != "bool"){
			return NULL;
		}
		else if(name == "or"){
			return new GeneralPtr(boolOr, boolSize, oneOver2);
		}
		else if(name == "and"){
			return new GeneralPtr(boolAnd, boolSize, oneOver2);
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
	IDouble(const IDouble& arg){
		value = arg.value;
	}
	double getValue(){
		return value;
	}
	void setValue(double val){
		value = val;
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
		else{
			return NULL;
		}
     }
};

/*
class IString {
     int length;
	char* string;
public:
     IString() : length(0), string(NULL){}
	IString(const char* s) : length(strlen(s) + 1){
		string = strdup(s);
	}
	~IString(){
		delete string;
	}
	void setValue(char *s){
		if (s)
			string = strdup(s);
		else
			string = NULL;
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
		out << addQuotes(string);
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
*/

class IString {
	char* string;
public:
     IString() : string(NULL){}
	IString(const char* s) {
		string = strdup(s);
	}
     IString(const String* s){
          string = strdup(s->chars());
     }
	IString(const IString& arg){
		string = strdup(arg.string);
	}
	~IString(){
		delete string;
	}
	void setValue(char *s){
		if (s)
			string = strdup(s);
		else
			string = NULL;
	}
	char* getValue(){
		return string;
	}
	IString(IString& str){
		string = strdup(str.string);
	}
	void display(ostream& out){
		assert(string);
		out << addQuotes(string);
	}
	IString& operator=(IString& str){
		string = strdup(str.string);
		return *this;
	}
	int packSize(){
		return strlen(string) + 1;
	}
	void marshal(char* to){
		strcpy(to, string);
	}
	void marshal(char* to, int len){
		strncpy(to, string, len);
		to[len - 1] = '\0';
	}
	void unmarshal(char* from){
		strcpy(string, from);
	}
	static GeneralPtr* getOperatorPtr(
		String name, TypeID root, TypeID arg, TypeID& retType){
		String msg = "No operator " + name + " (" + root + 
			", " + arg + ") defined";
		if(!arg.through(5).contains("string")){
			THROW(new Exception(msg), NULL);
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
			THROW(new Exception(msg), NULL);
		}
	}
};

class IDate {
	time_t date;
public:
     IDate() : date(0){}
	IDate(const time_t date) : date(date) {}
	IDate(const IDate& arg){
		date = arg.date;
	}
	~IDate(){}
	void setValue(time_t arg){
		date = arg;
	}
	time_t getValue(){
		return date;
	}
	void display(ostream& out){
		out << date;
	}
	IDate& operator=(IDate& arg){
		date = arg.date;
		return *this;
	}
	int packSize(){
		return sizeof(time_t);
	}
	void marshal(char* to){
		memcpy(to, &date, sizeof(time_t));
	}
	void unmarshal(char* from){
		memcpy(&date, from, sizeof(time_t));
	}
	static GeneralPtr* getOperatorPtr(
		String name, TypeID root, TypeID arg, TypeID& retType){
		String msg = 
			"No operator " + name + " (" + root + ", " + arg + ") defined";
		if(arg != "date"){
			THROW(new Exception(msg), NULL);
		}
		else if(name == "="){
			retType = "bool";
			return new GeneralPtr(dateEq, boolSize, oneOver100);
		}
		else if(name == "<"){
			retType = "bool";
			return new GeneralPtr(dateLT, boolSize, oneOver3);
		}
		else if(name == ">"){
			retType = "bool";
			return new GeneralPtr(dateGT, boolSize, oneOver3);
		}
		else{
			THROW(new Exception(msg), NULL);
		}
	}
};

class Site;
class Interface;

class CatEntry {
	String singleName;
	Interface* interface;
	String typeNm;
public:
	CatEntry(String singleName) : 
		singleName(singleName), interface(NULL) {}
	~CatEntry();
	istream& read(istream& in); // Throws Exception
	void display(ostream& out);
	Site* getSite();
	Interface* getInterface();	// Throws Exception
	static GeneralMemberPtr* getMemberPtr(String name, TypeID& retType);
		// throws exception
	String getName(){
		return singleName;
	}
	String getType(){
		return typeNm;
	}
};

class IndexDesc {
	int numKeyFlds;
	String* keyFlds;
	int numAddFlds;
	String* addFlds;
	bool pointer;
	int rootPg;
	TypeID* keyTypes;
	TypeID* addTypes;
public:
	IndexDesc(){
		numKeyFlds = 0;
		numAddFlds = 0;
		keyFlds = NULL;
		addFlds = NULL;
		keyTypes = NULL;
		addTypes = NULL;
	}
	IndexDesc(int numKeyFlds, String* keyFlds, 
		int numAddFlds, String* addFlds, 
		bool pointer, int rootPg,
		TypeID* keyTypes, TypeID* addTypes) :
		numKeyFlds(numKeyFlds), keyFlds(keyFlds),
		numAddFlds(numAddFlds), addFlds(addFlds),
		pointer(pointer), rootPg(rootPg),
		keyTypes(keyTypes), addTypes(addTypes){}
	~IndexDesc(){
		delete [] keyFlds;
		delete [] addFlds;
		delete [] keyTypes;
		delete [] addTypes;
	}
	istream& read(istream& in); // Throws Exception
	void display(ostream& out);
	int getNumKeyFlds(){
		return numKeyFlds;
	}
	int getRootPg(){
		return rootPg;
	}
	const String* getKeyFlds(){
		return keyFlds;
	}
	int getNumAddFlds(){
		return numAddFlds;
	}
	const String* getAddFlds(){
		return addFlds;
	}
	int getTotNumFlds(){
		return numKeyFlds + numAddFlds;
	}
	bool isStandAlone(){
		return !pointer;
	}
	String* getAllAttrNms(){
		int totFlds = getTotNumFlds();
		String* retVal = new String[totFlds];
		for(int i = 0; i < numKeyFlds; i++){
			retVal[i] = keyFlds[i];
		}
		for(int i = 0; i < numAddFlds; i++){
			retVal[i + numKeyFlds] = addFlds[i];
		}
		return retVal;
	}
	TypeID* getAllTypeIDs(){
		int totFlds = getTotNumFlds();
		String* retVal = new TypeID[totFlds];
		for(int i = 0; i < numKeyFlds; i++){
			retVal[i] = keyTypes[i];
		}
		for(int i = 0; i < numAddFlds; i++){
			retVal[i + numKeyFlds] = addTypes[i];
		}
		return retVal;
	}
};

class Schema {
	String* attributeNames;
	int numFlds;
public:
	Schema() : attributeNames(NULL), numFlds(0) {}
	Schema(String* attributeNames, int numFlds) :
		attributeNames(attributeNames), numFlds(numFlds) {}
	~Schema(){
		delete [] attributeNames;
	}
	istream& read(istream& in); // Throws Exception
	void display(ostream& out);
	int getNumFlds(){
		return numFlds;
	}
	String* getAttributeNames(){
		assert(attributeNames);
		String* retVal = attributeNames;
		attributeNames = NULL;
		return retVal;
	}
};

void displayAs(ostream& out, void* adt, String type);

int packSize(String type);    // throws exception

int packSize(const TypeID* types, int numFlds);    // throws exception

int packSize(void* adt, String type);

int packSize(Tuple* tup, TypeID* types, int numFlds);

String rTreeEncode(String type);

void marshal(Type* adt, char* to, String type);

void marshal(Tuple* tup, char* to, TypeID* types, int numFlds);

Type* unmarshal(char* from, String type);

GeneralPtr* getOperatorPtr(
	String name, TypeID root, TypeID arg, TypeID& retType);
	// throws exception

GeneralMemberPtr* getMemberPtr(String name, TypeID root, TypeID& retType);
	// throws exception

ReadPtr getReadPtr(TypeID root);
WritePtr getWritePtr(TypeID root);	// throws exception

Type * getNullValue(TypeID &);

AttrType getDeviseType(String type);

Type* createNegInf(TypeID typeID);

Type* createPosInf(TypeID typeID);

void destroyTuple(Tuple* tuple, int numFlds, DestroyPtr* destroyers); // throws

int tupleCompare(int *compare_flds, int num_compare_flds, 
       GeneralPtr **comparePtrs, Tuple *left, Tuple *right); 
// returns 1 if compare(left,right) = 1 on any compare_fld, else returns 0

DestroyPtr getDestroyPtr(TypeID root); // throws

PromotePtr getPromotePtr(TypeID from, TypeID to); // throws

ADTCopyPtr getADTCopyPtr(TypeID adt); // throws

void updateHighLow(int _numFlds, const OperatorPtr* lessPtrs, 
	const OperatorPtr* greaterPtrs, const Tuple* tup, 
	Tuple* highTup, Tuple* lowTup);



#endif
