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

/*
	Implementation of TclClient class.
 */

/*
	$Id$

	$Log$
	Revision 1.1.2.2  1998/02/02 07:47:44  liping
	Fixed bugs related to: queue algorithms, bucket expansion, iterator reset

	Revision 1.1.2.1  1998/01/28 22:42:23  taodb
	Added support for group communicatoin

	Revision 1.2  1998/01/07 19:27:52  wenger
	Merged cleanup_1_4_7_br_4 thru cleanup_1_4_7_br_5 (integration of client/
	server library into Devise); updated solaris, sun, linux, and hp
	dependencies.

	Revision 1.1.2.1  1997/12/09 19:03:25  wenger
	deviseb now uses client/server library.

 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "TclClient.h"

//#define DEBUG
#undef DEBUG
#define DOASSERT(c,r) {if (!(c)) DoAbort(r); } 
TclClient::TclClient(char *name, char *hostname, int port, 
	char* initStr) :
	Client(name, hostname, port, initStr)
{
	_interp = Tcl_CreateInterp();
	DOASSERT(_interp, "Cannot create Tcl interpreter");
	if (Tcl_Init(_interp) == TCL_ERROR)
	DOASSERT(0, "Cannot initialize Tcl");
}

TclClient::~TclClient()
{
	Tcl_DeleteInterp(_interp);
}

// process all DEVise commands
int TclClient::ServerCmd(int argc, char **argv)
{
#ifdef DEBUG	
	printf("Function %s, %d args\n", argv[0], argc);
#endif

 	int flag = Client::ServerCmd(argc, argv);


	Tcl_SetResult(_interp, _result, TCL_VOLATILE);
	delete _result;
	_result = NULL;

	/*
	if (flag <= 0) {
		return TCL_ERROR;
	}
	*/

	return TCL_OK;
}

void TclClient::ReadServer()
{
	Client::ReadServer();
}

void TclClient::ControlCmd(int argc, char **argv)
{
	// paste the commands
	Client::ControlCmd(argc, argv);
#ifdef DEBUG
	printf("Executing control command: \"%s\"\n", _cmd);
#endif
	(void)Tcl_Eval(_interp, _cmd);
	delete _cmd;
	_cmd = NULL;
}
