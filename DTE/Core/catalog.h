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
  Revision 1.6  1996/12/15 06:41:07  donjerko
  Added support for RTree indexes

  Revision 1.5  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#ifndef CATALOG_H
#define CATALOG_H

#include <assert.h>
#include <limits.h>
#include "queue.h"
#include "exception.h"
#include "site.h"
#ifdef NO_RTREE
     #include "RTreeRead.dummy"
#else
     #include "RTreeRead.h"
#endif

const int INFINITY = INT_MAX;

class Catalog {
private:
	String fileName;
public:
	Catalog(String fileName) : fileName(fileName) {
	}
	Site* find(TableName* path);     // Throws Exception
	void write(String fileNm){
		ofstream out(fileNm);
		assert(!"not implemented");
	}
	Site* toSite(String directEntry){	// throws exception
		strstream entryStream;
		entryStream << directEntry << " ;";
		CatEntry* entry = new CatEntry(new TableName());
		TRY(entry->read(entryStream), NULL); 
		Site* retVal = entry->getSite();
		delete entry;
		return retVal;
	}
};

class Interface{
protected:
	List<RTreeIndex*> indexes;
public:
	Interface() {}
	virtual Site* getSite() = 0;
	virtual istream& read(istream& in) = 0;
	virtual void write(ostream& out){
		indexes.rewind();
		while(!indexes.atEnd()){
			out << "index ";
			indexes.get()->write(out);
			indexes.step();
			out << " ";
		}
	}
	istream& readIndex(istream& in){	// throws exception
		RTreeIndex* rTreeIndex = new RTreeIndex();
		TRY(rTreeIndex->read(in), in);
		indexes.append(rTreeIndex);
		return in;
	}
	void addIndex(RTreeIndex* index){
		indexes.append(index);
	}
};

class DeviseInterface : public Interface{
	String tableNm;
	String schemaNm;
	String dataNm;
public:
	DeviseInterface(String tableNm) : tableNm(tableNm){}
	virtual Site* getSite();
	virtual istream& read(istream& in){
		return in >> schemaNm >> dataNm;
	}
	virtual void write(ostream& out){
		out << "DeviseTable " << endl;
		out << "\t" << schemaNm << endl;
		out << "\t" << dataNm << " ";
		Interface::write(out);
	}
};

class StandardInterface : public Interface{
	String tableNm;
	String urlString;
public:
	StandardInterface(String tableNm) : tableNm(tableNm){}
	virtual Site* getSite();
	virtual istream& read(istream& in){
		return in >> urlString;
	}
	virtual void write(ostream& out){
		out << "StandardTable " << urlString << " ";
		Interface::write(out);
	}
};

class QueryInterface : public Interface{
	TableName* tableNm;
	String urlString;
	Site* site;
public:
	QueryInterface(TableName* tableNm) : tableNm(tableNm){}
	virtual Site* getSite(){
		return site;
	}
	virtual istream& read(istream& in){
		in >> urlString;
		site = new Site(urlString);
		return in;
	}
	virtual void write(ostream& out){
		out << "QueryInterface " << urlString << " ";
		Interface::write(out);
	}
};

class CGIInterface : public Interface{
	String tableNm;
	String urlString;
	int entryLen;
	CGISite::Entry* entries;
public:
	CGIInterface(String tableNm) : tableNm(tableNm), entries(NULL) {}
	virtual ~CGIInterface(){
		if(entries){
			delete [] entries;
		}
	}
	virtual Site* getSite(){	// getSite may be called only once
		assert(entries);
		Site* site = new CGISite(urlString, entries, entryLen);
		entries = NULL;
		return site;
	}
	virtual istream& read(istream& in);  // throws
	virtual void write(ostream& out){
		assert(entries);
		out << "CGIInterface " << urlString << " " << entryLen;
		for(int i = 0; i < entryLen; i++){
			out << endl;
			out << "\t";
			entries[i].write(out);
		}
		Interface::write(out);
	}
};

class CatalogInterface : public Interface {
	TableName* tableName;
	String fileName;
public:
	CatalogInterface(TableName* tableName) : tableName(tableName) {}
	virtual Site* getSite();
	virtual istream& read(istream& in){
		return in >> fileName;
	}
	virtual void write(ostream& out){
		out << "Catalog " << fileName << " ";
		Interface::write(out);
	}
};

#endif
