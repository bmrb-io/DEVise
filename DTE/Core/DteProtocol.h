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

#ifndef DTE_PROTOCOL_H
#define DTE_PROTOCOL_H

#include <string>
#include <assert.h>
#include "SockStream.h"
#include "exception.h"
#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif

class DteProtocol {
private:
	string host;
	int port;

	Cor_sockbuf sockBuf;
	iostream str;
public:
	DteProtocol(const string& host, int port) 
		: host(host), port(port), sockBuf(host.c_str(), port), str(&sockBuf)
	{
	}
	ostream& getOutStream()		// throw
	{
		if(!sockBuf.valid()){
			string err = "Connection to the DB Server failed.\n";
			err += "Make sure the server is running";
			THROW(new Exception(err), str);
		}
		assert(str);
		str << "DTE/0.0 ";
		return str;
	}
	istream& getInStream()		// throw
	{
		string response;
		getline(str, response);
		string dteProt = response.substr(0, 7);
		if(dteProt != "DTE/0.0"){
			string err = "Unknown DTE protocol: " + dteProt;
			THROW(new Exception(err), str);
		}
		string code = response.substr(8, 3);
		if(code != "200"){
			string err = "Server responded with error: ";
			err += response.substr(8);
			THROW(new Exception(err), str);
		}
		getline(str, response);		// remove the empty line
		return str;
	}
};

#endif
