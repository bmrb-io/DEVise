
#define JPEG_INTERNALS
#include "cdjpeg.h"		/* Common decls for cjpeg/djpeg applications */
#include "jversion.h"		/* for version message */


#include "Image.h"


LOCAL void
transdecode_master_selection (j_decompress_ptr cinfo)
{
  /* Entropy decoding: either Huffman or arithmetic coding. */
  if (cinfo->arith_code) {
    ERREXIT(cinfo, JERR_ARITH_NOTIMPL);
  } else {
    if (cinfo->progressive_mode) {
#ifdef D_PROGRESSIVE_SUPPORTED
      jinit_phuff_decoder(cinfo);
#else
      ERREXIT(cinfo, JERR_NOT_COMPILED);
#endif
    } else
      jinit_huff_decoder(cinfo);
  }

  /* Always get a full-image coefficient buffer. */
  jinit_d_coef_controller(cinfo, TRUE);

  /* We can now tell the memory manager to allocate virtual arrays. */
  (*cinfo->mem->realize_virt_arrays) ((j_common_ptr) cinfo);

  /* Initialize input side of decompressor to consume first scan. */
  (*cinfo->inputctl->start_input_pass) (cinfo);

}





LOCAL void
usage (void)
/* complain about bad command line */
{
  fprintf(stderr, "usage: jpgtodc jpegfile [switches] [outfile]\n");

  fprintf(stderr, "Switches (names may be abbreviated):\n");
  fprintf(stderr,"  -reduce r [default 1]\n");
  exit(EXIT_FAILURE);
}



static int
jpeg_dump_dc_coefs_(char *dumpcoefsfile,
		    jvirt_barray_ptr *coef_array, 
		    struct jpeg_decompress_struct *cinfo,
		    int reduce, char *errmsg); 


/*
 * Read the coefficient arrays from a JPEG file.
 * jpeg_read_header must be completed before calling this.
 *
 * The entire image is read into a set of virtual coefficient-block arrays,
 * one per component.  The return value is a pointer to the array of
 * virtual-array descriptors.  These can be manipulated directly via the
 * JPEG memory manager, or handed off to jpeg_write_coefficients().
 * To release the memory occupied by the virtual arrays, call
 * jpeg_finish_decompress() when done with the data.
 *
 * Returns NULL if suspended.  This case need be checked only if
 * a suspending data source is used.
 */

 /* modified to read only until all the DC coefficients have been read */

LOCAL jvirt_barray_ptr *
jpeg_read_dc_coefficients (j_decompress_ptr cinfo)
{
  int all_dc_seen, i;

  all_dc_seen = 0;

  if (cinfo->global_state == DSTATE_READY) {
    /* First call: initialize active modules */
    transdecode_master_selection(cinfo);
    cinfo->global_state = DSTATE_RDCOEFS;
  } else if (cinfo->global_state != DSTATE_RDCOEFS)
    ERREXIT1(cinfo, JERR_BAD_STATE, cinfo->global_state);

  while (!all_dc_seen) { 
    /* Absorb whole file into the coef buffer */
    int retcode;
    /* Absorb some more input */
    retcode = (*cinfo->inputctl->consume_input) (cinfo);
    if (retcode == JPEG_SUSPENDED)
      return NULL;
    if (retcode == JPEG_REACHED_EOI)
      break;
    if (((retcode == JPEG_SCAN_COMPLETED) || (retcode == JPEG_REACHED_SOS))
	&& cinfo->coef_bits) {
      all_dc_seen = 1;
      for (i=0;i<cinfo->num_components;i++) {
        if (cinfo->coef_bits[i][0] == -1) {
	  all_dc_seen = 0;
        }
      }
    }
  } 

  /* Set state so that jpeg_finish_decompress does the right thing */
  cinfo->global_state = DSTATE_STOPPING;
  return cinfo->coef->coef_arrays;
}

/*
 * The main program.
 */


GLOBAL int
main (int argc, char **argv)
{
  struct jpeg_decompress_struct srcinfo;
  struct jpeg_error_mgr jsrcerr; 
  jvirt_barray_ptr * coef_arrays;
  FILE * input_file;
  char *outname;
  int i, reduce; 
  char errmsg[200]; 



  if (argc < 2) usage();
  outname = NULL;
  reduce = 1; 

  i=2;
  while (i < argc) {
    if (!strncmp(argv[i],"-reduce",3)) {
      i++;
      if (i >= argc) usage();
      reduce = atoi(argv[i]);
      if (reduce < 1) reduce = 1; 
    } else if (i==(argc-1)) {
      outname = argv[i];
    } else {
      usage();
    }
    i++;
  }



  /* Initialize the JPEG decompression object with default error handling. */
  srcinfo.err = jpeg_std_error(&jsrcerr);
  jpeg_create_decompress(&srcinfo);

  /* Now safe to enable signal catcher.
   * Note: we assume only the decompression object will have virtual arrays.
   */
#ifdef NEED_SIGNAL_CATCHER
  enable_signal_catcher((j_common_ptr) &srcinfo);
#endif



  /* Open the input file. */
  if (!strcmp(argv[1],"-")) input_file = read_stdin();
  else if ((input_file = fopen(argv[1], READ_BINARY)) == NULL) {
      fprintf(stderr, "jpgtodc: can't open %s\n", argv[1]);
      exit(EXIT_FAILURE);
  }



  /* Specify data source for decompression */
  jpeg_stdio_src(&srcinfo, input_file);
  

  /* Read file header */
  (void) jpeg_read_header(&srcinfo, TRUE);
  
  srcinfo.do_block_smoothing = FALSE;

  /* Read source file as DCT coefficients */
  
  coef_arrays = jpeg_read_dc_coefficients(&srcinfo);
  

  if (!jpeg_dump_dc_coefs_(outname,
      coef_arrays, &srcinfo,reduce,errmsg)) {
      fprintf(stderr,"Error: %s\n",errmsg);
  } 

  jpeg_destroy_decompress(&srcinfo);

  /* Close files, if we opened them */
  if (input_file != stdin)
    fclose(input_file);


  /* All done. */
  exit(jsrcerr.num_warnings ?EXIT_WARNING:EXIT_SUCCESS);
  return 0;			/* suppress no-return-value warnings */
}




