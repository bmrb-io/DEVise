
/* 
** Copyright 1995,1996,1997 by Viresh Ratnakar, Miron Livny
** 
** Permission to use and copy this software and its documentation
** for any non-commercial purpose and without fee is hereby granted,
** provided that the above copyright notice appear in all copies and that
** both that copyright notice and this permission notice appear in
** supporting documentation.
** 
**
** The University of Wisconsin and the copyright holders make no
** representations about the suitability of this software for any
** purpose.  It is provided "as is" without express or implied warranty.
** 
** 
** THE UNIVERSITY OF WISCONSIN AND THE COPYRIGHT HOLDERS DISCLAIM ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
** OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE UNIVERSITY OF
** WISCONSIN OR THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT
** OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
** OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
** OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
** OR PERFORMANCE OF THIS SOFTWARE.
** 
** Author:  Viresh Ratnakar
** Email:   ratnakar@cs.wisc.edu
**
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>


#include "VR.h"

extern int errno;


extern VR_ReturnCode VR_RegisterFd(int fd)
{
  int i;
  if (VR_NumFds == VR_MAX_FDS) return(VR_TOO_MANY_FDS);

  for (i=1;i<VR_MAX_FDS;i++) {
    if (VR_Fds[i] == -1) {
      VR_Fds[i] = fd;
      break;
    }
  }
  VR_NumFds++;
  return(VR_SUCCESS);
}

extern VR_ReturnCode VR_UnRegisterFd(int fd)
{
  int i;
  for (i=1;i<VR_MAX_FDS;i++) {
    if (VR_Fds[i] == fd) {
      VR_Fds[i] = -1;
      VR_NumFds--;
      return(VR_SUCCESS);
    }
  }
  return(VR_NO_SUCH_FD);
}

static Bool AlwaysYes(Display *d, XEvent *e, char *s)
{
  return(1);
}



static fd_set read_fds;

extern VR_ReturnCode VR_WaitForSomething(fd_set ** return_fds)
{
  int i, nfds;
  XEvent evt;
  int select_ret;
  

  while (1) {
    nfds = 0;
    FD_ZERO(&read_fds);
    for (i=0;i<VR_MAX_FDS;i++) {
      if (VR_Fds[i] != -1) {FD_SET(VR_Fds[i], &read_fds);nfds++;}
    }

    if ((nfds==1) && !VR_NumWindows) return(VR_SUCCESS);

    if ((select_ret =
      select(FD_SETSIZE, &read_fds, (fd_set *) NULL, (fd_set *) NULL,
	   (struct timeval *) NULL)) < 0) {

	   if (errno == EBADF) {
	     errno = 0;
	     return(VR_BAD_FD);
	   }
	   else { /* errno == EINTR */
	     errno = 0;
	     return(VR_SIGNAL_OCCURRED);
	   }
    }

    if (FD_ISSET(VR_Fds[0], &read_fds)) {
      if (XEventsQueued(VR_TheDisplay, QueuedAfterReading)) {
        /* get "destroy events" first */
        while (XCheckTypedEvent(VR_TheDisplay, DestroyNotify, &evt)) {
  	  VR_HandleEvent(&evt);
        }
        /* other events */
        while (XCheckIfEvent(VR_TheDisplay, &evt,AlwaysYes,(char *) 0)) {
	  VR_HandleEvent(&evt);
        }
        FD_CLR(VR_Fds[0], &read_fds);
      }
    }
    for (i=1;i<VR_MAX_FDS;i++) {
      if ((VR_Fds[i] != -1) && (FD_ISSET(VR_Fds[i], &read_fds))) {
	*return_fds = &read_fds;
	return(VR_SUCCESS);
      }
    }
  }
}

extern int VR_NonFatalError(Display * disp, XErrorEvent *err)
{
  char msg[80];
  XGetErrorText(disp, err->error_code, msg, 80);
  fprintf(VR_errfile, "%d: %s\n", err->error_code, msg);
  fflush(VR_errfile);
  return(1);
}

extern int VR_FatalError(Display * disp)
{
  fprintf(VR_errfile, "Fatal error: Exiting\n");
  fflush(VR_errfile);
}

