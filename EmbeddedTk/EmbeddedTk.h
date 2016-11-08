
#ifndef _EMBEDDEDTK_H_
#define _EMBEDDEDTK_H_

#include <stdio.h>
#include <tcl.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define ETK_MAX_STRLEN 1024
#define ETK_MAX_WINDOWS 1024

#if defined(LIBCS)
#include "ETk.h"
#else
const int ETK_PORT = 6700;
#endif

//
// Clients will be given integer "handles" for the windows that are
// created. These integers are indeces into the global ETk_Windows
// array. The following typedef is used to identify these handles in the
// ETk code.
//
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

enum ETkStatus
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
    ETk_CouldNotConnect,
    ETk_Unknown
};

// For backwards compatibility only
typedef ETkStatus ETk_Status;

//
// Global variables
//
extern Display    *ETk_TheDisplay;        // X display
extern char       *ETk_TheDisplayName;    // X display name
extern int         ETk_NumWindows;        // Number of windows in use
extern ETk_Window *ETk_Windows;           // Array of window structs
extern int         ETk_MaxWindows;        // Current size of window array
extern char        ETk_TclResult[];       // Result from Tcl interpreter
extern FILE*       ETk_LogFile;           // Error log
extern int         ETk_ListenFD;          // FD for listening

extern void        StructureNotifyHandler(ClientData clientData,
					  XEvent* eventPtr);

extern ETkStatus   ETk_Init(char *dname, FILE *logfile);
extern void        ETk_ShutDown();
extern ETkStatus   ETk_FreeWindow(ETk_WinId wid);
extern ETkStatus   ETk_MoveWindow(ETk_WinId wid, int xCenter, int yCenter);
extern ETkStatus   ETk_ResizeWindow(ETk_WinId wid, int width, int height);
extern ETkStatus   ETk_MoveResizeWindow(ETk_WinId wid,
					int xCenter, int yCenter,
					int width, int height);
extern ETkStatus   ETk_MapWindow(ETk_WinId wid);
extern ETkStatus   ETk_UnmapWindow(ETk_WinId wid);
extern ETkStatus   ETk_EvalCmd(ETk_WinId wid, char *cmd,
			       char *returnValue);
extern char       *ETk_StatusToString(ETkStatus s);
extern bool        ETk_WindowExists(char *script, int x, int y,
				    int width, int height,
				    ETk_WinId &handle);

#endif

