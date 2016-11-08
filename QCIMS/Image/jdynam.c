#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "jinclude.h"
#include "jpeglib.h"
#include "jerror.h"

#include "jdynam.h" 



#ifndef JPEG_SOS
#define JPEG_SOS 0xDA
#endif

/* Expanded data source object for dynamic+suspending input source
 * application can read data off various file descriptors.
 * for each fd, either read-till-EOF or read-nbytes.
 */

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  DataSrc *ds; 
  JOCTET * buffer;		/* start of buffer */
  long extra_skip; /* remaining skip_bytes */
  long restart_bib; 
  JOCTET *last_nib;
  long last_bib; 
} dynamic_source_mgr;

typedef dynamic_source_mgr * dynamic_src_ptr;

#define INPUT_BUF_READ_STEP 4096
#define INPUT_BUF_SIZE  4200


/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF void
dynamic_init_source (j_decompress_ptr cinfo)
{
  dynamic_src_ptr src = (dynamic_src_ptr) cinfo->src;
  src->extra_skip = 0;
  src->restart_bib = 0;
  src->last_bib = -1; 
  src->last_nib = 0; 
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 */

METHODDEF boolean
dynamic_fill_input_buffer (j_decompress_ptr cinfo)
{
  dynamic_src_ptr src = (dynamic_src_ptr) cinfo->src;
  size_t nbytes, nbytes_leftover = (size_t) src->pub.bytes_in_buffer; 
  long rstep = INPUT_BUF_READ_STEP; 
  register JOCTET *inbuff = src->buffer; 


  if ((src->last_nib == src->pub.next_input_byte) &&
      (src->last_bib == src->pub.bytes_in_buffer)) {
	/* JPEG lib did not touch the data I had passed it */
	/* note that this cannot happen on the first call */
    if ((inbuff = (src->buffer + src->restart_bib))
	   != src->pub.next_input_byte) {
      while (nbytes_leftover > 0) { 
        *inbuff++ = *src->pub.next_input_byte++; 
        nbytes_leftover--; 
      } 
    }
    src->restart_bib += src->pub.bytes_in_buffer; 
  } else {
     /* JPEGlib has determined a new restart point */ 
     /* move leftover data to front */ 
     while (nbytes_leftover > 0) { 
       *inbuff++ = *src->pub.next_input_byte++; 
       nbytes_leftover--; 
     } 
     src->restart_bib = src->pub.bytes_in_buffer;
  } 

  src->pub.next_input_byte =
  src->last_nib =
  inbuff =
    src->buffer + src->restart_bib;
  src->last_bib =
  src->pub.bytes_in_buffer = 0;

  if ((rstep + src->restart_bib) > INPUT_BUF_SIZE) {
    rstep = INPUT_BUF_SIZE - src->restart_bib; 
  } 


  nbytes = DS_Read(src->ds, inbuff, rstep); 

  if (nbytes <= 0) {
    src->pub.next_input_byte =
    src->last_nib =
      src->buffer; 

    src->last_bib =
    src->pub.bytes_in_buffer =
      src->restart_bib; 

    src->restart_bib = 0;
    return(FALSE);
  }

  if (src->extra_skip > 0) {
    if (nbytes > src->extra_skip) {
      nbytes -= src->extra_skip;
      src->last_nib += src->extra_skip; 
      src->pub.next_input_byte = src->last_nib; 
      src->extra_skip = 0;
    } else {
      src->extra_skip -= nbytes;
      return(dynamic_fill_input_buffer(cinfo)); /* call again */
    }
  }
  src->last_bib = src->pub.bytes_in_buffer = nbytes;

  return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 */

METHODDEF void
dynamic_skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  dynamic_src_ptr src = (dynamic_src_ptr) cinfo->src;

  if (num_bytes > 0) {
    if (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      src->pub.bytes_in_buffer = 0;
      src->extra_skip = num_bytes;
    } else {
      src->pub.next_input_byte += (size_t) num_bytes;
      src->pub.bytes_in_buffer -= (size_t) num_bytes;
    }
  }
}




/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF void
dynamic_term_source (j_decompress_ptr cinfo)
{
  /* no work necessary here */
}


/*
 * Prepare for input from a suspending source
 * The caller must have already opened in_fd, and is responsible
 * for closing it after finishing decompression.
 */

extern void
jpeg_dynam_src (j_decompress_ptr cinfo, DataSrc *ds, 
  int headb, unsigned char *headbytes) 
{
  dynamic_src_ptr src;

  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(dynamic_source_mgr));
    src = (dynamic_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  INPUT_BUF_SIZE * SIZEOF(JOCTET));
  }

  src = (dynamic_src_ptr) cinfo->src;
  src->pub.init_source = dynamic_init_source;
  src->pub.fill_input_buffer = dynamic_fill_input_buffer;
  src->pub.skip_input_data = dynamic_skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = dynamic_term_source;
  src->ds = ds; 
  if (!headbytes) headb = 0;
  src->pub.bytes_in_buffer = headb; 
  src->pub.next_input_byte = src->buffer; 
  if (headb) memcpy(src->buffer, headbytes, headb); 

  src->extra_skip = 0;

}




/* Expanded data destination object for stdio output */

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  DataDest *dd; 
  JOCTET * buffer;		/* start of buffer */
} dynam_destination_mgr;

typedef dynam_destination_mgr * dynam_dest_ptr;

#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */


/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */

METHODDEF void
init_destination (j_compress_ptr cinfo)
{
  dynam_dest_ptr dest = (dynam_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 */

METHODDEF boolean
empty_output_buffer (j_compress_ptr cinfo)
{
  dynam_dest_ptr dest = (dynam_dest_ptr) cinfo->dest;

  if (DD_Write(dest->dd, dest->buffer, OUTPUT_BUF_SIZE) !=
         OUTPUT_BUF_SIZE)
    ERREXIT(cinfo, JERR_FILE_WRITE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF void
term_destination (j_compress_ptr cinfo)
{
  dynam_dest_ptr dest = (dynam_dest_ptr) cinfo->dest;
  long datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  /* Write any data remaining in the buffer */
  if (datacount > 0) {
    if (DD_Write(dest->dd, dest->buffer, datacount) != datacount)
      ERREXIT(cinfo, JERR_FILE_WRITE);
  }
}


GLOBAL void
jpeg_dynam_dest (j_compress_ptr cinfo, DataDest *dd) 
{
  dynam_dest_ptr dest;

  if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(dynam_destination_mgr));
  }

  dest = (dynam_dest_ptr) cinfo->dest;
  dest->pub.init_destination = init_destination;
  dest->pub.empty_output_buffer = empty_output_buffer;
  dest->pub.term_destination = term_destination;
  dest->dd = dd; 
}


