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
#include "CatalogComm.h"
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "Utility.h"
#include "DevRead.h"
#include "catalog.h"

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
	WritePtr* writePtrs = newWritePtrs(typeIDs, numFlds);
	assert(writePtrs);
	engine.initialize();
	ostrstream out;
	const Tuple* tup;
	while((tup = engine.getNext())){
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

char* dteListCatalog(const char* catName){

#if defined(DEBUG)
	cout << "in dteListCatalog(" << catName << ")\n";
#endif
	string query = "select cat.name, cat.interf.type from " +
		string(catName) + " as cat";
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	return retVal;
}

char* dteShowCatalogEntry(const char* tableName){
//	cout << "in dteShowCatalogEntry(" << tableName << ")\n";
	char* entryName;
	char* catName;
	getDirAndFileNames(tableName, catName, entryName);
	char* retVal = dteShowCatalogEntry(catName, entryName);
	delete entryName;
	delete catName;
	return retVal;
}

char* dteShowCatalogEntry(const char* catName, const char* entryName){

//	cout << "in dteShowCatalogEntry(" << catName << ", " << entryName << ")\n";
	string query = "select * from " +
		string(catName) + " as cat where cat.name = " +
		addQuotes(entryName);
	char* retVal = executeQuery(query);
	CATCH(
		cout << "DTE error coused by query: \n";
		cout << "   " << query << endl;
		currExcept->display();
		currExcept = NULL;
		cout << endl;
		exit(0);
	)
	return retVal;
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
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
}

void dteDeleteCatalogEntry(const char* catName, const char* entryName){

#if defined(DEBUG)
	cout << "in dteDeleteCatalogEntry(" << catName << ", " 
		<< entryName << ")\n";
#endif
	string query = "delete " +
		string(catName) + " as cat where cat.name = " +
		addQuotes(entryName);
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
}

void dteInsertCatalogEntry(const char* catName, const char* values){

#if defined(DEBUG)
	cout << "in dteInsertCatalogEntry(" << catName << ", " 
		<< values << ")\n";
#endif
	string query = "insert into " + string(catName) + " values(" +
		addQuotes(string(values) + " ;") + ")";
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
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

char* dteListAttributes(const char* tableName){
	int numFlds;
	string query = "schema " + string(tableName);
	Engine engine(query);
	engine.optimize();
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	assert(engine.getNumFlds() == 1);
	const TypeID* types = engine.getTypeIDs();
	assert(types[0] == "schema");
	engine.initialize();
	const Tuple* tuple;
	assert((tuple = engine.getNext()));
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
          currExcept->display();
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
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
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
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	assert(engine.getNumFlds() == 1);
	const TypeID* types = engine.getTypeIDs();
	assert(types[0] == "indexdesc");
	engine.initialize();
	const Tuple* tuple;
	assert((tuple = engine.getNext()));
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

char* dteListAllIndexes(){

	string query = "select t.table, t.name from .sysind as t";
	char* retVal = executeQuery(query);
     CATCH(
          cout << "DTE error coused by query: \n";
          cout << "   " << query << endl;
          currExcept->display();
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
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
}
