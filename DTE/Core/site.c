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
  Revision 1.26  1997/09/09 14:42:16  donjerko
  Bug fix

  Revision 1.25  1997/09/05 22:20:20  donjerko
  Made changes for port to NT.

  Revision 1.24  1997/08/22 23:13:06  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.23  1997/08/21 21:04:35  donjerko
  Implemented view materialization

  Revision 1.22  1997/08/14 02:08:56  donjerko
  Index catalog is now an independent file.

  Revision 1.21  1997/08/12 19:58:45  donjerko
  Moved StandardTable headers to catalog.

  Revision 1.20  1997/08/10 20:30:57  donjerko
  Fixed the NO_RTREE option.

  Revision 1.19  1997/07/30 21:39:26  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.18  1997/07/22 15:00:57  donjerko
  *** empty log message ***

  Revision 1.17  1997/06/21 22:48:07  donjerko
  Separated type-checking and execution into different classes.

  Revision 1.16  1997/06/16 16:04:54  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.15  1997/05/07 18:56:54  donjerko
  Added filterAll function.

  Revision 1.14  1997/04/14 20:44:16  donjerko
  Removed class Path and introduced new BaseSelection class Member.

  Revision 1.13  1997/04/10 21:50:30  donjerko
  Made integers inlined, added type cast operator.

  Revision 1.11  1997/03/23 23:45:24  donjerko
  Made boolean vars to be in the tuple.

  Revision 1.10  1997/03/20 20:42:25  donjerko
  Removed the List usage from Aggregates and replaced it with Plex, a
  form of dynamic array.

  Revision 1.9  1997/02/25 22:14:54  donjerko
  Enabled RTree to store data attributes in addition to key attributes.

  Revision 1.8  1997/02/18 18:06:06  donjerko
  Added skeleton files for sorting.

  Revision 1.7  1997/02/03 04:11:36  donjerko
  Catalog management moved to DTE

  Revision 1.6  1996/12/21 22:21:51  donjerko
  Added hierarchical namespace.

  Revision 1.5  1996/12/15 06:41:10  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/05 16:06:05  wenger
  Added standard Devise file headers.

 */

// #include <string.h>
// #include <ctype.h>


// #include "Iterator.h"

#include "site.h"
#include "Interface.h"
#include "ExecOp.h"
#include "RTreeRead.h"

#include <string>		// for strtok
#include <set>
#include <algo.h>	// for includes

class Iterator;

Site::Site(string nm) : name(nm), iterat(NULL) {
	numFlds = 0;
	tables = new set<string, ltstr>;
	mySelect = NULL;
	myFrom = new List<TableAlias*>;
	myWhere = new List<BaseSelection*>;
	stats = NULL;
	attributeNames = NULL;
	typeIDs = NULL;
}

Site::~Site(){
//	cerr << "deleting select where lists\n";
	delete tables;
	delete iterat;
	if(mySelect){
		for(mySelect->rewind(); !mySelect->atEnd(); mySelect->step()){
//			mySelect->get()->destroy();
			delete mySelect->get();
		}
	}
	delete mySelect;	// delete only list
	delete myFrom;
	for(myWhere->rewind(); !myWhere->atEnd(); myWhere->step()){
//		myWhere->get()->destroy();
		delete myWhere->get();
	}
	delete myWhere;	// delete everything
//	delete stats;	     // fix this
	delete [] attributeNames;
	delete [] typeIDs;
}

Iterator* Site::createExec(){
	int count = 2;
	string* options = new string[count];
	string* values = new string[count];
	options[0] = "query";
	options[1] = "execute";
	stringstream tmp;
	display(tmp);
	tmp << ends;
	values[0] = tmp.str();
	values[1] = "true";
	istream* in;
	cerr << "Shipping query: " << values[0] << endl;
	TRY(in = contactURL(name, options, values, count), NULL);
	delete [] options;
	delete [] values;

	return new StandReadExec(numFlds, typeIDs, in);
}

void Site::addTable(TableAlias* tabName){
	myFrom->append(tabName);
	string* alias = tabName->getAlias();
	assert(alias);
	tables->insert(*alias);
}

