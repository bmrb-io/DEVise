#include "sysdep.h"

#ifndef __GNUG__
using namespace std;
#endif


class WSInitSocket {
public:
	WSInitSocket() {
	
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;
	 
		wVersionRequested = MAKEWORD( 2, 0 );
	 
		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			exit(1);
		}
	 
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
			HIBYTE( wsaData.wVersion ) != 0 ) {
			cerr << "Couldn't find a usable Winsock.dll, Needs Winsock2";
			exit(1); 
		}
	}

	~WSInitSocket() {
		WSACleanup();
	}
};

WSInitSocket MyWSocket;