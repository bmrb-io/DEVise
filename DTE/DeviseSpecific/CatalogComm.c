/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.25  1998/03/13 04:02:24  donjerko
  *** empty log message ***

  Revision 1.24  1998/03/13 00:31:57  donjerko
  *** empty log message ***

  Revision 1.23  1998/03/12 18:23:45  donjerko
  *** empty log message ***

  Revision 1.22  1998/01/26 23:19:49  wenger
  Conditionaled out some debug output.

  Revision 1.21  1997/12/18 23:24:30  donjerko
  *** empty log message ***

  Revision 1.20  1997/12/04 04:05:30  donjerko
  *** empty log message ***

  Revision 1.19  1997/11/23 21:23:44  donjerko
  *** empty log message ***

  Revision 1.18  1997/11/12 23:17:45  donjerko
  Improved error checking.

  Revision 1.17  1997/11/05 00:20:24  donjerko
  Added some error checking calls to the DTE.

  Revision 1.16  1997/09/29 02:52:12  donjerko
  *** empty log message ***

  Revision 1.15  1997/09/23 19:58:19  wenger
  Opening and saving of sessions now working with new scheme of mapping
  automatically creating the appropriate TData.

  Revision 1.14  1997/08/21 21:04:50  donjerko
  Implemented view materialization

  Revision 1.13  1997/08/12 19:58:54  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.12  1997/08/04 14:53:39  donjerko
  *** empty log message ***


  Revision 1.9  1997/06/16 16:05:10  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.8  1997/04/22 15:25:24  wenger
  Conditionaled out lots of debug code; fixed data source visualization
  window so the window for the data again defaults to 'New' if there are
  no windows.

  Revision 1.6  1997/04/03 16:37:15  wenger
  Reduced memory and CPU usage in statistics; fixed a memory leak in the
  statistics code; switched devised back to listening on port 6100
  (changed accidentally?); turned off a bunch of debug output.

  Revision 1.5  1997/03/28 16:07:34  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <iostream.h>
#include <strstream.h>
#include <stdlib.h>
#include "CatalogComm.h"
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "Utility.h"
#include "DevRead.h"
#include "catalog.h"
#include "Interface.h"

// #define DEBUG

void getDirAndFileNames(const char* fullPath, char*& dir, char*& file){
	assert(fullPath[0] == '.');
	int len = strlen(fullPath);
	for(int i = len; i >= 0; i--){
		if(fullPath[i] == '.'){
			file = strdup(&(fullPath[i + 1]));
			if(i == 0){
				dir = strdup(".");
			}
			else{
				dir = new char[i + 1];
				memcpy(dir, fullPath, (i + 1) * sizeof(char));
				dir[i] = '\0';
			}
			return;
		}
	}
	assert(0);
}

char* executeQuery(const string& query){
	Engine engine(query);
	TRY(engine.optimize(), NULL);
	int numFlds = engine.getNumFlds();
	if(engine.getNumFlds() == 0){
		return NULL;
	}
	TRY(const TypeID* typeIDs = engine.getTypeIDs(), NULL);
	TRY(WritePtr* writePtrs = newWritePtrs(typeIDs, numFlds), NULL);
	assert(writePtrs);
	ostrstream out;
	const Tuple* tup;
	for(tup = engine.getFirst(); tup; tup = engine.getNext()){
		out << "{";
		for(int i = 0; i < numFlds; i++){
			(writePtrs[i])(out, tup[i]);
			out << ' ';
		}
		out << "} ";
	}
	out << ends;
	delete [] writePtrs;
	engine.finalize();
	char* retVal = out.str();

#ifdef DEBUG
	cout << "Returns: " << retVal << endl;
#endif

	return retVal;
}

char* dteListCatalog(const char* catName, int& errorCode){

#if defined(DEBUG)
	cout << "in dteListCatalog(" << catName << ")\n";
#endif
	errorCode = 0;
	string query = "select cat.name, cat.interf.type from " +
		string(catName) + " as cat";
	char* retVal = executeQuery(query);
     CATCH(
		string err = "DTE error coused by query: \n";
		err += query + "\n" + currExcept->toString() + "\n";
          currExcept = NULL;
		errorCode = 1;
		return strdup(err.c_str());
     )
	return retVal;
}

char* dteShowCatalogEntry(const char* tableName){
#if defined(DEBUG)
	cout << "in dteShowCatalogEntry(" << tableName << ")\n";
#endif
	char* entryName;
	char* catName;
	getDirAndFileNames(tableName, catName, entryName);
	char* retVal = dteShowCatalogEntry(catName, entryName);
	delete entryName;
	delete catName;
	return retVal;
}

