/*
  ========================================================================
  DEVise Data Visualization Software
  (c) Copyright 1992-1995
  By the DEVise Development Group
  Madison, Wisconsin
  All Rights Reserved.
  ========================================================================

  Under no circumstances is this software to be copied, distributed,
  or altered in any way without prior permission from the DEVise
  Development Group.
*/

/*

  Revision 1.1  1997/09/25 15:13:33  zhenhai
  Copy from GLWindowRep to GLWindowRep
*/
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <X11/keysym.h>
#include <GL/gl.h>

#include "GLWindowRep.h"
#include "GLDisplay.h"
#include "Compress.h"
#include "DevError.h"
#include "ETkIfc.h"
#include "DaliIfc.h"
#include "../xvertext/rotated.h"


#if !defined(LIBCS)
#include "Init.h"
#endif

// Defines to get around namespace conflicts with xv code. CEW 5/13/97.
#define String XVString
#define black XVblack

extern "C" {
#include "xv.h"
}

#undef DELETE
#include "MapIntToInt.h"
#include "DeviseKey.h"
// xv.h has a conflict DEBUG - it has a variable called int DEBUG
// so this define must come after xv.h.
//#define DEBUG

#include "GraphicsDebug.h"

#include "GLDebug.h"

// Define this to produce some extra drawing to X to help debug stuff...
//#define X_DEBUG

// Uncomment this line to print some debugging info relevant to 
// creation and deletion of embedded Tk windows.
//#define DEBUG_ETK

#include "Util.h"

#define MAXPIXELDUMP 0

#define ROUND(type, value) ((type)(value + 0.5))
#define DRAWABLE           (_win ? _win : _pixmap)

#ifndef IMPLEMENTDLIST_DALIIMAGELIST
#define IMPLEMENTDLIST_DALIIMAGELIST
//ImplementDList(DaliImageList, int);
#endif
static const int AltMask = Mod1Mask | Mod2Mask | Mod3Mask | Mod5Mask;
static MapIntToInt devise_keymap(103);

class GLWindowRepInit
{
  // the purpose of this class is to initialize devise keymap
  // there GLWindowRepInit() constructor is called exactly once
  // with the following line: static GLWindowRepInit glwindowrep_initializer;
  public:

    struct Xlate {
        int xkey;
        int dkey;
    };

    GLWindowRepInit();
};

GLWindowRepInit::GLWindowRepInit()
{
    static Xlate xlate[] = {
        { XK_BackSpace,   DeviseKey::BACKSPACE },
        { XK_Tab,         DeviseKey::TAB },
        { XK_Linefeed,    DeviseKey::LINEFEED },
        { XK_Clear,       DeviseKey::CLEAR },
        { XK_Return,      DeviseKey::RETURN },
        { XK_Pause,       DeviseKey::PAUSE },
        { XK_Scroll_Lock, DeviseKey::SCROLL_LOCK },
        { XK_Escape,      DeviseKey::ESCAPE },
        { XK_Delete,      DeviseKey::DELETE },

        { XK_Home,        DeviseKey::HOME },
        { XK_Left,        DeviseKey::LEFT },
        { XK_Up,          DeviseKey::UP },
        { XK_Right,       DeviseKey::RIGHT },
        { XK_Down,        DeviseKey::DOWN },
        { XK_Prior,       DeviseKey::PRIOR },
        { XK_Next,        DeviseKey::NEXT },
        { XK_End,         DeviseKey::END },
        { XK_Begin,       DeviseKey::BEGIN },

        { XK_Select,      DeviseKey::SELECT },
        { XK_Print,       DeviseKey::PRINT },
        { XK_Execute,     DeviseKey::EXECUTE },
        { XK_Insert,      DeviseKey::INSERT },
        { XK_Undo,        DeviseKey::UNDO },
        { XK_Redo,        DeviseKey::REDO },
        { XK_Menu,        DeviseKey::MENU },
        { XK_Find,        DeviseKey::FIND },
        { XK_Cancel,      DeviseKey::CANCEL },
        { XK_Help,        DeviseKey::HELP },
        { XK_Break,       DeviseKey::BREAK },
        { XK_Mode_switch, DeviseKey::MODE_SWITCH },
        { XK_script_switch, DeviseKey::SCRIPT_SWITCH },

        { XK_KP_Space,    DeviseKey::KP_SPACE },
        { XK_KP_Tab,      DeviseKey::KP_TAB },
        { XK_KP_Enter,    DeviseKey::KP_ENTER },
        { XK_KP_F1,       DeviseKey::KP_F1 },
        { XK_KP_F2,       DeviseKey::KP_F2 },
        { XK_KP_F3,       DeviseKey::KP_F3 },
        { XK_KP_F4,       DeviseKey::KP_F4 },
        { XK_KP_Equal,    DeviseKey::KP_EQUAL },
        { XK_KP_Multiply, DeviseKey::KP_MULTIPLY },
        { XK_KP_Add,      DeviseKey::KP_ADD },
        { XK_KP_Separator, DeviseKey::KP_SEPARATOR },
        { XK_KP_Subtract, DeviseKey::KP_SUBTRACT },
        { XK_KP_Decimal,  DeviseKey::KP_DECIMAL },
        { XK_KP_Divide,   DeviseKey::KP_DIVIDE },
        { XK_KP_0,        DeviseKey::KP_0 },
        { XK_KP_1,        DeviseKey::KP_1 },
        { XK_KP_2,        DeviseKey::KP_2 },
        { XK_KP_3,        DeviseKey::KP_3 },
        { XK_KP_4,        DeviseKey::KP_4 },
        { XK_KP_5,        DeviseKey::KP_5 },
        { XK_KP_6,        DeviseKey::KP_6 },
        { XK_KP_7,        DeviseKey::KP_7 },
        { XK_KP_8,        DeviseKey::KP_8 },
        { XK_KP_9,        DeviseKey::KP_9 },

        { XK_F1,          DeviseKey::F1 },
        { XK_F2,          DeviseKey::F2 },
        { XK_F3,          DeviseKey::F3 },
        { XK_F4,          DeviseKey::F4 },
        { XK_F5,          DeviseKey::F5 },
        { XK_F6,          DeviseKey::F6 },
        { XK_F7,          DeviseKey::F7 },
        { XK_F8,          DeviseKey::F8 },
        { XK_F9,          DeviseKey::F9 },
        { XK_F10,         DeviseKey::F10 },
        { XK_F11,         DeviseKey::F11 },
        { XK_F12,         DeviseKey::F12 },

#if defined(XK_Sys_Req)
        // these are not defined in X11R5, but are in X11R6
        { XK_Sys_Req,     DeviseKey::SYS_REQ },
        { XK_Page_Up,     DeviseKey::PAGE_UP },
        { XK_Page_Down,   DeviseKey::PAGE_DOWN },
        { XK_KP_Home,     DeviseKey::KP_HOME },
        { XK_KP_Left,     DeviseKey::KP_LEFT },
        { XK_KP_Up,       DeviseKey::KP_UP },
        { XK_KP_Right,    DeviseKey::KP_RIGHT },
        { XK_KP_Down,     DeviseKey::KP_DOWN },
        { XK_KP_Prior,    DeviseKey::KP_PRIOR },
        { XK_KP_Page_Up,  DeviseKey::KP_PAGE_UP },
        { XK_KP_Next,     DeviseKey::KP_NEXT },
        { XK_KP_Page_Down, DeviseKey::KP_PAGE_DOWN },
        { XK_KP_End,      DeviseKey::KP_END },
        { XK_KP_Begin,    DeviseKey::KP_BEGIN },
        { XK_KP_Insert,   DeviseKey::KP_INSERT },
        { XK_KP_Delete,   DeviseKey::KP_DELETE },
#endif


        { 0, 0 }                // must be last entry!
    };

    for(int i = 0 ; xlate[i].xkey != 0 ; i++) {
        DOASSERT(devise_keymap.insert(xlate[i].xkey, xlate[i].dkey) == 0,
                 "couldn't insert devise_keymap entry");
    }
}

static GLWindowRepInit glwindowrep_initializer;


/* Decorate window */
#ifdef TK_WINDOW_old
void GLWindowRep::Decorate(WindowRep *parent, char *name,
			   unsigned int min_width,
			   unsigned int min_height)
{ 
  DOASSERT(!isInTkWindow(), "Invalid Tk window");
  EmbedInTkWindow((GLWindowRep *)parent, name, min_width, min_height);
}

/* Undecorate window */
void GLWindowRep::Undecorate()
{
  if (isInTkWindow())
    DetachFromTkWindow();

}
#endif

/* Reparent this window to 'other' or vice versa. */
void GLWindowRep::Reparent(Boolean child, void *other, int x, int y)
{
  DOASSERT(_win, "Cannot reparent a pixmap");

  Window newParent = (child ? _win : (Window)other);
  Window newChild = (child ? (Window)other : _win);

  if (!newParent)
    newParent = DefaultRootWindow(_display);

#ifdef DEBUG
  printf("GLWindowRep(0x%p)::Reparent 0x%lx to 0x%lx at %d,%d\n", this,
	 newChild, newParent, x, y);
#endif

  XReparentWindow(_display, newChild, newParent, x, y);
}

/* Raise window to top of stacking order */
void GLWindowRep::Raise()
{
  if (_win)
    XRaiseWindow(_display, _win);
  
  if (!_parent)
    return;
  
  if (!_parent->_children.Delete(this)) {
    fprintf(stderr, "Cannot remove child from parent\n");
    return;
  }
  _parent->_children.Append(this);
}

/* Lower window to bottom of stacking order */
void GLWindowRep::Lower()
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::Lower window 0x%lx:\n", this, _win);
#endif
  
  if (_win)
    XLowerWindow(_display, _win);
  
  if (!_parent)
    return;
  
  if (!_parent->_children.Delete(this)) {
    fprintf(stderr, "Cannot remove child from parent\n");
    return;
  }
  _parent->_children.Insert(this);
}

/* Flush windowRep's content to display */
void GLWindowRep::Flush()
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::Flush\n",this);
#endif
  // force the output
  MAKECURRENT();
  if (double_buffer)
    glXSwapBuffers(_display, _win);
  else
    glFlush();
  glXWaitGL();
  glXWaitX();
}

/* Find out whether window has backing store. */
Boolean GLWindowRep::HasBackingStore()
{
  return Boolean(double_buffer);
}

