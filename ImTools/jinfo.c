#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <setjmp.h>



#include "jpeglib.h"
#include "jerror.h"


static int ZigZagToN[64] =
{
0, 1, 8, 16, 9, 2, 3, 10, 
17, 24, 32, 25, 18, 11, 4, 5, 
12, 19, 26, 33, 40, 48, 41, 34, 
27, 20, 13, 6, 7, 14, 21, 28, 
35, 42, 49, 56, 57, 50, 43, 36, 
29, 22, 15, 23, 30, 37, 44, 51, 
58, 59, 52, 45, 38, 31, 39, 46, 
53, 60, 61, 54, 47, 55, 62, 63,
};


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
  fprintf(stderr,"Usage: jinfo jpegfile\n");
  exit(1);
}




int
main(int argc, char *argv[])
{
  int width, height, nplanes, tot, totby4, rwidth;
  int i, j, scan_num, loc, nlines;
  char *jpegimage; 
  int n, zn, Q[64], num;

  JSAMPARRAY out_data_buffer;
  unsigned char * fullimage;

  FILE *jpegfp; 


  
  if (argc != 2) Usage();
  
  jpegimage = argv[1];


  if ((jpegfp = fopen(jpegimage,"r")) == NULL) {
      fprintf(stderr,"Could not open %s\n", jpegimage);
      exit(1);
  }



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
    if (fullimage) free(fullimage);
    if (out_data_buffer) free(out_data_buffer);
    return 0;
  }
  

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
  cinfo.do_block_smoothing = FALSE;

  jpeg_calc_output_dimensions(&cinfo); 

  width = cinfo.output_width;
  rwidth = width*nplanes;
  height = cinfo.output_height; 
  tot = width * height;
  totby4 = (width/2) * (height/2);


  if ((fullimage = (unsigned char *) calloc(1,sizeof(unsigned char)
				 *rwidth*height)) == NULL) {
    CallAbort("Out of memory");
  }
  if ((out_data_buffer = (JSAMPARRAY) calloc(1,sizeof(JSAMPROW)*
	 cinfo.rec_outbuf_height)) == NULL) {
    CallAbort("Out of memory");
  }
  
  jpeg_start_decompress(&cinfo); 

  scan_num = 1; 
  while (! jpeg_input_complete(&cinfo) ) {

    jpeg_start_output(&cinfo, scan_num); 


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
    


    jpeg_finish_output(&cinfo);

    scan_num++;

  }


  jpeg_finish_decompress(&cinfo);

  for (i=0;i<nplanes;i++) {
    printf("Quantization table for plane %d\n",i);
    num = cinfo.comp_info[i].quant_tbl_no;
    if (!cinfo.quant_tbl_ptrs[num]) printf("Default table\n");
    else {
      for (zn=0;zn<64;zn++)
        Q[ZigZagToN[zn]] = (int)
	  cinfo.quant_tbl_ptrs[num]->quantval[zn];
      for (n=0;n<64;n++) {
	printf("%3d ",Q[n]);
	if ((n%8)==7) printf("\n");
      }
    }
  }

  jpeg_destroy_decompress(&cinfo);

  fclose(jpegfp); 

  free(fullimage);
  free(out_data_buffer);





  return(0);
}



    
  
