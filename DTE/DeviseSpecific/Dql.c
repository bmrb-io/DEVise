/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1996
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream.h>
#include <fstream.h>
#include <ctype.h>

#include <tcl.h>
#include <tk.h>

#include "machdep.h"
#include "Util.h"
#include "Parse.h"

#include "Timer.h"

int dql(int argc, char **argv);

//#define DEBUG
//#define DEBUG_IO

#define COMPUTE_MIN_MAX
#define USE_RANGE_INFO

static Tcl_Interp *globalInterp = 0;

#define UPDATE_TCL { (void)Tcl_Eval(globalInterp, "update"); }

static const char *resultHeader = "Result of Execution\n";
static const char *schemaFmt =
	"ISchema ( < %[^>]> ) OrderAttr %d density %f, range %n";
static const char *dateAttr = "hmdy";
static const char *intAttr = "int";
static const char *doubleAttr = "double";
static const char *stringAttr = "string";
static const char *attrSep = ", ";
static const char attrTypeSep = ':';
static const char *rangeSep = " ... ";
static const char dataFieldSep = '\t';

static const char newline    = '\n';
static const char dateChar   = 'd';
static const char intChar    = 'i';
static const char doubleChar = 'f';
static const char stringChar = 's';

static char *schema = 0;
static const int maxNumAttrs = 16;
static char schemaDef[maxNumAttrs + 1];
static double minVal[maxNumAttrs];
static double maxVal[maxNumAttrs];
static int orderAttr = -1;
static float density = 0.0;

int dql_extract(ClientData cd, Tcl_Interp *interp, int argc, char **argv)
{
  // Allow other functions to UPDATE_TCL

	Timer::StopTimer();
  globalInterp = interp;

  DOASSERT(argc == 5, "Invalid parameters");

 for(int i = 0; i < argc; i++)
  printf(" ARGV - %d = %s \n",i,argv[i]);

  dql(argc, argv);

//  UPDATE_TCL
	Timer::StartTimer();
  return TCL_OK;

}
