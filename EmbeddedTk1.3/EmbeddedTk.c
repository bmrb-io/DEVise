
#include "EmbeddedTk.h"
#include "TkWindowRep.h"
#include <tcl.h>
#include <tk.h>
#include <errno.h>
#include <string.h>

extern "C" {
    char *strdup(const char *s1);
}

Display     *ETk_TheDisplay = NULL;
char        *ETk_TheDisplayName = NULL;
int          ETk_NumWindows = 0;
int          ETk_MaxWindows = ETK_MAX_WINDOWS;
ETk_Window  *ETk_Windows;
char         ETk_TclErrorMessage[ETK_MAX_STRLEN];
FILE        *ETk_LogFile = NULL;

static ETk_WinId
FindWindow(TkWindowRep *win);

//
// Initialize global variables
//
ETk_Status
ETk_Init(char *displayname, FILE *errfile)
{
    int i;
    
    // Make a copy of the display name
    if (displayname != NULL)
    {
	ETk_TheDisplayName = strdup(displayname);
    }
    else
    {
	char *temp = XDisplayName(NULL);
	if (temp == NULL)
	{
	    return ETk_DisplayError;
	}
	ETk_TheDisplayName = strdup(temp);
    }
    
#ifdef DEBUG
    // Keep a pointer to the error log
    ETk_LogFile = errfile;
#endif

    // Allocate an array of windows
    ETk_Windows = new ETk_Window[ETk_MaxWindows];
    if (ETk_Windows == NULL)
    {
	return ETk_OutOfMemory;
    }
    ETk_NumWindows = 0;
    
    // Mark all windows as unused
    for (i = 0; i < ETk_MaxWindows; i++)
    {
	ETk_Windows[i].in_use = 0;
	ETk_Windows[i].script = NULL;
    }

#if 0
    //
    // This is obsolete. I now let Tk handle all X events
    //
    if (!(ETk_TheDisplay = XOpenDisplay(ETk_TheDisplayName))) {
	return ETk_DisplayError;
    }
    ETk_TheScreen = DefaultScreen(ETk_TheDisplay);
    XSetIOErrorHandler(ETk_FatalError);
    XSetErrorHandler(ETk_NonFatalError);
#endif

    return ETk_OK;

}

//
// Free all resources
//
void ETk_ShutDown()
{
    // Do nothing for now...
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_ShutDown\n");    
#endif
}

/*
   
   2 ways that a window could be destroyed:
   
   * Receive Destroy event from X
     - StructureNofify() -> FreeWindowOnDestroy()
   
   * Client sends "free" command
     - ProcessCmd() -> FreeWindow() -> Tk_DestroyWindow()
       -> StructureNofify() -> FreeWindowOnDestroy()
       
   What should be done when a window is destroyed?...
   - delete Tcl interpreter
   - delete TkWindowRep
   - mark slot in ETk_Windows array as free
   - decrement ETk_NumWindows
   
*/

//
// Free a window before receiving destroy event
//
ETk_Status
ETk_FreeWindow(ETk_WinId handle)
{
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_FreeWindow %d\n", (int) handle);
#endif
    
    if (!ETk_Windows[handle].in_use)
	return ETk_InvalidWindow;
    
    if (ETk_Windows[handle].win == NULL)
	return ETk_InvalidWindow;
    
    Tk_Window tkwin = ETk_Windows[handle].win->GetTkWindow();
    if (tkwin == NULL)
	return ETk_InvalidWindow;

    //
    // Calling Tk_DestroyWindow will generate a Destroy event for the
    // window, and the StructureNotifyHandler will receive the
    // Destroy event and call ETk_FreeWindowOnDestroy, which actually
    // frees the resources for that window
    //
#ifdef DEBUG
    fprintf(ETk_LogFile, "Calling Tk_DestroyWindow 0x%x\n", (int) tkwin);
#endif
    Tk_DestroyWindow(tkwin);
    
    return ETk_OK;

}

