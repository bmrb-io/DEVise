
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
  Revision 1.51  1998/03/12 18:23:36  donjerko
  *** empty log message ***

  Revision 1.50  1998/02/09 21:12:25  donjerko
  Added Bin by clause and implementation.

  Revision 1.49  1998/02/04 00:43:44  okan
  *** empty log message ***

  Revision 1.48  1998/02/03 23:04:36  okan
  Made some changes for NT

  Revision 1.47  1997/12/22 17:54:11  donjerko
  Initial version of Saeed's sequence similarity search.

  Revision 1.46  1997/12/10 00:00:38  okan
  ODBC Interface Changes ...

  Revision 1.45  1997/12/04 04:05:22  donjerko
  *** empty log message ***

  Revision 1.44  1997/11/24 08:27:51  donjerko
  *** empty log message ***

  Revision 1.43  1997/11/24 06:01:28  donjerko
  Added more odbc files.

  Revision 1.42  1997/11/24 05:25:37  okan
  *** empty log message ***

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

const ISchema STRING_SCHEMA("1 string name");

const NewStat DEFAULT_STAT = NewStat();

extern const Catalog ROOT_CATALOG(DTE_ENV_VARS.rootCatalog);

extern const Directory MINMAX_DIR(DTE_ENV_VARS.minmaxCatalog);

