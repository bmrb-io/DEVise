#ifndef SMART_TIPSTER
#define SMART_TIPSTER

#include <stdio.h>
#include <stdlib.h>
#include "network.h"
#include "tcl.h"
#include "tk.h"

// #define SM_DEBUG

#define  SMART_TMPFILE  "/tmp/sm.query"
#define  SMART_DOC_TMPFILE  "/tmp/sm.doc.query"

#ifndef MAXMACNAME
#define MAXMACNAME  20
#endif

#ifndef MAXANSWER
#define MAXANSWER   10000
#endif

#ifndef MAXWORDLEN 
#define MAXWORDLEN 12
#endif

#ifndef MAXQUERYLEN 
#define MAXQUERYLEN 80
#endif

int smQuery(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int smAll(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int smQuit(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);

extern char smart_mac_name[MAXMACNAME];
extern char my_name[MAXMACNAME];

#endif
