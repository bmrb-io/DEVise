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
  Revision 1.4  1996/12/05 16:06:05  wenger
  Added standard Devise file headers.

 */

#ifndef SITE_H
#define SITE_H

#include <assert.h>
#include "myopt.h"
#include "queue.h"
#include "types.h"
#include "exception.h"
#include "GeneralRead.h"
#include "StandardRead.h"
#include "url.h"

class RTreeIndex {
public:
	int numAttrs;
	String* attrNames;
public:
	RTreeIndex(){}
	istream& read(istream& in){	// throws exception
		assert(in);
		in >> numAttrs;
		if(!in){
			String msg = "Number of index attributes expected";
			THROW(new Exception(msg), in);
		}
		attrNames = new String[numAttrs];
		for(int i = 0; i < numAttrs; i++){
			in >> attrNames[i];
			if(!in){
				String msg = "Name of the index attribute expected";
				THROW(new Exception(msg), in);
			}
		}
		return in;
	}
};

List<BaseSelection*>* createSelectList(String nm, GeneralRead* iterator);

class Site {
friend class LocalTable;
friend class SiteGroup;
protected:
	String name;
	GeneralRead* iterator;
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
		mySelect = new List<BaseSelection*>;
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
		String* alias = tabName->alias;
		if(alias){
			tables->append(alias);
		}
		else{
			tables->append(tabName->table);
		}
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
	virtual void display(ostream& out, int detail = 0){
		if(detail > 0){
			 out << "Site " << name << ":\n"; 
			 if(stats){
				 out	<< " stats: ";
				 stats->display(out);
			 }
			 out << "\n query:";
		}
		out << "   select ";
		displayList(out, mySelect, ", ", detail);
		out << "\n   from ";
		displayList(out, myFrom, ", ");
		if(!myWhere->isEmpty()){
			out << "\n   where ";
			displayList(out, myWhere, " and ", detail);
		}
		out << ';';
	}
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
		return numFlds;
	}
     virtual String* getTypeIDs(){
          return getTypesFromList(mySelect);
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
};

struct RTreePred {
	bool bounded[2];
	bool closed[2];
	ConstantSelection* values[2];
	RTreePred(){
		bounded[0] = bounded[1] = false;
		closed[0] = closed[1] = true;
		values[0] = values[1] = NULL;
	}
	RTreePred(String opName, ConstantSelection* constant){
		bounded[0] = bounded[1] = false;
		closed[0] = closed[1] = true;
		values[0] = values[1] = NULL;
		if(opName == "="){
			bounded[0] = bounded[1] = true;
			values[0] = values[1] = constant;
		}
		else if(opName == "<="){
			bounded[1] = true;
			values[1] = constant;
		}
		else if(opName == ">="){
			bounded[0] = true;
			values[1] = constant;
		}
		else if(opName == "<"){
			bounded[1] = true;
			closed[1] = false;
			values[1] = constant;
		}
		else if(opName == ">"){
			bounded[0] = true;
			closed[0] = false;
			values[0] = constant;
		}
		else {
			cout << "Operator \"" << opName; 
			cout << "\" should not be passed to this function\n";
			assert(0);
		}
	}
	~RTreePred(){
		// do not delete values;
	}
	void update(String opName, BaseSelection* constant){
		assert(constant->selectID() == CONST_ID);
		RTreePred tmp(opName, (ConstantSelection*) constant);
		intersect(tmp);
	}
	void intersect(const RTreePred& pred){
		if(pred.bounded[0] && !bounded[0]){
			bounded[0] = true;
			closed[0] = pred.closed[0];
			values[0] = pred.values[0];
		}
		else if(pred.bounded[0] && bounded[0]){
			assert(pred.values[0]);
			assert(values[0]);
			if(*pred.values[0] > *values[0]){
				values[0] = pred.values[0];
				closed[0] = pred.closed[0];
			}
			else if(*pred.values[0] == *values[0]){
				if(!pred.closed[0]){
					closed[0] = false;
				}
			}
		}
		if(pred.bounded[1] && !bounded[1]){
			bounded[1] = true;
			closed[1] = pred.closed[1];
			values[1] = pred.values[1];
		}
		else if(pred.bounded[1] && bounded[1]){
			assert(pred.values[1]);
			assert(values[1]);
			if(*pred.values[1] < *values[1]){
				values[1] = pred.values[1];
				closed[1] = pred.closed[1];
			}
			else if(*pred.values[1] == *values[1]){
				if(!pred.closed[1]){
					closed[1] = false;
				}
			}
		}
	}
	String toString(){
		String retVal;
		if(closed[0]){
			retVal += "[";
		}
		else {
			retVal += "<";
		}
		if(bounded[0]){
			retVal += values[0]->toString();
		}
		else {
			retVal += "-Infinity";
		}
		retVal += ", ";
		if(bounded[1]){
			retVal += values[1]->toString();
		}
		else {
			retVal += "Infinity";
		}
		if(closed[1]){
			retVal += "]";
		}
		else {
			retVal += ">";
		}
		return retVal;
	}
};