bool Site::have(const string& arg){
	set<string, ltstr> tmp;
	tmp.insert(arg);
	return includes(tables->begin(), tables->end(), tmp.begin(), tmp.end());
}

bool Site::have(const set<string, ltstr>& arg){
	return includes(tables->begin(), tables->end(), arg.begin(), arg.end());
}

void LocalTable::addTable(TableAlias* tabName){
	assert(myFrom->isEmpty());
	myFrom->append(tabName);
	string* alias = tabName->getAlias();
	assert(alias);
	tables->insert(*alias);
	name = *alias;
}

List<BaseSelection*>* createSelectList(const string& nm, PlanOp* iterat){
	assert(iterat);
	int numFlds = iterat->getNumFlds();
	const string* attNms = iterat->getAttributeNames();
	const string* types = iterat->getTypeIDs();
	Stats* stats = iterat->getStats();
	assert(stats);
	int* sizes = stats->fldSizes;
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	for(int i = 0; i < numFlds; i++){
		retVal->append(new PrimeSelection(
			new string(nm), new string(attNms[i]), types[i], sizes[i]));
	}
	return retVal;
}

List<BaseSelection*>* createSelectList(
	const string& table, List<string*>* attNms)
{
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	if(!attNms){
		return retVal;
	}
	for(attNms->rewind(); !attNms->atEnd(); attNms->step()){
		retVal->append(new 
			PrimeSelection(new string(table), new string(*attNms->get())));
	}
	return retVal;
}

/*	// this is not used

List<BaseSelection*>* createSelectList(PlanOp* iterat){
	assert(iterat);
	int numFlds = iterat->getNumFlds();
	const string* attNms = iterat->getAttributeNames();
	const string* types = iterat->getTypeIDs();
	Stats* stats = iterat->getStats();
	assert(stats);
	int* sizes = stats->fldSizes;
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	for(int i = 0; i < numFlds; i++){
		cerr << "Breaking " << attNms[i] << " into ";
		char* tmp = strdup(attNms[i].c_str());
		char* alias = strtok(tmp, ".");
		assert(alias);
		char* field = strtok(NULL, ".");
		assert(field);
		cerr << alias << " + " << field << endl;
		retVal->append(new 
			PrimeSelection(new string(alias), new string(field), types[i], sizes[i]));
		delete [] tmp;
	}
	return retVal;
}
*/

void LocalTable::typify(string option){	// Throws exception
	
	// option is ignored since the execution = profile + getNext

//	LOG(logFile << "Header: ");
//	LOG(iterat->display(logFile));	// iterat may be NULL

	if(!directSite){
		directSite = new DirectSite(name, iterat);
	}

	List<Site*>* tmpL = new List<Site*>;
	tmpL->append(directSite);
	TRY(typifyList(myWhere, tmpL), NVOID );
	TRY(boolCheckList(myWhere), NVOID );
	if(mySelect == NULL){
		assert(directSite);
		mySelect = createSelectList(name, iterat);
	}
	else{
		TRY(typifyList(mySelect, tmpL), NVOID );
	}
	numFlds = mySelect->cardinality();
	setStats();
	myFrom->rewind();
	TableAlias* ta = myFrom->get();
	if(ta && ta->getFunction()){
		string* function = ta->getFunction();
		int shiftVal = ta->getShiftVal();
		assert(!"Need to convert iterator to site");
		assert(iterat);
		assert(!"not implemented");
		// iterat = new FunctionRead(iterat, function, shiftVal);
		assert(iterat);
		// TRY(iterat->initialize(), NVOID );
	}
}

void Site::filter(List<BaseSelection*>* select, List<BaseSelection*>* where){
	filterList(select, this);
	if(where){
		filterList(where, this);
		where->rewind();
		while(!where->atEnd()){
			BaseSelection* currPred = where->get();
			if(currPred->exclusive(this)){
				where->remove();
				myWhere->append(currPred->selectionF());
			}
			else{
				where->step();
			}
		}
	}
	if(select != NULL){
		mySelect = new List<BaseSelection*>;
		collectFrom(select, this, mySelect);
		if(where){
			collectFrom(where, this, mySelect);
		}
	}
}

