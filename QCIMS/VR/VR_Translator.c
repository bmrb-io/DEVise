
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

/******** routines to memcpy 1, 2, or 3 bytes from pointer
 to unsigned long ****************************************/

typedef void (*VR_memcpy_func)(unsigned char *tgt,
	                       unsigned long *src);

static void mcpy_1(unsigned char *tgt, unsigned long *src)
{
  *tgt = (unsigned char) (*src);
}

#ifdef VR_ULONG_MSB_FIRST

static void mcpy_2_im_msb(unsigned char *tgt, unsigned long *src)
{
  *tgt = *(((unsigned char *) src) + 2);
  *(tgt+1) = *(((unsigned char *) src) + 3);
}

static void mcpy_3_im_msb(unsigned char *tgt, unsigned long *src)
{
  *tgt = *(((unsigned char *) src) + 1);
  *(tgt+1) = *(((unsigned char *) src) + 2);
  *(tgt+2) = *(((unsigned char *) src) + 3);
}

static void mcpy_4_im_msb(unsigned char *tgt, unsigned long *src)
{
  *(tgt) = *((unsigned char *) src);
  *(tgt+1) = *(((unsigned char *) src) + 1);
  *(tgt+2) = *(((unsigned char *) src) + 2);
  *(tgt+3) = *(((unsigned char *) src) + 3);
}

static void mcpy_2_im_lsb(unsigned char *tgt, unsigned long *src)
{
  *(tgt+1) = *(((unsigned char *) src) + 2);
  *(tgt) = *(((unsigned char *) src) + 3);
}

static void mcpy_3_im_lsb(unsigned char *tgt, unsigned long *src)
{
  *(tgt+2) = *(((unsigned char *) src) + 1);
  *(tgt+1) = *(((unsigned char *) src) + 2);
  *(tgt) = *(((unsigned char *) src) + 3);
}

static void mcpy_4_im_lsb(unsigned char *tgt, unsigned long *src)
{
  *(tgt+3) = *((unsigned char *) src);
  *(tgt+2) = *(((unsigned char *) src) + 1);
  *(tgt+1) = *(((unsigned char *) src) + 2);
  *(tgt) = *(((unsigned char *) src) + 3);
}

#else /* VR_ULONG_LSB_FIRST */

static void mcpy_2_im_msb(unsigned char *tgt, unsigned long *src)
{
  *tgt = *(((unsigned char *) src) + 1);
  *(tgt + 1) = *((unsigned char *) src);
}

static void mcpy_3_im_msb(unsigned char *tgt, unsigned long *src)
{
  *tgt  = *(((unsigned char *) src) + 2);
  *(tgt+1)  = *(((unsigned char *) src) + 1);
  *(tgt+2)  = *((unsigned char *) src);
}

static void mcpy_4_im_msb(unsigned char *tgt, unsigned long *src)
{
  *tgt  = *(((unsigned char *) src) + 3);
  *(tgt+1)  = *(((unsigned char *) src) + 2);
  *(tgt+2)  = *(((unsigned char *) src) + 1);
  *(tgt+3)  = *((unsigned char *) src);
}

static void mcpy_2_im_lsb(unsigned char *tgt, unsigned long *src)
{
  *(tgt+1) = *(((unsigned char *) src) + 1);
  *(tgt) = *((unsigned char *) src);
}

static void mcpy_3_im_lsb(unsigned char *tgt, unsigned long *src)
{
  *(tgt+2)  = *(((unsigned char *) src) + 2);
  *(tgt+1)  = *(((unsigned char *) src) + 1);
  *(tgt)  = *((unsigned char *) src);
}

static void mcpy_4_im_lsb(unsigned char *tgt, unsigned long *src)
{
  *(tgt+3)  = *(((unsigned char *) src) + 3);
  *(tgt+2)  = *(((unsigned char *) src) + 2);
  *(tgt+1)  = *(((unsigned char *) src) + 1);
  *(tgt)  = *((unsigned char *) src);
}

#endif

static VR_memcpy_func memcpy_funcs[2][5] =
  {{(VR_memcpy_func) 0, &mcpy_1, &mcpy_2_im_lsb, &mcpy_3_im_lsb, &mcpy_4_im_lsb},
   {(VR_memcpy_func) 0, &mcpy_1, &mcpy_2_im_msb, &mcpy_3_im_msb, &mcpy_4_im_msb}};

/*********************************************************
  free X color, only if we had allocated it, and no
  VR_window is using it 
*********************************************************/

extern void VR_XFreeColor(int cslot, unsigned long pixel)
{
  unsigned long loc_pix = pixel;
  Colormap cmap = VR_Colormaps[cslot].cmap;

  if ((!VR_FixedColors) &&
      (!VR_Colormaps[cslot].ColorUseCount[pixel]) &&
      (VR_Colormaps[cslot].Allocated[pixel])) { 
    XFreeColors(VR_TheDisplay, cmap, &loc_pix, 1, 0); 
    VR_Colormaps[cslot].NumValid--;
    VR_Colormaps[cslot].NumAllocated--;
    VR_Colormaps[cslot].ValidColor[pixel] = 0;
    VR_Colormaps[cslot].Allocated[pixel] = 0;
  }
}

/*******************************************************************
Allocate closest matching color
grayscales will only be matched with grayscales
********************************************************************/

static int VR_XAllocColor(int cslot, XColor *color)
{
  const float err_tolerance = (float) 300.0*256.0;
  const float err_tolerance_sq = err_tolerance*err_tolerance;

  Colormap map = VR_Colormaps[cslot].cmap;
  char *Allocated = VR_Colormaps[cslot].Allocated;
  XColor *cols_in_map = VR_Colormaps[cslot].Colors;
  char *col_is_valid = VR_Colormaps[cslot].ValidColor;

  unsigned long r, g, b;

  int is_gray, retry;

  #define NUM_COLS_TO_TRY 8 
  /* after finding the best match, we will try to XAllocColor it
     This may, however, fail (for several reasons, for instance
     if the app that had alloc'ed this color has exited and
     someone else has grabbed the colormap entry; or if it's not
     a shareable color cell). In such cases we will retry, with upto
     NUM_COLS_TO_TRY closest colors. 
  */ 

  long best_index[NUM_COLS_TO_TRY];
  float best_error[NUM_COLS_TO_TRY]; 
  float err, rerr, gerr, berr; 
  long i,j,n; 

  if (VR_VisualClass == TrueColor) {
    /* should not happen */
    return 0;
  }

  is_gray = (color->red == color->green) &&
	    (color->green == color->blue);

  if (VR_FixedColors) {
    /* visual is either StaticColor or StaticGray, and
       VR_Depth is <= 16, all the colors are in cols_in_map */

    best_error[0] = err_tolerance_sq;
    best_index[0] = VR_MaxPixel;

    if (is_gray) {
      /* only match against grayscales ie, colors where r=g=b */ 

      /* find best match in colormap
      */
      for (n = 0; n < VR_MaxPixel; n++) {
        if ((cols_in_map[n].red == cols_in_map[n].green) &&
	    (cols_in_map[n].green == cols_in_map[n].blue)) {

	    err = (float) cols_in_map[n].red - color->red;
	    if (err == (float) 0) {
	      best_index[0] = n;
	      break;
            }
	    err = err*err; 
          
	    if (err < best_error[0]) {
	      /* this is the best match we have seen..  */ 
	      best_error[0] = err;
	      best_index[0] = n;

    } } }   } else {
      /* full-color */ 
  
      /* find best match in colormap
      */
      for (n = 0; n < VR_MaxPixel; n++) {
	  rerr = (float) cols_in_map[n].red - color->red;
	  gerr = (float) cols_in_map[n].green - color->green;
	  berr = (float) cols_in_map[n].blue - color->blue;
	  err = (rerr*rerr + gerr*gerr + berr*berr)/3.0;

	  if (err == (float) 0) {
	      best_index[0] = n;
	      break;
          }
          
	  if (err < best_error[0]) {
	    /* this is the best match we have seen..  */
	    best_error[0] = err;
	    best_index[0] = n;
    } }   } 

    if (best_index[0] < VR_MaxPixel) {
      memcpy(color, &cols_in_map[best_index[0]],sizeof(XColor));
      return(1);
    } else {
     return(0);
    }
  }

  /****** at this point visual is either ************
    PseudoColor or DirectColor or GrayScale,
    and VR_Depth is <= 16
  ***************************************************/

  /* find if color already exists, exactly, in the list
     of colors we allocated */

  if (VR_Colormaps[cslot].NumAllocated) {
    for (i=0;i<VR_MaxPixel;i++) {
      if (Allocated[i] &&
  	  (cols_in_map[i].red == color->red) &&
	  (cols_in_map[i].green == color->green) &&
	  (cols_in_map[i].blue == color->blue)) { 
        memcpy(color,&cols_in_map[i],sizeof(XColor));
        return(1);
  } } }

  if (VR_Colormaps[cslot].isReadOnly &&
      XAllocColor(VR_TheDisplay, map, color)) {
    if (Allocated[color->pixel]) {
      /* sigh.. X did some close-matching.. and X expects
	 colors to be freed as many times as they were
	 allocated.. 
      */ 
      XFreeColors(VR_TheDisplay, map, &color->pixel, 1, 0); 
    } else {  
      Allocated[color->pixel] = 1; 
      VR_Colormaps[cslot].NumAllocated++;
      if (!col_is_valid[color->pixel]) { 
        col_is_valid[color->pixel] = 1;
        VR_Colormaps[cslot].NumValid++;
      }
    }
    memcpy(&cols_in_map[color->pixel], color, sizeof(XColor));
    return 1; 
  }


  /*  we didn't allocate this.. check if we already have a
  **  fairly good idea of how the colors in this colormap are
  **/
  retry = 0;

  do_retry:
  if ((VR_Colormaps[cslot].NumValid < (VR_MaxPixel/2)) || retry) {
    
    /* we don't.. so get a good idea using XQueryColors */

    VR_Colormaps[cslot].NumValid = VR_MaxPixel; 
    for (i = 0; i < VR_MaxPixel; i++) {
      cols_in_map[i].pixel = (unsigned long) i;
      col_is_valid[i] = 1; 
    }

    XQueryColors(VR_TheDisplay, map, cols_in_map, VR_MaxPixel);

  } 

  for (n = 0; n < NUM_COLS_TO_TRY; n++) {
      best_index[n] = -1;
      best_error[n] = err_tolerance_sq; 
  } 

  if (is_gray) {
    /* only match against grayscales ie, colors where r=g=b */ 

    /* find best match in colormap
    */
    for (n = 0; n < VR_MaxPixel; n++) {
      if (col_is_valid[n] &&
	 (cols_in_map[n].red == cols_in_map[n].green) &&
	 (cols_in_map[n].green == cols_in_map[n].blue)) {

	  err = (float) cols_in_map[n].red - color->red;
	  err = err*err; 
          
	  if (err < best_error[NUM_COLS_TO_TRY-1]) {
	    /* this is one of the best matches we have seen..
	       find position in sorted order
	    */ 
	    i = NUM_COLS_TO_TRY-2;
	    while ((i >= 0) && (err < best_error[i])) i--;
	    i++; /* the correct position of this color is i */
	    for (j=(NUM_COLS_TO_TRY-1); j > i; j--) {
	      best_error[j] = best_error[j-1];
	      best_index[j] = best_index[j-1];
	    }
	    best_error[i] = err;
	    best_index[i] = n;

  } } } } else {
    /* full-color */ 
  
    /* find best match in colormap
    */
    for (n = 0; n < VR_MaxPixel; n++) {
      if (col_is_valid[n]) { 
	  rerr = (float) cols_in_map[n].red - color->red;
	  gerr = (float) cols_in_map[n].green - color->green;
	  berr = (float) cols_in_map[n].blue - color->blue;
	  err = (rerr*rerr + gerr*gerr + berr*berr)/3.0;

          
	  if (err < best_error[NUM_COLS_TO_TRY-1]) {
	    /* this is one of the best matches we have seen..
	       find position in sorted order
	    */
	    i = NUM_COLS_TO_TRY-2;
	    while ((i >= 0) && (err < best_error[i])) i--;
	    i++; /* the correct position of this color is i */ 
	    for (j=(NUM_COLS_TO_TRY-1); j > i; j--) {
	      best_error[j] = best_error[j-1];
	      best_index[j] = best_index[j-1];
	    }
	    best_error[i] = err;
	    best_index[i] = n;
  } } } } 

  /* we know the best NUM_COLS_TO_TRY matches for this color in the
  ** colormap. Try XAllocColor until we get one of them
  */
  for (i = 0; i < NUM_COLS_TO_TRY; i++) {
    if (best_index[i] < 0) {
      if (retry) return 0;
      else {
	retry = 1;
	goto do_retry;
      }
    } 
    if (Allocated[best_index[i]]) {
      memcpy(color,&cols_in_map[best_index[i]],sizeof(XColor));
      /* success! */
      return 1; 
    } else if (VR_Colormaps[cslot].isReadOnly) { 
      color->red = cols_in_map[best_index[i]].red; 
      color->green = cols_in_map[best_index[i]].green; 
      color->blue = cols_in_map[best_index[i]].blue; 
      if (XAllocColor(VR_TheDisplay, map, color)) { 
         if (Allocated[color->pixel]) {
           /* sigh.. X did some close-matching.. and X expects
	      colors to be freed as many times as they were
     	      allocated.. 
           */ 
           XFreeColors(VR_TheDisplay, map, &color->pixel, 1, 0); 
         } else {  
           Allocated[color->pixel] = 1; 
           VR_Colormaps[cslot].NumAllocated++;
           if (!col_is_valid[color->pixel]) { 
             col_is_valid[color->pixel] = 1;
             VR_Colormaps[cslot].NumValid++;
         } } 
         memcpy(&cols_in_map[color->pixel],color,sizeof(XColor));
         /* success! */
         return 1; 
      } else {
        VR_Colormaps[cslot].NumValid--;
        col_is_valid[best_index[i]] = 0;
  } } } 

  if (retry) return 0;
  else {
    retry = 1;
    goto do_retry;
  }
} 

/* used for private colors */
static int VR_XAllocColorCells(int cslot, unsigned long * pixels,
	     unsigned long ncolors)
{
  Colormap cmap = VR_Colormaps[cslot].cmap;

  /* !VR_FixedColors */
  if (VR_Colormaps[cslot].isReadOnly) return(0); 

  return (XAllocColorCells(VR_TheDisplay, cmap, False, NULL, 0,
			       pixels, ncolors)); 
}

/* used for private colors, when !VR_FixedColors */
/* VR_XAllocColorCells must have been successful */ 
static void VR_XStoreColor(int cslot, XColor *col)
{
  Colormap cmap = VR_Colormaps[cslot].cmap;
  XStoreColor(VR_TheDisplay, cmap, col);
  VR_Colormaps[cslot].Allocated[col->pixel] = 1;
  VR_Colormaps[cslot].NumAllocated++; 
  if (!VR_Colormaps[cslot].ValidColor[col->pixel]) {
    memcpy(&VR_Colormaps[cslot].Colors[col->pixel],col,sizeof(XColor));
    VR_Colormaps[cslot].ValidColor[col->pixel] = 1;
    VR_Colormaps[cslot].NumValid++;
  }
}

