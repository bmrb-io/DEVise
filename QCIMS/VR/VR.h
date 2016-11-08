
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

#ifndef VIEWER_I_INCLUDED
#define VIEWER_I_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>


/************** platform-dependent configurations *************/ 

#define VR_ULONG_MSB_FIRST

#ifdef _RS6000
#include <sys/select.h>
#endif /* _RS6000 */

#ifdef _IRIX
#include <bstring.h>
#define FD_CLEAR FD_ZERO
#endif /* _IRIX */

#ifdef _PENTIUM
#include <sys/select.h>
#define FD_CLEAR FD_ZERO
#undef VR_ULONG_MSB_FIRST
#define VR_ULONG_LSB_FIRST
#endif

#ifdef _LINUX
#define FD_CLEAR FD_ZERO
#undef VR_ULONG_MSB_FIRST
#define VR_ULONG_LSB_FIRST
#endif

#ifdef _SUN
#define FD_CLEAR FD_ZERO
#endif

#ifdef _DEC
#undef VR_ULONG_MSB_FIRST
#define VR_ULONG_LSB_FIRST
#endif

#ifdef _MYUNIX
  /* add your own system dependencies here */
#endif

/********* the VR library has three basic objects: **********
 ********* Images, Windows, and Translators. You   **********
 ********* show an Image in a Window using a       **********
 ********* Translator.                             **********/

typedef long VR_ImId;       /* handle type for images */
typedef long VR_WinId;      /* handle type for windows */
typedef long VR_TransId;    /* handle type for translators */

typedef int VR_ReturnCode;  /* type of value returned by most functions */
                            /* for fuctions returning an Id, a negative
			       value indicates an error, and an error
			       message may be found in VR_errmsg */

/** different values possible for VR_ReturnCode **/
#define VR_SUCCESS 0
#define VR_OUT_OF_MEMORY 1
#define VR_XERROR 2
#define VR_KIND_MISMATCH 3
#define VR_NO_SUCH_IMAGE 4
#define VR_NO_SUCH_TRANS 5
#define VR_NO_SUCH_WINDOW 6
#define VR_DISPLAY_ERROR 7
#define VR_NO_VISUAL 8
#define VR_SIZE_MISMATCH 9
#define VR_IMAGE_NOT_SET 10
#define VR_EVENT_WIN_UNKNOWN 11
#define VR_EVENT_NOT_HANDLED 12
#define VR_TOO_MANY_FDS 13
#define VR_NO_SUCH_FD 14
#define VR_BAD_FD 15
#define VR_SIGNAL_OCCURRED 16
#define VR_NO_TRANS_TABLE 17
#define VR_VIS_MISMATCH 18
#define VR_BAD_EVENT 19
#define VR_EVENT_NOT_REG 20
#define VR_TOO_MANY_EVENTS 21
#define VR_EVENT_HANDLER_FAILED 22
#define VR_CANT_CHANGE_WINDOW 23
#define VR_SEE_ERR_MSG 24

#define VR_NUM_RETURN_CODES 25

extern char * VR_ReturnCodeToString(VR_ReturnCode rc);
  /* returns a string with the description of the return code */

#define VR_STRLENMAX 300 /* max string length */

extern char VR_errmsg[VR_STRLENMAX];

/****** Visual classes.. we just use X's symbol definitions ********
 ****** StaticGray, StaticColor, and TrueColor are "FixedColor" ****
 ****** visuals: only a fixed, immutable colormap is used.    ******/

#define VR_VisDefault -1
#define VR_VisStaticGray StaticGray
#define VR_VisGrayScale GrayScale
#define VR_VisStaticColor StaticColor
#define VR_VisPseudoColor PseudoColor
#define VR_VisTrueColor TrueColor
#define VR_VisDirectColor DirectColor

extern char * VR_ViewerClassName(int vclass);
  /** returns a string with the name of a visual class **/

