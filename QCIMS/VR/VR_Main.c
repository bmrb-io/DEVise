
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

#include "VR.h"
#include "VR_Icon.c"


static char * CodeStrings[] =
    { "Success",
      "Out of memory",
      "X error",
      "Gray/color/bitmap kind mismatch",
      "Nonexistent image specified",
      "Nonexistent translator specified",
      "Nonexistent window specified",
      "Error opening display",
      "Visual not found",
      "Size mismatch",
      "Image data wasn't set",
      "Unknown window",
      "Event not handled",
      "Too many file descriptors",
      "No such file descriptor",
      "Bad file descriptor",
      "Interrupt occurred",
      "Translator table absent",
      "Visual mismatch",
      "Bad event type: only key/button-press and configure allowed",
      "Event was not registered",
      "Too many events registered",
      "Event handler failed",
      "Cant change window attribs",
      VR_errmsg, 
      "Some strange error"
    };

#define WIN_VALIDITY_EXPIRY 5

extern int XWinIsValid(Window win, Window *root, Window *parent)
{
  #ifdef _TRUST_XIDS
  return 1;
  #else

  /* if this call is for the same win as last call, return 1 
     without checking, upto WIN_VALIDITY_EXPIRY times */
  static Window last_win = 0;
  Status ans; 
  static int num_last_win_checks = WIN_VALIDITY_EXPIRY;
  Window t_root, t_parent, *children = 0;
  unsigned int nchildren;

  if ((win == last_win) && (num_last_win_checks < WIN_VALIDITY_EXPIRY)) {
    num_last_win_checks++;
    return(1); 
  }


  ans = XQueryTree(VR_TheDisplay, win, (root)?root:&t_root,
	       (parent)?parent:&t_parent, &children, &nchildren);
  if (children) XFree(children);
  if (ans) { 
    num_last_win_checks = 1;
    last_win = win;
  }
  return(ans);
  #endif
}

extern char * VR_ReturnCodeToString(VR_ReturnCode rc)
{
  if (rc >= VR_NUM_RETURN_CODES) return(CodeStrings[VR_NUM_RETURN_CODES]);
  else return(CodeStrings[rc]);
}

static char *vclass_name[] = {
    "StaticGray",
    "GrayScale",
    "StaticColor",
    "PseudoColor",
    "TrueColor",
    "DirectColor"
};

extern char * VR_ViewerClassName(int vclass)
{
  if (vclass < 0) return(vclass_name[0]);
  if (vclass > 5) return(vclass_name[5]);
  return(vclass_name[vclass]);
}


