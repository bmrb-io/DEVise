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
  Revision 1.23  1997/06/30 23:05:05  donjerko
  CVS:

  Revision 1.22  1997/06/27 23:17:23  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.21  1997/06/21 22:48:08  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.20  1997/06/16 16:04:55  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.19  1997/06/14 22:34:36  liping
  re-write min/max and recId request with SQL queries

  Revision 1.18  1997/05/30 15:49:59  arvind
  Added a "comp" operator for all data types. Semantics are same as for
  strcmp(left, right):
  Returns a neg num if left < right,
          zero      if left = right,
          a pos num if left > right.

  Revision 1.17  1997/04/18 20:46:29  donjerko
  Added function pointers to marshall types.

  Revision 1.16  1997/04/18 15:42:29  arvind
  Modified stringLT so it returns (cmp < 0) instead of (cmp == -1).
  Similarly for stringGT

  Revision 1.15  1997/04/14 20:44:17  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.14  1997/04/10 21:50:30  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.13  1997/03/23 23:45:25  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.12  1997/03/14 18:36:13  donjerko
  Making space for the SQL UNION operator.

  Revision 1.11  1997/03/06 02:35:31  donjerko
  Undefined DEBUG

  Revision 1.10  1997/02/25 22:14:55  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.9  1997/02/18 18:06:07  donjerko
  Added skeleton files for sorting.

  Revision 1.8  1997/02/03 04:11:37  donjerko
  Catalog management moved to DTE

  Revision 1.7  1996/12/24 21:00:54  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.6  1996/12/21 22:21:52  donjerko
  Added hierarchical namespace.

  Revision 1.5  1996/12/16 11:13:11  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.4  1996/12/15 06:41:11  donjerko
  Added support for RTree indexes

  Revision 1.3  1996/12/09 10:01:54  kmurli
  Changed DTe/Core to include the moving aggregate functions. Also included
  changes to the my.yacc and my.lex to add sequenceby clause.

  Revision 1.2  1996/12/05 16:06:05  wenger
  Added standard Devise file headers.

 */

#include "types.h"
#include "exception.h"
#include "myopt.h"		// for TableName
#include "catalog.h"	// for Interface
#include "Utility.h"
#include "DateTime.h"
#include <String.h>
#include <time.h>

void dateEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1);
	EncodedDTF* val2 = ((EncodedDTF*)arg2);
	assert(val1 && val2);
     result = (Type*)(*val1 == *val2);
}

void dateLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1);
	EncodedDTF* val2 = ((EncodedDTF*)arg2);
	assert(val1 && val2);
     result = (Type*)(*val1 < *val2);
}

void dateGT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1);
	EncodedDTF* val2 = ((EncodedDTF*)arg2);
	assert(val1 && val2);
     result = (Type*)(*val1 > *val2);
}

void dateComp(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1);
	EncodedDTF* val2 = ((EncodedDTF*)arg2);
	assert(val1 && val2);
     result = (Type*)(val1->compare(*val2));
}

void time_tEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	time_t* val1 = ((time_t*)arg1);
	time_t* val2 = ((time_t*)arg2);
	assert(val1 && val2);
     result = (Type*)(*val1 == *val2);
}

void time_tLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	time_t* val1 = ((time_t*)arg1);
	time_t* val2 = ((time_t*)arg2);
	assert(val1 && val2);
     result = (Type*)(*val1 < *val2);
}

void time_tGT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	time_t* val1 = ((time_t*)arg1);
	time_t* val2 = ((time_t*)arg2);
	assert(val1 && val2);
     result = (Type*)(*val1 > *val2);
}

void catEntryName(const Type* arg1, Type* result){
	CatEntry* entry = (CatEntry*) arg1;
	String tmp = entry->getName();
	strcpy((char*) result, tmp.chars());
}

