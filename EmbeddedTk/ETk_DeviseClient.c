
/*
   This file is NOT part of the standard ETk server. It is only
   necessary to create the version of ETk that supports the Zoo
   object browser
*/

#include "ETk_DeviseClient.h"
#include "EmbeddedTk.h"
#include <sys/time.h>
#include <sys/types.h>
#include <tcl.h>

extern Tk_FileProc ServiceConnectionRequest;

ETk_DeviseClient::ETk_DeviseClient(char *name, char *hostname, int port,
				   ETk_Status &status)
: Client(name, hostname, port)
{
#if defined(DEBUG)
    printf("BEGIN %s\n", __FUNCTION__);
#endif
    status = ETk_OK;
    if (_serverFd < 0)
    {
	status = ETk_CouldNotConnect;
    }
#if defined(DEBUG)
    printf("END %s\n", __FUNCTION__);
#endif

}

ETk_DeviseClient::~ETk_DeviseClient()
{
#if defined(DEBUG)
    printf("BEGIN %s\n", __FUNCTION__);
#endif
#if defined(DEBUG)
    printf("END %s\n", __FUNCTION__);
#endif
}

int
ETk_DeviseClient::ServerCmd(int argc, char **argv, Tcl_Interp *interp)
{
    u_short flag;
    int rargc;
    char **rargv;
    char *result;

#if defined(DEBUG)
    printf("BEGIN %s\n", __FUNCTION__);
    printf("  Cmd: %s, %d args\n", argv[0], argc);
#endif
    
    if (NetworkSend(_serverFd, API_CMD, 0, argc, argv) < 0)
    {
	Tcl_SetResult(interp, "Server has terminated", TCL_STATIC);
	return TCL_ERROR;
    }

    //
    // Do a select on the client fd, and the listening fd. While the
    // client fd is idle and there is activity on the listening fd,
    // process the incoming commands.
    //

    fd_set fdset;
    int maxFdCheck;
    int numFds;
    
    do
    {
	//
	// Initialize the fd set
	//
	memset(&fdset, 0, sizeof fdset);
	FD_SET(ETk_ListenFD, &fdset);
	FD_SET(_serverFd, &fdset);
	maxFdCheck = (ETk_ListenFD > _serverFd ? ETk_ListenFD : _serverFd);
	//
	// select()
	//
	numFds = select(maxFdCheck + 1, &fdset, 0, 0, 0);
	if (numFds < 0)
	{
	    Tcl_SetResult(interp, "select() failed", TCL_STATIC);
	    return TCL_ERROR;
	}
	//
	// Handle a new connection request
	//
	if (FD_ISSET(ETk_ListenFD, &fdset))
	{
	    ServiceConnectionRequest((ClientData) ETk_ListenFD, 0);
	}
	
    } while (!FD_ISSET(_serverFd, &fdset));
    
    do
    {
	if (NetworkReceive(_serverFd, 1, flag, rargc, rargv) <= 0)
	{
	    Tcl_SetResult(interp, "Server has terminated", TCL_STATIC);
	    return TCL_ERROR;
	}
	if (flag == API_CTL)
	{
	    ControlCmd(rargc, rargv);
	}
    } while (flag != API_ACK && flag != API_NAK);
    
    result = NetworkPaste(rargc, rargv);
    DOASSERT(result, "Out of memory");
#ifdef DEBUG
    printf("Received reply from server: \"%s\"\n", result);
#endif
    
    if (interp != NULL)
    {
	Tcl_SetResult(interp, result, TCL_VOLATILE);
    }
    delete result;
    
#if defined(DEBUG)
    printf("END %s\n", __FUNCTION__);
#endif

    if (flag == API_NAK)
	return TCL_ERROR;
    
    return TCL_OK;

}

void
ETk_DeviseClient::ControlCmd(int argc, char **argv)
{
#if defined(DEBUG)
    printf("BEGIN %s\n", __FUNCTION__);
#endif
    char *cmd = NetworkPaste(argc, argv);
    DOASSERT(cmd, "Out of memory");
    printf("Received command from server: \"%s\"\n", cmd);
    delete cmd;
#if defined(DEBUG)
    printf("END %s\n", __FUNCTION__);
#endif
}

void
ETk_DeviseClient::MainLoop()
{
#if defined(DEBUG)
    printf("BEGIN %s\n", __FUNCTION__);
#endif
#if defined(DEBUG)
    printf("END %s\n", __FUNCTION__);
#endif
}

