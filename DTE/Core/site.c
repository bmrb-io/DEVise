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

#include <string.h>
#include <ctype.h>
#include "site.h"
#include "Iterator.h"
#include "catalog.h"
#include "ParseTree.h" 	// for getRootCatalog
#ifdef NO_RTREE
     #include "RTreeRead.dummy"
#else
     #include "RTreeRead.h"
#endif

List<BaseSelection*>* createSelectList(String nm, Iterator* iterator){
	assert(iterator);
	int numFlds = iterator->getNumFlds();
	String* attNms = iterator->getAttributeNames();
	String* types = iterator->getTypeIDs();
	Stats* stats = iterator->getStats();
	assert(stats);
	int* sizes = stats->fldSizes;
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	for(int i = 0; i < numFlds; i++){
		Path* path = new Path(&attNms[i], NULL);
		retVal->append(new 
			PrimeSelection(new String(nm), path, types[i], sizes[i], i));
	}
	return retVal;
}

List<BaseSelection*>* createSelectList(String table, List<String*>* attNms){
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	if(!attNms){
		return retVal;
	}
	for(attNms->rewind(); !attNms->atEnd(); attNms->step()){
		Path* path = new Path(new String(*attNms->get()), NULL);
		retVal->append(new 
			PrimeSelection(new String(table), path));
	}
	return retVal;
}

List<BaseSelection*>* createSelectList(Iterator* iterator){
	assert(iterator);
	int numFlds = iterator->getNumFlds();
	String* attNms = iterator->getAttributeNames();
	String* types = iterator->getTypeIDs();
	Stats* stats = iterator->getStats();
	assert(stats);
	int* sizes = stats->fldSizes;
	List<BaseSelection*>* retVal = new List<BaseSelection*>;
	for(int i = 0; i < numFlds; i++){
		String res[3];
		int numSplit = split(attNms[i], res, 3, String("."));
		assert(numSplit == 2);
		Path* path = new Path(new String(res[1]), NULL);
		retVal->append(new 
			PrimeSelection(new String(res[0]), path, types[i], sizes[i], i));
	}
	return retVal;
}