class IndexScan : public Site {
	RTreePred* rTreeQuery;	// query is an array of RTree predicates
	Site* parent;
	RTreeIndex* index;
	int numAttrs;
	bool isIndexable(BaseSelection* predicate);
public:
	IndexScan(Site* parent, RTreeIndex* index) : 
		Site(), parent(parent), index(index) {
		numAttrs = index->numAttrs;
		rTreeQuery = new RTreePred[numAttrs];
	}
	virtual void display(ostream& out, int detail = 0){
		if(detail > 0){
			 out << "Site " << name << ":\n"; 
			 if(stats){
				 out	<< " stats: ";
				 stats->display(out);
			 }
			 out << "\n query:";
		}
		out << "   select ";
		displayList(out, mySelect, ", ", detail);
		out << "\n   from ";
		displayList(out, myFrom, ", ");
		if(!myWhere->isEmpty()){
			out << "\n   where ";
			displayList(out, myWhere, " and ", detail);
		}
		out << ';' << endl;
		String tmp;
		for(int i = 0; i < numAttrs; i++){
			tmp += "Attr " + index->attrNames[i] + ": ";
			tmp += rTreeQuery[i].toString() + "\n";
		}
		out << tmp;
	}
	bool isApplicable(){
		bool retVal = false;
		List<BaseSelection*>* parentWhere = parent->getWhereList();	
		parentWhere->rewind();
		while(!parentWhere->atEnd()){
			BaseSelection* currPred = parentWhere->get();
			if(isIndexable(currPred)){
				retVal = true;
			}
			else{
				myWhere->append(currPred);
			}
			parentWhere->step();
		}
		return retVal;
	}
	double evaluateCost(){
		return 0;
	}
	virtual Tuple* getNext(){
		return NULL;
	}
};

class DirectSite : public Site {
public:
	DirectSite(String nm, GeneralRead* iterator) : Site(nm) {
		
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
	Site* directSite;
	List<RTreeIndex*> indexes;
	void setStats(){
		double selectivity = listSelectivity(myWhere);
		assert(directSite);
		Stats* baseStats = directSite->getStats();
		assert(baseStats);
		int cardinality = int(selectivity * baseStats->cardinality);
		int* sizes = sizesFromList(mySelect);
		stats = new Stats(numFlds, sizes, cardinality);
	}
public:
     LocalTable(String nm, GeneralRead* marsh, List<RTreeIndex*>* indx) : 
		Site(nm), directSite(NULL) {
		if(indx){
			indexes.addList(indx);
		}
		iterator = marsh;
	}
	virtual ~LocalTable(){}
	virtual void addTable(TableAlias* tabName){
		assert(myFrom->isEmpty());
		myFrom->append(tabName);
		String* alias = tabName->alias;
		if(alias){
			tables->append(alias);
			name = *alias;
		}
		else{
			tables->append(tabName->table);
			name = *tabName->table;
		}
	}
	virtual void enumerate(){
		assert(directSite);
		List<BaseSelection*>* baseSchema = directSite->getSelectList();
		TRY(enumerateList(mySelect, name, baseSchema), );
		TRY(enumerateList(myWhere, name, baseSchema), );
	}
	virtual void typify(String option){	// Throws exception
		
		// option is ignored since the execution = profile + getNext

		LOG(logFile << "Header: ");
		LOG(iterator->display(logFile));
		directSite = new DirectSite(name, iterator);
		List<Site*>* tmpL = new List<Site*>;
		tmpL->append(directSite);
          TRY(typifyList(myWhere, tmpL), );
		TRY(boolCheckList(myWhere), );
		if(mySelect == NULL){
			mySelect = createSelectList(name, iterator);
		}
		else{
			TRY(typifyList(mySelect, tmpL), );
		}
		numFlds = mySelect->cardinality();
		setStats();
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
		if(input){
			return tupleFromList(mySelect, input);
		}
		else{
			return NULL;
		}
	}
	virtual int getNumFlds(){
		return numFlds;
	}
	virtual List<Site*>* generateAlternatives(){
		List<Site*>* retVal = new List<Site*>;
		indexes.rewind();
		while(!indexes.atEnd()){
			IndexScan* newAlt = new IndexScan(this, indexes.get());
			if(newAlt->isApplicable()){
				retVal->append(newAlt);
			}
			indexes.step();
		}
		return retVal;
	}
};

class CGISite : public LocalTable {

	struct Entry{
		String option;
		String value;
		istream& read(istream& in);	// throws
	};

	Entry* entry;
	int entryLen;
	String urlString;
public:
	CGISite(String url, Entry* entry, int entryLen) : 
		LocalTable("", NULL, NULL), entry(entry), 
		entryLen(entryLen), urlString(url) {}
	virtual ~CGISite(){
		delete [] entry;
	}
	virtual void typify(String option);
};

class SiteGroup : public Site {
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
		delete checkList;
	     return true;	
	}
	virtual List<Site*>* getList(){
		return sites->duplicate();
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
	virtual Tuple* getNext(){
		bool cond = false;
		Tuple* innerTup = NULL;
		if(firstEntry){
			outerTup = site1->getNext();
			firstEntry = false;
		}
		while(cond == false){
			if(firstPass){
				innerTup = site2->getNext();
				if(innerTup){
					innerRel.append(innerTup);
				}
				else{
					firstPass = false;
					innerRel.rewind();
					if(innerRel.atEnd()){
						return NULL;
					}
					innerTup = innerRel.get();
					innerRel.step();
					outerTup = site1->getNext();
				}
			}
			else{
				if(innerRel.atEnd()){
					innerRel.rewind();
					outerTup = site1->getNext();
				}
				innerTup = innerRel.get();
				innerRel.step();
			}
			assert(innerTup);
			if(!outerTup){
				return NULL;
			}
			cond = evaluateList(myWhere, outerTup, innerTup);
		}
		assert(outerTup);
		return tupleFromList(mySelect, outerTup, innerTup);
	}
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
