#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <setjmp.h>



#include "jpeglib.h"
#include "jerror.h"

#include "Image.h"


 /* extending error handling as per IJG:
 *
 * Here's the extended error handler struct:
 */

struct jpgq_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct jpgq_error_mgr * jpgq_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF void
jpgq_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a jpgq_error_mgr struct, so coerce pointer */
  jpgq_error_ptr myerr = (jpgq_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}



struct jpeg_decompress_struct cinfo;
struct jpgq_error_mgr jerr;

void CallAbort(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  jpeg_destroy_decompress(&cinfo);
  exit(1);
}

void Usage(void)
{
  fprintf(stderr,"Usage: jpegqual jpegfile srcimage [-headpsnr]\n");
  exit(1);
}


static void JQDoSubSampling(unsigned char *orig,
  unsigned char *final, int inrows, int incols, int cnum)
{
  int  inrowskip, incolskip, outrows, outcols;
  int outi,outj,inj,inptrincr;
  unsigned char  *outptr, *inptr, *inrowptr;
  int boxsize, boxsizeby2;
  int val,i,j;

  outrows = inrows/2;
  outcols = incols/2;


  inrowskip = 2;
  incolskip = 2*3;
  boxsize = 4;
  boxsizeby2 = 2;
  inptrincr = inrowskip*incols*3;

  outptr = final;
  inptr = orig;
  for (outi=0;outi < outrows; outi++) {
    for (outj=0,inj=0; outj < outcols; outj++,inj+=incolskip) {
      val = boxsizeby2;
      inrowptr = inptr;
      for (i=0; i< inrowskip; i++) {
	for (j=0; j< incolskip; j+=3) {
	  val += (int) inrowptr[inj+j+cnum];
	}
	inrowptr += (incols*3);
      }
      val /= boxsize;
      *outptr++ = ((unsigned char) val);
    }
    inptr += inptrincr;
  }
}