void LocalTable::typify(String option){	// Throws exception
	
	// option is ignored since the execution = profile + getNext

//	LOG(logFile << "Header: ");
//	LOG(iterator->display(logFile));	// iterator may be NULL

	if(!directSite){
		directSite = new DirectSite(name, iterator);
	}

	List<Site*>* tmpL = new List<Site*>;
	tmpL->append(directSite);
	TRY(typifyList(myWhere, tmpL), );
	TRY(boolCheckList(myWhere), );
	if(mySelect == NULL){
		assert(directSite);
		mySelect = createSelectList(name, iterator);
	}
	else{
		TRY(typifyList(mySelect, tmpL), );
	}
	numFlds = mySelect->cardinality();
	setStats();
	myFrom->rewind();
	TableAlias* ta = myFrom->get();
	if(ta && ta->getFunction()){
		String* function = ta->getFunction();
		int shiftVal = ta->getShiftVal();
		assert(!"Need to convert iterator to site");
		assert(iterator);
		iterator = new FunctionRead(iterator, function, shiftVal);
		assert(iterator);
		TRY(iterator->initialize(), );
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

istream* contactURL(String name, 
	const String* options, const String* values, int count){

	// throws exception

	TRY(URL* url = new URL(name), NULL);
	ostream* out = url->getOutputStream();
	for(int i = 0; i < count; i++){
		*out << options[i];
		*out << "=";
		strstream tmp;
		tmp << values[i];
		ostrstream* encoded = URL::encode(tmp);
  		*out << encoded->rdbuf();
		delete encoded;
		if(i < count - 1){
			*out << "&";
		}
	}
	TRY(istream* in = url->getInputStream(), NULL);
	delete url;
	String code;
	*in >> code;
	int codeVal;
	if(code.length() == 0){
		String msg = "No response from DTE (it probably crashed)";
		THROW(new Exception(msg), NULL);
	}
	if(!isdigit(code[0])){
		codeVal = -1;
	}
	else{
		codeVal = atoi(code.chars());
	}
	char buff[100];
	if(codeVal != 0){
		ostrstream err;
		err << code << endl;
		while(*in){
			in->read(buff, 100);
			err.write(buff, in->gcount());
		} 
		err << ends;
		String msg = "Wrong code: " + String(err.str());
		THROW(new Exception(msg), NULL);
	}
	in->getline(buff, 100);	// ignore OK msg
	return in;
}

void Site::typify(String option){	// Throws a exception
	int count = 2;
	String* options = new String[count];
	String* values = new String[count];
	options[0] = "query";
	options[1] = option;
	strstream tmp;
	display(tmp);
	tmp << ends;
	char* tmpstr = tmp.str();
	values[0] = String(tmpstr);
	delete tmpstr;
	values[1] = "true";
	istream* in;
	TRY(in = contactURL(name, options, values, count), );
	delete [] options;
	delete [] values;
	iterator = new StandardRead(in);	
	TRY(iterator->open(), );
	LOG(logFile << "Header: ");
	LOG(iterator->display(logFile));
	stats = iterator->getStats();
	assert(stats);
	numFlds = iterator->getNumFlds();
	if(mySelect == NULL){
		mySelect = createSelectList(iterator);
		return;
	}
	TypeID* types = iterator->getTypeIDs();
	int* sizes = stats->fldSizes;
	mySelect->rewind();
	for(int i = 0; i < numFlds; i++){
		mySelect->get()->setTypeID(types[i]);
		mySelect->get()->setSize(sizes[i]);
		mySelect->step();
	}
}

void Site::display(ostream& out, int detail = 0){
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

void CGISite::typify(String option){	// Throws a exception
	TRY(URL* url = new URL(urlString), );
	ostream* out = url->getOutputStream();
	assert(myWhere);
	myWhere->rewind();
	while(!myWhere->atEnd()){
		BaseSelection* current = myWhere->get();
		assert(current->selectID() == OP_ID);
		Operator* currOp = (Operator*) current;
		String option = currOp->getAttribute();
		String value = currOp->getValue();
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
			String err = "Option " + name + "." + entry[i].option + 
				" has to be specified";
			THROW(new Exception(err), );
		}
	}

	for(int i = 0; i < entryLen; i++){
		strstream ss;
		ss << entry[i].value;
		ostrstream* encoded = URL::encode(ss);
		*out << entry[i].option << "=" << encoded->rdbuf();
		delete encoded;
		if(i < entryLen - 1){
			*out << "&";
		}
	}

	TRY(istream* in = url->getInputStream(), );
	delete url;
	assert(in);
	assert(in->good());
	iterator = new NCDCRead(in);
	assert(iterator);
	TRY(iterator->open(), );

	TRY(LocalTable::typify(option), );

}

void LocalTable::writeOpen(int mode = ios::app){
	if(fileToWrite == ""){
		THROW(new Exception("This site is read only"), );
	}
	if(fout){
		THROW(new Exception("Already opened"), );
	}
	fout = new ofstream(fileToWrite.chars(), mode);
	assert(fout);
	int numFlds = getNumFlds();
	writePtrs = new WritePtr[numFlds];
	TypeID* types = getTypeIDs();
	for(int i = 0; i < numFlds; i++){
		TRY(writePtrs[i] = getWritePtr(types[i]), );
	}
	if(mode == ios::out){
		iterator->writeHeader(*fout);
	}
}

istream& CGISite::Entry::read(istream& in){	// throws
	in >> option;
	char tmp;
	in >> tmp;
	if(tmp != '"'){
		THROW(new Exception(
			"Wrong format in the CGIInterface"), in);
	}
	bool escape = false;
	while(1){
		in.get(tmp);
		if(!in){
			String e = "Catalog ends while reading CGIInterface";
			THROW(new Exception(e), in);
		}
		if(tmp == '\\'){
			if(escape){
				value += '\\';
				escape = false;
			}
			else{
				escape = true;
			}
		}
		else if(tmp == '"'){
			if(escape){
				value += '"';
				escape = false;
			}
			else{
				break;
			}
		}
		else{
			if(escape){
				value += '\\' + tmp;
				escape = false;
			}
			else{
				value += tmp;
			}
		}
	}
	return in;
}

void CGISite::Entry::write(ostream& out){
	assert(!value.contains('"'));		// encode these characters!
	assert(!value.contains('\\'));
	out << option << " " << "\"" << value << "\"";
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

Site* findIndexFor(String tableStr){	// throws
	Catalog* catalog = getRootCatalog();
	assert(catalog);
	TableName* tableName = new TableName(".sysind");
	TRY(Interface* interf = catalog->findInterface(tableName), NULL);
	delete catalog;
	TRY(Site* site = interf->getSite(), NULL);
	delete interf;
	BaseSelection* name = new PrimeSelection("t", "table");
	BaseSelection* value = new ConstantSelection(
		"string", new IString(tableStr.chars()));
	BaseSelection* predicate = new Operator("=", name, value);
	site->addPredicate(predicate);
	site->addTable(new TableAlias(".sysind", "t"));
	TRY(site->typify("execute"), NULL);
	TRY(site->enumerate(), NULL);
	site->initialize();
	return site;
}

List<Site*>* LocalTable::generateAlternatives(){ // Throws exception
	List<Site*>* retVal = new List<Site*>;
	int totalNumPreds = myWhere->cardinality();
	assert(myFrom);
	assert(myFrom->cardinality() == 1);
	String tableNm = getFullNm();
	TRY(Site* indexForTable = findIndexFor(tableNm), NULL);
	indexForTable->initialize();
	int tmpNumFlds = indexForTable->getNumFlds();
	assert(tmpNumFlds == 3);
	Tuple* tup = new Tuple[tmpNumFlds];
	while(indexForTable->getNext(tup)){
		IndexDesc* indexDesc = (IndexDesc*) tup[2];
		cout << "Index Desc for " << tableNm << ": ";
		indexDesc->display(cout);
		cout << endl;

		int totNumIndFlds = indexDesc->getTotNumFlds();
		String* allAttrNms = indexDesc->getAllAttrNms();
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
		cout << "indexOnlyPredCnt = " << indexOnlyPredCnt << endl;
		cout << "selectInc = " << selectInc << endl;
		if(indexOnlyPredCnt == totalNumPreds && selectInc){
			
			// Can do index only scan

			newAlt = new LocalTable(name, mySelect, myWhere, currInd);
			cout << "Chose Index only scan" << endl;

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
					name, mySelect, myWhere, currInd, iterator);
		}
		else {

			// Cannot do anything with this index
			cout << "Index is useless" << endl;
		}

		if(newAlt){
			retVal->append(newAlt);
		}
	}
	delete tup;
	delete indexForTable;
	return retVal;
}

bool SiteGroup::getNext(Tuple* next){
	bool cond = false;
	Tuple* innerTup = NULL;
	assert(outerTup);
	if(firstEntry){
		moreOuter = site1->getNext(outerTup);
		firstEntry = false;
	}
	int innerNumFlds = site2->getNumFlds();
	while(cond == false){
		if(firstPass){
			innerTup = new Tuple[innerNumFlds];
			if(site2->getNext(innerTup)){
				innerRel.append(innerTup);
			}
			else{
				firstPass = false;
				innerRel.rewind();
				if(innerRel.atEnd()){
					return false;
				}
				innerTup = innerRel.get();
				innerRel.step();
				moreOuter = site1->getNext(outerTup);
			}
		}
		else{
			if(innerRel.atEnd()){
				innerRel.rewind();
				moreOuter = site1->getNext(outerTup);
			}
			innerTup = innerRel.get();
			innerRel.step();
		}
		assert(innerTup);
		if(!moreOuter){
			return false;
		}
		cond = evaluateList(myWhere, outerTup, innerTup);
	}
	assert(moreOuter);
	tupleFromList(next, mySelect, outerTup, innerTup);
	return true;
}

void SiteGroup::typify(String option){	// Throws exception
	
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
