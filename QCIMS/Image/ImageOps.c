#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "Image.h"
#include "ImageInternal.h" 

static void RetainImagePlane(Image *Im, int pnum)
{
  long nbytes;

  if (Im->Changed[pnum]) return; 
  nbytes = ((long) Im->NumRows) * Im->NumCols * Im->SampleBytes[pnum];
  if  ((Im->ImState == ImStateReadSome) ||
       ((Im->ImState == ImStateReadAll) && Im->RetainOrigOnChange) ) {
    if ((Im->RetainedData[pnum] = (char *) malloc(nbytes)) != NULL)
      memcpy(Im->RetainedData[pnum], Im->Im[pnum], nbytes); 
  } 
}



static void DoRGBtoYCC(Image *Im)
{
  unsigned long y,cb,cr;
  float y_flt, cb_flt, cr_flt;
  unsigned long imsize, i;
  float msampleby2;


  imsize = Im->CurrRows[0] * Im->CurrCols[0];
  msampleby2 = ((float) ((unsigned long) (Im->MaxSample[0]+1)/2));


  switch(Im->SampleBytes[0]) {

  case 1:
    for (i=0;i<imsize;i++) {
      y_flt  = 
	  (((float) Im->Im[0][i]) * ((float) 0.299)) +
	  (((float) Im->Im[1][i]) * ((float) 0.587)) +
	  (((float) Im->Im[2][i]) * ((float) 0.114)) +
	  ((float) 0.5);
      cb_flt = 
	  (((float) Im->Im[0][i]) * ((float) -0.16784)) +
	  (((float) Im->Im[1][i]) * ((float) -0.33126)) +
	  (((float) Im->Im[2][i]) * ((float) 0.5))      +
	  msampleby2 + ((float) 0.5);
      cr_flt = 
	  (((float) Im->Im[0][i]) * ((float) 0.5))      +
	  (((float) Im->Im[1][i]) * ((float) -0.41869)) +
	  (((float) Im->Im[2][i]) * ((float) -0.08131)) +
	  msampleby2 + ((float) 0.5);
      /* if (y_flt < 0) y_flt = 0; cannot happen! */
      if (cb_flt < 0) cb_flt = 0; 
      if (cr_flt < 0) cr_flt = 0; 
      y = (unsigned long) y_flt;
      cb = (unsigned long) cb_flt;
      cr = (unsigned long) cr_flt;
      if (y > Im->MaxSample[0]) y = Im->MaxSample[0];
      if (cb > Im->MaxSample[0]) cb = Im->MaxSample[0];
      if (cr > Im->MaxSample[0]) cr = Im->MaxSample[0];
    
      Im->Im[0][i] = (Pixel) y;
      Im->Im[1][i] = (Pixel) cb;
      Im->Im[2][i] = (Pixel) cr;
    }
    break;
  case 2:
    for (i=0;i<imsize;i++) {
      y_flt  = 
	  (((float) Im->Im2[0][i]) * ((float) 0.299)) +
	  (((float) Im->Im2[1][i]) * ((float) 0.587)) +
	  (((float) Im->Im2[2][i]) * ((float) 0.114)) +
	  ((float) 0.5);
      cb_flt = 
	  (((float) Im->Im2[0][i]) * ((float) -0.16784)) +
	  (((float) Im->Im2[1][i]) * ((float) -0.33126)) +
	  (((float) Im->Im2[2][i]) * ((float) 0.5))      +
	  msampleby2 + ((float) 0.5);
      cr_flt = 
	  (((float) Im->Im2[0][i]) * ((float) 0.5))      +
	  (((float) Im->Im2[1][i]) * ((float) -0.41869)) +
	  (((float) Im->Im2[2][i]) * ((float) -0.08131)) +
	  msampleby2 + ((float) 0.5);
      /* if (y_flt < 0) y_flt = 0; cannot happen! */
      if (cb_flt < 0) cb_flt = 0; 
      if (cr_flt < 0) cr_flt = 0; 
      y = (unsigned long) y_flt;
      cb = (unsigned long) cb_flt;
      cr = (unsigned long) cr_flt;
      if (y > Im->MaxSample[0]) y = Im->MaxSample[0];
      if (cb > Im->MaxSample[0]) cb = Im->MaxSample[0];
      if (cr > Im->MaxSample[0]) cr = Im->MaxSample[0];
    
      Im->Im2[0][i] = (Pixel2) y;
      Im->Im2[1][i] = (Pixel2) cb;
      Im->Im2[2][i] = (Pixel2) cr;
    }
    break;
  case 4:
    for (i=0;i<imsize;i++) {
      y_flt  = 
	  (((float) Im->Im4[0][i]) * ((float) 0.299)) +
	  (((float) Im->Im4[1][i]) * ((float) 0.587)) +
	  (((float) Im->Im4[2][i]) * ((float) 0.114)) +
	  ((float) 0.5);
      cb_flt = 
	  (((float) Im->Im4[0][i]) * ((float) -0.16784)) +
	  (((float) Im->Im4[1][i]) * ((float) -0.33126)) +
	  (((float) Im->Im4[2][i]) * ((float) 0.5))      +
	  msampleby2 + ((float) 0.5);
      cr_flt = 
	  (((float) Im->Im4[0][i]) * ((float) 0.5))      +
	  (((float) Im->Im4[1][i]) * ((float) -0.41869)) +
	  (((float) Im->Im4[2][i]) * ((float) -0.08131)) +
	  msampleby2 + ((float) 0.5);
      /* if (y_flt < 0) y_flt = 0; cannot happen! */
      if (cb_flt < 0) cb_flt = 0; 
      if (cr_flt < 0) cr_flt = 0; 
      y = (unsigned long) y_flt;
      cb = (unsigned long) cb_flt;
      cr = (unsigned long) cr_flt;
      if (y > Im->MaxSample[0]) y = Im->MaxSample[0];
      if (cb > Im->MaxSample[0]) cb = Im->MaxSample[0];
      if (cr > Im->MaxSample[0]) cr = Im->MaxSample[0];
    
      Im->Im4[0][i] = (Pixel4) y;
      Im->Im4[1][i] = (Pixel4) cb;
      Im->Im4[2][i] = (Pixel4) cr;
    }
    break;
  }
}

