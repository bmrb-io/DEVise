/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
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
  Revision 1.3  1997/03/28 16:07:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <String.h>
#include <iostream.h>
#include <strstream.h>
#include "exception.h"
#include "ParseTree.h"	// for getRootCatalog
#include "catalog.h"
#include "Inserter.h"

#include "Utility.h"

String& stripQuotes(char* str){
	strstream tmp;
	tmp << str << ends;
	return stripQuotes(tmp);
}

String& stripQuotes(istream& in){	// can throw excetion

	// strips backslashes from the string (and outer quotes)

	String& value = *(new String);
	char tmp;
	in >> tmp;
	if(!in){
		return value;
	}
	if(tmp != '"'){
		THROW(new Exception("Leading \" expected"), value);
	}
	bool escape = false;
	while(1){
		in.get(tmp);
		if(!in){
			String e = "Closing \" expected";
			THROW(new Exception(e), value);
		}
		if(escape){
			escape = false;
			switch(tmp){
			case '\\':
				value += '\\';
				break;
			case '"':
				value += '"';
				break;
			default:
				assert(0);
			}
		}
		else if(tmp == '\\'){
			escape = true;
		}
		else if(tmp == '"'){
			break;
		}
		else{
			value += tmp;
		}
	}
	return value;
}

String& addQuotes(String in){	// can throw excetion

	// adds backslashes from the string (and outer quotes)

	String& value = *(new String);
	char tmp;
	value += '"';
	for(unsigned int i = 0; i < in.length(); i++){
		tmp = in[i];
		switch(tmp){
		case '\\':
			value += "\\\\";
			break;
		case '"':
			value += "\\\"";
			break;
		default:
			value += tmp;
		}
	}
	value += '"';
	return value;
}

void insert(String tableStr, Tuple* tuple){	// throws exception
	Catalog* catalog = getRootCatalog();
	assert(catalog);
	TableName tableName(tableStr.chars());
	TRY(Interface* interf = catalog->findInterface(&tableName), );
	delete catalog;
	TRY(Inserter* inserter = interf->getInserter(&tableName), );
	delete interf;
	inserter->insert(tuple);
	delete inserter;
}

