
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
  Revision 1.38  1997/10/06 22:10:23  donjerko
  *** empty log message ***

  Revision 1.37  1997/10/02 02:27:35  donjerko
  Implementing moving aggregates.

  Revision 1.36  1997/09/17 02:35:52  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.35  1997/09/05 22:20:23  donjerko
  Made changes for port to NT.

  Revision 1.34  1997/08/25 15:28:16  donjerko
  Added minmax table

  Revision 1.33  1997/08/22 23:13:07  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.32  1997/08/21 21:04:38  donjerko
  Implemented view materialization

  Revision 1.31  1997/08/15 21:19:14  donjerko
  Added / operator for ints and doubles

  Revision 1.30  1997/08/15 00:17:37  donjerko
  Completed the Iterator destructor code.

  Revision 1.29  1997/08/14 02:08:58  donjerko
  Index catalog is now an independent file.

  Revision 1.28  1997/08/12 19:58:47  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.27  1997/08/04 14:50:50  donjerko
  Fixed the bug in insert and delete queries.

  Revision 1.26  1997/07/30 21:39:28  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.25  1997/07/26 01:24:26  okan
  *** empty log message ***

  Revision 1.24  1997/07/22 15:00:58  donjerko
  *** empty log message ***

  Revision 1.23  1997/06/27 23:17:23  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.22  1997/06/21 22:48:09  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.21  1997/06/16 16:04:56  donjerko
  New memory management in exec phase. Unidata included.

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

#ifndef DTE_TYPES_H
#define DTE_TYPES_H

#include <string>

#include <assert.h>
#include <new.h>
#include "exception.h"
#include "Utility.h"
#include "queue.h"
#include "Array.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

#ifndef RecId_h
#define RecId_h

typedef unsigned long RecId;

#endif

struct ltstr { 
	bool operator()(const string& s1, const string& s2) const { 
		return s1 < s2;
	}
};

class BaseSelection;

const int INITIAL_STRING_SIZE = 2000;
extern const int INITIAL_INTERFACE_SIZE;	// defined in Interface.c

// const char* ISO_TM = "%Y-%m-%d %H:%M:%S";

static size_t dummySz;	// used only as a place holder
const string UNKN_TYPE = "unknown";
const string INT_TP = "int";
const string DOUBLE_TP = "double";
const string DATE_TP = "date";
const string INTERVAL_TP = "interval" ;
const string STRING_TP = "string";
const string INTERFACE_TP = "interface";
const string SEQSV_TP = "seqsv";
const string SCHEMA_TP = "schema";

const string RID_STRING = "recId";
const string SCHEMA_STR("schema");

class DteEnvVars {
public:
	string materViewDir;
	string minmaxDir;
	string rootCatalog;
	string indexTable;
	string minmaxCatalog;
public:
	DteEnvVars();
private:
	string getDirectory(const string& envVar);
	string getFile(const string& env, const string& def);
};

extern const DteEnvVars DTE_ENV_VARS;

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
typedef string TypeID;
typedef Type* Tuple;
typedef const Type* ConstTuple;
typedef void (*OperatorPtr)(const Type*, const Type*, Type*&, size_t& = dummySz);
typedef void (*PromotePtr)(const Type*, Type*&, size_t& = dummySz);
typedef void (*ADTCopyPtr)(const Type*, Type*&, size_t& = dummySz);
typedef void (*MemberPtr)(const Type*, Type*);
typedef void (*ReadPtr)(istream&, Type*&);
typedef void (*DestroyPtr)(Type*);
typedef void (*WritePtr)(ostream&, const Type*);
typedef int (*SizePtr)(int, int);
typedef int (*MemberSizePtr)(int);
typedef double (*SelectyPtr)(BaseSelection* left, BaseSelection* right);
typedef void (*MarshalPtr)(const Type* adt, char* to);
typedef void (*UnmarshalPtr)(char* from, Type*& adt);
typedef void (*ConstructorPtr)(const Array<const Type*>& inp, Type*& res, size_t& = dummySz);
 
void insert(string tableStr, Tuple* tuple);	// throws exception

int domain(const TypeID adt);	// throws exception

bool sameType(TypeID t1, TypeID t2);

int typeCompare(TypeID arg1, TypeID arg2);	// throws