void Site::filterAll(List<BaseSelection*>* select){
	
	assert(select);
	mySelect = new List<BaseSelection*>;
	mySelect->addList(select);
	filterList(select, this);
}

istream* contactURL(string name, 
	const string* options, const string* values, int count){

	// throws exception

	TRY(URL* url = new URL(name), NULL);
	ostream* out = url->getOutputStream();
	for(int i = 0; i < count; i++){
		*out << options[i];
		*out << "=";
		stringstream tmp;
		tmp << values[i];
		tmp << ends;
		ostringstream* encoded = URL::encode(tmp);
  		*out << encoded->rdbuf();
		delete encoded;
		if(i < count - 1){
			*out << "&";
		}
	}
	TRY(istream* in = url->getInputStream(), NULL);
	assert(in);
	assert(in->good());
	delete url;
	string code;
	*in >> code;
	int codeVal;
	if(code.length() == 0){
		string msg = "No response from DTE (it probably crashed)";
		THROW(new Exception(msg), NULL);
	}
	if(!isdigit(code[0])){
		codeVal = -1;
	}
	else{
		codeVal = atoi(code.c_str());
	}
	char buff[100];
	if(codeVal != 0){
		ostringstream err;
		err << code << endl;
		while(*in){
			in->read(buff, 100);
			err.write(buff, in->gcount());
		} 
		err << ends;
		string msg = "Wrong code: " + string(err.str());
		THROW(new Exception(msg), NULL);
	}
	in->getline(buff, 100);	// ignore OK msg
	return in;
}

void Site::typify(string option){	// Throws an exception
	int count = 2;
	string* options = new string[count];
	string* values = new string[count];
	options[0] = "query";
	options[1] = "typecheck";
	stringstream tmp;
	display(tmp);
	tmp << ends;
	values[0] = tmp.str();
	values[1] = "true";
	istream* in;
	TRY(in = contactURL(name, options, values, count), NVOID );
	delete [] options;
	delete [] values;

	ISchema schema;
	*in >> schema;
	if(!(*in)){
		string err = "Illegal schema received from the DTE server";
		THROW(new Exception(err), NVOID );
	}
	delete in;
	numFlds = schema.getNumFlds();
	stats = new Stats(numFlds);
	assert(stats);
	if(mySelect == NULL){
		mySelect = new List<BaseSelection*>;
		for(int i = 0; i < numFlds; i++){
			string field = schema.getAttributeNames()[i];
			int dotpos = field.find(".");
			string* alias = new string(field.substr(0, dotpos));
			string* attnm = new string(field.substr(dotpos + 1));
			PrimeSelection* tmp = new PrimeSelection(alias, attnm);
			mySelect->append(tmp);
		}
	}
	const TypeID* types = schema.getTypeIDs();
	typeIDs = new TypeID[numFlds];
	for(int i = 0; i < numFlds; i++){
		typeIDs[i] = types[i];
	}
	int* sizes = stats->fldSizes;
	mySelect->rewind();
	for(int i = 0; i < numFlds; i++){
		mySelect->get()->setTypeID(types[i]);
		mySelect->get()->setSize(sizes[i]);
		mySelect->step();
	}
}

void Site::display(ostream& out, int detail){
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
}

void CGISite::typify(string option){	// Throws a exception
	TRY(URL* url = new URL(urlString), NVOID );
	ostream* out = url->getOutputStream();
	assert(myWhere);
	myWhere->rewind();
	while(!myWhere->atEnd()){
		BaseSelection* current = myWhere->get();
		assert(current->selectID() == OP_ID);
		Operator* currOp = (Operator*) current;
		string option = currOp->getAttribute();
		string value = currOp->getValue();
		int i = 0;
		while(1){
			if(i == entryLen){
				myWhere->step();
				break;
			}
			if(entry[i].option == option){
				entry[i].value = value;
				myWhere->remove();
				break;
			}
			i++;
		}
	}

	for(int i = 0; i < entryLen; i++){
		if(entry[i].value.empty()){
			string err = "Option " + name + "." + entry[i].option + 
				" has to be specified";
			THROW(new Exception(err), NVOID );
		}
	}

	for(int i = 0; i < entryLen; i++){
		stringstream ss;
		ss << entry[i].value;
		ostringstream* encoded = URL::encode(ss);
		*out << entry[i].option << "=" << encoded->rdbuf();
		delete encoded;
		if(i < entryLen - 1){
			*out << "&";
		}
	}

	TRY(istream* in = url->getInputStream(), NVOID );
	delete url;
	assert(in);
	assert(in->good());
	iterat = new NCDCRead();
	assert(iterat);
	TRY(iterat->open(in), NVOID );

	TRY(LocalTable::typify(option), NVOID );

}

