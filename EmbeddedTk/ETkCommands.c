
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
    
    if ((server = Tcl_GetVar2(interp,
			      "ETk", "ServerHost",
			      TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG)) == NULL)
    {
	printf("Error executing ETkServer command: %s\n",
	       interp->result);
	return TCL_ERROR;
    }
    
    if ((temp = Tcl_GetVar2(interp,
			    "ETk", "ServerPort",
			    TCL_GLOBAL_ONLY | TCL_LEAVE_ERR_MSG)) == NULL)
    {
	printf("Error executing ETkServer command: %s\n",
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