void catEntryType(const Type* arg1, Type* result){
	CatEntry* entry = (CatEntry*) arg1;
	String tmp = entry->getType();
	strcpy((char*) result, tmp.chars());
}

void intAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*) (val1 + val2);
}

void intSub(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*) (val1 - val2);
}

void intEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*) (val1 == val2);
}

void intLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*)(val1 < val2);
}

void intLE(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*)(val1 <= val2);
}

void intGT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*)(val1 > val2);
}

void intGE(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*)(val1 >= val2);
}

void intComp(const Type *arg1,const Type *arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
	if(val1 > val2){
		result = (Type*) 1;
	}
	else if(val1 == val2){
		result = (Type*)(0);
	}
	else{
		result = (Type*)(-1);
	}
}

void intMin(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*)(val1 > val2);
}

void doubleAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	*((double*) result) = val1 + val2;
}

void doubleSub(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	*((double*) result) = val1 - val2;
}

void doubleDiv(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	*((double*) result) = val1 / val2;
}

void doubleEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	result = (Type*)(val1 == val2);
}

void doubleLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	result = (Type*)(val1 < val2);
}

void doubleGT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	result = (Type*)(val1 > val2);
}

void doubleComp(const Type *arg1,const Type *arg2, Type*& result, size_t& rsz){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	if(val1 > val2){
		result = (Type*) 1;
	}
	else if(val1 == val2){
		result = (Type*)(0);
	}
	else{
		result = (Type*)(-1);
	}
}

void boolOr(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
     result = (Type*)(arg1 || arg2);
}

void boolAnd(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
     result = (Type*)(arg1 && arg2);
}

void boolEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
     result = (Type*)(arg1 == arg2);
}

void boolLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
     result = (Type*)(arg1 < arg2);
}

void boolComp(const Type *arg1,const Type *arg2, Type*& result, size_t& rsz){
	if(arg1 > arg2){
		result = (Type*) 1;
	}
	else if(arg1 == arg2){
		result = (Type*)(0);
	}
	else{
		result = (Type*)(-1);
	}
}

void stringEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	char* val1 = (char*) arg1;
	char* val2 = (char*) arg2;
	int cmp = strcmp(val1, val2);
	result = (Type*)(cmp == 0);
}

void stringLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	char* val1 = (char*) arg1;
	char* val2 = (char*) arg2;
	int cmp = strcmp(val1, val2);
	result = (Type*)(cmp < 0);
}

void stringGT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	char* val1 = (char*) arg1;
	char* val2 = (char*) arg2;
	int cmp = strcmp(val1, val2);
	result = (Type*)(cmp > 0);
}

void stringComp(const Type *arg1,const Type *arg2, Type*& result, size_t& rsz){
	char* val1 = (char*) arg1;
	char* val2 = (char*) arg2;
	result = (Type*) strcmp(val1, val2);
}

void intRead(istream& in, Type*& adt){
	int tmp;
	in >> tmp;
	adt = (Type*) tmp;
}

void doubleRead(istream& in, Type*& adt){
	double i;
	in >> i;
	*((double*) adt) = i;
}

void stringRead(istream& in, Type*& adt){
	stripQuotes(in, (char*) adt, INITIAL_STRING_SIZE);
}

void boolRead(istream& in, Type*& adt){
	bool tmp;
	in >> tmp;
	adt = (Type*) tmp;
}

void catEntryRead(istream& in, Type*& adt){
	String nameStr;
	TRY(nameStr = stripQuotes(in), );
	if(!in){
		return;
	}
	*((CatEntry*) adt) = CatEntry(nameStr);
	TRY(((CatEntry*) adt)->read(in), );
}

void schemaRead(istream& in, Type*& adt){
	ISchema* tmp = new ISchema();
	memcpy(adt, tmp, sizeof(ISchema));
	TRY(((ISchema*) adt)->read(in), );
}

