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

int main(int argc, char** argv)
{
  unsigned short port = 6571;
  const char* host = "localhost";
  if( argc > 1 ) {
    host = argv[1];
  }
  if( argc > 2 ) {
    port = atoi(argv[2]);
  }
  cerr << "connecting to " << host << ':' << port << '\n';
	Cor_sockbuf* sockBuf = new Cor_sockbuf(host, port);

	assert(sockBuf);

	if(!sockBuf->valid()){
		cerr << "Could not open connection to `" << host 
			<< "' at port " << port << endl;
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
