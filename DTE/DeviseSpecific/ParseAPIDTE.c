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
	String tmp = String(argv[0]) + ": no such command";
	control->ReturnVal(API_NAK, strdup(tmp.chars()));
	return -1;
}