void LocalTable::writeOpen(int mode){
	if(fileToWrite == ""){
		THROW(new Exception("This site is read only"), NVOID );
	}
	if(fout){
		THROW(new Exception("Already opened"), NVOID );
	}
	fout = new ofstream(fileToWrite.c_str(), mode);
	assert(fout);
	int numFlds = getNumFlds();
	writePtrs = new WritePtr[numFlds];
	const TypeID* types = getTypeIDs();
	for(int i = 0; i < numFlds; i++){
		TRY(writePtrs[i] = getWritePtr(types[i]), NVOID );
	}
}

void LocalTable::setStats(){
	double selectivity = listSelectivity(myWhere);
	assert(directSite);
	Stats* baseStats = directSite->getStats();
	assert(baseStats);
	int cardinality = int(selectivity * baseStats->cardinality);
	int* sizes = sizesFromList(mySelect);
	stats = new Stats(numFlds, sizes, cardinality);
}

Iterator* createIteratorFor(
	const ISchema& schema, istream* in, const string& tableStr)
{
	assert(in && in->good());
	int numFlds = schema.getNumFlds();
	StandReadExec* fs = new StandReadExec(numFlds, schema.getTypeIDs(), in);
	Array<ExecExpr*>* select = new Array<ExecExpr*>(numFlds);
	for(int i = 0; i < numFlds; i++){
		(*select)[i] = new ExecSelect(0, i);
	}

	BaseSelection* name = new EnumSelection(0, "string");
	BaseSelection* value = new ConstantSelection(
		"string", strdup(tableStr.c_str()));
	BaseSelection* predicate = new Operator("=", name, value);
	assert(predicate);
	predicate->typify(NULL);

	Array<ExecExpr*>* where = new Array<ExecExpr*>(1);
	TRY((*where)[0] = predicate->createExec("", NULL, "", NULL), NULL);
	assert((*where)[0]);
	return new SelProjExec(fs, select, where);
}

List<Site*>* LocalTable::generateAlternatives(){ // Throws exception
	List<Site*>* retVal = new List<Site*>;
	int totalNumPreds = myWhere->cardinality();
	assert(myFrom);
	assert(myFrom->cardinality() == 1);
	string tableNm = getFullNm();
	Iterator* indexIt;
	istream* indexStream = getIndexTableStream();
	if(!indexStream || !indexStream->good()){
		return retVal;
	}
	TRY(indexIt = createIteratorFor(INDEX_SCHEMA, indexStream, tableNm), NULL);
	assert(INDEX_SCHEMA.getNumFlds() == 3);
	indexIt->initialize();
	const Tuple* tup;
	while((tup = indexIt->getNext())){
		IndexDesc* indexDesc = new IndexDesc(*((IndexDesc*) tup[2]));

#ifdef DEBUG
		cout << "Index Desc for " << tableNm << ": ";
		indexDesc->display(cout);
		cout << endl;
#endif
		int totNumIndFlds = indexDesc->getTotNumFlds();
		string* allAttrNms = indexDesc->getAllAttrNms();
		RTreeIndex* currInd = new RTreeIndex(indexDesc);
		myWhere->rewind();
		int usablePredCnt = 0;
		List<BaseSelection*> indexOnlyPreds;
		while(!myWhere->atEnd()){
			BaseSelection* currPred = myWhere->get();
			TRY(bool usable = currInd->canUse(currPred), NULL);
			if(usable){
				usablePredCnt++;
			}
			if(currPred->exclusive(allAttrNms, totNumIndFlds)){
				indexOnlyPreds.append(currPred);
			}
			myWhere->step();
		}

		int indexOnlyPredCnt = indexOnlyPreds.cardinality();
		Site* newAlt = NULL;
		bool selectInc = exclusiveList(mySelect, allAttrNms, totNumIndFlds);
//		cout << "indexOnlyPredCnt = " << indexOnlyPredCnt << endl;
//		cout << "selectInc = " << selectInc << endl;
		if(indexOnlyPredCnt == totalNumPreds && selectInc){
			
			// Can do index only scan

			newAlt = new LocalTable(name, mySelect, myWhere, currInd);
//			cout << "Chose Index only scan" << endl;

		}
//		else if(indexOnlyPredCnt > 0){
//
//			// Can use index, insert Filter above index
//
//			Site* filter = new LocalTable(
//
//		}
		else if(usablePredCnt > 0){

			// Can use index, hook it directly to the indexscan

			newAlt = new IndexScan(
					name, mySelect, myWhere, currInd, iterat);
		}
		else {

			// cout << "Index is useless" << endl;
		}

		if(newAlt){
			retVal->append(newAlt);
		}
	}
	delete indexIt;
	return retVal;
}