extern VR_ReturnCode VR_InitViewerVisual(char *dname, FILE * errfile, int vclass)
{

  int i,j,nvis,bestdepth,bestloc;
  XColor xcolor;
  XVisualInfo vinfo, *vlist;
  Status ans;
  int triedVclass[6], visOrder[6];
  int t_new, t_rdonly;
  XImage *xim;
  Window win;
  XSetWindowAttributes attr;
  XGCValues gcv;

  strcpy(VR_errmsg,"");
  strcpy(VR_WindowTitle,"VR:"); 

  VR_ColormapsSize = 1;
  if ((VR_Colormaps = (VR_Colormap *) calloc(1, VR_ColormapsSize
	    * sizeof(VR_Colormap))) == NULL) { 
    return(VR_OUT_OF_MEMORY); 
  } 
  VR_NumColormaps = 0; 

  VR_ImagesSize = 4;
  if ((VR_Images = (VR_Image *) calloc(1, VR_ImagesSize
	    * sizeof(VR_Image))) == NULL) { 
    free(VR_Colormaps); 
    return(VR_OUT_OF_MEMORY); 
  } 
  VR_NumImages = 0; 


  VR_TranslatorsSize = 2;
  if ((VR_Translators = (VR_Translator *) calloc(1, VR_TranslatorsSize
	    * sizeof(VR_Translator))) == NULL) { 
    free(VR_Colormaps); 
    free(VR_Images); 
    return(VR_OUT_OF_MEMORY); 
  } 
  VR_NumTranslators = 0; 

  VR_WindowsSize = 2;
  if ((VR_Windows = (VR_Window *) calloc(1, VR_WindowsSize
	    * sizeof(VR_Window))) == NULL) { 
    free(VR_Colormaps); 
    free(VR_Translators); 
    free(VR_Images); 
    return(VR_OUT_OF_MEMORY); 
  } 
  VR_NumWindows = 0; 




  if (!(VR_TheDisplay = XOpenDisplay(dname))) {
    free(VR_Windows); 
    free(VR_Colormaps); 
    free(VR_Translators); 
    free(VR_Images); 
    return(VR_DISPLAY_ERROR);
  } 

  VR_TheScreen = DefaultScreen(VR_TheDisplay);

  VR_ScreenBacking = DoesBackingStore(DefaultScreenOfDisplay(VR_TheDisplay));

  switch (VR_ScreenBacking) {
    case NotUseful:
      VR_ScreenDoesBacking = 0;
      break;
    default:
      VR_ScreenDoesBacking = 1;
      break;
  }

  VR_ForeGround = 0;
  VR_BackGround = 1;

  /************** get visual **************/

  /* for non-def visuals, if the specified visual is not found,
     we will try the visuals in this order: */
  if (DefaultDepth(VR_TheDisplay, VR_TheScreen) <= 8) {
    visOrder[0] = PseudoColor;
    visOrder[1] = DirectColor;
    visOrder[2] = TrueColor;
    visOrder[3] = StaticColor;
    visOrder[4] = StaticGray;
    visOrder[5] = GrayScale;
  } else {
    visOrder[0] = TrueColor;
    visOrder[1] = PseudoColor;
    visOrder[2] = DirectColor;
    visOrder[3] = StaticColor;
    visOrder[4] = StaticGray;
    visOrder[5] = GrayScale;
  }

  vlist = (XVisualInfo *) 0;
  bestloc = -1;

  if (vclass == VR_VisDefault) {
    VR_TheVisual = DefaultVisual(VR_TheDisplay, VR_TheScreen);
    vinfo.visualid = XVisualIDFromVisual(VR_TheVisual);
    vlist = XGetVisualInfo(VR_TheDisplay, VisualIDMask, &vinfo, &nvis);
    if (!vlist) goto vis_not_found; /* should not happen */
    vclass = vlist[0].class;
    bestloc = 0;
    if ((vlist[0].depth <= 16) || (vclass==TrueColor)) {
      goto vis_found;
    }
    /* else fall through */
    XFree(vlist);
    vlist = (XVisualInfo *) 0;
  }

  for (i=0;i<6;i++) triedVclass[i] = 0;

  /* try preferred visual */
find_vis:
  vinfo.class = vclass;
  vinfo.screen = VR_TheScreen;
  vlist = XGetVisualInfo(VR_TheDisplay, VisualScreenMask | VisualClassMask,
     &vinfo, &nvis);
  triedVclass[vclass] = 1;
  bestdepth = 0;
  if (vlist) {
    for (i=0;i<nvis;i++) {
      if (((vlist[i].depth <= 16) || (vlist[i].class == TrueColor) )
                  && (vlist[i].depth > bestdepth)) {
	bestdepth = vlist[i].depth;
	bestloc = i;
    } }
    if (bestdepth > 0) {
      goto vis_found;
    } else {
      XFree(vlist);
      vlist = (XVisualInfo *) 0;
    }
  }

  /* try next visual class.
     for DefaultDepth <= 8, first pref is PseudoColor
     for DefaultDepth > 8, first pref is TrueColor
  */
  for (j=0;j<6;j++) {
    if (!triedVclass[visOrder[j]]) {
      vclass = visOrder[j];
      goto find_vis;
    }
  }

vis_not_found:
  XCloseDisplay(VR_TheDisplay);
  free(VR_Windows); 
  free(VR_Colormaps); 
  free(VR_Translators); 
  free(VR_Images); 
  return(VR_NO_VISUAL);

vis_found:
  /* vlist[bestloc] is the XVisualInfo struct to be used */
  /* check for the heck of it: */
  if ((!vlist) || (bestloc < 0)) goto vis_not_found;

  VR_TheVisual = vlist[bestloc].visual;
  VR_Depth = vlist[bestloc].depth;
  VR_MaxPixel = 1 << VR_Depth;
  VR_VisualClass =
  VR_VisualInfo.vclass = vlist[bestloc].class;


  VR_VisualInfo.colormap_size = vlist[bestloc].colormap_size;
  VR_VisualInfo.bits_per_rgb = vlist[bestloc].bits_per_rgb;

  VR_VisualInfo.Id = XVisualIDFromVisual(VR_TheVisual);
  if ((VR_VisualInfo.Id !=
    XVisualIDFromVisual(DefaultVisual(VR_TheDisplay, VR_TheScreen))) || 
      (VR_Depth != DefaultDepth(VR_TheDisplay, VR_TheScreen))) {
	VR_DefaultColormap = 
	  XCreateColormap(VR_TheDisplay, 
	     RootWindow(VR_TheDisplay, VR_TheScreen),
	     VR_TheVisual, AllocNone);
	  VR_FreeDefCmapOnExit = 1;
  } else {
	VR_DefaultColormap = DefaultColormap(VR_TheDisplay, 
	    VR_TheScreen); 
	  VR_FreeDefCmapOnExit = 0;
  } 


  if ((VR_VisualClass == StaticColor) ||
      (VR_VisualClass == TrueColor) ||
      (VR_VisualClass == PseudoColor) ||
      (VR_VisualClass == DirectColor)) {
	VR_ColorSupported = 1;
  } else {
	VR_ColorSupported = 0;
  }

  if ((VR_VisualClass == PseudoColor) ||
      (VR_VisualClass == DirectColor) ||
      (VR_VisualClass == GrayScale)) {
	VR_FixedColors = 0;
  } else {
	VR_FixedColors = 1;
  }

  if (VR_VisualClass == TrueColor) {
    VR_VisualInfo.red_shift = 0;
    VR_VisualInfo.green_shift = 0;
    VR_VisualInfo.blue_shift = 0;

    VR_VisualInfo.red_bits = 0;
    VR_VisualInfo.green_bits = 0;
    VR_VisualInfo.blue_bits = 0;

    if (vlist[bestloc].red_mask) {
      while (!(vlist[bestloc].red_mask & 1)) {
	VR_VisualInfo.red_shift++; vlist[bestloc].red_mask >>= 1;
      }
      while (vlist[bestloc].red_mask & 1) {
	VR_VisualInfo.red_bits++; vlist[bestloc].red_mask >>= 1;
      }
      VR_VisualInfo.red_shift = 24 + 8 - 
	  (VR_VisualInfo.red_shift + VR_VisualInfo.red_bits);
    }
    if (vlist[bestloc].green_mask) {
      while (!(vlist[bestloc].green_mask & 1)) {
	VR_VisualInfo.green_shift++; vlist[bestloc].green_mask >>= 1;
      }
      while (vlist[bestloc].green_mask & 1) {
	VR_VisualInfo.green_bits++; vlist[bestloc].green_mask >>= 1;
      }
      VR_VisualInfo.green_shift = 24 + 8 - 
	  (VR_VisualInfo.green_shift + VR_VisualInfo.green_bits);
    }
    if (vlist[bestloc].blue_mask) {
      while (!(vlist[bestloc].blue_mask & 1)) {
	VR_VisualInfo.blue_shift++; vlist[bestloc].blue_mask >>= 1;
      }
      while (vlist[bestloc].blue_mask & 1) {
	VR_VisualInfo.blue_bits++; vlist[bestloc].blue_mask >>= 1;
      }
      VR_VisualInfo.blue_shift = 24 + 8 - 
	  (VR_VisualInfo.blue_shift + VR_VisualInfo.blue_bits);
    }
    VR_VisualInfo.least_bits = VR_VisualInfo.blue_bits;
    if (VR_VisualInfo.green_bits < VR_VisualInfo.least_bits)
      VR_VisualInfo.least_bits = VR_VisualInfo.green_bits;
    if (VR_VisualInfo.red_bits < VR_VisualInfo.least_bits)
      VR_VisualInfo.least_bits = VR_VisualInfo.red_bits;

    VR_VisualInfo.least_mask = 0xFF &
       (0xFF << (8 - VR_VisualInfo.least_bits));
    VR_VisualInfo.red_mask = 0xFF &
       (0xFF << (8 - VR_VisualInfo.red_bits));
    VR_VisualInfo.green_mask = 0xFF &
       (0xFF << (8 - VR_VisualInfo.green_bits));
    VR_VisualInfo.blue_mask = 0xFF &
       (0xFF << (8 - VR_VisualInfo.blue_bits));
  } 

  XFree(vlist);

  /** figure our VR_BitsPerPixel.. the only unbreakable way seems
      to be to create an ximage (sigh) **/
  xim = XCreateImage(VR_TheDisplay, VR_TheVisual,
	VR_Depth, ZPixmap, 0, NULL, 64, 64, 8, 0);
  if (!xim) {
    /* shouldn't really happen */
    XCloseDisplay(VR_TheDisplay);
    free(VR_Windows); 
    free(VR_Colormaps); 
    free(VR_Translators); 
    free(VR_Images); 
    return(VR_OUT_OF_MEMORY);
  }
  VR_BitsPerPixel = xim->bits_per_pixel;
  XDestroyImage(xim);

  VR_BppBytes = VR_BitsPerPixel/8;
  VR_BppBits = VR_BitsPerPixel - (VR_BppBytes*8);
  VR_BppAuxBytes = VR_BppBytes;
  if (VR_BppBits > 0) VR_BppAuxBytes++;

  t_new = 1;
  t_rdonly = 1;
  VR_DefCmapLoc = FindOrSetCmap(VR_DefaultColormap, &t_new, &t_rdonly, 0);

  /* set byte order */
  if (ImageByteOrder(VR_TheDisplay) == LSBFirst) VR_ImageByteOrder = 0;
  else VR_ImageByteOrder = 1;

  /* create GraphicsContext */
  if (VR_Depth != DefaultDepth(VR_TheDisplay, VR_TheScreen)) {
    
    XFlush(VR_TheDisplay);
    XSync(VR_TheDisplay, False);

    attr.colormap = VR_DefaultColormap;
    attr.background_pixel = VR_BackGround;
    attr.border_pixel = VR_ForeGround;

    win = XCreateWindow(VR_TheDisplay,
       RootWindow(VR_TheDisplay, VR_TheScreen), 0, 0, 100, 100, 5,
       VR_Depth, InputOutput, VR_TheVisual,
       CWBackPixel | CWBorderPixel | CWColormap, &attr);
    
    XFlush(VR_TheDisplay);
    XSync(VR_TheDisplay, False);

    gcv.foreground = VR_ForeGround;
    gcv.background = VR_BackGround;

    VR_TheGC =
      XCreateGC(VR_TheDisplay, win, GCForeground | GCBackground, &gcv);
    XDestroyWindow(VR_TheDisplay, win);

  } else {
    VR_TheGC = XDefaultGC(VR_TheDisplay, VR_TheScreen);
    XGetGCValues(VR_TheDisplay, VR_TheGC, GCForeground | GCBackground, &gcv);
    VR_ForeGround = gcv.foreground;
    VR_BackGround = gcv.background;
  }


  /* create icon pixmap */
  VR_icon_pixmap = XCreateBitmapFromData(VR_TheDisplay,
	 RootWindow(VR_TheDisplay, VR_TheScreen), icon_bits,
	 icon_width, icon_height);


  for (i=0;i<VR_MAX_FDS;i++) {
    VR_Fds[i] = -1;
  }
  VR_NumFds = 1;
  VR_Fds[0] = ConnectionNumber(VR_TheDisplay);
  
  if (errfile) VR_errfile = errfile;
  else VR_errfile = stderr;

  XSetIOErrorHandler(VR_FatalError);
  XSetErrorHandler(VR_NonFatalError);

  XSelectInput(VR_TheDisplay,
    RootWindow(VR_TheDisplay, VR_TheScreen),
    SubstructureNotifyMask);

  VR_WaitCursor = XCreateFontCursor(VR_TheDisplay, XC_watch);

  return(VR_SUCCESS);

}

