#ifndef JSUSP_INCLUDED
#define JSUSP_INCLUDED

#include "jpeglib.h"

extern void
jpeg_susp_src (j_decompress_ptr cinfo, int fd);

extern void
unset_suspension (j_decompress_ptr cinfo);

#endif