static void DoYCCtoRGB(Image *Im)
{
  unsigned long r, g, b; 
  float r_flt, g_flt, b_flt;
  unsigned long imsize, i;
  float msampleby2;


  imsize = Im->CurrRows[0] * Im->CurrCols[0];
  msampleby2 = ((float) ((unsigned long) (Im->MaxSample[0]+1)/2));


  switch(Im->SampleBytes[0]) {

  case 1:
    for (i=0;i<imsize;i++) {
      r_flt  = 
	  ((float) Im->Im[0][i]) +
	  (((float) Im->Im[2][i] - msampleby2) * ((float) 1.402)) +
	  ((float) 0.5);
      g_flt = 
	  ((float) Im->Im[0][i]) - 
	  (((float) Im->Im[1][i] - msampleby2) * ((float) 0.34414)) -
	  (((float) Im->Im[2][i] - msampleby2) * ((float) 0.71414)) +
	  ((float) 0.5);
      b_flt = 
	  ((float) Im->Im[0][i]) + 
	  (((float) Im->Im[1][i] - msampleby2) * ((float) 1.772)) +
	  ((float) 0.5);
      if (r_flt < 0) r_flt = 0; 
      if (g_flt < 0) g_flt = 0; 
      if (b_flt < 0) b_flt = 0; 
      r = (unsigned long) r_flt;
      g = (unsigned long) g_flt;
      b = (unsigned long) b_flt;
      if (r > Im->MaxSample[0]) r = Im->MaxSample[0];
      if (g > Im->MaxSample[0]) g = Im->MaxSample[0];
      if (b > Im->MaxSample[0]) b = Im->MaxSample[0];
    
      Im->Im[0][i] = (Pixel) r;
      Im->Im[1][i] = (Pixel) g;
      Im->Im[2][i] = (Pixel) b; 
    }
    break;
  case 2:
    for (i=0;i<imsize;i++) {
      r_flt  = 
	  ((float) Im->Im2[0][i]) +
	  (((float) Im->Im2[2][i] - msampleby2) * ((float) 1.402)) +
	  ((float) 0.5);
      g_flt = 
	  ((float) Im->Im2[0][i]) - 
	  (((float) Im->Im2[1][i] - msampleby2) * ((float) 0.34414)) -
	  (((float) Im->Im2[2][i] - msampleby2) * ((float) 0.71414)) +
	  ((float) 0.5);
      b_flt = 
	  ((float) Im->Im2[0][i]) + 
	  (((float) Im->Im2[1][i] - msampleby2) * ((float) 1.772)) +
	  ((float) 0.5);
      if (r_flt < 0) r_flt = 0; 
      if (g_flt < 0) g_flt = 0; 
      if (b_flt < 0) b_flt = 0; 
      r = (unsigned long) r_flt;
      g = (unsigned long) g_flt;
      b = (unsigned long) b_flt;
      if (r > Im->MaxSample[0]) r = Im->MaxSample[0];
      if (g > Im->MaxSample[0]) g = Im->MaxSample[0];
      if (b > Im->MaxSample[0]) b = Im->MaxSample[0];
    
      Im->Im2[0][i] = (Pixel) r;
      Im->Im2[1][i] = (Pixel) g;
      Im->Im2[2][i] = (Pixel) b; 
    }
    break;
  case 4:
    for (i=0;i<imsize;i++) {
      r_flt  = 
	  ((float) Im->Im4[0][i]) +
	  (((float) Im->Im4[2][i] - msampleby2) * ((float) 1.402)) +
	  ((float) 0.5);
      g_flt = 
	  ((float) Im->Im4[0][i]) - 
	  (((float) Im->Im4[1][i] - msampleby2) * ((float) 0.34414)) -
	  (((float) Im->Im4[2][i] - msampleby2) * ((float) 0.71414)) +
	  ((float) 0.5);
      b_flt = 
	  ((float) Im->Im4[0][i]) + 
	  (((float) Im->Im4[1][i] - msampleby2) * ((float) 1.772)) +
	  ((float) 0.5);
      if (r_flt < 0) r_flt = 0; 
      if (g_flt < 0) g_flt = 0; 
      if (b_flt < 0) b_flt = 0; 
      r = (unsigned long) r_flt;
      g = (unsigned long) g_flt;
      b = (unsigned long) b_flt;
      if (r > Im->MaxSample[0]) r = Im->MaxSample[0];
      if (g > Im->MaxSample[0]) g = Im->MaxSample[0];
      if (b > Im->MaxSample[0]) b = Im->MaxSample[0];
    
      Im->Im4[0][i] = (Pixel) r;
      Im->Im4[1][i] = (Pixel) g;
      Im->Im4[2][i] = (Pixel) b; 
    }
    break;
  }
}



extern int
ImChangeCspace(Image *Im, ImColorSpaceType csp)
{
  int p; 
  void  (*change_func) (Image *Im); 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImChangeCspace: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImChangeCspace: image already dead!");
  
  if (!ImCspaceCompatible(csp, Im->CurrCspace)) 
    ImNonFatalError("ImChangeCspace: colorspace not compatible");
  
  if (csp == Im->CurrCspace) return(1);

  if (!Im->AllPlanesEqual) 
    ImNonFatalError("ImChangeCspace: plane dimensions unequal");

  if ((csp == ImC_RGB) && (Im->CurrCspace == ImC_YCbCr)) { 
    change_func = DoYCCtoRGB; 
  } else if ((csp == ImC_YCbCr) && (Im->CurrCspace == ImC_RGB)) { 
    change_func = DoRGBtoYCC; 
  } else { 
    ImNonFatalError("ImChangeCspace: conversion not supported");
  } 
  
  for (p=0;p<Im->NumPlanes;p++) {
    if (!Im->Changed[p]) 
      RetainImagePlane(Im, p);
    Im->Changed[p] = TRUE; 
  }
  (*change_func)(Im); 
  Im->CurrCspace = csp; 
  return(1); 

}