extern VR_ReturnCode VR_InitViewerVisual(char *dname, FILE * errfile, int vclass);
    /* dname is the name of the display, NULL implies default display 
     * Must be called before anything else.
     * errfile is not NULL, all Xerrors will be printed
     * in that file. by default, errfile = stderr.
     * Note that non-fatal Xerrors 
     * will *NOT* crash Viewer code. It is up to
     * the application to do whatever it wants.
     * The format of error strings printed on errfile is:
     * code: code-string\n

     * vclass is the preferred visual class.. set to
     * VR_VisDefault to use the default visual of screen.
     * If vclass-visual is not available, we will try all
     * visuals from DirectColor down to StaticGray

     * The highest depth available for the chosen visual class will
     * be used. Only for TrueColor will depth > 16 be accepted
     */

extern VR_ReturnCode VR_InitViewer(char *dname, FILE * errfile);
    /* old start-up interface.. just calls
       VR_InitViewerVisual(dname, errfile, VR_VisDefault) */

extern VR_ReturnCode VR_CloseViewer(void);
    /* Clean up everything.. should be called in the end */

typedef enum {VR_Gray, VR_RGB} VR_ImKind;
    /* Various kinds of images possible */

extern VR_ImId VR_GetNewImage(int width, int height,
		  VR_ImKind kind, char * name);
    /* allocate stuff for an image and return a handle */

extern VR_ReturnCode VR_SetImageData(VR_ImId iid, unsigned char * data);
    /* set the pixel data for an image 
     * data should contain the pixel values in
     * row-major order (R then G then B if VR_RGB is the VR_ImKind)
     * data can be changed arbitrarily by application
     * data will never be `free'd by Viewer */

extern VR_ReturnCode VR_SetRGBImageData(VR_ImId iid,
       unsigned char * red, unsigned char * green, unsigned char *blue);
     /* special way of setting RGB data using separate 
      * red, green, blue arrays */

extern VR_ReturnCode VR_SetNon8ImageData(VR_ImId iid, unsigned char * data,
       int pixw1, int pixw2, int pixw3);
    /* exactly like VR_SetImageData, except that image pixels 
       may be n-bytes wide, with n not necessarily 1.
       Note that only 1 byte will actually be used.
       For grayscale, the pixw1 is used 
    */

extern VR_ReturnCode VR_SetNon8RGBImageData(VR_ImId iid,
       unsigned char * red, unsigned char * green, unsigned char *blue,
	      int pixw1, int pixw2, int pixw3);
    /* exactly like VR_SetRGBImageData, except that image pixels 
       may be n-bytes wide, with n not necessarily 1.
       Note that only 1 byte will actually be used.
    */

extern unsigned long VR_TranslateGrayPixel(VR_TransId tid,
            int grayval, int *gray_ans);
     /* 
     Translates a gray value into a pixel value, according to
     the translator tid. *gray_ans returns the actual gray
     value of the returned pixel. grayval should be in [0,255]
     */ 

extern unsigned long VR_TranslateRGBPixel(VR_TransId tid,
            int r, int g, int b,
            int *r_ans, int *g_ans, int *b_ans);
     /* 
     Translates an RGB value into a pixel value, according to
     the translator tid. *r_ans, *g_ans, *b_ans return the actual 
     color values of the returned pixel. r,g,b should each be in [0,255]
     */ 



extern VR_TransId VR_GetTranslator(VR_ImId iid, int ColorFlag);
    /* allocate a color map for the image, and return
     * a translator for that color map
     * ColorFlag is the OR of one value each from
     * the following classes: */

/* In each class, the first value is the default. ColorFlag can be
   simply set to 0 to get all the defaults */

/* Class 1: what kind of colors? */
#define VR_CFL_FORCE_DEFAULT 0x1 /* force the use of default color map */
                                 /* always enforced for FixedColor */
