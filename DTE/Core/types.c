
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
  Revision 1.40  1997/11/19 00:19:19  donjerko
  Added ODBC skeleton.

  Revision 1.39  1997/11/13 22:19:26  okan
  Changes about compilation on NT

  Revision 1.38  1997/11/12 15:42:54  wenger
  Merged the cleanup_1_4_7_br branch through the cleanup_1_4_7_br_2 tag
  into the trunk.

  Revision 1.37.2.1  1997/10/30 20:13:40  wenger
  Got DEVise to compile, link, and run on sun and linux; compiles but
  doesn't link on hp (can't find memory mapping functions).  This includes
  moving the unidata config.h files to separate copies in the unidata
  directory for each architecture, since the config.h's are architecture-
  dependent.

  Revision 1.37  1997/10/06 22:10:11  donjerko
  *** empty log message ***

  Revision 1.36  1997/10/02 02:27:34  donjerko
  Implementing moving aggregates.

  Revision 1.35  1997/09/17 02:35:51  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.34  1997/09/05 22:20:21  donjerko
  Made changes for port to NT.

  Revision 1.33  1997/08/25 15:28:16  donjerko
  Added minmax table

  Revision 1.32  1997/08/22 23:13:06  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.31  1997/08/21 21:04:37  donjerko
  Implemented view materialization

  Revision 1.30  1997/08/15 21:19:13  donjerko
  Added / operator for ints and doubles

  Revision 1.29  1997/08/14 02:08:57  donjerko
  Index catalog is now an independent file.

  Revision 1.28  1997/08/12 19:58:46  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.27  1997/08/04 14:50:49  donjerko
  Fixed the bug in insert and delete queries.

  Revision 1.26  1997/07/30 21:39:27  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.25  1997/07/26 01:24:25  okan
  *** empty log message ***

  Revision 1.24  1997/07/22 15:00:58  donjerko
  *** empty log message ***

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

  Revision 1.10  1997/02/25 22:14:55  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

 */

#include "sysdep.h"
#include "types.h"
#include "exception.h"
#include "Interface.h"
#include "Utility.h"
#include "DateTime.h"
#include "catalog.h" 	// for root catalog
#include <string>

#if defined(SUN) || defined(LINUX)
#  include <values.h>
#  define DBL_MAX MAXDOUBLE
#endif

const DteEnvVars DTE_ENV_VARS;

extern const Catalog ROOT_CATALOG(DTE_ENV_VARS.rootCatalog);

extern const Directory MINMAX_DIR(DTE_ENV_VARS.minmaxCatalog);

string DteEnvVars::getDirectory(const string& envVar){
	const char* dmd = NULL;
	if((dmd = getenv(envVar.c_str()))){
	}
	else if((dmd = getenv("PWD"))){
	}
	else{
		string err("Could not read env. var ");
		err += envVar + " nor PWD";
		cerr << err << endl;
		exit(1);
	}
	assert(dmd);
	return dmd;
}

string DteEnvVars::getFile(const string& env, const string& def){
	char* nm = getenv(env.c_str());
	if(nm){
		return string(nm);
	}
	else{
		return def;
	}
}

DteEnvVars::DteEnvVars(){
	materViewDir = getDirectory("DEVISE_MATER_DIR");
	minmaxDir = getDirectory("DEVISE_MINMAX_DIR");
	rootCatalog = getFile("DEVISE_HOME_TABLE", "./catalog.dte");
	indexTable = getFile("DEVISE_INDEX_TABLE", "./sysind.dte");
	minmaxCatalog = getFile("DEVISE_MINMAX_TABLE", "./minmax.dte");
}

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

void dateSubD(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1) ;
	EncodedDTF* val2 = ((EncodedDTF*)arg2) ;
	*((EncodedIDT*) result) = (*val1 - *val2) ;
}

void dateSubI(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	*((EncodedDTF*) result) = (*val1 - *val2) ;
}

void dateAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedDTF* val1 = ((EncodedDTF*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	*((EncodedDTF*) result) = (*val1 + *val2) ;
}

void seqSimilar(const Type* arg1, const Type* arg2, Type*& result, size_t&){
	SeqSimVec* val1 = ((SeqSimVec*)arg1) ;
	SeqSimVec* val2 = ((SeqSimVec*)arg2) ;
//	*((double*) result) = val1->similar(*val2);
}

void intervalEq(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedIDT* val1 = ((EncodedIDT*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	assert(val1 && val2) ;
	result = (Type*)(*val1 == *val2) ;
}

void intervalLT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedIDT* val1 = ((EncodedIDT*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	assert(val1 && val2) ;
	result = (Type*)(*val1 < *val2) ;
}

void intervalGT(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedIDT* val1 = ((EncodedIDT*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	assert(val1 && val2) ;
	result = (Type*)(*val1 > *val2) ;
}
void intervalAdd(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedIDT* val1 = ((EncodedIDT*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	*((EncodedIDT*) result) = (*val1 + *val2) ;
}

void intervalSub(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	EncodedIDT* val1 = ((EncodedIDT*)arg1) ;
	EncodedIDT* val2 = ((EncodedIDT*)arg2) ;
	*((EncodedIDT*) result) = (*val1 - *val2) ;
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

void interfaceType(const Type* arg1, Type* result){
	Interface* interf = (Interface*) arg1;
	string tmp = interf->getTypeNm();
	strcpy((char*) result, tmp.c_str());
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

void intDiv(const Type* arg1, const Type* arg2, Type*& result, size_t& rsz){
	int val1 = int(arg1);
	int val2 = int(arg2);
     result = (Type*)(val1 / val2);
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

void time_tRead(istream& in, Type*& adt){
	time_t x;
	in >> x;
	*((time_t*) adt) = x;
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

void interfaceRead(istream& in, Type*& adt){
	if(!in){
		return;
	}
	string typeNm;
	in >> typeNm;
	if(!in){
		string msg = "Interface name must be specified";
		THROW(new Exception(msg), NVOID );
	}
	((Interface*) adt)->~Interface();	// calls the right destructor
	Interface* interf;
	if(typeNm == CatalogInterface::typeName){
		interf = new (adt) CatalogInterface();
		TRY(interf->read(in), NVOID );
		string semicolon;
		in >> semicolon;
		if(semicolon != ";"){
			string msg = "Semicolon expected instead of: " + semicolon;
			THROW(new Exception(msg), NVOID );
		}
		return;
	}
	if(typeNm == DeviseInterface::typeName){
		interf = new (adt) DeviseInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == StandardInterface::typeName){
		interf = new (adt) StandardInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == QueryInterface::typeName){
		interf = new (adt) QueryInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == CGIInterface::typeName){
		interf = new (adt) CGIInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == ViewInterface::typeName){
		interf = new (adt) ViewInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == DummyInterface::typeName){
		interf = new (adt) DummyInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == MaterViewInterface::typeName){
		interf = (ViewInterface*) new (adt) MaterViewInterface();
		TRY(interf->read(in), NVOID );
	}
	else if(typeNm == ODBCInterface::typeName){
		interf = (ODBCInterface*) new (adt) ODBCInterface();
		TRY(interf->read(in), NVOID );
	}
	else{
		interf = NULL;
		string msg = "Interface type: " + typeNm + " not defined";
		THROW(new Exception(msg), NVOID );
	}
	string indexStr;
	in >> indexStr;
	while(in && indexStr != ";"){
		string msg = 
			"Invalid catalog format: \"index\" or \";\" expected";
		THROW(new Exception(msg), NVOID );
	}
	if(!in){
		string msg = "Premature end of catalog";
		THROW(new Exception(msg), NVOID );
	}
}

void schemaRead(istream& in, Type*& adt){
	TRY(((ISchema*) adt)->read(in), NVOID );
}

void dateRead(istream& in, Type*& adt){
	EncodedDTF* tmp = (EncodedDTF*) adt;
	in >> *tmp;
}

void indexDescRead(istream& in, Type*& adt){
	IndexDesc* tmp = new IndexDesc();	
	memcpy(adt, tmp, sizeof(IndexDesc));
	TRY(((IndexDesc*) adt)->read(in), NVOID );
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

void interfaceWrite(ostream& out, const Type* adt){
	assert(adt);
	((Interface*) adt)->write(out);
}

void schemaWrite(ostream& out, const Type* adt){
	assert(adt);
	((ISchema*) adt)->write(out);
}

void indexDescWrite(ostream& out, const Type* adt){
	assert(adt);
	((IndexDesc*) adt)->display(out);
}

void dateWrite(ostream& out, const Type* adt){
	assert(adt);
	out << *((EncodedDTF*) adt);
}

void intervalWrite(ostream& out, const Type* adt){
	assert(adt);
	out << *((EncodedIDT*) adt);
}

void time_tWrite(ostream& out, const Type* adt){
	assert(adt);
	out << *((time_t*) adt);
}

int boolSize(int a, int b){
	return 1;
}

int doubleSize(int a, int b){
	return 4;
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

int packSize(const Type* adt, string type){
	if(type == "int"){
		return sizeof(int);
	}
	else if(type == "string"){
		return strlen((char*) adt) + 1;
	}
	else if(type == "double"){
		return ((IDouble*) adt)->packSize();
	}
	else if(type.substr(0, 6) == "string"){
		int len = atoi(type.substr(6).c_str());
		return len;
	}
	else{
		cout << "Don't know how to pack type: " << type << endl;
		assert(0);
		return 0;
	}
}

int packSize(string type){	// throws exception
	if(type == "int"){
		return sizeof(int);
	}
	else if(type == "string"){
		THROW(new Exception("Type \"string\" is of variable size"), 0);
	}
	else if(type == "double"){
		return sizeof(IDouble);
	}
	else if(type.substr(0, 6) == "string"){
		int len = atoi(type.substr(6).c_str()) + 1;
		return len;
	}
	else if(type == "date"){
		return sizeof(EncodedDTF);
	}
	else if(type == "interval") {
		return sizeof(EncodedIDT) ;
	}
	else if(type == "time_t"){
		return sizeof(time_t);
	}
	else{
		string msg = "Don't know size of " + type; 
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

void intervalMarshal(const Type* adt, char* to){
	memcpy(to, adt, sizeof(EncodedIDT));
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

MarshalPtr getMarshalPtr(string type){
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
	else if(type == "interval") {
		return intervalMarshal ;
	}
	else if(type == "time_t"){
		return time_tMarshal;
	}
	else if(type.substr(0, 6) == "string"){

		// length needs to be passed for the binary data

		return stringMarshal;
	}
	else{
		cout << "Don't know how to marshal type: " << type << endl;
		assert(0);
		return NULL;
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

void intervalUnmarshal(char* from, Type*& adt){
	memcpy(adt, from, sizeof(EncodedIDT));
}

void time_tUnmarshal(char* from, Type*& adt){
	memcpy(adt, from, sizeof(time_t));
}

UnmarshalPtr getUnmarshalPtr(string type){
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
	else if(type == "interval"){
		return intervalUnmarshal;
	}
	else if(type == "time_t"){
		return time_tUnmarshal;
	}
	else if(type.substr(0, 6) == "string"){

		// length needs to be passed for the binary data

		return stringUnmarshal;
	}
	else{
		cout << "Don't know how to unmarshal type: " << type << endl;
		assert(0);
		return NULL;
	}
}

GeneralPtr* getOperatorPtr(
	string name, TypeID root, TypeID arg, TypeID& retType){
	if(root == "int"){
		return IInt::getOperatorPtr(name, arg, retType);
	}
	else if(root.substr(0, 6) == "string"){
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
	else if(root == "interval"){
		return IInterval::getOperatorPtr(name, root, arg, retType);
	}
	else if(root == "time_t"){
		return ITime_t::getOperatorPtr(name, root, arg, retType);
	}
	else{
		string msg = "Cannot find OperatorPtr for type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

GeneralMemberPtr* getMemberPtr(string name, TypeID root, TypeID& retType){
	string err;
	if(root == "int" || root.substr(0, 6) == "string" || 
		root == "bool" || root == "double"){
		err = "Type " + root + " does not have a member \"" + name + "\"";
		THROW(new Exception(err), NULL);
	}
	else if(root == "interface"){
		GeneralMemberPtr* retVal;
		TRY(retVal = InterfWrapper::getMemberPtr(name, retType), NULL);
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
	else if(root.substr(0, 6) == "string"){
		return stringRead;
	}
	else if(root == "bool"){
		return boolRead;
	}
	else if(root == "double"){
		return doubleRead;
	}
	else if(root == "interface"){
		return interfaceRead;
	}
	else if(root == "schema"){
		return schemaRead;
	}
	else if(root == "indexdesc"){
		return indexDescRead;
	}
	else if(root == "time_t"){
		return time_tRead;
	}
	else if(root == DATE_TP){
		return dateRead;
	}
	else{
		cout << "No such type: " << root << endl;
		assert(0);
		return NULL;
	}
}

WritePtr getWritePtr(TypeID root){
	if(root == "int"){
		return intWrite;
	}
	else if(root.substr(0, 6) == "string"){
		return stringWrite;
	}
	else if(root == "bool"){
		return boolWrite;
	}
	else if(root == "double"){
		return doubleWrite;
	}
	else if(root == "interface"){
		return interfaceWrite;
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
	else if(root == "interval"){
		return intervalWrite;
	}
	else if(root == "time_t"){
		return time_tWrite;
	}
	else{
		string msg = "Cannot find WritePtr for type: " + root;
		THROW(new Exception(msg), NULL);
	}
}

void zeroOut(Type*& arg, TypeID &root){
	if(root == "int"){
		arg = (Type*) 0;
	}
	else if(root.substr(0, 6) == "string"){
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

string rTreeEncode(string type){
	if(type == "int"){
		return "i";
	}
	else if(type == "float"){
		return "f";
	}
	else if(type == "double"){
		return "d";
	}
	else if(type.substr(0, 6) == "string"){
		ostringstream tmp;
		tmp << "s[" << packSize(type) << "]" << ends;
		string retVal = tmp.str() ;
		return retVal;
	}
	else{
		string msg = "Don't know how to build index on type: " + type;
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
	else if(type.substr(0, 6) == "string"){
		return strdup("");
	}
	else{
		string msg = "Don't know what is -Infinity for type: " + type;
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
	else if(type.substr(0, 6) == "string"){
		return strdup("zzzzzzzzzzzz");
	}
	else{
		string msg = "Don't know what is +Infinity for type: " + type;
		THROW(new Exception(msg), "");
	}
}

GeneralMemberPtr* InterfWrapper::getMemberPtr(string name, TypeID& retType){
	if(name == "type"){
		retType = "string";
		return new GeneralMemberPtr(interfaceType);
	}
	else{
		string msg = "Type interface does not have member \"" + name + "\"";
		THROW(new Exception(msg), NULL);
	}
}

const Interface* InterfWrapper::getInterface(const Type* object){
	return (const Interface*) object;
}

istream& IndexDesc::read(istream& in){
	if(!in){
		return in;
	}
	in >> numKeyFlds;
	if(!in){
		return in;
	}
	int i;
	keyTypes = new TypeID[numKeyFlds];
	keyFlds = new string[numKeyFlds];
	for(i = 0; i < numKeyFlds; i++){
		in >> keyTypes[i];
	}
	for(i = 0; i < numKeyFlds; i++){
		in >> keyFlds[i];
	}
	in >> numAddFlds;
	if(!in){
		return in;
	}
	addFlds = new string[numAddFlds];
	addTypes = new TypeID[numAddFlds];
	for(i = 0; i < numAddFlds; i++){
		in >> addTypes[i];
	}
	for(i = 0; i < numAddFlds; i++){
		in >> addFlds[i];
	}
	in >> pointer;
	in >> rootPg;
	return in;
}

void IndexDesc::display(ostream& out){
	int i;
	assert(out.good());
	out << numKeyFlds << " ";
	for(i = 0; i < numKeyFlds; i++){
		out << keyTypes[i] << " ";
	}
	for(i = 0; i < numKeyFlds; i++){
		out << keyFlds[i] << " ";
	}
	out << numAddFlds << " ";
	for(i = 0; i < numAddFlds; i++){
		out << addTypes[i] << " ";
	}
	for(i = 0; i < numAddFlds; i++){
		out << addFlds[i] << " ";
	}
	out << pointer << " ";
	out << rootPg;
}

istream& ISchema::read(istream& in){ // Throws Exception
	return in >> *this;
}

void ISchema::write(ostream& out){
	out << *this;
}

ISchema::ISchema
	(int numFlds, const TypeID* typeIDs, const string* attributeNames)
{
	int i;
	this->numFlds = numFlds;
	this->typeIDs = new TypeID[numFlds];
	for(i = 0; i < numFlds; i++){
		this->typeIDs[i] = typeIDs[i];
	}
	this->attributeNames = new string[numFlds];
	for(i = 0; i < numFlds; i++){
		this->attributeNames[i] = attributeNames[i];
	}
}

ISchema& ISchema::operator=(const ISchema& x){
	if(this == &x){
		return *this;
	}
	numFlds = x.numFlds;
	if(attributeNames != x.attributeNames){
		delete attributeNames;
	}
	if(x.attributeNames){
		attributeNames = new string[numFlds];
		for(int i = 0; i < numFlds; i++){
			attributeNames[i] = x.attributeNames[i];
		}
	}
	else{
		attributeNames = NULL;
	}
	if(typeIDs != x.typeIDs){
		delete typeIDs;
	}
	if(x.typeIDs){
		typeIDs = new TypeID[numFlds];
		for(int i = 0; i < numFlds; i++){
			typeIDs[i] = x.typeIDs[i];
		}
	}
	else{
		typeIDs = NULL;
	}
	return *this;
}

ISchema::ISchema(const ISchema& x){
	numFlds = x.numFlds;
	if(x.attributeNames){
		attributeNames = new string[numFlds];
		for(int i = 0; i < numFlds; i++){
			attributeNames[i] = x.attributeNames[i];
		}
	}
	else{
		attributeNames = NULL;
	}
	if(x.typeIDs){
		typeIDs = new TypeID[numFlds];
		for(int i = 0; i < numFlds; i++){
			typeIDs[i] = x.typeIDs[i];
		}
	}
	else{
		typeIDs = NULL;
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
		TRY(DestroyPtr destroyer = getDestroyPtr(types[i]), NVOID );
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
	else if(root.substr(0, 6) == "string"){
		return stringDestroy;
	}
	else if(root == "bool"){
		return boolDestroy;
	}
	else if(root == "double"){
		return doubleDestroy;
	}
	else if(root == "interface"){
		return interfaceDestroy;
	}
	else if(root == "indexdesc"){
		return indexDescDestroy;
	}
	else if(root == "time_t"){
		return time_tDestroy;
	}
	else if(root == "date"){
		return dateDestroy;
	}
	else{
		string msg = "Don't know how to destroy type: " + root;
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

void dateDestroy(Type* adt){
	delete (EncodedDTF*) adt;
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

void interfaceDestroy(Type* adt){
	delete (Interface*) adt;
}

void indexDescDestroy(Type* adt){
	delete (IndexDesc*) adt;
}

void intToDouble(const Type* arg, Type*& result, size_t&){
	assert(result);
	*((double*) result) = int(arg);
}

void intToInt(const Type* arg, Type*& result, size_t&){
	result = (Type*) arg;
}

void doubleToDouble(const Type* arg, Type*& result, size_t&){
	assert(result);
	*((double*) result) = *((double*)(arg));
}

void stringLToString(const Type* arg, Type*& result, size_t& sz){
	assert(result);
	strncpy((char*) result, (char*) arg, sz);
}

void stringToStringL(const Type* arg, Type*& result, size_t& sz){
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
	else if(from.substr(0, 6) == "string" && to == STRING_TP){
		return stringLToString;
	}
	else if(from == STRING_TP && to.substr(0, 6) == "string"){
		return stringToStringL;
	}
	if(from == INT_TP && to == INT_TP){
		return intToInt;
	}
	else{
		string msg = string("Cannot promote ") + from + " to " + to;
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

void intervalCopy(const Type* arg, Type*& result, size_t& sz){
	*((EncodedIDT*) result) = *((EncodedIDT*) arg);
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
	else if(adt == INTERVAL_TP){
		return intervalCopy;
	}
	else if(adt == "time_t"){
		return time_tCopy;
	}
	else if(adt.substr(0, 6) == "string"){
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
		if(result ? true : false){
			lowTup[i] = tup[i];
		}
		greaterPtrs[i](tup[i], highTup[i], result);
		if(result ? true : false){
			highTup[i] = tup[i];
		}
	}
}

bool sameType(TypeID t1, TypeID t2){
	if(t1 == t2){
		return true;
	}
	else if(t1.substr(0, 6) == "string" && 
		t1.substr(0, 6) == "string"){

		return true;
	}
	else{
		return false;
	}
};

int domain(TypeID adt){	// throws exception
	if(adt.substr(0, 6) == "string" && !(adt == STRING_TP)){
		return 0;
	}
	else if(adt == STRING_TP){
		return 1;
	}
	else if(adt == "date"){
		return 2;
	}
	else if(adt == "interval"){
		return 3;
	}
	else if(adt == "bool"){
		return 4;
	}
	else if(adt == "int"){
		return 5;
	}
	else if(adt == "float"){
		return 6;
	}
	else if(adt == "double"){
		return 7;
	}
	else{
		string msg = string("Type \"") + adt + 
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

Type* duplicateObject(TypeID type, Type* obj){
	if(type == "int" || type == "bool"){
		return obj;
	}
	else if(type == "string"){
		return strdup((char*) obj);
	}
	else if(type == "double"){
		double val = *((double*) obj);
		return new double(val);
	}
	else if(type.substr(0, 6) == "string"){
		return strdup((char*) obj);
	}
	else if(type == "date"){
		EncodedDTF val = *((EncodedDTF*) obj);
		return new EncodedDTF(val);
	}
	else if(type == "time_t"){
		time_t val = *((time_t*) obj);
		return new time_t(val);
	}
	else if(type == "interface"){
		return ((Interface*) obj)->duplicate();
	}
	else if(type == "schema"){
		ISchema* val = ((ISchema*) obj);
		return new ISchema(*val);
	}
	else if(type == "indexdesc"){
		IndexDesc val = *((IndexDesc*) obj);
		return new IndexDesc(val);
	}
	else{
		cout << "Don't know how to duplicate type " << type << endl; 
		assert(0);
		return NULL;
	}
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
	else if(type.substr(0, 6) == "string"){
		size = packSize(type);
		return new char[size];
	}
	else if(type == "date"){
		return (char*) new EncodedDTF();
	}
	else if(type == "interval") {
		return (char*) new EncodedIDT() ;
	}
	else if(type == "time_t"){
		return (char*) new time_t;
	}
	else if(type == "interface"){
		size = INITIAL_INTERFACE_SIZE;
		char* space = new char[size];
		return (char*) new (space) StandardInterface;
	}
	else if(type == "schema"){
		return (char*) new ISchema();
	}
	else if(type == "indexdesc"){
		return (char*) new IndexDesc();
	}
	else{
		cout << "Don't know how to allocate space for " << type << endl; 
		assert(0);
		return NULL;
	}
}

istream& operator>>(istream& in, ISchema& s){
	
	in >> s.numFlds;
	if(!in){
		return in;
	}
	delete [] s.typeIDs;
	delete [] s.attributeNames;
	s.typeIDs = new TypeID[s.numFlds];
	s.attributeNames = new string[s.numFlds];
	string typeName;
	for(int i = 0; i < s.numFlds; i++){
		in >> typeName;
		// s.typeIDs[i] = TypeWrapper::create(typeName);
		s.typeIDs[i] = typeName;
		in >> s.attributeNames[i];
	}
	return in;
}

ISchema::ISchema(const string& str) :
	typeIDs(NULL), attributeNames(NULL) {
//	istrstream in(strdup(str.c_str()));
	istringstream in(str.c_str());	// trying to free the string??
	in >> *this;
}

ostream& operator<<(ostream& out, const ISchema& s){
	out << s.numFlds;
	for(int i = 0; i < s.numFlds; i++){
		if(s.typeIDs){
			out << " " << s.typeIDs[i];
		}
		else{
			out << " " << "unknown";
		}
		out << " " << s.attributeNames[i];
	}
	return out;
}

void dateConstructor
	(const Array<const Type*>& inp, Type*& res, size_t& = dummySz)
{
	EncodedDTF encDTF;
	const int year = IInt::getInt(inp[0]);
	const int month = IInt::getInt(inp[1]);
	const int day = IInt::getInt(inp[2]);
	encDTF.setDate(year, month, day);
	*((EncodedDTF*)res) = encDTF;
}

ConstructorPtr getConstructorPtr(
     const string& name, const TypeID* inpTypes, int numFlds, TypeID& retType)
{
	if(name == "date"){
		if(numFlds == 3 && inpTypes[0] == INT_TP && inpTypes[1] ==INT_TP &&
			inpTypes[2] == INT_TP){
			retType = DATE_TP;
			return dateConstructor;
		}
	}
	string msg = "Constructor " + name + "(";
	for(int i = 0; i < numFlds; i++){
		msg += inpTypes[i];
		if(i < numFlds - 1){
			msg += ", ";
		}
	}
	msg += ") not defined";
	THROW(new Exception(msg), 0);
}
