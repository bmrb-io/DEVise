#ifndef JSUSP_INCLUDED
#define JSUSP_INCLUDED

#include "jpeglib.h"

#include "DataSrcDest.h"

extern void
jpeg_dynam_src (j_decompress_ptr cinfo, DataSrc *ds,
  int headb, unsigned char *headbytes); 

extern void
jpeg_dynam_dest (j_compress_ptr cinfo, DataDest *dd); 

#endif

