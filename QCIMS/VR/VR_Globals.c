
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

Display * VR_TheDisplay;
int VR_ScreenDoesBacking; /* Is BackingStores supported? */
int VR_ScreenBacking;     /* Am I using backing stores? */ 
GC VR_TheGC;
int VR_TheScreen;
unsigned long VR_ForeGround, VR_BackGround;

char VR_WindowTitle[VR_STRLENMAX]; /* default title of a window */ 

char VR_errmsg[VR_STRLENMAX];  /* Contains an error message after
				  any VR call returning -1 on
				  failure fails */ 
int VR_Depth, VR_BitsPerPixel;
int VR_BppBytes, VR_BppAuxBytes, VR_BppBits;
int VR_ColorSupported; /* visual != GrayScale or StaticGray */
int VR_MaxPixel;
int VR_FixedColors; /* visual = TrueColor or StaticGray or StaticColor */

Visual * VR_TheVisual;
int VR_VisualClass;
VR_VisualInfoType VR_VisualInfo;


VR_Colormap *VR_Colormaps;
int VR_ColormapsSize;
int VR_NumColormaps; 
int VR_DefCmapLoc;

Colormap VR_DefaultColormap;
int VR_FreeDefCmapOnExit; 

int VR_NumImages;
int VR_ImagesSize; 
VR_Image *VR_Images; 

int VR_NumTranslators;
int VR_TranslatorsSize; 
VR_Translator *VR_Translators;

int VR_NumWindows;
int VR_WindowsSize; 
VR_Window *VR_Windows;

int VR_ImageByteOrder;

Cursor VR_WaitCursor;


Pixmap VR_icon_pixmap;

int VR_NumFds;

int VR_Fds[VR_MAX_FDS];

FILE * VR_errfile;