/* translate RGB values to TransTable index */


#define RGBTrioToIndex( r, g, b, tptr ) \
  (((((unsigned long)  ((r) & (tptr)->Rmask)) << 24) >> (tptr)->Rshift)  | \
   ((((unsigned long)  ((g) & (tptr)->Gmask)) << 24) >> (tptr)->Gshift)   | \
   ((((unsigned long)  ((b) & (tptr)->Bmask)) << 24) >> (tptr)->Bshift))

static unsigned long VR__cv_temp;

#define GrayToGrayIndex( g, tptr ) \
  (((VR__cv_temp = \
    (((unsigned long) ((g) & (tptr)->Graymask)) << 24)) \
		>> (tptr)->Rshift)  | \
   (VR__cv_temp >> (tptr)->Gshift) | \
   (VR__cv_temp >> (tptr)->Bshift))


/* Translate TransTable index to R, G, B */

#define IndexToR( idx, tptr ) \
  ((((unsigned long) (idx) << (tptr)->Rshift) >> 24) & (tptr)->Rmask)

#define IndexToG( idx, tptr ) \
  ((((unsigned long) (idx) << (tptr)->Gshift) >> 24) & (tptr)->Gmask)

#define IndexToB( idx, tptr ) \
  ((((unsigned long) (idx) << (tptr)->Bshift) >> 24) & (tptr)->Bmask)

#define GrayIndexToGray( idx, tptr) \
  ((((unsigned long) (idx) << (tptr)->Rshift) >> 24) & (tptr)->Graymask)

static int Color_Square_Table[512];

static int Color_Square_Table_Set = 0;

static void Set_Color_Square_Table(void) 
{
  int i;

  for (i=0;i<256;i++) {
    Color_Square_Table[256-i] =
    Color_Square_Table[256+i] = i*i;
  }
  Color_Square_Table_Set = 1;
}


#define RGBDist(r1,g1,b1,r2,g2,b2) \
  (Color_Square_Table[256 + (r1) -(r2)] + \
   Color_Square_Table[256 + (g1) -(g2)] + \
   Color_Square_Table[256 + (b1) -(b2)])


static unsigned char RGBtoY(unsigned char r, unsigned char g, unsigned char b)
{
  float fy;
  int iy;

  fy = ((float) r) * 0.299;
  fy += ((float) g) * 0.587;
  fy += ((float) b) * 0.114;

  iy = ((int) ((float) fy + 0.5));
  if (iy < 0) iy = 0;
  if (iy > 255) iy = 255;
  return((unsigned char) iy);
}

static unsigned char RGBtoCr(unsigned char r, unsigned char g, unsigned char b)
{
  float fy;
  int iy;

  fy = ((float) r) * 0.5;
  fy -= ((float) g) * 0.41869;
  fy -= ((float) b) * 0.08131;

  iy = ((int) ((float) fy + 128.5));
  if (iy < 0) iy = 0;
  if (iy > 255) iy = 255;
  return((unsigned char) iy);
}


static unsigned char RGBtoCb(unsigned char r, unsigned char g, unsigned char b)
{
  float fy;
  int iy;

  fy = ((float) b) * 0.5;
  fy -= ((float) r) * 0.16784;
  fy -= ((float) g) * 0.33126;

  iy = ((int) ((float) fy + 128.5));
  if (iy < 0) iy = 0;
  if (iy > 255) iy = 255;
  return((unsigned char) iy);
}

static unsigned char YCCtoR(unsigned char y, unsigned char cr, unsigned char cb)
{
  float fc;
  int ic;

  fc = (float) y;
  fc += ((float) cr - 128) * 1.402;
  ic = ((int) ((float) fc + 0.5));
  if (ic < 0) ic = 0;
  if (ic > 255) ic = 255;
  return((unsigned char) ic);
}

static unsigned char YCCtoG(unsigned char y, unsigned char cr, unsigned char cb)
{
  float fc;
  int ic;

  fc = (float) y;
  fc -= ((float) cb - 128) * 0.34414;
  fc -= ((float) cr - 128) * 0.71414;

  ic = ((int) ((float) fc + 0.5));
  if (ic < 0) ic = 0;
  if (ic > 255) ic = 255;
  return((unsigned char) ic);
}

static unsigned char YCCtoB(unsigned char y, unsigned char cr, unsigned char cb)
{
  float fc;
  int ic;

  fc = (float) y;
  fc += ((float) cb - 128) * 1.772;
  ic = ((int) ((float) fc + 0.5));
  if (ic < 0) ic = 0;
  if (ic > 255) ic = 255;
  return((unsigned char) ic);
}

/* from the pixel data (sz pixels), get histogram of gray values
   into the array cts.
   return number of grayscales used in *tot
*/
static void GetGrayCounts(unsigned char * data, int pixw,
      unsigned long sz, unsigned long * cts, unsigned long *tot)
{
  int i;
  unsigned char *loc_data = data;
  for (i=0;i<256;i++) cts[i] = 0;
  for (i=0;i<sz;i++) {
    cts[*loc_data]++;
    loc_data += pixw;
  }
  *tot = 0;
  for (i=0;i<256;i++) if (cts[i]) (*tot)++;
}

/* used for qsort */
struct sorterstruct {
    unsigned long val;
    unsigned long where;
};
typedef struct sorterstruct sorter;

static int my_compare(const void *ina, const void *inb)
{
  sorter * a = (sorter *) ina;
  sorter * b = (sorter *) inb;
  int diff;
  diff = (int) b->val - (int) a->val;
  return(diff);
}

/* sort grayscales in allocation order. cts is the
** histogram produced by GetGrayCounts. allocation order
** is boringly tedious.. but know this: sorted[i], for
** 0 <= i < out_sz, will be the order in which you
** try to XAllocColor
*/
static void SortGrayCounts(unsigned long * cts, 
  unsigned long * sorted,
    unsigned long out_sz)
{
  
  int i,j;
  sorter  cts_sorter[256];
  int done[256], dist[256];
  int maxdist, where, tdist, last_from_cts_sorter;

  for (i=0;i<256;i++) {
      cts_sorter[i].val = cts[i];
      cts_sorter[i].where = i;
  }
  qsort((void *) cts_sorter, 256, sizeof(sorter), my_compare);

  sorted[0] = cts_sorter[0].where;


  dist[cts_sorter[0].where] = 0;
  done[cts_sorter[0].where] = 1;
  last_from_cts_sorter = 0;

  for (i=1;i<256;i++) {
    done[cts_sorter[i].where] = 0;
    dist[cts_sorter[i].where] =
      cts_sorter[i].where - cts_sorter[0].where;
    if (dist[cts_sorter[i].where] < 0)
      dist[cts_sorter[i].where] = 0 - dist[cts_sorter[i].where];
  } 

  for (i=1;i<out_sz;i++) {
    if ((i & 1) || (i < 10)) {
      /* take color with max dist */
      maxdist = 0;
      where = 0;
      for (j=0;j<256;j++) {
	if (!done[j] && (dist[j] > maxdist)) {
	  maxdist = dist[j];
	  where = j;
	}
      }
    } else {
      while (done[cts_sorter[++last_from_cts_sorter].where]);
      where = cts_sorter[last_from_cts_sorter].where;
    }
    done[where] = 1;
    sorted[i] = where;
    for (j=0;j<256;j++) {
      if (!done[j]) {
	tdist = j - where;
	if (tdist < 0) tdist = 0 - tdist;
	if (tdist < dist[j]) dist[j] = tdist;
      }
    }
  }

}


/* Get counts of RGB colors (masked by tptr->R/G/Bmask
** into cts from data,
** where data has sz * 3 bytes, in the order RGBRGB..
** return # of colors into *tot
** if tptr says ToYCC then do RGB to YCC
*/
static void GetAllColorsFromTrails( unsigned  long * cts,
    unsigned char * data,
    int pixw1, int pixw2, int pixw3,
    unsigned long sz,
    unsigned long * tot, VR_Translator *tptr)
{
  unsigned long i, where;
  unsigned char r, g, b, y, cr, cb, * dptr;

  memset(cts, 0, tptr->TransTableSize*sizeof(unsigned long));


  dptr = data;

  if (tptr->ToYCC) {
    for (i=0;i<sz;i++) {
      r = (*dptr); dptr += pixw1;
      g = (*dptr); dptr += pixw2;
      b = (*dptr); dptr += pixw3;
      y = RGBtoY(r,g,b);
      cr = RGBtoCr(r,g,b);
      cb = RGBtoCb(r,g,b);
      where = RGBTrioToIndex(cb,y,cr,tptr);
      cts[where]++;
    }
  }
  else {
    for (i=0;i<sz;i++) {
      r = (*dptr); dptr += pixw1;
      g = (*dptr); dptr += pixw2;
      b = (*dptr); dptr += pixw3;
      where = RGBTrioToIndex(r,g,b,tptr);
      cts[where]++;
    }
  }

  *tot = 0;
  for (i=0;i<tptr->TransTableSize;i++)  if (cts[i]) (*tot)++;

}

/* Get counts of RGB colors (masked by tptr->R/G/Bmask
** into cts from red, green, blue, where each has sz bytes.
** return # of colors into *tot
** if tptr says DoYCC then do RGB to YCC
*/
static void GetAllColorsFromTrios( unsigned  long * cts,
                           unsigned char * red,
			   unsigned char * green,
			   unsigned char * blue,
                           int pixw1, int pixw2, int pixw3,
			   unsigned long sz,
    unsigned long * tot, VR_Translator *tptr)
{

  unsigned long i, where;
  unsigned char r, g, b, y, cr, cb, * rptr, * gptr, *bptr;

  memset(cts, 0, tptr->TransTableSize*sizeof(unsigned long));

  rptr = red;
  gptr = green;
  bptr = blue;

  if (tptr->ToYCC) {
    for (i=0;i<sz;i++) {
      r = (*rptr); rptr += pixw1;
      g = (*gptr); gptr += pixw2;
      b = (*bptr); bptr += pixw3;
      y = RGBtoY(r,g,b);
      cr = RGBtoCr(r,g,b);
      cb = RGBtoCb(r,g,b);
      where = RGBTrioToIndex(cb,y,cr,tptr);
      cts[where]++;
    }
   }
   else {
    for (i=0;i<sz;i++) {
      r = (*rptr); rptr += pixw1;
      g = (*gptr); gptr += pixw2;
      b = (*bptr); bptr += pixw3;
      where = RGBTrioToIndex(r,g,b,tptr);
      cts[where]++;
    }
   }

  *tot = 0;
  for (i=0;i<tptr->TransTableSize;i++)  if (cts[i]) (*tot)++;
  
}



extern int FindOrSetCmap(Colormap cmap, int *new_cmap, int *isReadOnly, int myCreation)
{
  unsigned long i, j;
  VR_Colormap * temp;

  *new_cmap = 0;
  for (i=0; i< VR_ColormapsSize; i++) {
    if (VR_Colormaps[i].inUse &&
	VR_Colormaps[i].cmap == cmap) {
	  *isReadOnly = VR_Colormaps[i].isReadOnly; 
	  return(i);
    } 
  }

  if (VR_NumColormaps == VR_ColormapsSize) {
    if ((temp = (VR_Colormap *) 
       calloc(1,sizeof(VR_Colormap)*(2*VR_ColormapsSize))) == NULL) {
	 strcpy(VR_errmsg,"FindOrSetCmap: out of memory");
         return(-1);
    }
    memcpy(temp,VR_Colormaps,sizeof(VR_Colormap)*VR_ColormapsSize);
    free(VR_Colormaps);
    VR_Colormaps = temp;
    VR_ColormapsSize *= 2; 
  }

  for (i=0; i< VR_ColormapsSize; i++) {
    if (!VR_Colormaps[i].inUse) {
      VR_Colormaps[i].cmap = cmap; 
      VR_Colormaps[i].myCreation = myCreation; 
      VR_Colormaps[i].isReadOnly = *isReadOnly;
      VR_Colormaps[i].ColorUseCount = 0;
      VR_Colormaps[i].ValidColor = 0;
      VR_Colormaps[i].Allocated = 0;
      VR_Colormaps[i].Colors = 0;
      VR_Colormaps[i].NumValid = 0;
      VR_Colormaps[i].NumAllocated = 0;
      if (!VR_FixedColors) {
	/** need to alloc Allocated ColorUseCount ValidColor Colors ***/
	VR_Colormaps[i].ColorUseCount =
	  (int *) calloc(1,sizeof(int)*VR_MaxPixel);
	if (!VR_Colormaps[i].ColorUseCount) {
	  strcpy(VR_errmsg,"FindOrSetCmap: out of memory");
	  return(-1);
	}
	VR_Colormaps[i].ValidColor =
	  (char *) calloc(1,sizeof(char)*VR_MaxPixel);
	if (!VR_Colormaps[i].ValidColor) {
	  free(VR_Colormaps[i].ColorUseCount);
	  strcpy(VR_errmsg,"FindOrSetCmap: out of memory");
	  return(-1);
	}
	VR_Colormaps[i].Allocated =
	  (char *) calloc(1,sizeof(char)*VR_MaxPixel);
	if (!VR_Colormaps[i].Allocated) {
	  free(VR_Colormaps[i].ColorUseCount);
	  free(VR_Colormaps[i].ValidColor);
	  strcpy(VR_errmsg,"FindOrSetCmap: out of memory");
	  return(-1);
	}
	VR_Colormaps[i].Colors =
	  (XColor *) malloc(sizeof(XColor)*VR_MaxPixel);
	if (!VR_Colormaps[i].Colors) {
	  free(VR_Colormaps[i].ColorUseCount);
	  free(VR_Colormaps[i].ValidColor);
	  free(VR_Colormaps[i].Colors);
	  strcpy(VR_errmsg,"FindOrSetCmap: out of memory");
	  return(-1);
	}
      } else if (VR_VisualClass != TrueColor) {
	/** need to alloc and set Colors ***/
	VR_Colormaps[i].Colors =
	  (XColor *) malloc(sizeof(XColor)*VR_MaxPixel);
	if (!VR_Colormaps[i].Colors) {
	  strcpy(VR_errmsg,"FindOrSetCmap: out of memory");
	  return(-1);
	}
        for (j = 0; j < VR_MaxPixel; j++) {
          VR_Colormaps[i].Colors[j].pixel = (unsigned long) j;
        }
        XQueryColors(VR_TheDisplay, cmap,
	   VR_Colormaps[i].Colors, VR_MaxPixel);
      }
      *new_cmap = 1;
      VR_Colormaps[i].inUse = 1;
      VR_Colormaps[i].inUseCount = 0;
      VR_NumColormaps++;
      return(i); 
    }
  }

  /* should not come here */
  return(-1);
} 

extern void VR_FreeTransTable(VR_TransId tid)
{

  if (!VR_Translators[tid].inUse) return; 
  if (VR_Translators[tid].TransTable)
    free(VR_Translators[tid].TransTable);
  VR_Translators[tid].TransTable = 0;
}

