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
  Revision 1.7  1997/09/05 22:20:23  donjerko
  Made changes for port to NT.

  Revision 1.6  1997/08/21 21:04:38  donjerko
  Implemented view materialization

  Revision 1.5  1997/04/26 13:59:03  donjerko
  Fixed the extraction of header.

  Revision 1.4  1997/02/03 04:11:38  donjerko
  Catalog management moved to DTE

  Revision 1.3  1996/12/16 11:13:14  kmurli
  Changes to make the code work for separate TDataDQL etc..and also changes
  done to make Aggregates more robust

  Revision 1.2  1996/12/05 16:06:06  wenger
  Added standard Devise file headers.

 */

#include "url.h"
#include "exception.h"

ostream& operator<<(ostream& out, URL url){
     out << "url: " << url.url << endl;
	out << "protocol: " << url.protocol << endl;
     out << "host: " << url.host << endl;
     out << "file: " << url.file << endl;
     return out;
}

istream* URL::getInputStream(){
	istream* retVal = NULL;
	if(protocol == "file"){
		assert(!outputRequested);
		retVal = new ifstream(file);
		if(!retVal->good()){
			string msg = "Cannot open file: " + string(file);
			THROW(new Exception(msg), NULL);
		}
		return retVal;
	}

	// protocol = "http"

	if(outputRequested){

		// will read the answer from the script
		
		post(userInput);
	}
	else{

		// have to GET this file

		ostringstream reqStream;
		reqStream << "GET " << file << " HTTP/1.0\r\n\r\n" ;
		sendRequest(reqStream);
	}
	retVal = new istream(sockBuf);
	removeHeader(retVal);
	const char* codeOff = strstr(header, " ");
	if(!codeOff || strncmp(codeOff, " 200", 4) != 0){
		string msg = "HTTP error:\n" + string(header);
		THROW(new Exception(msg), NULL);
	}
	return retVal;
}

void URL::parseURL(){	// Throws: unknown URL protocol
	if(strncmp(url, "file:", 5) == 0){
		protocol = "file";
		file = strdup(url + 5);
	}
	else if(url[0] == '/' || url[0] == '.'){
		protocol = "file";
		file = strdup(url);
	}
	else if(strncmp(url, "http:", 5) == 0){
		protocol = "http";
		char* slash1;
		char* slash2;
		char* tmp = strdup(url);
		for(slash1 = tmp; *slash1 != '/'; slash1 += 1){
		}
		for(slash2 = slash1 + 2; *slash2 != '/'; slash2 += 1){
		}
		*slash2 = '\0';
		host = strdup(slash1 + 2);
		*slash2 = '/';
		file = strdup(slash2);
		delete tmp;
	}
	else{
		string msg = "Unknow protocol: " + string(url);
		THROW(new Exception(msg), NVOID );
	}
}

ostringstream* URL::encode(stringstream& input){
	ostringstream* encoded = new ostringstream();
	string inputS = input.str();
	int len = inputS.length();
	char c;
	for(int i = 0; i < len; i++){
		input.get(c);
		switch(c){
		case '&':
			assert('&' == 0x26);
			*encoded << "%26";
			break;
		case '=':
			assert('=' == 0x3D);
			*encoded << "%3D";
			break;
		case '+':
			assert('+' == 0x2B);
			*encoded << "%2B";
			break;
		case '%':
			assert('%' == 0x25);
			*encoded << "%25";
			break;
		case ',':
			assert(',' == 0x2C);
			*encoded << "%2C";
			break;
		case ' ':
			*encoded << "+";
			break;
		case '\t':
			*encoded << "+";
			break;
		case '\0':
			*encoded << "%00";
			break;
		default:
			*encoded << c;
			break;
		}
	}
	return encoded;
}
