#ifndef URL_H
#define URL_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <String.h>
#include <string.h>
#include <assert.h>
#include <iostream.h>
#include <strstream.h>
#include <stdio.h>
#include <unistd.h>
#include "SockStream.h"
#include "exception.h"

const unsigned short httpport = 80;
const int MAX_HEADER = 512;

class URL {
friend ostream& operator<<(ostream& out, URL url);
private:
     char* url;
	char* host;
	char* file;
	String protocol;
	Cor_sockbuf* sockBuf;
	int sock;
	bool outputRequested;
	ostrstream userInput;
	char header[MAX_HEADER];
private:
	void parseURL();	// Throws: unknown URL protocol
     void sendRequest(ostrstream& req){
		ostream out(sockBuf);
		out << req.rdbuf();
		out << flush;
     }
	void removeHeader(istream* in){
		int space = MAX_HEADER;
		char* start = header;
	     in->getline(start, space);
		int cnt = in->gcount();
		while(cnt > 1){
			space -= cnt;
			assert(space > 0);
			start += cnt;
			*(start - 1) = '\n';
			in->getline(start, space); 
			cnt = in->gcount();
		}
	}
public:
	URL(String url) : url(strdup((char*) url.chars())) {
		outputRequested = false;
		host = file = NULL;
		sockBuf = NULL;
		parseURL();
		if(protocol == "http"){
			sockBuf = new Cor_sockbuf(host, httpport);
		}
	}
	URL(URL* baseURL, String relativeURL){
		outputRequested = false;
		host = file = NULL;
		sockBuf = NULL;
		String tmp = String(baseURL->url) + relativeURL;
		url = strdup((char*)tmp.chars());
		parseURL();
		if(protocol == "http"){
			sockBuf = new Cor_sockbuf(host, httpport);
		}
	}
	~URL(){

		// sockBuf cannot be deleted at this point because it is used
		// in the istream

		delete url;
		delete host;
		delete file;
	}
     istream* getInputStream();
     void post(ostrstream& content){
          ostrstream reqStream;
		int contentLen = content.pcount();
          reqStream << "POST " << file << " HTTP/1.0\r\n";
		reqStream << "Content-Type: application/x-www-form-urlencoded\r\n";
		reqStream << "Content-Length: " << contentLen << "\r\n\r\n";
		reqStream << content.rdbuf();
          sendRequest(reqStream);
     }
	ostream* getOutputStream(){
		if(protocol == "file"){
			assert(!"not implemented");
		}
		outputRequested = true;
		return &userInput;
	}
	char* getHeader(){
		return header;
	}
	static ostrstream* encode(strstream& input);
};

#endif