#define VR_CFL_TRY_DEFAULT 0x2   /* try the default map..if alloc fails, use private*/ 
                                 /* ignored for FixedColor  */ 
#define VR_CFL_PRIVATE 0x4       /* use a private color map.. fastest */
                                 /* ignored for FixedColor  */ 

/* Class 2:  in case of RGB (YCC) colors, # of bits to be used from each color */
/*  first value is for G (Y) second for R (Cb) third for B (Cr) */
/* ignored for TrueColor visuals */ 
#define VR_CCC_3x3x3 0x010
#define VR_CCC_2x2x2 0x020
#define VR_CCC_4x4x4 0x040
#define VR_CCC_5x5x5 0x080 /* very good */
#define VR_CCC_6x5x5 0x400 /* best */
#define VR_CCC_3x3x2 0x800
#define VR_CCC_4x2x2 0x100
#define VR_CCC_8x0x0 0x200 /* useful for RGB->YCC with grayscale display */
/* there is room for more here */

/* Class 3:  in case of RGB colors, should conersion to Y Cr Cb be done
   before color allocation? */
/* ignored for TrueColor visuals */ 
#define VR_YCC_ABSENT 0x1000
#define VR_YCC_PRESENT 0x2000

/* Class 4: indicates the max # of colors to be allocated */
/* ignored for FixedColor visuals */ 
/* choose a value between 1 and MaxPixel, leftshift it by 16
   and or it to the ColorFlag. Default is MaxPixel  */
/* note that this won't work for #colors > 2^16. indeed, we
   only allow > 2^16 colors for TrueColor visuals */ 
#define VR_CFL_SET_NCOLORS(cfl,n) \
   if ((n) >= VR_MaxPixel) (n) = 0; \
   (cfl) |= (((unsigned long) (n)) << 16); \
   if ((n) == 0) (n) = VR_MaxPixel
#define VR_CFL_GET_NCOLORS(cfl,n) \
   (n) = ((cfl) & ((unsigned long) 0xFFFF0000)) >> 16; \
   if (((n) < 2) || ((n) > VR_MaxPixel)) (n) = VR_MaxPixel


extern VR_TransId VR_GetTranslatorFromCM(VR_ImId iid, int ColorFlag,
		    Colormap cmap);
/* create a translator for image iid, but use the cmap */

extern VR_TransId VR_GetTranslatorFromCMofXWin(VR_ImId iid,
	int ColorFlag, Window win);
/* create a translator for image iid, but use the cmap of win */

extern void VR_FreeTransTable(VR_TransId tid);
  /* free the transtable associated with this translator..
     this translator won't be ysed again, but we are
     not ready to VR_FreeTranslator yet, as we don't want
     to XFreeColormap yet */ 

extern VR_WinId VR_GetWindow(int width, int height, int locx, int locy,
	    VR_TransId tid, int parent_given,
	    Window parent_win, int norefresh, int noq, int *trybacking);
    /* Create a new X window, associate a colormap and translator with it */
    /* if norefresh is not set, Exposure events will be gathered
       and re-displays attempted. If noq is not set, pressing the
       key `q' in the window will destroy it. If *trybacking is
       set, and the Xserver supports BackingStore, window contents
       will be preserved by Xserver. Return value in *trybacking
       indicates if it was successful. If it was, application
       may want to call VR_FreeWindowXImage(wid)
       after calling VR_ShowImage. Note that X reference says the
       server overheads for using backing stores is high, so you
       might want to use VR_'s refreshing.
       */ 

extern VR_WinId VR_SetWindow(int width, int height, int locx, int locy,
	    VR_TransId tid, Window the_win, int norefresh, int noq, int *trybacking, int retain_wattribs); 
    /* Create a new  VR_window, associate a  translator,
      and pre-existing Xwindow with it. if the cmap in the translator
      is different from that in the window, then install that cmap in 
      the window. See above for description of *trybacking */ 
    /* iff retain_wattribs is set, VR will not touch any of the
       X window's attributes ever */ 

