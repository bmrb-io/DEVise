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
  Revision 1.8  1997/08/22 23:13:07  okan
  Changed #include <string.h> 's to #include <string>

  Revision 1.7  1997/08/21 21:04:38  donjerko
  Implemented view materialization

  Revision 1.6  1997/06/16 16:04:56  donjerko
  New memory management in exec phase. Unidata included.

  Revision 1.5  1997/04/26 13:59:04  donjerko
  Fixed the extraction of header.

  Revision 1.4  1997/02/18 18:06:08  donjerko
  Added skeleton files for sorting.

  Revision 1.3  1997/02/03 04:11:38  donjerko
  Catalog management moved to DTE

  Revision 1.2  1996/12/05 16:06:06  wenger
  Added standard Devise file headers.

 */

#ifndef URL_H
#define URL_H

//#include <sys/types.h>   erased for sysdep.h
//#include <sys/socket.h>   erased for sysdep.h
// #include <sys/uio.h> // why is this here? (DD)
//#include <netinet/in.h>   erased for sysdep.h
//#include <netdb.h>   erased for sysdep.h (There was an ifndef SUN for this line)
#include <string>
#include <assert.h>
//#include <iostream.h>   erased for sysdep.h
//#include <strstream.h>   erased for sysdep.h
//#include <stdio.h>   erased for sysdep.h
//#include <unistd.h>   erased for sysdep.h
#include "SockStream.h"
#include "exception.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

const unsigned short httpport = 80;
const int MAX_HEADER = 512;

class URL {
friend ostream& operator<<(ostream& out, URL url);
private:
     char* url;
	char* host;
	char* file;
	string protocol;
	Cor_sockbuf* sockBuf;
	int sock;
	bool outputRequested;
	bool inputRequested;
	ostringstream userInput;
	char header[MAX_HEADER];
private:
	void parseURL();	// Throws: unknown URL protocol
     void sendRequest(ostringstream& req){
		ostream out(sockBuf);
		out << req.rdbuf();
		out << flush;
     }
	void removeHeader(istream* in){
		int space = MAX_HEADER;
		char* start = header;
	     in->getline(start, space);
		int cnt = in->gcount();
		while(cnt > 2){
			space -= cnt;
			assert(space > 0);
			start += cnt;
			*(start - 1) = '\n';
			in->getline(start, space); 
			cnt = in->gcount();
		}
	}
public:
	URL(string url) : url(strdup((char*) url.c_str())) {
		outputRequested = false;
		inputRequested = false;
		host = file = NULL;
		sockBuf = NULL;
		parseURL();
		if(protocol == "http"){
			sockBuf = new Cor_sockbuf(host, httpport);
		}
	}
	URL(URL* baseURL, string relativeURL){
		outputRequested = false;
		host = file = NULL;
		sockBuf = NULL;
		string tmp = string(baseURL->url) + relativeURL;
		url = strdup((char*)tmp.c_str());
		parseURL();
		if(protocol == "http"){
			sockBuf = new Cor_sockbuf(host, httpport);
		}
	}
	~URL(){

		// sockBuf cannot be deleted at this point because it is used
		// in the istream

		free(url);
		free(host);
		free(file);
	}
     istream* getInputStream();
     void post(ostringstream& content){
          ostringstream reqStream;
		string contentS = content.str();
		int contentLen = contentS.length();
          reqStream << "POST " << file << " HTTP/1.0\r\n";
		reqStream << "Content-Type: application/x-www-form-urlencoded\r\n";
		reqStream << "Content-Length: " << contentLen << "\r\n\r\n";
		reqStream << contentS;
          sendRequest(reqStream);
     }
	ostream* getOutputStream(int mode = ios::out){
		if(protocol == "file"){
			ostream* retVal =  new ofstream(file, mode);
			if(!retVal->good()){
				string msg = "Cannot open file: " + string(file);
				THROW(new Exception(msg), NULL);
			}
			return retVal;
		}
		if(inputRequested){
			THROW(new Exception("Cannot modify HTTP file"), NULL);
		}
		outputRequested = true;
		return &userInput;
	}
	char* getHeader(){
		return header;
	}
	static ostringstream* encode(stringstream& input);
};

#endif
