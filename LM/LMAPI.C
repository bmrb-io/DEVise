#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "LMAPI.h"
#include "LMControl.h"
#include "LMError.h"
#include "LMLayout.h"

#define ERR_MESG(str)  strcat(strcat(argv[0], str), argv[1])

// decide where to put these functions

int LMAPI(int argc, char **argv, LMControl *control)
{
#ifndef STANDALONE
  if (strcmp(argv[0], "hasTDataChanged") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    } else {
      char buf[3];
      sprintf(buf, "%d", win->HasTDataChanged());
      control->ReturnVal(LM_OK, buf);
    }
    return 1;
  }
  if (strcmp(argv[0], "dumpTData") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    } else {
      win->Write();
      control->ReturnVal(LM_OK, "1");
    }
    return 1;
  }
  if (strcmp(argv[0], "setTData" ) == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    } else {
      win->SetTDataFile(strdup(argv[2]));
      win->SetTDataName(strdup(argv[3]));
      control->ReturnVal(LM_OK, "1");
    }
    return 1;
  }
  if (strcmp(argv[0], "getTData" ) == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    } else {
      char buf[500];
      sprintf(buf, "{%s} {%s}", win->GetTDataFile(), win->GetTDataName());
      control->ReturnVal(LM_OK, buf);
    }
    return 1;
  }
  if (strcmp(argv[0], "XWindowCreated") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    } else {
      char buf[3];
      printf("Xwindowcreated = %d\n", win->XWindowCreated());
      fflush(stdout);
      sprintf(buf, "%d", win->XWindowCreated());
      control->ReturnVal(LM_OK, buf);
    }
    return 1;
  }
  if (strcmp(argv[0], "SetXWindowCreated") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_OK, "0");
    } else {
      win->SetXWindowCreated();
      control->ReturnVal(LM_OK, "Done");
    }
    return 1;
  }