/* Move and resize window, relative to the parent */
void GLWindowRep::MoveResize(int x, int y, unsigned w, unsigned h)
{
#if defined(DEBUG)
  printf("MoveResize(x=%d,y=%d,w=%d,h=%d)", x, y, w, h);
#endif
  if (_win) {
    XMoveResizeWindow(_display, _win, x, y, w, h);
    MAKECURRENT();
    glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
    GLMATRIXMODE(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLsizei)_width, 0.0, (GLsizei)_height);
  }
  else {
    /* Resizing a pixmap involves deleting it and creating a new one */
    XFreePixmap(_display, _pixmap);
    unsigned int depth = DefaultDepth(_display, DefaultScreen(_display));
    _pixmap = XCreatePixmap(_display, DefaultRootWindow(_display),
			    w, h, depth);
    DOASSERT(_pixmap, "Cannot create pixmap");
    _x = x;
    _y = y;

    // clear all pixels in background and foreground
    SetForeground(GetBackground());
  }
    ; // don't know how to do yet.
}

/* Iconify window. Not guaranteed to succeed.  */
void GLWindowRep::Iconify()
{
  DOASSERT(_win, "Cannot iconify pixmap");
  XIconifyWindow(_display, _win, 0);
}

void GLWindowRep::PushClip(Coord x, Coord y, Coord w, Coord h)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::PushClip(%.2f,%.2f,%.2f,%.2f)\n",this, x, y, w, h);
#endif
}

void GLWindowRep::PopClip()
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::PopClip\n",this);
#endif
}

/* export window image to other graphics formats */
void GLWindowRep::ExportImage(DisplayExportFormat format, char *filename)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::ExportImage()\n",this);
#endif
}

DevStatus GLWindowRep::DaliShowImage(Coord centerX, Coord centerY,
				Coord width, Coord height,
				char *filename, int imageLen,
				char *image, float timeoutFactor = 1.0,
				Boolean maintainAspect = true)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::DaliShowImage(%f, %f, %f, %f, %s)\n",this, centerX, centerY,
	 width, height, filename != NULL ? filename : "(image)");
#endif
  
  DevStatus result = StatusOk;
  
  if (_daliServer == NULL) {
    char *serverName;
    DevStatus tmpResult =DaliIfc::LaunchServer(serverName);
    result += tmpResult;
    if (tmpResult.IsComplete()) {
      GetDisplay()->SetTasvirServer(serverName);
      DeviseDisplay::GetPSDisplay()->SetTasvirServer(serverName);
#if !defined(LIBCS)
      Init::SetDaliServer(serverName);
      Init::SetDaliQuit(true);
#endif
    }
  }
  
  if (_daliServer == NULL) {
    reportError("No Tasvir server specified", devNoSyserr);
    result = StatusFailed;
  }
  else {
    if (filename == NULL) filename = "-";
    
    int handle;
    result += DaliIfc::ShowImage(_daliServer, _win, (int) centerX,
				 (int) centerY, (int) width, (int) height, filename, imageLen, image,
				 handle, timeoutFactor, 1000, maintainAspect);
    if (result.IsComplete()) {
#if defined(DEBUG)
      printf("Displayed Tasvir image; handle = %d\n", handle);
#endif
      
      _daliImages.Insert(handle);
    }
  }

  return result;
}

DevStatus GLWindowRep::DaliFreeImages()
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::DaliFreeImages()\n",this);
#endif
  
  DevStatus result = StatusOk;
  
  int index = _daliImages.InitIterator(false);
  while (_daliImages.More(index)) {
    int handle = _daliImages.Next(index);
    _daliImages.DeleteCurrent(index);
    result += DaliIfc::FreeImage(_daliServer, handle);
  }
  _daliImages.DoneIterator(index);

  return result;
}
//------------------------------------------------------------------------
// BEGIN ETk interface
//

//
// Utility function for searching the ETk window list. Given an ETk
// handle, returns a pointer to the ETkInfo struct for that window,
// or NULL if no window exists with that handle.
//
static ETkInfo *
_etkSearch(ETkWinList *list, int handle)
{
    ETkInfo *current, *result;
    int i;
    result = NULL;
    i = list->InitIterator(false);
    while (list->More(i))
    {
	current = list->Next(i);
	if (current == NULL)
	{
	    continue;
	}
	if (current->handle == handle)
	{
	    result = current;
	}
    }
    list->DoneIterator(i);
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_CreateWindow()
//
// Show an embedded Tcl/Tk window in this WindowRep. filename is the
// name of a Tcl/Tk script to be executed. argc and argv carry the
// command-line arguments that will be passed to the Tcl/Tk script.
//
DevStatus GLWindowRep::ETk_CreateWindow(Coord x, Coord y,
				   Coord width, Coord height,
				   ETkIfc::Anchor anchor,
				   char *filename,
				   int argc, char **argv,
				   int &handle)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::ETk_CreateWindow()\n",this);
#endif

#if defined(DEBUG) && defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_CreateWindow(%s)\n",this, filename);
#endif
    
    DevStatus result = StatusOk;

    if (_etkServer == NULL)
    {
      char *serverName;
      DevStatus tmpResult = ETkIfc::LaunchServer(serverName);
      result += tmpResult;
      if (tmpResult.IsComplete()) {
        GetDisplay()->SetETkServer(serverName);
        ETkIfc::SetServer(serverName);
        ETkIfc::SetQuitFlag(true);
      }
    }
    
    if (_etkServer == NULL)
    {
	reportError("No ETk server specified", devNoSyserr);
	result = StatusFailed;
    }
    else
    {
	if (filename == NULL)
	    filename = "-";

	int wx = int(x);
	int wy = int(y);
	int wwidth = int(width);
	int wheight = int(height);
	result += ETkIfc::CreateWindow(_etkServer, _win,
				       wx, wy,
				       wwidth, wheight,
				       anchor,
				       filename,
				       argc, (const char **) argv,
				       handle);
	
	if (result.IsComplete())
	{
#if defined(DEBUG) || defined(DEBUG_ETK)
	    printf("Displayed ETk window. handle = %d\n", handle);
#endif

	    ETkInfo *info = new ETkInfo;
	    DOASSERT(info, "Cannot create ETkInfo object");
	    info->handle = handle;
	    info->x = wx;
	    info->y = wy;
	    info->width = wwidth;
	    info->height = wheight;
	    info->in_use = true;
	    strncpy(info->script, filename, FILENAME_MAX);
	    info->script[FILENAME_MAX] = '\0';
	    _etkWindows.Insert(info);
	}
    }
    
    return result;

}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_FindWindow()
//
// Searches for an embedded Tk window at the specified coordinates.
// Returns a handle for the window, or -1 if none exists.
// NOTE: the x,y coords are DATA coords, not pixel values.
//
int GLWindowRep::ETk_FindWindow(Coord centerX, Coord centerY,
			   char *script)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::ETk_FindWindow()\n",this);
#endif
  int iter, handle;
  ETkInfo *info;
    
  iter = _etkWindows.InitIterator(false);
  handle = -1;
    
  while (_etkWindows.More(iter)) {
    info = _etkWindows.Next(iter);
    if (info == NULL) {
      reportError("Invalid ETkInfo pointer in ETkWindow list",
		  devNoSyserr);
      continue;
    }
    if (info->x == centerX && info->y == centerY
	&& !strcmp(info->script, script)) {
      handle = info->handle;
      break;
    }
  }
  _etkWindows.DoneIterator(iter);

  return handle;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_MoveWindow()
//
// Moves an embedded Tk window
//
DevStatus GLWindowRep::ETk_MoveWindow(int handle,
				 Coord centerX, Coord centerY)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_MoveWindow(%d,%d,%d)\n",this, handle,
	   (int) centerX, (int) centerY);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to move an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::MoveWindow(_etkServer, handle,
				 (int) centerX, (int) centerY);
    if (result.IsComplete()) {
      info->x = centerX;
      info->y = centerY;
    }
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_ResizeWindow()
//
// Resizes an embedded Tk window
//
DevStatus GLWindowRep::ETk_ResizeWindow(int handle, Coord width, Coord height)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_ResizeWindow(%d,%d,%d)\n",this, handle,
	   (int) width, (int) height);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to resize an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::ResizeWindow(_etkServer, handle,
				   (int) width, (int) height);
    if (result.IsComplete())
    {
	info->width = width;
	info->height = height;
    }
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_NotifyResize()
//
// Allows ETk clients to notify DEVise when window size changes
//
DevStatus GLWindowRep::ETk_NotifyResize(int handle,
				   Coord xcenter, Coord ycenter,
				   Coord width, Coord height)
{
    DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_NotifyResize(%d,%d,%d)\n",this, handle,
	   (int) width, (int) height);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to resize an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    info->x = xcenter;
    info->y = ycenter;
    info->width = width;
    info->height = height;
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_MoveResizeWindow()
//
// Moves and resizes an embedded Tk window
//
DevStatus GLWindowRep::ETk_MoveResizeWindow(int handle,
				       Coord xcenter, Coord ycenter,
				       Coord width, Coord height)
{
     DevStatus result = StatusOk;
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_MoveResizeWindow(%d,%d,%d,%d,%d)\n",this,
	   handle, (int) xcenter, (int) ycenter,
	   (int) width, (int) height);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to resize an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::MoveResizeWindow(_etkServer, handle,
				       (int) xcenter, (int) ycenter,
				       (int) width, (int) height);
    if (result.IsComplete())
    {
	info->x = xcenter;
	info->y = ycenter;
	info->width = width;
	info->height = height;
    }
    return result; 
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_FreeWindow()
//
// Frees a single embedded Tk window
//
//
DevStatus GLWindowRep::ETk_FreeWindow(int handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
  printf("GLWindowRep(0x%p)::ETk_FreeWindow(%d)\n",this, handle);
#endif
    
  ETkInfo *info;
  DevStatus result = StatusOk;
  int iter, current;
  iter = _etkWindows.InitIterator(false);
  while (_etkWindows.More(iter)) {
      info = _etkWindows.Next(iter);
      if (info == NULL) {
	  break;
      }
      if (info->handle == handle) {
	_etkWindows.DeleteCurrent(iter);
	delete info;
	_etkWindows.DoneIterator(iter);
	result += ETkIfc::SendCommand(_etkServer, "free",
				      0, NULL, handle);
	return result;
      }
  }
  _etkWindows.DoneIterator(iter);
  reportError("Attempt to delete an invalid ETk window", devNoSyserr);
  result = StatusFailed;
  return result;
    
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_MapWindow()
//
// Maps a previously created embedded Tk window
//
DevStatus GLWindowRep::ETk_MapWindow(int handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_MapWindow(%d)\n",this, handle);
#endif
    DevStatus result = StatusOk;
    ETkInfo *info;
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to map an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::SendCommand(_etkServer, "map",
				  0, NULL, handle);
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_UnmapWindow()
//
// Unmaps a previously created embedded Tk window
//
DevStatus GLWindowRep::ETk_UnmapWindow(int handle)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_UnmapWindow(%d)\n",this, handle);
#endif
    DevStatus result = StatusOk;
    ETkInfo *info;
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to unmap an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    result += ETkIfc::SendCommand(_etkServer, "unmap",
				  0, NULL, handle);
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_EvalCmd()
//
// Sends a command to the ETk window for evaluation inside the Tcl
// interpreter. If successful, result is stored in static area pointed
// to by returnValue.
//
DevStatus GLWindowRep::ETk_EvalCmd(int handle,
				   int argc, char **argv,
				   char *&returnValue)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_EvalCmd(%d)\n",this, handle);
#endif

    DevStatus result = StatusOk;
    ETkInfo *info;
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to operate on an invalid ETk window",
		    devNoSyserr);
	return StatusFailed;
    }

    result += ETkIfc::EvalCommand(_etkServer, handle,
				  argc, (const char **) argv,
				  returnValue);
    
    return result;

}

