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
  Revision 1.11  1996/12/26 03:42:02  kmurli
  MOdified to make joinprev work right

  Revision 1.10  1996/12/25 19:54:02  kmurli
  Files to do joinprev,joinnext and function calls.

  Revision 1.9  1996/12/24 21:00:54  kmurli
  Included FunctionRead to support joinprev and joinnext

  Revision 1.8  1996/12/21 22:21:51  donjerko
  Added hierarchical namespace.

 */

#ifndef SITE_H
#define SITE_H

#include <assert.h>
#include "queue.h"
#include "types.h"
#include "myopt.h"
#include "exception.h"
#include "Iterator.h"
#include "StandardRead.h"
#include "FunctionRead.h"
#ifdef NO_RTREE
     #include "RTreeRead.dummy"
#else
     #include "RTreeRead.h"
#endif
#include "url.h"

List<BaseSelection*>* createSelectList(String nm, Iterator* iterator);
List<BaseSelection*>* createSelectList(Iterator* iterator);

istream* contactURL(String url,
     const String* options, const String* values, int count);
	// throws exception

class Site {
friend class LocalTable;
friend class SiteGroup;
protected:
	String name;
	Iterator* iterator;
	int numFlds;
	List<String*>* tables;
	List<BaseSelection*>* mySelect;
	List<TableAlias*>* myFrom;
	List<BaseSelection*>* myWhere;
	friend ostream& operator<<(ostream& out, Site* site);
	Stats* stats;
public:
	Site(String nm = "") : name(nm), iterator(NULL) {
		numFlds = 0;
		tables = new List<String*>;
		mySelect = NULL;
		myFrom = new List<TableAlias*>;
		myWhere = new List<BaseSelection*>;
		stats = NULL;
	}
	virtual ~Site(){
		delete iterator;
		delete tables;
		delete mySelect;	// delete only list
		delete myFrom;
		delete myWhere;	// delete everything
	//	delete stats;	     // fix this
	}
	virtual void addTable(TableAlias* tabName){
		myFrom->append(tabName);
		String* alias = tabName->getAlias();
		if(alias){
			tables->append(alias);
		}
		else{
			assert(0);
			// tables->append(tabName->table);
		}
	}
	// Returns the name of the ordering attribute using the
	// read iterator.
	virtual String * getOrderingAttrib(){
		if(iterator)
			return iterator->getOrderingAttrib();
		else
			return NULL;
	}
	bool have(String* tabName){
		tables->rewind();
		while(!tables->atEnd()){
			if(*tables->get() == *tabName){
				return true;
			}
			tables->step();
		}
		return false;
	}
	virtual bool have(Site* siteGroup){
		return this == siteGroup;
	}
	void filter(List<BaseSelection*>* select, List<BaseSelection*>* where);
	virtual void display(ostream& out, int detail = 0);
	String getName(){
		return name;
	}
	virtual List<Site*>* getList(){
		List<Site*>* tmp = new List<Site*>;
		tmp->append(this);
		return tmp;
	}
	virtual void enumerate(){}
     virtual void typify(String option);	// Throws a exception
	virtual Tuple* getNext(){
		assert(iterator);
		return iterator->getNext();
	}
	virtual int getNumFlds(){
		if(mySelect){
			assert(numFlds == mySelect->cardinality());
			return numFlds;
		}
		else{
			return 0;
		}
	}
     virtual String* getTypeIDs(){
          return getTypesFromList(mySelect);
     }
	virtual String *getAttributeNames(){
		return getStringsFrom(mySelect);
	}
	virtual String *getAttNamesOnly(){
		return getAttStringsOnly(mySelect);
	}
	List<BaseSelection*>* getSelectList(){
		return mySelect;
	}
	List<BaseSelection*>* getWhereList(){
		return myWhere;
	}
	virtual Stats* getStats(){
		return stats;
	}
	void reset(int lowRid, int highRid){
		TRY(iterator->reset(lowRid, highRid), );
	}
	virtual List<Site*>* generateAlternatives(){
		List<Site*>* retVal = new List<Site*>;
		return retVal;
	}
	virtual void initialize(){
		if(iterator){
			iterator->initialize();
		}
	}
	virtual void finalize(){
	}
	virtual double evaluateCost(){
		return 1;
	}
     virtual Offset getOffset(){
          assert(!"getOffset not supported for this iterator");
          return Offset();
     }
	virtual void writeOpen(int mode = ios::app){
		assert(0);
	}
	virtual void writeClose(){
		assert(0);
	}
	virtual void write(String content){
		assert(0);
	}
	virtual void write(Tuple* tuple){
		assert(0);
	}
	virtual void insert(Tuple* tuple){
	}
};