extern int
ImCrop(Image *Im, int CropStartRow, int CropStartCol,
		  int CropEndRow, int CropEndCol) 
{
  int p, p1; 
  int origX, origY, origH, origW; 
  Pixel *buff[IM_MAXPLANES];
  long nbytes, nbytes_in_row, csize;
  long nbytes_in_orig_row; 
  Pixel *inptr, *outptr;
  long i, cwidth, cheight; 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImCrop: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImCrop: image already dead!");
  

  if (!Im->AllPlanesEqual) 
    ImNonFatalError("ImCrop: plane dimensions unequal");

  if ( (CropStartRow >= CropEndRow) || 
       (CropStartCol >= CropEndCol) || 
       (CropStartRow < 0) || 
       (CropStartCol < 0) || 
       (CropEndRow >=  Im->CurrRows[0]) || 
       (CropEndCol >=  Im->CurrCols[0]) ) 
    ImNonFatalError("ImCrop: bad crop location");

  cwidth = CropEndCol - CropStartCol + 1; 
  cheight = CropEndRow - CropStartRow + 1; 
  csize = cwidth * cheight; 

  for (p=0;p<Im->NumPlanes;p++) {
    nbytes = csize * Im->SampleBytes[p];
    if ((buff[p] = (Pixel *) malloc(nbytes)) == NULL) {
      for (p1 = 0; p1 < p; p1++) IM_FREE_PTR(buff[p1]); 
      ImNonFatalError("ImCrop: out of memory");
    } 
    memset(buff[p], 255, nbytes); 
  }

  /* okay .. will not fail after this */ 
  for (p=0;p<Im->NumPlanes;p++) {
    nbytes_in_orig_row = ((long) Im->CurrCols[p])* Im->SampleBytes[p]; 
    nbytes_in_row = ((long) cwidth)* Im->SampleBytes[p]; 
    inptr = Im->Im[p] +
	    (((CropStartRow*Im->CurrCols[p])+CropStartCol)*
	     Im->SampleBytes[p]); 
    outptr = buff[p];
    for (i=0;i<cheight;i++) {
      memcpy(outptr, inptr, nbytes_in_row);
      inptr += nbytes_in_orig_row; 
      outptr += nbytes_in_row; 
    } 

    if (!Im->Changed[p]) 
      RetainImagePlane(Im, p);
    IM_FREE_PTR(Im->Im[p]); 
    Im->Changed[p] = TRUE; 
    Im->Im[p] = buff[p]; 
    Im->Im2[p] = (Pixel2 *) buff[p]; 
    Im->Im4[p] = (Pixel4 *) buff[p]; 
  }
  
  /* figure out crop location wrt original image */
  if (Im->WasCropped) {
    origX = Im->CropStartCol; 
    origY = Im->CropStartRow;
    origW = Im->CropEndCol - Im->CropStartCol + 1; 
    origH = Im->CropEndRow - Im->CropStartRow + 1; 
  } else {
    origX = 0; 
    origY = 0; 
    origW = Im->NumCols; 
    origH = Im->NumRows; 
  } 
  Im->WasCropped = TRUE; 
  Im->CropStartRow = origY + ((CropStartRow*origH)/Im->CurrRows[0]);
  Im->CropEndRow = origY + ((CropEndRow*origH)/Im->CurrRows[0]);
  Im->CropStartCol = origX + ((CropStartCol*origW)/Im->CurrCols[0]);
  Im->CropEndCol = origX + ((CropEndCol*origW)/Im->CurrCols[0]);

  for (p=0;p<Im->NumPlanes;p++) {
    Im->CurrRows[p] = cheight; 
    Im->CurrCols[p] = cwidth; 
  }

  return(1); 
}


static int SmoothResize(Pixel *in, Pixel *out, int inrows,
   int incols, int outrows, int outcols)
{
  int smoothRows = (outrows > inrows); 
  int smoothCols = (outcols > incols); 
  /* at least one of above two is guaranteed to be true */ 
  /* also, incols,inrows > 1 */ 
  Pixel *inptr, *outptr;
  int i, j, k, l;
  long tot, sm_colval; 
  int match_row_start, match_row_num;
  int smooth_row_perc; 
  int *match_col_start, *match_col_num, col, newcol, start, ct;
  int *smooth_col_perc = (int *) 0; 
  float temp; 
  int incolsless1; 

  
  match_col_start = malloc(sizeof(int)*outcols);
  match_col_num = malloc(sizeof(int)*outcols);
  if (smoothCols)
    smooth_col_perc = match_col_num; 

  
  if (!match_col_start || !match_col_num) { 
    if (match_col_start) IM_FREE_PTR(match_col_start);
    if (match_col_num) IM_FREE_PTR(match_col_num);
    return(0);
  }

  if (smoothCols) {
    for (j=0;j<outcols;j++) {
      match_col_start[j] = (j * incols)/ outcols;
      temp = ((float) j)*((float) incols)/((float) outcols); 
      temp -= ((float) match_col_start[j]); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_col_perc[j] = ((int) temp); 
      if (smooth_col_perc[j] < 0) smooth_col_perc[j] = 0; 
      if (smooth_col_perc[j] > 100) smooth_col_perc[j] = 100; 
      if (match_col_start[j] == (incols - 1)) {
	/* use a little trick */
	/* this is why we need image dims >= 2x2 */ 
	match_col_start[j]--;
	smooth_col_perc[j] = 0; 
      } 
    }
  } else {
    col = 0; ct = 0, start = 0;
    for (j=0;j<incols;j++) {
      newcol = (j * outcols)/ incols;
      if (newcol == col) ct++;
      else {
	match_col_start[col] = start;
	match_col_num[col] = ct;
	ct = 1;
	start = j;
	col = newcol;
      }
    }
    match_col_start[col] = start;
    match_col_num[col] = ct;
  }

  
  outptr = out;

  if (smoothRows && smoothCols) { 
    incolsless1 = incols-1; 
    for (i=0;i<outrows;i++) {
      match_row_start = (i * inrows) / outrows;
      temp = ((float) i)*((float) inrows)/((float) outrows); 
      temp -= ((float) match_row_start); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_row_perc = ((int) temp); 
      if (smooth_row_perc < 0) smooth_row_perc = 0; 
      if (smooth_row_perc > 100) smooth_row_perc = 100; 
      if (match_row_start == (inrows-1)) {
	/* use the trick again */
	match_row_start--;
	smooth_row_perc = 0; 
      }   
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in +
	    ((((long) match_row_start)*incols) + match_col_start[j]);
        tot = 50; 
        sm_colval = 50; 
        sm_colval += ((long) (*inptr++))*smooth_col_perc[j];
        sm_colval += ((long) (*inptr))*(100-smooth_col_perc[j]);
        sm_colval /= 100; 
        tot += (sm_colval*smooth_row_perc); 
        inptr += incolsless1; 
        sm_colval = 50; 
        sm_colval += ((long) (*inptr++))*smooth_col_perc[j];
        sm_colval += ((long) (*inptr))*(100-smooth_col_perc[j]);
        sm_colval /= 100; 
        tot += (sm_colval*(100-smooth_row_perc)); 
        tot /= 100; 
        *outptr = (Pixel) tot;
      }
    }
  } else if (smoothRows && !smoothCols) { 
    for (i=0;i<outrows;i++) {
      match_row_start = (i * inrows) / outrows;
      temp = ((float) i)*((float) inrows)/((float) outrows); 
      temp -= ((float) match_row_start); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_row_perc = ((int) temp); 
      if (smooth_row_perc < 0) smooth_row_perc = 0; 
      if (smooth_row_perc > 100) smooth_row_perc = 100; 
      if (match_row_start == (inrows-1)) {
	/* use the trick again */
	match_row_start--;
	smooth_row_perc = 0; 
      }   
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in
	    + ((((long) match_row_start)*incols) + match_col_start[j]);
	tot = 50; 
	sm_colval = match_col_num[j]/2; 
  	for (l=0;l < match_col_num[j];l++,inptr++) {
	  sm_colval += (long) (*inptr);
	} 
	sm_colval /= match_col_num[j]; 
	tot += (sm_colval*smooth_row_perc); 
	inptr += (incols - match_col_num[j]); 
	sm_colval = match_col_num[j]/2; 
  	for (l=0;l < match_col_num[j];l++,inptr++) {
	  sm_colval += (long) (*inptr);
	} 
	sm_colval /= match_col_num[j]; 
	tot += (sm_colval*(100-smooth_row_perc)); 
	tot /= 100; 
        *outptr = (Pixel) tot;
      }
    }
  } else /* if (!smoothRows && smoothCols) */  { 
    incolsless1 = incols-1; 
    match_row_start = 0;
    match_row_num = 0;
    for (i=0;i<outrows;i++) {
      match_row_start  += match_row_num;
      j = match_row_start; match_row_num = 0;
      while (((j * outrows)/inrows) == i) {match_row_num++;j++;}
      for (j=0;j<outcols; j++, outptr++) {
        inptr = in + ((((long) match_row_start)*incols) + match_col_start[j]);
        tot = match_row_num/2;
        for (k=0;k < match_row_num; k++, inptr += incolsless1) {
	  sm_colval = 50; 
	  sm_colval += (((long) (*inptr++))*smooth_col_perc[j]);
	  sm_colval += (((long) (*inptr))*(100-smooth_col_perc[j]));
	  sm_colval /= 100; 
	  tot += sm_colval; 
        }
        tot /= match_row_num;
        *outptr = (Pixel) tot;
      }
    }
  } 
  IM_FREE_PTR(match_col_start);
  IM_FREE_PTR(match_col_num);
  return(1);
}