char* dteShowCatalogEntry(const char* catName, const char* entryName){

	string err = "Failed to show catalog entry " + string(entryName);

#if defined(DEBUG)
	cout << "in dteShowCatalogEntry(" << catName << ", " << entryName << ")\n";
#endif
	string query = "select cat.name, cat.interf from " +
		string(catName) + " as cat where cat.name = " +
		addSQLQuotes(entryName, '\'');
	Engine engine(query);
	CHECK(engine.optimize(), err, 0);
	string retVal;
	const Tuple* tup = engine.getFirst();
	if(!tup){
//		cerr << "query = " << query << endl << " is empty " << endl;
		return strdup("");
	}
	retVal += addQuotes(string(IString::getCStr(tup[0])));
	retVal += " ";
	CHECK(retVal += InterfWrapper::getInterface(tup[1])->guiRepresentation(),
		err, 0);
#if defined(DEBUG)
  	cerr << "Returning: " << retVal << endl;
#endif
	return strdup(retVal.c_str());
}

void dteDeleteCatalogEntry(const char* tableName){
#if defined(DEBUG)
	cout << "in dteDeleteCatalogEntry(" << tableName << ")\n";
#endif
	char* entryName;
	char* catName;
	getDirAndFileNames(tableName, catName, entryName);
	dteDeleteCatalogEntry(catName, entryName);
	delete entryName;
	delete catName;
	return;
}

void dteMaterializeCatalogEntry(const char* tableName){
	string query = "materialize " + string(tableName);
	char* retVal = executeQuery(query);
}

void dteDeleteCatalogEntry(const char* catName, const char* entryName){

#if defined(DEBUG)
	cout << "in dteDeleteCatalogEntry(" << catName << ", " 
		<< entryName << ")\n";
#endif
	string query = "delete " +
		string(catName) + " as cat where cat.name = " +
		addSQLQuotes(entryName, '\'');
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
}

int dteInsertCatalogEntry(const char* catName, const char* values){

#if defined(DEBUG)
	cout << "in dteInsertCatalogEntry(" << catName << ", " 
		<< values << ")\n";
#endif
	string tmp = string(values) + " ;";
	string query = "insert into " + string(catName) + " values(" +
		addSQLQuotes(tmp.c_str(), '\'') + ")";
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	return 0;
}

string dteCheckSQLViewEntry(const char* asClause, const char* queryToCheck){

#if defined(DEBUG)
	cout << "in dteCheckSQLViewEntry(" << asClause 
		<< ", " << queryToCheck << ")\n";
#endif
		
	string retVal;
	string asString(asClause);
	int asCard = 1;
	string::iterator it;
	for(it = asString.begin(); it != asString.end(); ++it){
		if(*it == ','){
			asCard++;
			*it = ' ';
		}
	}
	asString = string(" ") + asString + " ";
	     if(asString.find(" recId ") != string::npos ||
			asString.find("\"recId\"") != string::npos){
		retVal = "\"reciId\" is a reserved attribute name and may not be listed in the AS clause";
		return retVal;
	}
	Engine engine(queryToCheck);
	const ISchema* schema = engine.typeCheck();
     CATCH(
		string err("Coused by query:\n");
		err += queryToCheck;
		currExcept->append(err);
          retVal = currExcept->toString();
          currExcept = NULL;
		return retVal;
     )
	int numFlds = schema->getNumFlds();
	if(numFlds != asCard){
		ostringstream out;
		out << "Number of fields in the SELECT clause (" << numFlds << ")";
		out << " does not match number of fields in the AS clause (";
		out << asCard << ")" << ends;
		char* tmp = out.str();
		retVal = string(tmp);
		delete [] tmp;
		return retVal;
	}
	return retVal;
}

char* dteShowAttrNames(const char* schemaFile, const char* dataFile){
     char* schema = strdup(schemaFile);
     char* data = strdup(dataFile);
	DevRead tmp;
	tmp.Open(schema, data);
	CATCH(exit(1));
	int numFlds = tmp.getNumFlds();
	const string* attrs = tmp.getAttributeNames();
	string retVal;
	for(int i = 0; i < numFlds; i++){
		retVal += attrs[i] + " ";
	}
	return strdup(retVal.c_str());
}

char* dteListQueryAttributes(const char* query){
	Engine engine(query);
	TRY(const ISchema* schema = engine.typeCheck(), NULL);
	const string* attrNames = schema->getAttributeNames();
	int numFlds = schema->getNumFlds();
	string retVal;
	for(int i =  0; i < numFlds; i++){
		retVal += attrNames[i] + " ";
	}
#if defined(DEBUG)
  	cerr << "dteListQueryAttributes returning: " << retVal.c_str() << endl;
#endif
	return strdup(retVal.c_str());
}

