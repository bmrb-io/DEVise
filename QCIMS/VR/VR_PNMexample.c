
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

/*********************************************************************
  VR_pnm
  This is a simple example program to illustrate image display on X
  using the VR library. 

  Usage:
  VR_pnm [image_file]

  If no image_file is given, image will be read from standard input.
  VR_pnm understands only the PNM format (both grayscale--PGM and
  color--PPM, only in the raw-bytes mode).

  The image will be displayed in a window. The following keys can
  be pressed in that window with the associated results:

  q	Quit the application
  b	Brighten the image
  d	Darken the image
  e	Enhance contrast
  r	Reduce contrast
  u	Undo all changes 
  t	Use a new translator, generated quickly 
  g	Use a new translator of high quality, take a little more time
***********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h> 


#include "VR.h"


void CallAbort(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  exit(1);
}

void Usage(void)
{
  fprintf(stderr,"Usage: VR_pnm [image_file]\n"); 
  exit(1);
}

/*********** global variables **********/ 

/* image dimensions */ 
int width, height, num_planes, num_pixels, num_pixel_bytes;

VR_TransId tid;  /* VR_Translator */ 
VR_ImId iid;     /* VR_Image */ 
VR_WinId wid;    /* VR_Window */ 

unsigned char * orig_image, * curr_image;
    /* we use two copies of the image data. one copy is the
       "current" image, which might have undergone brightness
       and contrast changes, and the other copy is the original
       image, used to implement the "undo all changes function" */

/****************************************************************
   procedure to read a PNM format image into orig_image, copy
   it into curr_image. sets width, height, num_planes,
   num_pixels, num_pixel_bytes. 
*****************************************************************/

static void read_image(FILE *image_file)
{ 
  /* first, verify that it is in the PNM format. the first two
     bytes need to be P6 (for color) or P5 (for grayscale) */ 
  {
    int b1, b2;
    b1 = getc(image_file);
    b2 = getc(image_file);
    if (b1 != 'P') CallAbort("Not a PNM file"); 
    if (b2 == '5') num_planes = 1;
    else if (b2 == '6') num_planes = 3;
    else CallAbort("Not a raw PNM file"); 
  } 

  /* read width, height, max_pixel */
  {
    int max_pixel; 
    char buff[100];
    int params_read = 0;
    int i, c, nread; 
    while (params_read < 3) { 
      /* read a line */ 
      buff[0] = '\0';

      for (i=0;i<100;i++) {
	 c = getc(image_file);
	 if (c=='\n') {
	   buff[i] = '\0'; break;
	 } else if (c == EOF) CallAbort("Premature EOF"); 
	 else buff[i] = c; 
      } 

      if (buff[0]=='#') continue;

      nread = 0;
      if (params_read==0)
	nread = sscanf(buff,"%d%d%d",&width,&height,&max_pixel); 
      else if (params_read==1)
	nread = sscanf(buff,"%d%d", &height, &max_pixel); 
      else /* if (params_read==2) */ 
	nread = sscanf(buff,"%d", &max_pixel); 

      if (nread > 0) params_read += nread;

    } 
    if ((max_pixel < 0) || (max_pixel > 255)) 
      CallAbort("Invalid parameter in PNM file"); 
  } 

  num_pixels = width*height;
  num_pixel_bytes = num_pixels * num_planes;

  /* allocate memory */
  curr_image = (unsigned char *) malloc(num_pixel_bytes); 
  orig_image = (unsigned char *) malloc(num_pixel_bytes); 
  if (!curr_image || !orig_image) CallAbort("Could not malloc"); 
  
  /* read the image pixel data */
  if (fread(orig_image, 1, num_pixel_bytes, image_file) !=
	   num_pixel_bytes) CallAbort("Premature EOF"); 
  memcpy(curr_image, orig_image, num_pixel_bytes); 
} 

/********** Event Handlers *****************************************/

/*** increase brightness by 10 ***/
static int Brightener(VR_WinId wid, XEvent *evt)
{
  int i, val;
  unsigned char *data = curr_image;

  VR_ShowWaitCursor(wid);

  for (i=0;i<num_pixel_bytes;i++) {
    val = (int) (*data) + 10;
    if (val > 255) val = 255;
    *data++ = (unsigned char) val;
  }
  (void) VR_ShowImage(iid, wid, 1); 

  VR_ShowNormalCursor(wid);
  return(1); 
}

/*** decrease brightness by 10 ***/
static int Darkener(VR_WinId wid, XEvent *evt)
{
  int i, val;
  unsigned char *data = curr_image;

  VR_ShowWaitCursor(wid);

  for (i=0;i<num_pixel_bytes;i++) {
    val = (int) (*data) - 10;
    if (val < 0) val = 0;
    *data++ = (unsigned char) val;
  }
  (void) VR_ShowImage(iid, wid, 1); 

  VR_ShowNormalCursor(wid);
  return(1); 
}

/*** increase contrast by 1.05 ***/
static int Enhancer(VR_WinId wid, XEvent *evt)
{
  int i, val;
  unsigned char *data = curr_image;

  VR_ShowWaitCursor(wid);

  for (i=0;i<num_pixel_bytes;i++) {
    val = (int) (*data) * 105 / 100;
    if (val > 255) val = 255;
    *data++ = (unsigned char) val;
  }
  (void) VR_ShowImage(iid, wid, 1); 

  VR_ShowNormalCursor(wid);
  return(1); 
}

