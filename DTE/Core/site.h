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
  Revision 1.33  1997/11/05 00:19:49  donjerko
  Separated typechecking from optimization.

  Revision 1.32  1997/09/29 02:52:00  donjerko
  Eliminated class GlobalSelect.

  Revision 1.31  1997/09/17 02:35:51  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.30  1997/09/05 22:20:21  donjerko
  Made changes for port to NT.

  Revision 1.29  1997/08/21 21:04:36  donjerko
  Implemented view materialization

  Revision 1.28  1997/08/15 00:17:36  donjerko
  Completed the Iterator destructor code.

  Revision 1.27  1997/08/14 02:08:57  donjerko
  Index catalog is now an independent file.

  Revision 1.26  1997/08/04 14:50:48  donjerko
  Fixed the bug in insert and delete queries.

  Revision 1.25  1997/07/31 14:17:47  donjerko
  *** empty log message ***

  Revision 1.24  1997/07/30 21:39:27  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.23  1997/06/21 22:48:08  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.22  1997/06/16 16:04:54  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.19  1997/04/18 20:46:20  donjerko
  Added function pointers to marshall types.

  Revision 1.18  1997/04/08 01:47:36  donjerko
  Set up the basis for ORDER BY clause implementation.

  Revision 1.16  1997/03/23 23:45:24  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.15  1997/03/20 20:42:26  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.14  1997/02/25 22:14:55  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.13  1997/02/18 18:06:07  donjerko
  Added skeleton files for sorting.

  Revision 1.12  1997/02/03 04:11:37  donjerko
  Catalog management moved to DTE

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

#include "queue.h"
#include "types.h"
#include "myopt.h"
#include "exception.h"
#include "Iterator.h"
// #include "FunctionRead.h"	// temporarily broken
#include "url.h"
#include <string>
#include <assert.h>

// #include <set>

#ifndef __GNUG__
using namespace std;
#endif

template<class A, class B> class set;

class RTreeIndex;
class CGIEntry;

List<BaseSelection*>* createSelectList(const string& nm, PlanOp*);
// List<BaseSelection*>* createSelectList(PlanOp*);	// not used
List<BaseSelection*>* createSelectList(
	const string& table, List<string*>* attNms);

// copies over the attNms

istream* contactURL(string url,
     const string* options, const string* values, int count);
	// throws exception

class Site {
friend class LocalTable;
friend class SiteGroup;
protected:
	string name;
	PlanOp* iterat;
	int numFlds;
	set<string, ltstr>* tables;
	List<BaseSelection*>* mySelect;
	List<TableAlias*>* myFrom;
	List<BaseSelection*>* myWhere;
	friend ostream& operator<<(ostream& out, Site* site);
	Stats* stats;
	string fullNm;		// full table name used to retreive index;
	string* attributeNames;
	string* typeIDs;
public:
	Site(string nm = "");
	virtual ~Site();
	virtual void addTable(TableAlias* tabName);
	// Returns the name of the ordering attribute using the
	// read iterator.
	virtual string * getOrderingAttrib(){
		if(iterat)
			return iterat->getOrderingAttrib();
		else
			return NULL;
	}
	bool have(const string&);
	bool have(const set<string, ltstr>& arg);
	void filter(List<BaseSelection*>* select, 
		List<BaseSelection*>* where = NULL);
	void filterAll(List<BaseSelection*>* select);
	virtual void display(ostream& out, int detail = 0);
	string getName(){
		return name;
	}
	virtual List<Site*>* getList(){
		List<Site*>* tmp = new List<Site*>;
		tmp->append(this);
		return tmp;
	}
	virtual Iterator* createExec();
     virtual void typify(string option);	// Throws a exception
	virtual int getNumFlds(){
		if(mySelect){
			assert(numFlds == mySelect->cardinality());
			return numFlds;
		}
		else{
			return 0;
		}
	}
     virtual const string* getTypeIDs(){
		if(!typeIDs){
			typeIDs = getTypesFromList(mySelect);
		}
          return typeIDs;
     }
	virtual const string *getAttributeNames(){
		if(!attributeNames){
			attributeNames = getStringsFrom(mySelect);
		}
		return attributeNames;
	}
	virtual string *getAttNamesOnly(){
		return getAttStringsOnly(mySelect);
	}
	List<BaseSelection*>* getSelectList(){
		return mySelect;
	}
	List<BaseSelection*>* getWhereList(){
		return myWhere;
	}
	virtual Stats* getStats(){
		if(!stats){
			stats = new Stats(getNumFlds());
		}
		return stats;
	}
	virtual List<Site*>* generateAlternatives(){
		List<Site*>* retVal = new List<Site*>;
		return retVal;
	}
	virtual double evaluateCost(){
		return 1;
	}
	virtual void writeOpen(int mode = ios::app){
		assert(0);
	}
	virtual void writeClose(){
		assert(0);
	}
	virtual void write(string content){
		assert(0);
	}
	virtual void append(string content){
		assert(0);
	}
	virtual void write(Tuple* tuple){
		assert(0);
	}
	virtual void insert(Tuple* tuple){
	}
	virtual void addPredicate(BaseSelection* predicate){
		myWhere->append(predicate);
	}
	void setFullNm(string nm){
		fullNm = nm;
	}
	string getFullNm(){
		return fullNm;
	}
};