extern VR_ReturnCode VR_InitViewer(char *dname, FILE * errfile)
{
  return(VR_InitViewerVisual(dname, errfile, VR_VisDefault));
}

extern VR_ReturnCode VR_CloseViewer(void)
{
  int i; 

  XFreeCursor(VR_TheDisplay, VR_WaitCursor);

  for (i=0;i<VR_ImagesSize;i++)  {
    if (VR_Images[i].inUse) VR_FreeImage(i);
  }

  for (i=0;i<VR_TranslatorsSize;i++)  {
    if (VR_Translators[i].inUseCount) VR_FreeTranslator(i);
    /* will free colormaps too */ 
  }

  for (i=0;i<VR_WindowsSize;i++)  {
    if (VR_Windows[i].inUse) VR_FreeWindow(i);
  }

  XFreePixmap(VR_TheDisplay, VR_icon_pixmap);
  if (VR_Depth != DefaultDepth(VR_TheDisplay, VR_TheScreen))
    XFreeGC(VR_TheDisplay, VR_TheGC);

  XCloseDisplay(VR_TheDisplay);
  free(VR_Windows); 
  free(VR_Colormaps); 
  free(VR_Translators); 
  free(VR_Images); 

  if (VR_FreeDefCmapOnExit) {
    XFreeColormap(VR_TheDisplay, VR_DefaultColormap);
  }
  return(VR_SUCCESS);
}


extern VR_ImId VR_GetNewImage(int width, int height, VR_ImKind kind,
			      char * name)
{
  int i, slot;
  VR_Image * temp;

  if (VR_NumImages == VR_ImagesSize) {
    if ((temp = (VR_Image *) 
       calloc(1,sizeof(VR_Image)*(2*VR_ImagesSize))) == NULL)  {
	 strcpy(VR_errmsg,"VR_GetNewImage: out of memory");
         return(-1);
    }
    memcpy(temp,VR_Images,sizeof(VR_Image)*VR_ImagesSize);
    free(VR_Images);
    VR_Images = temp;
    VR_ImagesSize *= 2; 
  }

  slot = -1;
  for (i=0;i<VR_ImagesSize;i++) {
    if (!VR_Images[i].inUse) {
      slot = i;
      break;
    }
  }

  VR_Images[slot].inUse = 1;
  VR_Images[slot].kind = kind;
  if (kind==VR_RGB) VR_Images[slot].separateRGB = 0; /* default */
  VR_Images[slot].width = width;
  VR_Images[slot].height = height;
  VR_Images[slot].data = VR_Images[slot].red = VR_Images[slot].green 
		 = VR_Images[slot].blue = (unsigned char *) NULL;
  VR_Images[slot].pixw1 =
  VR_Images[slot].pixw2 =
  VR_Images[slot].pixw3 = 1;
  strcpy(VR_Images[slot].name,(name?name:"Untitled"));
  VR_NumImages++;

  return((VR_ImId) slot);

}