static int SmoothResize2(Pixel2 *in, Pixel2 *out, int inrows,
   int incols, int outrows, int outcols)
{
  int smoothRows = (outrows > inrows); 
  int smoothCols = (outcols > incols); 
  /* at least one of above two is guaranteed to be true */ 
  /* also, incols,inrows > 1 */ 
  Pixel2 *inptr, *outptr;
  int i, j, k, l;
  int incolsless1; 
  long tot, sm_colval; 
  int match_row_start, match_row_num;
  int smooth_row_perc; 
  int *match_col_start, *match_col_num, col, newcol, start, ct;
  int *smooth_col_perc = (int *) 0; 
  float temp; 

  
  match_col_start = malloc(sizeof(int)*outcols);
  match_col_num = malloc(sizeof(int)*outcols);
  if (smoothCols)
    smooth_col_perc = match_col_num; 

  
  if (!match_col_start || !match_col_num) { 
    if (match_col_start) IM_FREE_PTR(match_col_start);
    if (match_col_num) IM_FREE_PTR(match_col_num);
    return(0);
  }

  if (smoothCols) {
    for (j=0;j<outcols;j++) {
      match_col_start[j] = (j * incols)/ outcols;
      temp = ((float) j)*((float) incols)/((float) outcols); 
      temp -= ((float) match_col_start[j]); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_col_perc[j] = ((int) temp); 
      if (smooth_col_perc[j] < 0) smooth_col_perc[j] = 0; 
      if (smooth_col_perc[j] > 100) smooth_col_perc[j] = 100; 
      if (match_col_start[j] == (incols - 1)) {
	/* use a little trick */
	/* this is why we need image dims >= 2x2 */ 
	match_col_start[j]--;
	smooth_col_perc[j] = 0; 
      } 
    }
  } else {
    col = 0; ct = 0, start = 0;
    for (j=0;j<incols;j++) {
      newcol = (j * outcols)/ incols;
      if (newcol == col) ct++;
      else {
	match_col_start[col] = start;
	match_col_num[col] = ct;
	ct = 1;
	start = j;
	col = newcol;
      }
    }
    match_col_start[col] = start;
    match_col_num[col] = ct;
  }

  
  outptr = out;

  if (smoothRows && smoothCols) { 
    incolsless1 = incols-1; 
    for (i=0;i<outrows;i++) {
      match_row_start = (i * inrows) / outrows;
      temp = ((float) i)*((float) inrows)/((float) outrows); 
      temp -= ((float) match_row_start); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_row_perc = ((int) temp); 
      if (smooth_row_perc < 0) smooth_row_perc = 0; 
      if (smooth_row_perc > 100) smooth_row_perc = 100; 
      if (match_row_start == (inrows-1)) {
	/* use the trick again */
	match_row_start--;
	smooth_row_perc = 0; 
      }   
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in +
	    ((((long) match_row_start)*incols) + match_col_start[j]);
        tot = 50; 
        sm_colval = 50; 
        sm_colval += ((long) (*inptr++))*smooth_col_perc[j];
        sm_colval += ((long) (*inptr))*(100-smooth_col_perc[j]);
        sm_colval /= 100; 
        tot += (sm_colval*smooth_row_perc); 
        inptr += incolsless1; 
        sm_colval = 50; 
        sm_colval += ((long) (*inptr++))*smooth_col_perc[j];
        sm_colval += ((long) (*inptr))*(100-smooth_col_perc[j]);
        sm_colval /= 100; 
        tot += (sm_colval*(100-smooth_row_perc)); 
        tot /= 100; 
        *outptr = (Pixel2) tot;
      }
    }
  } else if (smoothRows && !smoothCols) { 
    for (i=0;i<outrows;i++) {
      match_row_start = (i * inrows) / outrows;
      temp = ((float) i)*((float) inrows)/((float) outrows); 
      temp -= ((float) match_row_start); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_row_perc = ((int) temp); 
      if (smooth_row_perc < 0) smooth_row_perc = 0; 
      if (smooth_row_perc > 100) smooth_row_perc = 100; 
      if (match_row_start == (inrows-1)) {
	/* use the trick again */
	match_row_start--;
	smooth_row_perc = 0; 
      }   
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in
	    + ((((long) match_row_start)*incols) + match_col_start[j]);
	tot = 50; 
	sm_colval = match_col_num[j]/2; 
  	for (l=0;l < match_col_num[j];l++,inptr++) {
	  sm_colval += (long) (*inptr);
	} 
	sm_colval /= match_col_num[j]; 
	tot += (sm_colval*smooth_row_perc); 
	inptr += (incols - match_col_num[j]); 
	sm_colval = match_col_num[j]/2; 
  	for (l=0;l < match_col_num[j];l++,inptr++) {
	  sm_colval += (long) (*inptr);
	} 
	sm_colval /= match_col_num[j]; 
	tot += (sm_colval*(100-smooth_row_perc)); 
	tot /= 100; 
        *outptr = (Pixel2) tot;
      }
    }
  } else /* if (!smoothRows && smoothCols) */  { 
    incolsless1 = incols-1; 
    match_row_start = 0;
    match_row_num = 0;
    for (i=0;i<outrows;i++) {
      match_row_start  += match_row_num;
      j = match_row_start; match_row_num = 0;
      while (((j * outrows)/inrows) == i) {match_row_num++;j++;}
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in + ((((long) match_row_start)*incols) + match_col_start[j]);
        tot = match_row_num/2;
        for (k=0;k < match_row_num; k++, inptr += incolsless1) {
	  sm_colval = 50; 
	  sm_colval += (((long) (*inptr++))*smooth_col_perc[j]);
	  sm_colval += (((long) (*inptr))*(100-smooth_col_perc[j]));
	  sm_colval /= 100; 
	  tot += sm_colval; 
        }
        tot /= match_row_num;
        *outptr = (Pixel2) tot;
      }
    }
  } 
  IM_FREE_PTR(match_col_start);
  IM_FREE_PTR(match_col_num);
  return(1);
}

