
#include "TkWindowRep.h"
#include "ETkCommands.h"
#if defined(LIBCS)
#include "ETk_DeviseClient.h"
#endif

int
ETk_GetCoordsCmd(ClientData clientData,     /* TkWindowRep *     */
		 Tcl_Interp *interp,        /* Tcl interpreter   */
		 int argc, char *argv[])    /* A Tcl command     */
{
    //
    // This Tcl commands returns the coordinates of the ETk window
    // relative to its parent. The x-y values returned are the coords
    // of the CENTER of the window, NOT the upper-left corner. The
    // result will be a list of 4 integers: x y width height.
    //
    
    int i;
    int result = TCL_OK;
    ETkStatus status;
    TkWindowRep *tkwinrep = (TkWindowRep *) clientData;
    int xleft, ytop;
    unsigned int width, height;
    int xcenter, ycenter;
    
    status = tkwinrep->GetLocation(xleft, ytop, width, height);
    if (status != ETk_OK)
    {
	printf("Could not get coords for TkWindowRep: %s\n",
	       ETk_StatusToString(status));
	Tcl_SetResult(interp, (char *) ETk_StatusToString(status),
		      TCL_STATIC);
	result = TCL_ERROR;
    }
    
    xcenter = xleft + (width / 2);
    ycenter = ytop + (height / 2);

    sprintf(interp->result, "%d %d %d %d",
	    xcenter, ycenter, width, height);
    return TCL_OK;

}

#if defined(LIBCS)
/*
   This procedure is NOT part of the standard ETk server. It is only
   necessary to create the version of ETk that supports the Zoo object
   browser
*/
int
ETk_ServerCmd(ClientData clientData,    /* Not used          */
	      Tcl_Interp *interp,       /* Tcl interpreter   */
	      int argc, char *argv[])   /* A Tcl command     */
{
    //
    // This Tcl command sets up a connection with a DEVise server
    // (created with the DEVise client-server library), sends a command
    // to the server, reads a reply from the server, then closes
    // the connection.
    //
    
    int i;
    char *server;
    char *temp;
    int port;
    int result = TCL_OK;
    ETkStatus status;
    
    printf("BEGIN %s\n", __FUNCTION__);
    for (i = 0; i < argc; i++)
    {
	printf("  arg: %s\n", argv[i]);
    }

    //
    // Assuming that the location of the DEVise server (hostname, port)
    // is stored in the Tcl ETk array
    //
    if ((server = Tcl_GetVar2(interp,
			      "ETk", "ServerHost",
			      TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG)) == NULL)
    {
	printf("Error executing ETk_Server command: %s\n",
	       interp->result);
	return TCL_ERROR;
    }
    if ((temp = Tcl_GetVar2(interp,
			    "ETk", "ServerPort",
			    TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG)) == NULL)
    {
	printf("Error executing ETk_Server command: %s\n",
	       interp->result);
	return TCL_ERROR;
    }
    port = atoi(temp);
    
    ETk_DeviseClient client("ETk_DeviseClient", server, port, status);
    if (status != ETk_OK)
    {
	printf("Could not connect to server: %s\n",
	       ETk_StatusToString(status));
	Tcl_SetResult(interp, (char *) ETk_StatusToString(status),
		      TCL_STATIC);
	result = TCL_ERROR;
    }
    else
    {
	if ((result = client.ServerCmd(argc - 1, &argv[1], interp)) == TCL_OK)
	{
	    printf("Received result from server: %s\n", interp->result);
	}
	char *argv2[] = { "exit" };
	client.ServerCmd(1, argv2, NULL);
    }
    
    return result;

}
#endif // #if defined(LIBCS)