//------------------------------------------------------------------------
// GLWindowRep::ETk_FreeWindows()
//
// Free all embedded Tk windows associated with this window.
//
DevStatus GLWindowRep::ETk_FreeWindows() {
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_FreeWindows()\n",this);
#endif
    DevStatus result = StatusOk;
    int iter, handle;
    
    ETkInfo *info;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter))
    {
	info = _etkWindows.Next(iter);
	if (info == NULL)
	{
	    reportError("NULL ETkInfo pointer found in ETkWindowList",
			devNoSyserr);
	    result = StatusFailed;
	    break;
	}
	_etkWindows.DeleteCurrent(iter);
	result += ETkIfc::SendCommand(_etkServer, "free",
				      0, NULL, info->handle);
	delete info;
    }
    _etkWindows.DoneIterator(iter);
    
    return result;

}

//------------------------------------------------------------------------
// GLWindowRep::ETk_Mark()
//
// Set the in_use flag for a single embedded Tk window
//
//
DevStatus GLWindowRep::ETk_Mark(int handle, bool in_use)
{
    ETkInfo *info;
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_Mark()\n",this);
#endif
    if ((info = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to mark an invalid ETk window", devNoSyserr);
	return StatusFailed;
    }
    info->in_use = in_use;
    return StatusOk;
}

//------------------------------------------------------------------------
// GLWindowRep::ETk_MarkAll()
//
// Set the in_use flag for all embedded Tk windows
//
DevStatus GLWindowRep::ETk_MarkAll(bool in_use)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_MarkAll()\n",this);
#endif
    DevStatus result = StatusOk;
    int iter;
    
    ETkInfo *info;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter)) {
	info = _etkWindows.Next(iter);
	if (info == NULL) {
	    reportError("NULL ETkInfo pointer found in ETkWindowList",
			devNoSyserr);
	    continue;
	}
	info->in_use = in_use;
    }
    _etkWindows.DoneIterator(iter);
    
    return result;
}

//------------------------------------------------------------------------
// GLWindowRep::ETk_Cleanup()
//
// Free all embedded Tk windows that have in_use == false
//
DevStatus GLWindowRep::ETk_Cleanup()
{
  #if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_Cleanup()\n",this);
#endif
    DevStatus result = StatusOk;
    int iter, handle;
    
    ETkInfo *info;
    iter = _etkWindows.InitIterator(false);
    while (_etkWindows.More(iter)) {
	info = _etkWindows.Next(iter);
	if (info == NULL) {
	    reportError("NULL ETkInfo pointer found in ETkWindowList",
			devNoSyserr);
	    result = StatusFailed;
	    break;
	}
	if (info->in_use == false) {
	    _etkWindows.DeleteCurrent(iter);
	    result += ETkIfc::SendCommand(_etkServer, "free",
					  0, NULL, info->handle);
	    delete info;
	}
    }
    _etkWindows.DoneIterator(iter);
    
    return result;
}

//------------------------------------------------------------------------
//
// GLWindowRep::ETk_GetInfo()
//
// Get info (coordinates, name of Tcl script) about an embedded Tk window
//
DevStatus GLWindowRep::ETk_GetInfo(int handle, ETkInfo &info)
{
#if defined(DEBUG) || defined(DEBUG_ETK)
    printf("GLWindowRep(0x%p)::ETk_GetInfo(%d)\n",this, handle);
#endif
    
    DevStatus result = StatusOk;
    ETkInfo *pInfo;
    if ((pInfo = _etkSearch(&_etkWindows, handle)) == NULL)
    {
	reportError("Attempt to retrieve info for an invalid ETk window",
		    devNoSyserr);
	return StatusFailed;
    }
    info = *pInfo;
    return result;
}

//
// END ETk interface
// -----------------------------------------------------------------------

/* drawing primitives */
/* Return TRUE if window is scrollable */
Boolean GLWindowRep::Scrollable()
{
    /* window is crollable if it has a backing store or
     if it is not obscured */
  /*
     printf("GLWindowRep(0x%p)::Scrollable: 0x%p backingstore: %d, unobscured %d\n",this,
     this, _backingStore, _unobscured);
     */
  if (_backingStore || _unobscured)
    return true;
  else return false;
}

/* Scroll a region in the window */
void GLWindowRep::Scroll(Coord x, Coord y, Coord width, Coord height,
			 Coord dstX, Coord dstY)
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::Scroll(%.2f,%.2f,%.2f,%.2f,%.2f,%.2f)\n",this,
	 x, y, width, height, dstX, dstY);
#endif
}
/* Scroll absolute */
void GLWindowRep::ScrollAbsolute(int x, int y, unsigned width,
				 unsigned height, int dstX, int dstY)
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::ScrollAbsolute(x:%d,y:%d,w:%d,h:%d,dx:%d,dy:%d)\n",this,
	 x, y, width, height, dstX, dstY);
#endif
}


void GLWindowRep::SetPattern(Pattern p)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::SetPatter()\n",this);
#endif
}

void GLWindowRep::SetLineWidth(int w)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::SetLineWidth()\n",this);
#endif
  WindowRep::SetLineWidth(w);
  GLCHECKERROR();
  MAKECURRENT();
  if (w>1)
  	glLineWidth(GLfloat(w));
  else
  	glLineWidth(GLfloat(1.0));
  GLCHECKERROR();
}
#ifdef LIBCS
void GLWindowRep::SetDashes(int dashCount, int dashes[], int startOffset);
#endif

void GLWindowRep::FillRect(Coord xlow, Coord ylow, Coord width,
			   Coord height)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::FillRect: x %.2f, y %.2f, width %.2f, height %.2f\n",this,
         xlow, ylow, width, height);
#endif
#ifdef GRAPHICS
  MAKECURRENT();
  bool filled = (GetPattern() != -Pattern1);
  if (filled) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GLCHECKERROR();
    GLBEGIN(GL_POLYGON);
    glVertex2f(xlow, ylow);
    glVertex2f(xlow+width, ylow);
    glVertex2f(xlow+width, ylow+height);
    glVertex2f(xlow, ylow+height);
    GLEND();
  }
  if( GetLineWidth() >= 0 || !filled) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLCHECKERROR();
    GLBEGIN(GL_POLYGON);
    glVertex2f(xlow, ylow);
    glVertex2f(xlow+width, ylow);
    glVertex2f(xlow+width, ylow+height);
    glVertex2f(xlow, ylow+height);
    GLEND();
  }
#endif
}

void GLWindowRep::FillRectAlign(Coord xlow, Coord ylow, Coord width,
				Coord height,
				SymbolAlignment alignment = AlignSouthWest,
				Coord orientation = 0.0)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::FillRectAlign: x %.2f, y %.2f, width %.2f, height %.2f\n",this,
         xlow, ylow, width, height);
#endif

#ifdef GRAPHICS

  GLCHECKERROR();
  MAKECURRENT(); // draw on my graphical context
  float point_x[4], point_y[4];
  
  CalculateLocation(xlow, ylow, width, height, alignment,
		    orientation, point_x, point_y);
  
  bool filled=(GetPattern() != -Pattern1);
  if (filled) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GLBEGIN(GL_POLYGON);
    glVertex2f(point_x[0], point_y[0]);
    glVertex2f(point_x[1], point_y[1]);
    glVertex2f(point_x[2], point_y[2]);
    glVertex2f(point_x[3], point_y[3]);
    GLEND();
  }
  
  if( GetLineWidth() >= 0 || !filled) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GLBEGIN(GL_POLYGON);
      glVertex2f(point_x[0], point_y[0]);
      glVertex2f(point_x[1], point_y[1]);
      glVertex2f(point_x[2], point_y[2]);
      glVertex2f(point_x[3], point_y[3]);
    GLEND();
  }
  GLCHECKERROR();
#endif
}


/* Fill rectangles, variable width/height */
void GLWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord *width, 
				Coord *height, int num,
				SymbolAlignment alignment = AlignSouthWest,
				Coord orientation = 0.0)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::FillRectArray: %d points\n",this, num);

  printf("Transformation matrix:\n");
  PrintTransform();
  printf("\n");

#if MAXPIXELDUMP > 0
  {
    printf("\nBefore transformation:\n\n");
    int k;
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 6 == 0)
        printf("\n");
      printf("(%.2f,%.2f,%.2f,%.2f)", xlow[k], ylow[k], width[k], height[k]);
    }
    printf("\n");
  }
#endif
#endif
#ifdef GRAPHICS
  GLCHECKERROR();
  for (int i=0; i<num; i++) {
	FillRectAlign(xlow[i], ylow[i], width[i], height[i],
			alignment, orientation);
  }
#endif
}

/* Fill rectangles, same width/height */
void GLWindowRep::FillRectArray(Coord *xlow, Coord *ylow, Coord width,
				Coord height, int num,
				SymbolAlignment alignment = AlignSouthWest,
				Coord orientation = 0.0)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::FillRectArray: %d points, width %.2f, height %.2f\n",this,
         num, width, height);

#if MAXPIXELDUMP > 0
  {
    printf("\nBefore transformation:\n\n");
    int k;
    for(k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
      if ((k + 1) % 10 == 0)
        printf("\n");
      printf("(%.2f,%.2f)", xlow[k], ylow[k]);
    }
    printf("\n");
  }
#endif
#endif

#ifdef GRAPHICS
  GLCHECKERROR();
  for (int i=0; i<num; i++) {
	FillRectAlign(xlow[i], ylow[i], width, height,
			alignment, orientation);
  }