extern VR_ReturnCode VR_SetImageData(VR_ImId iid, unsigned char * data)
{
  if (!VR_Images[iid].inUse) return(VR_NO_SUCH_IMAGE);
  VR_Images[iid].data = data;
  if (VR_Images[iid].kind==VR_RGB) VR_Images[iid].separateRGB = 0;

  VR_Images[iid].pixw1 =
  VR_Images[iid].pixw2 =
  VR_Images[iid].pixw3 = 1;

  return(VR_SUCCESS);
}

extern VR_ReturnCode VR_SetNon8ImageData(VR_ImId iid, unsigned char * data,
    int pixw1, int pixw2, int pixw3)
{
  if (!VR_Images[iid].inUse) return(VR_NO_SUCH_IMAGE);
  VR_Images[iid].data = data;
  if (VR_Images[iid].kind==VR_RGB) VR_Images[iid].separateRGB = 0;

  VR_Images[iid].pixw1 = pixw1;
  VR_Images[iid].pixw2 = pixw2;
  VR_Images[iid].pixw3 = pixw3;

  return(VR_SUCCESS);
}


extern VR_ReturnCode VR_SetRGBImageData(VR_ImId iid,
   unsigned char * red, unsigned char * green, unsigned char *blue)
{

  if (!VR_Images[iid].inUse) return(VR_NO_SUCH_IMAGE);
  if (VR_Images[iid].kind != VR_RGB) return(VR_KIND_MISMATCH);
  VR_Images[iid].red = red;
  VR_Images[iid].green = green;
  VR_Images[iid].blue = blue;
  VR_Images[iid].separateRGB = 1;

  VR_Images[iid].pixw1 =
  VR_Images[iid].pixw2 =
  VR_Images[iid].pixw3 = 1;

  return(VR_SUCCESS);

}

extern VR_ReturnCode VR_SetNon8RGBImageData(VR_ImId iid,
   unsigned char * red, unsigned char * green, unsigned char *blue,
   int pixw1, int pixw2, int pixw3)
{

  if (!VR_Images[iid].inUse) return(VR_NO_SUCH_IMAGE);
  if (VR_Images[iid].kind != VR_RGB) return(VR_KIND_MISMATCH);
  VR_Images[iid].red = red;
  VR_Images[iid].green = green;
  VR_Images[iid].blue = blue;
  VR_Images[iid].separateRGB = 1;

  VR_Images[iid].pixw1 = pixw1;
  VR_Images[iid].pixw2 = pixw2;
  VR_Images[iid].pixw3 = pixw3;

  return(VR_SUCCESS);

}

static int VR_WinRefresher(VR_WinId wid, XEvent *evt)
{
  if (VR_RefreshWindow(wid) == VR_SUCCESS) return(1);
  else return(0);
}

static int VR_WinKiller(VR_WinId wid, XEvent *evt)
{
  if (VR_FreeWindow(wid) == VR_SUCCESS) return(1);
  else return(0);
}