static int SmoothResize4(Pixel4 *in, Pixel4 *out, int inrows,
   int incols, int outrows, int outcols)
{
  int smoothRows = (outrows > inrows); 
  int smoothCols = (outcols > incols); 
  /* at least one of above two is guaranteed to be true */ 
  /* also, incols,inrows > 1 */ 
  Pixel4 *inptr, *outptr;
  int i, j, k, l;
  int incolsless1; 
  unsigned long tot, sm_colval; 
  int match_row_start, match_row_num;
  int smooth_row_perc; 
  int *match_col_start, *match_col_num, col, newcol, start, ct;
  int *smooth_col_perc = (int *) 0; 
  float temp; 

  
  match_col_start = malloc(sizeof(int)*outcols);
  match_col_num = malloc(sizeof(int)*outcols);
  if (smoothCols)
    smooth_col_perc = match_col_num; 

  
  if (!match_col_start || !match_col_num) { 
    if (match_col_start) IM_FREE_PTR(match_col_start);
    if (match_col_num) IM_FREE_PTR(match_col_num);
    return(0);
  }

  if (smoothCols) {
    for (j=0;j<outcols;j++) {
      match_col_start[j] = (j * incols)/ outcols;
      temp = ((float) j)*((float) incols)/((float) outcols); 
      temp -= ((float) match_col_start[j]); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_col_perc[j] = ((int) temp); 
      if (smooth_col_perc[j] < 0) smooth_col_perc[j] = 0; 
      if (smooth_col_perc[j] > 100) smooth_col_perc[j] = 100; 
      if (match_col_start[j] == (incols - 1)) {
	/* use a little trick */
	/* this is why we need image dims >= 2x2 */ 
	match_col_start[j]--;
	smooth_col_perc[j] = 0; 
      } 
    }
  } else {
    col = 0; ct = 0, start = 0;
    for (j=0;j<incols;j++) {
      newcol = (j * outcols)/ incols;
      if (newcol == col) ct++;
      else {
	match_col_start[col] = start;
	match_col_num[col] = ct;
	ct = 1;
	start = j;
	col = newcol;
      }
    }
    match_col_start[col] = start;
    match_col_num[col] = ct;
  }

  
  outptr = out;

  if (smoothRows && smoothCols) { 
    incolsless1 = incols - 1; 
    for (i=0;i<outrows;i++) {
      match_row_start = (i * inrows) / outrows;
      temp = ((float) i)*((float) inrows)/((float) outrows); 
      temp -= ((float) match_row_start); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_row_perc = ((int) temp); 
      if (smooth_row_perc < 0) smooth_row_perc = 0; 
      if (smooth_row_perc > 100) smooth_row_perc = 100; 
      if (match_row_start == (inrows-1)) {
	/* use the trick again */
	match_row_start--;
	smooth_row_perc = 0; 
      }   
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in +
	    ((((long) match_row_start)*incols) + match_col_start[j]);
        tot = 50; 
        sm_colval = 50; 
        sm_colval += ((unsigned long) (*inptr++))*smooth_col_perc[j];
        sm_colval += ((unsigned long) (*inptr))*(100-smooth_col_perc[j]);
        sm_colval /= 100; 
        tot += (sm_colval*smooth_row_perc); 
        inptr += incolsless1; 
        sm_colval = 50; 
        sm_colval += ((unsigned long) (*inptr++))*smooth_col_perc[j];
        sm_colval += ((unsigned long) (*inptr))*(100-smooth_col_perc[j]);
        sm_colval /= 100; 
        tot += (sm_colval*(100-smooth_row_perc)); 
        tot /= 100; 
        *outptr = (Pixel4) tot;
      }
    }
  } else if (smoothRows && !smoothCols) { 
    for (i=0;i<outrows;i++) {
      match_row_start = (i * inrows) / outrows;
      temp = ((float) i)*((float) inrows)/((float) outrows); 
      temp -= ((float) match_row_start); 
      temp = (((float) 1.0) - temp)*((float) 100.0) + ((float) 0.5);
      smooth_row_perc = ((int) temp); 
      if (smooth_row_perc < 0) smooth_row_perc = 0; 
      if (smooth_row_perc > 100) smooth_row_perc = 100; 
      if (match_row_start == (inrows-1)) {
	/* use the trick again */
	match_row_start--;
	smooth_row_perc = 0; 
      }   
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in
	    + ((((long) match_row_start)*incols) + match_col_start[j]);
	tot = 50; 
	sm_colval = match_col_num[j]/2; 
  	for (l=0;l < match_col_num[j];l++,inptr++) {
	  sm_colval += (unsigned long) (*inptr);
	} 
	sm_colval /= match_col_num[j]; 
	tot += (sm_colval*smooth_row_perc); 
	inptr += (incols-match_col_num[j]); 
	sm_colval = match_col_num[j]/2; 
  	for (l=0;l < match_col_num[j];l++,inptr++) {
	  sm_colval += (unsigned long) (*inptr);
	} 
	sm_colval /= match_col_num[j]; 
	tot += (sm_colval*(100-smooth_row_perc)); 
	tot /= 100; 
        *outptr = (Pixel4) tot;
      }
    }
  } else /* if (!smoothRows && smoothCols) */  { 
    incolsless1 = incols - 1; 
    match_row_start = 0;
    match_row_num = 0;
    for (i=0;i<outrows;i++) {
      match_row_start  += match_row_num;
      j = match_row_start; match_row_num = 0;
      while (((j * outrows)/inrows) == i) {match_row_num++;j++;}
      for (j=0;j<outcols;j++, outptr++) {
        inptr = in + ((((long) match_row_start)*incols) + match_col_start[j]);
        tot = match_row_num/2;
        for (k=0;k < match_row_num; k++, inptr += incolsless1) {
	  sm_colval = 50; 
	  sm_colval += (((unsigned long) (*inptr++))*smooth_col_perc[j]);
	  sm_colval += (((unsigned long) (*inptr))*(100-smooth_col_perc[j]));
	  sm_colval /= 100; 
	  tot += sm_colval; 
        }
        tot /= match_row_num;
        *outptr = (Pixel4) tot;
      }
    }
  } 
  IM_FREE_PTR(match_col_start);
  IM_FREE_PTR(match_col_num);
  return(1);
}