/*** decrease contrast by 1.05 ***/
static int Blurrer(VR_WinId wid, XEvent *evt)
{
  int i, val;
  unsigned char *data = curr_image;

  VR_ShowWaitCursor(wid);

  for (i=0;i<num_pixel_bytes;i++) {
    val = (int) (*data) * 95 / 100;
    *data++ = (unsigned char) val;
  }
  (void) VR_ShowImage(iid, wid, 1); 

  VR_ShowNormalCursor(wid);
  return(1); 
}

/*** restore original image ***/
static int Restorer(VR_WinId wid, XEvent *evt)
{

  VR_ShowWaitCursor(wid);

  memcpy(curr_image, orig_image, num_pixel_bytes);
  (void) VR_ShowImage(iid, wid, 1); 

  VR_ShowNormalCursor(wid);
  return(1); 
}

/*** install cheap new translator ***/
static int Remapper(VR_WinId wid, XEvent *evt)
{
  VR_TransId new_tid;
  VR_ReturnCode retval;

  VR_ShowWaitCursor(wid);

  new_tid = VR_GetTranslator(iid, 0);

  if (new_tid >= 0) {
    if ((retval = VR_ChangeTranslator(wid, new_tid)) == VR_SUCCESS) {
      (void) VR_ShowImage(iid, wid, 1); 
      tid = new_tid;
      fprintf(stderr,"Using %d (out of %d requested) colors.\n",
        VR_Translators[tid].TotalColorsUsed,
        VR_Translators[tid].TotalColorsRequested);
    } else {
      fprintf(stderr,"Failed to get translator: %s\n", 
	VR_ReturnCodeToString(retval));
    }
  } else {
    fprintf(stderr,"Failed to get translator: %s\n", VR_errmsg);
  }

  VR_ShowNormalCursor(wid);
  return(1); 
}


/*** install expensive new translator ***/
static int GoodRemapper(VR_WinId wid, XEvent *evt)
{
  VR_TransId new_tid;
  VR_ReturnCode retval;

  VR_ShowWaitCursor(wid);

  new_tid = VR_GetTranslator(iid, VR_CCC_6x5x5); 

  if (new_tid >= 0) {
    if ((retval = VR_ChangeTranslator(wid, new_tid)) == VR_SUCCESS) {
      (void) VR_ShowImage(iid, wid, 1); 
      tid = new_tid;
      fprintf(stderr,"Using %d (out of %d requested) colors.\n",
        VR_Translators[tid].TotalColorsUsed,
        VR_Translators[tid].TotalColorsRequested);
    } else {
      fprintf(stderr,"Failed to get translator: %s\n", 
	VR_ReturnCodeToString(retval));
    }
  } else {
    fprintf(stderr,"Failed to get translator: %s\n", VR_errmsg);
  }

  VR_ShowNormalCursor(wid);
  return(1); 
}



int
main(int argc, char *argv[])
{ 

  FILE * image_file;
  VR_ReturnCode VR_returned; 

  /* read the image */
  if (argc==1) read_image(stdin); 
  else {
    image_file = fopen(argv[1],"rb");
    if (!image_file) CallAbort("Could not open image file"); 
    read_image(image_file); 
    fclose(image_file); 
  } 

  fprintf(stderr,"%s: %d x %d x %d image..\n", 
    (argc==1) ? "<stdin>" : argv[1],
    width, height, num_planes);

  /* initialize viewer */
  VR_InitViewer(0,0); 

  /* get a VR_image */
  iid = VR_GetNewImage(width, height,
	       (num_planes == 1) ? VR_Gray : VR_RGB, 
	       (argc==1) ? "<stdin>" : argv[1]);
  if (iid < 0) CallAbort(VR_errmsg); 

  /* set image data */
  VR_returned = VR_SetImageData(iid, curr_image);
  if (VR_returned != VR_SUCCESS)
    CallAbort(VR_ReturnCodeToString(VR_returned)); 

  /* get a VR_translator */
  tid = VR_GetTranslator(iid, 0); 
  if (tid < 0) CallAbort(VR_errmsg); 

  /* get a VR_window */
  {
    int tryback = 1; 
    wid = VR_GetWindow(width, height,
		     0, 0,
		     tid,
		     0, 0,
		     0, /* norefresh */
		     0, /* noq */
		     &tryback); 
  }
  if (wid < 0) CallAbort(VR_errmsg);

  /* show the image in the window */
  VR_returned = VR_ShowImage(iid, wid, 1 /*dodither*/);

  fprintf(stderr,"Using %d (out of %d requested) colors.\n",
    VR_Translators[tid].TotalColorsUsed,
    VR_Translators[tid].TotalColorsRequested);

  if (VR_returned != VR_SUCCESS)
    CallAbort(VR_ReturnCodeToString(VR_returned));

  /* register event handlers for following key-presses 
  b	Brighten the image
  d	Darken the image
  e	Enhance contrast
  r	Reduce contrast
  u	Undo all changes 
  t	Use a new translator, generated quickly 
  g	Use a new translator of high quality, take a little more time
  */

  VR_RegisterEventHandler(wid, KeyPress, XK_b, Brightener);
  VR_RegisterEventHandler(wid, KeyPress, XK_d, Darkener);
  VR_RegisterEventHandler(wid, KeyPress, XK_e, Enhancer);
  VR_RegisterEventHandler(wid, KeyPress, XK_r, Blurrer);
  VR_RegisterEventHandler(wid, KeyPress, XK_u, Restorer);
  VR_RegisterEventHandler(wid, KeyPress, XK_t, Remapper);
  VR_RegisterEventHandler(wid, KeyPress, XK_g, GoodRemapper);

  {
    fd_set *dummy_set;
    VR_WaitForSomething(&dummy_set);
  }

  VR_CloseViewer();
  free(curr_image);
  free(orig_image);

  return(0);
} 