#endif

}

void GLWindowRep::DrawPixel(Coord x, Coord y)
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::DrawPixel: %.2f %.2f\n",this, x, y);
#endif
 /* DrawPixel not supported */
}

void GLWindowRep::DrawPixelArray(Coord *x, Coord *y, int num, int width)
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::DrawPixelArray: %d points, width = %d\n",this, num, width);

  printf("Transformation matrix:\n");
  PrintTransform();
  printf("\n");

#if MAXPIXELDUMP > 0
  printf("\nBefore transformation:\n\n");
  for(int k = 0; k < (num > MAXPIXELDUMP ? MAXPIXELDUMP : num); k++) {
    if ((k + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", x[k], y[k]);
  }
  printf("\n");
#endif
#endif
 /* DrawPixelArray not supported */
}

/* Fill rectangle. All coordinates are in pixels. x and y are
   at the center of the rectangle */
void GLWindowRep::FillPixelRect(Coord x, Coord y, Coord width, Coord height,
				Coord minWidth = 1.0, Coord minHeight = 1.0,
				SymbolAlignment alignment = AlignSouthWest,
				Coord orientation = 0.0)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::FillPixelRect: x %.2f, y %.2f, width %.2f, height %.2f\n",this,
         x, y, width, height);
  printf("  alignment: %d, orientation: %f\n", (int) alignment, orientation);
#endif
#ifdef GRAPHICS
  if (_dispGraphics) {
	width=MAX(width, minWidth);
	height=MAX(height, minHeight);
        FillRectAlign(x,y,width,height,alignment, orientation);
  }
#endif
}

void GLWindowRep::FillPoly(Point *points, int n)
{
  GLCHECKERROR();
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::FillPoly %d points\n",this, n);
#endif

#ifdef GRAPHICS
  MAKECURRENT();
  if (_dispGraphics) {
      // special-case the no-fill rectangle
      bool fill = (GetPattern() != -Pattern1);
      if( fill ) {

  GLCHECKERROR();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  GLCHECKERROR();
#if defined(DEBUG)
	PrintTransform();
#endif

  	GLCHECKERROR();
	GLBEGIN(GL_POLYGON);
  	GLCHECKERROR();
	for (int i=0; i<n; i++) {
	  glVertex2d(points[i].x, points[i].y);
  GLCHECKERROR();
#if defined(DEBUG)
	  printf("vertex[%d]=(%.2f,%.2f)\n", i, points[i].x, points[i].y);
#endif
	}
  GLCHECKERROR();
	GLEND();
  GLCHECKERROR();
      }
  GLCHECKERROR();

#if defined(DEBUG)
	PrintTransform();
#endif
  GLCHECKERROR();
      if( GetLineWidth() >= 0 || !fill ) {
  GLCHECKERROR();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  GLCHECKERROR();
	GLBEGIN(GL_POLYGON);
  GLCHECKERROR();
	for (int i=0; i<n; i++) {
	  glVertex2d(points[i].x, points[i].y);
  GLCHECKERROR();
#if defined(DEBUG)
	  printf("vertex[%d]=(%.2f,%.2f)\n", i, points[i].x, points[i].y);
#endif
	}
	GLEND();
  GLCHECKERROR();
      }
  }
#endif
  GLCHECKERROR();
}

void GLWindowRep::FillPixelPoly(Point *, int n)
{
  GLCHECKERROR();
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::FillPixelPoly: %d points\n",this,n);

#if MAXPIXELDUMP > 0
  for(int j = 0; j < (n < MAXPIXELDUMP ? MAXPIXELDUMP : n); j++) {
    if ((j + 1) % 10 == 0)
      printf("\n");
    printf("(%.2f,%.2f)", pts[j].x, pts[j].y);
  }
  printf("\n");
#endif
#endif
  GLCHECKERROR();
}

/* Draw an arc.  Angles are in radians. */
void GLWindowRep::Arc(Coord xCenter, Coord yCenter, Coord horizDiam,
		      Coord vertDiam, Coord startAngle, Coord endAngle)
{
  GLCHECKERROR();
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::Arc %.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",this,
	 xCenter, yCenter, horizDiam, vertDiam, startAngle, endAngle);
#endif
  GLCHECKERROR();
}

void GLWindowRep::Line(Coord x1, Coord y1, Coord x2, Coord y2, Coord width)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::Line %.2f,%.2f,%.2f,%.2f\n",this, x1, y1, x2, y2);
#endif
  GLCHECKERROR();
  MAKECURRENT();
  GLCHECKERROR();
  GLBEGIN(GL_LINES);
  GLCHECKERROR();
  glVertex2d(x1,y1);
  GLCHECKERROR();
  glVertex2d(x2,y2);
  glEnd();
  GLCHECKERROR();
}

void GLWindowRep::AbsoluteLine(int x1, int y1, int x2, int y2, int width)
{
  GLCHECKERROR();
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::AbsoluteLine %d,%d,%d,%d\n",this, x1, y1, x2, y2);
#endif
  MAKECURRENT();
  GLCHECKERROR();
  GLMATRIXMODE(GL_MODELVIEW);
  GLCHECKERROR();
  glPushMatrix();
  GLCHECKERROR();
  glLoadIdentity();
  GLCHECKERROR();
  GLBEGIN(GL_LINES);
  GLCHECKERROR();
  glVertex2i(x1,y1);
  GLCHECKERROR();
  glVertex2i(x2,y2);
  GLCHECKERROR();
  GLEND();
  GLCHECKERROR();
  glPopMatrix();
  GLCHECKERROR();
}

void GLWindowRep::ScaledText(char *text, Coord x, Coord y, Coord width,
			     Coord height, SymbolAlignment alignment = AlignCenter,
			     Boolean skipLeadingSpaces = false,
			     Coord orientation = 0.0)
{
  GLCHECKERROR();
#if defined (DEBUG)
  printf("GLWindowRep(0x%p)::ScaledText: <%s> at %.2f, %.2f, %.2f, %.2f, orientation %.2f\n",this,
	 text, x, y, width, height, orientation);
#endif
  DrawText(true, text, x, y, width, height, alignment,
    skipLeadingSpaces, orientation);
  GLCHECKERROR();
}

void GLWindowRep::AbsoluteText(char *text, Coord x, Coord y, Coord width, 
			       Coord height,
			       SymbolAlignment alignment = AlignCenter,
			       Boolean skipLeadingSpaces = false,
			       Coord orientation = 0.0)
{
  GLCHECKERROR();
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::AbsoluteText: <%s> at %.2f,%.2f,%.2f,%.2f, orientation %.2f\n",this,
	 text, x, y, width, height, orientation);
#endif
  DrawText(false, text, x, y, width, height, alignment,
    skipLeadingSpaces, orientation);
  GLCHECKERROR();
}

/* Set XOR or normal drawing mode on */
void GLWindowRep::SetXorMode()
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::SetXorMode\n",this);
#endif
}
void GLWindowRep::SetCopyMode()
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::SetCopyMode\n",this);
#endif
}

void GLWindowRep::SetOrMode()
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::SetOrMode\n",this);
#endif
}

/* Set font or return to normal */
void GLWindowRep::SetFont(char *family, char *weight, char *slant,
			  char *width, float pointSize)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::SetFont(%s,%f)\n", this, family, pointSize);
#endif
  GLCHECKERROR();
  GetDisplay()->SetFont(family, weight, slant, width, pointSize);
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
  if (!_specialfontstruct || _specialfontstruct->fid != fontStruct->fid) {
    if (_specialfontstruct) // need to free the display list of fonts
      glDeleteLists(_specialfont+32,96);
    // now create display lists of fonts
    MAKECURRENT();
    _specialfontstruct=fontStruct;
    _specialfont=glGenLists(256);
    glXUseXFont(_specialfontstruct->fid, 32, 96, _specialfont+32);
  }
  if (_specialfont!=_currentfont) {
    _currentfont=_specialfont;
    _currentfontstruct=_specialfontstruct;
    glListBase(_specialfont);
  }
  GLCHECKERROR();
}

void GLWindowRep::SetNormalFont()
{
  GLCHECKERROR();
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::SetNormalFont()\n", this);
#endif
  GLCHECKERROR();

  XFontStruct *fontStruct = GetDisplay()->GetNormalFontStruct();
  if (!_normalfontstruct || _normalfontstruct->fid != fontStruct->fid) {
    if (_normalfontstruct) // need to free the display list of fonts
      glDeleteLists(_normalfont,96);
    // now create display lists of fonts
    MAKECURRENT();
    _normalfontstruct=fontStruct;
    _normalfont=glGenLists(256);
    glXUseXFont(_normalfontstruct->fid, 32, 96, _normalfont+32);
  }
  if (_normalfont!=_currentfont) {
    _currentfont=_normalfont;
    _currentfontstruct=_normalfontstruct;
    glListBase(_normalfont);
  }
}

/* Draw rubberbanding rectangle */
void GLWindowRep::DrawRubberband(int x1, int y1, int x2, int y2)
{
  DOASSERT(_win, "Cannot draw rubberband in pixmap");

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  GLCHECKERROR();
  GLBEGIN(GL_POLYGON);
  glVertex2f(x1, _height-y1);
  glVertex2f(x1, _height-y2);
  glVertex2f(x2, _height-y2);
  glVertex2f(x2, _height-y1);
  GLEND();
  this->Flush();
  printf("Drawn rubber band\n");
}

/* Get window rep dimensions */
void GLWindowRep::Dimensions(unsigned int &width, unsigned int &height)
{
  width = _width;
  height = _height;
}

void GLWindowRep::PrintDimensions()
{
  cout << "Width = " << _width << endl;
  cout << "Height = " << _height << endl;
}

/* Get window rep origin */
void GLWindowRep::Origin(int &x, int &y)
{
  x = _x;
  y = _y;
}

/* Get absolute window rep origin from upper left corner
   of the screen */
void GLWindowRep::AbsoluteOrigin(int &x, int &y)
{
  DOASSERT(_win, "Cannot get screen position of pixmap");

  /* Find the offset from root window */
  x = 0;
  y = 0;
  
  Window current, root;
  Window parent = _win;
  do {
    current = parent;
    
    /* add distace to parent */
    Window winRoot;
    int winX, winY;
    unsigned winW, winH, winBorder, winDepth;
    XGetGeometry(_display, current, &winRoot, &winX, &winY,
		 &winW, &winH, &winBorder, &winDepth);
    x += winX;
    y += winY;
    
    Window *children;
    unsigned int nchildren;
    if (!XQueryTree(_display, current, &root, &parent,
		    &children, &nchildren)) {
      DOASSERT(0, "XQueryTree failed");
    }
    if (children != NULL)
      XFree(children);
  } while (parent != root);
}