class DirectSite : public Site {

	// Used only for typifying LocalTable

public:
	DirectSite(string nm, PlanOp* iterat);
	virtual ~DirectSite(){
		iterat = NULL;	// Local table is the owner of this iterator
	}
	virtual void typify(string option){}
	virtual Iterator* createExec();
	virtual void addTable(TableAlias* tabName){
		assert(!"addTable is done already in constructor");
	}
};

class LocalTable : public Site {
	void setStats();
	string fileToWrite;
	ofstream* fout;
	WritePtr* writePtrs;
protected:
	Site* directSite;
public:
     LocalTable(string nm, PlanOp* marsh, string fileToWrite = "") : 
		Site(nm), directSite(NULL) {
		iterat = marsh;
		directSite = NULL;  // will be set up in typify because it
						// needs the name
		this->fileToWrite = fileToWrite;
		fout = NULL;
		writePtrs = NULL;
	}
	LocalTable(string nm, List<BaseSelection*>* select, 
		List<BaseSelection*>* where, PlanOp* iterat) : Site(nm) {

		// Used as a simple filter, not as a real site

		mySelect = new List<BaseSelection*>;
		mySelect->addList(select);
		myWhere->addList(where);
		this->iterat = iterat;
		numFlds = mySelect->cardinality();
		directSite = new DirectSite(name, iterat);
		this->iterat = NULL;
		fout = NULL;
		writePtrs = NULL;
	}
	LocalTable(string nm, Site* base) : Site(nm) {

		// This site is used as a root, on top of all other sites.
		// It does includes leftover constants, if any.

		mySelect = new List<BaseSelection*>;
		this->iterat = NULL;
		directSite = base;
		fout = NULL;
		writePtrs = NULL;
	}
	virtual ~LocalTable(){
		delete fout;
		delete [] writePtrs;
		delete directSite;
	}
	virtual void addTable(TableAlias* tabName);
	virtual Iterator* createExec();
	virtual void typify(string option);	// Throws exception
	virtual List<Site*>* generateAlternatives();
	virtual void writeOpen(int mode = ios::app);
	virtual void writeClose(){
		assert(fout);
		fout->close();
		delete fout;	
		fout = NULL;
	}
	virtual void write(string content);
	virtual void append(string content);
	virtual void write(Tuple* tuple){
		assert(fout && fout->good());
		assert(writePtrs);
		for(int i = 0; i < numFlds; i++){
			assert(writePtrs[i]);
			writePtrs[i](*fout, tuple[i]);
			*fout << " ";
		}
		*fout << endl;
	}
	virtual string * getOrderingAttrib(){
		if(directSite){
			return directSite->getOrderingAttrib();
		}
		else{
			return Site::getOrderingAttrib();
		}
	}
};