#endif
  if (strcmp(argv[0], "createWindow") == 0) {
    Coord x, y, x0, y0;
    unsigned int w, h;
    int bg, fg;
    sscanf(argv[2], "%lf", &x0);
    sscanf(argv[3], "%lf", &y0);
    sscanf(argv[4], "%lf", &x);
    sscanf(argv[5], "%lf", &y);
    sscanf(argv[6], "%u", &w);
    sscanf(argv[7], "%u", &h);

    printf("%s : orig (%f, %f), pos (%f, %f), %u, %u , %s , %s \n", 
	   argv[1], x0, y0, x, y, w, h, argv[8], argv[9]);
    if (control->LMGetWindow(argv[1]) != NULL) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : Cannot create window -already exists"));
      return -1;
    }
    LMWindow *parent = NULL;

    if (argv[10]) {
      parent = control->LMGetWindow(argv[10]);
      if (!parent) {
	control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
	return -1;
      }
    }
    LMWindow *win = new LMWindow(argv[1], 
				 x0, y0, x, y,
				 w, h, 
				 argv[8],argv[9],parent);
    if(!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    control->LMCreateEntry(argv[1], win);
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }
  if (strcmp(argv[0], "getWinList") == 0) {
    Tcl_HashEntry *entryPtr;
    Tcl_HashSearch search;
    control->ResetReturnVal();
    for (entryPtr = control->InitWindowIterator(&search);
	 entryPtr != NULL;
	 entryPtr = control->GetNextWindow(&search)) {
      LMWindow *win = (LMWindow *)Tcl_GetHashValue(entryPtr);
      control->AppendReturnVal(win->Name());
    }
    return 1;
  }
  if (strcmp(argv[0], "deleteWindow") == 0){ 
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    control->LMDeleteEntry(argv[1]);
    win->DeleteThisWindow(atoi(argv[2]));
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }

  if (strcmp(argv[0], "printWindow") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    cout << *win;
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }

  if (strcmp(argv[0], "getBgColor") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    control->ReturnVal(LM_OK, win->BgColor());
    return 1;
  } 

  if (strcmp(argv[0], "getFgColor") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    control->ReturnVal(LM_OK, win->FgColor());
    return 1;
  } 
  
  if (strcmp(argv[0], "setBgColor") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    printf("setting bg  color : %s \n", argv[2]);
    win->SetBgColor(argv[2]);
    control->ReturnVal(LM_OK, "Done");
    return 1;
  } 

  if (strcmp(argv[0], "setFgColor") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    printf("setting fg color : %s \n", argv[2]);
    win->SetFgColor(argv[2]);
    control->ReturnVal(LM_OK, "Done");
    return 1;
  } 

  if (strcmp(argv[0], "setOrig") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    win->SetOrigin(atoi(argv[2]), atoi(argv[3]));
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }
  if (strcmp(argv[0], "setPosn") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }

    //   win->MoveWindow(atoi(argv[2]), atoi(argv[3]));
    //   win->ResizeWindow(atoi(argv[4]), atoi(argv[5]));
    win->HandleMoveResize(atoi(argv[2]), atoi(argv[3]),
			  atoi(argv[4]), atoi(argv[5]));
    
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }
  if (strcmp(argv[0], "getPosn") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    char buf[256];
    sprintf(buf, "%f %f %f %f %u %u", win->origX(), win->origY(), 
	    win->relX(), win->relY(), win->Width(), win->Height());
    control->ReturnVal(LM_OK, buf);
    return 1;
  } 
  if (strcmp (argv[0], "createProperty") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    /* TODO  - create property */
    control->ReturnVal(LM_OK, "Done");
    return 1;
  } 

  if (strcmp (argv[0], "deleteProperty") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    /* TODO  - delete property */
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }
  if (strcmp (argv[0], "getProperties") == 0) {
    /* get all properties */
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    char buf[256];
    /*
    printf("Properties :%s %d %d %d %d %u %u %s %s ", 
	   win->Name(),
	   win->origX(), win->origY(), 
	   win->relX(), win->relY(),
	   win->Width(), win->Height(),
	   win->FgColor(), win->BgColor());
	   */
    sprintf(buf, "{%s} %f %f %f %f %u %u %s %s ", 
	    win->Name(),
	    win->origX(), win->origY(), 
	    win->relX(), win->relY(),
	    win->Width(), win->Height(),
	    win->BgColor(), win->FgColor());
    control->ReturnVal(LM_OK, buf);
    return 1;
  }
  if (strcmp (argv[0], "setParent") == 0 ) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    LMWindow *parent = control->LMGetWindow(argv[2]);
    if (!parent) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    win->SetParent(parent);
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }
  if (strcmp (argv[0], "numChildren") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    char buf[4];
    sprintf(buf, "%d", win->NumChildren());
    control->ReturnVal(LM_OK, buf);
    return 1;
  }
  if (strcmp (argv[0], "getChildren") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(": cannot find window"));
      return -1;
    }
    if (!win->NumChildren()) {
      control->ReturnVal(LM_OK, "");
      return 1;
    }
    int index;  
    for (index = win->InitChildListIterator();
	 win->MoreChildren(index);  ) {
      LMWindow *winChild = win->NextChild(index);
      if (!win) {
	control->ReturnVal(LM_ERR, ERR_MESG(": cannot find window"));
	return -1;
      } 
      control->AppendReturnVal(winChild->Name());
    }
    win->DoneChildListIterator(index);
    return 1;
  }
  if (strcmp (argv[0], "getParent") == 0 ) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    LMWindow *parent = win->GetParent();
    if (!parent) {
      control->ReturnVal(LM_OK, "");
    } else {
      control->ReturnVal(LM_OK, parent->Name());
    }
    return 1;
  }
  if (strcmp (argv[0], "getLayoutProperties") == 0 ) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    char buf[100];
    LayoutMode mode;
    int rows, cols;
    win->GetLayoutProperties(mode,rows,cols);
    sprintf(buf, "%s %d %d", LMLayout::ModeTypeToStr(mode), rows, cols);
    control->ReturnVal(LM_OK, buf);
    return 1;
  }
  if (strcmp (argv[0], "getLayoutMode") == 0 ) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    LayoutMode modeType = win->GetLayoutMode();
    char *mode = LMLayout::ModeTypeToStr(modeType);
    control->ReturnVal(LM_OK, mode);
    return 1;
  }
  
  if (strcmp (argv[0], "moveWindow") == 0) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    win->MoveWindow(atoi(argv[2]), atoi(argv[3]));
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }

  if (strcmp (argv[0], "setLayoutProperties") == 0 ) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    LayoutMode mode = LMLayout::ModeStrToType(argv[2]);
    win->SetLayoutProperties(mode,atoi(argv[3]), atoi(argv[4]));
    control->ReturnVal(LM_OK, "Done");
    return 1;
  }
  if (strcmp (argv[0], "getLevel") == 0 ) {
    LMWindow *win = control->LMGetWindow(argv[1]);
    if (!win) {
      control->ReturnVal(LM_ERR, ERR_MESG(" : cannot find window"));
      return -1;
    }
    char buf[10];
    sprintf(buf,"%d", win->GetLevel());
    control->ReturnVal(LM_OK, buf);
    return 1;
  }
  
  if (strcmp(argv[0], "setFrameWidth") == 0) { 
    LMLayout::SetFrameWidth(atoi(argv[1]));
    control->ReturnVal(LM_OK, "1");
    return 1;
  } 
  if (strcmp(argv[0], "setScale") == 0) {
    float xscale, yscale;
    sscanf(argv[1], "%f", &xscale);
    sscanf(argv[2], "%f", &yscale);
    LMLayout::SetScale(xscale, yscale);
    control->ReturnVal(LM_OK, "1");
    return 1;
  }
  if (strcmp(argv[0], "exists") == 0) {
    if (control->LMGetWindow(argv[1]) == NULL) {
      control->ReturnVal(LM_OK, "0");
    } else {
      control->ReturnVal(LM_OK, "1");
    }
    return 1;
  }

  if (strcmp (argv[0], "remapChildren") == 0 ) {
  }

  if (strcmp (argv[0], "scaleChildren") == 0 ) {
  }

  if (strcmp (argv[0], "reparentWindow") == 0) {
  }


  if (strcmp (argv[0], "setRemapOnResize") == 0 ) {
  }
  
  if (strcmp (argv[0], "getRemapOnResize") == 0 ) {
  }
  
  if (strcmp (argv[0], "loadFromFile") == 0 ) {
  }

  if (strcmp (argv[0], "saveToFile") == 0 ) {
  }
  
  printf("Unknown command %s", argv[0]);
  return 0;
}


