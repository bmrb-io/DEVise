
#ifndef _EMBEDDEDTK_H_
#define _EMBEDDEDTK_H_

//#include "TkWindowRep.h"
#include <stdio.h>
#include <tcl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define ETK_MAX_STRLEN 1024
#define ETK_MAX_WINDOWS 1024

const int ETK_PORT = 6700;

typedef int ETk_WinId;

class TkWindowRep;

struct ETk_Window
{
    int in_use;
    int x;
    int y;
    int width;
    int height;
    TkWindowRep* win;
    char *script;
};

enum ETk_Status
{
    ETk_OK = 0,
    ETk_OutOfMemory,
    ETk_DisplayError,
    ETk_SizeMismatch,
    ETk_InvalidWindow,
    ETk_UnknownEventWindow,
    ETk_EventNotHandled,
    ETk_TooManyFDs,
    ETk_InvalidFD,
    ETk_BadFD,
    ETk_Interrupted,
    ETk_TclError,
    ETk_NoTclInterpreter,
    ETk_Unknown
};

//
// Global variables
//
extern Display    *ETk_TheDisplay;        // X display
extern char       *ETk_TheDisplayName;    // X display name
extern int         ETk_NumWindows;        // Number of windows in use
extern ETk_Window *ETk_Windows;           // Array of window structs
extern int         ETk_MaxWindows;        // Current size of window array
extern char        ETk_TclErrorMessage[]; // Error message from Tcl interp.
extern FILE*       ETk_LogFile;           // Error log

extern ETk_Status ETk_Init(char *dname, FILE *logfile);
extern void ETk_ShutDown();
extern ETk_Status ETk_FreeWindow(ETk_WinId wid);
extern void StructureNotifyHandler(ClientData clientData, XEvent* eventPtr);
extern ETk_Status ETk_MoveWindow(ETk_WinId wid, int xCenter, int yCenter);
extern ETk_Status ETk_ResizeWindow(ETk_WinId wid, int width, int height);
extern ETk_Status ETk_MoveResizeWindow(ETk_WinId wid,
				       int xCenter, int yCenter,
				       int width, int height);
extern ETk_Status ETk_MapWindow(ETk_WinId wid);
extern ETk_Status ETk_UnmapWindow(ETk_WinId wid);
extern char* ETk_StatusToString(ETk_Status s);
extern bool ETk_WindowExists(char *script, int x, int y,
			     int width, int height, ETk_WinId &handle);

#endif