static int Resize(Pixel *in, Pixel *out, int inrows, int incols,
   int outrows, int outcols) 
{
  Pixel *inptr, *outptr;
  int i, j, k, l;
  long tot, match_num, incolsless1;
  int match_row_start, match_row_num;
  int *match_col_start, *match_col_num, col, newcol, start, ct;

  

  match_col_start = malloc(sizeof(int)*outcols);
  match_col_num = malloc(sizeof(int)*outcols);
  
  if (!match_col_start || !match_col_num) {
    if (match_col_start) IM_FREE_PTR(match_col_start);
    if (match_col_num) IM_FREE_PTR(match_col_num);
    return(0);
  }

  if (outcols >= incols) {
    /* am expanding: only one incol for each outcol */
    for (j=0;j<outcols;j++) {
      match_col_start[j] = (j * incols)/ outcols;
      match_col_num[j] = 1;
    }
  } else {
    col = 0; ct = 0, start = 0;
    for (j=0;j<incols;j++) {
      newcol = (j * outcols)/ incols;
      if (newcol == col) ct++;
      else {
	match_col_start[col] = start;
	match_col_num[col] = ct;
	ct = 1;
	start = j;
	col = newcol;
      }
    }
    match_col_start[col] = start;
    match_col_num[col] = ct;
  }


  match_row_start = 0;
  match_row_num = 0;
  
  outptr = out;

  for (i=0;i<outrows;i++) {
    if (outrows >= inrows) {
      /* am expanding: only one inrow for each outrow */
      match_row_start = (i * inrows) / outrows;
      match_row_num = 1;
    } else {
      match_row_start  += match_row_num;
      j = match_row_start; match_row_num = 0;
      while (((j * outrows)/inrows) == i) {match_row_num++;j++;}
    }
    for (j=0;j<outcols;j++, outptr++) {


      match_num = ((long) match_row_num) * match_col_num[j];
      inptr = in +
	((((long) match_row_start)*incols) + match_col_start[j]);

      tot = match_num/2;
      incolsless1 = incols - match_col_num[j];
      for (k=0;k < match_row_num; k++, inptr += incolsless1) {
	for (l=0;l < match_col_num[j];l++,inptr++) {
	  tot += (long) (*inptr);
	}  
      }  
      tot /= match_num;
      *outptr = (Pixel) tot;
    }
  }

  IM_FREE_PTR(match_col_start);
  IM_FREE_PTR(match_col_num);
  return(1);
}

static int Resize2(Pixel2 *in, Pixel2 *out, int inrows, int incols,
   int outrows, int outcols)
{
  Pixel2 *inptr, *outptr;
  int i, j, k, l;
  long tot, match_num, incolsless1;
  int match_row_start, match_row_num;
  int *match_col_start, *match_col_num, col, newcol, start, ct;

  
  match_col_start = malloc(sizeof(int)*outcols);
  match_col_num = malloc(sizeof(int)*outcols);
  
  if (!match_col_start || !match_col_num) {
    if (match_col_start) IM_FREE_PTR(match_col_start);
    if (match_col_num) IM_FREE_PTR(match_col_num);
    return(0);
  }

  if (outcols >= incols) {
    /* am expanding: only one incol for each outcol */
    for (j=0;j<outcols;j++) {
      match_col_start[j] = (j * incols)/ outcols;
      match_col_num[j] = 1;
    }
  } else {
    col = 0; ct = 0, start = 0;
    for (j=0;j<incols;j++) {
      newcol = (j * outcols)/ incols;
      if (newcol == col) ct++;
      else {
	match_col_start[col] = start;
	match_col_num[col] = ct;
	ct = 1;
	start = j;
	col = newcol;
      }
    }
    match_col_start[col] = start;
    match_col_num[col] = ct;
  }

  match_row_start = 0;
  match_row_num = 0;
  
  outptr = out;

  for (i=0;i<outrows;i++) {
    if (outrows >= inrows) {
      /* am expanding: only one inrow for each outrow */
      match_row_start = (i * inrows) / outrows;
      match_row_num = 1;
    } else {
      match_row_start  += match_row_num;
      j = match_row_start; match_row_num = 0;
      while (((j * outrows)/inrows) == i) {match_row_num++;j++;}
    }
    for (j=0;j<outcols;j++, outptr++) {
      match_num = ((long) match_row_num) * match_col_num[j];
      inptr = in + ((((long) match_row_start)*incols) + match_col_start[j]);
      tot = match_num/2;
      incolsless1 = incols - match_col_num[j];
      for (k=0;k < match_row_num; k++, inptr += incolsless1) {
	for (l=0;l < match_col_num[j];l++,inptr++) {
	  tot += (long) (*inptr);
	}
      }
      tot /= match_num;
      *outptr = (Pixel2) tot;
    }
  }

  IM_FREE_PTR(match_col_start);
  IM_FREE_PTR(match_col_num);
  return(1);
}

