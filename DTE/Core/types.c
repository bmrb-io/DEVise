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
#include <String.h>

Type* dateEq(Type* arg1, Type* arg2){
	time_t val1 = ((IDate*)arg1)->getValue();
	time_t val2 = ((IDate*)arg2)->getValue();
     return (Type*)(val1 == val2);
}

Type* dateLT(Type* arg1, Type* arg2){
	time_t val1 = ((IDate*)arg1)->getValue();
	time_t val2 = ((IDate*)arg2)->getValue();
     return (Type*)(val1 < val2);
}

Type* dateGT(Type* arg1, Type* arg2){
	time_t val1 = ((IDate*)arg1)->getValue();
	time_t val2 = ((IDate*)arg2)->getValue();
     return (Type*)(val1 > val2);
}

Type* dateComp(Type* arg1, Type* arg2){
	time_t val1 = ((IDate*)arg1)->getValue();
	time_t val2 = ((IDate*)arg2)->getValue();
	if(val1 > val2){
		return (Type*) 1;
	}
	else if(val1 == val2){
		return (Type*)(0);
	}
	else{
		return (Type*)(-1);
	}
}

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
	int val1 = int(arg1);
	int val2 = int(arg2);
     return (Type*) (val1 + val2);
}

Type* intSub(Type* arg1, Type* arg2){
	int val1 = int(arg1);
	int val2 = int(arg2);
     return (Type*) (val1 - val2);
}

Type* intEq(Type* arg1, Type* arg2){
	int val1 = int(arg1);
	int val2 = int(arg2);
     return (Type*) (val1 == val2);
}

Type* intLT(Type* arg1, Type* arg2){
	int val1 = int(arg1);
	int val2 = int(arg2);
     return (Type*)(val1 < val2);
}

Type* intLE(Type* arg1, Type* arg2){
        int val1 = int(arg1);
        int val2 = int(arg2);
     return (Type*)(val1 <= val2);
}

Type* intGT(Type* arg1, Type* arg2){
	int val1 = int(arg1);
	int val2 = int(arg2);
     return (Type*)(val1 > val2);
}

Type* intGE(Type* arg1, Type* arg2){
        int val1 = int(arg1);
        int val2 = int(arg2);
     return (Type*)(val1 >= val2);
}

Type* intComp(Type* arg1, Type* arg2){
	int val1 = int(arg1);
	int val2 = int(arg2);
	if(val1 > val2){
		return (Type*) 1;
	}
	else if(val1 == val2){
		return (Type*)(0);
	}
	else{
		return (Type*)(-1);
	}
}

Type* doubleAdd(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
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

Type* doubleEq(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return (Type*)(val1 == val2);
}

Type* doubleLT(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return (Type*)(val1 < val2);
}

Type* doubleGT(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
     return (Type*)(val1 > val2);
}

Type* doubleComp(Type* arg1, Type* arg2){
	double val1 = ((IDouble*)arg1)->getValue();
	double val2 = ((IDouble*)arg2)->getValue();
	if(val1 > val2){
		return (Type*) 1;
	}
	else if(val1 == val2){
		return (Type*)(0);
	}
	else{
		return (Type*)(-1);
	}
}

Type* boolEq(Type* arg1, Type* arg2){
     return (Type*)(arg1 == arg2);
}

Type* boolOr(Type* arg1, Type* arg2){
     return (Type*)(arg1 || arg2);
}

Type* boolAnd(Type* arg1, Type* arg2){
     return (Type*)(arg1 && arg2);
}

Type* boolLT(Type* arg1, Type* arg2){
     return (Type*)(arg1 < arg2);
}

Type* boolComp(Type* arg1, Type* arg2){
	if(arg1 > arg2){
		return (Type*) 1;
	}
	else if(arg1 == arg2){
		return (Type*)(0);
	}
	else{
   	        return (Type*)(-1);
	}
}

Type* stringEq(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	int cmp = strcmp(val1, val2);
	return (Type*)(cmp == 0);
}

Type* stringLT(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	int cmp = strcmp(val1, val2);
	return (Type*)(cmp < 0);
}

Type* stringGT(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	int cmp = strcmp(val1, val2);
	return (Type*)(cmp > 0);
}

Type* stringComp(Type* arg1, Type* arg2){
	char* val1 = ((IString*)arg1)->getValue();
	char* val2 = ((IString*)arg2)->getValue();
	return (Type*) strcmp(val1, val2);
}

void intRead(istream& in, Type*& adt){
	int tmp;
	in >> tmp;
	adt = (Type*) tmp;
}

void doubleRead(istream& in, Type*& adt){
	double i;
	in >> i;
	adt = new IDouble(i);
}

void stringRead(istream& in, Type*& adt){
	String tmp = stripQuotes(in);
	adt = new IString(tmp.chars());
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
	adt = new CatEntry(nameStr);
	TRY(((CatEntry*) adt)->read(in), );
}

void schemaRead(istream& in, Type*& adt){
	adt = new Schema();
	TRY(((Schema*) adt)->read(in), );
}

void indexDescRead(istream& in, Type*& adt){
	adt = new IndexDesc();
	TRY(((IndexDesc*) adt)->read(in), );
}

void intWrite(ostream& out, Type* adt){
	out << int(adt);
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
	if(adt){
		out << true;
	}
	else{
		out << false;
	}
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
		out << int(adt);
	}
	else if(type.through(5).contains("string")){
		((IString*) adt)->display(out);
	}
	else if(type == "bool"){
		boolWrite(out, adt);
	}
	else if(type == "double"){
		((IDouble*) adt)->display(out);
	}
	else if(type == "catentry"){
		((CatEntry*) adt)->display(out);
	}
	else if(type == "date"){
		((IDate*) adt)->display(out);
	}
	else{
		cout << "Don't know how to display type: " << type << endl;
		assert(0);
	}
}

