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

#include "SockStream.h"
#include "sysdep.h"

const unsigned short PORT = 6571;
const char* HOST = "quarg.cs.wisc.edu";

int main(int argc, char** argv){

	Cor_sockbuf* sockBuf = new Cor_sockbuf(HOST, PORT);

	assert(sockBuf);

	if(!sockBuf->valid()){
		cerr << "Could not open connection to `" << HOST 
			<< "' at port " << PORT << endl;
		cerr << "Server might be down" << endl;
		exit(1);
	}

	iostream str(sockBuf);

	str << "DTE/0.0 ";
	char c;
	while(cin.get(c)){
		str.put(c);
	}

	str.flush();

	while(str.get(c)){
		cout.put(c);
	}
	return 0;
}