int
main(int argc, char *argv[])
{
  int width, height, nplanes, tot, totby4, rwidth;
  int i, j, scan_num, loc, nlines;
  char *srcimage, *jpegimage; 
  int headpsnr = 0;

  JSAMPARRAY out_data_buffer;
  unsigned char * fullimage, * cplane;

  Image OrigImage;
  Image *Im = &OrigImage;
  FILE *jpegfp; 

  double peaksq = ((double) (255*255));

  double tot_err, diff, psnr, tot_err1, diff1, psnr1,
      tot_err2, diff2, psnr2, tot_err3, diff3, psnr3;

  
  if (argc < 3) Usage();
  if (argc > 3) headpsnr = 1;
  
  jpegimage = argv[1];
  srcimage = argv[2];


  if ((jpegfp = fopen(jpegimage,"r")) == NULL) {
      fprintf(stderr,"Could not open %s\n", jpegimage);
      exit(1);
  }



  fullimage = (unsigned char *) 0;
  out_data_buffer = (JSAMPARRAY) 0;


  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jpgq_error_exit;
  /* Establish the setjmp return context for jpgq_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object
     */
    jpeg_destroy_decompress(&cinfo);
    if (fullimage) free(fullimage);
    if (out_data_buffer) free(out_data_buffer);
    return 0;
  }
  
  ImInitImg(Im);
  if (!ImSetImgFile(Im, srcimage, 0)) {
    CallAbort("Could not open original image file");
  }
  ImSetSilent(Im);

  jpeg_create_decompress(&cinfo);

  jpeg_stdio_src(&cinfo, jpegfp);

  jpeg_read_header(&cinfo, TRUE); 

  if (cinfo.jpeg_color_space == JCS_UNKNOWN) {
    CallAbort("Unknown JPEG image color space");
  }
  else if (cinfo.jpeg_color_space == JCS_GRAYSCALE) {
    nplanes = 1;
    cinfo.out_color_space = JCS_GRAYSCALE;
  }
  else if (cinfo.jpeg_color_space == JCS_YCbCr) {
    nplanes = 3;
    cinfo.out_color_space = JCS_YCbCr;
  }
  else {
    CallAbort("Unknown JPEG image color space");
  }

  cinfo.dct_method = JDCT_FLOAT;
  cinfo.buffered_image = TRUE;
  cinfo.do_block_smoothing = TRUE; 
  #ifdef HAVE_IJG_EXTENSIONS
  cinfo.do_fancy_pt_untrans = TRUE; 
  #endif

  jpeg_calc_output_dimensions(&cinfo); 

  width = cinfo.output_width;
  rwidth = width*nplanes;
  height = cinfo.output_height; 
  tot = width * height;
  totby4 = (width/2) * (height/2);


  ImSetKindRaw(Im, nplanes, height, width, 0);

  if (!ImPeekImg(Im) || (ImKindOfImg(Im) == IM_UNKNOWN)) {
    fprintf(stderr,"Image read error/ format unknown\n");
    exit(1);
  }

  if ( (ImPlanesOfImg(Im) != nplanes) ||
       (ImOrigRows(Im) != height) ||
       (ImOrigCols(Im) != width) ||
       (ImSampleBytes(Im) != 1)) {
	 fprintf(stderr,"Image mismatch\n");
	 exit(1);
  }

  if (!ImReadImg(Im) || (ImStateOfImg(Im) != ImStateReadAll)) {
	 fprintf(stderr,"Error reading original image\n");
	 exit(1);
  }
  if (nplanes==3) {
    ImChangeCspace(Im, ImC_YCbCr);
    ImStretchAndShrink(Im, 1, 1, 2, 1, 2, 0, 0);
    ImStretchAndShrink(Im, 2, 1, 2, 1, 2, 0, 0);
  }

  if ((fullimage = (unsigned char *) malloc(rwidth*height)) == NULL) {
    CallAbort("Out of memory");
  }
  memset(fullimage, 128, rwidth*height);
  if ((out_data_buffer = (JSAMPARRAY) calloc(1,sizeof(JSAMPROW)*
	 cinfo.rec_outbuf_height)) == NULL) {
    CallAbort("Out of memory");
  }
  
  if ((cplane = (unsigned char *) calloc(1,sizeof(unsigned char)
				 *totby4)) == NULL) {
    CallAbort("Out of memory");
  }
  
  /** output initial PSNR **/
  if (headpsnr) { 
    /** compute PSNRs **/
    if (nplanes == 1) { 
      tot_err = (double) 0.0;
      for (i=0; i<tot; i++) {
	diff = ((double) (ImPlane(Im,0)[i])) - 
	       ((double) fullimage[i]);
        tot_err += (diff*diff);
      }
      psnr = ((double) 10.0)*log10(peaksq*((double) tot)/tot_err);
      printf("PSNR: %lf dB\n",psnr);
    } else {

     tot_err1 = (double) 0.0;
     for (i=0,j=0; i<tot; i++, j+=3) {

	diff1 = ((double) (ImPlane(Im,0)[i])) - 
	       ((double) fullimage[j]);
        tot_err1 += (diff1*diff1);
     }

     JQDoSubSampling(fullimage, cplane, height, width, 1);
     tot_err2 = (double) 0.0;
     for (i=0; i<totby4; i++) {

	diff2 = ((double) (ImPlane(Im,1)[i])) - 
	       ((double) cplane[i]);
        tot_err2 += (diff2*diff2); 
     }

     JQDoSubSampling(fullimage, cplane, height, width, 2);
     tot_err3 = (double) 0.0;
     for (i=0; i<totby4; i++) {

	diff3 = ((double) (ImPlane(Im,2)[i])) - 
	       ((double) cplane[i]);
        tot_err3 += (diff3*diff3); 
     } 

      psnr1 = ((double) 10.0)*log10(peaksq* ((double) tot)/tot_err1);
      psnr2 = ((double) 10.0)*log10(peaksq* ((double) totby4)/tot_err2);
      psnr3 = ((double) 10.0)*log10(peaksq* ((double) totby4)/tot_err3);

      tot_err = (tot_err1 + tot_err2 + tot_err3);
      psnr = ((double) 10.0)*log10(peaksq*((double) (tot + (2*totby4)))/tot_err);

       
      printf("YPSNR: %lf dB CbPSNR: %lf dB CrPSNR: %lf dB PSNR: %lf dB\n",
	    psnr1,psnr2,psnr3,psnr);
    }
  }

  jpeg_start_decompress(&cinfo); 

  scan_num = 1; 
  while (! jpeg_input_complete(&cinfo) ) {

    jpeg_start_output(&cinfo, scan_num); 

    /* display lines */

    while (cinfo.output_scanline < cinfo.output_height) {
      
      /** set buffer array **/
      out_data_buffer[0] = ((JSAMPROW) fullimage) +
	  (cinfo.output_scanline * rwidth); 
      for (i=1;i<cinfo.rec_outbuf_height;i++) {
        out_data_buffer[i] = out_data_buffer[i-1] + rwidth;  
      }

      loc = cinfo.output_scanline;
      nlines = jpeg_read_scanlines(&cinfo, out_data_buffer, cinfo.rec_outbuf_height); 

    } 
    
    /** compute PSNRs **/
    if (nplanes == 1) { 
      tot_err = (double) 0.0;
      for (i=0; i<tot; i++) {
	diff = ((double) ImPlane(Im,0)[i]) - 
	       ((double) fullimage[i]);
        tot_err += (diff*diff);
      }
      psnr = ((double) 10.0)*log10(peaksq*((double) tot)/tot_err);
      printf("PSNR: %lf dB\n",psnr);
   }
   else {

     tot_err1 = (double) 0.0;
     for (i=0,j=0; i<tot; i++, j+=3) {

	diff1 = ((double) ImPlane(Im,0)[i]) - 
	       ((double) fullimage[j]);
        tot_err1 += (diff1*diff1);
     }

     JQDoSubSampling(fullimage, cplane, height, width, 1);
     tot_err2 = (double) 0.0;
     for (i=0; i<totby4; i++) {

	diff2 = ((double) ImPlane(Im,1)[i]) - 
	       ((double) cplane[i]);
        tot_err2 += (diff2*diff2); 
     }

     JQDoSubSampling(fullimage, cplane, height, width, 2);
     tot_err3 = (double) 0.0;
     for (i=0; i<totby4; i++) {

	diff3 = ((double) ImPlane(Im,2)[i]) - 
	       ((double) cplane[i]);
        tot_err3 += (diff3*diff3); 
     } 

      psnr1 = ((double) 10.0)*log10(peaksq* ((double) tot)/tot_err1);
      psnr2 = ((double) 10.0)*log10(peaksq* ((double) totby4)/tot_err2);
      psnr3 = ((double) 10.0)*log10(peaksq* ((double) totby4)/tot_err3);

      tot_err = (tot_err1 + tot_err2 + tot_err3);
      psnr = ((double) 10.0)*log10(peaksq*((double) (tot + (2*totby4)))/tot_err);

       
      printf("YPSNR: %lf dB CbPSNR: %lf dB CrPSNR: %lf dB PSNR: %lf dB\n",
	    psnr1,psnr2,psnr3,psnr);
   }



    jpeg_finish_output(&cinfo);

    scan_num++;

  }


  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  fclose(jpegfp); 

  free(fullimage);
  free(out_data_buffer);




  ImFreeImg(Im);


  return(0);
}



    
  