void SiteGroup::typify(string option){	// Throws exception
	
	List<Site*>* tmpL = new List<Site*>;
	tmpL->append(site1);
	tmpL->append(site2);
	if(mySelect == NULL){
		mySelect = createSelectList(name, iterat);
	}
	else{
		TRY(typifyList(mySelect, tmpL), NVOID );
	}
	numFlds = mySelect->cardinality();
	TRY(typifyList(myWhere, tmpL), NVOID );
	double selectivity = listSelectivity(myWhere);
	int card1 = site1->getStats()->cardinality;
	int card2 = site2->getStats()->cardinality;
	int cardinality = int(selectivity * card1 * card2);
	int* sizes = sizesFromList(mySelect);
	stats = new Stats(numFlds, sizes, cardinality);
	TRY(boolCheckList(myWhere), NVOID );
}

Iterator* LocalTable::createExec(){
	assert(directSite);
	List<BaseSelection*>* baseISchema = directSite->getSelectList();
	Array<ExecExpr*>* select;
	Array<ExecExpr*>* where;
	TRY(select = enumerateList(mySelect, name, baseISchema), NULL);
	TRY(where = enumerateList(myWhere, name, baseISchema), NULL);
	Iterator* it = directSite->createExec();
	return new SelProjExec(it, select, where);
}

Iterator* IndexScan::createExec(){
	assert(directSite);
	List<BaseSelection*>* baseISchema = directSite->getSelectList();
	Array<ExecExpr*>* select;
	Array<ExecExpr*>* where;
	TRY(select = enumerateList(mySelect, name, baseISchema), NULL);
	TRY(where = enumerateList(myWhere, name, baseISchema), NULL);
	Iterator* it = directSite->createExec();
	RTreeReadExec* indexIter = (RTreeReadExec*) index->createExec();
	return new IndexScanExec(indexIter, it, select, where);
}

Iterator* SiteGroup::createExec(){
	Array<ExecExpr*>* select;
	Array<ExecExpr*>* where;
	TRY(select = enumerateList(mySelect, site1->getName(), site1->mySelect, 
		site2->getName(), site2->mySelect), NULL);
	TRY(where = enumerateList(myWhere, site1->getName(), site1->mySelect, 
		site2->getName(), site2->mySelect), NULL);
	int innerNumFlds = site2->getNumFlds();
	TRY(Iterator* it1 = site1->createExec(), NULL);
	TRY(Iterator* it2 = site2->createExec(), NULL);
	TupleLoader* tupleLoader = new TupleLoader;
	TRY(tupleLoader->open(innerNumFlds, site2->getTypeIDs()), NULL);
	return new NLJoinExec(it1, it2, select, where, innerNumFlds, tupleLoader);
}

Iterator* UnionSite::createExec(){
	TRY(Iterator* it1 = iter1->createExec(), NULL);
	TRY(Iterator* it2 = iter2->createExec(), NULL);
	return new UnionExec(it1, it2);
}

Iterator* DirectSite::createExec(){
	assert(iterat);
	return iterat->createExec();  // no projections or selections
}