class IndexScan : public LocalTable {
	RTreeIndex* index;
	int numIndexablePreds;
public:
	IndexScan(string name, List<BaseSelection*>* select,
		List<BaseSelection*>* where, RTreeIndex* index, PlanOp* iterat) :
		LocalTable(name, select, where, iterat), index(index) {
		numIndexablePreds = 0;
	}
	virtual double evaluateCost(){
		// return 1.0 / (1 + numIndexablePreds);
		return 2;	// to be worse than index only scan (LocalTable)
	}
	virtual Iterator* createExec();
};

class CGISite : public LocalTable {
	CGIEntry* entry;
	int entryLen;
	string urlString;
public:
	CGISite(string url, CGIEntry* entry, int entryLen) : 
		LocalTable("", (PlanOp*) NULL), entry(entry), 
		entryLen(entryLen), urlString(url) {}
	virtual ~CGISite(){
		// do not delete entries, they are deleted in catalog
	}
	virtual void typify(string option);
};

class SiteGroup : public Site {
protected:
	List<Site*>* sites;
	Site* site1;
	Site* site2;
public:
	SiteGroup(Site* s1, Site* s2);
	virtual ~SiteGroup(){
//		delete sites;	// list only PROBLEm
		delete site1;
		delete site2;
	}
	virtual List<Site*>* getList(){
		List<Site *>* tmp = new List<Site *>;
		tmp->addList(site1->getList());
		tmp->addList(site2->getList());
		return tmp;
		//return sites->duplicate();
	}
	virtual Iterator* createExec();
	virtual void display(ofstream& out, int detail = 0){
		Site::display(out, detail);
		out << endl;
		out << "   children: " << site1->name << ", " << site2->name;
		out << endl;
		site1->display(out, detail);
		out << endl;
		site2->display(out, detail);
	}
	virtual int getNumFlds(){
		return mySelect->cardinality();
	}
	virtual void typify(string option);	// Throws exception
	virtual string * getOrderingAttrib(){
		return site1->getOrderingAttrib();
	}
};

class UnionSite : public Site {
	Site* iter1;
	Site* iter2;
public:
	UnionSite(Site* iter1, Site* iter2) : iter1(iter1), iter2(iter2) {}
	virtual int getNumFlds(){
		return iter1->getNumFlds();
	}
	virtual const string* getAttributeNames(){
		return iter1->getAttributeNames();
	}
	virtual const string* getTypeIDs(){
		return iter1->getTypeIDs();
	}
	virtual Iterator* createExec();
	~UnionSite(){
		delete iter1;
		delete iter2;
	}
	virtual string * getOrderingAttrib(){
		string* order1 = iter1->getOrderingAttrib();
		string* order2 = iter2->getOrderingAttrib();
		if(order1 && order2 && *order1 == *order2){
			return order1;
		}
		else{
			return NULL;
		}
	}
};

class ISchemaSite : public Site {
	ISchema* schema;	
public:
	ISchemaSite(const ISchema* schema) : Site(){
		assert(schema);
		this->schema = new ISchema(*schema);
	}
	ISchemaSite(int numFlds, TypeID* types, string* attrs){
		schema = new ISchema(types, attrs, numFlds);
	}
	virtual ~ISchemaSite(){
		// do not delete schema, ISchemaExec is the owner
	}
	virtual int getNumFlds(){
		return 1;
	}
     virtual const string* getTypeIDs(){
		return &SCHEMA_STR;
     }
     virtual const string *getAttributeNames(){
		return &SCHEMA_STR;
     }
	Iterator* createExec();
};

#endif