extern VR_ReturnCode VR_RegisterEventHandler(VR_WinId wid, int etype,
  long val, VR_EventHandler handler)
{
  int i;
  int evmask_exists = 0;
  long emask;

  if ((wid < 0) || (wid >= VR_NumWindows) ||
      (!VR_Windows[wid].inUse)) return(VR_NO_SUCH_WINDOW);
  
  if (VR_Windows[wid].dont_change) return(VR_CANT_CHANGE_WINDOW);

  if (VR_Windows[wid].numEvents >= VR_MAX_EVENTS)
    return(VR_TOO_MANY_EVENTS);

  switch (etype) {
  
  case KeyPress:
  case KeyRelease:
  case ButtonPress:
  case ButtonRelease:

    if (etype==KeyRelease) emask = KeyReleaseMask;
    else if (etype==KeyPress) emask = KeyPressMask;
    else if (etype==ButtonRelease) emask = ButtonReleaseMask;
    else /* if (etype==ButtonPress) */ emask = ButtonPressMask;

    for (i=0;i<VR_Windows[wid].numEvents;i++) {
      if (etype==VR_Windows[wid].eventTypes[i]) {
	evmask_exists = 1;
	if (VR_Windows[wid].vals[i]==val) {
	  /* override previous handler */
	  VR_Windows[wid].handler[i] = handler;
	  return(VR_SUCCESS);
	}
      }
    }
    break;

  case Expose:
  case ConfigureNotify:

    if (etype==Expose) emask = ExposureMask;
    else /* if (etype==ConfigureNotify) */ emask = StructureNotifyMask;

    for (i=0;i<VR_Windows[wid].numEvents;i++) {
      if (etype==VR_Windows[wid].eventTypes[i]) {
	/* override previous handler */
	VR_Windows[wid].handler[i] = handler;
	return(VR_SUCCESS);
      }
    }
    break;
  
  default:
    return(VR_BAD_EVENT);
  }

  VR_Windows[wid].emask |= emask;
  if (!evmask_exists) XSelectInput(VR_TheDisplay, VR_Windows[wid].win,
    VR_Windows[wid].emask);
  i = VR_Windows[wid].numEvents;
  VR_Windows[wid].numEvents++;
  VR_Windows[wid].eventTypes[i] = etype;
  VR_Windows[wid].vals[i] = val;
  VR_Windows[wid].handler[i] = handler;
  
  return(VR_SUCCESS);
}


extern VR_ReturnCode VR_UnRegisterEventHandler(VR_WinId wid, int etype,
  long val)
{

  int i, loc;
  long emask_ct = 0;
  long emask;

  if ((wid < 0) || (wid >= VR_NumWindows) ||
      (!VR_Windows[wid].inUse)) return(VR_NO_SUCH_WINDOW);
  
  if (etype==Expose) {
    emask = ExposureMask;
  } else if (etype==ConfigureNotify) {
    emask = StructureNotifyMask;
  } else if (emask==KeyPress) {
    emask = KeyPressMask;
  } else if (emask==KeyRelease) {
    emask = KeyReleaseMask;
  } else if (emask==ButtonPress) {
    emask = ButtonPressMask;
  } else if (emask==ButtonRelease) {
    emask = ButtonReleaseMask;
  } else return(VR_BAD_EVENT);

  loc = -1;
  for (i=0;i<VR_Windows[wid].numEvents;i++) {
    if (VR_Windows[wid].eventTypes[i] == etype) {
      emask_ct++;
      if ((etype==Expose) || (etype==ConfigureNotify)) loc = i;
      else if (val==VR_Windows[wid].vals[i]) loc = i;
    }
  }

  if (loc < 0) return(VR_EVENT_NOT_REG);
  for (i= (loc+1); i < VR_Windows[wid].numEvents;i++) {
    VR_Windows[wid].eventTypes[i-1] = VR_Windows[wid].eventTypes[i];
    VR_Windows[wid].vals[i-1] = VR_Windows[wid].vals[i];
    VR_Windows[wid].handler[i-1] = VR_Windows[wid].handler[i];
  }
  VR_Windows[wid].numEvents--;

  if (emask_ct==1) {
    VR_Windows[wid].emask &= ~emask;
    XSelectInput(VR_TheDisplay, VR_Windows[wid].win, VR_Windows[wid].emask);
  }

  return(VR_SUCCESS);
}


static VR_WinId FindWindow(Window win)
{
  int i;

  for (i=0;i<VR_WindowsSize;i++) {
    if (VR_Windows[i].inUse && (VR_Windows[i].win==win))
      return((VR_WinId) i);
  }
  return((VR_WinId) -1);
}