static int 
jpeg_dump_dc_coefs_(char *dumpcoefsfile,
		    jvirt_barray_ptr *coef_array, 
		    struct jpeg_decompress_struct *cinfo,
		    int reduce, char *errmsg) 
{
  int n, bi, bj,c, sz, i, ow, oh, ows, ohs;
  double q; 

  JBLOCKARRAY buffer;
  jpeg_component_info *compptr;
  Image OutIm;

  
  InitImage(&OutIm); 
  OutIm.Silent = 1; 
  OutIm.NumRows = cinfo->comp_info[0].height_in_blocks; 
  OutIm.NumCols = cinfo->comp_info[0].width_in_blocks; 
  OutIm.NumComponents = cinfo->num_components; 
  OutIm.ImKind = IM_RAW; 
  OutIm.SaveImKind = IM_PNM;

  if ((cinfo->max_h_samp_factor > 2) || (cinfo->max_v_samp_factor > 2)){
    strcpy(errmsg,"Max JPEG samp fact too high");
    return(0);
  } 

  if (cinfo->jpeg_color_space == JCS_GRAYSCALE) {
  } else if (cinfo->jpeg_color_space == JCS_RGB) {
  } else if (cinfo->jpeg_color_space == JCS_YCbCr) {
    OutIm.SaveColorConv = YCCtoRGB; 
  } else {
    strcpy(errmsg,"Unknown JPEG color space");
    return(0);
  } 


  for (n=0;n<OutIm.NumComponents;n++) { 
    compptr = &cinfo->comp_info[n]; 
    OutIm.InSamplingFactor[n][0] = cinfo->max_v_samp_factor/
				   compptr->v_samp_factor;
    if ((ohs = OutIm.NumRows/OutIm.InSamplingFactor[n][0]) <
	compptr->height_in_blocks) {
	  oh = ohs; 
    } else {
	  oh = compptr->height_in_blocks;
    }
    OutIm.InSamplingFactor[n][1] = cinfo->max_h_samp_factor/
				   compptr->h_samp_factor;
    if ((ows = OutIm.NumCols/OutIm.InSamplingFactor[n][1]) <
	compptr->width_in_blocks) {
	  ow = ows; 
    } else {
	  ow = compptr->width_in_blocks;
    }

    OutIm.OutSamplingFactor[n][0] = reduce; 
    OutIm.OutSamplingFactor[n][1] = reduce; 

    sz =  ows*ohs;
    if ((OutIm.Im[n] = (Pixel *) calloc(1, sz * sizeof(Pixel)))==NULL) {
      strcpy(errmsg,"Out of memory");
      for (i=0;i<n;i++) free(OutIm.Im[i]);
      return(0);
    } 
    OutIm.ImExists[n] = 1; 
    if (!cinfo->quant_tbl_ptrs[compptr->quant_tbl_no])
      q = (n==0) ? ((double) 16.0) : ((double) 17); 
    else 
      q = (double) cinfo->quant_tbl_ptrs[compptr->quant_tbl_no]->quantval[0];

    for (bi=0,i=0;bi < oh; bi++,i+=ows) {
        buffer = (*cinfo->mem->access_virt_barray)
          ((j_common_ptr) cinfo, coef_array[compptr->component_index],
           bi, 1, TRUE);
	for (bj=0;bj < ow; bj++) {
	  c = (int) ((double) ((double) buffer[0][bj][0]*q)/8.0 + 128.5);
	  if (c < 0) c = 0;
	  if (c > 255) c = 255;
	  OutIm.Im[n][i+bj] = (Pixel) c; 
	}
     }
     
     DoSubSampling(&OutIm,n); 
     OutIm.OutSamplingFactor[n][0] = 1; 
     OutIm.OutSamplingFactor[n][1] = 1; 
  } 
  
  OutIm.NumRows /= reduce;
  OutIm.NumCols /= reduce;
  SaveImg(&OutIm, dumpcoefsfile); 

  FreeImg(&OutIm); 

  return(1); 

}