extern VR_WinId VR_SetWindow(int width, int height,int locx, int locy,
               VR_TransId tid, Window the_win, int norefresh, int noq,
	       int *trybacking, int retain_wattribs) 
{

  int i, slot, cslot, emask;
  XSetWindowAttributes wattribs;
  XWindowAttributes wattribs_in;
  Colormap cmap; 
  unsigned long cwmask; 

  VR_Window * temp;

  if (!XWinIsValid(the_win, 0, 0)) {
    strcpy(VR_errmsg,"VR_SetWindow: XWindow does not seem valid");
    return(-1);
  }

  /* XGetWindowAttributes fails on a pixmap, so if it fails try it again
   * on the root window.  This is kind of kludgey, but it seems to work
   * okay.  RKW 1998-09-04. */
  if (XGetWindowAttributes(VR_TheDisplay, the_win, &wattribs_in) == 0) {
    XGetWindowAttributes(VR_TheDisplay, RootWindow(VR_TheDisplay,
      VR_TheScreen), &wattribs_in);
  }
  if (XVisualIDFromVisual(wattribs_in.visual) != VR_VisualInfo.Id) {
    strcpy(VR_errmsg,"VR_SetWindow: XWindow visual does not match VR_visual");
    return(-1);
  }
  if (wattribs_in.class != InputOutput) { 
    strcpy(VR_errmsg,"VR_SetWindow: XWindow is InputOnly");
    return(-1);
  }
  if (wattribs_in.root != RootWindow(VR_TheDisplay, VR_TheScreen)) { 
    strcpy(VR_errmsg,"VR_SetWindow: XWindow is on a different screen");
    return(-1);
  }
  if (wattribs_in.depth != VR_Depth) { 
    strcpy(VR_errmsg,"VR_SetWindow: XWindow has a different depth");
    return(-1);
  }

  if (VR_NumWindows == VR_WindowsSize) {
    if ((temp = (VR_Window *) 
       calloc(1,sizeof(VR_Window)*(2*VR_WindowsSize))) == NULL) {
         strcpy(VR_errmsg,"VR_SetWindow: out of memory");
         return(-1);
    }
    memcpy(temp,VR_Windows,sizeof(VR_Window)*VR_WindowsSize);
    free(VR_Windows);
    VR_Windows = temp;
    VR_WindowsSize *= 2; 
  } 

  slot = -1;
  for (i=0;i<VR_WindowsSize;i++) {
    if (!VR_Windows[i].inUse) {
      slot = i;
      break;
    }
  }

  
  if (!VR_Translators[tid].inUse) {
    strcpy(VR_errmsg,"VR_SetWindow: invalid translator"); 
    return(-1);
  }

  if (!VR_Colormaps[VR_Translators[tid].VR_cmap].inUse) {
    strcpy(VR_errmsg,"VR_SetWindow: invalid colormap in translator"); 
    return(-1);
  }

  cmap = VR_Colormaps[VR_Translators[tid].VR_cmap].cmap; 


  VR_Windows[slot].app_given = 1;
  VR_Windows[slot].kind = VR_Translators[tid].kind;
  VR_Windows[slot].tid = tid;
  VR_Windows[slot].width = width;
  VR_Windows[slot].height = height;
  VR_Windows[slot].xoff = locx;
  VR_Windows[slot].yoff = locy; 
  VR_Windows[slot].ximage = (XImage *) NULL;
  VR_Windows[slot].ximwidth =
  VR_Windows[slot].ximheight = 0;
  strcpy(VR_Windows[slot].title,VR_WindowTitle);


  VR_Windows[slot].win = the_win;
  VR_Windows[slot].parwin = the_win; 

  VR_Windows[slot].emask = wattribs_in.your_event_mask;

  VR_Windows[slot].OrigCmap = wattribs_in.colormap; 

  if (!retain_wattribs) { 
    cwmask = 0; 
    if (*trybacking) {
      if (!VR_ScreenDoesBacking) *trybacking = 0;
      else {
        wattribs.bit_gravity = NorthWestGravity; 
        wattribs.backing_store = VR_ScreenBacking;
        wattribs.backing_planes = VR_MaxPixel - 1;
        wattribs.backing_pixel = 0;
        cwmask |= (CWBitGravity | CWBackingStore | CWBackingPlanes
		| CWBackingPixel);
      }
    }
      
      
    /* find current colormap of window */
    if (wattribs_in.colormap != cmap) { 
      wattribs.colormap = cmap; 
      cwmask |= CWColormap;
    }

    if (cwmask) {
      XChangeWindowAttributes(VR_TheDisplay, the_win, 
        cwmask, &wattribs);
    }
    VR_Windows[slot].dont_change = 0; 
  } else {
    *trybacking = (wattribs_in.backing_store != NotUseful);
    norefresh = 1; /* don't let VR try to grab Expose events */ 
    noq = 1; /* avoid grabbing ButtonPress */ 
      
    if (wattribs_in.colormap != cmap) { 
      strcpy(VR_errmsg,"VR_SetWindow: invalid colormap in window"); 
      return(-1);
    }
    VR_Windows[slot].dont_change = 1; 
  } 

  VR_NumWindows++;
  VR_Windows[slot].inUse = 1;
  VR_Windows[slot].numEvents = 0;
  VR_Translators[tid].inUseCount++; 

  if (!VR_FixedColors) {
    cslot = VR_Translators[tid].VR_cmap; 
    for (i=0;i<VR_MaxPixel;i++) { 
      VR_Colormaps[cslot].ColorUseCount[i] +=
         VR_Translators[tid].ColorUseFlag[i]; 
    }
  }

  if (!norefresh && !(*trybacking)) 
    (void) VR_RegisterEventHandler(slot, Expose, 0,  VR_WinRefresher);

  if (!noq) 
    (void) VR_RegisterEventHandler(slot, KeyPress, XK_q, VR_WinKiller);

  return(slot);

}

extern VR_WinId VR_GetWindow(int width, int height, int locx, int locy,
	  VR_TransId tid, int parent_given, Window parent_win,
	  int norefresh, int noq, int *trybacking)
{

  int i, slot, cslot;
  XSetWindowAttributes wattribs;
  XSizeHints wsizeh;
  XWMHints wmhints;
  Window the_parent;
  char *par_name;
  Colormap cmap; 
  unsigned long cwmask;


  VR_Window * temp;

  if (parent_given && !XWinIsValid(parent_win, 0, 0)) {
    strcpy(VR_errmsg,"VR_GetWindow: specified parent XWindow seems invalid");
    return(-1);
  }

  if (VR_NumWindows == VR_WindowsSize) {
    if ((temp = (VR_Window *) 
       calloc(1,sizeof(VR_Window)*(2*VR_WindowsSize))) == NULL)  {
	 strcpy(VR_errmsg,"VR_GetWindow: out of memory");
         return(-1);
    }
    memcpy(temp,VR_Windows,sizeof(VR_Window)*VR_WindowsSize);
    free(VR_Windows);
    VR_Windows = temp;
    VR_WindowsSize *= 2; 
  } 

  slot = -1;
  for (i=0;i<VR_WindowsSize;i++) {
    if (!VR_Windows[i].inUse) {
      slot = i;
      break;
    }
  }

  
  if (!VR_Translators[tid].inUse) {
    strcpy(VR_errmsg,"VR_GetWindow: invalid translator");
    return(-1);
  }
  if (!VR_Colormaps[VR_Translators[tid].VR_cmap].inUse) {
    strcpy(VR_errmsg,"VR_GetWindow: invalid colormap in translator");
    return(-1);
  }
  cmap = VR_Colormaps[VR_Translators[tid].VR_cmap].cmap; 


  VR_Windows[slot].app_given = 0;
  VR_Windows[slot].dont_change = 0;
  VR_Windows[slot].kind = VR_Translators[tid].kind;
  VR_Windows[slot].tid = tid;
  VR_Windows[slot].width = width;
  VR_Windows[slot].height = height;
  VR_Windows[slot].ximage = (XImage *) NULL;
  VR_Windows[slot].ximwidth =
  VR_Windows[slot].ximheight = 0;
  strcpy(VR_Windows[slot].title,VR_WindowTitle);

  cwmask = CWColormap | CWBitGravity | CWBackingStore | CWEventMask |
	   CWBackPixel | CWBorderPixel | CWOverrideRedirect;; 
	   
  wattribs.background_pixel = VR_BackGround;
  wattribs.border_pixel = VR_ForeGround;
  wattribs.colormap = cmap; 
  wattribs.bit_gravity = NorthWestGravity;
  wattribs.event_mask = 0; 
  
  wattribs.backing_store = NotUseful;
  wattribs.override_redirect = False;

  if (*trybacking) {
    if (!VR_ScreenDoesBacking) *trybacking = 0;
    else {
      wattribs.backing_store = VR_ScreenBacking; 
      wattribs.backing_planes = VR_MaxPixel - 1;
      wattribs.backing_pixel = 0;
      cwmask |= (CWBackingPlanes | CWBackingPixel);
  } }
  
  if (parent_given) {
    the_parent = parent_win;
    XSelectInput(VR_TheDisplay, the_parent, SubstructureNotifyMask);
  }
  else the_parent = RootWindow(VR_TheDisplay, VR_TheScreen);

  VR_Windows[slot].win = XCreateWindow(VR_TheDisplay,
	  the_parent, locx, locy, width, height, 0,
	  VR_Depth, InputOutput, VR_TheVisual, cwmask, &wattribs);
  VR_Windows[slot].parwin = the_parent; 
  

  /** for now, make window size > 2x2 **/
  wsizeh.min_width = 2;
  wsizeh.min_height = 2;
  wsizeh.flags = PMinSize;
  XSetNormalHints(VR_TheDisplay,  VR_Windows[slot].win, &wsizeh);

  XStoreName(VR_TheDisplay, VR_Windows[slot].win, VR_Windows[slot].title);
  XSetIconName(VR_TheDisplay, VR_Windows[slot].win, VR_Windows[slot].title);

  wmhints.icon_pixmap = VR_icon_pixmap;
  wmhints.flags = IconPixmapHint;
  XSetWMHints(VR_TheDisplay, VR_Windows[slot].win, &wmhints);

  XMapWindow(VR_TheDisplay, VR_Windows[slot].win);

  VR_NumWindows++;
  VR_Windows[slot].inUse = 1;
  VR_Windows[slot].numEvents = 0;
  VR_Windows[slot].emask = 0;
  VR_Windows[slot].xoff = 0;
  VR_Windows[slot].yoff = 0;

  VR_Translators[tid].inUseCount++; 
  if (!VR_FixedColors) {
    cslot = VR_Translators[tid].VR_cmap; 
    for (i=0;i<VR_MaxPixel;i++) { 
      VR_Colormaps[cslot].ColorUseCount[i] +=
         VR_Translators[tid].ColorUseFlag[i]; 
    }
  }

  if (!norefresh && !(*trybacking)) 
    (void) VR_RegisterEventHandler(slot, Expose, 0,  VR_WinRefresher);

  if (!noq) 
    (void) VR_RegisterEventHandler(slot, KeyPress, XK_q, VR_WinKiller);

  return(slot);

}

