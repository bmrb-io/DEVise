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
  Revision 1.10  1997/11/23 21:23:36  donjerko
  Added ODBC stuff.

  Revision 1.9  1997/09/05 22:20:24  donjerko
  Made changes for port to NT.

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

#include <string>
#include <assert.h>
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
  string url;
  string host;
  string file;
  string protocol;
  Cor_sockbuf* sockBuf;
  // int sock;
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
	URL(const string& url) : url(url) {
		outputRequested = false;
		inputRequested = false;
		sockBuf = NULL;
		parseURL();
		if(protocol == "http"){
			sockBuf = new Cor_sockbuf(host.c_str(), httpport);
		}
	}
	URL(URL* baseURL, string relativeURL){
		outputRequested = false;
		sockBuf = NULL;
		url = baseURL->url + relativeURL;
		parseURL();
		if(protocol == "http"){
			sockBuf = new Cor_sockbuf(host.c_str(), httpport);
		}
	}
	~URL(){
		// sockBuf cannot be deleted at this point because it is used
		// in the istream
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
			ostream* retVal =  new ofstream(file.c_str(), mode);
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
  //static ostringstream* encode(istream& input);
};

#endif
