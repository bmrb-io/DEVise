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
  Revision 1.5  1996/12/07 15:14:27  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/05 16:06:01  wenger
  Added standard Devise file headers.

 */

#ifndef CATALOG_H
#define CATALOG_H

#include <assert.h>
#include "queue.h"
#include "exception.h"
#include "site.h"

class Catalog {
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
			out << "DeviseTable " << schemaNm << " " << dataNm << " ";
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
		String tableNm;
		String urlString;
		Site* site;
	public:
		QueryInterface(String tableNm) : tableNm(tableNm){}
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
		Site* site;
	public:
		CGIInterface(String tableNm) : tableNm(tableNm), entries(NULL) {}
		virtual ~CGIInterface(){
			delete site;
			delete [] entries;
		}
		virtual Site* getSite(){
			return site;
		}
		virtual istream& read(istream& in);  // throws
		virtual void write(ostream& out){
			out << "CGIInterface " << urlString << " " << entryLen;
			for(int i = 0; i < entryLen; i++){
				out << " ";
				entries[i].write(out);
			}
			Interface::write(out);
		}
	};

	struct Entry {
		String tableNm;
		Interface* interface;
		Entry(String tableNm) : tableNm(tableNm), interface(NULL) {};
		istream& read(istream& in); // Throws Exception
		void write(ostream& out);
		~Entry(){}
	};
private:
	List<Entry*> catList;
	bool catalogFileFound;
	String fileName;
public:
	Catalog() {
		catalogFileFound = false;
	}
	void read(String fileNm){ // Throws Exception
		ifstream in(fileNm);
		if(!in){
			fileName = fileNm;
			return;
		}
		catalogFileFound = true;
		while(true){
			String tableNm;
			in >> tableNm;
			if(in){
				Entry* entry = new Entry(tableNm);
				TRY(entry->read(in), ); 
				catList.append(entry);
			}
			else{
				break;
			}
		}
	}
	void write(String fileNm){
		ofstream out(fileNm);
		assert(out);
		catList.rewind();
		while(!catList.atEnd()){
			catList.get()->write(out);
			catList.step();
		}
	}
	Interface* find(String name){     // Throws Exception
		if(!catalogFileFound){
			String msg = "Could not find catalog file: " + fileName; 
			THROW(new Exception(msg), NULL);
		}
		catList.rewind();
		while(!catList.atEnd()){
			if(catList.get()->tableNm == name){
				return catList.get()->interface;
			}
			catList.step();
		}
		String msg = "Table " + name + " not defined\n";
		THROW(new Exception(msg), NULL);
	}
	Interface* toInterface(String directEntry){	// throws exception
		strstream entryStream;
		entryStream << directEntry << " ;";
		Entry* entry = new Entry("dummy");
		TRY(entry->read(entryStream), NULL); 
		Interface* retVal = entry->interface;
		delete entry;
		return retVal;
	}
};

#endif
