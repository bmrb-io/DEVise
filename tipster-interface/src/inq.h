#ifndef INQ_TIPSTER
#define INQ_TIPSTER

#include <stdio.h>
#include <stdlib.h>
#include "network.h"
#include "tcl.h"
#include "tk.h"

// #define INQ_DEBUG

#define  INQ_TMPFILE     "/tmp/inq.query"
#define  INQ_DOC_TMPFILE  "/tmp/inq.doc.query"

#ifndef MAXMACNAME
#define MAXMACNAME  20
#endif

#ifndef MAXANSWER
#define MAXANSWER   10000
#endif

int inqQuery(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int inqFetch(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);
int inqTitle(ClientData clientData, Tcl_Interp *interp, int argc, char **argv);

extern char inquery_mac_name[MAXMACNAME];
extern char my_name[MAXMACNAME];

#endif
