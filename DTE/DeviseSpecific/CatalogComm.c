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
  Revision 1.7  1997/04/04 23:10:39  donjerko
  Changed the getNext interface:
  	from: Tuple* getNext()
  	to:   bool getNext(Tuple*)
  This will make the code more efficient in memory allocation.

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

static int numFlds;	// just for debugging

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

char* executeQuery(const char* query){
	Engine engine(query);
	TRY(engine.optimize(), NULL);
	if(engine.getNumFlds() != numFlds){
		ostrstream err;
		err << "numFlds = " << engine.getNumFlds();
		err << " (" << numFlds << " expected)\n";
		THROW(new Exception(err.str()), NULL);
	}
	if(engine.getNumFlds() == 0){
		return NULL;
	}
	TRY(WritePtr* writePtrs = engine.getWritePtrs(), NULL);
	assert(writePtrs);
	engine.initialize();
	ostrstream out;
	Tuple tup[numFlds];
	while(engine.getNext(tup)){
		out << "{";
		for(int i = 0; i < numFlds; i++){
			(writePtrs[i])(out, tup[i]);
			out << ' ';
		}
		out << "} ";
	}
	out << ends;
	engine.finalize();
	char* retVal = out.str();

#ifdef DEBUG
	cout << "Returns: " << retVal << endl;
#endif

	return retVal;
}

char* dteListCatalog(const char* catName){

	numFlds = 2;

#if defined(DEBUG)
	cout << "in dteListCatalog(" << catName << ")\n";
#endif
	String query = "select cat.entry.name, cat.entry.type from " +
		String(catName) + " as cat";
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

	numFlds = 1;

//	cout << "in dteShowCatalogEntry(" << catName << ", " << entryName << ")\n";
	String query = "select cat.entry from " +
		String(catName) + " as cat where cat.entry.name = " +
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

void dteDeleteCatalogEntry(const char* catName, const char* entryName){

	numFlds = 0;

#if defined(DEBUG)
	cout << "in dteDeleteCatalogEntry(" << catName << ", " 
		<< entryName << ")\n";
#endif
	String query = "delete " +
		String(catName) + " as cat where cat.entry.name = " +
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

	numFlds = 0;

#if defined(DEBUG)
	cout << "in dteInsertCatalogEntry(" << catName << ", " 
		<< values << ")\n";
#endif
	String query = "insert into " + String(catName) + " values(" +
		addQuotes(String(values) + " ;") + ")";
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
	DevRead tmp(schema, data);
	int numFlds = tmp.getNumFlds();
	String* attrs = tmp.getAttributeNames();
	String retVal;
	for(int i = 0; i < numFlds; i++){
		retVal += attrs[i] + " ";
	}
	delete [] attrs;
	return strdup(retVal.chars());
}

char* dteListAttributes(const char* tableName){
	String* attrNames;
	int numFlds;
	String query = "schema " + String(tableName);
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
	TypeID* types = engine.getTypeIDs();
	assert(types[0] == "schema");
	engine.initialize();
	Tuple tuple[1];
	assert(engine.getNext(tuple));
	Schema* schema = (Schema*) tuple[0];
	assert(!engine.getNext(tuple));
	engine.finalize();
	attrNames = schema->getAttributeNames();
	numFlds = schema->getNumFlds();
	delete schema;
	String retVal;
	for(int i =  0; i < numFlds; i++){
		retVal += attrNames[i] + " ";
	}
	delete [] attrNames;
	return strdup(retVal.chars());
}

// readFilter is defined in Core/catalog.c

extern void readFilter(String viewNm, String& select,
	String*& attributeNames, int& numFlds, String& where);

char* dteReadSQLFilter(const char* fileName){
#if defined(DEBUG)
	cout << "in dteReadSQLFilter(" << fileName << ")\n";
#endif

	String* attributeNames;
	String select;
	String where;
	int numFlds;

	readFilter(fileName, select, attributeNames, numFlds, where);
     CATCH(
          cout << "DTE error while reading SQLFilter: " << fileName << endl;
          currExcept->display();
          currExcept = NULL;
          cout << endl;
          exit(0);
     )
	String retVal;
	retVal += addQuotes(select);
	retVal += " \"";
	for(int i = 0; i < numFlds - 1; i++){
		retVal += attributeNames[i] + ", ";
	}
	retVal += attributeNames[numFlds - 1];
	retVal += "\" ";
	retVal += addQuotes(where);
	char* retval = strdup(retVal.chars());
#if defined(DEBUG)
	cout << "Returning: " << retval << endl;
#endif
	return retval;
}

void dteCreateIndex(const char* tableName, const char* indexName, 
     const char* keyAttrs, const char* dataAttrs, const char* isStandAlone){

	numFlds = 0;
	String standAlone;
	if(strcmp(isStandAlone, "Yes") == 0){
		standAlone = "standAlone ";
	}
	String query = "create " + standAlone + "index " + indexName +
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

String join(const String* src, int n, const String& sep){
	String retVal;
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
	numFlds = 1;
	String query = 
		String("select t.descriptor from .sysind as t where t.table = \"") +
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
	TypeID* types = engine.getTypeIDs();
	assert(types[0] == "indexdesc");
	engine.initialize();
	Tuple tuple[1];
	assert(engine.getNext(tuple));
	IndexDesc* indexDesc = (IndexDesc*) tuple[0];
	assert(!engine.getNext(tuple));
	engine.finalize();
	int numKeyFlds = indexDesc->getNumKeyFlds();
	int numAddFlds = indexDesc->getNumAddFlds();
	const String* keyFlds = indexDesc->getKeyFlds();
	const String* addFlds = indexDesc->getAddFlds();
	String retVal("{");
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
	return strdup(retVal.chars());
}

char* dteListAllIndexes(){
	numFlds = 2;

	String query = "select t.table, t.name from .sysind as t";
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
     numFlds = 0;

     String query = String("drop index ") + tableName + " " + indexName;
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