typedef int (*VR_EventHandler) (VR_WinId wid, XEvent *event);

extern VR_ReturnCode VR_RegisterEventHandler(VR_WinId wid, int etype,
  long val, VR_EventHandler handler);

extern VR_ReturnCode VR_UnRegisterEventHandler(VR_WinId wid,
  int etype, long val);

extern void VR_FreeWindowXImage(VR_WinId wid); 
   /* free the ximage associated with wid */


extern Window VR_GetXWindow(int width, int height, 
   int locx, int locy, int parent_given,
   Window parent_win, char *name);
  /* create and map
    an Xwindow.. this is not managed by the viewer, and is
     only provided to be used as a parent window by applications,
     or in VR_SetWindow */

extern VR_ReturnCode VR_ChangeTranslator(VR_WinId wid, VR_TransId tid);


/* functions to free stuff */
extern VR_ReturnCode VR_FreeImage(VR_ImId iid);
extern VR_ReturnCode VR_FreeWindow(VR_WinId wid);
extern VR_ReturnCode VR_FreeTranslator(VR_TransId tid);

extern VR_ReturnCode VR_ShowImage(VR_ImId iid, VR_WinId wid, int dodither);
  /* display an image in a window 
   * VR_ImKind for the image must match that of the image
   * which was used to create the translator associated with that
   * window */

extern VR_ReturnCode VR_ShowImageRect(VR_ImId iid, VR_WinId wid,
	  int wherex, int wherey, int forx, int fory, int dodither);
  /* display an image in a window 
   * VR_ImKind for the image must match that of the image
   * which was used to create the translator associated with that
   * window */



extern VR_ReturnCode VR_RefreshWindow(VR_WinId wid);
  /* redisplay the image associated with the window */

extern int FindOrSetCmap(Colormap cmap, int *new_cmap, int *isReadOnly, int myCreation);

extern Display * VR_GetDisplay(void);
  /* get the display.. used for event gathering */

extern int VR_GetScreen(void);
  /* not needed usually, but just for completeness */

extern Visual * VR_GetVisual(VR_ImKind kind);
  /* not needed usually, but just for completeness */

extern void VR_XFreeColor(int cslot, unsigned long pixel);

extern Window VR_WidToWin(VR_WinId wid);
  /* get the X Window corresponding to wid */

extern VR_ReturnCode VR_HandleEvent(XEvent *event);
  /* It is the application's responsibility to set
   * up an event gathering loop such as:
   * ...
   * XEvent evt;
   * Display * disp;
   * ...
   * disp = VR_GetDisplay();
   * while (1) {
   *  XNextEvent(disp, &evt);
   *  VR_HandleEvent(&evt);
   * }
   * ...
   */

/* slightly higher level stuff for event handling */
/* the application "registers" a  bunch of
   file descriptors, and then simply calls
   VR_WaitForSomething in a loop */

extern VR_ReturnCode VR_RegisterFd(int fd);
   /* add this fd to the list on which
      VR_WaitForSomething should wait for input */

extern VR_ReturnCode VR_UnRegisterFd(int fd);
   /* delete fd from the above list */

extern int XWinIsValid(Window win, Window *root, Window *parent);
  /* do not have good code for this yet */

extern VR_ReturnCode VR_WaitForSomething(fd_set * * return_fds);
  /* Handles all xevents..
     returns one of
     VR_SUCCESS
     VR_BAD_FD
     VR_SIGNAL_OCCURRED
     In case of VR_SUCCESS, one of the registered fds is
     ready for input. *ret_fds will be set to point to
     a set of fds ready for input

     Typical use:
     ...
     fd_set *foo;
     ...
     VR_RegisterFd(0);
     ...
     while (1) {
       switch(VR_WaitForSomething(&foo)) {
	 case VR_SUCCESS:
	   scanf("....",....);
	   TakeAppropriateAction(...);
	   break;
	 default:
	   TakeAppropriateAction(...);
	   break;
       }
     }

  */

