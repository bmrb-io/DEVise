#ifndef JCTHRESH_INCLUDED
#define JCTHRESH_INCLUDED

#include "jdct.h"

extern boolean jc_do_thresh;
extern float jc_thresh_table[NUM_QUANT_TBLS][DCTSIZE2];
extern int jc_num_thresh_tables;

extern void
read_thresh_tables (char *threshfile, J_DCT_METHOD dct_method);

extern void
apply_thresholding (DCTELEM *workspace, int comp_num);


#ifdef DCT_FLOAT_SUPPORTED

extern void
apply_float_thresholding (FAST_FLOAT *workspace, int comp_num);

#endif /* DCT_FLOAT_SUPPORTED */

#endif /* JCTHRESH_INCLUDED */