char* dteListAttributes(const char* tableName){
	int numFlds;
	string query = "schema " + string(tableName);
	Engine engine(query);
	engine.optimize();
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	assert(engine.getNumFlds() == 1);
	const TypeID* types = engine.getTypeIDs();
	assert(types[0] == "schema");
	const Tuple* tuple;
	assert((tuple = engine.getFirst()));
	const ISchema* schema = (const ISchema*) tuple[0];
	assert(!(tuple = engine.getNext()));
	engine.finalize();
	const string* attrNames = schema->getAttributeNames();
	numFlds = schema->getNumFlds();
	string retVal;
	for(int i =  0; i < numFlds; i++){
		retVal += attrNames[i] + " ";
	}
	return strdup(retVal.c_str());
}

// readFilter is defined in Core/catalog.c

extern void readFilter(string viewNm, string& select,
	string*& attributeNames, int& numFlds, string& where);

char* dteReadSQLFilter(const char* fileName){
#if defined(DEBUG)
	cout << "in dteReadSQLFilter(" << fileName << ")\n";
#endif

	string* attributeNames;
	string select;
	string where;
	int numFlds;

	readFilter(fileName, select, attributeNames, numFlds, where);
     CATCH(
          cout << "DTE error while reading SQLFilter: " << fileName << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	string retVal;
	retVal += addQuotes(select);
	retVal += " \"";
	for(int i = 0; i < numFlds - 1; i++){
		retVal += attributeNames[i] + ", ";
	}
	retVal += attributeNames[numFlds - 1];
	retVal += "\" ";
	retVal += addQuotes(where);
	char* retval = strdup(retVal.c_str());
#if defined(DEBUG)
	cout << "Returning: " << retval << endl;
#endif
	return retval;
}

void dteCreateIndex(const char* tableName, const char* indexName, 
     const char* keyAttrs, const char* dataAttrs, const char* isStandAlone){

	string standAlone;
	if(strcmp(isStandAlone, "Yes") == 0){
		standAlone = "standAlone ";
	}
	string query = "create " + standAlone + "index " + indexName +
		" on " + tableName + "(" + keyAttrs + ") add (" + dataAttrs + ")"; 
#if defined(DEBUG)
	cerr << "in dteCreateIndex, query =" << query << endl;
#endif
	char* retVal = executeQuery(query);
}

string join(const string* src, int n, const string& sep){
	string retVal;
	int i = 0;
	while(i < n){
		retVal += src[i++];
		if(i < n){
			retVal += sep;
		}
	}
	return retVal;
}

char* dteShowIndexDesc(const char* tableName, const char* indexName){
#if defined(DEBUG)
	cout << "in dteShowIndexDesc(" << tableName << ", " << indexName << ")\n";
#endif
	string query = 
		string("select t.descriptor from .sysind as t where t.table = \"") +
		tableName + "\" and t.name = \"" + indexName + "\"";
	Engine engine(query);
	engine.optimize();
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	assert(engine.getNumFlds() == 1);
	const TypeID* types = engine.getTypeIDs();
	assert(types[0] == "indexdesc");
	const Tuple* tuple;
	assert((tuple = engine.getFirst()));
	IndexDesc* indexDesc = (IndexDesc*) tuple[0];
	assert(!(tuple = engine.getNext()));
	engine.finalize();
	int numKeyFlds = indexDesc->getNumKeyFlds();
	int numAddFlds = indexDesc->getNumAddFlds();
	const string* keyFlds = indexDesc->getKeyFlds();
	const string* addFlds = indexDesc->getAddFlds();
	string retVal("{");
	retVal += join(keyFlds, numKeyFlds, ", ");
	retVal += "} {";
	retVal += join(addFlds, numAddFlds, ", ");
	retVal += "} {";
	if(indexDesc->isStandAlone()){
		retVal += "Yes";
	}
	else{
		retVal += "No";
	}
	retVal += "}";
#if defined(DEBUG)
	cout << "returning " << retVal << endl;
#endif
	return strdup(retVal.c_str());
}

char* dteListAllIndexes(const char* tableName){

	return "This_Is_Not_Implemented_yet";
	string query = "select t.table, t.name from .sysind as t";
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	return retVal;
}

void dteDeleteIndex(const char* tableName, const char* indexName){

     string query = string("drop index ") + tableName + " " + indexName;
     char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          cout << currExcept->toString();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
}

Boolean isDteType(const char* type){
     if (!strcmp(type, "SQLView") || !strcmp(type, "Table") ||
       !strcmp(type, "DEVise") || !strcmp(type, "MaterView") ||
       !strcmp(type, "Directory") || !strcmp(type, "StandardTable")) {
	  return true;
     } else {
	  return false;
     }
}