// ---------------------------------------------------------- 

/* Return contents of window as a pixmap */
DevisePixmap *GLWindowRep::GetPixmap()
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::GetPixmap()\n",this);
#endif
  return 0;
}

/* Display pixmap in window */
void GLWindowRep::DisplayPixmap(DevisePixmap *pixmap)
{
#ifdef DEBUG
  printf("GLWindowRep(0x%p)::DisplayPixmap %d bytes\n",this, pixmap->compressedBytes);
#endif
}
/* Free pixmap from memory */
void GLWindowRep::FreePixmap(DevisePixmap *pixmap)
{
  free(pixmap->data);
  delete pixmap;
}


#ifdef TK_WINDOW_old
/* Tk window size changed -- update size of this window */
void GLWindowRep::TkWindowSizeChanged()
{
  DOASSERT(!isInTkWindow(), "Invalid Tk window");
  EmbedInTkWindow((GLWindowRep *)parent, name, min_width, min_height);
}
#endif

/* called by GLDisplay to create new X window */
GLWindowRep::GLWindowRep(Display *display, Window window, GLDisplay *DVDisp, 
			 GLWindowRep *parent, Boolean backingStore = false):
  WindowRep(DVDisp),_display(0),_win(0),_gc(0),
  _specialfontstruct(0),_specialfont(0),
  _normalfontstruct(0),_normalfont(0),_currentfont(0)
{
#if defined(DEBUG)
  printf("GLWindowRep::GLWindowRep(this = %p, parent = %p,
    window = 0x%lx)\n", this, parent, window);
#endif
  _display = display;
  _win = window;
  _pixmap = 0;
  _parent = parent;
  if (_parent)
    _parent->_children.Append(this);
  _backingStore = false;
  _lineStyle = LineSolid;
  _daliServer = NULL;
  _etkServer = NULL;
  Init();
} 

/* called by GLDisplay to create new X pixmap */
GLWindowRep::GLWindowRep(Display *display, Pixmap pixmap, GLDisplay *DVDisp, 
			 GLWindowRep *parent, int x, int y):
  WindowRep(DVDisp),_display(0),_win(0),_gc(0)
{
#if defined(DEBUG)
  printf("GLWindowRep::GLWindowRep(this = %p, parent = %p, "
		  "pixmap = 0x%lx)\n", this, parent, pixmap);
#endif
  _display = display;
  _win = 0;
  _pixmap = pixmap;
  _parent = parent;
  if (_parent)
    _parent->_children.Append(this);
  _backingStore = false;

  _x = x;
  _y = y;

  Init();

}

/* called by constructors to initialize object */
void GLWindowRep::Init()
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::Init()\n",this);
#endif

  XVisualInfo *vi;

#if defined(LIBCS)
  _dispGraphics = true;
#else
  _dispGraphics = Init::DispGraphics();
#endif
  _compress = new SimpleCompress();

  UpdateWinDimensions();
  _unobscured = false;
  // use double buffer. Use color index mode instead of RGB mode
/*  int configuration[] = {GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE, 12,
			 GLX_RED_SIZE, 1, GLX_BLUE_SIZE, 1,
			 GLX_GREEN_SIZE, 1, None};*/
  int configuration[] = {GLX_DEPTH_SIZE, 12,
			 GLX_RED_SIZE, 1, GLX_BLUE_SIZE, 1,
			 GLX_GREEN_SIZE, 1, None};
  int configuration2[] = {None};

  vi = glXChooseVisual(_display, DefaultScreen(_display), configuration);
  if (vi == NULL) {
    vi = glXChooseVisual(_display, DefaultScreen(_display), configuration2);
    DOASSERT(vi != NULL, "no appropriate RGB visual with depth buffer");
    double_buffer=GL_FALSE;
  }
  else
//    double_buffer=GL_TRUE;
    double_buffer=GL_FALSE; // changed temperarily

  GLCHECKERROR();
  _gc = glXCreateContext(_display, vi,
			 /* No sharing of display lists */ NULL,
			 /* Direct rendering if possible */ True);
  GLCHECKERROR();
  SetCopyMode();
  GLCHECKERROR();
  SetLineWidth(0);
  GLCHECKERROR();
  WindowRep::SetPattern(Pattern0);
  GLCHECKERROR();
  
  SetNormalFont();
  GLCHECKERROR();
  // font pixmap creating elimated.
  MAKECURRENT();
  glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
  GLMATRIXMODE(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLsizei)_width, 0.0, (GLsizei)_height);
  GLMATRIXMODE(GL_MODELVIEW);
  glLoadIdentity();
#if defined(DEBUG)
  PrintTransform();
#endif
}

/* destructor */
GLWindowRep::~GLWindowRep()
{
#if defined(DEBUG)
  printf("GLWindowRep::~GLWindowRep(this = %p)\n", this);
#endif
  glXDestroyContext(_display, _gc);
  
  /* _win or _pixmap is destroyed by GLDisplay */
  DOASSERT(_win == 0 && _pixmap == 0, "X window or pixmap not freed");

  // This should have already been done by GLDisplay::DestroyWindowRep(),
  // but do it again here just in case...  If it's already been done,
  // this won't actually do anything.
  (void) DaliFreeImages();
  delete [] _daliServer;

  (void) ETk_FreeWindows();
  delete [] _etkServer;
  
  if (_parent) {
    if (!_parent->_children.Delete(this))
      fprintf(stderr, "Cannot remove child from parent window\n");
  }

  if (_children.Size() > 0)
    reportErrNosys("Child windows should have been destroyed first");
}

//******************************************************************************
// Getters and Setters
//******************************************************************************

void    GLWindowRep::SetForeground(PColorID fgid)
{
        XColorID        xfgid = AP_GetXColorID(fgid);

        WindowRep::SetForeground(fgid);

#ifdef GRAPHICS
        if (_dispGraphics)
                glIndexi(xfgid);
#endif
}

void    GLWindowRep::SetBackground(PColorID bgid)
{
        XColorID        xbgid = AP_GetXColorID(bgid);

        WindowRep::SetBackground(bgid);

#ifdef GRAPHICS
        if (_dispGraphics) {
	  	glClearIndex(xbgid);
  		glClear(GL_COLOR_BUFFER_BIT);
	}
#endif
}

//******************************************************************************
// Utility Functions
//******************************************************************************

void    GLWindowRep::SetWindowBackground(PColorID bgid)
{
#ifdef GRAPHICS
        if (_dispGraphics)
                XSetWindowBackground(_display, _win, AP_GetXColorID(bgid));
#endif
}

void    GLWindowRep::ClearPixmap(void)
{
#ifdef GRAPHICS
        PColorID        savePColorID = GetForeground();

        SetForeground(GetBackground());
        //XFillRectangle(_display, _pixmap, _gc, 0, 0, _width, _height);
        SetForeground(savePColorID);
#endif
}

static int check_expose(Display *, XEvent *e, char *arg)
{
  if (e->xany.type == Expose && e->xany.window == *((Window *)arg))
    return 1;
  return 0;
}