//
// Free a window, after receiving Destroy event. Destroys the TkWindowRep
// object, freeing all resources for the window.
//
static ETk_Status
FreeWindowOnDestroy(ETk_WinId wid)
{
#ifdef DEBUG
    fprintf(ETk_LogFile, "BEGIN FreeWindowOnDestroy %d\n", (int) wid);
#endif
    
    if (!ETk_Windows[wid].in_use) 
	return ETk_InvalidWindow;

    delete ETk_Windows[wid].win;
    delete [] ETk_Windows[wid].script;

    ETk_NumWindows--;
    ETk_Windows[wid].in_use = 0;
    ETk_Windows[wid].win = NULL;
    ETk_Windows[wid].script = NULL;
    
#ifdef DEBUG
    fprintf(ETk_LogFile, "  Number of active windows = %d\n", ETk_NumWindows);
    fprintf(ETk_LogFile, "END FreeWindowOnDestroy %d\n", (int) wid);
#endif

    return ETk_OK;

}

//
// Event handler for StructureNotify events in the Tk windows
//
void 
StructureNotifyHandler(ClientData clientData, XEvent *eventPtr)
{
    TkWindowRep *win = (TkWindowRep *) clientData;

#ifdef DEBUG
    fprintf(ETk_LogFile, "BEGIN StructureNotifyHandler 0x%x\n", (int) win);
#endif
    
    if (eventPtr->type != DestroyNotify) {
#ifdef DEBUG
	fprintf(ETk_LogFile, "  Not a Destroy event!\n");
#endif
	return;
    }
    
    ETk_WinId handle = FindWindow(win);

    if (handle < 0) {
#ifdef DEBUG
	fprintf(ETk_LogFile, "  Invalid window\n");
#endif
	return;
    }
    
    FreeWindowOnDestroy(handle);
    
#ifdef DEBUG
    fprintf(ETk_LogFile, "END StructureNotifyHandler 0x%x\n", (int) win);
#endif
    
}

//
// Search the window array for a window that exists at the specified
// coordinates, and that is running a specific script
//
bool
ETk_WindowExists(char *script, int x, int y, int width, int height,
		 ETk_WinId &handle)
{
    int i;
    ETk_Window *w;
    for (i = 0; i < ETk_MaxWindows; i++)
    {
	w = &(ETk_Windows[i]);
	if (w->in_use && (w->x == x) && (w->y == y)
	    && (w->width == width) && (w->height == height)
	    && (script != NULL) && (w->script != NULL)
	    && (!strcmp(w->script, script)))
	{
	    handle = (ETk_WinId) i;
	    return true;
	}
    }
    return false;
}

//
// Search the window array and return an index for window win
//
static ETk_WinId
FindWindow(TkWindowRep *win)
{
    int i;
    for (i = 0; i < ETk_MaxWindows; i++)
    {
	if (ETk_Windows[i].in_use && (ETk_Windows[i].win == win))
	    return (ETk_WinId) i;
    }
    return (ETk_WinId) -1;
}

ETk_Status
ETk_RefreshWindow(ETk_WinId wid)
{
    if (!ETk_Windows[wid].in_use)
	return ETk_InvalidWindow;
    
    // Do nothing for now...
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_RefreshWindow %d\n", (int) wid);
#endif
    
    return ETk_OK;

}

ETk_Status
ETk_MoveWindow(ETk_WinId wid, int xCenter, int yCenter)
{
    if (!ETk_Windows[wid].in_use)
	return ETk_InvalidWindow;
    
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_MoveWindow %d. x = %d, y = %d\n",
	    (int) wid, xCenter, yCenter);
#endif
    
    TkWindowRep *tkwin = ETk_Windows[wid].win;
    if (tkwin == NULL)
	return ETk_InvalidWindow;
    
    int xleft, ytop, width, height;
    int newX, newY;
    
    tkwin->GetLocation(xleft, ytop, width, height);
    newX = xCenter - (width / 2);
    newY = yCenter - (height / 2);
    
    return tkwin->Move(newX, newY);

}