class DirectSite : public Site {
public:
	DirectSite(String nm, Iterator* iterator) : Site(nm) {
		
		// Used only for typifying LocalTable

		assert(iterator);
		Site::iterator = iterator;
		numFlds = iterator->getNumFlds();
		stats = iterator->getStats();
		assert(stats);
		mySelect = createSelectList(nm, iterator);
	}
};

class LocalTable : public Site {
	List<RTreeIndex*> indexes;
	void setStats();
	String fileToWrite;
	ofstream* fout;
	WritePtr* writePtrs;
protected:
	Site* directSite;
public:
     LocalTable(String nm, Iterator* marsh, List<RTreeIndex*>* indx, 
		String fileToWrite = "") : 
		Site(nm), directSite(NULL) {
		if(indx){
			indexes.addList(indx);
		}
		iterator = marsh;
		this->fileToWrite = fileToWrite;
		fout = NULL;
		writePtrs = NULL;
	}
	LocalTable(String nm, List<BaseSelection*>* select, 
		List<BaseSelection*>* where, Iterator* iterator) : Site(nm) {

		// Used as a simple filter, not as a real site

		mySelect = new List<BaseSelection*>;
		mySelect->addList(select);
		myWhere->addList(where);
		this->iterator = iterator;
		numFlds = mySelect->cardinality();
		directSite = new DirectSite(name, iterator);
	}
	virtual ~LocalTable(){
		delete fout;
		delete writePtrs;
	}
	virtual void finalize(){}
	virtual void addTable(TableAlias* tabName){
		assert(myFrom->isEmpty());
		myFrom->append(tabName);
		String* alias = tabName->getAlias();
		if(alias){
			tables->append(alias);
			name = *alias;
		}
		else{
			assert(0);
		}
	}
	virtual void enumerate(){
		assert(directSite);
		List<BaseSelection*>* baseSchema = directSite->getSelectList();
		TRY(enumerateList(mySelect, name, baseSchema), );
		TRY(enumerateList(myWhere, name, baseSchema), );
	}
	virtual void typify(String option);	// Throws exception
	virtual void initialize(){
		Site::initialize();
	}
	virtual Tuple* getNext(){
		bool cond = false;
		Tuple* input = NULL;
		while(!cond){
			input = iterator->getNext();
			if(!input){
				return NULL;
			}
			cond = evaluateList(myWhere, input);
		}
		assert(input);
		return tupleFromList(mySelect, input);
	}
	virtual List<Site*>* generateAlternatives();
     virtual Offset getOffset(){
          return iterator->getOffset();
     }
	virtual void writeOpen(int mode = ios::app);
	virtual void writeClose(){
		assert(fout);
		fout->close();
		delete fout;	
		fout = NULL;
	}
	virtual void write(String content){
		assert(fout);
		*fout << content << endl;
	}
	virtual void write(Tuple* tuple){
		assert(fout);
		assert(writePtrs);
		for(int i = 0; i < numFlds; i++){
			writePtrs[i](*fout, tuple[i]);
		}
		*fout << endl;
	}
};

class IndexScan : public LocalTable {
	RTreeIndex* index;
	int numIndexablePreds;
public:
	IndexScan(String name, List<BaseSelection*>* select,
		List<BaseSelection*>* where, RTreeIndex* index, Iterator* iterator) :
		LocalTable(name, select, where, iterator), index(index) {
		numIndexablePreds = 0;
	}
	virtual double evaluateCost(){
		// return 1.0 / (1 + numIndexablePreds);
		return 2;	// to be worse than index only scan (LocalTable)
	}
	virtual void initialize(){
		assert(index);
		index->initialize();
		Site::initialize();
	}
	virtual Tuple* getNext(){
		bool cond = false;
		Tuple* input = NULL;
		while(!cond){
			Offset offset = index->getNextOffset();
			if(offset.isNull()){
				return NULL;
			}
			iterator->setOffset(offset);
			input = iterator->getNext();
			assert(input);
			cond = evaluateList(myWhere, input);
		}
		assert(input);
		return tupleFromList(mySelect, input);
	}
};