void indexDescRead(istream& in, Type*& adt){
	IndexDesc* tmp = new IndexDesc();	
	memcpy(adt, tmp, sizeof(IndexDesc));
	TRY(((IndexDesc*) adt)->read(in), );
}

void intWrite(ostream& out, const Type* adt){
	out << int(adt);
}

void stringWrite(ostream& out, const Type* adt){
	assert(adt);
	out << addQuotes((char*) adt);
}

void doubleWrite(ostream& out, const Type* adt){
	assert(adt);
	((IDouble*) adt)->display(out);
}

void boolWrite(ostream& out, const Type* adt){
	assert(adt);
	if(adt){
		out << true;
	}
	else{
		out << false;
	}
}

void catEntryWrite(ostream& out, const Type* adt){
	assert(adt);
	((CatEntry*) adt)->display(out);
}

void schemaWrite(ostream& out, const Type* adt){
	assert(adt);
	((ISchema*) adt)->display(out);
}

void indexDescWrite(ostream& out, const Type* adt){
	assert(adt);
	((IndexDesc*) adt)->display(out);
}

void dateWrite(ostream& out, const Type* adt){
	assert(adt);
	out << *((EncodedDTF*) adt);
}

void time_tWrite(ostream& out, const Type* adt){
	assert(adt);
	out << *((time_t*) adt);
}

int boolSize(int a, int b){
	return 1;
}

int sameSize(int a, int b){
	return a;
}