static VR_ReturnCode FreeWindowOnDestroy(VR_WinId wid)
{
  VR_TransId tid, cslot, i;

  if (!VR_Windows[wid].inUse) return(VR_SUCCESS);
  if (VR_Windows[wid].ximage) XDestroyImage(VR_Windows[wid].ximage);
  VR_Windows[wid].ximage = (XImage *) NULL;
  VR_Windows[wid].ximwidth = 
  VR_Windows[wid].ximheight = 0;

  tid = VR_Windows[wid].tid;
  if ((tid >= 0) && VR_Translators[tid].inUse) { 
    if (!VR_FixedColors) {
      cslot = VR_Translators[tid].VR_cmap;
      for (i=0;i<VR_MaxPixel;i++) {
        if (VR_Translators[tid].ColorUseFlag[i] &&
	  VR_Colormaps[cslot].ColorUseCount[i]) {
	    if (--VR_Colormaps[cslot].ColorUseCount[i] == 0) {
	      VR_XFreeColor(cslot, i);
      } }   }
      XFlush(VR_TheDisplay); 
    }
    VR_NumWindows--;
    VR_Windows[wid].inUse = 0;
    if (--VR_Translators[tid].inUseCount == 0)
      VR_FreeTranslator(tid); 
  } else {
    VR_NumWindows--;
    VR_Windows[wid].inUse = 0;
  }
  return(VR_SUCCESS);
}


extern VR_ReturnCode VR_HandleEvent(XEvent *event)
{
  VR_WinId wid;
  KeySym key;
  XEvent dummy;
  int i;
  
  switch (event->type) {

  case DestroyNotify:
    if ((wid=FindWindow(event->xdestroywindow.window)) == -1)
      return(VR_EVENT_WIN_UNKNOWN);
    return(FreeWindowOnDestroy(wid));
    break;

  case Expose:
    if ((wid=FindWindow(event->xexpose.window)) == -1)
      return(VR_EVENT_WIN_UNKNOWN);
    /* remove all expose events for this window */
    while (XCheckTypedWindowEvent(VR_TheDisplay, 
	   event->xexpose.window, Expose, &dummy));
    for (i=0;i<VR_Windows[wid].numEvents;i++) {
      if (VR_Windows[wid].eventTypes[i]==Expose) {
	if ((*VR_Windows[wid].handler[i])(wid, event)) return(VR_SUCCESS);
	else return(VR_EVENT_HANDLER_FAILED);
      }
    }
    return(VR_EVENT_NOT_HANDLED);
    break;
  
  case ConfigureNotify:
    if ((wid=FindWindow(event->xconfigure.window)) == -1)
      return(VR_EVENT_WIN_UNKNOWN);
    for (i=0;i<VR_Windows[wid].numEvents;i++) {
      if (VR_Windows[wid].eventTypes[i]==ConfigureNotify) {
	if ((*VR_Windows[wid].handler[i])(wid, event)) return(VR_SUCCESS);
	else return(VR_EVENT_HANDLER_FAILED);
      }
    }
    return(VR_EVENT_NOT_HANDLED);
    break;
  
  case KeyPress:
  case KeyRelease:
    if ((wid=FindWindow(event->xkey.window)) == -1)
      return(VR_EVENT_WIN_UNKNOWN);
    key = XKeycodeToKeysym(VR_TheDisplay,event->xkey.keycode,0);
    for (i=0;i<VR_Windows[wid].numEvents;i++) {
      if ((VR_Windows[wid].eventTypes[i]==event->type) &&
	  (key==VR_Windows[wid].vals[i])) {
	if ((*VR_Windows[wid].handler[i])(wid, event)) return(VR_SUCCESS);
	else return(VR_EVENT_HANDLER_FAILED);
      }
    }
    return(VR_EVENT_NOT_HANDLED);
    break;

  case ButtonPress:
  case ButtonRelease:
    if ((wid=FindWindow(event->xbutton.window)) == -1)
      return(VR_EVENT_WIN_UNKNOWN);
    for (i=0;i<VR_Windows[wid].numEvents;i++) {
      if ((VR_Windows[wid].eventTypes[i]==event->type) &&
	  (event->xbutton.button==VR_Windows[wid].vals[i])) {
	if ((*VR_Windows[wid].handler[i])(wid, event)) return(VR_SUCCESS);
	else return(VR_EVENT_HANDLER_FAILED);
      }
    }
    return(VR_EVENT_NOT_HANDLED);
    break;

  default:
    return(VR_EVENT_NOT_HANDLED);
  }
}



