#include <iostream.h>
#include "ParseAPI.h"
#include "ParseAPIDTE.h"
#include "CatalogComm.h"

int ParseAPIDTE(int argc, char **argv, TkControlPanel *control)
{
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
	    cout << "************** listing = " << catListing << endl;
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
}
