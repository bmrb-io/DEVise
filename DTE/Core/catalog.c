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
  Revision 1.21  1997/07/30 21:39:23  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.20  1997/07/22 15:00:54  donjerko
  *** empty log message ***

  Revision 1.19  1997/06/30 23:05:04  donjerko
  CVS:

  Revision 1.18  1997/06/21 22:48:06  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.17  1997/06/16 16:04:47  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.16  1997/05/21 14:31:40  wenger
  More changes for Linux.

  Revision 1.15  1997/04/10 21:50:26  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.13  1997/03/23 23:45:22  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.12  1997/03/20 20:42:21  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.11  1997/03/14 18:36:11  donjerko
  Making space for the SQL UNION operator.

  Revision 1.10  1997/02/25 22:14:52  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.9  1997/02/18 18:06:03  donjerko
  Added skeleton files for sorting.

  Revision 1.8  1997/02/03 04:11:33  donjerko
  Catalog management moved to DTE

  Revision 1.7  1996/12/24 21:00:52  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.6  1996/12/21 22:21:47  donjerko
  Added hierarchical namespace.

  Revision 1.5  1996/12/15 06:41:07  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#include <string.h>
#include <fstream.h>
#include <iostream.h>
#include "StandardRead.h"
#include "catalog.h"
#include "Iterator.h"
#include "DevRead.h"
#include "url.h"
#include "site.h"
#include "Utility.h"
#include "Engine.h"
#include "Inserter.h"

void readFilter(String viewNm, String& select, 
		String*& attributeNames, int& numFlds, String& where){
	ifstream vin(viewNm);
	if(!vin){
		String msg = "Could not open view file \"" + viewNm + "\"";
		THROW(new Exception(msg), );
	}
	vin >> numFlds;
	if(!vin){
		String msg = "Number of fieds expected in file \"" + viewNm + "\"";
		THROW(new Exception(msg), );
	}
	attributeNames = new String[numFlds];
     for(int i = 0; i < numFlds; i++){
          vin >> attributeNames[i];
     }
     TRY(select = stripQuotes(vin), );
     TRY(where = stripQuotes(vin), );
}

Site* DeviseInterface::getSite(){
	char* schema = strdup(schemaNm.chars());
	char* data = strdup(dataNm.chars());
	DevRead* unmarshal = new DevRead();
	TRY(unmarshal->Open(schema, data), NULL);
	if(viewNm == ""){
		return new LocalTable("", unmarshal);	
	}

	// Need to apply view

	String select;
	String* attributeNames;
	String where;
	int numFlds;

	TRY(readFilter(viewNm, select, attributeNames, numFlds, where), NULL);

	String from = "Table " + schemaNm +  " " + dataNm + " \"\" ;";
	String query = "select " + select + " from " + addQuotes(from) + " as t";
	if(!where.empty()){
		query += " where " + where;
	}
	ViewInterface tmpView(numFlds, attributeNames, query);

	// attributeNames are deleted in ViewInterface destructor

	TRY(Site* retVal = tmpView.getSite(), NULL);
	return retVal;
}

const ISchema* DeviseInterface::getISchema(TableName* table){
	assert(table);
	assert(table->isEmpty());
	int numFlds;
	String* attributeNames;
	if(viewNm.empty()){
		char* schema = strdup(schemaNm.chars());
		char* data = strdup(dataNm.chars());
		DevRead tmp;
		TRY(tmp.Open(schema, data), NULL);
		numFlds = tmp.getNumFlds();
		attributeNames = dupStrArr(tmp.getAttributeNames(), numFlds);
	}
	else {
		String select;
		String where;
		TRY(readFilter(viewNm, select, attributeNames, numFlds, where), NULL);
	}
	return new ISchema(attributeNames, numFlds);
}

Site* StandardInterface::getSite(){ // Throws a exception

	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	PlanOp* unmarshal = new StandardRead();
	TRY(unmarshal->open(in), NULL);

	delete url;
     return new LocalTable("", unmarshal, urlString);	
}

Inserter* StandardInterface::getInserter(TableName* table){ // Throws 
	assert(table);
	assert(table->isEmpty());

	LOG(logFile << "Inserting into " << urlString << endl);

	Inserter* inserter = new Inserter();
	TRY(inserter->open(urlString), NULL);

     return inserter;
}

const ISchema* StandardInterface::getISchema(TableName* table){
	assert(table);
	assert(table->isEmpty());
	int numFlds;
	String* attributeNames;
	TRY(URL* url = new URL(urlString), NULL);
	TRY(istream* in = url->getInputStream(), NULL);
	PlanOp* iterator = new StandardRead();
	TRY(iterator->open(in), NULL);
	numFlds = iterator->getNumFlds();
	attributeNames = dupStrArr(iterator->getAttributeNames(), numFlds);
	delete iterator;
	return new ISchema(attributeNames, numFlds);
}

Site* CatalogInterface::getSite(){ // Throws a exception
	Interface* interf = new StandardInterface(fileName);
	Site* retVal = interf->getSite();
	delete interf;
	return retVal;
}