extern VR_ReturnCode VR_ChangeTranslator(VR_WinId wid, VR_TransId tid)
{
  XSetWindowAttributes wattribs;
  XWindowAttributes wattribs_in;
  VR_TransId old_tid; 
  Colormap cmap; 
  int i, old_cslot, cslot;



  if (!VR_Windows[wid].inUse) return(VR_NO_SUCH_WINDOW);

  if (!VR_Translators[tid].inUse) return(VR_NO_SUCH_TRANS);

  cslot = VR_Translators[tid].VR_cmap; 
  if (!VR_Colormaps[cslot].inUseCount)
    return(VR_NO_SUCH_TRANS);


  VR_Windows[wid].kind = VR_Translators[tid].kind;

  old_tid = VR_Windows[wid].tid;

  /* XGetWindowAttributes fails on a pixmap, so if it fails try it again
   * on the root window.  This is kind of kludgey, but it seems to work
   * okay.  RKW 1998-09-04. */
  if (XGetWindowAttributes(VR_TheDisplay, VR_Windows[wid].win,
	 &wattribs_in) == 0) {
    XGetWindowAttributes(VR_TheDisplay, RootWindow(VR_TheDisplay,
      VR_TheScreen), &wattribs_in);
  }
  cmap = VR_Colormaps[cslot].cmap; 
  if (wattribs_in.colormap != cmap) { 
    if (VR_Windows[wid].dont_change) return(VR_CANT_CHANGE_WINDOW); 
    wattribs.colormap = cmap;
    XChangeWindowAttributes(VR_TheDisplay, VR_Windows[wid].win,
      CWColormap, &wattribs);
  }  

  VR_Windows[wid].tid = tid;
  VR_Translators[tid].inUseCount++; 


  if (!VR_FixedColors) {
    old_cslot = VR_Translators[old_tid].VR_cmap;
    for (i=0;i<VR_MaxPixel;i++) { 
      VR_Colormaps[cslot].ColorUseCount[i] +=
         VR_Translators[tid].ColorUseFlag[i]; 
    } 
    for (i=0;i<VR_MaxPixel;i++) {
        if (VR_Translators[old_tid].ColorUseFlag[i] &&
	    VR_Colormaps[old_cslot].ColorUseCount[i]) {
	      if (--VR_Colormaps[old_cslot].ColorUseCount[i] == 0) {
	        VR_XFreeColor(old_cslot, i);
	      }
        }
    } 
    XFlush(VR_TheDisplay); 
  }

  if (--VR_Translators[old_tid].inUseCount == 0)
      VR_FreeTranslator(old_tid); 

  return(VR_SUCCESS);
}


extern VR_ReturnCode VR_FreeImage(VR_ImId iid)
{
  if (!VR_Images[iid].inUse) return(VR_SUCCESS);
  VR_Images[iid].data =
  VR_Images[iid].red =
  VR_Images[iid].green =
  VR_Images[iid].blue = (unsigned char *) NULL;
  VR_Images[iid].inUse = 0;
  VR_NumImages--;
  return(VR_SUCCESS);
}

extern void VR_FreeWindowXImage(VR_WinId wid)
{
  VR_TransId tid, cslot, i;

  if (!VR_Windows[wid].inUse) return;
  if (VR_Windows[wid].ximage) XDestroyImage(VR_Windows[wid].ximage);
  VR_Windows[wid].ximage = 0;
  VR_Windows[wid].ximwidth =
  VR_Windows[wid].ximheight = 0; 
}

