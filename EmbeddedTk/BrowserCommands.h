#ifndef _BROWSERCOMMANDS_H_
#define _BROWSERCOMMANDS_H_


#include <tcl.h>

void BrowserInit();
int BrowserOpenURLCmd(ClientData clientData, Tcl_Interp *interp,
                      int argc, char *argv[]);


#endif // _BROWSERCOMMANDS_H_
