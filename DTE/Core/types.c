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
#include <String.h>

Type* catEntryName(Type* arg1){
	CatEntry* entry = (CatEntry*) arg1;
	String tmp = entry->getName();
	return new IString(tmp.chars());
}

Type* catEntryType(Type* arg1){
	CatEntry* entry = (CatEntry*) arg1;
	String tmp = entry->getType();
	return new IString(tmp.chars());
}

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

Type* intDoubleDiv(Type* arg1, Type* arg2){
	int val1 = ((IInt*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IDouble(val1 / val2);
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

Type* doubleDiv(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return new IDouble(val1/val2);
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

Type* boolAnd(Type* arg1, Type* arg2){
	bool val1 = ((IBool*)arg1)->getValue();
	bool val2 = ((IBool*)arg2)->getValue();
     return new IBool(val1 && val2);
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
	String tmp = stripQuotes(in);
	return new IString(tmp.chars());
}

Type* boolRead(istream& in){
	bool i;
	in >> i;
	return new IBool(i);
}

Type* catEntryRead(istream& in){
	String nameStr;
	TRY(nameStr = stripQuotes(in), NULL);
	if(!in){
		return NULL;
	}
	CatEntry* retVal = new CatEntry(nameStr);
	retVal->read(in);
	return retVal;
}

Type* schemaRead(istream& in){
	Schema* retVal = new Schema();
	TRY(retVal->read(in), NULL);
     if(!in){
          delete retVal;
          return NULL;
     }
	return retVal;
}

Type* indexDescRead(istream& in){
	IndexDesc* retVal = new IndexDesc();
	TRY(retVal->read(in), NULL);
	return retVal;
}

void intWrite(ostream& out, Type* adt){
	assert(adt);
	((IInt*) adt)->display(out);
}

void stringWrite(ostream& out, Type* adt){
	assert(adt);
	((IString*) adt)->display(out);
}

void doubleWrite(ostream& out, Type* adt){
	assert(adt);
	((IDouble*) adt)->display(out);
}

void boolWrite(ostream& out, Type* adt){
	assert(adt);
	((IBool*) adt)->display(out);
}

void catEntryWrite(ostream& out, Type* adt){
	assert(adt);
	((CatEntry*) adt)->display(out);
}

void schemaWrite(ostream& out, Type* adt){
	assert(adt);
	((Schema*) adt)->display(out);
}

void indexDescWrite(ostream& out, Type* adt){
	assert(adt);
	((IndexDesc*) adt)->display(out);
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
	else if(type == "catentry"){
		((CatEntry*) adt)->display(out);
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
	else if(type.matches("string")){
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
		return sizeof(IInt);
	}
	else if(type == "string"){
		THROW(new Exception("Type \"string\" is of variable size"), 0);
	}
	else if(type == "double"){
		return sizeof(IDouble);
	}
	else if(type.matches("string")){
		int len = atoi(type.from(6).chars());
		return len;
	}
	else{
		String msg = "Don't know size of " + type; 
		THROW(new Exception(msg), 0);
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
	else if(type.matches("string")){
		int len = atoi(type.from(6).chars());
		((IString*) adt)->marshal(to, len);
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
	else if(type == "double"){
		IDouble* adt = new IDouble;
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
		String msg = "No such type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

GeneralMemberPtr* getMemberPtr(String name, TypeID root, TypeID& retType){
	String err;
	if(root == "int" || root == "string" || 
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
		err = "No such type: " + root;
		THROW(new Exception(err), NULL);
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
	else if(root == "string"){
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
	else{
		String msg = "No such type: " + root;
		THROW(new Exception(msg), NULL);
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

Type * getNullValue(TypeID &root){
	if(root == "int"){
		return new IInt(0);
	}
	else if(root == "string"){
		return new IString("");
	}
	else if(root == "bool"){
		return new IBool(false);
	}
	else if(root == "double"){
		return new IDouble(0);
	}
	else{
		cout << "No such type: " << root << endl;
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
	else if(type == "string"){
		String msg = "Don't know how to build index on type: " + type;
		THROW(new Exception(msg), "");
	}
	else{
		String msg = "Don't know how to build index on type: " + type;
		THROW(new Exception(msg), "");
	}
}

int packSize(Tuple* tup, TypeID* types, int numFlds){
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

void marshal(Tuple* tup, char* to, TypeID* types, int numFlds){
	int offset = 0;
	for(int i = 0; i < numFlds; i++){
		marshal(tup[i], to + offset, types[i]);
		offset += packSize(tup[i], types[i]);
	}
}

Type* createNegInf(TypeID type){
	if(type == "int"){
		return new IInt(INT_MIN);
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
		return new IInt(INT_MAX);
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

istream& Schema::read(istream& in){ // Throws Exception
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

void Schema::display(ostream& out){
	out << numFlds;
	for(int i = 0; i < numFlds; i++){
		out << " " << attributeNames[i];
	}
}

void destroyTuple(Tuple* tuple, int numFlds, DestroyPtr* destroyers){ // throws
	assert(destroyers);
	for(int i = 0; i < numFlds; i++){
		destroyers[i](tuple[i]);
		delete tuple[i];
	}
}

DestroyPtr getDestroyPtr(TypeID root){ // throws
	if(root == "int"){
		return intDestroy;
	}
	else if(root == "string"){
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
	else{
		String msg = "Don't know how to destroy type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

void intDestroy(Type* adt){
	delete (IInt*) adt;
}

void boolDestroy(Type* adt){
	delete (IBool*) adt;
}

void doubleDestroy(Type* adt){
	delete (IDouble*) adt;
}

void stringDestroy(Type* adt){
	delete (IString*) adt;
}

void catEntryDestroy(Type* adt){
	delete (CatEntry*) adt;
}

Type* intToDouble(const Type* intarg){
	return new IDouble(((IInt*) intarg)->getValue());
}

PromotePtr getPromotePtr(TypeID from, TypeID to){ // throws
	if(from == "int" && to == "double"){
		return intToDouble;
	}
	else{
		String msg = String("Cannot promote ") + from + " to " + to;
		THROW(new Exception(msg), NULL);
	}
}

Type* intCopy(const Type* arg){
	return new IInt(*((IInt*) arg));
}

Type* doubleCopy(const Type* arg){
	return new IDouble(*((IDouble*) arg));
}

Type* stringCopy(const Type* arg){
	return new IString(*((IString*) arg));
}

ADTCopyPtr getADTCopyPtr(TypeID adt){ // throws
	if(adt == "int"){
		return intCopy;
	}
	else if(adt == "double"){
		return doubleCopy;
	}
	else if(adt == "string"){
		return stringCopy;
	}
	else{
		String msg = String("No function to copy ") + adt;
		THROW(new Exception(msg), NULL);
	}
}

void updateHighLow(int numFlds, const OperatorPtr* lessPtrs, 
	const OperatorPtr* greaterPtrs, const Tuple* tup, 
	Tuple* highTup, Tuple* lowTup){
	for (int i = 0; i < numFlds; i++){
		if(((IBool*) lessPtrs[i](tup[i], lowTup[i]))->getValue()){
			lowTup[i] = tup[i];
		}
		if(((IBool*) greaterPtrs[i](tup[i], highTup[i]))->getValue()){
			highTup[i] = tup[i];
		}
	}
}
