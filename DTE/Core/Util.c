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
  Revision 1.9  1997/08/25 15:28:12  donjerko
  Added minmax table

  Revision 1.8  1997/08/21 21:04:28  donjerko
  Implemented view materialization

  Revision 1.7  1997/08/14 02:08:54  donjerko
  Index catalog is now an independent file.

  Revision 1.6  1997/07/30 21:39:22  donjerko
  Separated execution part from typchecking in expressions.

  Revision 1.5  1997/06/16 16:04:46  donjerko
  New memory management in exec phase. Unidata included.


  Revision 1.3  1997/03/28 16:07:28  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include "types.h"
#include "exception.h"
// #include "catalog.h"
#include "Inserter.h"

#include "Utility.h"

#include <string>
//#include <fstream.h>   erased for sysdep.h
//#include <strstream.h>   erased for sysdep.h
//#include <stdlib.h>   erased for sysdep.h
#include "sysdep.h"

string selectFileName(const string& env, const string& def){
	char* nm = getenv(env.c_str());
	if(nm){
		return string(nm);
	}
	else{
		return def;
	}
}


/*
const Catalog* getRootCatalog(){
	string catalogName;
	catalogName = selectFileName("DEVISE_HOME_TABLE", "./catalog.dte");
	return &ROOT_CATALOG;
}
*/

istream* getIndexTableStream(){
	string catalogName = DTE_ENV_VARS.indexTable;
	istream* in = new ifstream(catalogName.c_str());
	if(!in || !in->good()){
		cerr << "Warning: could not open index file \"" << catalogName
			<< "\"\n";
	}
	return in;
}

ostream* getIndexTableOutStream(int mode){
	string catalogName = DTE_ENV_VARS.indexTable;
	ostream* in = new ofstream(catalogName.c_str(), mode);
	if(!in || !in->good()){
		cerr << "Warning: could not open index file \"" << catalogName
			<< "\"\n";
	}
	return in;
}

string stripQuotes(char* str){
	stringstream tmp;
	tmp << str << ends;
	string tmpstr;
	stripQuotes(tmp, tmpstr);
	return tmpstr;
}

void stripQuotes(istream& in, string& value){	// can throw excetion

	// use the other version

	// strips backslashes from the string (and outer quotes)

	value = "";
	char tmp;
	in >> tmp;
	if(!in){
		return;
	}
	if(tmp != '"'){
		THROW(new Exception("Leading \" expected"), NVOID );
	}
	bool escape = false;
	while(1){
		in.get(tmp);
		if(!in){
			string e = "Closing \" expected";
			THROW(new Exception(e), NVOID );
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
}

void stripQuotes(istream& in, char* buf, int bufsz){// can throw excetion

	// strips backslashes from the string (and outer quotes)

	char tmp;
	in >> tmp;
	int length = 0;
	buf[length] = '\0';
	if(!in){
		return;
	}
	if(tmp != '"'){
		THROW(new Exception("Leading \" expected"), NVOID );
	}
	bool escape = false;
	while(1){
		assert(length < bufsz);
		in.get(tmp);
		if(!in){
			string e = "Closing \" expected";
			buf[length] = '\0';
			THROW(new Exception(e), NVOID );
		}
		if(escape){
			escape = false;
			switch(tmp){
			case '\\':
				buf[length++] = '\\';
				break;
			case '"':
				buf[length++] = '"';
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
			buf[length++] = tmp;
		}
	}
	buf[length] = '\0';
}

string addQuotes(const string& in){	// can throw excetion

	// adds backslashes from the string (and outer quotes)

	char tmp;
	string value = "\"";
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

string* dupStrArr(const string* inp, int numFlds){
	string* retVal = new string[numFlds];
	for(int i = 0; i < numFlds; i++){
		retVal[i] = inp[i];
	}
	return retVal;
}
