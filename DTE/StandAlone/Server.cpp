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

#include "InitShut.h"
#include "SockStream.h"
#include "Common.h"
#include "sysdep.h"
#include "exception.h"

const unsigned short PORT = 6571;

int main(int argc, char** argv){

	initialize_system();
	
	Cor_sockbuf listener(PORT);

	while(true){
		Cor_sockbuf* sockBuf = listener.AcceptConnection(); 
		iostream str(sockBuf);
		string protocol;
		str >> protocol;
		string query = extractQuery(str);
		str << "DTE/0.0 ";
		processQuery(query, str, "200 OK\n\n");
		CATCH(str << 500 << " " << currExcept->toString() << "\n\n";)
		delete sockBuf;
	}

	shutdown_system();
	return 1;
}
