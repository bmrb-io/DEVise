#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <setjmp.h>
#include <X11/cursorfont.h>



#include "jpeglib.h"
#include "jerror.h"
#include "jsuspsrc.h"

#include "VR.h"


 /* extending error handling as per IJG:
 *
 * Here's the extended error handler struct:
 */

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF void
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}



struct jpeg_decompress_struct cinfo;
struct my_error_mgr jerr;

void CallAbort(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  jpeg_destroy_decompress(&cinfo);
  exit(1);
}

void Usage(void)
{
  fprintf(stderr,"Usage: xdjpeg [-display display] [-visual vclass] [-loc x y]\n\t[-title <title>] [-remap][-privremap] [-ack]\n\t[-showafter <percent>] [-qfromstdin] [-parwin <win_number>]\n\t[-win <win_number>] [-ccube n] [-maxc <m>] [-dither]\n\t[-tryXback] [jpegfile]\n");
  exit(1);
}

static unsigned long ccubeToflag(int c)
{
  if (c < 2) return(VR_CCC_2x2x2);
  else if (c == 2) return(VR_CCC_3x3x2);
  else if (c == 3) return(VR_CCC_3x3x3);
  else if (c == 4) return(VR_CCC_4x4x4);
  else if (c == 5) return(VR_CCC_5x5x5);
  else return(VR_CCC_6x5x5);
}


