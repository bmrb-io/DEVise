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
   IMPORTANT!!  All member functions of the ControlPanel class that are
   called here must be implemented in the ControlPanelSimple class in
   Session.c.  RKW Oct. 2, 1997.
 */

/*
  $Id$

  $Log$
  Revision 1.11  1997/11/24 23:13:21  weaver
  Changes for the new ColorManager.

  Revision 1.10  1997/11/23 21:23:45  donjerko
  *** empty log message ***

  Revision 1.9  1997/11/12 23:17:46  donjerko
  Improved error checking.

  Revision 1.8  1997/11/05 00:20:27  donjerko
  Added some error checking calls to the DTE.

  Revision 1.7  1997/10/02 18:46:14  wenger
  Opening and saving batch-style sessions in back end now fully working;
  added tk2ds.tcl script for conversion.

  Revision 1.6  1997/08/21 21:04:51  donjerko
  Implemented view materialization

  Revision 1.5.6.1  1997/05/21 20:37:51  weaver
  Changes for new ColorManager

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
#include <string>

#ifdef DTE_STANDALONE
	#include "DTEControl.h"		// defines ControlPanel and API_ACK ...
#else
	#include "Control.h"
	#include "ParseAPI.h"
#endif

#include "ParseAPIDTE.h"
#include "CatalogComm.h"
#include "exception.h"

int ParseAPIDTE(int argc, char **argv, ControlPanel *control){

	if (argc == 1) {

		// This is for DTE exit ONLY!
		if(!strcmp(argv[0], "exit")){
			char* retVal="";
			control->ReturnVal(API_ACK, retVal);
			return 1;
		}

	}
	if (argc == 2) {

		if(!strcmp(argv[0], "dteListAllIndexes")){
			char* retVal = dteListAllIndexes(argv[1]);
			control->ReturnVal(API_ACK, retVal);
			return 1;
		}
		
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

	// TEMPTEMP -- Kent, I traced the type 11 error to here...

		char* catEntry = dteShowCatalogEntry(argv[1]);
		CATCH(
			string err = currExcept->toString();
			control->ReturnVal(API_NAK, (char*) err.c_str());
			return -1;
		)
		control->ReturnVal(API_ACK, catEntry);
		delete [] catEntry;
		return 1;
	}

	if(!strcmp(argv[0], "dteListCatalog")){
		int errorCode;	// 0 means OK
		char* catListing = dteListCatalog(argv[1], errorCode);
		if(errorCode){
			control->ReturnVal(API_NAK, catListing);
			delete [] catListing;
			return -1;
		}
		else{
			control->ReturnVal(API_ACK, catListing);
			delete [] catListing;
			return 1;
		}
	}

	if(!strcmp(argv[0], "dteListQueryAttributes")){
		char* attrListing = dteListQueryAttributes(argv[1]);
		CATCH(
			string err = "Failed to find available attributes.\n";
			err += currExcept->toString();
			control->ReturnVal(API_NAK, (char*) err.c_str());
			return -1;
		)
		control->ReturnVal(API_ACK, attrListing);
		delete [] attrListing;
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
    if(!strcmp(argv[0], "dteInsertCatalogEntry")){
      dteInsertCatalogEntry(argv[1], argv[2]);
      control->ReturnVal(API_ACK, "");
	 return 1;
    }
		if(!strcmp(argv[0], "dteCheckSQLViewEntry")){
			string errmsg = dteCheckSQLViewEntry(argv[1], argv[2]);
			if(errmsg.empty()){
				control->ReturnVal(API_ACK, "");
			}
			else{
				// this cast is OK
				control->ReturnVal(API_ACK, (char*) errmsg.c_str());
			}
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
	string tmp = string(argv[0]) +
	  ": no such command or wrong number of args";
	control->ReturnVal(API_NAK, strdup(tmp.c_str()));
	return -1;
}
