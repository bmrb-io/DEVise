
#ifndef _ZOOCOMMANDS_H_
#define _ZOOCOMMANDS_H_

/*
   This file is NOT part of the standard ETk server. It is only
   necessary to create the version of ETk that supports the Zoo
   object browser
*/

#include <tcl.h>

int ZooQueryCmd(ClientData clientData, Tcl_Interp *interp,
		int argc, char *argv[]);
	    
int ZooInsertCmd(ClientData clientData, Tcl_Interp *interp,
		 int argc, char *argv[]);

int ZooUpdateCmd(ClientData clientData, Tcl_Interp *interp,
		 int argc, char *argv[]);

#endif