//
// Resize a window, leaving the center of the window at the same
// position
//
ETk_Status
ETk_ResizeWindow(ETk_WinId wid, int width, int height)
{
    if (!ETk_Windows[wid].in_use)
	return ETk_InvalidWindow;
    
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_ResizeWindow %d. x = %d, y = %d\n",
	    (int) wid, width, height);
#endif
    
    TkWindowRep *tkwin = ETk_Windows[wid].win;
    if (tkwin == NULL)
	return ETk_InvalidWindow;
    
    int xleft, ytop, oldwidth, oldheight;
    int xcenter, ycenter;
    
    tkwin->GetLocation(xleft, ytop, oldwidth, oldheight);
    xcenter = xleft + (oldwidth / 2);
    ycenter = ytop + (oldheight / 2);
    
    return tkwin->MoveResize(xcenter - (width / 2), ycenter - (height / 2),
			     width, height);

}

//
// Resize and move a window
//
ETk_Status
ETk_MoveResizeWindow(ETk_WinId wid, int xcenter, int ycenter,
		     int width, int height)
{
    if (!ETk_Windows[wid].in_use)
	return ETk_InvalidWindow;
    
#ifdef DEBUG
    fprintf(ETk_LogFile,
	    "Executing ETk_MoveResizeWindow %d. x %d, y %d, w %d, h %d\n",
	    (int) wid, xcenter, ycenter, width, height);
#endif
    
    TkWindowRep *tkwin = ETk_Windows[wid].win;
    if (tkwin == NULL)
	return ETk_InvalidWindow;
    
    return tkwin->MoveResize(xcenter - (width / 2), ycenter - (height / 2),
			     width, height);

}

ETk_Status
ETk_MapWindow(ETk_WinId wid)
{
    if (!ETk_Windows[wid].in_use)
	return ETk_InvalidWindow;
    
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_MapWindow %d\n", (int) wid);
#endif
    
    TkWindowRep *tkwin = ETk_Windows[wid].win;
    if (tkwin == NULL)
	return ETk_InvalidWindow;
    
    return tkwin->Map();

}

ETk_Status
ETk_UnmapWindow(ETk_WinId wid)
{
    if (!ETk_Windows[wid].in_use)
	return ETk_InvalidWindow;
    
#ifdef DEBUG
    fprintf(ETk_LogFile, "Executing ETk_UnmapWindow %d\n", (int) wid);
#endif
    
    TkWindowRep *tkwin = ETk_Windows[wid].win;
    if (tkwin == NULL)
	return ETk_InvalidWindow;
    
    return tkwin->Unmap();

}

char *
ETk_StatusToString(ETk_Status s)
{
    switch (s) {

      case ETk_OK:
	return "OK";
	break;
      case ETk_OutOfMemory:
	return "Out of memory";
	break;
      case ETk_DisplayError:
	return "Error opening display";
	break;
      case ETk_SizeMismatch:
	return "Size mismatch";
	break;
      case ETk_UnknownEventWindow:
	return "Unknown event window";
	break;
      case ETk_InvalidWindow:
	return "Invalid window";
	break;
      case ETk_EventNotHandled:
	return "Event not handled";
	break;
      case ETk_TooManyFDs:
	return "Too many file descriptors";
	break;
      case ETk_InvalidFD:
	return "No such file descriptor";
	break;
      case ETk_BadFD:
	return "Bad file descriptor";
	break;
      case ETk_Interrupted:
	return "Interrupt occurred";
	break;
      case ETk_TclError:
	return "Error in Tcl interpreter";
	break;
      case ETk_NoTclInterpreter:
	return "No Tcl interpreter exists";
	break;
      case ETk_Unknown:
	return "Unknown error";
	break;

    }
    
    return "Unknown error";
    
}

