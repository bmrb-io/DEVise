#include <iostream.h>
#include <strstream.h>
#include "GroupDir.h"
#include "Control.h"
#include "CatalogComm.h"
#include "TDataDQLInterp.h"
#include "types.h"
#include "exception.h"
#include "Engine.h"
#include "Utility.h"
#include "DevRead.h"
#include "catalog.h"

static int numFlds;	// just for debugging

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
	Tuple* tup;
	engine.initialize();
	ostrstream out;
	while((tup = engine.getNext())){
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
	cout << "Returns: " << retVal << endl;
	return retVal;
}

char* dteListCatalog(const char* catName){

	numFlds = 2;

	cout << "in dteListCatalog(" << catName << ")\n";
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
	cout << "in dteShowCatalogEntry(" << tableName << ")\n";
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

	cout << "in dteShowCatalogEntry(" << catName << ", " << entryName << ")\n";
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
	cout << "in dteDeleteCatalogEntry(" << tableName << ")\n";
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

	cout << "in dteDeleteCatalogEntry(" << catName << ", " 
		<< entryName << ")\n";
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

	cout << "in dteInsertCatalogEntry(" << catName << ", " 
		<< values << ")\n";
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

extern GroupDir* gdir;	// defined in graphics.new/ParseCat.c
extern InsertCatFile(char* tableName);	// defined in graphics.new/ParseCat.c

char* dteImportFileType(char* name){

	cout << "in dteImportFileType(" << name << ")\n";
	String query = "select * from " + String(name) + " as t";

	gdir->add_entry(name);
	TDataDQLInterpClassInfo* DQLclass;
	DQLclass = new TDataDQLInterpClassInfo(name, query.chars());
	
	ControlPanel::RegisterClass(DQLclass,true);
	InsertCatFile(strdup(name));
	
	if (Init::PrintTDataAttr()){
		cout << "Should print attributes ?????????????????????" << endl;
		//attrs->Print();
	}

	if (gdir->num_topgrp(name) == 0)
	{
	  Group *newgrp = new Group("__default", NULL, TOPGRP);
	  gdir->add_topgrp(name,newgrp);

	  AttrList attrs(*DQLclass->GetAttrList());
	  int numAttrs = attrs.NumAttrs(); 

	  for (int i=0; i < numAttrs; i++) {
	    AttrInfo *iInfo = attrs.Get(i);
            newgrp->insert_item(iInfo->name);
	  }

	}
	return name;
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
	Tuple* tuple = engine.getNext();
	assert(tuple);
	Schema* schema = (Schema*) tuple[0];
	assert(engine.getNext() == NULL);
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
	cout << "in dteReadSQLFilter(" << fileName << ")\n";

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
	cout << "Returning: " << retval << endl;
	return retval;
}

