/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1997
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*
  $Id$

  $Log$
  Revision 1.5  1997/04/30 21:45:07  wenger
  Fixed non-constant strings in complex mappings bug; TDataAsciiInterp
  no longer gives warning message on blank data lines; added makefile
  targets to make a Purify'd version of multi; fixed uninitialized memory
  read in the DList code; fixed bug that caused 1.4 version of multi to
  always crash; better error messages in DTE command parser; version is
  now 1.4.4.

  Revision 1.4  1997/03/28 16:07:37  wenger
  Added headers to all source files that didn't have them; updated
  solaris, solsparc, and hp dependencies.

 */

#include <string.h>
#include <String.h>

#ifdef DTE_STANDALONE
	#include "DTEControl.h"		// defines ControlPanel and API_ACK ...
#else
	#include "Control.h"
	#include "ParseAPI.h"
#endif

#include "ParseAPIDTE.h"
#include "CatalogComm.h"

int ParseAPIDTE(int argc, char **argv, ControlPanel *control){

	if (argc == 1) {

		if(!strcmp(argv[0], "dteListAllIndexes")){
			char* retVal = dteListAllIndexes();
			control->ReturnVal(API_ACK, retVal);
			return 1;
		}
		
		// This is for DTE exit ONLY!
		if(!strcmp(argv[0], "exit")){
			char* retVal="";
			control->ReturnVal(API_ACK, retVal);
			return 1;
		}

	}
	if (argc == 2) {
    if(!strcmp(argv[0], "dteDeleteCatalogEntry")){
      dteDeleteCatalogEntry(argv[1]);
      control->ReturnVal(API_ACK, "");
      return 1;
    }

    if(!strcmp(argv[0], "dteMaterializeCatalogEntry")){
      dteMaterializeCatalogEntry(argv[1]);
      control->ReturnVal(API_ACK, "");
      return 1;
    }

    if(!strcmp(argv[0], "dteReadSQLFilter")){
      char* retVal = dteReadSQLFilter(argv[1]);
      control->ReturnVal(API_ACK, retVal);
      return 1;
    }
    if(!strcmp(argv[0], "dteShowCatalogEntry")){
      char* catEntry = dteShowCatalogEntry(argv[1]);
      control->ReturnVal(API_ACK, catEntry);
      return 1;
    }

    if(!strcmp(argv[0], "dteListCatalog")){
      char* catListing = dteListCatalog(argv[1]);
      control->ReturnVal(API_ACK, catListing);
      return 1;
    }

    if(!strcmp(argv[0], "dteListAttributes")){
      char* attrListing = dteListAttributes(argv[1]);
      control->ReturnVal(API_ACK, attrListing);
      return 1;
    }
	}

	if (argc == 3) {
    if(!strcmp(argv[0], "dteDeleteIndex")){
      dteDeleteIndex(argv[1], argv[2]);
      control->ReturnVal(API_ACK, "");
      return 1;
    }
    if(!strcmp(argv[0], "dteShowIndexDesc")){
      char* indexDesc = dteShowIndexDesc(argv[1], argv[2]);
      control->ReturnVal(API_ACK, indexDesc);
      return 1;
    }
    if(!strcmp(argv[0], "dteShowAttrNames")){
      char* attrListing = dteShowAttrNames(argv[1], argv[2]);
      control->ReturnVal(API_ACK, attrListing);
      return 1;
    }
    if(!strcmp(argv[0], "dteShowCatalogEntry")){
      char* catEntry = dteShowCatalogEntry(argv[1], argv[2]);
      control->ReturnVal(API_ACK, catEntry);
      return 1;
    }
    if(!strcmp(argv[0], "dteInsertCatalogEntry")){
      dteInsertCatalogEntry(argv[1], argv[2]);
      control->ReturnVal(API_ACK, "");
      return 1;
    }
	}

	if (argc == 6) {
     if (!strcmp(argv[0],"dteCreateIndex")){
          dteCreateIndex(argv[1], argv[2], argv[3], argv[4], argv[5]);
          control->ReturnVal(API_ACK, "");
          return 1;
     }
	}
	String tmp = String(argv[0]) +
	  ": no such command or wrong number of args";
	control->ReturnVal(API_NAK, strdup(tmp.chars()));
	return -1;
}
