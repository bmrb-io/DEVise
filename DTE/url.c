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
	istream* retVal;
	if(protocol == "file"){
		assert(!outputRequested);
		retVal = new ifstream(file);
		if(!retVal->good()){
			String msg = "Cannot open file: " + String(file);
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

		ostrstream reqStream;
		reqStream << "GET " << file << " HTTP/1.0\r\n\r\n";
		sendRequest(reqStream);
	}
	retVal = new istream(sockBuf);
	removeHeader(retVal);
	if(strncmp(header, "HTTP/1.0 200", 12)){
		String msg = "HTTP error:\n" + String(header);
		THROW(new Exception(msg), NULL);
	}
	return retVal;
}

void URL::parseURL(){	// Throws: unknown URL protocol
	if(strncmp(url, "file:", 5) == 0){
		protocol = "file";
		file = strdup(url + 5);
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
		String msg = "Unknow protocol: " + String(url);
		THROW(new Exception(msg), );
	}
}

ostrstream* URL::encode(strstream& input){
	ostrstream* encoded = new ostrstream();
	int len = input.pcount();
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
