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
  Revision 1.35  1998/04/09 20:26:24  donjerko
  *** empty log message ***

  Revision 1.34  1998/03/17 17:19:09  donjerko
  Added new namespace management through relation ids.

  Revision 1.33  1998/03/12 18:23:33  donjerko
  *** empty log message ***

  Revision 1.32  1997/12/10 00:00:37  okan
  ODBC Interface Changes ...

  Revision 1.31  1997/12/04 04:05:21  donjerko
  *** empty log message ***

  Revision 1.30  1997/11/23 21:23:33  donjerko
  Added ODBC stuff.

  Revision 1.29  1997/11/13 22:19:25  okan
  Changes about compilation on NT

  Revision 1.28  1997/11/05 00:19:48  donjerko
  Separated typechecking from optimization.

  Revision 1.27  1997/10/02 02:27:33  donjerko
  Implementing moving aggregates.

  Revision 1.26  1997/09/29 02:51:58  donjerko
  Eliminated class GlobalSelect.

  Revision 1.25  1997/09/17 02:35:49  donjerko
  Fixed the broken remote DTE interface.

  Revision 1.24  1997/09/05 22:20:19  donjerko
  Made changes for port to NT.

  Revision 1.23  1997/08/25 15:28:15  donjerko
  Added minmax table

  Revision 1.22  1997/08/21 21:04:34  donjerko
  Implemented view materialization

  Revision 1.21  1997/08/14 02:08:56  donjerko
  Index catalog is now an independent file.

  Revision 1.20  1997/08/12 19:58:45  donjerko
  Moved StandardTable headers to catalog.

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

#include "types.h"
#include "myopt.h"
#include "site.h"
#include "ExecExpr.h"
#include "catalog.h" 	// for root catalog
#include "Interface.h"
#include "listop.h"
#include "RelationManager.h"

bool PrimeSelection::exclusive(Site* site){
	assert(alias);
	return site->have(*alias);
}

TypeID PrimeSelection::typeCheck(){
	string msg = "Table " + *alias + " does not have field " + *fieldNm;
	THROW(new Exception(msg), "unknown");
}

bool PrimeSelection::exclusive(string* attributeNames, int numFlds){
	string me = toStringAttOnly();
	for(int i = 0; i < numFlds; i++){
		// cout << "attr = " << attributeNames[i] << endl;
		if(me == attributeNames[i]){
			return true;
		}
	}
	// cout << "me = " << me << endl;
	return false;
}

ExecExpr* BaseSelection::createExec(Site* site1, Site* site2)
{
     List<BaseSelection*>* selList;
     int leftRight = 0;
	if(site1 && exclusive(site1)){
          selList = site1->getSelectList();
          leftRight = 0;
     }
     else if(site2 && exclusive(site2)){
          selList = site2->getSelectList();
          leftRight = 1;
     }
     else{
		return NULL;
	}
	selList->rewind();
	int i = 0;
	while(!selList->atEnd()){
		if(match(selList->get())){
			return new ExecSelect(leftRight, i);
		}
		selList->step();
		i++;
	}
	return NULL;
}

ExecExpr* Operator::createExec(Site* site1, Site* site2)
{
	ExecExpr* retVal = BaseSelection::createExec(site1, site2);
	if(retVal){
		return retVal;
	}
	ExecExpr* l;
     TRY(l = left->createExec(site1, site2), NULL);
	ExecExpr* r;
     TRY(r = right->createExec(site1, site2), NULL);
	size_t objSz = 0;
	TRY(Type* value = allocateSpace(typeID, objSz), NULL);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecOperator(l, r, opPtr, value, objSz, destroyPtr);
}

ExecExpr* EnumSelection::createExec(Site* site1, Site* site2)
{
	return new ExecSelect(0, position);
}

ExecExpr* PrimeSelection::createExec(Site* site1, Site* site2)
{
	ExecExpr* retVal = BaseSelection::createExec(site1, site2);
	if(retVal){
		return retVal;
	}
	string msg = "Table " + *alias + " does not have attribute " +
		*fieldNm;
	THROW(new Exception(msg), NULL);
}

bool Operator::isIndexable(
	string& attrName, string& opName, BaseSelection*& value){
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
//	size_t objSz;
	TRY(Type* newvalue = duplicateObject(typeID, value), NULL);
	return new ConstantSelection(typeID, newvalue);
}

ExecExpr* ConstantSelection::createExec(Site* site1, Site* site2)
{
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
	if(!(*alias == *y->alias)){
		return false;
	}
	assert(y->fieldNm);
	assert(fieldNm);
	if(!(*fieldNm == *y->fieldNm)){
		return false;
	}
	return true;
}