extern VR_ReturnCode VR_FreeWindow(VR_WinId wid)
{
  VR_TransId tid, cslot, i;
  XSetWindowAttributes wattribs; 

  if (!VR_Windows[wid].inUse) return(VR_SUCCESS);
  if (VR_Windows[wid].ximage) XDestroyImage(VR_Windows[wid].ximage);
  VR_Windows[wid].ximage = 0;
  VR_Windows[wid].ximwidth =
  VR_Windows[wid].ximheight = 0; 

  for (i=VR_Windows[wid].numEvents-1;i>=0;i--)
    VR_UnRegisterEventHandler(wid, VR_Windows[wid].eventTypes[i],
       VR_Windows[wid].vals[i]);
  

  tid = VR_Windows[wid].tid; 
  if (!VR_Windows[wid].app_given)
    XDestroyWindow(VR_TheDisplay, VR_Windows[wid].win);
  else { 
    if ((tid < 0) || !VR_Translators[tid].inUse ||
	(VR_Colormaps[VR_Translators[tid].VR_cmap].cmap !=
	 VR_Windows[wid].OrigCmap)) { 
      wattribs.colormap = VR_Windows[wid].OrigCmap; 
      XChangeWindowAttributes(VR_TheDisplay, VR_Windows[wid].win,
        CWColormap, &wattribs);
    }
  } 

  VR_Windows[wid].ximage = (XImage *) NULL;
  if ((tid >= 0) && VR_Translators[tid].inUse) { 
    if (!VR_FixedColors) {
      cslot = VR_Translators[tid].VR_cmap;
      for (i=0;i<VR_MaxPixel;i++) {
        if (VR_Translators[tid].ColorUseFlag[i] &&
  	    VR_Colormaps[cslot].ColorUseCount[i]) {
	      if (--VR_Colormaps[cslot].ColorUseCount[i] == 0) {
	        VR_XFreeColor(cslot, i);
    } } }     }
    VR_NumWindows--;
    VR_Windows[wid].inUse = 0;
    if (--VR_Translators[tid].inUseCount == 0)
      VR_FreeTranslator(tid); 
  } else {
    VR_NumWindows--;
    VR_Windows[wid].inUse = 0;
  } 

  XFlush(VR_TheDisplay);
  return(VR_SUCCESS);
}


static unsigned char * AllocXimData(VR_ImId iid)
{
  unsigned long size;
  unsigned long bits_per_row, bytes_per_row;

  bits_per_row = VR_Images[iid].width * VR_BitsPerPixel;
  bytes_per_row = bits_per_row/8;
  if ((bytes_per_row * 8) != bits_per_row) bytes_per_row++;

  size = bytes_per_row * VR_Images[iid].height;
  return((unsigned char *) calloc(1,size));
}



extern VR_ReturnCode VR_ShowImage(VR_ImId iid, VR_WinId wid, int dodither)
{
  VR_TransId tid;
  VR_ImKind kind;
  unsigned char * data;
  char tname[200];

  if (!VR_Images[iid].inUse) return(VR_NO_SUCH_IMAGE);
  if (!VR_Windows[wid].inUse) return(VR_NO_SUCH_WINDOW);
  tid = VR_Windows[wid].tid;
  if (!VR_Translators[tid].inUse) return(VR_NO_SUCH_TRANS);


  kind = VR_Windows[wid].kind;
  if (VR_Images[iid].kind != kind) return(VR_KIND_MISMATCH);


  if (kind==VR_RGB) {
    if (VR_Images[iid].separateRGB) {
      if (!VR_Images[iid].red || !VR_Images[iid].green || !VR_Images[iid].green)
	return(VR_IMAGE_NOT_SET);
    }
    else if (!VR_Images[iid].data) return(VR_IMAGE_NOT_SET);
  }
  else if (!VR_Images[iid].data) return(VR_IMAGE_NOT_SET);


  /* everything matches */

  if (VR_Windows[wid].ximage) {
    if ((VR_Windows[wid].ximwidth != VR_Images[iid].width) ||
        (VR_Windows[wid].ximheight != VR_Images[iid].height)) {
      data = AllocXimData(iid);
      if (!data) return(VR_OUT_OF_MEMORY);
      VR_FreeWindowXImage(wid);
      VR_Windows[wid].ximage = XCreateImage(VR_TheDisplay,
	VR_TheVisual, VR_Depth, ZPixmap, 0, data,
	VR_Images[iid].width, VR_Images[iid].height, 8, 0);
    } else {
      data = VR_Windows[wid].ximage->data;
    }
  } else {
    data = AllocXimData(iid);
    if (!data) return(VR_OUT_OF_MEMORY);
    VR_Windows[wid].ximage = XCreateImage(VR_TheDisplay, VR_TheVisual,
	VR_Depth, ZPixmap, 0, data,
	VR_Images[iid].width, VR_Images[iid].height, 8, 0);
  }

  VR_Windows[wid].ximwidth = VR_Images[iid].width;
  VR_Windows[wid].ximheight = VR_Images[iid].height;

  if (dodither && (VR_Depth < 24)) {
    VR_ApplyFSTrans(iid, data, tid);
  } else {
    VR_ApplyTrans(iid, data, tid);
  }

  XPutImage(VR_TheDisplay, VR_Windows[wid].win,
      VR_TheGC, VR_Windows[wid].ximage, 0, 0, 
      VR_Windows[wid].xoff, VR_Windows[wid].yoff,
      VR_Images[iid].width, VR_Images[iid].height);

  if (!VR_Windows[wid].app_given) {
    sprintf(tname,"%s %s\0", VR_WindowTitle, VR_Images[iid].name);
    if (strcmp(tname,VR_Windows[wid].title)) {
      strcpy(VR_Windows[wid].title,tname);
      XStoreName(VR_TheDisplay, VR_Windows[wid].win, tname);
      XSetIconName(VR_TheDisplay, VR_Windows[wid].win, tname);
    }
  }

  XFlush(VR_TheDisplay);

  return(VR_SUCCESS);
}

