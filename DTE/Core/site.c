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
  Revision 1.5  1996/12/15 06:41:10  donjerko
  Added support for RTree indexes

  Revision 1.4  1996/12/05 16:06:05  wenger
  Added standard Devise file headers.

 */

#include <string.h>
#include "site.h"
#include "Iterator.h"
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

void Site::filter(List<BaseSelection*>* select, List<BaseSelection*>* where){
	assert(where);
	filterList(select, this);
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
	if(select == NULL){
		mySelect = NULL;
	}
	else{
		collectFrom(select, this, mySelect);
		collectFrom(where, this, mySelect);
	}
}

void Site::typify(String option){	// Throws a exception
	TRY(URL* url = new URL(name), );
	ostream* out = url->getOutputStream();
	*out << "query=";
	strstream tmp;
	display(tmp);
	String msg = "&" + option + "=true";
	ostrstream* encoded = URL::encode(tmp);
	*out << encoded->rdbuf() << msg;
	delete encoded;
	TRY(istream* in = url->getInputStream(), );
	delete url;
	LOG(logFile << "Request " << option << " from " << name << endl);
	int code;
	*in >> code;
	char buff[100];
	if(code != 0){
		ostrstream err;
		err << code << endl;
		while(*in){
			in->read(buff, 100);
			err.write(buff, in->gcount());
		} 
		err << ends;
		String msg = "Wrong code: " + String(err.str());
		THROW(new Exception(msg), );
	}
	in->getline(buff, 100);	// ignore OK msg
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


List<Site*>* LocalTable::generateAlternatives(){ // Throws exception
	List<Site*>* retVal = new List<Site*>;
	indexes.rewind();
	int totalNumPreds = myWhere->cardinality();
	while(!indexes.atEnd()){
		RTreeIndex* currInd = indexes.get();
		int numIndFlds = currInd->getNumFlds();
		String* indAttrNms = currInd->getAttributeNames();

		// This is the point where to get real aternatives

		myWhere->rewind();
		int usablePredCnt = 0;
		List<BaseSelection*> indexOnlyPreds;
		while(!myWhere->atEnd()){
			BaseSelection* currPred = myWhere->get();
			if(currInd->canUse(currPred)){
				usablePredCnt++;
			}
			if(currPred->exclusive(indAttrNms, numIndFlds)){
				indexOnlyPreds.append(currPred);
			}
			myWhere->step();
		}

		int indexOnlyPredCnt = indexOnlyPreds.cardinality();
		Site* newAlt = NULL;
		bool selectInc = exclusiveList(mySelect, indAttrNms, numIndFlds);
		cout << "indexOnlyPredCnt = " << indexOnlyPredCnt << endl;
		cout << "selectInc = " << selectInc << endl;
		if(indexOnlyPredCnt == totalNumPreds && selectInc){
			
			// Can do index only scan

			newAlt = new LocalTable(name, mySelect, myWhere, currInd);
			cout << "Chose Index only scan" << endl;

		}
		/*
		else if(indexOnlyPredCnt > 0){

			// Can use index, insert Filter above index

			Site* filter = new LocalTable(

		}
		*/
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
		indexes.step();
	}
	return retVal;
}