void GLWindowRep::HandleEvent(XEvent &event)
{
  DOASSERT(_win, "Cannot handle events for pixmap");

  XEvent ev;
  Atom protocol;
  int d_modifier;
  int ks;
  int d_key;
  int count;
  char buf[40];
  KeySym keysym;
  XComposeStatus compose;

  switch(event.xany.type) {

  case KeyPress:

    d_modifier = 0;
#if 1
    if( event.xkey.state & ControlMask ) {
	d_modifier |= DeviseKey::CONTROL;
	event.xkey.state &= ~ControlMask; // avoid conversion by XLookupString
    }
    if( event.xkey.state & ShiftMask ) {
	d_modifier |= DeviseKey::SHIFT;
    }
    if( event.xkey.state & AltMask ) {
	d_modifier |= DeviseKey::ALT;
    }
#endif

    count = XLookupString((XKeyEvent *)&event, buf, sizeof(buf),
                          &keysym, &compose);

    ks = int(keysym);
    d_key = 0;
    if( devise_keymap.lookup(ks, d_key) ) {
	// not found in xkey->devise_key translation map
	if( count == 1 ) {		// regular key
	    d_key = buf[0];
            if( d_modifier & (DeviseKey::CONTROL|DeviseKey::ALT) ) {
              // always report as upper case if control or alt used
              d_key = toupper(d_key);
            } else {
              // don''t report shift for normal keys, eg, 'C', not SHIFT-C
              d_modifier &= ~DeviseKey::SHIFT;
            }
	} else {
	    d_key = 0;		// ignore keypress
	}
    }

#if defined(DEBUG)
    printf("KeyPress: KeySym:0x%x, state: 0x%x, d_key: %c 0x%x, d_mod: 0x%x\n",
	   int(keysym), event.xkey.state,
	   isgraph(char(d_key)) ? d_key : '~', d_key, d_modifier);
#endif

    if (d_key) {
	d_key |= d_modifier;
	WindowRep::HandleKey(d_key, event.xkey.x, event.xkey.y);
    }

    break;

#ifdef RAWMOUSEEVENTS
  case ButtonPress:
  case ButtonRelease:
  case MotionNotify:
    WindowRep::HandleButton(event.xbutton.x, event.xbutton.y,
			    event.xbutton.button, event.xbutton.state,
			    event.xbutton.type);
    break;
#else
  case ButtonPress:
    int buttonXlow, buttonYlow, buttonXhigh, buttonYhigh;
    if (event.xbutton.button == 2) {
      /* handle popup */
      DoPopup(event.xbutton.x, event.xbutton.y, event.xbutton.button);
    } else if (event.xbutton.button <= 3) {
      DoButtonPress(event.xbutton.x, event.xbutton.y,
		    buttonXlow, buttonYlow, buttonXhigh, buttonYhigh,
		    event.xbutton.button);
      
      /*
	 if (buttonXhigh > buttonXlow && buttonYhigh > buttonYlow) {
      */
      WindowRep::HandleButtonPress(buttonXlow, buttonYlow,
				   buttonXhigh, buttonYhigh,
				   event.xbutton.button);
    }
    break;

  case ButtonRelease:
  case MotionNotify:
    break;
#endif

  case Expose:
    Coord minX, minY, maxX, maxY;
    minX = (Coord)event.xexpose.x;
    minY = (Coord)event.xexpose.y;
    maxX = minX + (Coord)event.xexpose.width - 1;
    maxY = minY + (Coord)event.xexpose.height - 1;
#ifdef DEBUG
    printf("GLWindowRep(0x%p) Exposed %d,%d to %d,%d\n", this,
	   (int)minX, (int)minY, (int)maxX, (int)maxY);
#endif
    while (XCheckIfEvent(_display, &ev, check_expose, (char *)&_win)) {
      /* merge next expose event with current one to form
	 a bigger expose region */
#ifdef DEBUG
      printf("Merging expose with other exposure: %d,%d,%d,%d\n",
	     (int)ev.xexpose.x, (int)ev.xexpose.y,
	     (int)(ev.xexpose.x + ev.xexpose.width - 1),
	     (int)(ev.xexpose.y + ev.xexpose.height - 1));
#endif
      Geom::MergeRects(minX, minY, maxX, maxY,
		       (Coord)ev.xexpose.x, (Coord)ev.xexpose.y,
		       (Coord)(ev.xexpose.x + ev.xexpose.width - 1),
		       (Coord)(ev.xexpose.y + ev.xexpose.height - 1));
    }
    WindowRep::HandleExpose((int)minX, (int)minY,
			    (unsigned)(maxX - minX + 1), 
			    (unsigned)(maxY - minY + 1));
    break;

  case ConfigureNotify:
    /* resize event */
    int saveX, saveY; 
    unsigned int saveWidth, saveHeight;
    saveX = _x;
    saveY = _y;
    saveWidth = _width;
    saveHeight = _height;
#ifdef DEBUG
    printf("GLWindowRep(0x%p) ConfigureNotify: %d,%d,%d,%d\n", this,
	   _x, _y, _width, _height);
#endif
    UpdateWinDimensions();
    if (_x != saveX || _y != saveY || _width != saveWidth
	|| _height != saveHeight) {
      /* There is a real change in size */
      WindowRep::HandleResize(_x, _y, _width, _height);
    }
    break;

  case VisibilityNotify:
    if (event.xvisibility.state == VisibilityUnobscured) {
      _unobscured = true;
#if defined( DEBUG)
      printf("GLWindowRep(0x%p) unobscured %d\n", this, _unobscured);
#endif
    }
    else if (event.xvisibility.state == VisibilityPartiallyObscured) {
      _unobscured = false;
#ifdef DEBUG
      printf("GLWindowRep(0x%p) partially obscured\n",this);
#endif
    }
    else {
      _unobscured = false;
#ifdef DEBUG
      printf("GLWindowRep(0x%p) obscured\n",this);
#endif
    }
    break;

  case MapNotify:
#ifdef DEBUG
    printf("XWin 0x%lx mapped\n", event.xmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(true);
    break;

  case UnmapNotify:
#ifdef DEBUG
    printf("XWin 0x%lx unmapped\n", event.xunmap.window);
#endif
    WindowRep::HandleWindowMappedInfo(false);
    break;

  case ReparentNotify:
#ifdef DEBUG
    printf("XWin 0x%lx reparented\n", event.xunmap.window);
#endif
    break;

  case DestroyNotify:
#ifdef DEBUG
    printf("XWin 0x%lx destroyed\n", event.xdestroywindow.window);
#endif
    break;

  case ClientMessage:
    protocol = (Atom)event.xclient.data.l[0];
#ifdef DEBUG
    printf("XWin 0x%lx receives client message %ld\n",
           event.xclient.window, protocol);
#endif
    if (protocol == XInternAtom(_display, "WM_DELETE_WINDOW", False)) {
#ifdef DEBUG
      printf("Client message is WM_DELETE_WINDOW\n");
#endif
      WindowRep::HandleWindowDestroy();
    } else {
      printf("Unrecognized client message %ld for window 0x%lx\n",
             protocol, event.xclient.window);
    }
    break;

  case GraphicsExpose:
#ifdef DEBUG
    printf("XWin 0x%lx receives graphics expose event (%d more)\n",
           event.xgraphicsexpose.drawable, event.xgraphicsexpose.count);
    printf("    at %d,%d size %dx%d\n", event.xgraphicsexpose.x,
           event.xgraphicsexpose.y,  event.xgraphicsexpose.width,
           event.xgraphicsexpose.height);
#endif
    break;

  case NoExpose:
#ifdef DEBUG
    printf("XWin 0x%lx receives no-expose event\n",
           event.xnoexpose.drawable);
#endif
    break;

  default:
    printf("Unexpected event %d for window 0x%lx\n",
           event.xany.type, event.xany.window);
    break;
  }
}

/* Assign window to a new parent. */
#if defined(TK_WINDOW_OLD)
/* Build Tk window around this window */
void GLWindowRep::EmbedInTkWindow(GLWindowRep *parent,
				  char *name,
				  unsigned int min_width,
				  unsigned int min_height)
{
  DOASSERT(_win, "Cannot embed pixmap in Tk window");

  extern Tcl_Interp *ControlPanelTclInterp;
  extern Tk_Window ControlPanelMainWindow;

  // get location and size of window

  int x, y;
  unsigned int w, h;
  Origin(x, y);
  Dimensions(w, h);

  // figure out the correct margins for this type of window

  // default: root data display window

  _leftMargin   = TkRootLeftMargin;
  _rightMargin  = TkRootRightMargin;
  _topMargin    = TkRootTopMargin;
  _bottomMargin = TkRootBottomMargin;

  // for views: left and bottom margin

  if (parent) {
    _leftMargin   = TkViewLeftMargin;
    _rightMargin  = TkViewRightMargin;
    _topMargin    = TkViewTopMargin;
    _bottomMargin = TkViewBottomMargin;
  }

  static int tkWinCount = 1;
  char cmd[256];
  sprintf(_tkPathName, ".devisewin%d", tkWinCount++);
  sprintf(cmd, "CreateTkDataWindow %s {%s} %d %d %u %u %u %u %u %u %u %u",
	  _tkPathName, name, x, y, w, h, _leftMargin,
	  _rightMargin, _topMargin, _bottomMargin,
	  min_width, min_height);
#ifdef DEBUG
  printf("Executing: %s\n", cmd);
#endif
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot create Tk window");

  _tkWindow = Tk_NameToWindow(ControlPanelTclInterp,
			      _tkPathName,
			      ControlPanelMainWindow);
  DOASSERT(_tkWindow, "Cannot get name of Tk window");

#ifdef DEBUG
  printf("Created %s, id 0x%p, X id 0x%p, at %d,%d size %u,%u\n",
	 _tkPathName, _tkWindow, Tk_WindowId(_tkWindow),
	 x, y, w, h);
#endif

  unsigned long mask = StructureNotifyMask;
  Tk_CreateEventHandler(_tkWindow, mask, HandleTkEvents, this);

  // first make this window a child of the new Tk window

#ifdef DEBUG
  printf("GLWindowRep(0x%p)::Reparenting 0x%p to 0x%p at %d,%d\n",this,
	 _win, Tk_WindowId(_tkWindow), _leftMargin, _topMargin);
#endif
  XReparentWindow(_display, _win, Tk_WindowId(_tkWindow),
		  _leftMargin, _topMargin);

  // make this window smaller so that margins have space inside Tk window

  w -= _leftMargin + _rightMargin;
  h -= _topMargin + _bottomMargin;
  MoveResize(_leftMargin, _topMargin, w, h);

  // then optionally make the Tk window a child of this window's parent
  // i.e. the Tk window gets inserted between this window and its parent
  
  if (parent) {
#ifdef DEBUG
    printf("GLWindowRep(0x%p)::Reparenting 0x%p to 0x%p at %d,%d\n",this,
	   Tk_WindowId(_tkWindow), parent->_win, x, y);
#endif
    XReparentWindow(_display, Tk_WindowId(_tkWindow), parent->_win, x, y);
  }
}

/* Detach window from Tk window */
void GLWindowRep::DetachFromTkWindow()
{
  DOASSERT(_win, "Cannot detach pixmap from Tk window");

  extern Tcl_Interp *ControlPanelTclInterp;

#ifdef DEBUG
  printf("ViewWin::Detaching 0x%p from 0x%0x\n", this, _tkWindow);
#endif

  DOASSERT(_tkWindow, "Invalid Tk window");
  unsigned long mask = StructureNotifyMask;
  Tk_DeleteEventHandler(_tkWindow, mask, HandleTkEvents, this);

  // get location and size of window

  int x, y;
  unsigned int w, h;
  Origin(x, y);
  Dimensions(w, h);

  // adjust location and size since margins are disappearing

  x -= _leftMargin;
  w += _leftMargin + _rightMargin;
  y -= _topMargin;
  H += _topMargin + _bottomMargin;
  MoveResize(x, y, w, h);

  XReparentWindow(_display, _win, DefaultRootWindow(_display), x, y);

  // destroy Tk window

  char cmd[256];
  sprintf(cmd, "destroy %s", _tkPathName);
  int status = Tcl_Eval(ControlPanelTclInterp, cmd);
  DOASSERT(status == TCL_OK, "Cannot destroy Tk window");

  _tkPathName[0] = 0;
  _tkWindow = 0;

}

#endif

/* get geometry of root window enclosing this window */
void GLWindowRep::GetRootGeometry(int &x, int &y, unsigned int &w, unsigned int &h)
{
  if (_pixmap) {
    Origin(x, y);
    Dimensions(w, h);
    return;
  }

  Window root = FindTopWindow(_win);
  Window dummy;
  unsigned int border_width, depth;
  XGetGeometry(_display, root, &dummy, &x, &y, &w, &h, &border_width,
               &depth);
#ifdef DEBUG
  printf("Geometry of root 0x%lx of XWin 0x%p is %d,%d,%u,%u\n",
         root, this, x, y, w, h);
#endif
}

/* find top window */
Window GLWindowRep::FindTopWindow(Window win)
{
  Window parent, root;
  Window *children;
  unsigned int nchildren;
  if (!XQueryTree(_display, win, &root, &parent,
                  &children, &nchildren)) {
    DOASSERT(0, "XQueryTree failed");
  }
  if (children)
    XFree(children);

  /* if the root window (display) is the parent of the current 
     window, we've found the top enclosing window enclosing */

  if (parent == root)
    return win;

  return FindTopWindow(parent);
}

/* export window image as GIF */
void GLWindowRep::ExportGIF(FILE *fp, int isView = 0)
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::ExportGIF",this);
#endif
}

/* recursively copy the contents of subpixmaps onto parent pixmap */
void GLWindowRep::CoalescePixmaps(GLWindowRep *root)
{
#if defined(DEBUG)
  printf("GLWindowRep::CoalescePixmaps(%d)\n",(int)root);
#endif
}

