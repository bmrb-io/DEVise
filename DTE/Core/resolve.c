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
  Revision 1.19  1997/07/30 21:39:26  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.18  1997/07/22 15:00:57  donjerko
  *** empty log message ***

  Revision 1.17  1997/06/27 23:17:22  donjerko
  Changed date structure from time_t and tm to EncodedDTF

  Revision 1.16  1997/06/16 16:04:53  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.15  1997/04/14 20:44:15  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.14  1997/04/10 21:50:29  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.13  1997/03/25 17:57:41  wenger
  Merged rel_1_3_3c through rel_1_3_4b changes into the main trunk.

  Revision 1.12  1997/03/20 20:42:24  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.11  1997/02/25 22:14:54  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.10  1997/02/18 18:06:05  donjerko
  Added skeleton files for sorting.

  Revision 1.9  1997/02/03 04:11:36  donjerko
  Catalog management moved to DTE

  Revision 1.8.4.2  1997/03/25 15:41:08  wenger
  Slight change to resolve.c to get it to compile on SPARC/SunOS.

  Revision 1.8.4.1  1997/03/18 14:41:20  wenger
  Various minor changes to get 1.3 to compile on SGI.

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
#include "ExecExpr.h"

BaseSelection* PrimeSelection::filter(Site* site){
	if(*alias != "" && !site->have(alias)){
		return NULL;
	}
	else if(*alias == ""){
		return NULL;	// This global function cannot be done on this site
	}
	else{
		return new GlobalSelect(site, this);
	}
}