extern VR_ReturnCode VR_FreeTranslator(VR_TransId tid)
{
  int cslot, i;

  if (!VR_Translators[tid].inUse) return(VR_SUCCESS);

  cslot = VR_Translators[tid].VR_cmap; 
  if (--VR_Colormaps[cslot].inUseCount == 0) { 
    VR_FreeColormap(cslot);
  }

  if (VR_Translators[tid].TransTable)
    free(VR_Translators[tid].TransTable);
  if (VR_Translators[tid].ColorUseFlag)
    free(VR_Translators[tid].ColorUseFlag);

  VR_Translators[tid].TransTable = 0;
  VR_Translators[tid].ColorUseFlag = 0;

  VR_Translators[tid].inUse = 0;
  VR_Translators[tid].inUseCount = 0; 
  VR_NumTranslators--;

  for (i=0;i<VR_WindowsSize;i++) {
    if (VR_Windows[i].inUse && (VR_Windows[i].tid == tid))
      VR_FreeWindow(i); 
  } 

  return(VR_SUCCESS);
}


static void BitCopyCompact(unsigned char *src,
	    unsigned char *tgt, int bits_in_unit,
	    int num_units,
	    int tgt_byte_offset, int tgt_bit_offset)
{
  /* count bit- and byte-offsets from left */
  static const unsigned long bit_clearer[8] =
    {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
  static const unsigned long bit_setter[8] =
    {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
  register int curr_bit, src_bit;
  int src_unit, src_bit_start;
  int src_byte;
  int frac_byte_num;
  unsigned char *curr_src, *curr_tgt;
  int full_bytes_in_unit, rem_bits_in_unit;


  full_bytes_in_unit = bits_in_unit/8;
  rem_bits_in_unit = bits_in_unit - (full_bytes_in_unit * 8);

  if (rem_bits_in_unit > 0) {
    full_bytes_in_unit++;
  } else {
    rem_bits_in_unit = 8;
  }

  if (VR_ImageByteOrder == 0) frac_byte_num = (full_bytes_in_unit - 1); /*LSBFirst*/
  else frac_byte_num = 0;

  curr_tgt = tgt + tgt_byte_offset;
  curr_bit = tgt_bit_offset;

  curr_src = src;

  for (src_unit=0; src_unit < num_units; src_unit++) {
    
    for (src_byte = 0; src_byte < full_bytes_in_unit; src_byte++) {

      src_bit_start = 8 - ((src_byte==frac_byte_num)?rem_bits_in_unit:8);

      for (src_bit = src_bit_start; src_bit < 8; src_bit++) {
        /* clear the target bit */
        (*curr_tgt) &= bit_clearer[curr_bit];
        /* set if needed */
        if ((*curr_src) & bit_setter[src_bit]) {
	  (*curr_tgt) |= bit_setter[curr_bit];
        }
        curr_bit++;
        if (curr_bit==8) {
	  curr_bit=0;
	  curr_tgt++;
      } }
      curr_src++;
  } }
}

extern void VR_ApplyTrans(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid)
{

  VR_ApplyTransRect(iid, xdata, tid, 0, 0,
		      VR_Images[iid].width, VR_Images[iid].height);
}


static void VR_ApplyTransRectInner(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid, int wherex, int wherey,
		       int forx, int fory)
{
  unsigned long i,j, size, index;
  long temp, tempsrc, tempsrc1, tempsrc2, tempsrc3;
  long offset, roffset, goffset, boffset;
  unsigned char *src, *rsrc, *gsrc, *bsrc, *tgt;
  unsigned char r, g, b, y, cr, cb;
  VR_Translator * tptr;
  VR_memcpy_func do_memcpy = memcpy_funcs[VR_ImageByteOrder][VR_BppAuxBytes];
  int pixw1 = VR_Images[iid].pixw1;
  int pixw2 = VR_Images[iid].pixw2;
  int pixw3 = VR_Images[iid].pixw3;


  /* assume all ids are valid, and transtable has not been free'd */

  tptr = &VR_Translators[tid];

  if (VR_VisualClass == TrueColor) {

    if (VR_Images[iid].kind==VR_Gray) {

      temp = wherey*VR_Images[iid].width + wherex;

      src = VR_Images[iid].data + (temp*pixw1);
      tgt = xdata + (temp*VR_BppAuxBytes);

      temp = VR_Images[iid].width - forx;
      tempsrc = temp*pixw1; /* end-of-row increment for src */
      temp *= VR_BppAuxBytes; /* end-of-row increment for tgt */

      switch(VR_BppAuxBytes) {
      case 1:
        /* optimized: avoids call to do_memcpy */
        for (i=0;i<fory;i++) {
	  for (j=0;j<forx;j++) {
	    r = *src; src += pixw1;
	    *tgt++ = (unsigned char) GrayToGrayIndex(r,tptr);
	  }
	  tgt += temp;
	  src += tempsrc;
        }
        break;
      case 2:
      case 3:
      case 4:
        for (i=0;i<fory;i++) {
	  for (j=0;j<forx;j++) {
	    r = *src; src += pixw1;
	    index = GrayToGrayIndex(r,tptr);
	    do_memcpy(tgt, &index);
	    tgt += VR_BppAuxBytes;
	  }
	  tgt += temp;
	  src += tempsrc;
        }
        break;
      }
    }
    else if (VR_Images[iid].kind==VR_RGB) {
      /* note that for TrueColor, ToYCC is False */

      temp = wherey*VR_Images[iid].width + wherex;
      tgt = xdata + (temp*VR_BppAuxBytes);

      if (VR_Images[iid].separateRGB) {
        rsrc = VR_Images[iid].red + (temp*pixw1);
        gsrc = VR_Images[iid].green + (temp*pixw2);
        bsrc = VR_Images[iid].blue + (temp*pixw3);
        roffset = pixw1;
        goffset = pixw2;
        boffset = pixw3;
      } else {
        offset = pixw1 + pixw2 + pixw3;
        rsrc = VR_Images[iid].data + (offset*temp);
        gsrc = rsrc + pixw1;
        bsrc = gsrc + pixw2;
	roffset = 
	goffset = 
	boffset = offset;
      }
      
      temp = VR_Images[iid].width - forx;
      tempsrc1 = temp*roffset; /* end-of-row increment for rsrc */
      tempsrc2 = temp*goffset; /* end-of-row increment for gsrc */
      tempsrc3 = temp*boffset; /* end-of-row increment for bsrc */
      temp *= VR_BppAuxBytes; /* end-of-row increment for tgt */

      switch(VR_BppAuxBytes) {
      case 1:
	/* optimized: does not call do_memcpy */
        for (i=0;i<fory;i++) {
          for (j=0;j<forx;j++) {
	      *tgt++ = (unsigned char) 
		RGBTrioToIndex((*rsrc),(*gsrc),(*bsrc),tptr);
	      rsrc += roffset;
	      gsrc += goffset;
	      bsrc += boffset;
	  }
	  rsrc += tempsrc1;
	  gsrc += tempsrc2;
	  bsrc += tempsrc3;
	  tgt += temp;
        }
        break;
      case 2:
      case 3:
      case 4:
        for (i=0;i<fory;i++) {
          for (j=0;j<forx;j++) {
	      index = RGBTrioToIndex((*rsrc),(*gsrc),(*bsrc),tptr);
	      do_memcpy(tgt, &index);
	      tgt += VR_BppAuxBytes;
	      rsrc += roffset;
	      gsrc += goffset;
	      bsrc += boffset;
	  }
	  rsrc += tempsrc1;
	  gsrc += tempsrc2;
	  bsrc += tempsrc3;
	  tgt += temp;
        } 
        break;
      }
    }
    return;
  }

  /********* visual is not TrueColor, so TransTable needs to be used **/

  if (VR_Images[iid].kind==VR_Gray) {

    temp = wherey*VR_Images[iid].width + wherex;

    src = VR_Images[iid].data + (temp*pixw1);
    tgt = xdata + (temp*VR_BppAuxBytes);

    temp = VR_Images[iid].width - forx;
    tempsrc = temp*pixw1; /* end-of-row increment for src */
    temp *= VR_BppAuxBytes; /* end-of-row increment for tgt */

    switch(VR_BppAuxBytes) {
    case 1:
      /* optimized: avoids call to do_memcpy */
      for (i=0;i<fory;i++) {
	for (j=0;j<forx;j++) {
	  *tgt++ = (unsigned char) 
	    VR_Translators[tid].TransTable[*src];
	  src += pixw1;
	}
	tgt += temp;
	src += tempsrc;
      }
      break;
    case 2:
    case 3:
    case 4:
      for (i=0;i<fory;i++) {
	for (j=0;j<forx;j++) {
	  do_memcpy(tgt, &VR_Translators[tid].TransTable[*src]);
	  tgt += VR_BppAuxBytes;
	  src += pixw1;
	}
	tgt += temp;
	src += tempsrc;
      }
      break;
    }
  }
  else if (VR_Images[iid].kind==VR_RGB) {

    temp = wherey*VR_Images[iid].width + wherex;

    tgt = xdata + (temp*VR_BppAuxBytes);

    if (VR_Images[iid].separateRGB) {
        rsrc = VR_Images[iid].red + (temp*pixw1);
        gsrc = VR_Images[iid].green + (temp*pixw2);
        bsrc = VR_Images[iid].blue + (temp*pixw3);
        roffset = pixw1;
        goffset = pixw2;
        boffset = pixw3;
    } else {
        offset = pixw1 + pixw2 + pixw3;
        rsrc = VR_Images[iid].data + (offset*temp);
        gsrc = rsrc + pixw1;
        bsrc = gsrc + pixw2;
	roffset = 
	goffset = 
	boffset = offset;
    }
      
    temp = VR_Images[iid].width - forx;
    tempsrc1 = temp*roffset; /* end-of-row increment for rsrc */
    tempsrc2 = temp*goffset; /* end-of-row increment for gsrc */
    tempsrc3 = temp*boffset; /* end-of-row increment for bsrc */
    temp *= VR_BppAuxBytes; /* end-of-row increment for tgt */

    switch(VR_BppAuxBytes) {
    case 1:
      /* optimized: no calls to do_memcpy */
      if (tptr->ToYCC) {
        for (i=0;i<fory;i++) {
          for (j=0;j<forx;j++) {
  	    r =  (*rsrc);
	    g =  (*gsrc);
	    b =  (*bsrc);
	    y  = RGBtoY(r,g,b);
	    cr = RGBtoCr(r,g,b);
	    cb = RGBtoCb(r,g,b);
	    index = RGBTrioToIndex(cb,y,cr,tptr);
	    *tgt++ = (unsigned char)
	      VR_Translators[tid].TransTable[index];
	    rsrc += roffset;
	    gsrc += goffset;
	    bsrc += boffset;
	  }
	  rsrc += tempsrc1;
	  gsrc += tempsrc2;
	  bsrc += tempsrc3;
	  tgt += temp;
	}
      }
      else {
        for (i=0;i<fory;i++) {
          for (j=0;j<forx;j++) {
	    index = RGBTrioToIndex((*rsrc),(*gsrc),(*bsrc),tptr);
	    *tgt++ = (unsigned char)
	      VR_Translators[tid].TransTable[index];
	    rsrc += roffset;
	    gsrc += goffset;
	    bsrc += boffset;
	  }
	  rsrc += tempsrc1;
	  gsrc += tempsrc2;
	  bsrc += tempsrc3;
	  tgt += temp;
      } }
      break;
    case 2:
    case 3:
    case 4:
      if (tptr->ToYCC) {
        for (i=0;i<fory;i++) {
          for (j=0;j<forx;j++) {
  	    r =  (*rsrc);
	    g =  (*gsrc);
	    b =  (*bsrc);
	    y  = RGBtoY(r,g,b);
	    cr = RGBtoCr(r,g,b);
	    cb = RGBtoCb(r,g,b);
	    index = RGBTrioToIndex(cb,y,cr,tptr);
	    do_memcpy(tgt, &VR_Translators[tid].TransTable[index]);
	    tgt += VR_BppAuxBytes;
	    rsrc += roffset;
	    gsrc += goffset;
	    bsrc += boffset;
	  }
	  rsrc += tempsrc1;
	  gsrc += tempsrc2;
	  bsrc += tempsrc3;
	  tgt += temp;
	}
      }
      else {
        for (i=0;i<fory;i++) {
          for (j=0;j<forx;j++) {
	    index = RGBTrioToIndex((*rsrc),(*gsrc),(*bsrc),tptr);
	    do_memcpy(tgt, &VR_Translators[tid].TransTable[index]);
	    tgt += VR_BppAuxBytes;
	    rsrc += roffset;
	    gsrc += goffset;
	    bsrc += boffset;
	  }
	  rsrc += tempsrc1;
	  gsrc += tempsrc2;
	  bsrc += tempsrc3;
	  tgt += temp;
      } }
      break;
    }
  } 

}

extern void VR_ApplyTransRect(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid, int wherex, int wherey,
		       int forx, int fory)
{
  unsigned char * xdata_buffer, *tgt;
  long i, wherey_loc, bytes_per_row, bits_per_row;
  long byte_offset, bit_offset, bits_in_row, tgt_decr;

  if (VR_BppBits == 0) {
    VR_ApplyTransRectInner(iid, xdata, tid, wherex, wherey, forx, fory);
  } else {
    /* get one row of pixel values into xdata_buffer,
       then transfer it to xdata compactly */
    xdata_buffer = (unsigned char *) calloc(1, forx*VR_BppAuxBytes);
    if (!xdata_buffer) return;
    tgt = xdata_buffer -
      (((wherey*VR_Images[iid].width) + wherex)*VR_BppAuxBytes);

    bits_per_row = VR_Images[iid].width*VR_BitsPerPixel;
    bytes_per_row = bits_per_row/8;
    if ((bytes_per_row*8) < bits_per_row) bytes_per_row++;
    
    bits_in_row = wherex*VR_BitsPerPixel;
    byte_offset = bits_in_row/8;
    bit_offset = bits_in_row - (byte_offset*8);
    byte_offset += (bytes_per_row*wherey);
    tgt_decr = VR_Images[iid].width*VR_BppAuxBytes;

    for (i = 0, wherey_loc = wherey; i < fory;
	 i++, wherey_loc++, byte_offset += bytes_per_row,
	 tgt -= tgt_decr) {
      VR_ApplyTransRectInner(iid, tgt, tid,
	     wherex, wherey_loc, forx, 1);
      BitCopyCompact(xdata_buffer, xdata, VR_BitsPerPixel, forx,
	    byte_offset, bit_offset);
    }

    free(xdata_buffer);
  }
}

/**** Floyd-Steinberg dithering with ApplyTrans and ApplyTransRect ****/

typedef short FSERROR;          /* 16 bits should be enough */
typedef int LOCFSERROR;         /* use 'int' for calculation temps */

typedef FSERROR *FSERRPTR;

static FSERRPTR fserrors;    /* accumulated-errors array */
static int * error_limiter = (int *) 0;
                             /* table for clamping the applied error */

static void init_fs_dither(void)
/* Allocate and fill in the error_limiter table */
{
  int * table;
  int in, out;

  if (error_limiter) return;

  table = (int *) malloc((size_t) ((255*2+1) * sizeof(int)));
  if (! table) return;

  table += 255;         /* so can index -255 .. +255 */
  error_limiter = table;

#define STEPSIZE ((255+1)/16)
  /* Map errors 1:1 up to +- 255/16 */
  out = 0;
  for (in = 0; in < STEPSIZE; in++, out++) {
    table[in] = out; table[-in] = -out;
  }
  /* Map errors 1:2 up to +- 3*255/16 */
  for (; in < STEPSIZE*3; in++, out += (in&1) ? 0 : 1) {
    table[in] = out; table[-in] = -out;
  }
  /* Clamp the rest to final out value (which is (255+1)/8) */
  for (; in <= 255; in++) {
    table[in] = out; table[-in] = -out;
  }
#undef STEPSIZE
}




extern void VR_ApplyFSTrans(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid)
{
  VR_ApplyFSTransRect(iid, xdata, tid, 0, 0,
		      VR_Images[iid].width, VR_Images[iid].height);
}


#define RANGE(a,b,c) \
  if ((a) < (b)) (a) = (b); if ((a) > (c)) (a) = (c)

static void DoDitherGray(unsigned char * src, int pixw1,
			 unsigned char *tgt,
			 int width, int height,
			 int iwidth, VR_Translator * tptr)
{
  register LOCFSERROR cur0; /* current error or pixel value */
  LOCFSERROR belowerr0; /* error for pixel below cur */
  LOCFSERROR bpreverr0; /* error for below/prev col */
  register FSERRPTR errorptr;   /* => fserrors[] at column before current */
  int dir;                      /* +1 or -1 depending on direction */
  int i,row, col;
  int on_odd_row; 
  int cslot = tptr->VR_cmap;
  unsigned char *in, *outptr;
  FSERRPTR fserrors;
  VR_memcpy_func do_memcpy = memcpy_funcs[VR_ImageByteOrder][VR_BppAuxBytes];

  init_fs_dither(); /* fill error_limiter table */


  on_odd_row = 0;

  if ((fserrors = (FSERRPTR) calloc(1, sizeof(FSERROR)*(width+2)))==NULL)
    return; /* sigh */

  for (row = 0; row < height; row++) {
    
    in = src + (iwidth*row*pixw1);
    outptr = tgt + (iwidth*row*VR_BppAuxBytes);

    if (on_odd_row) {
      /* work right to left in this row */
      in += ((width-1)*pixw1); /* go to rightmostpixel */
      outptr += ((width-1)*VR_BppAuxBytes);
      dir = -1;
      errorptr = fserrors + (width+1); /* => entry after last column */
      on_odd_row = 0;    /* flip for next time */
    } else {
      /* work left to right in this row */
      dir = 1;
      errorptr = fserrors;   /* => entry before first real column */
      on_odd_row = 1;     /* flip for next time */
    }
    /* Preset error values: no error propagated to first pixel from left */
    cur0 = 0;
    /* and no error propagated to row below yet */
    belowerr0 = 0;
    bpreverr0 = 0;

    for (col = width; col > 0; col--) {
      cur0 = (cur0 + errorptr[dir] + 8) >> 4;
      cur0 = error_limiter[cur0];
      cur0 += *in;
      RANGE(cur0, 0, 255);


      /* Now emit the colormap index for this cell */
      { unsigned long pixcode;
	if (VR_VisualClass == TrueColor) {
	  pixcode = (unsigned long) GrayToGrayIndex(cur0, tptr);
	  cur0 -= (int) GrayIndexToGray( pixcode, tptr);
	} else {
	  pixcode = tptr->TransTable[cur0];
	  cur0 -= (int) (VR_Colormaps[cslot].Colors[pixcode].red >> 8);
	}
	do_memcpy(outptr, &pixcode);
        /* cur0 now has the representation error for this pixel */
      }
      /* Compute error fractions to be propagated to adjacent pixels.
       * Add these into the running sums, and simultaneously shift the
       * next-line error sums left by 1 column.
       */
      { register LOCFSERROR bnexterr, delta;

        bnexterr = cur0;        /* Process component 0 */
        delta = cur0 * 2;
        cur0 += delta;          /* form error * 3 */
        errorptr[0] = (FSERROR) (bpreverr0 + cur0);
        cur0 += delta;          /* form error * 5 */
        bpreverr0 = belowerr0 + cur0;
        belowerr0 = bnexterr;
        cur0 += delta;          /* form error * 7 */
      }
      /* At this point cur0 contains the 7/16 error value to be propagated
       * to the next pixel on the current line, and all the errors for the
       * next line have been shifted over.  We are therefore ready to move on.
       */
      in += (dir*pixw1); 
      outptr += (dir*VR_BppAuxBytes);
      errorptr += dir;         /* advance errorptr to current column */
    }
    /* Post-loop cleanup: we must unload the final error values into the
     * final fserrors[] entry.  Note we need not unload belowerrN because
     * it is for the dummy column before or after the actual array.
     */
    errorptr[0] = (FSERROR) bpreverr0; /* unload prev errs into array */
  }


  free(fserrors);
}


static void DoDitherRGB(unsigned char * rsrc,
			unsigned char * gsrc,
			unsigned char * bsrc,
			unsigned char * tgt,
			int width, int height,
			int rincr, int gincr, int bincr,
			int iwidth, VR_Translator * tptr)
{
  register LOCFSERROR cur0, cur1, cur2; /* current error or pixel value */
  LOCFSERROR belowerr0, belowerr1, belowerr2; /* error for pixel below cur */
  LOCFSERROR bpreverr0, bpreverr1, bpreverr2; /* error for below/prev col */
  register FSERRPTR errorptr;   /* => fserrors[] at column before current */
  int dir;                      /* +1 or -1 depending on direction */
  int dir3;                     /* 3*dir, for advancing errorptr */
  int i,row, col;
  int on_odd_row; 
  unsigned long idx; 
  long t_offset;
  int cslot = tptr->VR_cmap;
  unsigned char *rin, *gin, *bin, *outptr;
  FSERRPTR fserrors;
  VR_memcpy_func do_memcpy = memcpy_funcs[VR_ImageByteOrder][VR_BppAuxBytes];

  init_fs_dither(); /* fill error_limiter table */


  on_odd_row = 0;

  if ((fserrors = (FSERRPTR) calloc(1, sizeof(FSERROR)*(width+2)*3))==NULL)
    return; /* sigh */

  for (row = 0; row < height; row++) {
    
    t_offset = iwidth*row;
    rin    = rsrc + (t_offset*rincr);
    gin    = gsrc + (t_offset*gincr);
    bin    = bsrc + (t_offset*bincr);
    outptr = tgt  + (t_offset*VR_BppAuxBytes);

    if (on_odd_row) {
      /* work right to left in this row */
      t_offset = width-1;
      rin    += (t_offset*rincr); /* go to rightmostpixel */
      gin    += (t_offset*gincr);
      bin    += (t_offset*bincr);
      outptr += (t_offset*VR_BppAuxBytes);
      dir = -1;
      dir3 = -3;
      errorptr = fserrors + (width+1)*3; /* => entry after last column */
      on_odd_row = 0;    /* flip for next time */
    } else {
      /* work left to right in this row */
      dir = 1;
      dir3 = 3;
      errorptr = fserrors;   /* => entry before first real column */
      on_odd_row = 1;     /* flip for next time */
    }
    /* Preset error values: no error propagated to first pixel from left */
    cur0 = cur1 = cur2 = 0;
    /* and no error propagated to row below yet */
    belowerr0 = belowerr1 = belowerr2 = 0;
    bpreverr0 = bpreverr1 = bpreverr2 = 0;

    for (col = width; col > 0; col--) {
      cur0 = (cur0 + errorptr[dir3+0] + 8) >> 4;
      cur1 = (cur1 + errorptr[dir3+1] + 8) >> 4;
      cur2 = (cur2 + errorptr[dir3+2] + 8) >> 4;
      cur0 = error_limiter[cur0];
      cur1 = error_limiter[cur1];
      cur2 = error_limiter[cur2];
      cur0 += *rin;
      cur1 += *gin;
      cur2 += *bin;
      RANGE(cur0, 0, 255);
      RANGE(cur1, 0, 255);
      RANGE(cur2, 0, 255);

      /* Find value of pixel */
      idx = RGBTrioToIndex(cur0,cur1,cur2,tptr);

      /* Now emit the colormap index for this cell */
      { unsigned long pixcode;
	if (VR_VisualClass == TrueColor) {
	  pixcode = idx;
	  cur0 -= (int) IndexToR( idx, tptr);
	  cur1 -= (int) IndexToG( idx, tptr);
	  cur2 -= (int) IndexToB( idx, tptr);
	} else {
	  pixcode = tptr->TransTable[idx];
	  cur0 -= (int) (VR_Colormaps[cslot].Colors[pixcode].red >> 8);
	  cur1 -= (int) (VR_Colormaps[cslot].Colors[pixcode].green >> 8);
	  cur2 -= (int) (VR_Colormaps[cslot].Colors[pixcode].blue >> 8);
	}
	do_memcpy(outptr, &pixcode);
      }
      /* Compute error fractions to be propagated to adjacent pixels.
       * Add these into the running sums, and simultaneously shift the
       * next-line error sums left by 1 column.
       */
      { register LOCFSERROR bnexterr, delta;

        bnexterr = cur0;        /* Process component 0 */
        delta = cur0 * 2;
        cur0 += delta;          /* form error * 3 */
        errorptr[0] = (FSERROR) (bpreverr0 + cur0);
        cur0 += delta;          /* form error * 5 */
        bpreverr0 = belowerr0 + cur0;
        belowerr0 = bnexterr;
        cur0 += delta;          /* form error * 7 */
        bnexterr = cur1;        /* Process component 1 */
        delta = cur1 * 2;
        cur1 += delta;          /* form error * 3 */
        errorptr[1] = (FSERROR) (bpreverr1 + cur1);
        cur1 += delta;          /* form error * 5 */
        bpreverr1 = belowerr1 + cur1;
        belowerr1 = bnexterr;
        cur1 += delta;          /* form error * 7 */
        bnexterr = cur2;        /* Process component 2 */
        delta = cur2 * 2;
        cur2 += delta;          /* form error * 3 */
        errorptr[2] = (FSERROR) (bpreverr2 + cur2);
        cur2 += delta;          /* form error * 5 */
        bpreverr2 = belowerr2 + cur2;
        belowerr2 = bnexterr;
        cur2 += delta;          /* form error * 7 */
      }
      /* At this point curN contains the 7/16 error value to be propagated
       * to the next pixel on the current line, and all the errors for the
       * next line have been shifted over.  We are therefore ready to move on.
       */
      rin    += (dir*rincr); 
      gin    += (dir*gincr);
      bin    += (dir*bincr);  /* Advance pixel pointers to next column */
      outptr += (dir*VR_BppAuxBytes);
      errorptr += dir3;         /* advance errorptr to current column */
    }
    /* Post-loop cleanup: we must unload the final error values into the
     * final fserrors[] entry.  Note we need not unload belowerrN because
     * it is for the dummy column before or after the actual array.
     */
    errorptr[0] = (FSERROR) bpreverr0; /* unload prev errs into array */
    errorptr[1] = (FSERROR) bpreverr1;
    errorptr[2] = (FSERROR) bpreverr2;
  }


  free(fserrors);
}

static void DoDitherYCC(unsigned char * rsrc,
			unsigned char * gsrc,
			unsigned char * bsrc,
			unsigned char * tgt,
			int width, int height,
			int rincr, int gincr, int bincr,
			int iwidth, VR_Translator * tptr)
{
  register LOCFSERROR cur0, cur1, cur2; /* current error or pixel value */
  LOCFSERROR belowerr0, belowerr1, belowerr2; /* error for pixel below cur */
  LOCFSERROR bpreverr0, bpreverr1, bpreverr2; /* error for below/prev col */
  register FSERRPTR errorptr;   /* => fserrors[] at column before current */
  int dir;                      /* +1 or -1 depending on direction */
  int dir3;                     /* 3*dir, for advancing errorptr */
  int i,row, col;
  int on_odd_row; 
  unsigned long idx;
  long t_offset;
  int cslot = tptr->VR_cmap;
  unsigned char *rin, *gin, *bin, *outptr;
  unsigned char r, g, b, y, cb, cr;
  FSERRPTR fserrors;
  VR_memcpy_func do_memcpy = memcpy_funcs[VR_ImageByteOrder][VR_BppAuxBytes];

  init_fs_dither(); /* fill error_limiter table */


  on_odd_row = 0;

  if ((fserrors = (FSERRPTR) calloc(1, sizeof(FSERROR)*(width+2)*3))==NULL)
    return; /* sigh */

  for (row = 0; row < height; row++) {
    
    t_offset = iwidth*row;
    rin    = rsrc + (t_offset*rincr);
    gin    = gsrc + (t_offset*gincr);
    bin    = bsrc + (t_offset*bincr);
    outptr = tgt  + (t_offset*VR_BppAuxBytes);

    if (on_odd_row) {
      /* work right to left in this row */
      t_offset = width-1;
      rin    += (t_offset*rincr); /* go to rightmostpixel */
      gin    += (t_offset*gincr);
      bin    += (t_offset*bincr);
      outptr += (t_offset*VR_BppAuxBytes);
      dir = -1;
      dir3 = -3;
      errorptr = fserrors + (width+1)*3; /* => entry after last column */
      on_odd_row = 0;    /* flip for next time */
    } else {
      /* work left to right in this row */
      dir = 1;
      dir3 = 3;
      errorptr = fserrors;   /* => entry before first real column */
      on_odd_row = 1;     /* flip for next time */
    }
    /* Preset error values: no error propagated to first pixel from left */
    cur0 = cur1 = cur2 = 0;
    /* and no error propagated to row below yet */
    belowerr0 = belowerr1 = belowerr2 = 0;
    bpreverr0 = bpreverr1 = bpreverr2 = 0;

    for (col = width; col > 0; col--) {
      cur0 = (cur0 + errorptr[dir3+0] + 8) >> 4;
      cur1 = (cur1 + errorptr[dir3+1] + 8) >> 4;
      cur2 = (cur2 + errorptr[dir3+2] + 8) >> 4;
      cur0 = error_limiter[cur0];
      cur1 = error_limiter[cur1];
      cur2 = error_limiter[cur2];
      r = *rin;
      g = *gin;
      b = *bin;
      y = RGBtoY(r,g,b);
      cur0 += (int) y;
      cb = RGBtoCb(r,g,b);
      cur1 += (int) cb;
      cr = RGBtoCr(r,g,b);
      cur2 += (int) cr;
      RANGE(cur0, 0, 255);
      RANGE(cur1, 0, 255);
      RANGE(cur2, 0, 255);

      /* Find value of pixel */
      idx = RGBTrioToIndex(cur1,cur0,cur2,tptr);

      /* Now emit the colormap index for this cell */
      /* note that for YCC, visual is *not* TrueColor */
      { unsigned long pixcode = tptr->TransTable[idx];
	r = VR_Colormaps[cslot].Colors[pixcode].red >> 8;
	g = VR_Colormaps[cslot].Colors[pixcode].green >> 8;
	b = VR_Colormaps[cslot].Colors[pixcode].blue >> 8;
        y = RGBtoY(r,g,b);
        cur0 -= (int) y;
        cb = RGBtoCb(r,g,b);
        cur1 -= (int) cb;
        cr = RGBtoCr(r,g,b);
        cur2 -= (int) cr;
	do_memcpy(outptr, &pixcode);
      }
      /* Compute error fractions to be propagated to adjacent pixels.
       * Add these into the running sums, and simultaneously shift the
       * next-line error sums left by 1 column.
       */
      { register LOCFSERROR bnexterr, delta;

        bnexterr = cur0;        /* Process component 0 */
        delta = cur0 * 2;
        cur0 += delta;          /* form error * 3 */
        errorptr[0] = (FSERROR) (bpreverr0 + cur0);
        cur0 += delta;          /* form error * 5 */
        bpreverr0 = belowerr0 + cur0;
        belowerr0 = bnexterr;
        cur0 += delta;          /* form error * 7 */
        bnexterr = cur1;        /* Process component 1 */
        delta = cur1 * 2;
        cur1 += delta;          /* form error * 3 */
        errorptr[1] = (FSERROR) (bpreverr1 + cur1);
        cur1 += delta;          /* form error * 5 */
        bpreverr1 = belowerr1 + cur1;
        belowerr1 = bnexterr;
        cur1 += delta;          /* form error * 7 */
        bnexterr = cur2;        /* Process component 2 */
        delta = cur2 * 2;
        cur2 += delta;          /* form error * 3 */
        errorptr[2] = (FSERROR) (bpreverr2 + cur2);
        cur2 += delta;          /* form error * 5 */
        bpreverr2 = belowerr2 + cur2;
        belowerr2 = bnexterr;
        cur2 += delta;          /* form error * 7 */
      }
      /* At this point curN contains the 7/16 error value to be propagated
       * to the next pixel on the current line, and all the errors for the
       * next line have been shifted over.  We are therefore ready to move on.
       */
      rin    += (dir*rincr); 
      gin    += (dir*gincr);
      bin    += (dir*bincr);   /* Advance pixel pointers to next column */
      outptr += (dir*VR_BppAuxBytes);
      errorptr += dir3;         /* advance errorptr to current column */
    }
    /* Post-loop cleanup: we must unload the final error values into the
     * final fserrors[] entry.  Note we need not unload belowerrN because
     * it is for the dummy column before or after the actual array.
     */
    errorptr[0] = (FSERROR) bpreverr0; /* unload prev errs into array */
    errorptr[1] = (FSERROR) bpreverr1;
    errorptr[2] = (FSERROR) bpreverr2;
  }


  free(fserrors);
}



static void VR_ApplyFSTransRectInner(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid, int wherex, int wherey,
		       int forx, int fory)
{
  unsigned long i,j, size, index, temp;
  unsigned char *src, *rsrc, *gsrc, *bsrc, *tgt;
  int offset, roffset, goffset, boffset;
  unsigned char r, g, b, y, cr, cb;
  VR_Translator * tptr;

  /* assume tid is valid, and transtable has not been free'd */


  if (VR_Images[iid].kind==VR_Gray) {
    temp = wherey*VR_Images[iid].width + wherex;
    src = VR_Images[iid].data + (temp*VR_Images[iid].pixw1);
    tgt = xdata + (temp*VR_BppAuxBytes);

    DoDitherGray(src, VR_Images[iid].pixw1,
	tgt, forx, fory, VR_Images[iid].width,
	&VR_Translators[tid]);

  } else if (VR_Images[iid].kind==VR_RGB) {

    tptr = &VR_Translators[tid];

    temp = wherey*VR_Images[iid].width + wherex;

    if (VR_Images[iid].separateRGB) {
      rsrc = VR_Images[iid].red + (temp*VR_Images[iid].pixw1);
      gsrc = VR_Images[iid].green + (temp*VR_Images[iid].pixw2);
      bsrc = VR_Images[iid].blue + (temp*VR_Images[iid].pixw3);
      roffset = VR_Images[iid].pixw1;
      goffset = VR_Images[iid].pixw2;
      boffset = VR_Images[iid].pixw3;
    } else {
      offset = VR_Images[iid].pixw1 +
	       VR_Images[iid].pixw2 +
	       VR_Images[iid].pixw3;
      rsrc = VR_Images[iid].data + (offset*temp);
      gsrc = rsrc + VR_Images[iid].pixw1;
      bsrc = gsrc + VR_Images[iid].pixw2;
      roffset = 
      goffset = 
      boffset = offset;
    }

    tgt = xdata + (temp*VR_BppAuxBytes);

    if (tptr->ToYCC) {
      DoDitherYCC(rsrc, gsrc, bsrc, tgt, forx, fory,
		  roffset, goffset, boffset, VR_Images[iid].width, tptr);
    }
    else {
      DoDitherRGB(rsrc, gsrc, bsrc, tgt, forx, fory,
		  roffset, goffset, boffset, VR_Images[iid].width, tptr);
  } }

}


extern void VR_ApplyFSTransRect(VR_ImId iid, unsigned char * xdata,
		       VR_TransId tid, int wherex, int wherey,
		       int forx, int fory)
{
  unsigned char * xdata_buffer, *tgt, *src;
  int i, j, wherey_loc, bytes_per_row, bits_per_row;
  int byte_offset, bit_offset, bits_in_row;
  long tgt_decr, src_incr;
  int good_dither_ht = 5;
  int num_loops, remaining, rows_in_loop;

  if (VR_BppBits == 0) {
    VR_ApplyFSTransRectInner(iid, xdata, tid, wherex, wherey, forx, fory);

  } else {
    /* get good_dither_ht rows of pixel values into xdata_buffer,
       then transfer it to xdata compactly */

    if (fory < good_dither_ht) good_dither_ht = fory;
    xdata_buffer = (unsigned char *)
      calloc(1, good_dither_ht*VR_BppAuxBytes*VR_Images[iid].width);
    if (!xdata_buffer) return;
    tgt = xdata_buffer -
      (((wherey*VR_Images[iid].width) + wherex)*VR_BppAuxBytes);

    num_loops = fory/good_dither_ht;
    remaining = fory - (num_loops*good_dither_ht);
    if (remaining > 0) num_loops++;
    else remaining = good_dither_ht;

    bits_per_row = VR_Images[iid].width*VR_BitsPerPixel;
    bytes_per_row = bits_per_row/8;
    if ((bytes_per_row*8) < bits_per_row) bytes_per_row++;
    
    bits_in_row = wherex*VR_BitsPerPixel;
    byte_offset = bits_in_row/8;
    bit_offset = bits_in_row - (byte_offset*8);
    byte_offset += (bytes_per_row*wherey);
    tgt_decr = good_dither_ht*VR_Images[iid].width*VR_BppAuxBytes;
    src_incr = VR_Images[iid].width*VR_BppAuxBytes;

    for (i = 0, wherey_loc = wherey; i < num_loops;
	 i++, wherey_loc += good_dither_ht,
	 tgt -= tgt_decr) {
      rows_in_loop = ((i == (num_loops-1)) ? remaining :
		      good_dither_ht);
      VR_ApplyFSTransRectInner(iid, tgt, tid,
	     wherex, wherey_loc, forx, rows_in_loop);
      src = xdata_buffer;
      for (j=0; j<rows_in_loop;
	    j++,src += src_incr, byte_offset += bytes_per_row) {
        BitCopyCompact(src,
	    xdata, VR_BitsPerPixel, forx,
	    byte_offset, bit_offset);
      }
    }

    free(xdata_buffer);
  }
}


static void SetShiftsAndMasks(VR_Translator * tptr,
			      unsigned long CCC_kind)
{
    if (VR_VisualClass == TrueColor) {
	tptr->TransTableSize = 0;
	tptr->Graymask = VR_VisualInfo.least_mask;
	tptr->Rmask = VR_VisualInfo.red_mask;
	tptr->Gmask = VR_VisualInfo.green_mask;
	tptr->Bmask = VR_VisualInfo.blue_mask;
	tptr->Rshift = VR_VisualInfo.red_shift;
	tptr->Gshift = VR_VisualInfo.green_shift;
	tptr->Bshift = VR_VisualInfo.blue_shift;
	return;
    }

    switch (CCC_kind) {
      case VR_CCC_2x2x2:
	tptr->TransTableSize = 64;
	tptr->Graymask =
	tptr->Rmask = 
	tptr->Gmask = 
	tptr->Bmask = 0xc0;
	tptr->Rshift = 28;
	tptr->Gshift = 26;
	tptr->Bshift = 30;
        break;
      case VR_CCC_4x4x4:
	tptr->TransTableSize = 4096;
	tptr->Graymask =
	tptr->Rmask = 
	tptr->Gmask = 
	tptr->Bmask = 0xf0;
	tptr->Rshift = 24;
	tptr->Gshift = 20;
	tptr->Bshift = 28;
        break;
      case VR_CCC_5x5x5:
	tptr->TransTableSize = 32768;
	tptr->Graymask =
	tptr->Rmask = 
	tptr->Gmask = 
	tptr->Bmask = 0xf8;
	tptr->Rshift = 22;
	tptr->Gshift = 17;
	tptr->Bshift = 27;
        break;
      case VR_CCC_6x5x5:
	tptr->TransTableSize = 65536;
	tptr->Graymask = 0xf8;
	tptr->Rmask = 0xf8; 
	tptr->Gmask = 0xfc;
	tptr->Bmask = 0xf8;
	tptr->Rshift = 22;
	tptr->Gshift = 16;
	tptr->Bshift = 27;
	break;
      case VR_CCC_4x2x2:
	tptr->TransTableSize = 256;
	tptr->Graymask = 0xc0;
	tptr->Rmask = 0xc0;
	tptr->Gmask = 0xf0;
	tptr->Bmask = 0xc0;
	tptr->Rshift = 28;
	tptr->Gshift = 24;
	tptr->Bshift = 30;
        break;
      case VR_CCC_8x0x0:
	tptr->TransTableSize = 256;
	tptr->Graymask = 0xff;
	tptr->Rmask = 0x00;
	tptr->Gmask = 0xff;
	tptr->Bmask = 0x00;
	tptr->Rshift = 0;
	tptr->Gshift = 24;
	tptr->Bshift = 0;
        break;
      case VR_CCC_3x3x2:
	tptr->TransTableSize = 256;
	tptr->Graymask = 0xc0;
	tptr->Rmask = 0xe0; 
	tptr->Gmask = 0xe0; 
	tptr->Bmask = 0xc0;
	tptr->Rshift = 27;
	tptr->Gshift = 24;
	tptr->Bshift = 30;
        break;
      case VR_CCC_3x3x3:
      default:
	tptr->TransTableSize = 512;
	tptr->Graymask = 
	tptr->Rmask = 
	tptr->Gmask = 
	tptr->Bmask = 0xe0;
	tptr->Rshift = 26;
	tptr->Gshift = 23;
	tptr->Bshift = 29;
        break;
    }
}

struct RepColorStruct {
  /* representative colors for this box */
  unsigned long green;
  unsigned long red;
  unsigned long blue;
};
typedef struct RepColorStruct RepColor;

struct BoxStruct {
  /* representative colors for this box */
  unsigned long green;
  unsigned long red;
  unsigned long blue;
  /* min and max limits (inclusive) on g r and b for this box */
  unsigned long glow, ghigh;
  unsigned long rlow, rhigh;
  unsigned long blow, bhigh;
  unsigned long volume;
  unsigned long count;
};
typedef struct BoxStruct ColorBox;

#define R_SCALE 2
#define G_SCALE 3
#define B_SCALE 1

static int HIST_R_BITS;
static int HIST_G_BITS;
static int HIST_B_BITS;

static int HIST_R_SHIFT;
static int HIST_G_SHIFT;
static int HIST_B_SHIFT;

#define IndexFromShifted(r,g,b,tptr) \
  (((((unsigned long) r) << (24 + HIST_R_SHIFT)) >> tptr->Rshift) | \
   ((((unsigned long) g) << (24 + HIST_G_SHIFT)) >> tptr->Gshift) | \
   ((((unsigned long) b) << (24 + HIST_B_SHIFT)) >> tptr->Bshift))


#define INVALID_COLOR_VAL 257

/**********************
** shrink the min/max to include only nonzero counts
** recalculate volume and population
***********************/
static void
UpdateBox(ColorBox *box, unsigned long * RGBCounts,
	  VR_Translator * tptr)
{
  unsigned long glow, ghigh, rlow, rhigh, blow, bhigh, r, g, b;
  unsigned long idx;
  unsigned long rspan, gspan, bspan;

  glow = box->glow; ghigh = box->ghigh;
  rlow = box->rlow; rhigh = box->rhigh;
  blow = box->blow; bhigh = box->bhigh;

  if (ghigh > glow) {
    for (g=glow;g<=ghigh;g++) {
      for (r=rlow;r<=rhigh;r++) {
        idx = IndexFromShifted(r,g,blow,tptr);
        /* we'll increase idx as b increases.. this assumes
	   that b is stored last in the index! */
        for (b=blow;b<=bhigh;b++,idx++) {
	  if (RGBCounts[idx]) {
	    glow = box->glow = g;
	    goto have_glow;
  } } } } }

  have_glow:
  if (ghigh > glow) {
    for (g=ghigh;g>=glow;g--) {
      for (r=rlow;r<=rhigh;r++) {
        idx = IndexFromShifted(r,g,blow,tptr);
        for (b=blow;b<=bhigh;b++,idx++) {
  	  if (RGBCounts[idx]) {
	    ghigh = box->ghigh = g;
	    goto have_ghigh;
  } } } } }

  have_ghigh:
  if (rhigh > rlow) {
    for (r=rlow;r<=rhigh;r++) {
      for (g=glow;g<=ghigh;g++) {
        idx = IndexFromShifted(r,g,blow,tptr);
        for (b=blow;b<=bhigh;b++,idx++) {
  	  if (RGBCounts[idx]) {
	    rlow = box->rlow = r;
	    goto have_rlow;
  } } } } }

  have_rlow:
  if (rhigh > rlow) {
    for (r=rhigh;r>=rlow;r--) {
      for (g=glow;g<=ghigh;g++) {
        idx = IndexFromShifted(r,g,blow,tptr);
        for (b=blow;b<=bhigh;b++,idx++) {
  	  if (RGBCounts[idx]) {
	    rhigh = box->rhigh = r;
	    goto have_rhigh;
  } } } } }

  have_rhigh:
  if (bhigh > blow) {
    for (b=blow;b<=bhigh;b++) {
      for (g=glow;g<=ghigh;g++) {
        for (r=rlow;r<=rhigh;r++) {
          idx = IndexFromShifted(r,g,b,tptr);
  	  if (RGBCounts[idx]) {
	    blow = box->blow = b;
	    goto have_blow;
  } } } } }

  have_blow:
  if (bhigh > blow) {
    for (b=bhigh;b>=blow;b--) {
      for (g=glow;g<=ghigh;g++) {
        for (r=rlow;r<=rhigh;r++) {
          idx = IndexFromShifted(r,g,b,tptr);
  	  if (RGBCounts[idx]) {
	    bhigh = box->bhigh = b;
	    goto have_bhigh;
  } } } } }

  have_bhigh:

  /* calculate volume */
  rspan = ((rhigh - rlow) << HIST_R_SHIFT) * R_SCALE;
  gspan = ((ghigh - glow) << HIST_G_SHIFT) * G_SCALE;
  bspan = ((bhigh - blow) << HIST_B_SHIFT) * B_SCALE;

  box->volume = rspan*rspan + gspan*gspan + bspan*bspan;

  /* calculate population */
  box->count = 0;
  for (g=glow;g<=ghigh;g++) {
    for (r=rlow;r<=rhigh;r++) {
      idx = IndexFromShifted(r,g,blow,tptr);
      for (b=blow;b<=bhigh;b++,idx++) {
	  if (RGBCounts[idx]) box->count++;
  } } }

}


/* median cut code lifted, mostly from IJG's jquant2.c */
/* That code, btw is (c) 1991-1994 Thomas G. Lane */

static ColorBox *
BiggestPopulation(ColorBox * ColorBoxes, int NumBoxes)
{
  register ColorBox * bptr;
  register long i;
  register long maxpop = 0;
  ColorBox * ans = (ColorBox *) 0;

  for (i=0, bptr = ColorBoxes; i < NumBoxes; i++, bptr++) {
    if ((bptr->count > maxpop) && (bptr->volume > 0)) {
      ans = bptr;
      maxpop = bptr->count;
    }
  }
  return(ans);

}


static ColorBox *
BiggestVolume(ColorBox * ColorBoxes, int NumBoxes)
{
  register ColorBox * bptr;
  register long i;
  register unsigned long maxvol = 0;
  ColorBox * ans = (ColorBox *) 0;

  for (i=0, bptr = ColorBoxes; i < NumBoxes; i++, bptr++) {
    if (bptr->volume > maxvol) {
      ans = bptr;
      maxvol = bptr->volume;
    }
  }
  return(ans);

}

static void ComputeRepColor(ColorBox * box,
   unsigned long *RGBCounts, VR_Translator *tptr)
{
  unsigned long glow, ghigh, rlow, rhigh, blow, bhigh, r, g, b;
  long count;
  long tot = 0;
  long rtot = 0, gtot = 0, btot = 0;
  unsigned long idx;

  glow = box->glow; ghigh = box->ghigh;
  rlow = box->rlow; rhigh = box->rhigh;
  blow = box->blow; bhigh = box->bhigh;

  for (g=glow;g<=ghigh;g++) {
    for (r=rlow;r<=rhigh;r++) {
      idx = IndexFromShifted(r,g,blow,tptr); 
      for (b=blow;b<=bhigh;b++,idx++) {
	if ((count = RGBCounts[idx]) != 0) {
	  tot += count;
	  rtot += ((r << HIST_R_SHIFT) + ((1<<HIST_R_SHIFT)>>1)) * count;
	  gtot += ((g << HIST_G_SHIFT) + ((1<<HIST_G_SHIFT)>>1)) * count;
	  btot += ((b << HIST_B_SHIFT) + ((1<<HIST_B_SHIFT)>>1)) * count;
  } } } }
  
  box->red = ((rtot + (tot>>1)) / tot);
  box->green = ((gtot + (tot>>1)) / tot);
  box->blue = ((btot + (tot>>1)) / tot);
  box->count = tot;

}

static int
MedianCut(ColorBox *ColorBoxes,
	  long NumBoxes,
	  unsigned long total,
	  unsigned long *RGBCounts,  VR_Translator *tptr)
{
  long n, r, g, b, lb, cmax;
  register ColorBox *b1, * b2;

  while (NumBoxes < total) {

    if (NumBoxes*2 <= total) {
      b1 = BiggestPopulation(ColorBoxes, NumBoxes);
    } else {
      b1 = BiggestVolume(ColorBoxes, NumBoxes);
    }
    if (b1 == NULL)             /* no splittable boxes left! */
      break;
    b2 = &ColorBoxes[NumBoxes];    /* where new box will go */
    /* Copy the color bounds to the new box. */
    b2->glow = b1->glow; b2->ghigh = b1->ghigh;
    b2->rlow = b1->rlow; b2->rhigh = b1->rhigh;
    b2->blow = b1->blow; b2->bhigh = b1->bhigh;

    r = ((b1->rhigh - b1->rlow) << HIST_R_SHIFT) * R_SCALE;
    g = ((b1->ghigh - b1->glow) << HIST_G_SHIFT) * G_SCALE;
    b = ((b1->bhigh - b1->blow) << HIST_B_SHIFT) * B_SCALE;
    /* We want to break any ties in favor of green, then red, blue last.
     */

    /* n==0: g selected
       n==1: r selected
       n==2: b selected */

    cmax = g; n = 0;
    if (r > cmax) { cmax = r; n = 1; }
    if (b > cmax) { n = 2; }

    /* Choose split point along selected axis, and update box bounds.
     * Current algorithm: split at halfway point.
     * (Since the box has been shrunk to minimum volume,
     * any split will produce two nonempty subboxes.)
     * Note that lb value is max for lower box, so must be < old max.
     */
    switch (n) {
    case 0:
      lb = (b1->ghigh + b1->glow) / 2;
      b1->ghigh = lb;
      b2->glow = lb+1;
      break;
    case 1:
      lb = (b1->rhigh + b1->rlow) / 2;
      b1->rhigh = lb;
      b2->rlow = lb+1;
      break;
    case 2:
      lb = (b1->bhigh + b1->blow) / 2;
      b1->bhigh = lb;
      b2->blow = lb+1;
      break;
    }
    /* Update stats for boxes */
    UpdateBox(b1, RGBCounts, tptr);
    UpdateBox(b2, RGBCounts, tptr);
    NumBoxes++;
  }
  return NumBoxes;


}


/***************************************************
** RGBCounts[idx] is count of color given by idx
** On return, RGBCounts[idx] will be the quantized color
** to use for idx
** exactly total quantized colors will be obtained
** the value of the quantized color determines its order
** in trying to allocate
*/
static int SortRGBCounts(unsigned long *RGBCounts,
	    RepColor *RGBquantized,
	    VR_Translator *tptr,
	    unsigned  long * orig_total)
{
  ColorBox *ColorBoxes;
  long i,j,NumBoxes;
  unsigned long mask, r, g, b, idx;
  register ColorBox *box;
  sorter  *cts_sorter;
  unsigned long *dist;
  unsigned char *done;
  long *sorted;
  long *inv_sorted;
  long maxdist, where, tdist, last_from_cts_sorter;
  unsigned long total = *orig_total;

  /* allocate memory */
  ColorBoxes = (ColorBox *) malloc(sizeof(ColorBox)*total);
  cts_sorter = (sorter *) malloc(sizeof(sorter)*total);
  dist       = (unsigned long *) malloc(sizeof(unsigned long)*total);
  done       = (unsigned char *) malloc(sizeof(unsigned char)*total);
  sorted     = (long *) malloc(sizeof(long)*total);
  inv_sorted = (long *) malloc(sizeof(long)*total);

  if (!ColorBoxes || !cts_sorter || !dist || !done ||
      !sorted || !inv_sorted) {
    if (ColorBoxes) free(ColorBoxes);
    if (cts_sorter) free(cts_sorter);
    if (dist) free(dist);
    if (done) free(done);
    if (sorted) free(sorted);
    if (inv_sorted) free(inv_sorted);
    strcpy(VR_errmsg,"SortRGBCounts: out of memory");
    return(0);
  }


  if (tptr->TransTableSize == total) {
    /* Boxification is easy */
    for (i=0;i<tptr->TransTableSize;i++) {
      ColorBoxes[i].green = IndexToG(i, tptr);
      ColorBoxes[i].red = IndexToR(i, tptr);
      ColorBoxes[i].blue = IndexToB(i, tptr);
      ColorBoxes[i].count = RGBCounts[i];
    }
    NumBoxes = total;
  } else {
    /* total < tptr->TransTableSize */
    /******
    ** This code follows IJG's jquant2.c / xv's xv24to8.c
    ** both of which are based upon Heckbert's color quantization
    *******/

    mask = tptr->Rmask;
    HIST_R_SHIFT = 0;
    while (!(mask & 1) && (HIST_R_SHIFT<8)) {HIST_R_SHIFT++;mask >>= 1;}
    HIST_R_BITS = 8 - HIST_R_SHIFT;
    mask = tptr->Gmask;
    HIST_G_SHIFT = 0;
    while (!(mask & 1) && (HIST_G_SHIFT<8)) {HIST_G_SHIFT++;mask >>= 1;}
    HIST_G_BITS = 8 - HIST_G_SHIFT;
    mask = tptr->Bmask;
    HIST_B_SHIFT = 0;
    while (!(mask & 1) && (HIST_B_SHIFT<8)) {HIST_B_SHIFT++;mask >>= 1;}
    HIST_B_BITS = 8 - HIST_B_SHIFT;

    NumBoxes = 1;
    ColorBoxes[0].glow = 0;
    ColorBoxes[0].ghigh = 0xFF >> HIST_G_SHIFT;
    ColorBoxes[0].rlow = 0;
    ColorBoxes[0].rhigh = 0xFF >> HIST_R_SHIFT;
    ColorBoxes[0].blow = 0;
    ColorBoxes[0].bhigh = 0xFF >> HIST_B_SHIFT;

    UpdateBox(&ColorBoxes[0], RGBCounts, tptr);

    NumBoxes = MedianCut(ColorBoxes, NumBoxes, total, RGBCounts, tptr);

    total = *orig_total = NumBoxes; /* can't hurt */

    for (i=0;i < NumBoxes; i++) ComputeRepColor(&ColorBoxes[i], RGBCounts, tptr);

  }

  /********* Sort them **************/

  for (i=0;i<total;i++) {
      cts_sorter[i].val = ColorBoxes[i].count;
      cts_sorter[i].where = i;
  }
  qsort((void *) cts_sorter, total, sizeof(sorter), my_compare);

  sorted[0] = cts_sorter[0].where;
  dist[cts_sorter[0].where] = 0;
  done[cts_sorter[0].where] = 1;
  last_from_cts_sorter = 0;

  for (i=1;i<total;i++) {
    done[cts_sorter[i].where] = 0;
    dist[cts_sorter[i].where] =
      RGBDist(ColorBoxes[cts_sorter[i].where].red,
	      ColorBoxes[cts_sorter[i].where].green,
	      ColorBoxes[cts_sorter[i].where].blue,
              ColorBoxes[cts_sorter[0].where].red,
	      ColorBoxes[cts_sorter[0].where].green,
	      ColorBoxes[cts_sorter[0].where].blue);
  } 

  for (i=1;i<total;i++) {
    if ((i & 1) || (i < 10)) {
      /* take color with max dist */
      maxdist = 0;
      where = 0;
      for (j=0;j<total;j++) {
	if (!done[j] && (dist[j] > maxdist)) {
	  maxdist = dist[j];
	  where = j;
	}
      }
    } else {
      while (done[cts_sorter[++last_from_cts_sorter].where]);
      where = cts_sorter[last_from_cts_sorter].where;
    }
    done[where] = 1;
    sorted[i] = where;
    for (j=0;j<total;j++) {
      if (!done[j]) {
	tdist = 
             RGBDist(ColorBoxes[where].red,
	             ColorBoxes[where].green,
	             ColorBoxes[where].blue,
                     ColorBoxes[j].red,
	             ColorBoxes[j].green,
	             ColorBoxes[j].blue);
	if (tdist < dist[j]) dist[j] = tdist;
      }
    }
  }


  /* move colorboxes to RGBquantized */
  for (i=0;i<total;i++) {
    RGBquantized[i].red = ColorBoxes[sorted[i]].red;
    RGBquantized[i].green = ColorBoxes[sorted[i]].green;
    RGBquantized[i].blue = ColorBoxes[sorted[i]].blue;
  }

  for (i=0;i<total;i++) {
    inv_sorted[sorted[i]] = i;
  }

  /* repermute RGBCounts which is now a map from index to quantized color */
  if (total==tptr->TransTableSize) { 
    for (i=0;i<tptr->TransTableSize;i++) {
      RGBCounts[i] = inv_sorted[i];
			  /* now an index into the quantized colors. */
    }
  } else {
    for (i=0;i<tptr->TransTableSize;i++) {
      RGBCounts[i] = INVALID_COLOR_VAL;
    } 
    for (i=0,box = &ColorBoxes[0]; i< NumBoxes; i++, box++) {
      for (g=box->glow;g<=box->ghigh;g++) {
        for (r=box->rlow;r<=box->rhigh;r++) {
          idx = IndexFromShifted(r,g,box->blow,tptr); 
          for (b=box->blow;b<=box->bhigh;b++,idx++) {
            RGBCounts[idx] = inv_sorted[i];
    } } } }
  }
  free(ColorBoxes);
  free(cts_sorter);
  free(dist);
  free(done);
  free(sorted);
  free(inv_sorted);
  return(1);
}



static int GetTranslatorTrueColor(long tslot, VR_ImKind kind)
{
  VR_Translator * tptr;
  
  tptr = &VR_Translators[tslot];
  SetShiftsAndMasks(tptr, 0);
  tptr->ToYCC = 0;
  tptr->kind = kind;
  tptr->TotalColorsUsed =
  tptr->TotalColorsRequested = VR_MaxPixel;
  tptr->TransTable = 0;
  tptr->ColorUseFlag = 0;
  tptr->inUse = 1;
  tptr->inUseCount = 0;
  tptr->VR_cmap = VR_DefCmapLoc;
  return(VR_DefCmapLoc);
}

static int GetTranslatorFixedColors(long tslot, VR_ImKind kind,
  int ColorFlag)
{
  
  long i;
  XColor xcolor;
  VR_TransEntry *ttable;
  unsigned char r, g, b, y, cr, cb;
  VR_Translator * tptr;
  unsigned long CCC_kind;
  unsigned long DoYCC;

  CCC_kind = ColorFlag & 0xff0;
  DoYCC = ColorFlag &  VR_YCC_PRESENT;
  if (!VR_ColorSupported) {
      DoYCC = VR_YCC_PRESENT;
      CCC_kind = VR_CCC_8x0x0;
  }


  xcolor.flags = DoRed | DoGreen | DoBlue;

  tptr = &VR_Translators[tslot];

  if (kind==VR_Gray) {

    /** Grayscale case *****/

    if ((ttable = (VR_TransEntry *)
	 malloc(sizeof(VR_TransEntry) * 256)) == NULL) {
	strcpy(VR_errmsg,"GetTranslatorFixedColors: out of memory");
	return(-1);
    }
    
    tptr->TransTableSize = 256;

    for (i=0;i<256;i++) {
      xcolor.red =
      xcolor.green =
      xcolor.blue = ((unsigned short) i) << 8;
      if (!VR_XAllocColor(VR_DefCmapLoc, &xcolor)) {
	/* should not happen! */
        free(ttable);
	strcpy(VR_errmsg,"GetTranslatorFixedColors: VR_XAllocColor failed!");
        return(-1); 
      }

      ttable[i] = xcolor.pixel;
    }

  } else if (kind==VR_RGB) {
    
    /** RGB case ******/


    SetShiftsAndMasks(tptr, CCC_kind);

    if ((ttable = (VR_TransEntry *) calloc(1, sizeof(VR_TransEntry)
	     * tptr->TransTableSize)) == NULL) {
	strcpy(VR_errmsg,"GetTranslatorFixedColors: out of memory");
	return(-1);
    }
    
    
    if (DoYCC) {
      if (CCC_kind == VR_CCC_8x0x0) {
        /* special case */
        for (i=0;i<tptr->TransTableSize;i++) {
	  y = IndexToG(i,tptr);
          xcolor.red = 
          xcolor.green = 
          xcolor.blue = ((unsigned short) y) << 8;
          if (!VR_XAllocColor(VR_DefCmapLoc, &xcolor)) {
	    /* should not happen! */
            free(ttable);
	    strcpy(VR_errmsg,"GetTranslatorFixedColors: VR_XAllocColor failed!");
            return(-1); 
          }
          ttable[i] = xcolor.pixel;
        }
      } else {
        for (i=0;i<tptr->TransTableSize;i++) {
	  y = IndexToG(i,tptr);
	  cb = IndexToR(i,tptr);
	  cr = IndexToB(i,tptr);
	  r = YCCtoR(y,cr,cb);
	  g = YCCtoG(y,cr,cb);
	  b = YCCtoB(y,cr,cb);
          xcolor.red = ((unsigned short) r) << 8;
          xcolor.green = ((unsigned short) g) << 8;
          xcolor.blue = ((unsigned short) b) << 8;
          if (!VR_XAllocColor(VR_DefCmapLoc, &xcolor)) {
	    /* should not happen! */
            free(ttable);
	    strcpy(VR_errmsg,"GetTranslatorFixedColors: VR_XAllocColor failed!");
            return(-1); 
          }
          ttable[i] = xcolor.pixel;
        }
      }
    } else {
      for (i=0;i<tptr->TransTableSize;i++) {
	r = IndexToR(i,tptr);
	g = IndexToG(i,tptr);
	b = IndexToB(i,tptr);
        xcolor.red = ((unsigned short) r) << 8;
        xcolor.green = ((unsigned short) g) << 8;
        xcolor.blue = ((unsigned short) b) << 8;
        if (!VR_XAllocColor(VR_DefCmapLoc, &xcolor)) {
	  /* should not happen! */
          free(ttable);
	  strcpy(VR_errmsg,"GetTranslatorFixedColors: VR_XAllocColor failed!");
          return(-1); 
        }
        ttable[i] = xcolor.pixel;
      }
    }

  } else {
    /* bitmaps later */
  }

  tptr->inUse = 1;
  tptr->inUseCount = 0;
  tptr->kind = kind;
  tptr->ToYCC = DoYCC;
  tptr->ColorUseFlag = 0;
  tptr->TotalColorsUsed = VR_MaxPixel;
  if (tptr->TotalColorsUsed > tptr->TransTableSize)
    tptr->TotalColorsUsed = tptr->TransTableSize;
  tptr->TotalColorsRequested = tptr->TotalColorsUsed;
  tptr->VR_cmap = VR_DefCmapLoc;
  tptr->TransTable = ttable;

  return(VR_DefCmapLoc);

}


/****** Used For PseudoColor, GrayScale, DirectColor ******/
static int GetGrayTranslatorInner(long tslot, VR_ImId iid,
           int ColorFlag, Colormap *suggested_cmap)
{
  
  unsigned long GrayCounts[256];
  unsigned long SortedGrayIndex[256];
  long i, j,  bestj,  cslot,  gotall, numgot, dist, tdist;
  unsigned long total, size;
  unsigned long ColorVal[256];
  XColor xcolor;
  Colormap cmap;
  int myCreation;
  VR_TransEntry *ttable;
  VR_Translator * tptr;
  unsigned long CFL_kind;
  long maxcols;
  int new_cmap;
  int isReadOnly; 


  size = VR_Images[iid].width * VR_Images[iid].height;

  CFL_kind = ColorFlag & 0xf;
  if (!CFL_kind)
    CFL_kind = VR_CFL_FORCE_DEFAULT;
  VR_CFL_GET_NCOLORS(ColorFlag, maxcols);
  if (maxcols > 256) maxcols = 256;


  xcolor.flags = DoRed | DoGreen | DoBlue;

  tptr = &VR_Translators[tslot];




  if ((ttable = (VR_TransEntry *)
	 malloc(sizeof(VR_TransEntry) * 256)) == NULL) {
	strcpy(VR_errmsg,"GetGrayTranslatorInner: out of memory");
	return(-1);
  }

  tptr->TransTableSize = 256;
  tptr->TransTable = ttable;
    
  GetGrayCounts(VR_Images[iid].data, VR_Images[iid].pixw1,
    size, GrayCounts, &total);
  if (total > maxcols) total = maxcols;
  SortGrayCounts(GrayCounts, SortedGrayIndex,  total);

  /* create color map if needed */
  if (CFL_kind != VR_CFL_PRIVATE) {
    cmap = *suggested_cmap; 
    myCreation = 0;
    isReadOnly = 1; 
  } else {
    myCreation = 1;
    isReadOnly = 0; 
    cmap = XCreateColormap(VR_TheDisplay, 
		       RootWindow(VR_TheDisplay, VR_TheScreen),
		       VR_TheVisual, AllocNone);
  }


  if ((cslot = FindOrSetCmap(cmap, &new_cmap, &isReadOnly,myCreation)) < 0) {
    free(ttable);
    if (myCreation) XFreeColormap(VR_TheDisplay, cmap);
    /* VR_errmsg already set by FindOrSetCmap */
    return(-1); 
  }

  if (myCreation && !new_cmap) { 
    /* shouldn't happen! */ 
    free(ttable);
    return(-1); 
  } 
    
  numgot = 0; 

  if (!myCreation) { 
      /* try to allocate shared ReadOnly colors */
      numgot = total; 
      for (i=0;i<total;i++) {
        xcolor.red =
        xcolor.green =
        xcolor.blue = (unsigned short) SortedGrayIndex[i] << 8;
        if (! VR_XAllocColor(cslot, &xcolor)) { 
  	  numgot = i;
  	  break;
        } else {
	  ColorVal[i] = xcolor.pixel;
      } }

      if ((numgot < total) &&
	  (CFL_kind != VR_CFL_FORCE_DEFAULT)) {
	/* use private cmap now */ 
        for (i=0;i<numgot;i++) { 
	   VR_XFreeColor(cslot, ColorVal[i]);
        }
        cmap = XCreateColormap(VR_TheDisplay,
	     RootWindow(VR_TheDisplay, VR_TheScreen),
	     VR_TheVisual, AllocNone);
        myCreation = 1;
	isReadOnly = 0; 
	if ((cslot = FindOrSetCmap(cmap, &new_cmap, &isReadOnly, myCreation)) < 0) {
	  free(ttable);
	  XFreeColormap(VR_TheDisplay, cmap);
          /* VR_errmsg already set by FindOrSetCmap */
	  return(-1); 
        }
	if (!new_cmap) {
	  /* shouldn't happen */
	  free(ttable);
	  return(-1);
  } } } 

  if (myCreation) { 
      /* either CFL_kind is VR_CFL_PRIVATE or it is
	 VR_CFL_TRY_DEFAULT and i could not get all colors */

    for (i=0;i<total;i++) { 
	if (! VR_XAllocColorCells(cslot, &ColorVal[i], 1))
		break;
    }

    numgot = i; 

    /* store numgot colors.. use first numgot out of total */

    for (i=0;i<numgot;i++) {
        xcolor.red =
        xcolor.green =
        xcolor.blue = (unsigned short) SortedGrayIndex[i] << 8;
	xcolor.pixel = ColorVal[i] = i;
	VR_XStoreColor(cslot, &xcolor);
  } } 

  tptr->TotalColorsRequested = total;
  total = numgot;

  if (total < 1) { 
      free(ttable);
      if (myCreation) VR_FreeColormap(cslot);
      strcpy(VR_errmsg,"GetGrayTranslatorInner: could not get any colors");
      return(-1); 
  }

  /* have allocated "total" cells. */
  /* set translator */


  for (i=0;i<256;i++) {
      ttable[i] = INVALID_COLOR_VAL;
  }

  for (i=0;i<total;i++) {
      ttable[SortedGrayIndex[i]] = ColorVal[i];
  }

  for (i=0;i<256;i++) {
      if (ttable[i]==INVALID_COLOR_VAL) {
	/* find closest entry in colormap */
	dist = 500;
	bestj = 0;
	for (j=0;j<256;j++) {
	  if (ttable[j] != INVALID_COLOR_VAL) {
	    tdist = i - (VR_Colormaps[cslot].Colors[ttable[j]].red>>8);
	    if (tdist < 0) tdist = -tdist;
	    if (tdist < dist) {
	      dist = tdist;
	      bestj = j;
        } } }
	ttable[i] = ttable[bestj];
  } }

  tptr->kind = VR_Gray;


  tptr->inUse = 1;
  tptr->inUseCount = 0;
  tptr->VR_cmap = cslot; 
  for (i=0;i<VR_MaxPixel;i++) tptr->ColorUseFlag[i] = 0;
  tptr->TotalColorsUsed = 0; 
  for (i=0;i<total;i++) {
    if (!tptr->ColorUseFlag[ColorVal[i]]) { 
      tptr->ColorUseFlag[ColorVal[i]] = 1;
      tptr->TotalColorsUsed++;
    }
  }
  return(cslot);

}



static int GetRGBTranslatorInner(long tslot, VR_ImId iid,
         int ColorFlag, Colormap *suggested_cmap)
{
  
  long i, j,  bestj, cslot,  gotall, numgot, dist, tdist;
  unsigned long total, size;
  unsigned long *ColorVal;
  XColor xcolor;
  Colormap cmap;
  int myCreation;
  VR_TransEntry *ttable;

  unsigned char r, g, b, y, cr, cb;
  unsigned long  *RGBCounts; 
  RepColor *RGBquantized;

  VR_Translator * tptr;
  unsigned long CFL_kind;
  unsigned long CCC_kind;
  unsigned long DoYCC;
  long maxcols;
  int new_cmap;
  int isReadOnly; 

  size = VR_Images[iid].width * VR_Images[iid].height;

  if (!Color_Square_Table_Set) Set_Color_Square_Table();

  CFL_kind = ColorFlag & 0xf;
  if (!CFL_kind)
    CFL_kind = VR_CFL_FORCE_DEFAULT;

  CCC_kind = ColorFlag & 0xff0;
  DoYCC = ColorFlag &  VR_YCC_PRESENT;

  if (!VR_ColorSupported) {
      DoYCC = VR_YCC_PRESENT;
      CCC_kind = VR_CCC_8x0x0;
  }

  VR_CFL_GET_NCOLORS(ColorFlag, maxcols);

  xcolor.flags = DoRed | DoGreen | DoBlue;

  tptr = &VR_Translators[tslot];


  tptr->ToYCC = DoYCC;
  tptr->kind = VR_RGB;
    
  SetShiftsAndMasks(tptr, CCC_kind);

  if ((ttable = (VR_TransEntry *) calloc(1, sizeof(VR_TransEntry)
	     * tptr->TransTableSize)) == NULL) {
    free(ttable);
    strcpy(VR_errmsg,"GetRGBTranslatorInner: out of memory");
    return(-1);
  }
  RGBCounts = (unsigned long *) ttable; /* temporary use */

  tptr->TransTable = ttable;

  if (!VR_Images[iid].separateRGB) {
    if (!VR_Images[iid].data) {
	free(ttable);
	strcpy(VR_errmsg,"GetRGBTranslatorInner: image data not set");
	return(-1);
    }
    GetAllColorsFromTrails(RGBCounts,VR_Images[iid].data,
        VR_Images[iid].pixw1, VR_Images[iid].pixw2, VR_Images[iid].pixw3,
			    size,&total,tptr); 
  } else {
    if (!VR_Images[iid].red ||
	  !VR_Images[iid].green ||
	  !VR_Images[iid].blue) {
	    free(ttable);
	    strcpy(VR_errmsg,"GetRGBTranslatorInner: image RGB data not set");
	    return(-1);
    }
    GetAllColorsFromTrios(RGBCounts, 
	                     VR_Images[iid].red,
			     VR_Images[iid].green,
			     VR_Images[iid].blue,
	                     VR_Images[iid].pixw1,
			     VR_Images[iid].pixw2,
			     VR_Images[iid].pixw3,
			     size, &total,tptr);
  } 
    
  if (maxcols < total) total = maxcols;

  RGBquantized = (RepColor *) malloc(sizeof(RepColor) * total);
  ColorVal = (unsigned long *) malloc(sizeof(unsigned long) * total);
  if (!ColorVal || !RGBquantized) {
    if (ColorVal) free(ColorVal);
    if (RGBquantized) free(RGBquantized);
    free(ttable);
    strcpy(VR_errmsg,"GetRGBTranslatorInner: out of memory");
    return(-1);
  }

    
  /** sort in alloc order and color-quantize to 8-bits **/
  /* replace RGBCounts[i] with quantized value for color i */
  /* quantized values will be in range 0..(total-1) */

  if (!SortRGBCounts(RGBCounts, RGBquantized, tptr, & total)) {
      free(ttable);
      free(RGBquantized);
      free(ColorVal);
      return(-1);
  }

  /* create color map if needed */
  if (CFL_kind != VR_CFL_PRIVATE) {
    cmap = *suggested_cmap; 
    myCreation = 0;
    isReadOnly = 1; 
  } else {
    cmap = XCreateColormap(VR_TheDisplay, 
		       RootWindow(VR_TheDisplay, VR_TheScreen),
		       VR_TheVisual, AllocNone);
    myCreation = 1;
    isReadOnly = 0; 
  }


  if ((cslot = FindOrSetCmap(cmap, &new_cmap, &isReadOnly, myCreation)) < 0) {
      if (myCreation) XFreeColormap(VR_TheDisplay, cmap);
      free(ttable);
      free(RGBquantized);
      free(ColorVal);
      return(-1);
  }
  if (myCreation && !new_cmap) { 
      /* shouldn't happen! */ 
      free(ttable);
      free(RGBquantized);
      free(ColorVal);
      return(-1);
  } 


  if (!myCreation) { 
      numgot = total; 
      /* try to allocate shared colors */
      if (DoYCC) {
        if (CCC_kind == VR_CCC_8x0x0) {
  	  /* special case */
          for (i=0;i<total;i++) {
	    y = RGBquantized[i].green;
            xcolor.red =
	    xcolor.green =
	    xcolor.blue = ((unsigned short) y) << 8;
            if (! VR_XAllocColor(cslot, &xcolor)) { 
	      numgot = i; 
	      break;
            } else {
	      ColorVal[i] = xcolor.pixel;
        } } } else {
          for (i=0;i<total;i++) {
	    y = RGBquantized[i].green;
	    cb = RGBquantized[i].red;
	    cr = RGBquantized[i].blue;
	    r = YCCtoR(y,cr,cb);
	    g = YCCtoG(y,cr,cb);
	    b = YCCtoB(y,cr,cb);
            xcolor.red = ((unsigned short) r) << 8;
            xcolor.green = ((unsigned short) g) << 8;
            xcolor.blue = ((unsigned short) b) << 8;
            if (! VR_XAllocColor(cslot, &xcolor)) { 
	      numgot = i; 
	      break;
            } else {
	      ColorVal[i] = xcolor.pixel;
	    }
	  }
        }
      } else {
	/* ! DoYCC */ 
        for (i=0;i<total;i++) {
  	  g = RGBquantized[i].green;
	  r = RGBquantized[i].red;
	  b = RGBquantized[i].blue;
          xcolor.red = ((unsigned short) r) << 8;
          xcolor.green = ((unsigned short) g) << 8;
          xcolor.blue = ((unsigned short) b)  << 8;
          if (! VR_XAllocColor(cslot, &xcolor)) { 
	    numgot = i; 
	    break;
          } else {
	    ColorVal[i] = xcolor.pixel;
	  }
	}
      }
    
      if ((numgot < total) &&
	  (CFL_kind != VR_CFL_FORCE_DEFAULT)) { 
	/* use private cmap now */ 
        for (i=0;i<numgot;i++) { 
	   VR_XFreeColor(cslot, ColorVal[i]);
        }
        cmap = XCreateColormap(VR_TheDisplay,
	     RootWindow(VR_TheDisplay, VR_TheScreen),
	     VR_TheVisual, AllocNone);
        myCreation = 1;
	isReadOnly = 0; 
	if ((cslot = FindOrSetCmap(cmap, &new_cmap, &isReadOnly, myCreation)) < 0) {
	  free(ttable);
	  XFreeColormap(VR_TheDisplay, cmap);
          free(RGBquantized);
          free(ColorVal);
	  return(-1); 
        }
	if (!new_cmap) {
	  /* shouldn't happen */
	  free(ttable);
          free(RGBquantized);
          free(ColorVal);
	  return(-1);
	}
      }
    }

    if (myCreation) {
      /*** allocate private RW colors ***/ 

      numgot = total; 
      for (i=0;i<total;i++)  { 
	if (! VR_XAllocColorCells(cslot, &ColorVal[i], 1)) { 
	  numgot = i; 
          break;
      } } 

      /* store numgot colors.. */ 
      if (DoYCC) {
        if (CCC_kind == VR_CCC_8x0x0) {
	  /* special case */
          for (i=0;i<numgot;i++) {
	    y = RGBquantized[i].green;
            xcolor.red =
	    xcolor.green =
	    xcolor.blue = ((unsigned short) y) << 8;
	    xcolor.pixel = ColorVal[i] = i;
	    VR_XStoreColor(cslot, &xcolor);
        } } else {
          for (i=0;i<numgot;i++) {
	    y = RGBquantized[i].green;
	    cb = RGBquantized[i].red;
	    cr = RGBquantized[i].blue;
	    r = YCCtoR(y,cr,cb);
	    g = YCCtoG(y,cr,cb);
	    b = YCCtoB(y,cr,cb);
            xcolor.red = ((unsigned short) r) << 8;
            xcolor.green = ((unsigned short) g) << 8;
            xcolor.blue = ((unsigned short) b) << 8;
	    xcolor.pixel = ColorVal[i] = i;
	    VR_XStoreColor(cslot, &xcolor);
      } } } else {
	/* ! DoYCC */ 
        for (i=0;i<numgot;i++) {
	  g = RGBquantized[i].green;
	  r = RGBquantized[i].red;
	  b = RGBquantized[i].blue;
          xcolor.red = ((unsigned short) r) << 8;
          xcolor.green = ((unsigned short) g) << 8;
          xcolor.blue = ((unsigned short) b) << 8;
	  xcolor.pixel = ColorVal[i] = i; 
	  VR_XStoreColor(cslot, &xcolor);
    } } }

    /* have allocated "numgot" cells. */
    if (numgot < 1) { 
      free(ttable);
      if (myCreation) XFreeColormap(VR_TheDisplay, cmap);
      free(RGBquantized);
      free(ColorVal);
      strcpy(VR_errmsg,"GetRGBTranslatorInner: could not get any colors");
      return(-1); 
    }

    tptr->TotalColorsRequested = total;

    total = numgot;


    /* note that ttable and RGBCounts are using the same 
    ** memory.. however, our over-writing is organized
    ** enough for this to be OK
    */
    for (i=0;i<tptr->TransTableSize;i++) {
      if (RGBCounts[i] < total) {
	ttable[i] = ColorVal[RGBCounts[i]];
      } else {
	/* mostly for places set to INVALID_PIXEL_VAL
	   by SortRGBCounts */ 
	if (DoYCC) {
	  if (CCC_kind == VR_CCC_8x0x0) {
	    y = IndexToG(i,tptr);
	    r = g = b = y;
	  } else {
	    cb = IndexToR(i,tptr);
	    y = IndexToG(i,tptr);
	    cr = IndexToB(i,tptr);
	    r = YCCtoR(y,cr,cb);
	    g = YCCtoG(y,cr,cb);
	    b = YCCtoB(y,cr,cb);
	} } else {
	  r = IndexToR(i,tptr);
	  g = IndexToG(i,tptr);
	  b = IndexToB(i,tptr);
	}
	dist = 196608; /* 256*256*3 */
	bestj = 0;
	for (j=0;j<total;j++) {
	  tdist = RGBDist(r,g,b,
		  (VR_Colormaps[cslot].Colors[ColorVal[j]].red>>8),
		  (VR_Colormaps[cslot].Colors[ColorVal[j]].green>>8),
		  (VR_Colormaps[cslot].Colors[ColorVal[j]].blue>>8));
	  if (tdist < dist) {
	    bestj = j;
	    dist = tdist;
	} }
	ttable[i] = ColorVal[bestj];
  } } 



  VR_Colormaps[cslot].inUseCount++; 

  tptr->inUse = 1;
  tptr->inUseCount = 0;
  tptr->VR_cmap = cslot; 
  for (i=0;i<VR_MaxPixel;i++) tptr->ColorUseFlag[i] = 0;
  tptr->TotalColorsUsed = 0; 
  for (i=0;i<total;i++) {
    if (!tptr->ColorUseFlag[ColorVal[i]]) { 
      tptr->ColorUseFlag[ColorVal[i]] = 1;
      tptr->TotalColorsUsed++;
    }
  }
  free(RGBquantized);
  free(ColorVal);
  return(cslot);

}

/*************************************************
** create a translator from the image iid.
** *suggested_cmap is the colormap to be used, with Read-Only
** colors. If this fails, and ColorFlag does not say
** VR_CFL_FORCE_DEFAULT, a new colormap will be created
** for private use, and Read-Write colors allocated..
** *suggested_cmap will then contain the value of the new 
** colormap.
**
** if VR_FixedColors is set then VR_CFL_FORCE_DEFAULT will be set too
**
** ToYCC is ignored for TrueColor
**
** ColorFlag contains the following info:
** 1. Colormap kind: (used only for !VR_FixedColors)
**    VR_CFL_FORCE_DEFAULT: Must use *suggested_cmap,
**       irrespective of # colors gotten
**    VR_CFL_TRY_DEFAULT: if *suggested_cmap cannot get
**            all the colors, a new, private, RW cmap
**            will be created and used
**    VR_CFL_PRIVATE: ignore input value in *suggested_cmap,
**            create and use a new, private RW cmap
** 2. Maxcolors: (used only for !VR_FixedColors)
**      Maximum # of colors to be used (<= VR_MaxPixel)
** 3. Ccube: (used only for (RGB && !TrueColor))
**      if image is RGB (or YCC), ccube determines the
**      # of bits used from the color values for gathring
**      color histograms
**
**************************************************/
static VR_TransId GetTranslatorInner(VR_ImId iid,
       int ColorFlag, Colormap *suggested_cmap)
{
  
  long i, tslot, cslot;

  VR_Translator * temp;

  if (!VR_Images[iid].inUse) {
    strcpy(VR_errmsg,"GetTranslatorInner: invalid image id");
    return(-1);
  }
  if (VR_NumTranslators == VR_TranslatorsSize) {
    if ((temp = (VR_Translator *) 
       calloc(1,sizeof(VR_Translator)*(2*VR_TranslatorsSize))) == NULL) {
	 strcpy(VR_errmsg,"GetTranslatorInner: out of memory");
         return(-1);
    }
    memcpy(temp,VR_Translators,sizeof(VR_Translator)*VR_TranslatorsSize);
    free(VR_Translators);
    VR_Translators = temp;
    VR_TranslatorsSize *= 2; 
  } 



  for (i=0;i<VR_TranslatorsSize;i++) {
    if (!VR_Translators[i].inUse) {
      tslot = i;
      break;
    }
  }

  
  if (VR_VisualClass == TrueColor) {
    cslot = GetTranslatorTrueColor(tslot, VR_Images[iid].kind);
  } else if (VR_FixedColors) {
    cslot = GetTranslatorFixedColors(tslot, VR_Images[iid].kind, ColorFlag);
  } else {
    cslot = -1;
    if ((VR_Translators[tslot].ColorUseFlag = (int *)
      calloc(1,sizeof(int)*VR_MaxPixel)) == NULL) return(-1);
    if (VR_Images[iid].kind == VR_Gray) {
      cslot = GetGrayTranslatorInner(tslot, iid, ColorFlag, suggested_cmap);
    } else if (VR_Images[iid].kind == VR_RGB) {
      cslot = GetRGBTranslatorInner(tslot, iid, ColorFlag, suggested_cmap);
    } else {
      /* bitmaps not done */
    }
    if (cslot < 0) {
      free(VR_Translators[tslot].ColorUseFlag);
    }
  }

  if (cslot < 0) return(-1);

  VR_NumTranslators++;
  VR_Colormaps[cslot].inUseCount++;

  return(tslot);

}

/* get translator, using the X default colormap as 
   suggested cmap */ 
extern VR_TransId VR_GetTranslator(VR_ImId iid, int ColorFlag)
{
  
  Colormap cmap;
  cmap = VR_DefaultColormap; 
  return(GetTranslatorInner(iid, ColorFlag, &cmap)); 

}

/** get translator, using the given cmap as suggested cmap */
extern VR_TransId VR_GetTranslatorFromCM(VR_ImId iid, int ColorFlag,
		 Colormap cmap)
{
  Colormap loc_cmap = cmap;
  return(GetTranslatorInner(iid, ColorFlag, &loc_cmap)); 
} 


/** get translator, using the cmap of given window as suggested cmap */
extern VR_TransId VR_GetTranslatorFromCMofXWin(VR_ImId iid,
    int ColorFlag, Window win) 
{
  
  Colormap cmap; 
  XWindowAttributes wattribs; 

  if (!XWinIsValid(win,0,0)) {
    strcpy(VR_errmsg,"GetTranslatorFromCMofXWin: XWindow seems invalid");
    return(-1);
  }

  /* XGetWindowAttributes fails on a pixmap, so if it fails try it again
   * on the root window.  This is kind of kludgey, but it seems to work
   * okay.  RKW 1998-09-04. */
  if (XGetWindowAttributes(VR_TheDisplay, win, &wattribs) == 0) {
    XGetWindowAttributes(VR_TheDisplay, RootWindow(VR_TheDisplay,
      VR_TheScreen), &wattribs);
  }
  if (VR_VisualInfo.Id != XVisualIDFromVisual(wattribs.visual)) {
    strcpy(VR_errmsg,"GetTranslatorFromCMofXWin: given XWindow's visual not same as VR_visual");
    return(-1);
  }
  cmap = wattribs.colormap; 
  return(GetTranslatorInner(iid, ColorFlag, &cmap)); 
} 

extern void VR_FreeColormap(int cslot)
{
  if (!VR_Colormaps[cslot].inUse ||
      !VR_Colormaps[cslot].myCreation) return;

  XFreeColormap(VR_TheDisplay, VR_Colormaps[cslot].cmap);
  if (VR_Colormaps[cslot].Colors) free(VR_Colormaps[cslot].Colors);
  if (VR_Colormaps[cslot].ColorUseCount) free(VR_Colormaps[cslot].ColorUseCount);
  if (VR_Colormaps[cslot].Allocated) free(VR_Colormaps[cslot].Allocated);
  if (VR_Colormaps[cslot].ValidColor) free(VR_Colormaps[cslot].ValidColor);
  VR_Colormaps[cslot].inUse = 0;
  VR_NumColormaps--;
}



extern unsigned long VR_TranslateGrayPixel(VR_TransId tid,
            int grayval, int *gray_ans) 
     /* 
     Translates a gray value into a pixel value, according to
     the translator tid. *gray_ans returns the actual gray
     value of the returned pixel. grayval should be in [0,255]
     */ 
{

  VR_Translator * tptr = &VR_Translators[tid];
  unsigned long pixcode; 

  if (VR_VisualClass == TrueColor) {
    pixcode = (unsigned long) GrayToGrayIndex(grayval, tptr);
    *gray_ans = (int) GrayIndexToGray( pixcode, tptr);
  } else {
    pixcode = tptr->TransTable[grayval];
    *gray_ans = (int)
      (VR_Colormaps[tptr->VR_cmap].Colors[pixcode].red >> 8);
  }
  return pixcode; 

}

extern unsigned long VR_TranslateRGBPixel(VR_TransId tid,
            int r, int g, int b,
            int *r_ans, int *g_ans, int *b_ans) 
     /* 
     Translates an RGB value into a pixel value, according to
     the translator tid. *r_ans, *g_ans, *b_ans return the actual 
     color values of the returned pixel. r,g,b should each be in [0,255]
     */ 
{
  VR_Translator * tptr = &VR_Translators[tid];
  unsigned long pixcode; 
  unsigned long idx = RGBTrioToIndex(r,g,b,tptr);

  if (VR_VisualClass == TrueColor) {
    pixcode = idx;
    *r_ans = (int) IndexToR( idx, tptr);
    *g_ans = (int) IndexToG( idx, tptr);
    *b_ans = (int) IndexToB( idx, tptr);
  } else {
    pixcode = tptr->TransTable[idx];
    *r_ans = (int) (VR_Colormaps[tptr->VR_cmap].Colors[pixcode].red >> 8);
    *g_ans = (int) (VR_Colormaps[tptr->VR_cmap].Colors[pixcode].green >> 8);
    *b_ans = (int) (VR_Colormaps[tptr->VR_cmap].Colors[pixcode].blue >> 8);
  }  
  return pixcode; 

}