void GLWindowRep::UpdateWinDimensions()
{

  Window root;
  unsigned int border_width, depth;

  if (_win) {
    XGetGeometry(_display, _win, &root, &_x, &_y, &_width, &_height,
		 &border_width, &depth);
#if defined(DEBUG)
    printf("GLWindowRep(0x%p)::UpdateWinDimensions():\n",this);
    printf("_x=%d, _y=%d, _width=%u, _height=%u\n", _x, _y, _width, _height);
#endif
    if (_gc) {
      MAKECURRENT();
      glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
      GLMATRIXMODE(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(0.0, (GLsizei)_width, 0.0, (GLsizei)_height);
    }
  }
  else {
    // pixmaps don't have position information so XGetGeometry returns
    // X = 0 and Y = 0... just ignore them
    int dummyX, dummyY;
    XGetGeometry(_display, _pixmap, &root, &dummyX, &dummyY, &_width, &_height,
		 &border_width, &depth);
  }
}

#ifndef RAWMOUSEEVENTS
static long buttonMasks[3] = {
  Button1MotionMask, Button2MotionMask, Button3MotionMask
};

/* Handle button press event. Return the region covered by
   the selection in window coordinates */
void GLWindowRep::DoButtonPress(int x, int y, int &xlow, int &ylow, int &xhigh,
		   int &yhigh, int button)
{

  DOASSERT(_win, "Cannot handle button press in pixmap");
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::DoButtonPress()\n",this);
#endif
  DOASSERT(_win, "Cannot handle button press in pixmap");

  /* grab server */
  XGrabServer(_display);
  int x1,x2;
  x1 = x2 = x;
  int y1,y2;
  y1 = y2 = y;

  /* draw rubberband rectangle */

  MAKECURRENT();
  GLMATRIXMODE(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_INDEX_LOGIC_OP);
  glLogicOp(GL_INVERT);

  DrawRubberband(x1, y1, x2, y2);
  
  Boolean done = false;
  long buttonMask = buttonMasks[button - 1];
  while(!done) {
    XEvent event;
    XWindowEvent(_display, _win, ButtonReleaseMask | buttonMask, &event);
    switch(event.xany.type) {
    case ButtonRelease:
      if (event.xbutton.button == (unsigned int)button) {
	/* final button position */
	DrawRubberband(x1,y1,x2,y2);

	x2 = event.xbutton.x;
	y2 = event.xbutton.y;
	done = true;
      }
      break;
    case MotionNotify:
      DrawRubberband(x1, y1, x2, y2);
    
      /* get rid of all remaining motion events */
      do {
	x2 = event.xmotion.x;
	y2 = event.xmotion.y;
      } while(XCheckWindowEvent(_display,_win, buttonMask, &event));
      
      DrawRubberband(x1, y1, x2, y2);
      break;
    }
  }
  glDisable(GL_INDEX_LOGIC_OP);
  glPopMatrix();

  xlow = MIN(x1, x2);
  ylow = MIN(y1, y2);
  xhigh = MAX(x1, x2);
  yhigh = MAX(y1, y2);
  
  if (xhigh - xlow <= 5 || yhigh - ylow <= 5) {
    xhigh = xlow;
    yhigh = ylow;
  }
  
  /* ungrab server */
  XUngrabServer(_display);
  XSync(_display,false);
}
#endif

void GLWindowRep::DrawText(Boolean scaled, char *text, Coord x, Coord y,
			   Coord width, Coord height,
			   SymbolAlignment alignment = AlignCenter,
			   Boolean skipLeadingSpaces = false,
			   Coord orientation=0.0)
{
  GLCHECKERROR();
  if (skipLeadingSpaces) {
    /* skip leading spaces before drawing text */
    while (*text == ' ')
      text++;
  }

  MAKECURRENT();
  DOASSERT(_currentfontstruct, "GLWindowRep::DrawText: called before a font is set");
  int textwidth = XTextWidth(_currentfontstruct, text, strlen(text));
  int textheight = _currentfontstruct->ascent + _currentfontstruct->descent;

  //  glBitmap(0, 0, 0, 0,
  //	   x / 2 - width / 2,
  //	   y / 2 - (_current_fontstruct->ascent +
  //	    _current_fontstruct->descent) / 2, 0);
  GLdouble xloc, yloc;

  switch(alignment) {
  case AlignNorthWest:
  case AlignWest:
  case AlignSouthWest:
    xloc=x;
    break;
  case AlignNorthEast:
  case AlignEast:
  case AlignSouthEast:
    xloc=x+width-textwidth;
    break;
  case AlignNorth:
  case AlignSouth:
    xloc=x+(width-textwidth)/2;
    break;
  case AlignCenter:
    xloc=x+(width-textwidth)/2;
    break;
  default:
    DOASSERT(FALSE, "Unknow orientation passed to GLWindowRep::DrawText");
  }
  switch(alignment) {
  case AlignNorth:
  case AlignNorthWest:
  case AlignNorthEast:
    yloc=y+height;
    break;
  case AlignSouth:
  case AlignSouthWest:
  case AlignSouthEast:
    yloc=y+textheight;
    break;
  case AlignEast:
  case AlignWest:
    yloc=y+(height-textheight)/2;
    break;
  case AlignCenter:
    yloc=y+(height-textheight)/2;
    break;
  default:
    DOASSERT(FALSE, "Unknow orientation passed to GLWindowRep::DrawText");
  }

  glRasterPos2i(0, 0);
  glBitmap(0, 0, 0, 0, xloc, yloc, 0);
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);

  GLCHECKERROR();
}

/* Calculate the locations of points needed to draw a rectangle,
 * based on the alignment and orientation.  points array is only
 * needed if orientation is non-zero. */

void GLWindowRep::CalculateLocation(Coord center_x, Coord center_y, Coord width,
			     Coord height, SymbolAlignment alignment,
			     Coord orientation,
			     GLfloat point_x[4], GLfloat point_y[4])
{
  Coord sine, cosine;
  if (orientation != 0.0) {

    /* Minus sign is needed to preserve positive rotation being counter-
     * clockwise in X's upside-down coordinate system. */
    if (orientation==0.0) {
	sine=0.0;
	cosine=0.0;
    }
    else {
    	Coord radians = -orientation * M_PI / 180.0;
    	sine = sin(radians);
    	cosine = cos(radians);
    }

    Point relativePoints[4];

    switch (alignment) {
    case AlignNorthWest:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = 0.0;
      relativePoints[1].y = height;
      relativePoints[2].x = width;
      relativePoints[2].y = height;
      relativePoints[3].x = width;
      relativePoints[3].y = 0.0;
      break;

    case AlignNorth:
      relativePoints[0].x = -width / 2.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = -width / 2.0;
      relativePoints[1].y = height;
      relativePoints[2].x = width / 2.0;
      relativePoints[2].y = height;
      relativePoints[3].x = width / 2.0;
      relativePoints[3].y = 0.0;
      break;

    case AlignNorthEast:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = -width;
      relativePoints[1].y = 0.0;
      relativePoints[2].x = -width;
      relativePoints[2].y = height;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = height;
      break;

    case AlignWest:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = height / 2.0;
      relativePoints[1].x = width;
      relativePoints[1].y = height / 2.0;
      relativePoints[2].x = width;
      relativePoints[2].y = -height / 2.0;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = -height / 2.0;
      break;

    case AlignCenter:
      relativePoints[0].x = width / 2.0;
      relativePoints[0].y = height / 2.0;
      relativePoints[1].x = -width / 2.0;
      relativePoints[1].y = height / 2.0;
      relativePoints[2].x = -width / 2.0;
      relativePoints[2].y = -height / 2.0;
      relativePoints[3].x = width / 2.0;
      relativePoints[3].y = -height / 2.0;
      break;

    case AlignEast:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = -height / 2.0;
      relativePoints[1].x = -width;
      relativePoints[1].y = -height / 2.0;
      relativePoints[2].x = -width;
      relativePoints[2].y = height / 2.0;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = height / 2.0;
      break;

    case AlignSouthWest:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = width;
      relativePoints[1].y = 0.0;
      relativePoints[2].x = width;
      relativePoints[2].y = -height;
      relativePoints[3].x = 0.0;
      relativePoints[3].y = -height;
      break;

    case AlignSouth:
      relativePoints[0].x = width / 2.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = width / 2.0;
      relativePoints[1].y = -height;
      relativePoints[2].x = -width / 2.0;
      relativePoints[2].y = -height;
      relativePoints[3].x = -width / 2.0;
      relativePoints[3].y = 0.0;
      break;

    case AlignSouthEast:
      relativePoints[0].x = 0.0;
      relativePoints[0].y = 0.0;
      relativePoints[1].x = 0.0;
      relativePoints[1].y = -height;
      relativePoints[2].x = -width;
      relativePoints[2].y = -height;
      relativePoints[3].x = -width;
      relativePoints[3].y = 0.0;
      break;

    default:
      DOASSERT(false, "Illegal alignment value");
      break;
    }

    point_x[0] =
      center_x + relativePoints[0].x * cosine - relativePoints[0].y * sine;
    point_y[0] = 
      center_y + relativePoints[0].y * cosine + relativePoints[0].x * sine;
    point_x[1] = center_x + relativePoints[1].x * cosine -
      relativePoints[1].y * sine;
    point_y[1] = center_y + relativePoints[1].y * cosine +
      relativePoints[1].x * sine;
    point_x[2] = center_x + relativePoints[2].x * cosine -
      relativePoints[2].y * sine;
    point_y[2] = center_y + relativePoints[2].y * cosine +
      relativePoints[2].x * sine;
    point_x[3] = center_x + relativePoints[3].x * cosine -
      relativePoints[3].y * sine;
    point_y[3] = center_y + relativePoints[3].y * cosine +
      relativePoints[3].x * sine;
  }

  switch (alignment) {
  case AlignNorthWest:
    // Symbol x and y values stay the same.
    break;

  case AlignNorth:
    center_x -= width / 2;
    break;

  case AlignNorthEast:
    center_x -= width;
    break;

  case AlignWest:
    center_y -= height / 2;
    break;

  case AlignCenter:
    center_x -= width / 2;
    center_y -= height / 2;
    break;

  case AlignEast:
    center_x -=  width;
    center_y -=  height / 2;
    break;

  case AlignSouthWest:
    center_y -= height;
    break;

  case AlignSouth:
    center_x -= width / 2;
    center_y -= height;
    break;

  case AlignSouthEast:
    center_x -= width;
    center_y -= height;
    break;

  default:
    DOASSERT(false, "Illegal alignment value");
    break;
  }
  point_x[0]=center_x-width/2.0;
  point_y[0]=center_y-height/2.0;
  point_x[1]=center_x+width/2.0;
  point_y[1]=center_y-height/2.0;
  point_x[2]=center_x+width/2.0;
  point_y[2]=center_y+height/2.0;
  point_x[3]=center_x-width/2.0;
  point_y[3]=center_y+height/2.0;
}