int
main(int argc, char *argv[])
{
  int width, height, nplanes, rwidth, tot;
  int i, scan_num, loc, nlines;
  VR_ImKind kind;
  VR_ImId iid;
  VR_WinId wid;
  VR_TransId tid, good_tid; 
  fd_set * foo;
  VR_ReturnCode ans;
  int lines_till_showing, sperc, cmap_exists;
  int parent_given;
  Window the_parent;
  int win_given;
  int ccube = 3;
  unsigned long ccc_flag = 0;
  Window the_win;
  int tryback;
  int maxc; 
  int dodither;
  Cursor cursor;
  int cursor_shape = XC_watch;
  int qfromstdin = 0;
  int vclass;

  long ptemp;

  JSAMPARRAY out_data_buffer;
  unsigned char * fullimage;

  unsigned long priv_flag;

  char *dname; /* display */
  int wherex, wherey;
  int remap, filegiven;
  int imfilefd;
  int ack_needed;
  
  char ImTitle[100]; 

  
  dname = (char *) 0;
  vclass = VR_VisDefault;
  wherex = 0;
  wherey = 0;
  remap = 0;
  filegiven = 0;
  sperc = 100;
  parent_given = 0;
  win_given = 0;
  ImTitle[0] = '\0'; 
  ack_needed = 0;
  tryback = 0; 
  dodither = 0;
  maxc = 256;

  /* parse switches */
  i = 1;
  while (i < argc) {
    if (!strncmp(argv[i],"-display", 4)) {
      i++;
      if (i >= argc) Usage();
      dname = argv[i];
    }
    else if (!strncmp(argv[i],"-title", 6)) {
      i++;
      if (i >= argc) Usage();
      strcpy(ImTitle, argv[i]); 
    }
    else if (!strncmp(argv[i],"-loc", 4)) {
      i++;
      if (i >= argc) Usage();
      wherex = atoi(argv[i]);
      i++;
      if (i >= argc) Usage();
      wherey = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-remap", 6)) {
      remap = 1;
      priv_flag = VR_CFL_FORCE_DEFAULT;
    }
    else if (!strncmp(argv[i],"-ack", 4)) {
      ack_needed = 1;
    }
    else if (!strncmp(argv[i],"-qfromstdin", 2)) {
      qfromstdin = 1;
    }
    else if (!strncmp(argv[i],"-tryXback", 5)) {
      tryback = 1;
    }
    else if (!strncmp(argv[i],"-dither", 4)) {
      dodither = 1;
    }
    else if (!strncmp(argv[i],"-privremap", 6)) {
      remap = 2;
      priv_flag = VR_CFL_PRIVATE;
    }
    else if (!strncmp(argv[i],"-ccube", 3)) {
      i++;
      if (i >= argc) Usage();
      ccube = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-maxc", 3)) {
      i++;
      if (i >= argc) Usage();
      maxc = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-visual", 4)) {
      i++;
      if (i >= argc) Usage();
      vclass = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-showafter", 7)) {
      i++;
      if (i >= argc) Usage();
      sperc = atoi(argv[i]);
    }
    else if (!strncmp(argv[i],"-win", 4)) {
      i++;
      if (i >= argc) Usage();
      ptemp = strtol(argv[i], (char **) 0, 16);
      the_win = (Window) ptemp;
      win_given = 1;
    }
    else if (!strncmp(argv[i],"-oldcmap", 5)) {
      /* no longer used */ 
    }
    else if (!strncmp(argv[i],"-parwin", 7)) {
      i++;
      if (i >= argc) Usage();
      ptemp = strtol(argv[i], (char **) 0, 16);
      the_parent = (Window) ptemp;
      parent_given = 1;
    }
    else {
      /* could be file name */
      if (i == (argc-1)) filegiven = 1;
      else Usage();
    }
    i++;
  }

  ans = VR_InitViewerVisual(dname, (FILE *) 0, vclass);
  if (ans != VR_SUCCESS) {
    fprintf(stderr,"InitViewer said: %s\n", VR_ReturnCodeToString(ans));
    exit(1);
  } 

  if (filegiven) {
    if (ImTitle[0] == '\0') strcpy(ImTitle, argv[argc-1]); 
    if ((imfilefd = open(argv[argc-1],O_RDONLY,0)) < 0) {
      fprintf(stderr,"Could not open %s\n", argv[argc-1]);
      exit(1);
    }
  }
  else {
    imfilefd = 0;
  }

  if (ImTitle[0] == '\0') strcpy(ImTitle, "Image"); 

  VR_RegisterFd(imfilefd);

  ccc_flag = ccubeToflag(ccube);
  VR_CFL_SET_NCOLORS(ccc_flag, maxc);

  fullimage = (unsigned char *) 0;
  out_data_buffer = (JSAMPARRAY) 0;


  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object
     */
    jpeg_destroy_decompress(&cinfo);
    VR_UnRegisterFd(imfilefd);
    if (imfilefd != 0) close(imfilefd); 
    if (fullimage) free(fullimage);
    if (out_data_buffer) free(out_data_buffer);
    if (qfromstdin) VR_RegisterFd(0);
    VR_WaitForSomething(&foo);
    VR_CloseViewer();
    return 0;
  }

  jpeg_create_decompress(&cinfo);

  jpeg_susp_src(&cinfo, imfilefd);

  jpeg_read_header(&cinfo, TRUE); /* won't suspend */

  if (cinfo.jpeg_color_space == JCS_UNKNOWN) {
    CallAbort("Unknown JPEG image");
  }
  else if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
    kind = VR_Gray;
    nplanes = 1;
    cinfo.out_color_space = JCS_GRAYSCALE;
  }
  else {
    kind = VR_RGB;
    nplanes = 3;
    cinfo.out_color_space = JCS_RGB;
  }

  if (ack_needed) {
	  fprintf(stdout,"OK\n");
	  fflush(stdout);
  } 

  cinfo.dct_method = JDCT_FLOAT;
  cinfo.buffered_image = TRUE;
  cinfo.do_block_smoothing = FALSE; 
  #ifdef HAVE_IJG_EXTENSIONS
  cinfo.do_fancy_pt_untrans = TRUE; 
  #endif

  jpeg_calc_output_dimensions(&cinfo); /* won't suspend */

  width = cinfo.output_width;
  rwidth = width*nplanes;
  height = cinfo.output_height; 
  tot = width * height;

  lines_till_showing = (height * sperc) / 100;
  if (lines_till_showing < 1) lines_till_showing = 1;
  if (lines_till_showing > height) lines_till_showing = height;
  cmap_exists = 0;

  if ((fullimage = (unsigned char *) calloc(1,sizeof(unsigned char)
				 *rwidth*height)) == NULL) {
    CallAbort("Out of memory");
  }
  if ((out_data_buffer = (JSAMPARRAY) calloc(1,sizeof(JSAMPROW)*
	 cinfo.rec_outbuf_height)) == NULL) {
    CallAbort("Out of memory");
  }
  

  jpeg_start_decompress(&cinfo); /* won't suspend */

  scan_num = 1; 
  while (! jpeg_input_complete(&cinfo) ) {

    jpeg_start_output(&cinfo, scan_num); /* won't suspend */

    /* display lines */

    while (cinfo.output_scanline < cinfo.output_height) {
      
      /** set buffer array **/
      out_data_buffer[0] = ((JSAMPROW) fullimage) +
	  (cinfo.output_scanline * rwidth); 
      for (i=1;i<cinfo.rec_outbuf_height;i++) {
        out_data_buffer[i] = out_data_buffer[i-1] + rwidth;  
      }

      loc = cinfo.output_scanline;
      nlines = jpeg_read_scanlines(&cinfo, out_data_buffer, cinfo.rec_outbuf_height); /* won't suspend */

      if (cmap_exists) {
	if (VR_ShowImageRect(iid, wid, 0, loc, width, nlines, dodither)
	  != VR_SUCCESS) CallAbort("Could not display");
      }
      else if (cinfo.output_scanline >= lines_till_showing) {
        iid = VR_GetNewImage(width, height, kind, ImTitle);
        VR_SetImageData(iid, fullimage);
        
	if (!win_given) {
	  if (parent_given) {
	    tid = VR_GetTranslatorFromCMofXWin(iid, ccc_flag, the_parent);
	  }
	  else {
            tid = VR_GetTranslator(iid, ccc_flag);
	  }

          wid = VR_GetWindow(width, height, wherex, wherey, tid, 
	     parent_given, the_parent, 0, 0, &tryback);
	} else {
          tid = VR_GetTranslatorFromCMofXWin(iid, ccc_flag, the_win);
	  wid = VR_SetWindow(width, height, tid, the_win, 0, 0, &tryback);
	}

	the_win = VR_Windows[wid].win;

	if ((wid < 0) || (tid < 0)) 
	  CallAbort("Could not create window/translator");

        if (VR_ShowImageRect(iid, wid, 0, 0, width, cinfo.output_scanline, dodither) != VR_SUCCESS)
	   CallAbort("Could not display");
	cmap_exists = 1;
      }
    } 

    /* suspension will usually occur here */
    ans = VR_SUCCESS;
    while (!jpeg_finish_output(&cinfo)) {
      ans = VR_WaitForSomething(&foo);
      if (ans != VR_SUCCESS) {
	/* probably fd is broken */
	break;
      }

    }

    if (ans != VR_SUCCESS) break; 

    scan_num++;

  }


  if (remap && jpeg_input_complete(&cinfo)) {
    cursor = XCreateFontCursor(VR_TheDisplay, cursor_shape);
    XDefineCursor(VR_TheDisplay, the_win, cursor);
    priv_flag |= VR_CCC_6x5x5;
    VR_CFL_SET_NCOLORS(priv_flag, maxc); 
    good_tid = VR_GetTranslatorFromCMofXWin(iid, priv_flag,
	the_win);
    if (good_tid >= 0) {
      VR_ChangeTranslator(wid, good_tid);
      if (VR_ShowImage(iid, wid, dodither) != VR_SUCCESS)
	   CallAbort("Could not display");
    }
    XDefineCursor(VR_TheDisplay, the_win, None);
    XFreeCursor(VR_TheDisplay, cursor);
    XFlush(VR_TheDisplay);
  } else {
    good_tid = tid; 
  }

  unset_suspension(&cinfo);
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  VR_UnRegisterFd(imfilefd);
  if (imfilefd != 0) close(imfilefd);


  free(fullimage);
  free(out_data_buffer);


  VR_FreeTransTable(good_tid);

  if (tryback) {
    VR_FreeWindowXImage(wid);
  } 

  if (qfromstdin) VR_RegisterFd(0);
  VR_WaitForSomething(&foo);
  VR_CloseViewer();

  return(0);
}



    
  
