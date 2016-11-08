
#ifndef _ETK_DEVISECLIENT_H_
#define _ETK_DEVISECLIENT_H_

/*
   This file is NOT part of the standard ETk server. It is only
   necessary to create the version of ETk that supports the Zoo
   object browser
*/

#include "EmbeddedTk.h"
#if defined(LIBCS)
#include "ClientServer.h"
#endif

class ETk_DeviseClient : public Client
{
  public:
    ETk_DeviseClient(char *name, char *hostname, int port,
		     ETk_Status &status);
    int GetServerFD();
    virtual int ServerCmd(int argc, char **argv, Tcl_Interp *interp);
    virtual void MainLoop();
    virtual void ControlCmd(int argc, char **argv);
};

#endif;

