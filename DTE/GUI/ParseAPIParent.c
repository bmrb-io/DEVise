#include "ParseAPI.h"
#include "ParseAPIParent.h"
#include "CatalogComm.h"

#include <iostream.h>
#include <sys/ddi.h>
#include <string.h>

int ParseAPIParent(int argc, char **argv, TkControlPanel *control)
{
  int  code;

  if ((argc>=1)&&(strlen(argv[0])>=3)&&(!strncmp(argv[0],"dte",3)))
    {
      code=ParseAPIDTE(argc, argv, control);
    }
  else if (!strcmp(argv[0], "exit")) 
    {
      control->ReturnVal(API_ACK, "done");
      control->RestartSession();
      return 1;
    }

  else
   {
      cout << "Wrong argument: argv[0] does not begin with <dte>\n";
    }

  
  return code;
}