TypeID Operator::typeCheck(){
	TRY(TypeID root = left->getTypeID(),"");
	TRY(TypeID arg = right->getTypeID(),"");
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
		string msg = "No operator " + name + "(" + root + ", " +
			arg + ") defined";
		THROW(new Exception(msg), "Unknown");
	}
	opPtr = genPtr->opPtr;
	avgSize = genPtr->sizePtr(left->getSize(), right->getSize());
	return typeID;
}

double Operator::getSelectivity(){
	if(typeID == "bool"){
		/*
		SelectyPtr selectyPtr = genPtr->selectyPtr;
		if(!selectyPtr){
			string msg = "Undefined selectiviy for operator " + name;
			THROW(new Exception(msg), "Unknown");
		}
		else{
			selectivity = selectyPtr(left, right);
		}
		*/

		if(right->selectID() == SELECT_ID){
			
			// selectivity = 1 / cardinalityOfRightTable

			PrimeSelection* rightc = (PrimeSelection*) right;
			const TableAlias* rightTable = rightc->getTable();
			assert(rightTable);
			Cardinality c = rightTable->getCardinality();
			if(!c){
				return 1;
			}
			return 1.0 / c; 
		}
		else{
			return 0.1;	// fix later
		}
	}
	else{
		assert(0);
		return 0;
	}
}

ExecExpr* TypeCast::createExec(Site* site1, Site* site2)
{
	ExecExpr* retVal = BaseSelection::createExec(site1, site2);
	if(retVal){
		return retVal;
	}
	ExecExpr* execInp;
	TRY(execInp = input->createExec(site1, site2), NULL);
	size_t valueSize;
	Type* value = allocateSpace(typeID, valueSize);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecTypeCast(execInp, promotePtr, value, valueSize, destroyPtr);
}

ExecExpr* Member::createExec(Site* site1, Site* site2)
{
	ExecExpr* retVal = BaseSelection::createExec(site1, site2);
	if(retVal){
		return retVal;
	}
	ExecExpr* execInp;
	TRY(execInp = input->createExec(site1, site2), NULL);
	size_t valueSize;
	Type* value = allocateSpace(typeID, valueSize);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecMember(
		execInp, memberPtr, value, valueSize, destroyPtr);
}

bool Operator::match(BaseSelection* x){
	if(!(selectID() == x->selectID())){
		return false;
	}
	Operator* y = (Operator*) x;
	if(!(name == y->name)){
		return false;
	}
	if(!left->match(y->left)){
		return false;
	}
	if(!right->match(y->right)){
		return false;
	}
	return true;
}

TableName TableName::dirName(){
	List<string*>* tmp = new List<string*>;
	tableName->rewind();
	while(!tableName->atEnd()){
		string* s = tableName->get();
		tableName->step();
		if(tableName->atEnd()){
			break;
		}
		else{
			tmp->append(s);
		}
	}
	return TableName(tmp);
}

string TableName::fileName(){
	string* tmp = tableName->getTail();
	if(tmp){
		return *tmp;
	}
	else{
		return "";
	}
}

Constructor::Constructor(string* name, vector<BaseSelection*>* argVec)
	: name(name), consPtr(NULL)
{
	translate(*argVec, args);
	delete argVec;
}

TypeID Constructor::typeCheck(){
	assert(args);
	int numFlds = args->cardinality();
	TypeID* inpTypes = new TypeID[numFlds];	
	int i = 0;
	for(args->rewind(); !args->atEnd(); args->step()){
		inpTypes[i] = args->get()->getTypeID();
		i++;
	}
	TRY(consPtr = 
		getConstructorPtr(*name, inpTypes, numFlds, typeID), UNKN_TYPE);
	delete [] inpTypes;
	return typeID;
}

ExecExpr* Constructor::createExec(Site* site1, Site* site2)
{
	ExecExpr* retVal = BaseSelection::createExec(site1, site2);
	if(retVal){
		return retVal;
	}
	int numFlds = args->cardinality();
	Array<ExecExpr*>* input = new Array<ExecExpr*>(numFlds);
	args->rewind();
	for(int i = 0; i < numFlds; i++){
		TRY((*input)[i] = args->get()->createExec(site1, site2), NULL);
		args->step();
	}
	size_t objSz;
	TRY(Type* value = allocateSpace(typeID, objSz), NULL);
	TRY(DestroyPtr destroyPtr = getDestroyPtr(typeID), NULL);
	return new ExecConstructor(input, consPtr, value, objSz, destroyPtr);
}

void TypeCast::collect(Site* site, List<BaseSelection*>* to){
	if(exclusive(site)){
		to->append(this);
	}
	else{
		input->collect(site, to);
	}
}

void Member::collect(Site* site, List<BaseSelection*>* to){
	if(exclusive(site)){
		to->append(this);
	}
	else{
		input->collect(site, to);
	}
}

