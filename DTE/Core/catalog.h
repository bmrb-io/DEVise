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
		istream& readIndex(istream& in){	// throws exception
			RTreeIndex* rTreeIndex = new RTreeIndex();
			TRY(rTreeIndex->read(in), in);
			indexes.append(rTreeIndex);
			return in;
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
	};

	class CGIInterface : public Interface{
		String tableNm;
		String urlString;
		Site* site;
	public:
		CGIInterface(String tableNm) : tableNm(tableNm){}
		virtual ~CGIInterface(){
			delete site;
		}
		virtual Site* getSite(){
			return site;
		}
		virtual istream& read(istream& in);  // throws
	};

	struct Entry {
		String tableNm;
		Interface* interface;
		Entry(String tableNm) : tableNm(tableNm), interface(NULL) {};
		istream& read(istream& in){ // Throws Exception
			String typeNm;
			in >> typeNm;
			if(!in){
				String msg = "Interface for table " + tableNm + 
					" must be specified";
				THROW(new Exception(msg), in);
			}
			if(typeNm == "DeviseTable"){
				interface = new DeviseInterface(tableNm);
				TRY(interface->read(in), in);
			}
			else if(typeNm == "StandardTable"){
				interface = new StandardInterface(tableNm);
				TRY(interface->read(in), in);
			}
			else if(typeNm == "QueryInterface"){
				interface = new QueryInterface(tableNm);
				TRY(interface->read(in), in);
			}
			else if(typeNm == "CGIInterface"){
				interface = new CGIInterface(tableNm);
				TRY(interface->read(in), in);
			}
			else{
				String msg = "Table " + tableNm + " interface: " + 
					typeNm + ", not defined";
				THROW(new Exception(msg), in);
			}
			String indexStr;
			in >> indexStr;
			if(in && indexStr == "index"){
				TRY(interface->readIndex(in), in);
			}
			else if(in && indexStr == ";") {
			}
			else {
				String msg = 
				"Invalid catalog format: \"index\" or \";\" expected";
				THROW(new Exception(msg), in);
			}
			return in;
		}
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
