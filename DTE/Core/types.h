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
  Revision 1.20  1997/06/14 22:34:36  liping
  re-write min/max and recId request with SQL queries

  Revision 1.19  1997/05/30 15:49:59  arvind
  Added a "comp" operator for all data types. Semantics are same as for
  strcmp(left, right):
  Returns a neg num if left < right,
          zero      if left = right,
          a pos num if left > right.

  Revision 1.18  1997/04/18 20:46:31  donjerko
  Added function pointers to marshall types.

  Revision 1.17  1997/04/18 15:42:30  arvind
  Modified stringLT so it returns (cmp < 0) instead of (cmp == -1).
  Similarly for stringGT

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
#include "Utility.h"
#include "queue.h"

class BaseSelection;

const int INITIAL_STRING_SIZE = 4000;
// const char* ISO_TM = "%Y-%m-%d %H:%M:%S";

static size_t ds;	// used only as a place holder
const String UNKN_TYPE = "unknown";
const String INT_TP = "int";
const String DOUBLE_TP = "double";

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
typedef void (*OperatorPtr)(const Type*, const Type*, Type*&, size_t& = ds);
typedef void (*PromotePtr)(const Type*, Type*&, size_t& = ds);
typedef void (*ADTCopyPtr)(const Type*, Type*&, size_t& = ds);
typedef void (*MemberPtr)(const Type*, Type*);
typedef void (*ReadPtr)(istream&, Type*&);
typedef void (*DestroyPtr)(Type*);
typedef void (*WritePtr)(ostream&, const Type*);
typedef int (*SizePtr)(int, int);
typedef int (*MemberSizePtr)(int);
typedef double (*SelectyPtr)(BaseSelection* left, BaseSelection* right);
typedef void (*MarshalPtr)(const Type* adt, char* to);
typedef void (*UnmarshalPtr)(char* from, Type*& adt);
 
void insert(String tableStr, Tuple* tuple);	// throws exception

int domain(const TypeID adt);	// throws exception

int typeCompare(TypeID arg1, TypeID arg2);	// throws

void intCopy(const Type* arg, Type*& result, size_t& = ds);
void doubleCopy(const Type* arg, Type*& result, size_t& = ds);
void stringCopy(const Type* arg, Type*& result, size_t& = ds);

void intMarshal(const Type* adt, char* to);
void doubleMarshal(const Type* adt, char* to);
void stringMarshal(const Type* adt, char* to);
void dateMarshal(const Type* adt, char* to);

void intUnmarshal(char* from, Type*& adt);
void doubleUnmarshal(char* from, Type*& adt);
void stringUnmarshal(char* from, Type*& adt);
void dateUnmarshal(char* from, Type*& adt);
void tmUnmarshal(char* from, Type*& adt);

int boolSize(int a, int b);
int sameSize(int a, int b);
int memberSameSize(int a);

void intDestroy(Type* adt);
void stringDestroy(Type* adt);
void boolDestroy(Type* adt);
void doubleDestroy(Type* adt);
void catEntryDestroy(Type* adt);
void indexDescDestroy(Type* adt);

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

void catEntryName(const Type* arg1, Type* result);
void catEntryType(const Type* arg1, Type* result);

void intAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intSub(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intLE(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intGE(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void intComp(const Type *arg1,const Type *arg2, Type*& result, size_t& = ds);
void intMin(const Type *arg1,const Type *arg2, Type*& result, size_t& = ds);

void dateEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void dateLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void dateGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void dateComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);

void intToDouble(const Type* arg, Type*& result, size_t& = ds);
void doubleToDouble(const Type* arg, Type*& result, size_t& = ds);
void intToInt(const Type* arg, Type*& result, size_t& = ds);

void doubleAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void doubleSub(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void doubleDiv(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void doubleEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void doubleLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void doubleGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void doubleComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);

void stringEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void stringLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void stringGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void stringComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);

void boolOr(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void boolAnd(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void boolEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void boolLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);
void boolComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = ds);

void intRead(istream&, Type*&);
void stringRead(istream&, Type*&);
void doubleRead(istream&, Type*&);
void boolRead(istream&, Type*&);
void catEntryRead(istream&, Type*&);
void schemaRead(istream&, Type*&);
void indexDescRead(istream&, Type*&);

void intWrite(ostream&, const Type*);
void stringWrite(ostream&, const Type*);
void doubleWrite(ostream&, const Type*);
void boolWrite(ostream&, const Type*);
void catEntryWrite(ostream&, const Type*);
void schemaWrite(ostream&, const Type*);
void indexDescWrite(ostream&, const Type*);
void dateWrite(ostream& out, const Type* adt);

class IInt {
/*
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
*/
     
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
		else if(name == "<="){
			retType = "bool";
			return new GeneralPtr(intLE, boolSize, oneOver3);
                }
		else if(name == ">"){
			retType = "bool";
			return new GeneralPtr(intGT, boolSize, oneOver3);
		}
		else if(name == ">="){
                        retType = "bool";
                        return new GeneralPtr(intGE, boolSize, oneOver3);
                }
		else if(name == "comp"){
			retType = "int";
			return new GeneralPtr(intComp, sameSize);
		}
		else if(name == "min"){
			retType = "int";
			return new GeneralPtr(intMin, sameSize);
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
		else if(name == "comp"){
			retType = "int";
			return new GeneralPtr(boolComp, sameSize);
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
   		        else if(name == "comp"){
			        retType = "int";
			        return new GeneralPtr(doubleComp, sameSize);
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
	char string[0];
public:
/*
	void setValue(char *s){
		if (s)
			string = strdup(s);
		else
			string = NULL;
	}
	char* getValue(){
		return string;
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
	void unmarshal(char* from){
		strcpy(string, from);
	}
*/
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
		char tmstr[100];
		cftime(tmstr, NULL, &date);	// could be ISO_TIME
		out << tmstr;
	}
	IDate& operator=(IDate& arg){
		date = arg.date;
		return *this;
	}
     void unmarshal(char* from){
          memcpy(&date, from, sizeof(time_t));
     }
	int packSize(){
		return sizeof(time_t);
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
		else if(name == "comp"){
			retType = "int";
			return new GeneralPtr(dateComp, sameSize);
		}
		else{
			THROW(new Exception(msg), NULL);
		}
	}
};

/*
class IDateTime {
	time_t date;
public:
     IDateTime() : date(0){}
	IDateTime(const time_t date) : date(date) {}
	IDateTime(const IDateTime& arg){
		date = arg.date;
	}
	~IDateTime(){}
	void setValue(time_t arg){
		date = arg;
	}
	time_t getValue(){
		return date;
	}
	void display(ostream& out){
		out << date;
	}
	IDateTime& operator=(IDateTime& arg){
		date = arg.date;
		return *this;
	}
     void unmarshal(char* from){
          memcpy(&date, from, sizeof(time_t));
     }
	int packSize(){
		return sizeof(time_t);
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
		else if(name == "comp"){
		        retType = "int";
			return new GeneralPtr(dateComp, sameSize);
		}
		else{
			THROW(new Exception(msg), NULL);
		}
	}
};
*/

class Site;
class Interface;

class CatEntry {
	String singleName;
	Interface* interface;
	String typeNm;
public:
	CatEntry(String singleName = "") : 
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

class ISchema {
	String* attributeNames;
	int numFlds;
public:
	ISchema() : attributeNames(NULL), numFlds(0) {}
	ISchema(String* attributeNames, int numFlds) :
		attributeNames(attributeNames), numFlds(numFlds) {}
	ISchema(const ISchema& x){
		numFlds = x.numFlds;
		attributeNames = new String[numFlds];
		for(int i = 0; i < numFlds; i++){
			attributeNames[i] = x.attributeNames[i];
		}
	}
	~ISchema(){
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

class MemoryLoader {
	const size_t PAGE_SZ = 4 * 1024;
	size_t remainSpace;
	List<void*> pagePtrs;
	void* spacePtr;
public:
	MemoryLoader(){
		remainSpace = 0;
		spacePtr = NULL;
	}
	virtual ~MemoryLoader(){
		for(pagePtrs.rewind(); !pagePtrs.atEnd(); pagePtrs.step()){
			delete [] pagePtrs.get();
		}
	}
	void* allocate(size_t spaceNeed){
		void* retVal;	
		if(remainSpace >= spaceNeed){
			remainSpace -= spaceNeed;
		}
		else {
			if(!pagePtrs.atEnd()){
				spacePtr = pagePtrs.get();
			}
			else {
				spacePtr = (Type*) new char[PAGE_SZ];
				pagePtrs.append(spacePtr);
			}
			pagePtrs.step();
			remainSpace = PAGE_SZ;
		}
		retVal = spacePtr;
		spacePtr = (char*)(spacePtr) + spaceNeed;
		return retVal;
	}
	virtual void reset(){
		pagePtrs.rewind();
	}
	virtual Type* load(const Type*){
		assert(0);
		return NULL;
	}
};

class IntLoader : public MemoryLoader {
public:
	virtual Type* load(const Type* arg){
		return (Type*) arg;
	}
};

class DoubleLoader : public MemoryLoader {
public:
	virtual Type* load(const Type* arg){
		IDouble* retVal = (IDouble*) allocate(sizeof(IDouble));
		*retVal = *((IDouble*) arg);
		return retVal;
	}
};

MemoryLoader** newTypeLoaders(const TypeID* types, int numFlds);    // throws

class TupleLoader : public MemoryLoader {
	int numFlds;
	MemoryLoader** typeLoaders;
public:
	TupleLoader(){
		typeLoaders = NULL;
	}
	virtual ~TupleLoader(){
		delete [] typeLoaders;
	}
	void open(int numFlds, const TypeID* typeIDs){ // throws
		this->numFlds = numFlds;
		TRY(typeLoaders = newTypeLoaders(typeIDs, numFlds), );
	}
	Tuple* insert(const Tuple* tuple){ // throws
		size_t spaceNeed = numFlds * sizeof(Type*);
		Tuple* retVal = (Tuple*) allocate(spaceNeed);
		for(int i = 0; i < numFlds; i++){
			retVal[i] = typeLoaders[i]->load(tuple[i]);
		}
		return retVal;
	}
	virtual void reset(){
		for(int i = 0; i < numFlds; i++){
			typeLoaders[i]->reset();
		}
		MemoryLoader::reset();
	}
};

void displayAs(ostream& out, void* adt, String type);

int packSize(String type);    // throws exception

int packSize(const TypeID* types, int numFlds);    // throws exception

int packSize(const Type* adt, String type);

int packSize(const Tuple* tup, TypeID* types, int numFlds);

String rTreeEncode(String type);

MarshalPtr getMarshalPtr(String type);

UnmarshalPtr getUnmarshalPtr(String type);

void marshal(const Tuple* tup, char* to, MarshalPtr* marshalPtrs, 
	int* sizes, int numFlds);

Type* unmarshal(char* from, String type);

GeneralPtr* getOperatorPtr(
	String name, TypeID root, TypeID arg, TypeID& retType);
	// throws exception

GeneralMemberPtr* getMemberPtr(String name, TypeID root, TypeID& retType);
	// throws exception

ReadPtr getReadPtr(TypeID root);
WritePtr getWritePtr(TypeID root);	// throws exception

void zeroOut(Type*& arg, TypeID &root);

Type* createNegInf(TypeID typeID);

Type* createPosInf(TypeID typeID);

void destroyTuple(Tuple* tuple, int numFlds, DestroyPtr* destroyers); // throws

void destroyTuple(Tuple* tuple, int numFlds, const TypeID* types); // throws

int tupleCompare(int *compare_flds, int num_compare_flds, 
       GeneralPtr **comparePtrs, const Tuple *left, const Tuple *right); 
// returns 1 if compare(left,right) = 1 on any compare_fld, else returns 0

DestroyPtr getDestroyPtr(TypeID root); // throws

PromotePtr getPromotePtr(TypeID from, TypeID to); // throws

ADTCopyPtr getADTCopyPtr(TypeID adt); // throws

void updateHighLow(int _numFlds, const OperatorPtr* lessPtrs, 
	const OperatorPtr* greaterPtrs, const Tuple* tup, 
	Tuple* highTup, Tuple* lowTup);

WritePtr* newWritePtrs(const TypeID* types, int numFlds);	// throws
DestroyPtr* newDestroyPtrs(const TypeID* types, int numFlds);	// throws

char* allocateSpace(TypeID type, size_t& size = ds);

#endif
