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
  Revision 1.10  1997/02/18 18:06:05  donjerko
  Added skeleton files for sorting.

  Revision 1.9  1997/02/03 04:11:36  donjerko
  Catalog management moved to DTE

  Revision 1.8  1996/12/21 22:21:50  donjerko
  Added hierarchical namespace.

  Revision 1.7  1996/12/18 00:55:59  donjerko
  Introduced the Operator* parent into the ExecOperator so that
  getTypeID work.

  Revision 1.6  1996/12/15 06:41:09  donjerko
  Added support for RTree indexes

  Revision 1.5  1996/12/07 15:14:29  donjerko
  Introduced new files to support indexes.

  Revision 1.4  1996/12/04 22:09:54  donjerko
  Removed the IntegerConstant and StringConstant and replaced them with
  ConstantSelection

  Revision 1.3  1996/12/03 23:51:02  donjerko
  The begining of the support for indexes, also a minor bug fix for select *.

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

bool PrimeSelection::exclusive(String* attributeNames, int numFlds){
	String me = toStringAttOnly();
	for(int i = 0; i < numFlds; i++){
		// cout << "attr = " << attributeNames[i] << endl;
		if(me == attributeNames[i]){
			return true;
		}
	}
	// cout << "me = " << me << endl;
	return false;
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

	TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
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
	BaseSelection* retVal;
	while(!selList->atEnd()){
		Path* upTo = NULL;
		if(selection->match(selList->get(), upTo)){
			retVal = new ExecSelect(this, leftRight, i, upTo);
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
			typeID = selList->get()->getTypeID();
			avgSize = selList->get()->getSize();
			if(upTo){
				TRY(typeID = upTo->typify(typeID, sites), "unknown");
				avgSize = upTo->getSize();
			}
			// TRY(BaseSelection::typify(sites), "Unknown");
			return typeID;
		}
		selList->step();
	}
	cout << "Could not find: ";
	selection->display(cout);
	cout << endl;
	cout << "Options were:\n";
	selList->rewind();
	while(!selList->atEnd()){
		selList->get()->display(cout);
		cout << endl;
		selList->step();
	}
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
	return new ExecOperator(name, l, r, opPtr, this);
}

TypeID PrimeSelection::typify(List<Site*>* sites){
	assert(alias);
	if(*alias == ""){
		TRY(TypeID retVal = nextPath->typify("global", sites), "unknown");
		return retVal;
	}
     assert(sites->cardinality() == 1);
     sites->rewind();
     Site* current = sites->get();
	List<BaseSelection*>* selList = current->getSelectList();
	String siteNm = current->getName();
	selList->rewind();
	int i = 0;
	while(!selList->atEnd()){
		Path* upTo = NULL;
		if(match(selList->get(), upTo)){
			typeID = selList->get()->getTypeID();
			avgSize = selList->get()->getSize();
			position = i;
			if(upTo){
				TRY(typeID = upTo->typify(typeID, sites), "unknown");
				avgSize = upTo->getSize();
			}
			return typeID;
		}
		selList->step();
		i++;
	}
	ostrstream tmp;
	displayList(tmp, selList);
	tmp << ends;
	char* tmpc = tmp.str();
	String msg = "Table " + siteNm + "(" + String(tmpc) + ")" +
		" does not have attribute \"" +
		*nextPath->path + "\"";
	delete tmpc;
	THROW(new Exception(msg), (char *) NULL);
}

BaseSelection* PrimeSelection::enumerate(
	String site1, List<BaseSelection*>* list1,
	String site2, List<BaseSelection*>* list2){

	TRY(BaseSelection::enumerate(site1, list1, site2, list2), NULL);
	assert(site2 == "" && list2 == NULL);
	int leftRight = 0;
	List<BaseSelection*>* selList = list1;
	assert(selList);
	selList->rewind();
	int i = 0;
	while(!selList->atEnd()){
		Path* upTo = NULL;
		if(match(selList->get(), upTo)){
			BaseSelection* retVal;
			// assert(i == position); failed for RTreeIndex
			retVal = new ExecSelect(this, leftRight, i, upTo);
			return retVal;
		}
		selList->step();
		i++;
	}
	String msg = "Table " + *alias + " does not have attribute " +
		*nextPath->path;
	THROW(new Exception(msg), NULL);
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
	if(ls == SELECT_ID && rs == CONST_ID){
		left->getPath()->display(os);
		value = right;
	}
	else if(rs == SELECT_ID && ls == CONST_ID){
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

TypeID Path::typify(TypeID parentType, List<Site*>* sites){

	//cout << "Typifying function: " << *path << " on " << parentType << endl;

	TypeID retType;
	GeneralMemberPtr* genPtr;
	TRY(genPtr = getMemberPtr(*path, parentType, retType), "unknown");
	assert(genPtr);
	memberPtr = genPtr->memberPtr;
	assert(memberPtr);
	if(nextPath){
		TRY(retType = nextPath->typify(retType, sites), "unknown");
	}
	return retType;
}

Type* Path::evaluate(Type* base){
	assert(memberPtr);
	Type* member = memberPtr(base);
	if(nextPath){
		return nextPath->evaluate(member);
	}
	else{
		return member;
	}
}

ConstantSelection* ConstantSelection::promote(TypeID typeToPromote) const { 
	
	// throws

	if(typeToPromote != typeID){
		TRY(PromotePtr ptr = getPromotePtr(typeID, typeToPromote), NULL);
		Type* newvalue = ptr(value);
		return new ConstantSelection(typeToPromote, newvalue);
	}
	else {
		return new ConstantSelection(typeID, value);
	}
}

BaseSelection* ConstantSelection::duplicate() {
	return new ConstantSelection(typeID, value);
}