extern void VR_FreeColormap(int cslot);

extern Display * VR_TheDisplay;
extern int VR_ScreenDoesBacking; 
extern int VR_ScreenBacking; 
extern GC VR_TheGC;
extern int VR_TheScreen;
extern unsigned long VR_ForeGround, VR_BackGround;

extern int VR_Depth, VR_BitsPerPixel;
extern int VR_BppBytes, VR_BppAuxBytes, VR_BppBits;
extern int VR_MaxPixel;
extern int VR_ColorSupported;
extern int VR_FixedColors;
extern Visual * VR_TheVisual;
extern int VR_VisualClass;

extern Cursor VR_WaitCursor; 

/* if a visual is TrueColor, needed info stored as: */
struct VR_VisualInfoStruct {
  int vclass;
  VisualID Id;

  /* following entries useful only for TrueColor (we use
     DirectColor exactly like PseudoColor) */
  unsigned long red_mask;
  unsigned long green_mask;
  unsigned long blue_mask;
  int colormap_size;
  int bits_per_rgb;
  int red_shift; /* how many right-shifts needed to align MS part
		    of 8-bit value (left-shifted 24 places)
		    to MS part of mask */
  int green_shift;
  int blue_shift;
  int red_bits;
  int green_bits;
  int blue_bits;
  int least_bits;
  unsigned long least_mask;
};
typedef struct VR_VisualInfoStruct VR_VisualInfoType;

extern VR_VisualInfoType VR_VisualInfo;

struct VR_ColormapStruct {
  int inUse;
  int inUseCount; /* # translators using this colormap */ 
  int NumValid;
  int NumAllocated; /* NumValid >= NumAllocated */ 
  char isReadOnly; /* we don't mix RW and RO colorcells in a colormap */
  char myCreation;
  Colormap cmap;
  /* Colors are stored only when depth <= 16 */
  XColor *Colors;
  /* Following stored only when !VR_FixedColors */
  int *ColorUseCount; /* # windows using a colorcell */
  char *Allocated;
  char *ValidColor; /* (Allocated[i] == 1) => (ValidColor[i] == 1) */
};
typedef struct VR_ColormapStruct VR_Colormap;

extern VR_Colormap *VR_Colormaps;
extern int VR_ColormapsSize;
extern int VR_NumColormaps; 
extern int VR_DefCmapLoc;

extern Colormap VR_DefaultColormap;
extern int VR_FreeDefCmapOnExit; 


struct VR_ImageStruct {
  int inUse;
  VR_ImKind kind;
  int separateRGB; /* used when kind==VR_RGB */
  int width;
  int height;
  unsigned char * data;
  unsigned char * red;
  unsigned char * green;
  unsigned char * blue;
  int pixw1;
  int pixw2;
  int pixw3;
  char  name[VR_STRLENMAX];
};
typedef struct VR_ImageStruct VR_Image;

#define VR_IidValid(iid) \
  (((iid) >= 0) && ((iid) < VR_NumImages) && (VR_Images[(iid)].inUse))

#define VR_ResetImageSize(iid, wd, ht) \
  VR_Images[(iid)].width = (wd); \
  VR_Images[(iid)].height = (ht)

#define VR_SetImageName(iid, name) strcpy(VR_Images[(iid)].name,(name))

typedef unsigned long VR_TransEntry;

#define GRAY_TRANS_TABLE_SIZE 256
#define MAX_TRANS_TABLE_SIZE 65536

struct VR_TranslatorStruct {
  int inUse; 
  int inUseCount; /* # windows using this translator */ 
  VR_ImKind kind;