int packSize(void* adt, String type){
	if(type == "int"){
		return sizeof(int);
	}
	else if(type == "string"){
		return ((IString*) adt)->packSize();
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
		int len = atoi(type.from(6).chars());
		return len;
	}
	else if(type == "date"){
		return sizeof(IDate);
	}
	else{
		String msg = "Don't know size of " + type; 
		assert(0);
		THROW(new Exception(msg), 0);
	}
}

void intMarshal(Type* adt, char* to){
	memcpy(to, &adt, sizeof(int));
}

void stringMarshal(Type* adt, char* to){
	strcpy(to, ((IString*) adt)->getValue());
}

void doubleMarshal(Type* adt, char* to){
	memcpy(to, adt, sizeof(double));
}

void dateMarshal(Type* adt, char* to){
	memcpy(to, adt, sizeof(time_t));
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
	else if(type.through(5).contains("string")){

		// length needs to be passed for the binary data
		// int len = atoi(type.from(6).chars());
		// ((IString*) adt)->marshal(to, len);

		return stringMarshal;
	}
	else{
		cout << "Don't know how to marshal type: " << type << endl;
		assert(0);
	}
}

Type* unmarshal(char* from, String type){
	if(type == "int"){
		int tmp;
		memcpy(&tmp, from, sizeof(int));
		return (Type*) tmp;
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
	else if(type == "date"){
		IDate* adt = new IDate;
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
	else{
		String msg = "Cannot fine WritePtr for type: " + root;
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
	else if(type.through(5).contains("string")){
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
		return (Type*)(0);
	}
	else if(root.through(5).contains("string")){
		return new IString("");
	}
	else if(root == "bool"){
		return (Type*)(false);
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
	else if(type.through(5).contains("string")){
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

void marshal(Tuple* tup, char* to, MarshalPtr* marshalPtrs, 
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

int tupleCompare(int *compare_flds, int num_compare_flds, 
                 GeneralPtr **comparePtrs, Tuple *left, Tuple *right)
{
  // Returns -1 if left < right 
  //          0 if left = right
  //          1 if left > right
  // Each comparePtrs is a GeneralPtr with name = "comp"

  for(int i = 0; i < num_compare_flds; i++)
    {
      assert (compare_flds[i] >= 0 );
      int cmp = int(comparePtrs[compare_flds[i]]->opPtr(left[compare_flds[i]],
						    right[compare_flds[i]]));
      if (cmp < 0) 
        return -1;

      if (cmp > 0)
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
	else{
		String msg = "Don't know how to destroy type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

void intDestroy(Type* adt){
	// do nothing since the type is intupled
}

void boolDestroy(Type* adt){
	// do nothing since the type is intupled
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
	return new IDouble(int(intarg));
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
	return (Type*) arg;
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
	else if(adt.through(5).contains("string")){
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
		if(lessPtrs[i](tup[i], lowTup[i])){
			lowTup[i] = tup[i];
		}
		if(greaterPtrs[i](tup[i], highTup[i])){
			highTup[i] = tup[i];
		}
	}
}

int domain(TypeID adt){	// throws exception
	if(adt.through(5).contains("string")){
		return 0;
	}
	else if(adt == "date"){
		return 1;
	}
	else if(adt == "bool"){
		return 2;
	}
	else if(adt == "int"){
		return 3;
	}
	else if(adt == "float"){
		return 4;
	}
	else if(adt == "double"){
		return 5;
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
	return (d1 > d2) ? 1 : -1;
}