class CGISite : public LocalTable {

	struct Entry{
		String option;
		String value;
		istream& read(istream& in);	// throws
		void write(ostream& out);
	};

	Entry* entry;
	int entryLen;
	String urlString;
public:
	CGISite(String url, Entry* entry, int entryLen) : 
		LocalTable("", NULL, NULL), entry(entry), 
		entryLen(entryLen), urlString(url) {}
	virtual ~CGISite(){
		// do not delete entries, they are deleted in catalog
	}
	virtual void typify(String option);
};

class SiteGroup : public Site {
protected:
	List<Site*>* sites;
	Site* site1;
	Site* site2;
	List<Tuple*> innerRel;
	bool firstEntry;
	bool firstPass;
	Tuple* outerTup;
public:
	SiteGroup(Site* s1, Site* s2) : Site(""), site1(s1), site2(s2){
		sites = new List<Site*>;

		List<Site*>* tmp1 = site1->getList();
		List<Site*>* tmp2 = site2->getList();

		sites->addList(tmp1);
		sites->addList(tmp2);
		sites->rewind();

		assert(!sites->atEnd());
		name = sites->get()->getName();
		sites->step();
		while(!sites->atEnd()){
			name += "+" + sites->get()->getName();
			sites->step();
		}
		delete tmp1;
		delete tmp2;
		firstEntry = true;
		firstPass = true;
		outerTup = NULL;
	}
	virtual void initialize(){
		if (site1)
			site1->initialize();
		if(site2)
			site2->initialize();
	}
	virtual ~SiteGroup(){
//		delete sites;	// list only PROBLEm
		delete site1;
		delete site2;
	}
	virtual bool have(Site* siteGroup){
		List<Site*>* checkList = siteGroup->getList();

		checkList->rewind();
		while(!checkList->atEnd()){
			Site* checkSite = checkList->get();
			sites->rewind();
			while(true){
				if(sites->atEnd()){
					return false;
				}
				if(sites->get() == checkSite){
					break;
				}
				sites->step();
			}
			checkList->step();
		}
		// #########/
		//delete checkList;
	     return true;	
	}
	virtual List<Site*>* getList(){
		List<Site *>* tmp = new List<Site *>;
		tmp->addList(site1->getList());
		tmp->addList(site2->getList());
		return tmp;
		//return sites->duplicate();
	}
	virtual void enumerate(){
		TRY(enumerateList(mySelect, site1->getName(), site1->mySelect, 
			site2->getName(), site2->mySelect), );
		TRY(enumerateList(myWhere, site1->getName(), site1->mySelect, 
			site2->getName(), site2->mySelect), );
		TRY(site1->enumerate(), );
		TRY(site2->enumerate(), );
	}
	virtual void display(ofstream& out, int detail = 0){
		Site::display(out, detail);
		out << endl;
		out << "   children: " << site1->name << ", " << site2->name;
		out << endl;
		site1->display(out, detail);
		out << endl;
		site2->display(out, detail);
	}
	virtual Tuple* getNext();
	virtual int getNumFlds(){
		return mySelect->cardinality();
	}
	virtual void typify(String option){	// Throws exception
		
		List<Site*>* tmpL = new List<Site*>;
		tmpL->append(site1);
		tmpL->append(site2);
		if(mySelect == NULL){
			mySelect = createSelectList(name, iterator);
		}
		else{
			TRY(typifyList(mySelect, tmpL), );
		}
		numFlds = mySelect->cardinality();
          TRY(typifyList(myWhere, tmpL), );
		double selectivity = listSelectivity(myWhere);
		int card1 = site1->getStats()->cardinality;
		int card2 = site2->getStats()->cardinality;
		int cardinality = int(selectivity * card1 * card2);
		int* sizes = sizesFromList(mySelect);
		stats = new Stats(numFlds, sizes, cardinality);
		TRY(boolCheckList(myWhere), );
     }
};

#endif