static int Resize4(Pixel4 *in, Pixel4 *out, int inrows, int incols,
   int outrows, int outcols)
{
  Pixel4 *inptr, *outptr;
  int i, j, k, l;
  unsigned long tot, match_num, incolsless1;
  int match_row_start, match_row_num;
  int *match_col_start, *match_col_num, col, newcol, start, ct;

  
  match_col_start = malloc(sizeof(int)*outcols);
  match_col_num = malloc(sizeof(int)*outcols);
  
  if (!match_col_start || !match_col_num) {
    if (match_col_start) IM_FREE_PTR(match_col_start);
    if (match_col_num) IM_FREE_PTR(match_col_num);
    return(0);
  }

  if (outcols >= incols) {
    /* am expanding: only one incol for each outcol */
    for (j=0;j<outcols;j++) {
      match_col_start[j] = (j * incols)/ outcols;
      match_col_num[j] = 1;
    }
  } else {
    col = 0; ct = 0, start = 0;
    for (j=0;j<incols;j++) {
      newcol = (j * outcols)/ incols;
      if (newcol == col) ct++;
      else {
	match_col_start[col] = start;
	match_col_num[col] = ct;
	ct = 1;
	start = j;
	col = newcol;
      }
    }
    match_col_start[col] = start;
    match_col_num[col] = ct;
  }

  match_row_start = 0;
  match_row_num = 0;
  
  outptr = out;

  for (i=0;i<outrows;i++) {
    if (outrows >= inrows) {
      /* am expanding: only one inrow for each outrow */
      match_row_start = (i * inrows) / outrows;
      match_row_num = 1;
    } else {
      match_row_start  += match_row_num;
      j = match_row_start; match_row_num = 0;
      while (((j * outrows)/inrows) == i) {match_row_num++;j++;}
    }
    for (j=0;j<outcols;j++, outptr++) {
      match_num = ((long) match_row_num) * match_col_num[j];
      inptr = in + ((((long) match_row_start)*incols) + match_col_start[j]);
      tot = match_num/2;
      incolsless1 = incols - match_col_num[j];
      for (k=0;k < match_row_num; k++, inptr += incolsless1) {
	for (l=0;l < match_col_num[j];l++,inptr++) {
	  tot += (unsigned long) (*inptr);
	}
      }
      tot /= match_num;
      *outptr = (Pixel4) tot;
    }
  }

  IM_FREE_PTR(match_col_start);
  IM_FREE_PTR(match_col_num);
  return(1);
}

extern int ImSetImgDim(Image *Im, int planenum, int rows, int cols,
		      int do_smooth, int maintain_aspect)
{
  long new_rows, new_cols;
  float col_aspect, row_aspect; 
  float orig_rows_for_aspect, orig_cols_for_aspect;
  int old_val;
  int pstart, pend, p, p1, change[IM_MAXPLANES], change_some;
  int ret = 1;
  Pixel *buff[IM_MAXPLANES];
  Pixel2 *buff2;
  Pixel4 *buff4;
  int smoothing_done[IM_MAXPLANES]; 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImSetImgDim: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImSetImgDim: image already dead!");

  new_rows = rows;
  new_cols = cols;

  if (maintain_aspect) {
    /* we always shrink to maintain aspect ratio */
    if (Im->WasCropped) {
      orig_rows_for_aspect = (float) (Im->CropEndRow - Im->CropStartRow + 1);
      orig_cols_for_aspect = (float) (Im->CropEndCol - Im->CropStartCol + 1);
    } else {
      orig_rows_for_aspect = (float) Im->NumRows;
      orig_cols_for_aspect = (float) Im->NumCols;
    }
    col_aspect = ((float) new_cols)/orig_cols_for_aspect;
    row_aspect = ((float) new_rows)/orig_rows_for_aspect;
    if (col_aspect < row_aspect) {
      /* reduce height */
      old_val = new_rows;
      new_rows = (int) ((float) col_aspect*orig_rows_for_aspect + 0.5);
      if (new_rows >= (old_val - 1)) new_rows = old_val;
      if (new_rows==0) new_rows = 1;
    } else if (col_aspect > row_aspect) {
      old_val = new_cols;
      new_cols = (int) ((float) row_aspect*orig_cols_for_aspect + 0.5);
      if (new_cols >= (old_val - 1)) new_cols = old_val;
      if (new_cols==0) new_cols = 1;
    }
  }

  /* see if no change is needed */
  if (planenum < 0) {
    pstart = 0;
    pend = Im->NumPlanes - 1;
  } else {
    if (planenum >= Im->NumPlanes)
      ImNonFatalError("ImSetImgDim: plane number too high");
    pstart = pend = planenum;
  }

  change_some = 0;
  for (p=pstart;p<=pend;p++) {
    if ((Im->CurrRows[p]!=new_rows) || (Im->CurrCols[p]!=new_cols)) {
      change_some = 1;
      change[p] = TRUE;
      buff[p] = (Pixel *)
	malloc(new_rows * new_cols *Im->SampleBytes[p]);
      if (!buff[p]) {
	for (p1=pstart; p1 < p; p1++) {
	  if (change[p1]) IM_FREE_PTR(buff[p1]);
	} 
	ImNonFatalError("ImSetImgDim: out of memory");
      } 
    } else {
      change[p] = FALSE;
      buff[p] = (Pixel *) Im->Im[p]; 
    }
  }

  if (!change_some) return(1);


  for (p=pstart;p<=pend;p++) {
    if (change[p]) {

      smoothing_done[p] = do_smooth && (Im->CurrRows[p] > 1) &&
                       (Im->CurrCols[p] > 1) &&
		       ((new_rows > Im->CurrRows[p]) || 
		        (new_cols > Im->CurrCols[p])); 


      switch(Im->SampleBytes[p]) {
      case 1:
	if (smoothing_done[p]) { 
	  ret = SmoothResize(Im->Im[p], buff[p],
	      Im->CurrRows[p], Im->CurrCols[p],
	      new_rows, new_cols); 
	} else {
	  ret = Resize(Im->Im[p], buff[p],
	      Im->CurrRows[p], Im->CurrCols[p],
	      new_rows, new_cols); 
	}
        if (!ret) {
	  for (p1=pstart; p1 <= pend; p1++) {
	    if (change[p1]) IM_FREE_PTR(buff[p1]);
	  } 
	  ImNonFatalError("ImSetImgDim: out of memory");
	}
        break;
      case 2:
	buff2 = (Pixel2 *) buff[p];
	if (smoothing_done[p]) { 
	  ret = SmoothResize2(Im->Im2[p], buff2,
	      Im->CurrRows[p], Im->CurrCols[p],
	      new_rows, new_cols); 
	} else {
	  ret = Resize2(Im->Im2[p], buff2,
	      Im->CurrRows[p], Im->CurrCols[p],
	      new_rows, new_cols); 
	}
        if (!ret) {
	  for (p1=pstart; p1 <= pend; p1++) {
	    if (change[p1]) IM_FREE_PTR(buff[p1]);
	  } 
	  ImNonFatalError("ImSetImgDim: out of memory");
	}
        break;
      case 4:
	buff4 = (Pixel4 *) buff[p];
	if (smoothing_done[p]) { 
	  ret = SmoothResize4(Im->Im4[p], buff4,
	      Im->CurrRows[p], Im->CurrCols[p],
	      new_rows, new_cols); 
	} else {
	  ret = Resize4(Im->Im4[p], buff4,
	      Im->CurrRows[p], Im->CurrCols[p],
	      new_rows, new_cols); 
	}
        if (!ret) {
	  for (p1=pstart; p1 <= pend; p1++) {
	    if (change[p1]) IM_FREE_PTR(buff[p1]);
	  } 
	  ImNonFatalError("ImSetImgDim: out of memory");
	}
        break;
      default:
	for (p1=pstart; p1 <= pend; p1++) {
	  if (change[p1]) IM_FREE_PTR(buff[p1]);
	} 
	ImNonFatalError("ImSetImgDim: bad # sample-bytes");
	break;
      }

    } 
  } 

  for (p=pstart;p<=pend;p++) {
    if (change[p]) {
      if (!Im->Changed[p]) {
	RetainImagePlane(Im, p);
      } 
      IM_FREE_PTR(Im->Im[p]);
      Im->Im[p] = (Pixel *) buff[p];
      Im->Im2[p] = (Pixel2 *) buff[p];
      Im->Im4[p] = (Pixel4 *) buff[p];
      
      Im->SmoothingDone[p] = smoothing_done[p]; 
      Im->Changed[p] = TRUE;
      Im->CurrRows[p] = new_rows;
      Im->CurrCols[p] = new_cols;

    }
  }

  Im->AllPlanesEqual = TRUE;
  for (p=0;p<Im->NumPlanes;p++) {
    if ((Im->CurrRows[p] != new_rows) ||
	(Im->CurrCols[p] != new_cols)) {
	  Im->AllPlanesEqual = FALSE;
	  break;
    }
  }
  return(1);
}