void intCopy(const Type* arg, Type*& result, size_t& = dummySz);
void doubleCopy(const Type* arg, Type*& result, size_t& = dummySz);
void stringCopy(const Type* arg, Type*& result, size_t& = dummySz);
void dateCopy(const Type* arg, Type*& result, size_t& sz);
void intervalCopy(const Type* arg, Type*& result, size_t& sz);
void time_tCopy(const Type* arg, Type*& result, size_t& sz);

void intMarshal(const Type* adt, char* to);
void doubleMarshal(const Type* adt, char* to);
void stringMarshal(const Type* adt, char* to);
void dateMarshal(const Type* adt, char* to);
void intervalMarshal(const Type* adt, char* to);
void time_tMarshal(const Type* adt, char* to);

void dateToUnixTime(const Type* adt, char* to);

void intUnmarshal(char* from, Type*& adt);
void doubleUnmarshal(char* from, Type*& adt);
void stringUnmarshal(char* from, Type*& adt);
void dateUnmarshal(char* from, Type*& adt);
void intervalUnmarshal(char* from, Type*& adt);
void time_tUnmarshal(char* from, Type*& adt);

int boolSize(int a, int b);
int doubleSize(int a, int b);
int sameSize(int a, int b);
int memberSameSize(int a);

void intDestroy(Type* adt);
void time_tDestroy(Type* adt);
void dateDestroy(Type* adt);
void stringDestroy(Type* adt);
void boolDestroy(Type* adt);
void doubleDestroy(Type* adt);
void interfaceDestroy(Type* adt);
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

void interfaceType(const Type* arg1, Type* result);

void intAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intSub(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intLE(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intGE(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intComp(const Type *arg1,const Type *arg2, Type*& result, size_t& = dummySz);
void intMin(const Type *arg1,const Type *arg2, Type*& result, size_t& = dummySz);
void intDiv(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void dateEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void dateLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void dateGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void dateComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void time_tEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void time_tLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void time_tGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void intToDouble(const Type* arg, Type*& result, size_t& = dummySz);
void doubleToDouble(const Type* arg, Type*& result, size_t& = dummySz);
void intToInt(const Type* arg, Type*& result, size_t& = dummySz);
void stringLToString(const Type* arg, Type*& result, size_t& = dummySz);
void stringToStringL(const Type* arg, Type*& result, size_t& sz);

void seqSimilar(const Type* arg1, const Type* arg2, Type*& result, 
	size_t& = dummySz);

void doubleAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void doubleSub(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void doubleDiv(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void doubleEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void doubleLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void doubleGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void doubleComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void stringEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void stringLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void stringGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void stringComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void boolOr(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void boolAnd(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void boolEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void boolLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void boolComp(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void dateSubD(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void dateSubI(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void dateAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void intervalEq(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intervalLT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intervalGT(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intervalSub(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);
void intervalAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& = dummySz);

void intRead(istream&, Type*&);
void stringRead(istream&, Type*&);
void doubleRead(istream&, Type*&);
void boolRead(istream&, Type*&);
void interfaceRead(istream&, Type*&);
void schemaRead(istream&, Type*&);
void indexDescRead(istream&, Type*&);

void intWrite(ostream&, const Type*);
void stringWrite(ostream&, const Type*);
void doubleWrite(ostream&, const Type*);
void boolWrite(ostream&, const Type*);
void interfaceWrite(ostream&, const Type*);
void schemaWrite(ostream&, const Type*);
void indexDescWrite(ostream&, const Type*);
void dateWrite(ostream& out, const Type* adt);
void intervalWrite(ostream& out, const Type* adt);
void time_tWrite(ostream& out, const Type* adt);

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
		string name, TypeID arg, TypeID& retType){
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
		else if(name == "/"){
			retType = "int";
			return new GeneralPtr(intDiv, sameSize);
		}
		else{
			return NULL;
		}
	}
	static const int getInt(const Type* object){
		return (int) object;
	}
};

class SeqSimVec;

class ISeqSimVec {
//	SeqSimVec sv;
public:
	static GeneralPtr* getOperatorPtr(
		string name, TypeID arg, TypeID& retType){
		if(!(arg == SEQSV_TP)){
			return NULL;
		}
		if(name == "similar"){
			retType = "double";
			return new GeneralPtr(seqSimilar, doubleSize);
		}
		else{
			return NULL;
		}
	}
	static const SeqSimVec* getSeqSV(const Type* object){
		return (SeqSimVec*) object;
	}
};

class IBool {

	// This class is "intupled", meaning that the void* from the tuple
	// caries the value.
	// This solution is a bit dirty but the fast execution of
	// relational operators is critical for reasonable performance

public:
	static GeneralPtr* getOperatorPtr(
		string name, TypeID arg, TypeID& retType){
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
		string name, TypeID arg, TypeID& retType){
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
//	char string[0];
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
		string name, TypeID root, TypeID arg, TypeID& retType){
		string msg = "No operator " + name + " (" + root + 
			", " + arg + ") defined";
		if(!(arg.substr(0, 6) == "string")){
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
		else if(name == "comp"){
			retType = "bool";
			return new GeneralPtr(stringComp, boolSize, oneOver3);
		}
		else{
			THROW(new Exception(msg), NULL);
		}
	}
	static const char* getCStr(const Type* object){
		return (const char*)object;
	}
	static const Type* getTypePtr(const char* c){
		return (const Type*) c;
	}
};

class IDate {
public:
	static GeneralPtr* getOperatorPtr(
		string name, TypeID root, TypeID arg, TypeID& retType){
		string msg = 
			"No operator " + name + " (" + root + ", " + arg + ") defined";
          if(name == "-") {
               if (arg == "date") {
                    retType = "interval" ;
                    return new GeneralPtr(dateSubD, sameSize) ;
			}
               else if (arg == "interval") {
				retType = "date" ;
				return new GeneralPtr(dateSubI, sameSize) ;
			}
			else {
				THROW(new Exception(msg), NULL);
			}
		}
		if (name == "+") {
			if (arg == "interval") {
				retType = "date" ;
				return new GeneralPtr(dateAdd, sameSize) ;
			}
			else {
				THROW(new Exception(msg), NULL) ;
			}
		}
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
		else {
			THROW(new Exception(msg), NULL);
		}
	}
};

class IInterval {
public:
	static GeneralPtr* getOperatorPtr (
		string name, TypeID root, TypeID arg, TypeID& retType) {
		string msg =
			"No operator " + name + " (" + root + ", " + arg + ") defined" ;
		if (arg != "interval") {
			THROW(new Exception(msg), NULL);
		}
		else if (name == "=") {
			retType = "bool" ;
			return new GeneralPtr(intervalEq, boolSize, oneOver100) ;
		}
		else if (name == "<") {
			retType = "bool" ;
			return new GeneralPtr(intervalLT, boolSize, oneOver3) ;
		}
		else if (name == ">") {
			retType = "bool" ;
			return new GeneralPtr(intervalGT, boolSize, oneOver3) ;
		}
		else if (name == "-") {
			retType = "interval" ;
			return new GeneralPtr(intervalSub, sameSize) ;
		}
		else if (name == "+") {
			retType = "interval" ;
			return new GeneralPtr(intervalAdd, sameSize) ;
		}
		else {
			THROW(new Exception(msg), NULL) ;
		}
	}
};
	
class ITime_t {
public:
	static GeneralPtr* getOperatorPtr(
		string name, TypeID root, TypeID arg, TypeID& retType){
		string msg = 
			"No operator " + name + " (" + root + ", " + arg + ") defined";
		if(arg != "time_t"){
			THROW(new Exception(msg), NULL);
		}
		else if(name == "="){
			retType = "bool";
			return new GeneralPtr(time_tEq, boolSize, oneOver100);
		}
		else if(name == "<"){
			retType = "bool";
			return new GeneralPtr(time_tLT, boolSize, oneOver3);
		}
		else if(name == ">"){
			retType = "bool";
			return new GeneralPtr(time_tGT, boolSize, oneOver3);
		}
		/*
		else if(name == "comp"){
			retType = "int";
			return new GeneralPtr(dateComp, sameSize);
		}
		*/
		else{
			THROW(new Exception(msg), NULL);
		}
	}
};

class Site;
class Interface;

class InterfWrapper {
public:
	static GeneralMemberPtr* getMemberPtr(string name, TypeID& retType);
		// throws exception
	static const Interface* getInterface(const Type* object);
};

class IndexDesc {
	int numKeyFlds;
	string* keyFlds;
	int numAddFlds;
	string* addFlds;
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
	IndexDesc(int numKeyFlds, string* keyFlds, 
		int numAddFlds, string* addFlds, 
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
	int getNumKeyFlds() const {
		return numKeyFlds;
	}
	int getRootPg() const {
		return rootPg;
	}
	const string* getKeyFlds(){
		return keyFlds;
	}
	int getNumAddFlds() const {
		return numAddFlds;
	}
	const string* getAddFlds(){
		return addFlds;
	}
	int getTotNumFlds(){
		return numKeyFlds + numAddFlds;
	}
	bool isStandAlone(){
		return !pointer;
	}
	string* getAllAttrNms(){
		int totFlds = getTotNumFlds();
		int i ;
		string* retVal = new string[totFlds];
		for(i = 0; i < numKeyFlds; i++){
			retVal[i] = keyFlds[i];
		}
		for(i = 0; i < numAddFlds; i++){
			retVal[i + numKeyFlds] = addFlds[i];
		}
		return retVal;
	}
	TypeID* getAllTypeIDs(){
		int totFlds = getTotNumFlds();
		int i ;
		string* retVal = new TypeID[totFlds];
		for(i = 0; i < numKeyFlds; i++){
			retVal[i] = keyTypes[i];
		}
		for(i = 0; i < numAddFlds; i++){
			retVal[i + numKeyFlds] = addTypes[i];
		}
		return retVal;
	}
};

class ISchema {
	TypeID* typeIDs;
	string* attributeNames;
	int numFlds;
public:
	ISchema() : typeIDs(NULL), attributeNames(NULL), numFlds(0) {}
	ISchema(const string& str); 
	ISchema(TypeID* typeIDs, string* attributeNames, int numFlds) :
		typeIDs(typeIDs),
		attributeNames(attributeNames), 
		numFlds(numFlds) {}
	ISchema(int numFlds, const TypeID* typeIDs, const string* attributeNames); 
	ISchema(const ISchema& x);
	ISchema& operator=(const ISchema& x);
	ISchema operator+(const ISchema& x) const;
	~ISchema(){
		delete [] typeIDs;
		delete [] attributeNames;
	}
	istream& read(istream& in); // Throws Exception
	void write(ostream& out);
	int getNumFlds() const {
		return numFlds;
	}
	const string* getAttributeNames() const {
		assert(attributeNames);
		return attributeNames;
	}
	const TypeID* getTypeIDs() const {
		assert(typeIDs);
		return typeIDs;
	}
	static const ISchema* getISchema(const Type* arg){
		return (const ISchema*) arg;
	}
	friend istream& operator>>(istream& in, ISchema& s);
	friend ostream& operator<<(ostream& out, const ISchema& s);
};

const ISchema DIR_SCHEMA("2 string name interface interf");
const ISchema INDEX_SCHEMA("3 string table string name indexdesc descriptor");

int packSize(string type);    // throws exception

int packSize(const TypeID* types, int numFlds);    // throws exception

int packSize(const Type* adt, string type);

int packSize(const Tuple* tup, TypeID* types, int numFlds);

string rTreeEncode(string type);

MarshalPtr getMarshalPtr(string type);

UnmarshalPtr getUnmarshalPtr(string type);

void marshal(const Tuple* tup, char* to, MarshalPtr* marshalPtrs, 
	int* sizes, int numFlds);

GeneralPtr* getOperatorPtr(
	string name, TypeID root, TypeID arg, TypeID& retType);
	// throws exception

GeneralMemberPtr* getMemberPtr(string name, TypeID root, TypeID& retType);
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

Type* duplicateObject(TypeID type, Type* obj);   // throws

void updateHighLow(int _numFlds, const OperatorPtr* lessPtrs, 
	const OperatorPtr* greaterPtrs, const Tuple* tup, 
	Tuple* highTup, Tuple* lowTup);

WritePtr* newWritePtrs(const TypeID* types, int numFlds);	// throws
DestroyPtr* newDestroyPtrs(const TypeID* types, int numFlds);	// throws

char* allocateSpace(TypeID type, size_t& size = dummySz);

ConstructorPtr getConstructorPtr(
	const string& name, const TypeID* inpTypes, int numFlds, TypeID& retType);

#endif