bool PrimeSelection::exclusive(Site* site){
	if(site->have(alias)){
		return true;
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
}

BaseSelection* GlobalSelect::filter(Site* siteGroup){
	if(siteGroup->have(site)){
		return new GlobalSelect(siteGroup, this);
	}
	else{
		return NULL;
	}
}

bool GlobalSelect::exclusive(Site* s){
	if(s->have(site)){
		return true;
	}
	else{
		return false;
	}
}

ExecExpr* GlobalSelect::createExec(
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
		if(selection->match(selList->get())){
			return new ExecSelect(leftRight, i);
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
			cerr << "among: ";
			for(sites->rewind(); !sites->atEnd(); sites->step()){
				cerr << sites->get()->getName() << ", ";
			}
			cerr << endl;
			assert(0);
		}
	}
	List<BaseSelection*>* selList = currSite->getSelectList();
	selList->rewind();
	while(!selList->atEnd()){
		if(selection->match(selList->get())){
			typeID = selList->get()->getTypeID();
			avgSize = selList->get()->getSize();
//			assert(typeID == selection->getTypeID());
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
	return ""; //avoid compiler warning
}

bool GlobalSelect::match(BaseSelection* x){
	if(!(selectID() == x->selectID())){
		return false;
	}
	GlobalSelect* y = (GlobalSelect*) x;
	if(site->getName() != y->site->getName()){
		return false;
	}
	if(!selection->match(y->selection)){
		return false;
	}
	return true;
}

ExecExpr* Operator::createExec(
	String site1, List<BaseSelection*>* list1,
     String site2, List<BaseSelection*>* list2){
	ExecExpr* l;
     TRY(l = left->createExec(site1, list1, site2, list2), NULL);
	ExecExpr* r;
     TRY(r = right->createExec(site1, list1, site2, list2), NULL);
	size_t objSz;
	TRY(Type* value = allocateSpace(typeID, objSz), NULL);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecOperator(l, r, opPtr, value, objSz, destroyPtr);
}

TypeID PrimeSelection::typify(List<Site*>* sites){
	assert(alias);
	if(*alias == ""){
		assert(!"global functions not implemented");

		// TRY(TypeID retVal = nextPath->typify("global", sites), "unknown");
		// return retVal;
	}
     assert(sites->cardinality() == 1);
     sites->rewind();
     Site* current = sites->get();
	List<BaseSelection*>* selList = current->getSelectList();
	String siteNm = current->getName();
	selList->rewind();
	int i = 0;
	while(!selList->atEnd()){
		if(match(selList->get())){
			typeID = selList->get()->getTypeID();
			avgSize = selList->get()->getSize();
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
		" does not have attribute \"" + *fieldNm + "\"";
	delete tmpc;
	THROW(new Exception(msg), (char *) NULL);
}

ExecExpr* PrimeSelection::createExec(
	String site1, List<BaseSelection*>* list1,
	String site2, List<BaseSelection*>* list2){

	assert(site2 == "" && list2 == NULL);
	int leftRight = 0;
	List<BaseSelection*>* selList = list1;
	assert(selList);
	selList->rewind();
	int i = 0;
	while(!selList->atEnd()){
		if(match(selList->get())){
			return new ExecSelect(leftRight, i);
		}
		selList->step();
		i++;
	}
	String msg = "Table " + *alias + " does not have attribute " +
		*fieldNm;
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
	if(ls == SELECT_ID && rs == CONST_ID){
		attrName = *(((PrimeSelection*) left)->getFieldNm());
		value = right;
	}
	else if(rs == SELECT_ID && ls == CONST_ID){
		attrName = *(((PrimeSelection*) right)->getFieldNm());
		value = left;
	}
	else{
		return false;
	}
	opName = name;
	return true;
}

ConstantSelection* ConstantSelection::promote(TypeID typeToPromote) const { 
	
	// throws

	TRY(PromotePtr ptr = getPromotePtr(typeID, typeToPromote), NULL);
	size_t objSz;
	Type* newvalue = allocateSpace(typeToPromote, objSz);
	ptr(value, newvalue, objSz);
	return new ConstantSelection(typeToPromote, newvalue);
}

BaseSelection* ConstantSelection::duplicate() {
	size_t objSz;
	TRY(Type* newvalue = duplicateObject(typeID, value), NULL);
	return new ConstantSelection(typeID, newvalue);
}

ExecExpr* ConstantSelection::createExec(
		String site1, List<BaseSelection*>* list1,
		String site2, List<BaseSelection*>* list2){
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	TRY(Type* newvalue = duplicateObject(typeID, value), NULL);
	return new ExecConst(newvalue, destroyPtr);
}

bool PrimeSelection::match(BaseSelection* x){
	assert(x);
	if(!(selectID() == x->selectID())){
		return false;
	}
	PrimeSelection* y = (PrimeSelection*) x;
	assert(y->alias);
	assert(alias);
	if(*alias != *y->alias){
		return false;
	}
	assert(y->fieldNm);
	assert(fieldNm);
	if(*fieldNm != *y->fieldNm){
		return false;
	}
	return true;
}

TypeID Operator::typify(List<Site*>* sites){
	TRY(left->typify(sites), "");
	TRY(right->typify(sites), "");
	TypeID root = left->getTypeID();
	TypeID arg = right->getTypeID();
	GeneralPtr* genPtr;
	genPtr = getOperatorPtr(name, root, arg, typeID);
	CATCH(;);
	if(!genPtr){

		// need to typecast

		TRY(int typeComp = typeCompare(root, arg), "");
		if(typeComp > 0){

			// typecast arg to root

			TRY(PromotePtr cast = getPromotePtr(arg, root), "");
			right = new TypeCast(root, right, cast);
			arg = root;
		}
		else{

			// typecast root to arg

			TRY(PromotePtr cast = getPromotePtr(root, arg), "");
			left = new TypeCast(arg, left, cast);
			root = arg;
		}
		TRY(genPtr = getOperatorPtr(name, root, arg, typeID), "unknown");
	}
	if(!genPtr){
		String msg = "No operator " + name + "(" + root + ", " +
			arg + ") defined";
		THROW(new Exception(msg), "Unknown");
	}
	opPtr = genPtr->opPtr;
	avgSize = genPtr->sizePtr(left->getSize(), right->getSize());
	if(typeID == "bool"){
		SelectyPtr selectyPtr = genPtr->selectyPtr;
		if(!selectyPtr){
			String msg = "Undefined selectiviy for operator " + name;
			THROW(new Exception(msg), "Unknown");
		}
		else{
			selectivity = selectyPtr(left, right);
		}
	}
	return typeID;
}

ExecExpr* TypeCast::createExec(
	String site1, List<BaseSelection*>* list1,
	String site2, List<BaseSelection*>* list2){
	ExecExpr* execInp;
	TRY(execInp = input->createExec(site1, list1, site2, list2), NULL);
	size_t valueSize;
	Type* value = allocateSpace(typeID, valueSize);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecTypeCast(execInp, promotePtr, value, valueSize, destroyPtr);
}

ExecExpr* Member::createExec(
	String site1, List<BaseSelection*>* list1,
	String site2, List<BaseSelection*>* list2){
	ExecExpr* execInp;
	TRY(execInp = input->createExec(site1, list1, site2, list2), NULL);
	size_t valueSize;
	Type* value = allocateSpace(typeID, valueSize);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecMember(
		execInp, memberPtr, value, valueSize, destroyPtr);
}