extern int ImStretchAndShrink(Image *Im, int planenum,
	   int vert_stretch, int vert_shrink,
           int horz_stretch, int horz_shrink,
           int do_smooth, int maintain_aspect)
{
  int rows, cols;
  int i;

  if ((Im->ImState < ImStateReadSome) || (Im->ImState > ImStateReadAll))
    ImNonFatalError("ImStretchAndShrink: image was not read, or is already dead");

  if (planenum < 0) {

    for (i=0;i<Im->NumPlanes; i++) {
      rows = Im->CurrRows[i] * vert_stretch / vert_shrink;
      cols = Im->CurrCols[i] * horz_stretch / horz_shrink;

      if (!ImSetImgDim(Im, i, rows, cols, do_smooth, maintain_aspect))
	return(0);
    }
    return(1);

  } else {
    if (planenum >= Im->NumPlanes)
      ImNonFatalError("ImStretchAndShrink: bad plane number");
    
    rows = Im->CurrRows[planenum] * vert_stretch / vert_shrink;
    cols = Im->CurrCols[planenum] * horz_stretch / horz_shrink;

    return(ImSetImgDim(Im, planenum, rows, cols, do_smooth, maintain_aspect));
  }

}

extern int ImBrighten(Image *Im, int planenum, int bright_add) 
{
  int pstart, pend, p; 
  long imsize, i, val; 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImBrighten: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImBrighten: image already dead!");


  if (planenum < 0) {
    pstart = 0;
    pend = Im->NumPlanes - 1;
  } else {
    if (planenum >= Im->NumPlanes)
      ImNonFatalError("ImBrighten: plane number too high");
    pstart = pend = planenum;
  }



  if (bright_add == 0) return(1); 

  for (p=pstart;p<=pend;p++) {

      if (!Im->Changed[p]) {
	RetainImagePlane(Im, p);
      }


      imsize = ((long) Im->CurrRows[p])*Im->CurrCols[p]; 


      switch(Im->SampleBytes[p]) {
      case 1:
	for (i=0;i<imsize;i++) {
	  val = ((long) Im->Im[p][i]) + bright_add; 
	  if (val < 0) val = 0;
	  if (val > Im->MaxSample[p]) val = Im->MaxSample[p]; 
	  Im->Im[p][i] = (Pixel) val; 
	} 
	break; 
      case 2:
	for (i=0;i<imsize;i++) {
	  val = ((long) Im->Im2[p][i]) + bright_add; 
	  if (val < 0) val = 0;
	  if (val > Im->MaxSample[p]) val = Im->MaxSample[p]; 
	  Im->Im2[p][i] = (Pixel2) val; 
	} 
	break; 
      case 4:
	for (i=0;i<imsize;i++) {
	  val = ((long) Im->Im4[p][i]) + bright_add; 
	  if (val < 0) val = 0;
	  if (val > Im->MaxSample[p]) val = Im->MaxSample[p]; 
	  Im->Im4[p][i] = (Pixel4) val; 
	} 
	break; 
      default:
	ImNonFatalError("ImBrighten: bad # sample-bytes");
	break;
      }

      Im->Brightness[p] += bright_add;
      Im->Changed[p] = TRUE; 
  } 

  return(1);
}

extern int ImContrast(Image *Im, int planenum, float contrast) 
{
  int pstart, pend, p; 
  long imsize, i; 
  float val; 

  if (Im->ImState < ImStateReadSome)
    ImNonFatalError("ImContrast: cannot be called before ImReadImg");
  else if (Im->ImState >= ImStateDead)
    ImNonFatalError("ImContrast: image already dead!");


  if (planenum < 0) {
    pstart = 0;
    pend = Im->NumPlanes - 1;
  } else {
    if (planenum >= Im->NumPlanes)
      ImNonFatalError("ImContrast: plane number too high");
    pstart = pend = planenum;
  }


  if (contrast == ((float) 1.0)) return(1); 

  for (p=pstart;p<=pend;p++) {

      if (!Im->Changed[p]) {
	RetainImagePlane(Im, p);
      }


      imsize = ((long) Im->CurrRows[p])*Im->CurrCols[p]; 


      switch(Im->SampleBytes[p]) {
      case 1:
	for (i=0;i<imsize;i++) {
	  val = ((float) Im->Im[p][i])*contrast + 0.5; 
	  if (val < 0) val = 0;
	  if (val > Im->MaxSample[p]) val = Im->MaxSample[p]; 
	  Im->Im[p][i] = (Pixel) val; 
	} 
	break; 
      case 2:
	for (i=0;i<imsize;i++) {
	  val = ((float) Im->Im2[p][i])*contrast + 0.5; 
	  if (val < 0) val = 0;
	  if (val > Im->MaxSample[p]) val = Im->MaxSample[p]; 
	  Im->Im2[p][i] = (Pixel2) val; 
	} 
	break; 
      case 4:
	for (i=0;i<imsize;i++) {
	  val = ((float) Im->Im4[p][i])*contrast + 0.5; 
	  if (val < 0) val = 0;
	  if (val > Im->MaxSample[p]) val = Im->MaxSample[p]; 
	  Im->Im4[p][i] = (Pixel4) val; 
	} 
	break; 
      default:
	ImNonFatalError("ImContrast: bad # sample-bytes");
	break;
      }

      Im->Contrast[p] *= contrast; 
      Im->Changed[p] = TRUE; 
  } 

  return(1);
}

