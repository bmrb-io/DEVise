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
  Revision 1.2  1996/11/26 02:01:48  donjerko
  Added the files that will perform aggregate functions

  Revision 1.1  1996/11/23 02:00:44  donjerko
  Restructered DTE directory

  Revision 1.3  1996/11/21 16:21:26  wenger
  Updated dependencies; fixed some compile errors and warnings.

  Revision 1.2  1996/11/19 15:31:44  wenger
  Various fixes to try to get DTE to compile on HP, Sun, etc.

 */

#include "myopt.h"
#include "site.h"
#include "machdep.h"

BaseSelection* PrimeSelection::filter(Site* site){
	Path* endPath = NULL;
	Path* nextGlobal = NULL;
	if(*alias != "" && !site->have(alias)){
		if(nextPath){
			nextPath->propagate(site);
		}
		return NULL;
	}
	if(nextPath){
		endPath = nextPath->filter(site);
	}
	if(endPath){
		nextGlobal = endPath->nextPath;
		endPath->nextPath = NULL;
		return new GlobalSelect(site, this, nextGlobal);
	}
	else if(*alias == ""){
		return NULL;	// This global function cannot be done on this site
	}
	else{
		return new GlobalSelect(site, this, nextGlobal);
	}
}

bool PrimeSelection::exclusive(Site* site){
	if(site->have(alias)){
		if(nextPath){
			return nextPath->exclusive(site);
		}
		else{
			return true;
		}
	}
	else{
		return false;
	}
}

void GlobalSelect::display(ostream& out, int detail = 0){
	out << "{" << site->getName() << ": ";
	selection->display(out, detail);
	out << "}";
	BaseSelection::display(out, detail);
}

BaseSelection* GlobalSelect::filter(Site* siteGroup){
	if(siteGroup->have(site)){
		Path* endPath = NULL;
		Path* nextGlobal = NULL;
		if(nextPath){
			endPath = nextPath->filter(siteGroup);
		}
		if(endPath){
			nextGlobal = endPath->nextPath;
			endPath->nextPath = NULL;
		}
		return new GlobalSelect(siteGroup, this, nextGlobal);
	}
	else{
		if(nextPath){
			nextPath->propagate(siteGroup);
		}
		return NULL;
	}
}

bool GlobalSelect::exclusive(Site* s){
	if(s->have(site)){
		if(nextPath){
			return nextPath->exclusive(s);
		}
		else{
			return true;
		}
	}
	else{
		return false;
	}
}

BaseSelection* GlobalSelect::enumerate(
     String site1, List<BaseSelection*>* list1,
     String site2, List<BaseSelection*>* list2){

     List<BaseSelection*>* selList;
     int leftRight = 0;
	if(site->getName() == site1){
          selList = list1;
          leftRight = 0;
     }
     else if(site->getName() == site2){
          selList = list2;
          leftRight = 1;
     }
     else{
          cerr << "Could not find site: " << site->getName() << endl;
          cerr << "Options were: " << site1 << " and " << site2 << endl;
          assert(0);
	}
	selList->rewind();
	int i = 0;
	while(!selList->atEnd()){
		Path* upTo = NULL;
		if(selection->match(selList->get(), upTo)){
			Path* newNext = NULL;
			if(upTo){
				assert(!"not implemented");
			}
			BaseSelection* retVal = new ExecSelect(leftRight, i, newNext);
			TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
			return retVal;
		}
		selList->step();
		i++;
	}
	cout << "Could not find: ";
	selection->display(cout);
	cout << endl;
	assert(0);
}

TypeID GlobalSelect::typify(List<Site*>* sites){
	String myName = site->getName();
	sites->rewind();
	Site* currSite = NULL;
	while(true){
		currSite = sites->get();
		if(myName == currSite->getName()){
			break;
		}
		sites->step();
		if(sites->atEnd()){
			cerr << "Could not find site: " << myName << endl;
			assert(0);
		}
	}
	List<BaseSelection*>* selList = currSite->getSelectList();
	selList->rewind();
	while(!selList->atEnd()){
		Path* upTo = NULL;
		if(selection->match(selList->get(), upTo)){
			Path* newNext = NULL;
			if(upTo){
				assert(!"not implemented");
			}
			TRY(BaseSelection::typify(sites), "Unknown");
			typeID = selList->get()->getTypeID();
			avgSize = selList->get()->getSize();
			return typeID;
		}
		selList->step();
	}
	cout << "Could not find: ";
	selection->display(cout);
	cout << endl;
	assert(0);
}

bool GlobalSelect::match(BaseSelection* x, Path*& upTo){
	if(!(selectID() == x->selectID())){
		return false;
	}
	GlobalSelect* y = (GlobalSelect*) x;
	if(site->getName() != y->site->getName()){
		return false;
	}
	if(!selection->match(y->selection, upTo)){
		return false;
	}
	return BaseSelection::match(y, upTo);
}

BaseSelection* Operator::enumerate(
	String site1, List<BaseSelection*>* list1,
     String site2, List<BaseSelection*>* list2){
	BaseSelection* l;
     TRY(l = left->enumerate(site1, list1, site2, list2), NULL);
	BaseSelection* r;
     TRY(r = right->enumerate(site1, list1, site2, list2), NULL);
	return new ExecOperator(name, l, r, opPtr);
}

TypeID PrimeSelection::typify(List<Site*>* sites){
     assert(sites->cardinality() == 1);
     sites->rewind();
     Site* current = sites->get();
	List<BaseSelection*>* selList = current->getSelectList();
	selList->rewind();
	int i = 0;
	assert(alias);
	if(*alias == ""){
		assert(!"Typifying functions not implemented");
	}
	while(!selList->atEnd()){
		Path* upTo = NULL;
		if(match(selList->get(), upTo)){
			if(upTo){
				assert(!"Typifying functions not implemented");
			}
			typeID = selList->get()->getTypeID();
			avgSize = selList->get()->getSize();
			position = i;
			return typeID;
		}
		selList->step();
		i++;
	}
	ostrstream tmp;
	displayList(tmp, selList);
	tmp << ends;
	char* tmpc = tmp.str();
	String msg = "Table " + *alias + "(" + String(tmpc) + ")" +
		" does not have attribute " +
		*nextPath->path;
	delete tmpc;
	THROW(new Exception(msg), (char *) NULL);
}

BaseSelection* Operator::distributeWrapper(Site* site){
	BaseSelection* leftG = left->distributeWrapper(site);
	BaseSelection* rightG = right->distributeWrapper(site);
	if(leftG){
		left = leftG;	// nothing is lost
	}
	if(rightG){
		right = rightG;	// nothing is lost
	}
	BaseSelection::distributeWrapper(site);
	return this;
}


bool Operator::isIndexable(
	String& attrName, String& opName, BaseSelection*& value){
	SelectID ls = left->selectID();
	SelectID rs = right->selectID();
	ostrstream os;
	if(ls == SELECT_ID && (rs == STRING_ID || rs == INT_ID)){
		left->getPath()->display(os);
		value = right;
	}
	else if(rs == SELECT_ID && (ls == STRING_ID || ls == INT_ID)){
		right->getPath()->display(os);
		value = left;
	}
	else{
		return false;
	}
	os << ends;
	attrName = String(os.str());
	opName = name;
	return true;
}