  /* following fields for use in case of kind==VR_RGB only */
  /* For converting RGB to index in TransTable: */
  int ToYCC; /* convert to YCC? */
  unsigned long Rmask;
  unsigned long Gmask;
  unsigned long Bmask;
  unsigned long Graymask;
  int Rshift; /* these are right-shifts, to be applied after the
		 value has been masked and left-shifted 24 places */
  int Gshift;
  int Bshift;
  /*** end of RGB-only-fields ****/

  int *ColorUseFlag; /* only for !FixedColors */
  int TotalColorsUsed;
  int TotalColorsRequested;
  int VR_cmap; 
  int TransTableSize;
  VR_TransEntry * TransTable; /* only !TrueColor */
};
typedef struct VR_TranslatorStruct VR_Translator;

#define VR_TransLacksTable(tid) \
  ((VR_VisualClass != TrueColor) && !VR_Translators[(tid)].TransTable)

#define VR_MAX_EVENTS 10

struct VR_WindowStruct {
  int inUse;
  int app_given;
  VR_ImKind kind;
  VR_TransId tid;
  int dont_change; 
  int width;
  int height;
  int xoff;
  int yoff; 
  XImage *ximage;
  int ximwidth;
  int ximheight;
  Window win;
  Window parwin; 
  Colormap OrigCmap; /* for app_given windows, to be reinstated
			on freeing, if has been changed */
  char  title[VR_STRLENMAX];
  int numEvents;
  int eventTypes[VR_MAX_EVENTS];
  long vals[VR_MAX_EVENTS];
  long emask;
  VR_EventHandler handler[VR_MAX_EVENTS];

};
typedef struct VR_WindowStruct VR_Window;
  

extern int VR_NumImages;
extern int VR_ImagesSize; 
extern VR_Image *VR_Images; 

extern int VR_NumTranslators;
extern int VR_TranslatorsSize; 
extern VR_Translator *VR_Translators;

extern int VR_NumWindows;
extern int VR_WindowsSize; 
extern VR_Window *VR_Windows;

#define VR_WidValid(wid) \
  (((wid) >= 0) && ((wid) < VR_NumWindows) && (VR_Windows[(wid)].inUse))

#define VR_ResetWinSize(wid, wd, ht) \
  VR_Windows[(wid)].width = (wd); \
  VR_Windows[(wid)].height = (ht)

#define VR_ResetWinLoc(wid, x, y) \
  VR_Windows[(wid)].xoff = (x); \
  VR_Windows[(wid)].yoff = (y);

#define VR_ResetWinLocX(wid, x) \
  VR_Windows[(wid)].xoff = (x);

#define VR_ResetWinLocY(wid, y) \
  VR_Windows[(wid)].yoff = (y);

extern int VR_ImageByteOrder; /* 0==LSBFirst,1==MSBFirst */

extern char VR_WindowTitle[VR_STRLENMAX]; 

#define VR_SetTitle(s) strcpy(VR_WindowTitle,(s))


extern Pixmap VR_icon_pixmap;

extern int VR_NumFds;

#define VR_MAX_FDS 20       /* maximum number of file descriptors
	                      we can deal with.. these are the
			      fds the main loop will sit with
			      a select on */

extern int VR_Fds[VR_MAX_FDS];

extern FILE * VR_errfile;

extern int VR_NonFatalError(Display * disp, XErrorEvent *err);

extern int VR_FatalError(Display * disp);



extern void VR_ApplyTrans(VR_ImId iid, unsigned char * xdata,
                   VR_TransId tid);

extern void VR_ApplyTransRect(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid, int wherex, int wherey,
		       int forx, int fory);

extern void VR_ApplyFSTrans(VR_ImId iid, unsigned char * xdata,
                   VR_TransId tid);

extern void VR_ApplyFSTransRect(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid, int wherex, int wherey,
		       int forx, int fory);

extern void VR_ShowWaitCursor(VR_WinId wid); 

extern void VR_ShowNormalCursor(VR_WinId wid); 

#endif /* VIEWER_I_INCLUDED */