int memberSameSize(int a){
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

int packSize(const Type* adt, String type){
	if(type == "int"){
		return sizeof(int);
	}
	else if(type == "string"){
		return strlen((char*) adt) + 1;
	}
	else if(type == "double"){
		return ((IDouble*) adt)->packSize();
	}
	else if(type.through(5).contains("string")){
		int len = atoi(type.from(6).chars());
		return len;
	}
	else{
		cout << "Don't know how to pack type: " << type << endl;
		assert(0);
	}
}

int packSize(String type){	// throws exception
	if(type == "int"){
		return sizeof(int);
	}
	else if(type == "string"){
		THROW(new Exception("Type \"string\" is of variable size"), 0);
	}
	else if(type == "double"){
		return sizeof(IDouble);
	}
	else if(type.through(5).contains("string")){
		int len = atoi(type.from(6).chars()) + 1;
		return len;
	}
	else if(type == "date"){
		return sizeof(EncodedDTF);
	}
	else if(type == "time_t"){
		return sizeof(time_t);
	}
	else{
		String msg = "Don't know size of " + type; 
		assert(0);
		THROW(new Exception(msg), 0);
	}
}

void intMarshal(const Type* adt, char* to){
	memcpy(to, &adt, sizeof(int));
}

void stringMarshal(const Type* adt, char* to){
	strcpy(to, (char*) adt);
}

void doubleMarshal(const Type* adt, char* to){
	memcpy(to, adt, sizeof(double));
}

void dateMarshal(const Type* adt, char* to){
	memcpy(to, adt, sizeof(EncodedDTF));
}

void time_tMarshal(const Type* adt, char* to){
	memcpy(to, adt, sizeof(time_t));
}

void dateToUnixTime(const Type* adt, char* to){
	EncodedDTF* val = (EncodedDTF*) adt;
	struct tm tmp;
	val->maketm(tmp);
	time_t ut = mktime(&tmp);
	if(ut == (time_t) -1){
		cerr << "Failed to convert " << *val << " to unix time" << endl;
	}
	memcpy(to, &ut, sizeof(time_t));
}

MarshalPtr getMarshalPtr(String type){
	if(type == "int"){
		return intMarshal;
	}
	else if(type == "string"){
		return stringMarshal;
	}
	else if(type == "double"){
		return doubleMarshal;
	}
	else if(type == "date"){
		return dateMarshal;
	}
	else if(type == "time_t"){
		return time_tMarshal;
	}
	else if(type.through(5).contains("string")){

		// length needs to be passed for the binary data

		return stringMarshal;
	}
	else{
		cout << "Don't know how to marshal type: " << type << endl;
		assert(0);
	}
}

void intUnmarshal(char* from, Type*& adt){
	memcpy(&adt, from, sizeof(int));
}

void stringUnmarshal(char* from, Type*& adt){
	strcpy((char*) adt, from);
}

void doubleUnmarshal(char* from, Type*& adt){
	memcpy(adt, from, sizeof(IDouble));
}

void dateUnmarshal(char* from, Type*& adt){
	memcpy(adt, from, sizeof(EncodedDTF));
}

void time_tUnmarshal(char* from, Type*& adt){
	memcpy(adt, from, sizeof(time_t));
}

UnmarshalPtr getUnmarshalPtr(String type){
	if(type == "int"){
		return intUnmarshal;
	}
	else if(type == "string"){
		return stringUnmarshal;
	}
	else if(type == "double"){
		return doubleUnmarshal;
	}
	else if(type == "date"){
		return dateUnmarshal;
	}
	else if(type == "time_t"){
		return time_tUnmarshal;
	}
	else if(type.through(5).contains("string")){

		// length needs to be passed for the binary data

		return stringUnmarshal;
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
	else if(root.through(5).contains("string")){
		return IString::getOperatorPtr(name, root, arg, retType);
	}
	else if(root == "bool"){
		return IBool::getOperatorPtr(name, arg, retType);
	}
	else if(root == "double"){
		return IDouble::getOperatorPtr(name, arg, retType);
	}
	else if(root == "date"){
		return IDate::getOperatorPtr(name, root, arg, retType);
	}
	else if(root == "time_t"){
		return ITime_t::getOperatorPtr(name, root, arg, retType);
	}
	else{
		String msg = "Cannot find OperatorPtr for type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

GeneralMemberPtr* getMemberPtr(String name, TypeID root, TypeID& retType){
	String err;
	if(root == "int" || root.through(5).contains("string") || 
		root == "bool" || root == "double"){
		err = "Type " + root + " does not have a member \"" + name + "\"";
		THROW(new Exception(err), NULL);
	}
	else if(root == "catentry"){
		GeneralMemberPtr* retVal;
		TRY(retVal = CatEntry::getMemberPtr(name, retType), NULL);
		return retVal;
	}
	else{
		err = "Cannot find MemberPtr for type: " + root;
		THROW(new Exception(err), NULL);
	}
}

ReadPtr getReadPtr(TypeID root){
	if(root == "int"){
		return intRead;
	}
	else if(root.through(5).contains("string")){
		return stringRead;
	}
	else if(root == "bool"){
		return boolRead;
	}
	else if(root == "double"){
		return doubleRead;
	}
	else if(root == "catentry"){
		return catEntryRead;
	}
	else if(root == "schema"){
		return schemaRead;
	}
	else if(root == "indexdesc"){
		return indexDescRead;
	}
	else{
		cout << "No such type: " << root << endl;
		assert(0);
	}
}

WritePtr getWritePtr(TypeID root){
	if(root == "int"){
		return intWrite;
	}
	else if(root.through(5).contains("string")){
		return stringWrite;
	}
	else if(root == "bool"){
		return boolWrite;
	}
	else if(root == "double"){
		return doubleWrite;
	}
	else if(root == "catentry"){
		return catEntryWrite;
	}
	else if(root == "schema"){
		return schemaWrite;
	}
	else if(root == "indexdesc"){
		return indexDescWrite;
	}
	else if(root == "date"){
		return dateWrite;
	}
	else if(root == "time_t"){
		return time_tWrite;
	}
	else{
		String msg = "Cannot find WritePtr for type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

void zeroOut(Type*& arg, TypeID &root){
	if(root == "int"){
		arg = (Type*) 0;
	}
	else if(root.through(5).contains("string")){
		*((char*) arg) = '\0';
	}
	else if(root == "bool"){
		arg = (Type*) false;
	}
	else if(root == "double"){
		*((double*) arg) = 0;
	}
	else{
		cout << "Cannot zero out: " << root << endl;
		assert(0);
	}
}

String rTreeEncode(String type){
	if(type == "int"){
		return "i";
	}
	else if(type == "float"){
		return "f";
	}
	else if(type == "double"){
		return "d";
	}
	else if(type.through(5).contains("string")){
		String msg = "Don't know how to build index on type: " + type;
		THROW(new Exception(msg), "");
	}
	else{
		String msg = "Don't know how to build index on type: " + type;
		THROW(new Exception(msg), "");
	}
}

int packSize(const Tuple* tup, TypeID* types, int numFlds){
	int retVal = 0;
	for(int i = 0; i < numFlds; i++){
		retVal += packSize(tup[i], types[i]);
	}
	return retVal;
}

int packSize(const TypeID* types, int numFlds){	// throws exception
	int retVal = 0;
	for(int i = 0; i < numFlds; i++){
		TRY(retVal += packSize(types[i]), 0);
	}
	return retVal;
}

void marshal(const Tuple* tup, char* to, MarshalPtr* marshalPtrs, 
	int* sizes, int numFlds){
	int offset = 0;
	for(int i = 0; i < numFlds; i++){
		marshalPtrs[i](tup[i], to + offset);
		offset += sizes[i];
	}
}

Type* createNegInf(TypeID type){
	if(type == "int"){
		return (Type*)(INT_MIN);
	}
	else if(type == "double"){
		return new IDouble(-DBL_MAX);
	}
	else{
		String msg = "Don't know what is -Infinity for type: " + type;
		THROW(new Exception(msg), "");
	}
}

Type* createPosInf(TypeID type){
	if(type == "int"){
		return (Type*)(INT_MAX);
	}
	else if(type == "double"){
		return new IDouble(DBL_MAX);
	}
	else{
		String msg = "Don't know what is +Infinity for type: " + type;
		THROW(new Exception(msg), "");
	}
}

GeneralMemberPtr* CatEntry::getMemberPtr(String name, TypeID& retType){
	if(name == "name"){
		retType = "string";
		return new GeneralMemberPtr(catEntryName);
	}
	else if(name == "type"){
		retType = "string";
		return new GeneralMemberPtr(catEntryType);
	}
	else{
		String msg = "Type catentry does not have member \"" + name + "\"";
		THROW(new Exception(msg), NULL);
	}
}

Site* CatEntry::getSite(){
	return interface->getSite();
}

Interface* CatEntry::getInterface(){
	assert(interface);
	Interface* retVal = interface;
	interface = NULL;
	return retVal;
}

CatEntry::~CatEntry(){
	delete interface;
}

CatEntry& CatEntry::operator=(const CatEntry& a){
	if(this == &a){
		return (*this);
	}
	singleName = a.singleName;
//	assert(interface != a.interface);
	delete interface;
	interface = NULL;
	if(a.interface){
		interface = a.interface->duplicate();
	}
	typeNm = a.typeNm;
	return *this;
}

void CatEntry::display(ostream& out){
     out << addQuotes(singleName) << " " << typeNm;
     interface->write(out);
     out << " ;";
}

istream& CatEntry::read(istream& in){ // Throws Exception
	in >> typeNm;
	if(!in){
		String msg = "Interface for table " + singleName + 
			" must be specified";
		THROW(new Exception(msg), in);
	}
	if(typeNm == "Directory"){
		interface = new CatalogInterface();
		TRY(interface->read(in), in);
		String semicolon;
		in >> semicolon;
		if(semicolon != ";"){
			String msg = "Semicolon expected instead of: " + semicolon;
			THROW(new Exception(msg), in);
		}
		return in;
	}
	if(typeNm == "Table"){
		interface = new DeviseInterface(singleName);
		TRY(interface->read(in), in);
	}
	else if(typeNm == "StandardTable"){
		interface = new StandardInterface();
		TRY(interface->read(in), in);
	}
	else if(typeNm == "DEVise"){
		interface = new QueryInterface();
		TRY(interface->read(in), in);
	}
	else if(typeNm == "CGIInterface"){
		interface = new CGIInterface(singleName);
		TRY(interface->read(in), in);
	}
	else if(typeNm == "SQLView"){
		interface = new ViewInterface(singleName);
		TRY(interface->read(in), in);
	}
	else if(typeNm == "UNIXFILE"){
		interface = new DummyInterface();
		TRY(interface->read(in), in);
	}
	else{
		String msg = "Table " + singleName + " interface: " + 
			typeNm + ", not defined";
		THROW(new Exception(msg), in);
	}
	String indexStr;
	in >> indexStr;
	while(in && indexStr != ";"){
		String msg = 
			"Invalid catalog format: \"index\" or \";\" expected";
		THROW(new Exception(msg), in);
	}
	if(!in){
		String msg = "Premature end of catalog";
		THROW(new Exception(msg), in);
	}
	return in;
}

istream& IndexDesc::read(istream& in){
	if(!in){
		return in;
	}
	in >> numKeyFlds;
	if(!in){
		return in;
	}
	keyTypes = new TypeID[numKeyFlds];
	keyFlds = new String[numKeyFlds];
	for(int i = 0; i < numKeyFlds; i++){
		in >> keyTypes[i];
	}
	for(int i = 0; i < numKeyFlds; i++){
		in >> keyFlds[i];
	}
	in >> numAddFlds;
	if(!in){
		return in;
	}
	addFlds = new String[numAddFlds];
	addTypes = new TypeID[numAddFlds];
	for(int i = 0; i < numAddFlds; i++){
		in >> addTypes[i];
	}
	for(int i = 0; i < numAddFlds; i++){
		in >> addFlds[i];
	}
	in >> pointer;
	in >> rootPg;
	return in;
}

void IndexDesc::display(ostream& out){
	assert(out.good());
	out << numKeyFlds << " ";
	for(int i = 0; i < numKeyFlds; i++){
		out << keyTypes[i] << " ";
	}
	for(int i = 0; i < numKeyFlds; i++){
		out << keyFlds[i] << " ";
	}
	out << numAddFlds << " ";
	for(int i = 0; i < numAddFlds; i++){
		out << addTypes[i] << " ";
	}
	for(int i = 0; i < numAddFlds; i++){
		out << addFlds[i] << " ";
	}
	out << pointer << " ";
	out << rootPg;
}

istream& ISchema::read(istream& in){ // Throws Exception
	in >> numFlds;
	if(!in){
		return in;
	}
	attributeNames = new String[numFlds];
	for(int i = 0; i < numFlds; i++){
		in >> attributeNames[i];
	}
	return in;
}

void ISchema::display(ostream& out){
	out << numFlds;
	for(int i = 0; i < numFlds; i++){
		out << " " << attributeNames[i];
	}
}

void destroyTuple(Tuple* tuple, int numFlds, DestroyPtr* destroyers){ // throws
	assert(destroyers);
	for(int i = 0; i < numFlds; i++){
		destroyers[i](tuple[i]);
	}
	delete [] tuple;
}

void destroyTuple(Tuple* tuple, int numFlds, const TypeID* types){ // throws
	for(int i = 0; i < numFlds; i++){
		TRY(DestroyPtr destroyer = getDestroyPtr(types[i]), );
		destroyer(tuple[i]);
	}
}

int tupleCompare(int *compare_flds, int num_compare_flds, 
	GeneralPtr **comparePtrs, const Tuple *left, const Tuple *right)
{
  // Returns -1 if left < right 
  //          0 if left = right
  //          1 if left > right
  // Each comparePtrs is a GeneralPtr with name = "comp"

  assert(right);
  assert(left);

  Type* cmp;

  for(int i = 0; i < num_compare_flds; i++)
    {
      assert (compare_flds[i] >= 0 );
      comparePtrs[compare_flds[i]]->opPtr(left[compare_flds[i]],
						    right[compare_flds[i]], cmp);
      if (int(cmp) < 0) 
        return -1;

      if (int(cmp) > 0)
        return 1;
     } // continue checking only if the tuples are equal on current compare_fld

  return 0; // The tuples are equal on all comparison fields
}


DestroyPtr getDestroyPtr(TypeID root){ // throws
	if(root == "int"){
		return intDestroy;
	}
	else if(root.through(5).contains("string")){
		return stringDestroy;
	}
	else if(root == "bool"){
		return boolDestroy;
	}
	else if(root == "double"){
		return doubleDestroy;
	}
	else if(root == "catentry"){
		return catEntryDestroy;
	}
	else if(root == "indexdesc"){
		return indexDescDestroy;
	}
	else if(root == "time_t"){
		return time_tDestroy;
	}
	else{
		String msg = "Don't know how to destroy type: " + root;
		cout << msg << endl;
		exit(1);
		THROW(new Exception(msg), NULL);
	}
}

void intDestroy(Type* adt){
	// do nothing since the type is intupled
}

void time_tDestroy(Type* adt){
	delete (ITime_t*) adt;
}

void boolDestroy(Type* adt){
	// do nothing since the type is intupled
}

void doubleDestroy(Type* adt){
	delete (IDouble*) adt;
}

void stringDestroy(Type* adt){
	delete [] (char*) adt;
}

void catEntryDestroy(Type* adt){
	delete (CatEntry*) adt;
}

void indexDescDestroy(Type* adt){
	delete (IndexDesc*) adt;
}

void intToDouble(const Type* arg, Type*& result, size_t&){
	assert(result);
	*((double*) result) = int(arg);
}

void intToInt(const Type* arg, Type*& result, size_t&){
	((int) result) = int(arg);
}

void doubleToDouble(const Type* arg, Type*& result, size_t&){
	assert(result);
	*((double*) result) = *((double*)(arg));
}

void stringLToString(const Type* arg, Type*& result, size_t& sz){
	assert(result);
	strncpy((char*) result, (char*) arg, sz);
}

PromotePtr getPromotePtr(TypeID from, TypeID to){ // throws
	if(from == "int" && to == "double"){
		return intToDouble;
	}
	else if(from == "double" && to == "double"){
		return doubleToDouble;
	}
	else if(from.through(5).contains(STRING_TP) && to == STRING_TP){
		return stringLToString;
	}
	if(from == INT_TP && to == INT_TP){
		return intToInt;
	}
	else{
		String msg = String("Cannot promote ") + from + " to " + to;
		THROW(new Exception(msg), NULL);
	}
}

void intCopy(const Type* arg, Type*& result, size_t& sz){
	result = (Type*) arg;
}

void doubleCopy(const Type* arg, Type*& result, size_t& sz){
	*((double*) result) = *((double*) arg);
}

void dateCopy(const Type* arg, Type*& result, size_t& sz){
	*((EncodedDTF*) result) = *((EncodedDTF*) arg);
}

void stringCopy(const Type* arg, Type*& result, size_t& sz){
	strncpy((char*) result, (char*) arg, sz);
}

void time_tCopy(const Type* arg, Type*& result, size_t& sz){
	*((time_t*) result) = *((time_t*) arg);
}

ADTCopyPtr getADTCopyPtr(TypeID adt){ // throws
	if(adt == "int"){
		return intCopy;
	}
	else if(adt == "double"){
		return doubleCopy;
	}
	else if(adt == DATE_TP){
		return dateCopy;
	}
	else if(adt == "time_t"){
		return time_tCopy;
	}
	else if(adt.through(5).contains("string")){
		return stringCopy;
	}
	else{
		cerr << "Function to copy " << adt << " not implemented\n";
		assert(0);
		return NULL;
	}
}

void updateHighLow(int numFlds, const OperatorPtr* lessPtrs, 
	const OperatorPtr* greaterPtrs, const Tuple* tup, 
	Tuple* highTup, Tuple* lowTup){
	Type* result;
	for (int i = 0; i < numFlds; i++){
		lessPtrs[i](tup[i], lowTup[i], result);
		if(bool(result)){
			lowTup[i] = tup[i];
		}
		greaterPtrs[i](tup[i], highTup[i], result);
		if(bool(result)){
			highTup[i] = tup[i];
		}
	}
}

bool sameType(TypeID t1, TypeID t2){
	if(t1 == t2){
		return true;
	}
	else if(t1.through(5).contains(STRING_TP) && 
		t1.through(5).contains(STRING_TP)){

		return true;
	}
	else{
		return false;
	}
};

int domain(TypeID adt){	// throws exception
	if(adt.through(5).contains(STRING_TP) && adt != STRING_TP){
		return 0;
	}
	else if(adt == STRING_TP){
		return 1;
	}
	else if(adt == "date"){
		return 2;
	}
	else if(adt == "bool"){
		return 3;
	}
	else if(adt == "int"){
		return 4;
	}
	else if(adt == "float"){
		return 5;
	}
	else if(adt == "double"){
		return 6;
	}
	else{
		String msg = String("Type \"") + adt + 
			"\" is not listed in fn domain";
		THROW(new Exception(msg), 0);
	}
}

int typeCompare(TypeID arg1, TypeID arg2){	// throws
	if(arg1 == arg2){
		return 0;
	}
	TRY(int d1 = domain(arg1), 0);
	TRY(int d2 = domain(arg2), 0);
	return d1 - d2;
}

WritePtr* newWritePtrs(const TypeID* types, int numFlds){ // throws
	WritePtr* retVal = new WritePtr[numFlds];
	for(int i = 0; i < numFlds; i++){
		TRY(retVal[i] = getWritePtr(types[i]), NULL);
	}
	return retVal;
}

DestroyPtr* newDestroyPtrs(const TypeID* types, int numFlds){ // throws
	DestroyPtr* retVal = new DestroyPtr[numFlds];
	for(int i = 0; i < numFlds; i++){
		TRY(retVal[i] = getDestroyPtr(types[i]), NULL);
	}
	return retVal;
}

char* allocateSpace(TypeID type, size_t& size){
	if(type == "int" || type == "bool"){
		return NULL;
	}
	else if(type == "string"){
		size = INITIAL_STRING_SIZE;
		return new char[size];
	}
	else if(type == "double"){
		return (char*) new double;
	}
	else if(type.through(5).contains("string")){
		size = packSize(type);
		return new char[size];
	}
	else if(type == "date"){
		return (char*) new EncodedDTF();
	}
	else if(type == "time_t"){
		return (char*) new time_t;
	}
	else if(type == "catentry"){
		return (char*) new CatEntry();
	}
	else if(type == "schema"){
		return (char*) new ISchema();
	}
	else if(type == "indexdesc"){
		// return new char[sizeof(IndexDesc)];
		return (char*) new IndexDesc();
	}
	else{
		cout << "Don't know how to allocate space for " << type << endl; 
		assert(0);
		return NULL;
	}
}

MemoryLoader** newTypeLoaders(const TypeID* types, int numFlds){
	MemoryLoader** retVal = new MemoryLoader*[numFlds];
	for(int i = 0; i < numFlds; i++){
		if(types[i] == INT_TP){
			retVal[i] = new IntLoader();
		}
		else if(types[i] == DOUBLE_TP){
			retVal[i] = new DoubleLoader();
		}
		else{
			assert(!"loader not implemented for this type");
		}
	}
	return retVal;
}