void Constructor::collect(Site* site, List<BaseSelection*>* to){
	if(exclusive(site)){
		to->append(this);
	}
	else{
		assert(args);
		for(args->rewind(); !args->atEnd(); args->step()){
			BaseSelection* curr = args->get();
			curr->collect(site, to);
          }
	}
}

void PrimeSelection::collect(Site* site, List<BaseSelection*>* to){
	if(exclusive(site)){
		to->append(this);
	}
}

void Operator::collect(Site* site, List<BaseSelection*>* to){
	if(exclusive(site)){
		to->append(this);
		return;
	}
	left->collect(site, to);
	right->collect(site, to);
}

TypeID Member::typeCheck(){
	TypeID parentType = input->getTypeID();
	GeneralMemberPtr* genPtr;
	TRY(genPtr = getMemberPtr(*name, parentType, typeID), "unknown");
	assert(genPtr);
	memberPtr = genPtr->memberPtr;
	assert(memberPtr);
	return typeID;
}

TableName::TableName(const TableName& arg){
	tableName = new List<string*>;
	List<string*>* other = arg.tableName;
	for(other->rewind(); !other->atEnd(); other->step()){
		tableName->append(new string(*other->get()));
	}
}

TableName& TableName::operator=(const TableName& arg){
	if(this != &arg){
		delete tableName;
		tableName = new List<string*>;
		List<string*>* other = arg.tableName;
		for(other->rewind(); !other->atEnd(); other->step()){
			tableName->append(new string(*other->get()));
		}
	}
	return *this;
}

TypeID TypeCast::typeCheck(){
	if(promotePtr){

		// already typified when consturcted 

		return typeID;
	}
	TypeID inpType = input->getTypeID();
	TRY(PromotePtr promotePtr = getPromotePtr(inpType, typeID), "");
	return typeID;
}

vector<BaseSelection*> Constructor::getChildren(){
	vector<BaseSelection*> tmp;
	for(args->rewind(); !args->atEnd(); args->step()){
		tmp.push_back(args->get());
	}
	return tmp;
}

void Constructor::setChildren(const vector<BaseSelection*>& children){
	vector<BaseSelection*>::const_iterator it;
	delete args;
	args = new List<BaseSelection*>;
	for(it = children.begin(); it != children.end(); ++it){
		args->append(*it);
	}
}

TableMap PrimeSelection::getTableMap(const vector<TableAlias*>& x) const {
	int pos = 1;
	bool found = false;
	vector<TableAlias*>::const_iterator it;
	for(it = x.begin(); it != x.end(); ++it){
		if(*alias == *((*it)->getAlias())){
			found = true;
			break;
		}
		pos <<= 1;
	}
	assert(found);
	return TableMap(pos);
}

TableMap Operator::getTableMap(const vector<TableAlias*>& x) const {
	return left->getTableMap(x) | right->getTableMap(x);
}

void TableAlias::display(ostream& out, int detail){
	assert(table);
	if (function ){
		out << *function << " (" ;
		table->display(out) ;
		cout << ") " ;
	}
	else{
		table->display(out);
	}
	if(alias){
		out << " as " << *alias;
	}
}

void NumberAlias::display(ostream& out, int detail){
	out << relId;
	if(alias){
		out << " as " << *alias;
	}
}

Site* NumberAlias::createSite()
{
	Interface* interf = 0;
	TRY(interf = RELATION_MNGR.createInterface(relId), 0);

	// because the empty table name is passed in, this will not work
	// for remote server

	TRY(Site* retVal = interf->getSite(), 0);
	delete interf;
	return retVal;
}

NumberAlias::~NumberAlias(){
}

TableName* NumberAlias::getTable(){

	// There is no table name for quoted tables, but 
	// the optimizer needs to know table name to find indexes

	return new TableName();
}

ISchema NumberAlias::getISchema() const
{
	Interface* interf;
	TRY(interf = RELATION_MNGR.createInterface(relId), ISchema());
	const ISchema* schema;

	// because the empty table name is passed in, this will not work
	// for remote server

	TRY(schema = interf->getISchema(new TableName()), ISchema());
	assert(schema);
	ISchema retVal(*schema);
	delete interf;
	return retVal;
}

Site* TableAlias::createSite(){
	TRY(return ROOT_CATALOG.find(table), NULL);
}

ISchema TableAlias::getISchema() const
{
	TableName* tableNameCopy = new TableName(*table);
	Interface* interf;
	TRY(interf = ROOT_CATALOG.createInterface(tableNameCopy), ISchema());
	const ISchema* schema;
	TRY(schema = interf->getISchema(tableNameCopy), ISchema());
	assert(schema);
	return *schema;
}

ISchema QuoteAlias::getISchema() const
{
	assert(!"not implemented");
	return ISchema();
}

Site* QuoteAlias::createSite(){
	assert(interf || !"not implemented");
	return interf->getSite();
}

QuoteAlias::~QuoteAlias(){
	delete quote;
	delete interf;
}