#ifndef RAWMOUSEEVENTS
void GLWindowRep::DoPopup(int x, int y, int button)
{
  DOASSERT(_win, "Cannot display pop-up window in pixmap");

  char **msgs;
  int numMsgs;
  if (!HandlePopUp(x, y, button, msgs, numMsgs) || numMsgs <= 0)
    /* no message for pop-up */
    return;
  
  /* find width and height of window */
  SetNormalFont(); // In case the last font we used was different.
  XFontStruct *fontStruct = GetDisplay()->GetFontStruct();
  int textWidth = 0;
  int textHeight = 0;
  int charAscent = fontStruct->max_bounds.ascent;
  int charHeight = fontStruct->max_bounds.ascent + 
    fontStruct->max_bounds.descent;

  int i;
  for(i = 0; i < numMsgs; i++) {
#if defined(DEBUG)
    printf("msgs[%d]: %s\n", i, msgs[i]);
#endif
    textWidth = MAX(textWidth,
		    XTextWidth(fontStruct, msgs[i], strlen(msgs[i])));
  }
  textHeight = charHeight * numMsgs;
  
  XColorID bgnd = AP_GetXColorID(GetForeground());
  XColorID fgnd = AP_GetXColorID(GetBackground());

  /* Create window */
  XSetWindowAttributes attr;
  attr.background_pixmap	= None;
  attr.background_pixel		= bgnd;
  attr.border_pixmap		= CopyFromParent;
  attr.border_pixel		= fgnd;
  attr.bit_gravity		= ForgetGravity /* CenterGravity */;
  attr.win_gravity		= NorthWestGravity;
  attr.backing_store		= Always /* NotUseful WhenMapped */;
  attr.backing_planes		= AllPlanes;
  attr.backing_pixel		= 0;
  attr.save_under		= True;
  attr.event_mask		= 0;
  attr.do_not_propagate_mask	= 0;
  attr.override_redirect	= True;
  attr.colormap			= DefaultColormap(_display,
						  DefaultScreen(_display));
  attr.cursor			= None;
  
  /* Create the window. */
  Window win;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  int absX, absY;
  AbsoluteOrigin(absX, absY);
  x += absX;
  y += absY;

  /* Make bottom right of window appear at cursor position */
  textWidth +=  2; /* allow for border */
  textHeight += 2; /* allow for border */
  x = x - textWidth + 1;
  y = y - textHeight + 1;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  
  if ((win = XCreateWindow(_display, DefaultRootWindow(_display), x, y,
			   textWidth, textHeight, 1, 0, InputOutput,
			   CopyFromParent, AllPlanes, &attr)) == 0) {
    DOASSERT(0, "Cannot create popup window");
  }
  
  XSelectInput(_display, win, ExposureMask | ButtonPressMask);
  
  /* Map the window so that it appears on the display. */
  if (XMapWindow(_display, win) < 0) {
    DOASSERT(0, "Cannot map popup window");
  }
  
  /* Do a sync to force the window to appear immediately */
  XSync(_display, false);
  
  XGCValues gcVal;
  gcVal.function = GXcopy;
  gcVal.plane_mask = AllPlanes;
  gcVal.foreground = fgnd;
  gcVal.background = bgnd;
  gcVal.font = fontStruct->fid;
  GC popUpGc = XCreateGC(_display, win, GCFunction | GCPlaneMask
			 | GCForeground | GCBackground | GCFont,
			 &gcVal);
  /*
    XSetState(_display, popUpGc, fgnd, bgnd, GXcopy,AllPlanes);
    */
  
#if defined(LIBCS)
  Boolean savePopup = false;
#else
  Boolean savePopup = Init::SavePopup();
#endif
  if (!savePopup)
    XGrabServer(_display);

  /* Draw the text messages into window */
  for(i = 0; i < numMsgs; i++) {
    int startY = charHeight * i;
    XDrawString(_display, win, popUpGc, 1, startY + charAscent, 
		msgs[i], strlen(msgs[i]));
  }

  while(1) {
    XEvent event;
    /* check parent for button release */
    if (savePopup) {
      if (XCheckWindowEvent(_display, win, ButtonPressMask, &event)) {
	/* done */
	XDestroyWindow(_display, win);
	break;
      }
    } else {
      if (XCheckWindowEvent(_display, _win, ButtonReleaseMask, &event)) {
	if (event.xbutton.button == (unsigned int)button) {
	  /* done */
	  XDestroyWindow(_display, win);
	  break;
	}
      }
    }
    
    /* check popUp window for exposure */
    if (XCheckWindowEvent(_display, win, ExposureMask, &event)) {
      /* Draw the text messages into window */
      for(i = 0; i < numMsgs; i++) {
	int startY = charHeight * i;
	XDrawString(_display, win, popUpGc, 1, startY + charAscent, 
		    msgs[i], strlen(msgs[i]));
      }
    }
  }

  XFreeGC(_display, popUpGc);
  if (!savePopup)
    XUngrabServer(_display);
  XSync(_display,false);
}
#endif

// initially not set to any graphical context
GLWindowRep * GLWindowRep::currWinRep=0;

// Make the window and GL graphical context the current graphical context
GLboolean GLWindowRep::makeCurrent() {
  GLCHECKERROR();
//  if (currWinRep != this) {
    switch (glXMakeCurrent(_display, _win, _gc)) {
    case GL_FALSE:
      currWinRep=0;
      printf("Cannot make current!\n");
      GLCHECKERROR();
      return GL_FALSE;
    case GL_TRUE:
      currWinRep=this;
#ifdef DEBUG
      printf("Can make current currWinRep=%d\n", int(currWinRep));
#endif
      GLCHECKERROR();
      return GL_TRUE;
    default:
      printf("glXMakeCurrent() returned unexpected val\n");
      GLCHECKERROR();
      return GL_FALSE;
    }
//  }
/*  else {
    DOASSERT(glXGetCurrentContext()==_gc, "GLGraphical context changed with unknown reason");
    GLCHECKERROR();
    return GL_TRUE;
  }*/
}

void GLWindowRep::PushTop()
{
#if defined (DEBUG)
  printf("GLWindow::PushTop\n");
#endif
  GLCHECKERROR();
  MAKECURRENT();
  GLMATRIXMODE(GL_MODELVIEW);
  DEBUGE(glPushMatrix());
#if defined (DEBUG)
  PrintTransform();
#endif
  GLCHECKERROR();
}

void GLWindowRep::PopTransform()
{
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::PopMatrix\n",this);
#endif
  GLCHECKERROR();
  MAKECURRENT();
#if defined(DEBUG)
#endif
  GLMATRIXMODE(GL_MODELVIEW);
  DEBUGE(glPopMatrix());
#if defined(DEBUG)
  PrintTransform();
#endif
  GLCHECKERROR();
}

void GLWindowRep::Scale(Coord sx, Coord sy) {
#if defined (DEBUG)
  printf("GLWindow::Scale(%f,%f)\n", sx, sy);
#endif
  GLCHECKERROR();
  MAKECURRENT();
  GLMATRIXMODE(GL_MODELVIEW);
  DEBUGE(glScaled(sx,sy,1.0));
  GLCHECKERROR();
}

void GLWindowRep::Translate(Coord dx, Coord dy) {
#if defined (DEBUG)
  printf("GLWindowRep(0x%p)::Translate %f %f\n",this, dx, dy);
#endif
  GLCHECKERROR();
  MAKECURRENT();
  GLMATRIXMODE(GL_MODELVIEW);
  GLdouble a[]={1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, dx, dy, 0, 1};
  DEBUGE(glTranslated(dx, dy, 0.0));
  //DEBUGE(glMultMatrixd(a));
  GLCHECKERROR();
}

void GLWindowRep::MakeIdentity() {
#if defined (DEBUG)
  printf("GLWindowRep(0x%p)::MakeIdentity()\n",this);
#endif
  GLCHECKERROR();
  MAKECURRENT();
  GLMATRIXMODE(GL_MODELVIEW);
  DEBUGE(glLoadIdentity());
  GLCHECKERROR();
}

void GLWindowRep::InverseTransform(Coord x, Coord y, Coord &newX, Coord &newY)
{
  GLdouble model[16];
  GLdouble proj[16];
  GLint view[4];
  MAKECURRENT();
  glGetDoublev(GL_MODELVIEW_MATRIX, model);
  glGetDoublev(GL_PROJECTION_MATRIX, proj);
  glGetIntegerv(GL_VIEWPORT, view);
  GLdouble objectX, objectY, objectZ;
  gluProject(x,y,0, model, proj, view, &objectX, &objectY, &objectZ);
  newX=objectX;
  newY=objectY;
}


/* return the transform on top of the stack. */
Transform2D *GLWindowRep::TopTransform() {
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::TopTransform(): ###############Not Implemented\n",this);
#endif
 return 0;
}

void GLWindowRep::PrintTransform()
{
  GLCHECKERROR();
  MAKECURRENT();
  // get the top matrix and print
  GLfloat a[16];
  printf("x=%d, y = %d, Width = %u, Height = %u\n", _x, _y, _width, _height);
  GLint depth;
  GLMATRIXMODE(GL_MODELVIEW);
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
  printf("Model view stack depth=%d\n", depth);
  GLCHECKERROR();
  glGetFloatv(GL_MODELVIEW_MATRIX, a);
  printf("Transform matrix for 0x%p\n", this);
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++)
      printf("%.2f\t", a[i+j*4]);
    printf("\n");
  }
  GLCHECKERROR();
  GLfloat width;
  glGetFloatv(GL_LINE_WIDTH, &width);
  printf("Line width = %.2f\tRecord width = %d\n", width, GetLineWidth());
}

/* Clear the transformation stack and put an identity 
   matrix as top of the stack */
void GLWindowRep::ClearTransformStack() {
  GLCHECKERROR();
#if defined(DEBUG)
  printf("GLWindowRep(0x%p)::ClearTransformStack()\n",this);
#endif
  MAKECURRENT();
  GLint depth;
  GLMATRIXMODE(GL_MODELVIEW);
  glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, &depth);
  while (depth>1) {
    glPopMatrix();
    depth--;
  }
  glLoadIdentity();
  GLCHECKERROR();
}
