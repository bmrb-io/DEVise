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
 */

#include <string.h>
#include "site.h"
#include "GeneralRead.h"

List<BaseSelection*>* createSelectList(String nm, GeneralRead* iterator){
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
		while(*in){
			in->read(buff, 100);
			err.write(buff, in->gcount());
		} 
		err << ends;
		THROW(new Exception(String(err.str())), );
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
		mySelect = createSelectList(name, iterator);
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


bool IndexScan::isIndexable(BaseSelection* predicate){
	String attr;
	String opName;
	BaseSelection* constant;
	if(predicate->isIndexable(attr, opName, constant)){
		for(int i = 0; i < index->numAttrs; i++){
			if(index->attrNames[i] == attr){
				cout << "Updating rtree query on att " << i;
				cout << "with: " << opName << " ";
				constant->display(cout);
				cout << endl;
				rTreeQuery[i].update(opName, constant);
				return true;
			}
		}
	}
	return false;
}
