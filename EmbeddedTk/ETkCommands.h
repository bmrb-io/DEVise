
#ifndef _ETKCOMMANDS_H_
#define _ETKCOMMANDS_H_

#include <tcl.h>

int
ETk_GetCoordsCmd(ClientData clientData,    /* TkWindowRep *     */
		 Tcl_Interp *interp,       /* Tcl interpreter   */
		 int argc, char *argv[]);  /* The Tcl command   */

#if defined(LIBCS)
/*
   This procedure is NOT part of the standard ETk server. It is only
   necessary to create the version of ETk that supports the Zoo object
   browser
*/
int ETk_ServerCmd(ClientData clientData,       /* Not used          */
		  Tcl_Interp *interp,          /* Tcl interpreter   */
		  int argc, char *argv[]);     /* The Tcl command   */
#endif
	      
#endif