const ISchema* CatalogInterface::getISchema(TableName* table){
	assert(table);
	assert(table->isEmpty());
	String* attributeNames = new String("catentry");
	return new ISchema(attributeNames, 1);
}

istream& CGIInterface::read(istream& in){  // throws
	in >> urlString;
	in >> entryLen;
	if(!in){
		String e = "Catalog error: number of entries expected";
		THROW(new Exception(e), in);
	}

	entries = new CGISite::Entry[entryLen];
	for(int i = 0; i < entryLen; i++){
		TRY(entries[i].read(in), in);
	}
	return in;
}

istream& ViewInterface::read(istream& in){ // throws
	in >> numFlds;
	if(!in){
		String e = "Number of attributes expected";
		THROW(new Exception(e), in);
	}
	attributeNames = new String[numFlds];
	for(int i = 0; i < numFlds; i++){
		in >> attributeNames[i];
	}
	TRY(query = stripQuotes(in), in);
	return in;
}

void ViewInterface::write(ostream& out){
	out << " " << numFlds;
	for(int i = 0; i < numFlds; i++){
		out << " " << attributeNames[i];
	}
	out << " " << addQuotes(query);
	Interface::write(out);
}
 
Site* ViewInterface::getSite(){ 
	// Throws a exception
	
	// This is just a temporary hack because it does not provide
	// any optimization. View is executed unmodified.

	Engine* engine = new ViewEngine(query, 
		(const String *) attributeNames, numFlds);
	TRY(engine->optimize(), NULL);
	int numEngFlds = engine->getNumFlds();
	if(numEngFlds != numFlds + 1){
		ostrstream estr;
		estr << "Number of fields in the view (" << numFlds << ") ";
		estr << "is not equal to the number in the query ";
		estr << "(" << numEngFlds << ")" << ends;
		char* e = estr.str();
		String except(e);
		delete e;
		THROW(new Exception(except), NULL);
	}
	return new LocalTable("", engine); 
}

const ISchema* QueryInterface::getISchema(TableName* table){

	// throws exception

	int count = 2;
	String* options = new String[count];
	String* values = new String[count];
	options[0] = "query";
	options[1] = "execute";
	strstream tmp;
	tmp << "schema ";
	table->display(tmp);
	tmp << ends;
	char* tmpstr = tmp.str();
	values[0] = String(tmpstr);
	values[1] = "true";
	delete tmpstr;
	istream* in;
	TRY(in = contactURL(urlString, options, values, count), NULL);
	delete [] options;
	delete [] values;
	PlanOp* sr = new StandardRead();
	TRY(sr->open(in), NULL);
	int numFlds = sr->getNumFlds();
	assert(numFlds == 1);
	const TypeID* types = sr->getTypeIDs();
	assert(types[0] == "schema");
	TRY(Iterator* iterator = sr->createExec(), NULL);
	iterator->initialize();
	const Tuple* tuple;
	assert(tuple = iterator->getNext());
	ISchema* schema = new ISchema(*((ISchema*) tuple[0]));
	assert(!iterator->getNext());
	delete iterator;
	return schema;
}

Site* Catalog::find(TableName* path){ // Throws Exception
	TRY(Interface* interf = findInterface(path), NULL);	
	Site* retVal = interf->getSite();
	delete interf;
	return retVal;
}

Interface* Catalog::findInterface(TableName* path){ // Throws Exception
	if(path->isEmpty()){
		return new StandardInterface(fileName);
	}
	ifstream* in = new ifstream(fileName);
	StandardRead* fileRead = new StandardRead();	
	TRY(fileRead->open(in), NULL);
	TRY(Iterator* iterator = fileRead->createExec(), NULL);
	iterator->initialize();

	String firstPathNm = *path->getFirst();
	path->deleteFirst();
	const Tuple* tuple;
//	cerr << "searching for " << firstPathNm << " in " << fileName << endl;

	while((tuple = iterator->getNext())){
		CatEntry* entry = (CatEntry*) tuple[0];
		assert(entry);
		String tableNm = entry->getName();
		if(tableNm == firstPathNm){
			delete in;
			TRY(Interface* interf = entry->getInterface(), NULL);
			if(interf->getType() == Interface::CATALOG){
				CatalogInterface* tmp = (CatalogInterface*) interf;
				TRY(String newFileNm = tmp->getCatalogName(), NULL);
				Catalog tmpCat(newFileNm);
				return tmpCat.findInterface(path);
			}
			else if(interf->getType() != Interface::QUERY &&
					path->cardinality() > 0){
				String msg = "Table " + firstPathNm + " is not a catalog";
				THROW(new Exception(msg), NULL);
			}
			else{
				return interf;	// should be return interf->duplicate()
			}
		}
	}
	delete in;
	String msg = "Table " + firstPathNm + " not defined in file: " +
		fileName;
		cout << msg;
	THROW(new Exception(msg), NULL);
}