string DteEnvVars::getDirectory(const string& envVar) const {
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

string DteEnvVars::getFile(const string& env, const string& def) const {
	char* nm = getenv(env.c_str());
	if(nm){
		return string(nm);
	}
	else{
		return def;
	}
}

DteEnvVars::DteEnvVars(){
	materViewDirN = "DEVISE_MATER_DIR";
	minmaxDirN = "DEVISE_MINMAX_DIR";
	rootCatalogN = "DEVISE_HOME_TABLE";
	indexTableN = "DEVISE_INDEX_TABLE";
	minmaxCatalogN = "DEVISE_MINMAX_TABLE";
	definitionFileN = "DEVISE_DEF_FILE";
	idFileN = "DEVISE_ID_FILE";
	convert_bulk = "DEVISE_CONVERT_BULK";

	materViewDir = getDirectory(materViewDirN);
	minmaxDir = getDirectory(minmaxDirN);
	rootCatalog = getFile(rootCatalogN, "./catalog.dte");
	indexTable = getFile(indexTableN, "./sysind.dte");
	minmaxCatalog = getFile(minmaxCatalogN, "./minmax.dte");
	definitionFile = getFile(definitionFileN);
	idFile = getFile(idFileN);
}

string DteEnvVars::valueOf(const string& envVar) const {
	if(envVar == convert_bulk){
		return getFile(envVar, "./RTree/convert_bulk");
	}
	else{
		return getFile(envVar);
	}
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

// This function is called for evaluating the operator 'similar' for similarity
// queries in the sequence database.
void seqSimilar(const Type* arg1, const Type* arg2, Type*& result, size_t&){
	SeqSimVec* val1 = ((SeqSimVec*)arg1) ;
	const SeqSimVec* val2 = ((SeqSimVec*)arg2) ;
	result = (Type*) (val1->similar(val2));
}

// This function is called while calculating the moving aggregate for the
// similarity vector in similar queries. It updates the moving aggregate to
// add a new value in the window for which the aggregate is being called
void seqAddTup(const Type* arg1, const Type* arg2, Type*& result, size_t&){
	SeqSimVec* val1 = ((SeqSimVec*) arg1);
	double * val2 = ((double*)arg2);

	//((SeqSimVec*) result) = val1->addTup(val2);
	// The above thing would not compile, lets hope that this works

	result = val1->addTup(val2);

}

// This function is called while calculating the moving aggregate for the
// similarity vector in similar queries. It updates the moving aggregate to
// remove a value from the window for which the aggregate is being called. Thus
// seqAddTup and seqSubTup combined will update the moving aggregate by 
// advancing the window by one tuple.
void seqSubTup(const Type* arg1, const Type* arg2, Type*& result, size_t&){
	SeqSimVec* val1 = ((SeqSimVec*) arg1);
	double * val2 = ((double*)arg2);
//	((SeqSimVec*) result) = val1->subTup(val2);
	// The above thing would not compile, lets hope that this works
	result = val1->subTup(val2);
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
		return;

// the following does not work over the socket
//		string msg = "Interface name must be specified";
//		THROW(new Exception(msg), NVOID );

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
	else if(typeNm == DBServerInterface::typeName){
          interf = (DBServerInterface*) new (adt) DBServerInterface();
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
			"Invalid catalog format: \";\" expected instead of \"" +
			indexStr + "\"";
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

// This function is to read the type 'seqsv'. It reads it from the file
// stream into the structure of SeqSimVec

void seqVecRead(istream& in, Type*& adt){
	SeqSimVec* ssv = (SeqSimVec*) adt;
	for (int i=0; i < 2*NFA+2;  i++){
		in >> ssv->DFT_pts[i];
	}
	assert(!"This has to be fixed. SeqSimVec constructor has to create legal objects");
	/*

	// This old code would not compile on NT

	SeqSimVec * temp;	
	int i;
	temp = new SeqSimVec;
	for (i=0; i < 2*NFA+2;  i++)
		in >> temp->DFT_pts[i];
	((SeqSimVec*) adt)->SeqSimVec();
	for (i=0; i < 2*NFA+2;  i++)
		((SeqSimVec*) adt)->DFT_pts[i] = temp->DFT_pts[i];
	delete temp;
	*/
}

void intWrite(ostream& out, const Type* adt){
	out << int(adt);
}

void stringWrite(ostream& out, const Type* adt){
	assert(adt);
	out << addQuotes((char*) adt);
	// out << addSQLQuotes((char*) adt, '\''); // should be this ??
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

// This function is to write a structure of type SeqSimVec into a table
// as a field.
void seqVecWrite(ostream& out, const Type* adt){
	assert(adt);
	out << *((ISeqSimVec*) adt);
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
	else if (type == SEQSV_TP){	
		return sizeof (SeqSimVec);
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
	else if(type == SEQSV_TP){
		return sizeof(SeqSimVec);
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
	else if(root == SEQSV_TP){	
		return ISeqSimVec::getOperatorPtr(name,root,arg,retType);
	}
	else{
		cout << "None is the type we have got \n";
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
	else if(root == SEQSV_TP){
		return seqVecRead;
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
	else if(root == SEQSV_TP){
		return seqVecWrite;
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
	else if(root == SEQSV_TP){ 	
		return seqSimVecDestroy;
	}
	else if(root == SCHEMA_TP){
		return schemaDestroy;
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
//  This doesn't work in NT, it is core dumping.. 
//	delete (EncodedDTF*) adt;
}

// This function is called to destroy a structure of type ISeqSimVec
void seqSimVecDestroy(Type* adt){
	delete (ISeqSimVec*) adt;
}

void schemaDestroy(Type* adt){
	delete (ISchema*) adt;
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

// This function is called to copy a value and make it into a SeqSimVec
void seqSimVecCopy(const Type* arg, Type*& result, size_t& sz){
// Add code to initialize the seqVector
//	((SeqSimVec*)result)->SeqSimVec();
	assert(!"This has to be fixed. SeqSimVec constructor has to create legal objects");
   ((SeqSimVec*)result)->addTup((double*)arg);

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
	else if(adt == SEQSV_TP){ 
		return seqSimVecCopy;
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

	size = 0;	// to avoid purify warning

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
	else if(type == SEQSV_TP){ 
		size = packSize(type);
		return (char*) new ISeqSimVec();
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

/* This solves the query D(Q,aS+b) <= epsilon, where a and b are also bounded,
 * This is essentially the problem where the normal forms of Q and S are close
 * to each other and the amount of scaling and shifting is also bounded. We 
 * first convert this problem into the internal representation, where bounds on
 * a and b are converted to bounds on alpha and sigma of S. */
void svShiftScaleConstructor
	 (const Array<const Type*>& inp, Type*& res, size_t& = dummySz)
{
	// inp[0] is S, inp[1] is Q, inp[2] is epsilon, inp[3] is l_a, inp[4] is u_a
	// inp[5] is l_b inp[6] is u_b

	// For queries with just scaling b=0, so l_b=0 and u_b = 0
	// For queries with just shifting a=1, so l_a=1 and u_a = 1
	SeqSimVec vec1, vec2;
	vec1 = *((SeqSimVec *) inp[0]);
	vec2 = *((SeqSimVec *) inp[1]);
	double sum = 0;
	for (int i=2; i < 2*NFA+2; i++)
		sum += pow((vec1.DFT_pts[i] - vec2.DFT_pts[i]),2);
	double epsilon = *((double *)inp[2]);
	if (epsilon > vec2.DFT_pts[1])
		epsilon = vec2.DFT_pts[1];
	double epsiloni = sqrt(2 - 2* sqrt(1- epsilon*epsilon/(vec2.DFT_pts[1]*vec2.DFT_pts[1])));
	double l_alpha, u_alpha, l_sigma, u_sigma;
	if ((*(double *) inp[3]) > (*(double *) inp[4]))
	{
		cout << "In similarity search the lower bound on 'a' (the scaling factor) should not be more then upper bound on 'a'" << endl;
		assert(0);
	}
	if ((*(double *) inp[5]) > (*(double *) inp[6]))
	{
		cout << "In similarity search the lower bound on 'b' (the shifting factor) should not be more then upper bound on 'b'" << endl;
		assert(0);
	}
	if ((*(double *) inp[4]) == 0.0 || (*(double *) inp[3]) == 0.0)
	{
		cout << "In similarity search the bounds on 'a' (the scaling factor) should be non-zero" << endl;
		assert(0);
	}
	l_sigma = vec2.DFT_pts[1]/(*(double *) inp[4]);
	u_sigma = vec2.DFT_pts[1]/(*(double *) inp[3]);
	l_alpha = (vec2.DFT_pts[0] - (*(double *) inp[6]))/(*(double *) inp[4]);
	u_alpha = (vec2.DFT_pts[0] - (*(double *) inp[5]))/(*(double *) inp[3]);
	if (sum <= epsiloni)
	{
		if ( l_sigma <= vec1.DFT_pts[1] && vec1.DFT_pts[1] <= u_sigma &&
			 l_alpha <= vec1.DFT_pts[0] && vec1.DFT_pts[0] <= u_alpha)
   			IBool::setBool(res, true);
		else
   			IBool::setBool(res, false);
	}
	else
   		IBool::setBool(res, false);
}

/* This solves the query D(Q,aS+b) <= epsilon, where a and b are unbounded,
 * This is essentially the problem where the normal forms of Q and S are close
 * to each other. No comparison is needed for alpha's and sigmas */ 
void svNormalConstructor
	 (const Array<const Type*>& inp, Type*& res, size_t& = dummySz)
{
	// inp[0] is S, inp[1] is Q, inp[2] is epsilon

	// For these queries l_b=0 and u_b = INF
	// For queries with just shifting a=1, so l_a=1 and u_a = INF
    // calculate ret val
	SeqSimVec vec1, vec2;
	vec1 = *((SeqSimVec *) inp[0]);
	vec2 = *((SeqSimVec *) inp[1]);
	double sum = 0;
	for (int i=2; i < 2*NFA+2; i++)
		sum += pow((vec1.DFT_pts[i] - vec2.DFT_pts[i]),2);
	double epsilon = *((double *)inp[2]);
	if (epsilon > vec2.DFT_pts[1])
		epsilon = vec2.DFT_pts[1];
	double epsiloni = sqrt(2 - 2* sqrt(1- epsilon*epsilon/(vec2.DFT_pts[1]*vec2.DFT_pts[1])));
	if (sum <= epsiloni)
   		IBool::setBool(res, true);
	else
   		IBool::setBool(res, false);
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
	else if (name == "similar"){
		if(numFlds == 3 && inpTypes[0] == SEQSV_TP && inpTypes[1] ==SEQSV_TP &&
			inpTypes[2] == DOUBLE_TP ){
			retType = "bool";
			return svNormalConstructor;
		}
		else 
		if (numFlds == 7 && inpTypes[0] == SEQSV_TP && inpTypes[1] ==SEQSV_TP &&
			inpTypes[2] == DOUBLE_TP && inpTypes[3] == DOUBLE_TP && 
			inpTypes[4] == DOUBLE_TP && inpTypes[5] == DOUBLE_TP &&
			inpTypes[6] == DOUBLE_TP)
		{
			retType = "bool";
			return svShiftScaleConstructor; 
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