extern VR_ReturnCode VR_ShowImageRect(VR_ImId iid, VR_WinId wid, 
    int wherex, int wherey, int forx, int fory, int dodither)
{
  VR_TransId tid;
  VR_ImKind kind;
  unsigned char * data;
  char tname[200];

  if (!VR_Images[iid].inUse) return(VR_NO_SUCH_IMAGE);
  if (!VR_Windows[wid].inUse) return(VR_NO_SUCH_WINDOW);
  tid = VR_Windows[wid].tid;
  if (!VR_Translators[tid].inUse) return(VR_NO_SUCH_TRANS);

  kind = VR_Windows[wid].kind;
  if (VR_Images[iid].kind != kind) return(VR_KIND_MISMATCH);


  if (kind==VR_RGB) {
    if (VR_Images[iid].separateRGB) {
      if (!VR_Images[iid].red || !VR_Images[iid].green || !VR_Images[iid].green)
	return(VR_IMAGE_NOT_SET);
    }
    else if (!VR_Images[iid].data) return(VR_IMAGE_NOT_SET);
  }
  else if (!VR_Images[iid].data) return(VR_IMAGE_NOT_SET);


  /* everything matches */

  if (VR_Windows[wid].ximage) {
    if ((VR_Windows[wid].ximwidth != VR_Images[iid].width) ||
        (VR_Windows[wid].ximheight != VR_Images[iid].height)) {
      data = AllocXimData(iid);
      if (!data) return(VR_OUT_OF_MEMORY);
      VR_FreeWindowXImage(wid);
      VR_Windows[wid].ximage = XCreateImage(VR_TheDisplay,
	VR_TheVisual, VR_Depth, ZPixmap, 0, data,
	VR_Images[iid].width, VR_Images[iid].height, 8, 0);
    } else {
      data = VR_Windows[wid].ximage->data;
    }
  } else {
    data = AllocXimData(iid);
    if (!data) return(VR_OUT_OF_MEMORY);
    VR_Windows[wid].ximage = XCreateImage(VR_TheDisplay, VR_TheVisual,
	VR_Depth, ZPixmap, 0, data,
	VR_Images[iid].width, VR_Images[iid].height, 8, 0);
  }

  VR_Windows[wid].ximwidth = VR_Images[iid].width;
  VR_Windows[wid].ximheight = VR_Images[iid].height;


  if (dodither && (VR_Depth < 24)) {
    VR_ApplyFSTransRect(iid, data, tid, wherex, wherey, forx, fory);
  } else {
    VR_ApplyTransRect(iid, data, tid, wherex, wherey, forx, fory);
  }


  XPutImage(VR_TheDisplay, VR_Windows[wid].win,
      VR_TheGC, VR_Windows[wid].ximage, wherex, wherey,
      wherex + VR_Windows[wid].xoff, wherey + VR_Windows[wid].yoff,
      forx, fory);

  if (!VR_Windows[wid].app_given) {
    sprintf(tname,"%s %s\0", VR_WindowTitle, VR_Images[iid].name);
    if (strcmp(tname,VR_Windows[wid].title)) {
      strcpy(VR_Windows[wid].title,tname);
      XStoreName(VR_TheDisplay, VR_Windows[wid].win, tname);
      XSetIconName(VR_TheDisplay, VR_Windows[wid].win, tname);
    }
  }

  XFlush(VR_TheDisplay);

  return(VR_SUCCESS);
}

extern VR_ReturnCode VR_RefreshWindow(VR_WinId wid)
{

  if ((wid < 0) || (wid >= VR_NumWindows)) return(VR_NO_SUCH_WINDOW);
  if (!VR_Windows[wid].inUse) return(VR_NO_SUCH_WINDOW);
  if (!VR_Windows[wid].ximage) return(VR_IMAGE_NOT_SET);

  XPutImage(VR_TheDisplay, VR_Windows[wid].win,
      VR_TheGC, VR_Windows[wid].ximage, 0, 0, 
      VR_Windows[wid].xoff, VR_Windows[wid].yoff,
      VR_Windows[wid].ximwidth, VR_Windows[wid].ximheight);

  XFlush(VR_TheDisplay);
}


    
extern Display * VR_GetDisplay(void)
{
  return(VR_TheDisplay);
}

extern int VR_GetScreen(void)
{
  return(VR_TheScreen);
}

extern Visual * VR_GetVisual(VR_ImKind kind)
{
  return(VR_TheVisual);
}

extern Window VR_WidToWin(VR_WinId wid)
{
  if ((wid < 0) || (wid >= VR_NumWindows) || 
      (!VR_Windows[wid].inUse)) return(-1);
  else return(VR_Windows[wid].win);
}



extern Window VR_GetXWindow(int width, int height, int locx, int locy, 
			    int parent_given, Window parent_win, char * name)
  /* create a simple Xwindow.. this is not managed by the viewer, and is
       only provided to be used as a parent window by applications */
{

  Window the_parent, win;
  XSetWindowAttributes wattribs;
  unsigned long cwmask;

  if (parent_given) {
    the_parent = parent_win;
  }
  else the_parent = RootWindow(VR_TheDisplay, VR_TheScreen);

  cwmask = CWColormap | CWBitGravity | CWOverrideRedirect |
	   CWBackingStore | CWBackPixel | CWBorderPixel; 
	   
  wattribs.colormap = VR_DefaultColormap; 
  wattribs.bit_gravity = NorthWestGravity;
  wattribs.backing_store = NotUseful;
  wattribs.background_pixel = VR_BackGround;
  wattribs.border_pixel = VR_ForeGround;
  
  wattribs.override_redirect = False;

  win = XCreateWindow(VR_TheDisplay,
	  the_parent, locx, locy, width, height, 0,
	  VR_Depth, InputOutput, VR_TheVisual,cwmask,&wattribs);

  

  XStoreName(VR_TheDisplay, win, (name) ? name : "VR-Window");
  XSetIconName(VR_TheDisplay,  win, (name) ? name : "VR-Window");


  XMapWindow(VR_TheDisplay, win);

  return(win);

}

extern void VR_ShowWaitCursor(VR_WinId wid)
{
  Window win = VR_WidToWin(wid);

  if (((long) win < 0) || VR_Windows[wid].dont_change) return; 
     
  XDefineCursor(VR_TheDisplay, win, VR_WaitCursor);

  XFlush(VR_TheDisplay); 

}

extern void VR_ShowNormalCursor(VR_WinId wid)
{
  Window win = VR_WidToWin(wid);

  if (((long) win < 0) || VR_Windows[wid].dont_change) return; 

  XDefineCursor(VR_TheDisplay, win, None);

  XFlush(VR_TheDisplay); 
}

